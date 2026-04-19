# ABOT ROS1 学习文档

> 万创鑫诚教育机器人竞赛小车源代码学习指南

## 项目概述

这是一个基于 ROS1 Melodic 的教育机器人（麦克纳姆轮全向小车）的完整源代码。系统运行于 Ubuntu 18.04，Python 2.7 环境。包含从底层硬件驱动到上层竞赛任务逻辑的完整功能栈。

---

## 文档地图

```
docs/
├── 📖 README.md                          ← 你在这里
├── 01-ros-fundamentals/                  ROS1 第一性原理
│   └── README.md                         章节入口
├── 02-system-architecture/               系统整体架构与数据流
│   └── README.md                         章节入口
├── 03-hardware-driver/                   硬件驱动层
│   ├── README.md                         硬件驱动层总览
│   ├── serial-protocol.md                串口通信协议详解
│   ├── motor-control.md                  电机控制与 PID 调参
│   ├── imu-driver.md                     IMU 驱动与校准
│   └── odom-ekf.md                       里程计与 EKF 融合
├── 04-orientation-filter/                姿态滤波
│   ├── README.md                         姿态滤波总览
│   ├── mahony-filter.md                  Mahony 互补滤波器
│   └── madgwick-filter.md                Madgwick 梯度下降滤波器
├── 05-slam-navigation/                   SLAM 与导航
│   ├── README.md                         SLAM 与导航总览
│   ├── gmapping.md                       Gmapping 建图
│   ├── cartographer.md                   Cartographer 建图
│   ├── amcl.md                           AMCL 定位
│   ├── costmap.md                        代价地图详解
│   ├── global-planner.md                 全局路径规划
│   ├── dwa-local-planner.md              DWA 局部规划与调参 ⭐
│   ├── move-base.md                      move_base 框架
│   └── competition-tuning.md             竞赛调参实战指南 ⭐
├── 06-perception/                        感知
│   ├── README.md                         感知层总览
│   ├── ar-tag-tracking.md                AR 标签检测与追踪
│   ├── find-object-2d.md                 2D 物体识别
│   └── cam-track-pid.md                  视觉追踪 PID 控制
├── 07-competition-tasks/                 竞赛任务
│   ├── README.md                         竞赛任务层总览
│   ├── mission-node.md                   任务状态机详解 ⭐
│   ├── shoot-control.md                  发射机构控制
│   ├── voice-interaction.md              语音交互
│   └── multi-goal-navigation.md          多目标导航策略
└── 08-competition-playbook/              竞赛实战手册 ⭐
    └── README.md                         章节入口
```

⭐ = 竞赛中最常需要参考的文档

---

## 推荐阅读路径

### 路径 A："我什么都不懂，从零开始"
[`01-ros-fundamentals/README.md`](01-ros-fundamentals/README.md) → [`02-system-architecture/README.md`](02-system-architecture/README.md) → [`03-hardware-driver/README.md`](03-hardware-driver/README.md) → [`03-hardware-driver/serial-protocol.md`](03-hardware-driver/serial-protocol.md) → [`03-hardware-driver/motor-control.md`](03-hardware-driver/motor-control.md) → [`04-orientation-filter/README.md`](04-orientation-filter/README.md) → [`05-slam-navigation/README.md`](05-slam-navigation/README.md) → [`05-slam-navigation/dwa-local-planner.md`](05-slam-navigation/dwa-local-planner.md) → [`06-perception/README.md`](06-perception/README.md) → [`07-competition-tasks/mission-node.md`](07-competition-tasks/mission-node.md) → [`08-competition-playbook/README.md`](08-competition-playbook/README.md)

### 路径 B："我想知道怎么调参让车跑得更好"
[`05-slam-navigation/dwa-local-planner.md`](05-slam-navigation/dwa-local-planner.md) → [`05-slam-navigation/competition-tuning.md`](05-slam-navigation/competition-tuning.md) → [`03-hardware-driver/motor-control.md`](03-hardware-driver/motor-control.md) → [`05-slam-navigation/costmap.md`](05-slam-navigation/costmap.md) → [`08-competition-playbook/README.md`](08-competition-playbook/README.md)

### 路径 C："我想修改竞赛任务逻辑"
[`07-competition-tasks/mission-node.md`](07-competition-tasks/mission-node.md) → [`07-competition-tasks/multi-goal-navigation.md`](07-competition-tasks/multi-goal-navigation.md) → [`06-perception/cam-track-pid.md`](06-perception/cam-track-pid.md) → [`07-competition-tasks/shoot-control.md`](07-competition-tasks/shoot-control.md) → [`08-competition-playbook/README.md`](08-competition-playbook/README.md)

### 路径 D："我的车有问题，需要排查"
[`08-competition-playbook/README.md`](08-competition-playbook/README.md)（竞赛实战手册）→ 根据故障现象查阅对应章节

---

## 关键术语速查

| 术语 | 全称 | 含义 |
|------|------|------|
| `cmd_vel` | Command Velocity | 速度指令话题，控制机器人运动 |
| `odom` | Odometry | 里程计，机器人的位置和速度估计 |
| `TF` | Transform Frame | ROS 坐标变换系统 |
| `costmap` | Cost Map | 代价地图，表示环境中障碍物位置和代价值 |
| `DWA` | Dynamic Window Approach | 动态窗口法，局部路径规划算法 |
| `SLAM` | Simultaneous Localization and Mapping | 同步定位与建图 |
| `AMCL` | Adaptive Monte Carlo Localization | 自适应蒙特卡洛定位 |
| `EKF` | Extended Kalman Filter | 扩展卡尔曼滤波，传感器融合算法 |
| `PID` | Proportional-Integral-Derivative | 比例-积分-微分控制器 |
| `IMU` | Inertial Measurement Unit | 惯性测量单元（加速度计+陀螺仪+磁力计） |
| `URDF` | Unified Robot Description Format | 统一机器人描述格式 |

---

## 代码仓库结构

```
abot/
├── src/
│   ├── abot_base/                底盘基础包（4 个子包）
│   │   ├── abot_bringup/         电机驱动、里程计、串口通信
│   │   ├── abot_imu/             IMU 驱动与校准
│   │   ├── abot_model/           URDF 机器人模型与 Gazebo 仿真
│   │   └── lidar_filters/        激光雷达滤波
│   ├── imu_filter/               姿态滤波（Mahony/Madgwick）
│   ├── robot_slam/               SLAM、导航、建图、定位
│   ├── cam_track/                视觉追踪（PID 控制转向）
│   ├── user_demo/                任务状态机（航点导航+射击）
│   ├── shoot_cmd/                发射机构串口控制
│   ├── abot_find/                `find_object_2d` ROS 包源码目录
│   └── robot_voice/              科大讯飞语音交互
```
