# 竞赛任务层总览

> 机器人不是 “会导航” 就够了，比赛真正比的是 “知道什么时候去哪里、看到什么后做什么”

## 第一性原理

导航层解决的是：

- 我怎么从 A 走到 B？

感知层解决的是：

- 我看到了什么？目标偏差多大？

而竞赛任务层解决的是更高一层的问题：

1. **现在该执行哪一步？**
2. **下一目标点该发给谁？**
3. **什么时候从导航切换到视觉对准？**
4. **什么时候应该射击？**
5. **完成一轮动作后，下一步是什么？**

所以任务层本质上是一个**状态机 / 流程控制器**。

你可以把整个系统理解成：

```text
感知层：告诉我现在看到了什么
导航层：帮我移动到指定地点
任务层：决定我下一步应该做什么
执行层：把命令真的打到电机和发射机构上
```

---

## abot 中这一层的 4 个关键角色

| 模块 | 作用 | 关键文件 |
|------|------|----------|
| `mission_node` | 比赛主状态机，管航点、AR 对准、射击 | `src/user_demo/include/user_demo/mission_node.hpp` |
| `shoot_control` | 把 `/shoot` 命令变成串口发射协议 | `src/shoot_cmd/src/shoot_control.cpp` |
| `voice_assistant` | 语音交互与播报 | `src/robot_voice/src/voice_assistant.cpp` |
| `navigation_multi_goals.py` | 多航点导航 + 目标区域判断 | `src/robot_slam/scripts/navigation_multi_goals.py` |

这几者共同组成 “比赛行为层”。

---

## 这一层最重要的认知

### 1. 任务层不是算法层，而是调度层

任务层自己通常不做：

- SLAM
- 路径规划
- 底层 PID

它主要做的是：

- 调用导航
- 等待结果
- 根据视觉结果切换状态
- 根据比赛规则决定下一步

所以当你看不懂任务代码时，最好的方法不是逐行死抠，而是先问：

> 当前节点在整个比赛流程里扮演什么角色？

### 2. 任务层最怕 “接口不统一”

例如这个仓库里就存在：

- `mission_node` 往 `/shoot` 发的是 `"1"`
- `cam_track` 往 `/shoot` 发的是 `"shoot!"`
- `shoot_control.cpp` 执行层只认 `"shoot"` / `"stopshoot"`

这类问题不会在架构图里显眼地写出来，但比赛里会直接让整条链路失效。

### 3. 任务层的 bug 往往不是“程序崩了”，而是“行为不对”

例如：

- 到了点却不切下一个状态
- 对准了却不发射
- 看到了目标却没走分支航点
- 语音播报和实际状态不同步

这也是为什么任务层文档必须把 “状态转换条件” 讲清楚，而不只是讲 API。

---

## 本章节文档导航

| 文档 | 用途 |
|------|------|
| [mission-node.md](mission-node.md) | 理解主比赛状态机与 A/B/C/E 航点逻辑 |
| [shoot-control.md](shoot-control.md) | 理解发射机构串口协议与命令兼容性问题 |
| [voice-interaction.md](voice-interaction.md) | 理解语音播报、语音输入与比赛触发方式 |
| [multi-goal-navigation.md](multi-goal-navigation.md) | 理解多航点脚本如何结合识别结果做条件导航 |

---

## 这一层和前面几层怎么接起来

### 输入侧

任务层通常依赖：

- `/move_base/result` 或 `/move_base/status`
- `/ar_pose_marker`
- `/object_position`
- `/voiceWords`
- `/snowman/ask`

### 输出侧

任务层通常发布：

- `/move_base_simple/goal`
- `move_base action goal`
- `/cmd_vel`
- `/shoot`
- `/voiceWords`

也就是说，任务层是一个非常典型的“中间编排层”：

- 上接感知与状态输入
- 下接导航、控制、执行和播报

---

## 比赛视角下最值得优先读懂的事

如果你时间不够，建议优先搞懂这三件事：

1. `mission_node` 的状态推进条件
2. `/shoot` 命令从谁发、谁收、谁真正执行
3. `navigation_multi_goals.py` 是怎么根据 `object_position` 改路线的

因为这三件事直接决定：

- 车会不会按比赛预期跑
- 到点后会不会真打
- 识别结果会不会影响策略

---

## 一句最重要的话

> 任务层不是 “多余的一层胶水代码”，它其实是把整个比赛系统拧成一台机器的地方。

如果导航、感知、驱动都像零件，那么任务层就是装配顺序和控制逻辑。

你去年比赛里很多“明明各模块都好像能工作，但整体不赢”的问题，往往就藏在这一层。