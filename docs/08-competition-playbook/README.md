# 竞赛实战手册

> 比赛当天的操作指南、故障排查、参数速查

## 本章节导航

- 当前主文档：`README.md`
- 后续可扩展子主题：
  - 赛前检查清单
  - 分步启动与一键启动策略
  - 建图与定位实战建议
  - 参数速查与安全/激进配置
  - 故障排查决策树
  - 实时调试工具与比赛前修复项

---

## 赛前检查清单

### 硬件检查

- [ ] 机器人电池满电（低电压会导致电机输出不足、传感器异常）
- [ ] 激光雷达数据线连接稳固（松动会导致 scan 话题中断）
- [ ] IMU 固定牢靠（松动会引入振动噪声）
- [ ] 摄像头焦距已调好（AR 标签识别率取决于清晰度）
- [ ] 发射机构装填完毕，串口连接正常
- [ ] 轮胎气压正常 / 无明显磨损
- [ ] 所有 USB 设备已被 `ls /dev/ttyACM*` 和 `ls /dev/video*` 识别

### 软件检查

- [ ] `roscore` 可以正常启动
- [ ] 所有功能包已编译（`catkin_make` 无错误）
- [ ] 地图文件存在于 `src/robot_slam/maps/` 目录
- [ ] YAML 参数文件中的航点坐标已更新为赛场坐标
- [ ] AR 标签 ID 配置与赛场标签一致（`mission.yaml` 中的 `Ar_0_id` 等）

### 已知问题确认

- [ ] `dwa_local_planner_params.yaml` 中 `vy_samples` 已改为非零值
- [ ] `occdist_scale` 已从 0.03 提高到合理值
- [ ] `inflation_radius` 已从 0.1 提高到 0.2+
- [ ] `amcl.launch.xml` 中 `recovery_alpha_fast` 已设为非零值
- [ ] 射击命令格式已统一

---

## 完整启动流程

### 方式 A：一键启动（如果你本地有 6-mission.sh）

```bash
cd ~/abot
source devel/setup.bash
cd ~
bash 6-mission.sh  # 前提：你的家目录里已有这个本地脚本
```

> [!NOTE]
> 这是一种历史上常见的本地一键启动方式，不代表当前仓库里自带 `6-mission.sh`。如果你本机没有这份脚本，直接按下面的逐步启动方式执行更稳妥。

> [!WARNING]
>
> 一键启动的问题：所有终端同时打开，7 个标签页同时开始加载，如果某个依赖还没就绪就会报错。适合熟练后使用。

### 方式 B：逐步启动（推荐调试时使用）

```bash
# 终端 1：ROS Master
roscore

# 等待 roscore 启动完成，然后：

# 终端 2：硬件驱动
source devel/setup.bash
roslaunch abot_bringup robot_with_imu.launch

# 终端 3：导航栈（等驱动就绪后）
roslaunch robot_slam navigation.launch

# 终端 4：摄像头 + AR 标签检测
source ~/abot_vision/devel/setup.bash
roslaunch track_tag usb_cam_with_calibration.launch

# 终端 5：任务节点
roslaunch user_demo mission.launch
```

逐步启动的优势：可以看到每一步的输出，立即发现问题。

---

## 赛场建图最佳实践

### 建图参数建议

建图时应使用**保守参数**确保地图质量：

```yaml
# gmapping.launch.xml 中的建议修改
particles: 30        # 默认 8 太少，提高到 30
delta: 0.05          # 默认 0.02 太精细，0.05 足够
minimumScore: 50     # 默认 30，提高以减少错误匹配
```

### 建图操作步骤

1. 启动 gmapping 模式
2. 用键盘/手柄控制机器人
3. **缓慢移动**（< 0.3 m/s），避免地图畸变
4. **完整走遍**所有区域，包括走廊和死角
5. **回到起点**完成回环（loop closure）
6. 观察地图是否有断裂、重影，如有则重走该区域
7. 保存地图：
   ```bash
   rosrun map_server map_saver -f ~/abot/src/robot_slam/maps/my_lab
   ```
8. 验证地图：用 `eog my_lab.pgm` 查看

### 建图常见问题

| 现象 | 可能原因 | 解决方法 |
|------|----------|----------|
| 地图断裂 | 移动太快 | 降速重走 |
| 地图重影 | 里程计漂移严重 | 检查 IMU 校准，重走 |
| 地图有黑洞 | 激光雷达被遮挡 | 检查雷达安装位置 |
| 地图变形 | 轮径参数不准 | 校准 `wheel_diameter` |

---

## 参数速查表

### 下位机 PID 参数（`abot_bringup`）

