# 多目标导航策略

> 不是 “按顺序走完所有点”，而是 “根据途中识别结果决定后面值不值得去”

## 第一性原理

如果比赛任务里存在 “先看结果，再决定后面去不去” 的规则，那么单纯固定航点巡航就不够了。

这时候就需要一种更高层的策略：

1. 先按预设路线走几个关键点
2. 在途中用视觉识别获取环境信息
3. 根据识别结果判断目标属于哪个区域
4. 再决定是否前往附加航点

这正是 `navigation_multi_goals.py` 在做的事。

源码位置：

- `src/robot_slam/scripts/navigation_multi_goals.py`

---

## 这个脚本做了什么

从结构上看，它主要做三件事：

### 1. 管理一串 `move_base` 目标点

它通过：

- `actionlib.SimpleActionClient("move_base", MoveBaseAction)`

直接发送 `MoveBaseGoal`。

位置：

- `src/robot_slam/scripts/navigation_multi_goals.py:39-40`
- `src/robot_slam/scripts/navigation_multi_goals.py:100-125`

这说明它不是发 `/move_base_simple/goal`，而是直接走 actionlib 接口。

### 2. 订阅视觉结果 `/object_position`

它订阅：

- `/object_position`

消息类型：

- `geometry_msgs/Point`

位置：

- `src/robot_slam/scripts/navigation_multi_goals.py:38`

而这个 `/object_position` 正是 `print_objects_detected_node.cpp` 从 `find_object_2d` 结果里提炼出来的。

所以这里形成了一条非常关键的跨层链路：

```text
find_object_2d → print_objects_detected → /object_position → navigation_multi_goals.py
```

### 3. 根据识别到的 ID 所属区间决定区域编号

核心逻辑在 `ar_cb()`：

- 若 `z` 在 `1~8` → 归为区域 `0`
- 若 `z` 在 `9~16` → 归为区域 `1`
- 若 `z` 在 `17~24` → 归为区域 `2`

位置：

- `src/robot_slam/scripts/navigation_multi_goals.py:42-66`

这里的 `point_msg.z` 被当成“识别到的目标 ID”。

也就是说：

> 这个脚本并不是关心目标在画面里的位置，而是关心“识别到的目标编号属于哪一块区域”。

---

## 为什么它订阅的是 `object_position` 而不是 `/ar_pose_marker`

因为它解决的问题不是“怎么精确对准标签”，而是：

- 这个识别结果属于哪一类区域？
- 接下来要不要绕去某个额外点位？

所以它用的是经过下游节点包装好的简单接口：

- `x`：目标中心 x
- `y`：目标中心 y
- `z`：目标 ID

而在这个脚本里，真正被用到的是：

- `z`

这是一种很典型的任务层思路：

> 我不需要完整视觉细节，我只需要一个足够简单的决策信号。

---

## 多航点流程怎么走

主逻辑在 `__main__` 下半部分。

大致流程是：

1. 从参数读取一组目标点坐标和朝向
2. 人工输入 `1` 后开始
3. 先去第一个点 `goals[0]`
4. 只有当早期识别把 `flog0 / flog1 / flog2` 中至少一个从 `255` 改成了有效区域编号，才会进入后续分支导航
5. 进入后续阶段后，把 `move_flog = 1`
6. 依次去 `goals[1]`、`goals[2]`
7. 再根据当前 `id` 是否等于前面锁定过的区域编号，决定是否去 `goals[3]`
8. 去 `goals[4]` 之后，再次用同样条件决定是否去 `goals[5]`

关键代码：

- `src/robot_slam/scripts/navigation_multi_goals.py:130-171`

所以这套逻辑不是简单的“6 点全走完”，而是：

> 先根据早期识别结果锁定一个区域，再决定后面额外点值不值得去。

---

## 这里的几个关键全局变量

脚本里用了很多全局变量：

- `id`
- `flog0`
- `flog1`
- `flog2`
- `move_flog`

位置：

- `src/robot_slam/scripts/navigation_multi_goals.py:27-32`

它们的含义可以大致这样理解：

| 变量 | 作用 |
|------|------|
| `id` | 当前最新识别出的区域编号（默认 `255` 表示未锁定） |
| `flog0/1/2` | 是否已经锁定过三个区域中的某个；初值 `255`，命中后分别改成 `0/1/2` |
| `move_flog` | 是否已经进入后续移动阶段 |

这套写法不是最优雅，但很典型地反映了竞赛脚本风格：

- 先追求能跑
- 用最直接的标志位把流程串起来

---

## actionlib 的价值是什么

这里之所以不用简单 topic，而用 `SimpleActionClient`，是因为 action 更适合 “去目标点并等待结果” 这种任务。

在这个脚本里：

- `goto()` 里构造 `MoveBaseGoal`
- 调 `send_goal()`
- 再 `wait_for_result()`
- 超时则 `cancel_goal()`

位置：

- `src/robot_slam/scripts/navigation_multi_goals.py:100-125`

这和 `/move_base_simple/goal` 相比的优势是：

- 可以知道是否成功
- 可以做超时控制
- 可以取消
- 更适合脚本式任务编排

所以它更像“程序员接口”，而 `/move_base_simple/goal` 更像“RViz 手点接口”。

---

## 语音播报在这里扮演什么角色

这个脚本还会发布：

- `/voiceWords`

位置：

- `src/robot_slam/scripts/navigation_multi_goals.py:37`
- 导航完成时：`src/robot_slam/scripts/navigation_multi_goals.py:88-92`

这意味着它在做多航点任务时，还顺手把“到点了”这个状态广播给语音系统。

所以这是一条很有意思的链：

```text
多航点脚本完成一步 → 发布 /voiceWords → 语音节点播报
```

这让操作者即便不一直盯屏幕，也能知道系统跑到哪一步了。

---

## 这份脚本暴露出的工程风格

它很典型地体现了教育竞赛机器人代码的特点：

### 优点

- 很直接
- 容易改点位
- 和比赛规则贴得近
- 出现临时规则变化时容易 patch

### 缺点

- 全局变量多
- 状态命名不清晰
- 可维护性一般
- 对识别结果的依赖耦合较强

也就是说，它更像：

> 一份 “能快速适应比赛策略” 的脚本，而不是一份高度工程化的任务框架。

---

## 调试这份脚本时最值得看的东西

1. `/object_position`
   - `z` 是否真的是你期望的目标 ID 或区域来源
2. `id / flog0 / flog1 / flog2`
   - 是否真的从默认值 `255` 切换到了期望区域
3. `move_base` 返回状态
   - 是否真的每步都成功到达
4. `/voiceWords`
   - 到点播报是否和实际状态一致

---

## 一句话总结

> `navigation_multi_goals.py` 的核心价值，不是“多走几个点”，而是把视觉识别结果接进路线决策，让导航从固定流程变成条件流程。

这就是它比普通巡航脚本更像“比赛策略节点”的地方。