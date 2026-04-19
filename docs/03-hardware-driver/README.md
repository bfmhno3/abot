# 硬件驱动层总览

> 与物理世界对话的桥梁

## 为什么需要驱动层？

传感器和执行器不懂 ROS——它们只懂电信号和串口协议。驱动层的职责是：**把硬件的语言翻译成 ROS 的语言**。

- 电机控制器通过串口通信 → 驱动层把速度指令翻译成串口帧，把返回的编码器数据翻译成 Odometry 消息
- IMU 通过串口返回原始数据 → 驱动层校准后发布为标准 `sensor_msgs/Imu`
- 激光雷达通过 USB 返回点云 → 驱动层发布为 `sensor_msgs/LaserScan`

## 架构：Transport → Dataframe → BaseDriver

`abot_bringup` 包采用三层架构，每一层只关心自己的职责：

```
┌──────────────────────────────────┐
│         BaseDriver               │  ROS 层：订阅/发布话题
│  cmd_vel_callback()              │
│  update_odom() → publish odom     │
│  update_speed() → send velocity  │
├──────────────────────────────────┤
│       Simple_dataframe           │  协议层：组帧/解帧
│  interact(ID_SET_VELOCITY)       │
│  send_message() / data_recv()    │
│  [0x5A][id][len][data][checksum] │
├──────────────────────────────────┤
│       Serial_transport           │  传输层：串口读写
│  Boost.Asio async I/O            │
│  read_buffer / write_buffer      │
│  /dev/abot @ 921600 baud         │
└──────────────────────────────────┘
```

这种分层的妙处在于：如果将来换用 TCP 或 CAN 总线通信，只需要替换 Transport 层，上两层完全不变。

## 两个包的协作

```
abot_driver (abot_bringup)         abot_imu
        │                              │
        │ 读取 IMU 原始数据             │ 读取 IMU 原始数据
        │ (imu_type == 'E' 时)          │
        │                              │
        ├──→ /raw_imu ──→ abot_imu ──→ /imu/data_raw ──→ Mahony/Madgwick
        │                              │
        │ /wheel_odom（轮式里程计）       │ /imu/mag (磁力计)
        │ /tf (odom→base_link)          │
        ▼                              ▼
    robot_pose_ekf (EKF 融合)
```

> [!NOTE]
>
> 当 `imu_type == 'E'`（ASCII 69）时，`abot_driver` 额外承担了转发 IMU 原始数据到 `/raw_imu` 话题的职责。IMU 数据的校准和转换由 `abot_imu` 节点完成。

## 本章节文档

| 文档 | 内容 | 何时阅读 |
|------|------|----------|
| [串口通信协议详解](serial-protocol.md) | 帧格式、消息类型、数据结构 | 需要理解或修改底层通信时 |
| [电机控制与 PID 调参](motor-control.md) | cmd_vel 处理、里程计、PID 调参 | 机器人运动不正常时 |
| [IMU 驱动与校准](imu-driver.md) | IMU 校准流程、磁力计校准 | 姿态角不准时 |
| [里程计与 EKF 融合](odom-ekf.md) | 轮式里程计原理、EKF 融合策略 | 定位不准时 |
