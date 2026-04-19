# mission_node 任务状态机详解

> 这不是一个普通节点，而是比赛流程的“总导演”

## 第一性原理

`mission_node` 的本质不是做导航，也不是做视觉识别。

它真正做的是：

1. 发布目标点
2. 等待导航成功
3. 切换到 AR 标签对准
4. 满足条件后发射
5. 再推进到下一个目标点

所以它本质上是一个**离散状态机**。

你可以把它想成：

```text
去 A 点 → 打 A → 去 B 点 → 打 B → 去 C 点 → 打 C → 去 E 点结束
```

只不过这条链路里，每一步都依赖别的模块：

- 导航是否成功
- AR 标签是否检测到
- 偏差是否足够小
- 射击命令是否真的生效

源码位置：

- `src/user_demo/include/user_demo/mission_node.hpp`
- 启动入口：`src/user_demo/launch/mission.launch`
- 参数文件：`src/user_demo/param/mission.yaml`

---

## 启动方式与配置来源

启动文件非常简单：

```xml
<node pkg="user_demo" type="mission_node" name="mission_node" output="screen">
  <rosparam command="load" file="$(find user_demo)/param/mission.yaml" />
</node>
```

位置：

- `src/user_demo/launch/mission.launch:1-4`

参数文件当前值：

```yaml
A_x: 1.0
A_y: -0.52
B_x: 0.55
B_y: -1.80
C_x: 0.68
C_y: -2.98
E_x: 0.18
E_y: -2.98
Yaw_th: 0.035
Ar_0_id: 5
Ar_1_id: 0
Ar_2_id: 5
Track_max_vel_x: 0.6
Track_thres_vel_x: 0.3
```

位置：

- `src/user_demo/param/mission.yaml:0-13`

这说明这套状态机并不是硬编码死在代码里，而是：

- 航点坐标可调
- 标签 ID 可调
- 对准阈值可调
- 视觉控制速度范围可调

也就是说，它是“比赛专用流程框架 + 一套现场可换参数”。

---

## 输入输出接口

### 订阅

`mission_node` 订阅：

- `/move_base/result`
- `/ar_pose_marker`
- `/abot/pose`

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:52-54`

含义分别是：

| 话题 | 用途 |
|------|------|
| `/move_base/result` | 判断导航是否成功到达目标点 |
| `/ar_pose_marker` | 获取 AR 标签位姿，供对准/射击使用 |
| `/abot/pose` | 获取机器人当前位姿与 yaw，用于恢复旋转 |

### 发布

`mission_node` 发布：

- `/cmd_vel`
- `/move_base_simple/goal`
- `/shoot`

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:56-60`

含义分别是：

| 话题 | 用途 |
|------|------|
| `/cmd_vel` | 对准标签时直接发控制命令 |
| `/move_base_simple/goal` | 让导航去下一个目标点 |
| `/shoot` | 发射命令 |

---

## 状态变量怎么看

代码里没有写 `enum State { ... }` 这种显式状态机，但它其实是由多组布尔标志拼起来的。

最关键的是：

- 发布状态：`pub_A`, `pub_B`, `pub_C`, `pub_E`
- 射击状态：`shoot_A`, `shoot_B`, `shoot_C`
- 到达状态：`reach_sign`
- 视觉状态：`marker_found`

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:79-87`

你可以把它们翻译成自然语言：

| 标志 | 代表什么 |
|------|----------|
| `pub_A` | A 点目标是否已经发给导航 |
| `shoot_A` | A 点的射击是否已完成 |
| `reach_sign` | 当前导航目标是否已到达 |
| `marker_found` | 当前目标标签是否在当前帧被检测到 |

这就是这套状态机的核心：

> 用“点是否发布过 + 当前点是否打完”来推进整个比赛流程。

---

## 完整流程图

把 `execCallback()` 展开后，大致等价于：

```text
初始
 └─ 若 A 未发布且 A 未完成射击 → 发布 A
      └─ 到达 A 后 → 对准并射击 A
           └─ 若 A 已射击且 B 未发布 → 发布 B
                └─ 到达 B 后 → 对准并射击 B
                     └─ 若 B 已射击且 C 未发布 → 发布 C
                          └─ 到达 C 后 → 对准并射击 C
                               └─ 若 C 已射击且 E 未发布 → 发布 E
