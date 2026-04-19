# 系统整体架构与数据流

> 从全局视角理解 abot 的 11 个 ROS 包如何协同工作

## 本章节导航

- 当前主文档：`README.md`
- 后续可扩展子主题：
  - 功能分层与模块职责
  - 导航/射击任务数据流
  - TF 树与坐标关系
  - 话题连接图
  - 启动时序与运行模式
  - 已知工程问题与风险点

---

## 架构总览

abot 系统分为 6 个功能层，数据从硬件层向上流动，控制指令从任务层向下流动：

```
┌─────────────────────────────────────────────────┐
│           任务层 (Competition Tasks)              │
│   mission_node / cam_track / control_center      │
│   "去哪、打什么、怎么打"                           │
├─────────────────────────────────────────────────┤
│           感知层 (Perception)                     │
│   ar_track_alvar / find_object_2d / usb_cam      │
│   "看到了什么、在哪"                               │
├─────────────────────────────────────────────────┤
│         导航层 (SLAM & Navigation)               │
│   gmapping/AMCL + move_base (DWA)               │
│   "我在哪、怎么走"                                │
├─────────────────────────────────────────────────┤
│         融合层 (Sensor Fusion)                   │
│   robot_pose_ekf + Mahony/Madgwick filter        │
│   "综合各传感器得到更准确的位姿"                    │
├─────────────────────────────────────────────────┤
│         驱动层 (Hardware Driver)                  │
│   abot_bringup + abot_imu + rplidar              │
│   "与硬件对话"                                    │
├─────────────────────────────────────────────────┤
│         硬件层 (Hardware)                         │
│   电机+编码器 / IMU / 激光雷达 / 摄像头 / 发射机构  │
└─────────────────────────────────────────────────┘
```

---

## 11 个 ROS 包的功能定位

| 包名 | 层级 | 功能 | 语言 | 关键节点 |
|------|------|------|------|----------|
| `abot_bringup` | 驱动层 | 电机驱动、串口通信、里程计 | C++ | `abot_driver` |
| `abot_imu` | 驱动层 | IMU 数据读取与校准 | C++ | `abot_imu` |
| `abot_model` | 驱动层 | URDF 机器人模型、Gazebo 仿真 | XML | 无（数据包） |
| `lidar_filters` | 驱动层 | 激光雷达数据滤波 | XML | 无（配置包） |
| `imu_filter` | 融合层 | Mahony/Madgwick 姿态解算 | C++ | `Mahony_filter` / `Madgwick_filter` |
| `robot_slam` | 导航层 | SLAM、AMCL、move_base、多目标导航 | C++/Python | `navigate_node` + 外部节点 |
| `cam_track` | 感知+任务 | 视觉追踪 AR 标签并 PID 转向射击 | C++ | `cam_track_node` |
| `user_demo` | 任务层 | 航点导航 + AR 标签对齐射击状态机 | C++ | `mission_node` |
| `shoot_cmd` | 驱动层 | 发射机构串口控制 | C++ | `shoot_control` |
| `find_object_2d` | 感知层 | 2D 特征匹配物体识别 | C++ | `find_object_2d_node` |
| `robot_voice` | 任务层 | 科大讯飞语音交互 | C++ | `voice_assistant` |

---

## 完整数据流图

### 导航模式下的数据流

```
                    ┌─────────────┐
                    │  rplidar     │
                    └──────┬──────┘
                           │ /scan
                           ▼
                    ┌──────────────┐
                    │ lidar_filters│ ──── /scan_filtered ───→ gmapping/AMCL
                    └──────────────┘
                                                    │
                                                    │ map → odom TF
                                                    ▼
┌─────────┐    /raw_imu    ┌─────────┐  /imu/data_raw  ┌──────────────┐
│ IMU硬件  │ ────────────→ │ abot_imu │ ─────────────→ │ Mahony/Madg  │
└─────────┘                └─────────┘                  └──────┬───────┘
                                                        │ /imu/data (quaternion+ypr)
                                                        │ odom → imu TF
                                                        ▼
                                                  ┌──────────────┐
┌─────────┐  串口协议   ┌──────────────┐ /odom   │robot_pose_ekf│
│电机+编码器│ ←────────→ │ abot_driver   │───────→│              │
└─────────┘             └──────┬───────┘         └──────┬───────┘
                               │                        │ 融合 odom
                               │ /cmd_vel               │ (EKF 输出)
                               ▼                        ▼
                        ┌──────────────────────────────────┐
                        │         move_base                │
                        │  ┌──────────┐  ┌──────────────┐ │
                        │  │GlobalPlanner│  │DWA LocalPlan│ │
                        │  │(Dijkstra)  │  │              │ │
                        │  └──────────┘  └──────┬───────┘ │
                        └───────────────────────┼─────────┘
                                                │ /cmd_vel
                                                ▼
                                         ┌──────────────┐
                                         │ abot_driver   │
                                         │ (电机控制)     │
                                         └──────────────┘
```

