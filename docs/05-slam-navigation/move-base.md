# move_base 框架

> 导航栈的总指挥：把定位、规划、避障、控制组织起来

## 第一性原理

单独有 AMCL、GlobalPlanner、DWA 都不够。还需要一个“总调度器”来负责：

- 接收目标点
- 请求全局路径
- 调用局部规划器生成控制命令
- 检测失败和振荡
- 触发恢复行为

这个总调度器就是 `move_base`。

你可以把它理解成：

> AMCL 是 “地图上的我在哪”
> GlobalPlanner 是 “路线设计师”
> DWA 是 “司机”
> move_base 是 “调度员兼裁判”

---

## 在 abot 中的配置

关键文件：

- `src/robot_slam/launch/include/move_base.launch.xml`
- `src/robot_slam/params/carto/move_base_params.yaml`

当前配置：

```yaml
base_local_planner: dwa_local_planner/DWAPlannerROS
base_global_planner: global_planner/GlobalPlanner
```

说明：

- 全局规划器：GlobalPlanner
- 局部规划器：DWAPlannerROS

这是一套非常标准的 ROS1 导航栈组合。

---

## move_base 的执行流程

### 1. 接收目标点

来源通常有两种：

- RViz 手点 2D Nav Goal
- 脚本发布 `/move_base_simple/goal`

在 abot 中，`mission_node`、`navigate.cpp`、`navigation_multi_goals.py` 都会发目标点。

### 2. 请求全局路径

`move_base` 调用 `GlobalPlanner`，在全局代价地图上生成一条从当前位置到目标点的路径。

### 3. 局部控制循环

局部规划器（DWA）以 `controller_frequency` 周期运行：

- 读取当前局部代价地图
- 读取全局路径
- 采样候选轨迹
- 选出最优轨迹
- 输出 `/cmd_vel`

### 4. 监测失败与恢复

如果：

- 一直规划不出全局路径
- 局部规划长期找不到可执行轨迹
- 机器人原地振荡

`move_base` 会认为导航失败，并尝试恢复。

---

## 关键参数详解

### `controller_frequency = 10.0`

- 含义：局部控制循环频率
- 当前值：10Hz

这表示 DWA 每 0.1 秒重新规划一次局部动作。

如果太低：
- 反应慢
- 避障不及时

如果太高：
- 计算压力更大

10Hz 对当前 abot 是合理值。

### `planner_frequency = 5.0`

- 含义：全局路径重规划频率
- 当前值：5Hz

也就是每 0.2 秒全局路径会刷新一次。

这在动态场景里很有帮助，因为局部避障后，全局路径也能重新贴合当前状态。

### `controller_patience = 15.0`

- 含义：局部规划器持续失败多久后，move_base 才认为“控制失败”
- 当前值：15 秒

这其实比较宽松。说明系统愿意给 DWA 很长时间去挣扎。

### `planner_patience = 5.0`

- 含义：全局规划失败多久后，认为“规划失败”
- 当前值：5 秒

### `oscillation_timeout = 10.0`

- 含义：如果机器人一直在原地附近来回折腾，10 秒后判定为振荡失败

### `oscillation_distance = 0.2`

- 含义：如果机器人在 20cm 内来回折腾，视为没有实质进展

---

## `patience` 参数怎么理解

这几个参数本质上是在定义：

> “我愿意给这个系统多长时间证明它还能继续工作？”

### `controller_patience` 太大

- 优点：偶发困难情况下系统不容易过早放弃
- 缺点：如果真的卡死，会浪费比赛时间

### `controller_patience` 太小

- 优点：更果断地进入恢复
- 缺点：可能正常的窄路通过也被误判失败

对竞赛来说，时间非常宝贵，所以过于宽松的 patience 也未必是好事。

---

## 振荡检测机制

`move_base` 会观察机器人是否在 “原地附近左右摆、前后试探，但就是不真正前进”。

如果在 `oscillation_distance = 0.2m` 范围内持续 `10s`，就会认为：

- 这不是正常导航
- 这是振荡

振荡常见原因：

1. DWA 参数不合理（尤其 min_vel_theta 太大）
2. costmap 过于紧张，左右都觉得危险
3. 终点容差太严格
4. 全向车却没开横向采样（`vy_samples = 0`）

---

## recovery behavior（恢复行为）

虽然当前配置文件里没有显式写出 recovery behaviors，但 move_base 默认支持恢复机制，例如：

- 清空局部代价地图
- 原地旋转重新观察环境
- 重新规划

但是要注意：

> move_base 的恢复能否真正发挥作用，很大程度还取决于 AMCL 和 costmap 是否允许系统重新建立正确状态。

如果 AMCL 自己都不能恢复（例如 `recovery_alpha_fast = 0`），那么 move_base 即使做恢复动作，也可能恢复不到正确定位。

---

## abot 中 move_base 的定位

在这个项目里，move_base 不是 “一个普通节点”，它实际上是所有竞赛行为的基础中间层：

- `mission_node` 不直接控制每一段路线，而是发目标点给 move_base
- `navigate.cpp` 不自己算轨迹，而是依赖 move_base
- `navigation_multi_goals.py` 通过 actionlib 调 move_base

所以：

> 如果 move_base 状态不稳定，上层所有比赛逻辑都会一起不稳定。

---

## 比赛视角下最该关注的 4 件事

1. **定位是否稳**
   - 否则 move_base 一切规划都建立在错误位置上

2. **全局路径是否合理**
   - 否则 DWA 只能在错误战略下挣扎

3. **局部轨迹是否可靠**
   - 否则会撞墙、抖动、过不去

4. **失败判定是否太慢**
   - 否则比赛中会卡很久才进入下一步

---

## 推荐理解方式

如果你在调比赛系统，建议把 move_base 当成一个“症状汇聚点”：

- 它出问题，不代表它本身有 bug
- 很多时候，是它下游（DWA）或上游（AMCL / costmap）出了问题

所以不要只盯着 `/move_base/status`，还要同时看：

- `/particlecloud`
- `/move_base/GlobalPlanner/plan`
- `/move_base/DWAPlannerROS/local_plan`
- `/cmd_vel`

这样你才能知道问题到底卡在哪一层。

---

## 源码参考

| 文件 | 路径 |
|------|------|
| move_base 参数 | `src/robot_slam/params/carto/move_base_params.yaml` |
| 启动配置 | `src/robot_slam/launch/include/move_base.launch.xml` |
| 顶层导航入口 | `src/robot_slam/launch/navigation.launch` |
