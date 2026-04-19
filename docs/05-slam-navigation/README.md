# SLAM 与导航总览

> 从“我在哪”到“我该怎么走”

## 导航栈全貌

在 abot 项目里，导航栈负责回答四个问题：

1. **地图是什么样？** → SLAM（Gmapping / Cartographer）
2. **我现在在哪？** → AMCL 定位
3. **从这里到目标点大概要怎么走？** → GlobalPlanner 全局规划
4. **下一秒钟电机该怎么转？** → DWA 局部规划

把它们串起来就是：

```
激光雷达 / 里程计 / IMU
        │
        ▼
  建图（Gmapping / Cartographer）
        │ 生成 /map
        ▼
   AMCL 在地图上定位
        │ 发布 map → odom
        ▼
 GlobalPlanner 规划全局路径
        │ 发布 global plan
        ▼
 DWA 局部规划器采样速度轨迹
        │ 输出 /cmd_vel
        ▼
    abot_driver 驱动电机
```

---

## 两种运行模式

### 1. 建图模式

适用于第一次进入赛场，或者赛场布局变化较大时。

```bash
roslaunch abot_bringup robot_with_imu.launch
roslaunch robot_slam gmapping.launch
```

此时机器人一边移动，一边构建地图。建图完成后保存：

```bash
rosrun map_server map_saver -f ~/abot/src/robot_slam/maps/my_lab
```

### 2. 导航模式

适用于比赛正式运行。地图已经提前建好。

```bash
roslaunch abot_bringup robot_with_imu.launch
roslaunch robot_slam navigation.launch
```

此时系统加载静态地图，AMCL 在地图上定位，`move_base` 开始工作。

---

## scan_filtered 话题的作用

导航栈并不直接使用 `/scan`，而是使用 `/scan_filtered`。

来源关系：

```
rplidar ──→ /scan ──→ lidar_filters ──→ /scan_filtered ──→ gmapping / amcl / move_base
```

这样做的意义是：

- 可以在 `lidar_filters` 中去掉车体自身的反射点
- 可以裁剪掉无效量程
- 避免脏数据直接污染地图、定位和避障

> [!TIP]
>
> 如果你发现建图有 “车体自己的轮廓” 或者定位时雷达点云明显错乱，第一件事就是检查 `/scan_filtered` 是否正常。

---

## 导航中的 5 个核心组件

| 组件 | 作用 | 你最常调的参数 |
|------|------|----------------|
| Gmapping | 在线建图 | `particles`, `minimumScore`, `delta` |
| Cartographer | 另一套 SLAM 建图 | `num_range_data`, `min_score` |
| AMCL | 静态地图定位 | `min`/`max_particles`, `odom_alpha`, `recovery_alpha` |
| Costmap | 环境障碍表示 | `inflation_radius`, `obstacle_range` |
| DWAPlannerROS | 局部轨迹生成 | `max_vel_x`, `vy_samples`, `occdist_scale` |

---

## 这一层最重要的认知

### 1. 地图、定位、规划是串联关系

只要前面一层错了，后面再怎么调都没用：

- 地图歪了 → AMCL 再准也会错
- AMCL 飘了 → 全局路径看起来“合理”，但对机器人来说是错的
- 全局路径没问题 → DWA 参数差，仍然会撞墙或抖动

### 2. “机器人行为”本质上是参数的外显

比如：

- **贴墙走** → `inflation_radius` 太小 / `occdist_scale` 太低
- **到目标点附近抖动** → `xy_goal_tolerance` 太小 / `min_vel_trans` 太大
- **不会横移** → `vy_samples = 0`
- **定位丢失后发疯** → `recovery_alpha_fast = 0`

### 3. 先保证安全，再追求速度

竞赛里最容易犯的错误不是 “慢”，而是 “撞”。

建议调参顺序：

1. 先让机器人稳定不撞
2. 再让它更平滑
3. 最后再提速度

---

## 本章节文档导航

| 文档 | 用途 |
|------|------|
| [gmapping.md](gmapping.md) | 理解 Gmapping 建图原理与参数 |
| [cartographer.md](cartographer.md) | 理解 Cartographer 配置与调优 |
| [amcl.md](amcl.md) | 理解定位机制和丢定位问题 |
| [costmap.md](costmap.md) | 理解障碍物是怎么 “变成代价” 的 |
| [global-planner.md](global-planner.md) | 理解全局路径为何这样规划 |
| [dwa-local-planner.md](dwa-local-planner.md) | 理解局部规划和速度调参 |
| [move-base.md](move-base.md) | 理解整个导航执行框架 |
| [competition-tuning.md](competition-tuning.md) | 比赛前的参数调整实战手册 |