### 射击任务中的数据流

```
┌──────────┐  /camera/image   ┌────────────────┐  /ar_pose_marker
│ usb_cam   │ ───────────────→ │ ar_track_alvar │ ──────────────→
└──────────┘                   └────────────────┘                 │
                                                                   ▼
                                              ┌──────────────────────────────┐
                                              │       mission_node           │
                                              │                              │
                                              │ 1. 发布航点 → /move_base_simple/goal
                                              │ 2. move_base 到达后           │
                                              │ 3. P 控制 AR 标签对齐         │
                                              │ 4. 对齐后 → /shoot "1"       │
                                              └──────────┬───────────────────┘
                                                         │ /shoot
                                                         ▼
                                                  ┌──────────────┐
                                                  │ shoot_control │
                                                  │ (串口发射)     │
                                                  └──────────────┘
```

---

## TF 树结构

```
map                          ← 全局坐标系（由 SLAM/AMCL 提供）
 └── odom                    ← 里程计坐标系（由 EKF 融合提供）
      └── base_link          ← 机器人中心
           ├── laser_link    ← 激光雷达（URDF 静态定义）
           ├── imu           ← IMU 传感器（URDF 静态定义）
           └── camera_link   ← 摄像头（URDF 静态定义）
```

### TF 发布者与发布频率

| 变换 | 发布者 | 频率 | 备注 |
|------|--------|------|------|
| `map → odom` | AMCL / gmapping | ~10 Hz | 全局定位修正 |
| `odom → base_link` | robot_pose_ekf (EKF) | ~30 Hz | 融合轮速计+IMU |
| `odom → base_link` | abot_driver | ~1000 Hz | 无 IMU 时的轮式里程计 |
| `base_link → 传感器` | robot_state_publisher | 50 Hz | URDF 静态变换 |

> [!WARNING]
>
> `odom → base_link` 变换只能有一个发布者。当使用 IMU+EKF 时，`base_params_with_imu.yaml` 中 `publish_tf: true` 是因为 EKF 需要轮式里程计的 TF 作为输入，但 EKF 自己也发布 `odom → base_link`。实际运行中，由 `robot_pose_ekf` 节点接管此变换的发布。

---

## 话题连接图

### 核心控制话题

```
发布者                    话题                    订阅者
─────────────────────────────────────────────────────────
move_base (DWA)    ──→  /cmd_vel             ──→ abot_driver
mission_node       ──→  /cmd_vel             ──→ abot_driver
cam_track_node     ──→  /cmd_vel             ──→ abot_driver

abot_driver        ──→  /odom                ──→ robot_pose_ekf
                                                 move_base (local_costmap)

abot_imu           ──→  /raw_imu             ──→ abot_driver (转发)

Mahony/Madgwick    ──→  /imu/data_raw        ──→ robot_pose_ekf

robot_pose_ekf     ──→  /robot_pose_ekf/odom ──→ odom_ekf.py
odom_ekf.py        ──→  /odom_combined       ──→ (供 RViz 可视化)

rplidar            ──→  /scan                ──→ lidar_filters
lidar_filters      ──→  /scan_filtered       ──→ gmapping / AMCL / move_base
```

### 任务控制话题

```bash
发布者                    话题                    订阅者
─────────────────────────────────────────────────────────
mission_node       ──→  /move_base_simple/goal ──→ move_base
mission_node       ──→  /shoot                 ──→ shoot_control
ar_track_alvar     ──→  /ar_pose_marker        ──→ mission_node
pub_pos.py         ──→  /abot/pose             ──→ mission_node
move_base          ──→  /move_base/result      ──→ mission_node
voice_assistant    ──→  /snowman/ask           ──→ control_center
```

---

## 典型的一键启动时序

用 `gnome-terminal` 打开 7 个标签页，每个标签页启动一个 ROS 组件：

