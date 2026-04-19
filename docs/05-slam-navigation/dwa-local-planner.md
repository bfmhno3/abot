# DWA 局部路径规划与调参

> 这一秒电机该怎么转，基本都由它决定

## 第一性原理

DWA（Dynamic Window Approach，动态窗口法）不是 “沿着全局路径走” 这么简单。

它每个控制周期都在做三件事：

1. 在速度空间里采样很多组候选速度
2. 用机器人动力学把每组速度向前模拟一段时间
3. 给每条候选轨迹打分，选最好的一条输出成 `/cmd_vel`

可以把它想成：

> “机器人每 0.1 秒都在心里排练很多种开法，然后挑一种最值得赌的。”

---

## abot 当前 DWA 配置的核心问题

文件：

- `src/robot_slam/params/carto/dwa_local_planner_params.yaml`

当前配置里有两个极重要的问题：

> [!CAUTION]
>
> `vy_samples: 0` —— 这会直接阻止全向机器人进行横向运动规划。

> [!CAUTION]
>
> `occdist_scale: 0.03` —— 这让障碍物代价几乎没有话语权，机器人会更愿意贴着障碍冲过去。

这两点，基本就解释了为什么比赛里车会 “不会横移”, “容易贴墙”, “看起来像不会躲障碍”。

---

## 速度参数

当前配置：

```yaml
max_vel_x: 0.20
min_vel_x: -0.20
max_vel_y: 0.20
min_vel_y: -0.20
max_vel_trans: 0.20
min_vel_trans: 0.13
max_vel_theta: 1.4
min_vel_theta: 0.8
```

### 如何理解

| 参数 | 含义 |
|------|------|
| `max_vel_x` | 最大前进速度 |
| `min_vel_x` | 最大后退速度 |
| `max_vel_y` | 最大横向速度 |
| `min_vel_y` | 最大反向横移速度 |
| `max_vel_trans` | 最大平移速度总量 |
| `min_vel_trans` | 最小平移速度 |
| `max_vel_theta` | 最大转向角速度 |
| `min_vel_theta` | 最小转向角速度 |

### 关键观察

#### 1. 速度整体偏保守

`max_vel_x = 0.20 m/s` 对比赛来说比较慢，更像“先保证能跑”的设置。

#### 2. min_vel_trans = 0.13 偏高

这意味着机器人一旦要动，平移速度不能低于 0.13 m/s。

后果：

- 接近目标点时不容易慢慢蹭过去
- 更容易 overshoot（冲过头）
- 小范围细调动作不够柔和

> [!TIP]
>
> 如果你发现机器人快到目标点时总是来回修正，可以尝试把 `min_vel_trans` 降到 0.05~0.08。

#### 3. min_vel_theta = 0.8 很高

这意味着机器人一旦转向，角速度至少接近 46°/s。

后果：

- 小角度精修时动作会显得猛
- 容易在终点附近左右摆动

---

## 加速度限制

```yaml
acc_lim_x: 2.5
acc_lim_y: 2.5
acc_lim_theta: 3.0
```

这些参数决定候选轨迹是否“物理可行”。

如果加速度限制设得太小：
- 机器人反应会显得迟钝

如果设得太大：
- 规划器会选择一些机器人实际难以跟上的轨迹
- 实车表现会和仿真预期脱节

---

## 前向仿真参数

```yaml
sim_time: 2.0
vx_samples: 10
vy_samples: 0
vth_samples: 20
controller_frequency: 10.0
```

### `sim_time = 2.0`

DWA 会把每个候选速度向前模拟 2 秒。

- 大：轨迹更平滑、看得更远，但反应慢
- 小：反应更快，但轨迹更短视

2.0 是常见中间值。

### `vx_samples = 10` / `vth_samples = 20`

表示在 x 速度和角速度上分别采样多少个候选值。

采样越多：
- 轨迹选择更细致
- 计算更多

### `vy_samples = 0`（严重问题）

尽管下面写着：

```yaml
holonomic_robot: true
```

但 `vy_samples = 0` 意味着：

