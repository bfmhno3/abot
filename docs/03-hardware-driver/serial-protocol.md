# 串口通信协议详解

> ROS 与电机控制器板之间的"语言"

## 为什么需要理解串口协议？

当你遇到以下问题时，理解协议是排查的关键：

- 电机完全不动（可能是帧格式错误）
- 里程计数值异常（可能是数据结构不对齐）
- 修改参数后不生效（可能是参数覆盖顺序问题）

---

## 帧格式

ROS（主机）与电机控制器板（从机）之间通过串口通信，每帧数据格式如下：

```
┌────────┬─────────┬──────────┬──────────────────┬──────────┐
│ 0x5A   │ msg_id  │ length   │ payload[0..N-1]  │ checksum │
│ (1 B)  │ (1 B)   │ (1 B)    │ (N B)            │ (1 B)    │
└────────┴─────────┴──────────┴──────────────────┴──────────┘
```

### 逐字段解析

| 字段 | 大小 | 含义 | 值域 |
|------|------|------|------|
| `0x5A` | 1 字节 | 固定帧头，标识一帧的开始 | 始终为 `0x5A` |
| `msg_id` | 1 字节 | 消息类型 ID | 0-7 |
| `length` | 1 字节 | 载荷数据长度 | 0-255 |
| `payload` | N 字节 | 有效数据 | 取决于消息类型 |
| `checksum` | 1 字节 | 校验和 | 0x00-0xFF |

### 校验和计算

校验和 = `(帧头 + msg_id + length + payload[0] + ... + payload[N-1]) & 0xFF`

即：从帧头到载荷最后一个字节的所有字节之和，取低 8 位。

```cpp
// 源码：simple_dataframe.h 中 Message 构造函数
check = 0;
check += head.flag;       // 0x5A
check += head.msg_id;
check += head.length;
for (int i = 0; i < head.length; i++) {
    check += data[i];
}
// check 的低 8 位即为校验和
```

---

## 消息类型

| ID | 枚举名 | 方向 | 载荷 | 说明 |
|----|--------|------|------|------|
| 0 | `ID_GET_VERSION` | 请求→响应 | 无请求/32B 响应 | 获取固件版本 |
| 1 | `ID_SET_ROBOT_PARAMTER` | 命令→确认 | 64B 请求/无响应数据 | 设置机器人参数 |
| 2 | `ID_GET_ROBOT_PARAMTER` | 请求→响应 | 无请求/64B 响应 | 读取机器人参数 |
| 3 | `ID_INIT_ODOM` | 命令→确认 | 无请求/无响应数据 | 重置里程计 |
| 4 | `ID_SET_VELOCITY` | 命令→确认 | 6B 请求/无响应数据 | 设置速度 |
| 5 | `ID_GET_ODOM` | 请求→响应 | 无请求/16B 响应 | 获取里程计 |
| 6 | `ID_GET_PID_DATA` | 请求→响应 | 无请求/32B 响应 | 获取 PID 调试数据 |
| 7 | `ID_GET_IMU_DATA` | 请求→响应 | 无请求/36B 响应 | 获取 IMU 原始数据 |

---

## 关键数据结构的内存布局

> [!IMPORTANT]
>
> 所有结构体使用 `#pragma pack(1)` 紧凑排列，无对齐填充。这是为了确保 C++ 结构体的内存布局与下位机固件一致。如果结构体大小不匹配，数据会错位，导致所有值都错误。

### `Robot_parameter`（64 字节）— 消息 ID 1/2

```
偏移  大小   类型     字段                   默认值(data_holder.h)  说明
──────────────────────────────────────────────────────────────────────────
0x00  2B   uint16  wheel_diameter            97              轮径(mm)
0x02  2B   uint16  wheel_track               225             轮距(mm)
0x04  2B   uint16  encoder_resolution         3960           编码器分辨率
0x06  1B   uint8   do_pid_interval            10             PID 周期(ms)
0x07  1B   (padding)                           —              对齐填充
0x08  2B   uint16  kp                         320            比例增益
0x0A  2B   uint16  ki                         2700           积分增益
0x0C  2B   uint16  kd                         0              微分增益
0x0E  2B   uint16  ko                         10             输出缩放因子
0x10  2B   uint16  cmd_last_time              250            命令超时(ms)
0x12  2B   uint16  max_v_liner_x              50             最大线速度
0x14  2B   uint16  max_v_liner_y              50             最大横向速度
0x16  2B   uint16  max_v_angular_z            250            最大角速度
0x18  1B   uint8   imu_type                   69 ('E')       IMU 类型
0x19  ...  (padding to 64B)                    —              填充至 64 字节
```

### `Robot_velocity`（6 字节）— 消息 ID 4

```
偏移  大小   类型     字段             单位          说明
──────────────────────────────────────────────────────────────
0x00  2B   int16   v_liner_x        cm/s          前进速度（正=前进）
0x02  2B   int16   v_liner_y        cm/s          横向速度（正=左移）
0x04  2B   int16   v_angular_z      0.01 rad/s    旋转速度（正=逆时针）
```