```bash
时间轴 ──────────────────────────────────────────────────→

0s  [Tab 1] roscore                                    ← ROS Master 必须先启动
3s  [Tab 2] abot_bringup robot_with_imu.launch         ← 硬件驱动（等待 roscore 就绪）
4s  [Tab 3] robot_slam navigation.launch               ← 导航栈（等待驱动就绪）
4s  [Tab 4] track_tag usb_cam_with_calibration.launch  ← 摄像头 + AR标签
3s  [Tab 5] find_object_2d find_object_2d.launch       ← 物体识别
4s  [Tab 6] robot_slam multi_goal.launch               ← 多目标导航
4s  [Tab 7] robot_slam view_nav.launch                 ← RViz 可视化
```

---

## 两种运行模式

### 建图模式

用于首次进入赛场，建立环境地图。

```bash
# 1. 启动硬件驱动
roslaunch abot_bringup robot_with_imu.launch

# 2. 启动 SLAM
roslaunch robot_slam gmapping.launch

# 3. 遥控机器人走遍赛场（使用键盘或手柄）

# 4. 保存地图
roslaunch robot_slam save_map.launch
```

建图时使用 Gmapping 或 Cartographer 实时构建栅格地图，地图保存在 `src/robot_slam/maps/` 目录。

### 导航模式

用于比赛，在已知地图上定位和导航。

```bash
# 1. 启动硬件驱动
roslaunch abot_bringup robot_with_imu.launch

# 2. 启动导航栈（含 AMCL 定位 + move_base）
roslaunch robot_slam navigation.launch

# 3. 启动任务节点
roslaunch user_demo mission.launch
# 或
roslaunch robot_slam multi_goal.launch
```

---

## 代码中发现的已知问题

> [!CAUTION]
>
> 以下问题可能直接影响竞赛表现，建议优先修复。

### 1. DWA 配置中 `vy_samples=0`（致命）

- **位置**: `src/robot_slam/params/carto/dwa_local_planner_params.yaml`
- **问题**: `holonomic_robot: true`（麦克纳姆轮）但 `vy_samples: 0`（不采样横向速度）
- **后果**: 机器人永远不会规划横向运动，全向能力被浪费
- **修复**: 设置 `vy_samples: 10`

### 2. DWA 避障权重极低（高危）

- **位置**: 同上 YAML 文件
- **问题**: `occdist_scale: 0.03`，而 `path_distance_bias: 32.0`、`goal_distance_bias: 24.0`
- **后果**: 避障效果几乎为零，机器人会撞向障碍物
- **修复**: 设置 `occdist_scale: 0.5~1.0`

### 3. AMCL 恢复机制被禁用（高危）

- **位置**: `src/robot_slam/launch/include/amcl.launch.xml`
- **问题**: `recovery_alpha_slow: 0.0`、`recovery_alpha_fast: 0.0`
- **后果**: 定位丢失后无法自恢复，机器人将盲目行动
- **修复**: 设置 `recovery_alpha_slow: 0.0`、`recovery_alpha_fast: 0.1`

### 4. 代价地图膨胀半径过小（中危）

- **位置**: `src/robot_slam/params/carto/costmap_common_params.yaml`
- **问题**: `inflation_radius: 0.1`（仅 10cm）
- **后果**: 路径会贴着墙壁规划，极易碰撞
- **修复**: 设置 `inflation_radius: 0.2~0.3`

### 5. read_param() 硬编码参数覆盖（中危）

- **位置**: `abot_base/abot_bringup/src/base_driver.cpp`
- **问题**: `read_param()` 读取下位机参数后，立即用 hardcoded 值覆盖
- **后果**: dynamic_reconfigure 修改的参数在节点重启后被覆盖
- **修复**: 删除 hardcoded 覆盖代码，改为从 YAML 读取

### 6. 射击命令消息不匹配（中危）

- **位置**: `cam_track_node.hpp` 发送 `"shoot!"`，`mission_node.hpp` 发送 `"1"`，`shoot_control.cpp` 期望 `"shoot"`
- **问题**: 三种不同的射击命令格式
- **后果**: 只有与 shoot_control 期望格式匹配的节点才能触发射击
- **修复**: 统一为 `"shoot"` 格式

### 7. 参数默认值不一致（低危）

- **位置**: `data_holder.h` 中的默认值与 `abot_parameter.cfg` 中的默认值不同
- **问题**: `wheel_diameter` 在 data_holder.h 中为 97mm，在 .cfg 中为 115mm
- **后果**: 编译时默认值与运行时默认值可能不同，造成混乱
- **修复**: 统一所有默认值

---

## 下一步

- 想了解底层硬件驱动？→ [硬件驱动层](../03-hardware-driver/README.md)
- 想了解导航参数怎么调？→ [SLAM 与导航](../05-slam-navigation/README.md)
- 想了解竞赛任务逻辑？→ [竞赛任务层](../07-competition-tasks/README.md)