| 参数 | 安全值 | 默认值 | 激进值 | 说明 |
|------|--------|--------|--------|------|
| `wheel_diameter` | 实测值 | 97/115 | 实测值 | 用卡尺测量，单位 mm |
| `wheel_track` | 实测值 | 225/300 | 实测值 | 左右轮中心距，单位 mm |
| `encoder_resolution` | 实测值 | 3960/1560 | 实测值 | 编码器线数×4 |
| `kp` | 100 | 320 | 500 | 比例增益，越大响应越快 |
| `ki` | 500 | 2700 | 3000 | 积分增益，消除稳态误差 |
| `kd` | 0 | 0 | 10 | 微分增益，抑制超调 |
| `cmd_last_time` | 500 | 250 | 200 | 命令超时(ms)，太长不安全 |
| `max_v_liner_x` | 40 | 50 | 80 | 最大线速度(cm/s) |
| `max_v_angular_z` | 150 | 250 | 400 | 最大角速度(0.01 rad/s) |

### DWA 局部规划器参数（robot_slam）

| 参数 | 安全值 | 当前值 | 激进值 | 说明 |
|------|--------|--------|--------|------|
| `max_vel_x` | 0.2 | 0.2 | 0.5 | 最大前进速度(m/s) |
| `max_vel_theta` | 1.0 | 1.4 | 2.0 | 最大旋转速度(rad/s) |
| `vy_samples` | **10** | **0** | 15 | 横向速度采样数（必改！） |
| `occdist_scale` | **0.5** | **0.03** | 0.2 | 避障权重（必改！） |
| `path_distance_bias` | 32 | 32 | 48 | 路径跟随权重 |
| `goal_distance_bias` | 24 | 24 | 36 | 目标接近权重 |
| `sim_time` | 2.0 | 2.0 | 1.5 | 前向仿真时长(s) |
| `xy_goal_tolerance` | 0.15 | 0.15 | 0.10 | 位置容差(m) |
| `yaw_goal_tolerance` | 0.15 | 0.15 | 0.10 | 朝向容差(rad) |

### 代价地图参数

| 参数 | 安全值 | 当前值 | 激进值 | 说明 |
|------|--------|--------|--------|------|
| `inflation_radius` | **0.25** | **0.1** | 0.15 | 膨胀半径(m)（必改！） |
| `cost_scaling_factor` | 3.0 | 3.0 | 5.0 | 代价衰减因子 |
| `obstacle_range` | 3.0 | 3.0 | 4.0 | 障碍物标记范围(m) |

### AMCL 定位参数

| 参数 | 安全值 | 当前值 | 激进值 | 说明 |
|------|--------|--------|--------|------|
| `min_particles` | 500 | 500 | 200 | 最少粒子数 |
| `max_particles` | 2000 | 2000 | 5000 | 最多粒子数 |
| `recovery_alpha_fast` | **0.1** | **0.0** | 0.05 | 全局重定位（必改！） |
| `update_min_d` | 0.25 | 0.25 | 0.15 | 最小更新距离(m) |

### 视觉追踪 PID 参数

| 参数 | 安全值 | 当前值 | 激进值 | 说明 |
|------|--------|--------|--------|------|
| `PID_Control_P` | 0.5 | 0.5/0.75 | 1.0 | P 增益 |
| `Max_yaw_vel` | 0.2 | 0.2 | 0.4 | 最大转向速度 |
| `yaw_th` | 0.03 | 0.02/0.035 | 0.01 | 对齐阈值(rad) |

---

## 常见故障排查决策树

### 机器人不动

```
机器人不动
├── rostopic echo /cmd_vel 有数据？
│   ├── 否 → move_base 没在发命令
│   │   ├── rostopic echo /move_base/status 有输出？
│   │   │   ├── 否 → 没有目标点，检查是否发布了 goal
│   │   │   └── 是 → move_base 在规划但无法执行
│   │   │       └── 检查 costmap 是否全被标记为障碍
│   └── 是 → abot_driver 收到了但不执行
│       ├── 检查串口连接：ls /dev/abot
│       ├── 检查 abot_driver 节点是否在运行
│       └── 检查 cmd_last_time 是否过短导致立即超时
└── roscore 在运行？
    └── 否 → 启动 roscore
```

### 机器人偏离路径 / 撞墙

```
撞墙/偏离
├── 地图质量好吗？
│   ├── 否 → 重新建图
│   └── 是 →
│       ├── inflation_radius 太小？→ 提高到 0.25
│       ├── occdist_scale 太小？→ 提高到 0.5
│       └── 定位准确吗？→ 观察 AMCL 粒子分布
│           ├── 粒子分散 → 定位丢失
│           │   ├── 重新发布初始位姿
│           │   └── 检查 recovery_alpha_fast 是否为 0
│           └── 粒子集中 → 定位正常，检查规划参数
```

### AR 标签检测不到

```
检测不到 AR 标签
├── rostopic echo /ar_pose_marker 有数据？
│   ├── 否 → 摄像头或 ar_track_alvar 问题
│   │   ├── 检查摄像头：rostopic echo /camera/image
│   │   ├── 检查曝光（太亮/太暗都识别不了）
│   │   └── 检查标签大小和打印质量
│   └── 是 → mission_node 没收到
│       ├── 检查话题名称是否匹配
│       └── 检查标签 ID 是否匹配配置
```