```

对应代码主体：

- `src/user_demo/include/user_demo/mission_node.hpp:286-394`

这类结构特别适合比赛，因为它：

- 逻辑直观
- 不需要复杂状态框架
- 赛前通过改 YAML 就能换点位

但缺点也很明显：

- 状态多了会变乱
- 标志位之间容易互相影响
- 一旦某个标志没被正确置位，就可能卡死整条流程

---

## 航点 A / B / C / E 的意义

从代码上看：

- `A`：第一个导航点，到了之后打第一次
- `B`：第二个导航点，到了之后打第二次
- `C`：第三个导航点，到了之后打第三次
- `E`：最后的结束点

这几个点的坐标在 `mission.yaml` 里定义。

关键位置：

- `src/user_demo/include/user_demo/mission_node.hpp:33-40`

在 `execCallback()` 里，每个点都是通过发布 `geometry_msgs::PoseStamped` 到 `/move_base_simple/goal` 来触发导航。

相关代码：

- A 点：`src/user_demo/include/user_demo/mission_node.hpp:289-307`
- B 点：`src/user_demo/include/user_demo/mission_node.hpp:309-329`
- C 点：`src/user_demo/include/user_demo/mission_node.hpp:331-351`
- E 点：`src/user_demo/include/user_demo/mission_node.hpp:353-372`

---

## 射击阶段是怎么工作的

当 `reach_sign == true` 后，状态机会进入“打靶逻辑”：

- A 还没打完 → `shootToar(0)`
- B 还没打完 → `shootToar(1)`
- C 还没打完 → `shootToar(2)`

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:373-393`

### `shootToar(i)` 的逻辑

它的流程可以总结成：

1. 选择当前应打的标签 ID
2. 如果标签已找到：
   - 用 `offset_x` 计算角速度
   - 发布 `/cmd_vel`
3. 若偏差进入阈值范围：
   - 等待 `0.8~0.9s`
   - 连续向 `/shoot` 发两次字符串 `"1"`
   - 把该点的 `shoot_*` 标志置为完成
4. 如果标签没找到：
   - 做一个基于当前朝向的恢复旋转

相关代码：

- A 点：`src/user_demo/include/user_demo/mission_node.hpp:112-152`
- B 点：`src/user_demo/include/user_demo/mission_node.hpp:154-188`
- C 点：`src/user_demo/include/user_demo/mission_node.hpp:190-224`

> [!WARNING]
> `mission_node` 当前并不是向 `/shoot` 发送 `"shoot"`，而是发送字符串 `"1"`。如果执行层跑的是 `src/shoot_cmd/src/shoot_control.cpp`，那么这条命令默认不会被识别；`shoot_control.cpp` 只处理 `"shoot"` 和 `"stopshoot"`。

---

## AR 标签对准逻辑

### 当前目标标签 ID

参数：

- `Ar_0_id`
- `Ar_1_id`
- `Ar_2_id`

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:43-46`

这意味着：

- 三次射击并不一定打三个完全不同的标签
- 实际目标可以由 YAML 改

当前配置里：

```yaml
Ar_0_id: 5
Ar_1_id: 0
Ar_2_id: 5
```

也就是说，当前方案甚至允许第一枪和第三枪打同一个 ID。

### 横向偏差如何得到

`tag_cb()` 会遍历 `/ar_pose_marker` 中的所有标签：

- 若 `marker.id == current_target_id`
- 则 `marker_found = true`
- `offset_x = marker.pose.pose.position.x`
- `vel_x = -PID_Control_P * offset_x`

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:261-283`