- 局部规划器根本不采样横向速度
- 所以即使底盘能横移，DWA 也**不会考虑这种走法**

这就好比你有一台能侧移的车，但驾驶员从来不允许自己打横方向盘。

> [!WARNING]
>
> 这是当前配置里最关键的功能性错误之一。

建议直接改为：

```yaml
vy_samples: 10
```

---

## 轨迹评分权重

```yaml
path_distance_bias: 32.0
goal_distance_bias: 24.0
occdist_scale: 0.03
```

这是 DWA 性格的核心。

### `path_distance_bias`

越大，机器人越想贴着全局路径走。

### `goal_distance_bias`

越大，机器人越想朝目标点直接逼近。

### `occdist_scale`

越大，机器人越怕障碍物。

### 当前配置的问题

32、24、0.03 放在一起，意味着：

- 跟路径、冲目标这两件事非常重要
- 避障几乎不重要

后果是：

> 只要路径还说得过去，机器人就会觉得“贴着障碍冲也行”。

这在比赛中非常危险，因为真实环境里定位误差、车体宽度、控制延迟都会放大风险。

建议至少改为：

```yaml
occdist_scale: 0.3 ~ 0.8
```

如果你更想保守一点，甚至可以先试 1.0。

---

## 终点容差

```yaml
xy_goal_tolerance: 0.15
yaw_goal_tolerance: 0.15
latch_xy_goal_tolerance: false
```

含义：

- 位置误差在 15cm 内算到达
- 朝向误差在 0.15rad（约 8.6°）内算到达
- 必须同时满足位置和朝向

如果你在比赛里不特别在意到点时的朝向，可以让容差更宽一点，减少终点振荡。

---

## 机器人行为与参数的直接映射

### 现象 1：不会横向躲障碍

直接原因：

- `vy_samples = 0`

### 现象 2：贴墙走、看起来不怕障碍

直接原因：

- `occdist_scale` 太低
- `inflation_radius` 太小

### 现象 3：快到终点时来回摆

常见原因：

- `min_vel_trans` 太大
- `min_vel_theta` 太大
- `xy_goal_tolerance` 太小

### 现象 4：动作太慢

常见原因：

- `max_vel_x` 太低
- `sim_time` 太长导致保守
- 加速度限制太低

---

## 推荐调参流程

### 第一步：先修功能性错误

```yaml
vy_samples: 10
occdist_scale: 0.5
```

### 第二步：先做安全配置

```yaml
max_vel_x: 0.20
max_vel_y: 0.20
min_vel_trans: 0.08
max_vel_theta: 1.0
min_vel_theta: 0.3
```

### 第三步：观察 3 类行为

1. 会不会撞
2. 会不会抖
3. 会不会慢

### 第四步：按优先级调

- 撞 → 先调 `occdist_scale` / `inflation_radius`
- 抖 → 先调 `min_vel_trans` / `min_vel_theta` / goal tolerances
- 慢 → 最后再调 `max_vel_x` / `max_vel_theta`

---

## 给比赛的一个保守推荐配置

```yaml
max_vel_x: 0.25
min_vel_x: -0.15
max_vel_y: 0.20
min_vel_y: -0.20
max_vel_trans: 0.25
min_vel_trans: 0.06
max_vel_theta: 1.0
min_vel_theta: 0.3

sim_time: 1.5
vx_samples: 12
vy_samples: 10
vth_samples: 20

path_distance_bias: 32.0
goal_distance_bias: 20.0
occdist_scale: 0.6

xy_goal_tolerance: 0.15
yaw_goal_tolerance: 0.15
```

这组参数比当前版本更像“真的拿去比赛”的配置：

- 会横移
- 更怕障碍
- 靠近目标时更柔和
- 速度仍然不算激进

---

## 源码参考

| 文件 | 路径 |
|------|------|
| DWA 参数 | `src/robot_slam/params/carto/dwa_local_planner_params.yaml` |
| move_base 主参数 | `src/robot_slam/params/carto/move_base_params.yaml` |
| 参数加载入口 | `src/robot_slam/launch/include/move_base.launch.xml` |