> [!NOTE]
>
> 注意单位转换：ROS 的 `geometry_msgs/Twist` 使用 m/s 和 rad/s，但串口协议使用 cm/s 和 0.01 rad/s。`cmd_vel_callback()` 中乘以 100 完成转换。

### `Robot_odom`（16 字节）— 消息 ID 5

```
偏移  大小   类型     字段             单位          说明
──────────────────────────────────────────────────────────────
0x00  2B   int16   v_liner_x        cm/s          当前线速度
0x02  2B   int16   v_liner_y        cm/s          当前横向速度
0x04  2B   int16   v_angular_z      0.01 rad/s    当前角速度
0x06  4B   int32   x                cm            X 坐标
0x0A  4B   int32   y                cm            Y 坐标
0x0E  2B   int16   yaw              0.01 rad      航向角
```

### `Robot_pid_data`（32 字节）— 消息 ID 6

```
偏移  大小   类型     字段              说明
──────────────────────────────────────────────────
0x00  16B  int32   input[4]          4 个电机的 PID 输入值
0x10  16B  int32   output[4]         4 个电机的 PID 输出值
```

### `IMU` 数据（36 字节）— 消息 ID 7

```
偏移  大小   类型     字段                    说明
────────────────────────────────────────────────────────────
0x00  4B   float   accelerometer_x         X 轴加速度
0x04  4B   float   accelerometer_y         Y 轴加速度
0x08  4B   float   accelerometer_z         Z 轴加速度
0x0C  4B   float   gyroscope_x             X 轴角速度
0x10  4B   float   gyroscope_y             Y 轴角速度
0x14  4B   float   gyroscope_z             Z 轴角速度
0x18  4B   float   magnetometer_x          X 轴磁场
0x1C  4B   float   magnetometer_y          Y 轴磁场
0x20  4B   float   magnetometer_z          Z 轴磁场
```

> [!NOTE]
>
> IMU 数据是 9 个 float（36 字节），**没有**使用 `#pragma pack(1)` 的紧凑结构体，而是存储在 `float imu_data[9]` 数组中。

---

## 交互时序

`interact()` 函数实现了严格的请求-响应模式：

```
主机 (ROS)                              从机 (电机控制器板)
    │                                        │
    │──── [0x5A][id][len][data][chk] ───────→│  发送请求帧
    │                                        │
    │     等待响应（150ms 超时）               │
    │                                        │
    │←─── [0x5A][id][len][data][chk] ────────│  返回响应帧
    │                                        │
    │  data_parse() 处理响应数据               │
    │                                        │
```

### 接收状态机

`data_recv(unsigned char c)` 逐字节解析：

```
STATE_RECV_FIX  ──收到 0x5A──→  STATE_RECV_ID
                                      │
                                 收到 msg_id < 8
                                      │
                                      ▼
STATE_RECV_LEN  ──收到长度──→  STATE_RECV_DATA
                                      │
                                 收到 length 字节
                                      │
                                      ▼
STATE_RECV_CHECK ──校验和匹配──→ 完成，调用 data_parse()
                  ──校验和失败──→ 丢弃，等待下一帧
```

---

## 波特率 921600 的选择

921600 是标准串口波特率中较高的值。选择原因：

- 主循环以 1000 Hz 运行，每轮需要发送 `ID_GET_ODOM` 并等待响应
- 一帧最短 4 字节（帧头+ID+长度+校验和），最长 4+255+1=260 字节
- 在 921600 baud 下，传输 1 字节约 10.9 μs，20 字节帧约 0.22 ms
- 150ms 的超时远大于通信延迟，但如果用 115200 baud，传输 64 字节参数需约 5.5ms，仍然可行

> [!TIP]
>
> 如果通信不稳定，可以尝试降低波特率。修改 `base_params.yaml` 中的 `buadrate` 参数即可，但要确保下位机固件也使用相同波特率。

---

## 源码参考

| 文件 | 路径 | 关键函数 |
|------|------|----------|
| 帧格式定义 | `src/abot_base/abot_bringup/include/abot_bringup/simple_dataframe.h` | `Message` 构造函数 |
| 协议实现 | `src/abot_base/abot_bringup/src/simple_dataframe_master.cpp` | `interact()`, `send_message()`, `data_recv()` |
| 传输层 | `src/abot_base/abot_bringup/src/serial_transport.cpp` | `init()`, `read()`, `write()` |
| 数据结构 | `src/abot_base/abot_bringup/include/abot_bringup/data_holder.h` | `Robot_parameter`, `Robot_velocity`, `Robot_odom` |
| 主循环 | `src/abot_base/abot_bringup/src/base_driver.cpp` | `work_loop()`, `update_odom()`, `update_speed()` |