这里要特别注意：

> 这不是像素坐标，而是标签位姿估计中的横向位移。

所以它更接近“相机/标记坐标系中的横向误差”，而不是简单的“图像中心偏差”。

---

## 一个很关键的实现问题：satfunc 没真正生效

代码里本来显然想做“限幅 + 死区”：

```cpp
vel_x = -PID_Control_P * offset_x;
satfunc(vel_x, track_max_vel_x, track_thres_vel_x);
```

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:277-279`

同时 `satfunc()` 的定义也确实写了：

- 小于阈值直接置 0
- 大于最大值则截断

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:397-411`

但是：

> [!WARNING]
> `satfunc()` 的返回值没有重新赋回 `vel_x`，所以这套控制整形在当前代码里并没有真正用上。

这意味着：

- 设计意图是合理的
- 当前实现却没完全接上

比赛里如果你发现这套视觉控制“没有想象中那种死区/限幅效果”，这就是一个很值得优先怀疑的点。

---

## 另一个关键实现问题：marker_found 没可靠清零

`tag_cb()` 中，当找到当前目标标签时会：

- `marker_found = true`

但当前代码没有在“本帧没找到当前目标标签”时显式把它重置回 `false`。

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:261-283`

这会带来什么后果？

- 从设计上看，代码是想在目标丢失时进入恢复旋转
- 但从实现上看，`marker_found` 可能保留旧状态
- 于是状态机行为可能不像你以为的那样清晰

> [!WARNING]
> 这类“状态位残留”问题，是比赛代码里非常典型、也非常难现场肉眼发现的问题。

---

## 射击时序

A/B/C 三个点的射击逻辑本质一致：

1. 偏差进入阈值
2. 等待约 `0.8~0.9s`
3. 发送一次射击命令
4. 等待 `0.5s`
5. 再发一次
6. 对应 `shoot_* = true`

对应位置：

- A 点：`src/user_demo/include/user_demo/mission_node.hpp:127-139`
- B 点：`src/user_demo/include/user_demo/mission_node.hpp:164-174`
- C 点：`src/user_demo/include/user_demo/mission_node.hpp:200-209`

也就是说，`mission_node` 默认策略是：

> 每个点打两发，而且在认为已经对准后还会故意等一小段时间再打。

这通常是在对抗：

- 车体还没完全停稳
- 视觉结果刚进入阈值，但姿态还在细微变化

---

## 一个比赛级故障：/shoot 命令格式不统一

`mission_node` 发的是：

```cpp
shoot_msg.data = string("1");
```

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:133`
- `src/user_demo/include/user_demo/mission_node.hpp:169`
- `src/user_demo/include/user_demo/mission_node.hpp:205`

但 `shoot_control.cpp` 认的是：

- `"shoot"`
- `"stopshoot"`

位置：

- `src/shoot_cmd/src/shoot_control.cpp:52-60`

这说明：

> [!WARNING]
> 按当前源码，`mission_node` 和 `shoot_control` 的字符串协议并不兼容。

这很可能导致：

- 状态机以为自己“已经发射”
- 但真正的执行层什么都没做

这正是比赛里最危险的一类 bug：

- 日志上看好像都正常
- 但赛场上就是不出球

---

## 这份代码最应该怎么读

如果你以后要自己改它，最推荐的阅读顺序是：

1. 先看 `mission.yaml`
   - 知道比赛点位和阈值是什么
2. 再看 `execCallback()`
   - 知道状态怎么推进
3. 再看 `shootToar()`
   - 知道到点后到底怎么打
4. 最后看 `tag_cb()` / `status_cb()` / `pos_cb()`
   - 知道状态变量从哪来

这样读，会比从头逐行硬啃高效很多。

---

## 一句话总结

> `mission_node` 是把“到点、看见、对准、发射、转场”串成比赛流程的核心节点。

它最大的价值不在算法，而在流程编排；它最大的风险也不在数学，而在状态位和接口协议。