### 机器人不停转圈

```
不停转圈
├── 是在找 AR 标签？
│   └── 是 → 正常的恢复旋转行为
│       └── 如果转太久 → 检查摄像头是否正常
├── 是 move_base 的行为？
│   └── 检查 DWA 轨迹评分是否振荡
│       ├── 降低 max_vel_theta
│       ├── 增加 oscillation_reset_dist
│       └── 检查 vy_samples 是否为 0（全向车可能因此振荡）
└── 是里程计问题？
    └── 检查 /odom 输出是否正常
        └── 轮径参数是否准确
```

---

## 实时调试技巧

### 可视化节点和话题关系

```bash
# 方法 1：rqt_graph（推荐）
rosrun rqt_graph rqt_graph
# 选择 "nodes_only" 模式查看节点关系
# 选择 "nodes/topics" 模式查看话题连接

# 方法 2：命令行查看
rosnode list          # 列出所有活跃节点
rosnode info /abot_driver  # 查看某节点的订阅/发布/服务
rostopic list         # 列出所有话题
```

### 实时监控话题数据

```bash
# 查看速度指令
rostopic echo /cmd_vel

# 查看里程计
rostopic echo /odom

# 查看导航状态
rostopic echo /move_base/status

# 查看频率
rostopic hz /scan_filtered   # 检查雷达数据率
rostopic hz /odom            # 检查里程计发布率
```

### 动态调参

```bash
# 方法 1：GUI（推荐）
rosrun rqt_reconfigure rqt_reconfigure
# 在左侧树中找到对应节点，调整参数

# 方法 2：命令行
rosrun dynamic_reconfigure dynparam set /abot_driver max_v_liner_x 80
rosrun dynamic_reconfigure dynparam get /abot_driver  # 查看当前所有参数
```

### RViz 调试可视化

在 RViz 中添加以下 Display 可以帮助调试：

| Display 类型 | 话题 | 作用 |
|-------------|------|------|
| `LaserScan` | `/scan_filtered` | 查看雷达数据是否正常 |
| `Map` | `/map` | 查看当前地图 |
| `Path` | `/move_base/GlobalPlanner/plan` | 查看全局规划路径 |
| `Path` | `/move_base/DWAPlannerROS/local_plan` | 查看局部规划路径 |
| `PoseArray` | `/particlecloud` | 查看 AMCL 粒子分布 |
| `TF` | — | 查看坐标变换树 |
| `Camera` | `/camera/image` | 查看摄像头画面 |

---

## 必须修复的代码问题清单

> [!CAUTION]
>
> 以下问题建议在比赛前修复，按优先级排序。

### P0 - 必须修复（直接影响功能）

| # | 问题 | 文件 | 修复方法 |
|---|------|------|----------|
| 1 | `vy_samples: 0` 导致全向车无横向运动 | `src/robot_slam/params/carto/dwa_local_planner_params.yaml` | 改为 `vy_samples: 10` |
| 2 | 射击命令格式不统一 | `src/cam_track/include/cam_track/cam_track_node.hpp` | 将 `"shoot!"` 改为 `"shoot"` |

### P1 - 强烈建议修复（影响安全性）

| # | 问题 | 文件 | 修复方法 |
|---|------|------|----------|
| 3 | `occdist_scale: 0.03` 避障几乎无效 | `src/robot_slam/params/carto/dwa_local_planner_params.yaml` | 改为 `occdist_scale: 0.5` |
| 4 | `inflation_radius: 0.1` 路径贴墙 | `src/robot_slam/params/carto/costmap_common_params.yaml` | 改为 `inflation_radius: 0.25` |
| 5 | AMCL 恢复机制禁用 | `src/robot_slam/launch/include/amcl.launch.xml` | 设 `recovery_alpha_fast: 0.1` |

### P2 - 建议修复（影响可维护性）

| # | 问题 | 文件 | 修复方法 |
|---|------|------|----------|
| 6 | `read_param()` 硬编码覆盖参数 | `src/abot_base/abot_bringup/src/base_driver.cpp` | 删除 hardcoded 值，改为从参数服务器读取 |
| 7 | 参数默认值在 data_holder.h 和 .cfg 之间不一致 | 多个文件 | 统一所有默认值为实际测量值 |
| 8 | `control_center.cpp` 中有变量名错误 | `src/shoot_cmd/src/control_center.cpp` | 修复 C_goal/D_goal 变量名 |

---

## 调参记录模板

每次调参时记录以下信息，方便回溯：

```
日期：____
赛场：____
问题描述：____

修改的参数：
- 文件：____  参数：____  旧值：____  新值：____
- 文件：____  参数：____  旧值：____  新值：____

修改后的效果：
□ 明显改善
□ 略有改善
□ 无变化
□ 变差

备注：____
```
