# 竞赛调参实战指南

> 不是 “把每个参数都调懂”，而是 “先把车调到能赢”

## 最重要的策略：先分层，再调参

不要一上来就到处改 YAML。比赛调参一定要按层次来：

1. **底盘层**：轮子能不能稳定执行速度命令？
2. **定位层**：机器人知道自己在哪吗？
3. **环境层**：costmap 画得对吗？
4. **局部规划层**：DWA 会不会撞？会不会抖？
5. **任务层**：航点和射击逻辑合理吗？

如果你跳过前面层直接调后面，通常只会把问题掩盖掉。

---

## 从零开始的调参清单（按优先级）

### Step 1：确认底盘与里程计

目标：让机器人“命令 0.2 m/s，实际大致也是 0.2 m/s”。

检查：

- `/cmd_vel` 是否正常发布
- `/odom` 是否方向正确、数值合理
- 直行 1m，里程计是否接近 1m
- 原地转 90°，yaw 是否接近 90°

如果不准，优先检查：

- `wheel_diameter`
- `wheel_track`
- `encoder_resolution`
- 下位机 PID

### Step 2：确认定位

目标：机器人在地图上的位置和实际位置一致。

检查：

- RViz 里激光点云是否和墙对齐
- `/particlecloud` 是否集中
- 推一下机器人后，AMCL 能否重新收敛

优先调：

- `recovery_alpha_fast`
- `odom_alpha1~5`
- `min_particles/max_particles`

### Step 3：确认 costmap

目标：机器人不要贴墙，也不要觉得哪里都不能走。

检查：

- 障碍边界是否合理
- footprint 是否匹配真实车体
- 膨胀层是否太薄

优先调：

- `inflation_radius`
- `cost_scaling_factor`
- `footprint`

### Step 4：调 DWA

目标：不撞、不抖、动作像个正常机器人。

先修掉关键问题：

```yaml
vy_samples: 10
occdist_scale: 0.5
```

再逐步调：

- `min_vel_trans`
- `min_vel_theta`
- `max_vel_x`
- `max_vel_theta`
- `sim_time`

### Step 5：最后才调任务逻辑

目标：航点顺序、射击时机、视觉追踪都顺。

这一步之前，默认导航系统已经稳定。

---

## 参数耦合关系

很多参数不是独立的，改一个会影响另一个。

### 1. `inflation_radius` × `occdist_scale`

- `inflation_radius` 决定障碍影响范围
- `occdist_scale` 决定障碍影响权重

这两个一起决定“机器人到底有多怕障碍”。

### 2. `min_vel_trans` × `xy_goal_tolerance`

- `min_vel_trans` 太大
- `xy_goal_tolerance` 太小

组合后经常导致：

> 机器人想精准到点，但又不允许自己慢下来，于是开始来回修正。

### 3. `vy_samples` × `holonomic_robot`

- `holonomic_robot=true`
- 但 `vy_samples=0`

这是明显矛盾：系统嘴上承认你是全向车，行动上却不让你横着走。

### 4. `odom_alpha` × DWA 参数

如果定位本身就漂：
- DWA 再怎么调，也会“看起来不聪明”

所以定位误差会放大成路径误差，进而误导你去错调 DWA。

---

## 两套推荐配置思路

### 方案 A：安全优先（推荐先用）

适合第一次上场、地图不太稳、你更怕撞而不是更怕慢。

```yaml
# costmap
inflation_radius: 0.25
cost_scaling_factor: 2.5

# DWA
max_vel_x: 0.25
max_vel_theta: 1.0
min_vel_trans: 0.06
min_vel_theta: 0.3
vy_samples: 10
occdist_scale: 0.6
sim_time: 1.5

# AMCL
recovery_alpha_fast: 0.1
```

表现：

- 会更慢一点
- 更稳
- 不容易撞
- 更适合先把整套比赛流程跑通

### 方案 B：速度优先（在 A 稳定后再尝试）

```yaml
# costmap
inflation_radius: 0.18
cost_scaling_factor: 3.0

# DWA
max_vel_x: 0.40
max_vel_theta: 1.5
min_vel_trans: 0.08
min_vel_theta: 0.4
vy_samples: 12
occdist_scale: 0.4
sim_time: 1.2
```

表现：

- 更快
- 更激进
- 对地图精度和定位质量要求更高
- 容错更差

> [!WARNING]
>
> 比赛中如果你还没有把安全配置跑稳定，不建议直接上速度优先方案。

---

## 常见失败场景与调法

### 场景 1：出发后贴墙走

优先处理：

1. `inflation_radius ↑`
2. `occdist_scale ↑`
3. 检查 AMCL 是否有轻微偏移

### 场景 2：终点附近左右抖

优先处理：

1. `min_vel_theta ↓`
2. `min_vel_trans ↓`
3. `xy_goal_tolerance ↑`（必要时）

### 场景 3：不会横移，只会笨拙转向

优先处理：

1. `vy_samples = 10`
2. 确认 `holonomic_robot = true`

### 场景 4：场地里偶尔突然定位错位

优先处理：

1. `recovery_alpha_fast ↑`
2. `odom_alpha` 根据打滑情况微调
3. 地图重建 / 提高地图质量

### 场景 5：机器人很稳但就是太慢

最后再处理：

1. `max_vel_x ↑`
2. `max_vel_theta ↑`
3. `sim_time ↓`

---

## 一个非常实用的比赛调参顺序

如果比赛只剩很短时间，我建议按这个顺序做：

1. 修 `vy_samples=0`
2. 提 `occdist_scale`
3. 提 `inflation_radius`
4. 开启 `recovery_alpha_fast`
5. 降 `min_vel_trans`
6. 再考虑提 `max_vel_x`

为什么？

因为这 6 步对应的都是**最常见、收益最高、最不容易误伤系统**的点。

---

## 调参记录模板

建议你以后每次比赛前都维护这样一份记录：

```md
## 日期
2026-xx-xx

## 赛场
xxx 比赛场地

## 现象
- 贴墙
- 终点抖动
- 第二个航点附近丢定位

## 改动
- inflation_radius: 0.10 -> 0.25
- occdist_scale: 0.03 -> 0.60
- vy_samples: 0 -> 10

## 结果
- 贴墙明显改善
- 横移出现
- 终点仍有轻微抖动

## 下次尝试
- min_vel_theta: 0.8 -> 0.3
```

这比 “凭感觉记忆” 靠谱太多。

---

## 最后的比赛建议

比赛里最容易让人焦虑的是：

- 看到车跑得不对
- 立刻乱改一堆参数
- 结果不知道是哪一项生效了，或者一起坏了

真正有效的方法是：

> 一次只改 1~2 个强相关参数，改完立刻验证。

对你来说，最值得记住的一句话是：

> **先让机器人像个稳重的人，再把它训练成跑得快的人。**

---

## 参考文件

| 文件 | 路径 |
|------|------|
| DWA 参数 | `src/robot_slam/params/carto/dwa_local_planner_params.yaml` |
| 代价地图参数 | `src/robot_slam/params/carto/costmap_common_params.yaml` |
| AMCL 参数 | `src/robot_slam/launch/include/amcl.launch.xml` |
| move_base 参数 | `src/robot_slam/params/carto/move_base_params.yaml` |
