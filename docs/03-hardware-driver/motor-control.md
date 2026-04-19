# 电机控制与 PID 调参

> 速度指令如何变成电机转动，以及如何调得更好

## 从 cmd_vel 到电机：数据流全景

```
move_base / mission_node
        │
        │ /cmd_vel (geometry_msgs/Twist)
        │ linear.x = 0.2 m/s, angular.z = 0.5 rad/s
        ▼
┌──────────────────────────────┐
│     cmd_vel_callback()       │  单位转换
│  v_liner_x = 0.2*100 = 20   │  m/s → cm/s
│  v_angular_z = 0.5*100 = 50 │  rad/s → 0.01 rad/s
│  need_update_speed = true    │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│      update_speed()          │  发送串口帧
│  frame->interact(ID_SET_VELOCITY)│
│  payload: Robot_velocity     │
│  [20, 0, 50] (6 bytes)      │
└──────────────┬───────────────┘
               │
               ▼ 串口 921600 baud
┌──────────────────────────────┐
│     电机控制器板（下位机）      │
│  接收速度指令                  │
│  PID 控制电机达到目标速度       │
│  读取编码器反馈                │
│  返回里程计数据               │
└──────────────┬───────────────┘
               │
               ▼ 串口响应
┌──────────────────────────────┐
│      update_odom()           │  发布里程计
│  Data_holder::odom           │
│  缩放 0.01 (cm→m, 0.01rad→rad)│
│  发布 `bdg.odom_topic` + 可选 TF │
└──────────────────────────────┘
```

---

## 速度指令处理细节

### `cmd_vel_callback()`

```cpp
// 源码：src/abot_base/abot_bringup/src/base_driver.cpp
void BaseDriver::cmd_vel_callback(const geometry_msgs::Twist& vel_cmd)
{
    // ROS 单位 → 串口协议单位
    Data_holder::get()->velocity.v_liner_x  = vel_cmd.linear.x  * 100;  // m/s → cm/s
    Data_holder::get()->velocity.v_liner_y  = vel_cmd.linear.y  * 100;  // m/s → cm/s
    Data_holder::get()->velocity.v_angular_z = vel_cmd.angular.z * 100;  // rad/s → 0.01 rad/s
    need_update_speed = true;
}
```

> [!NOTE]
>
> 乘以 100 的原因：串口协议中速度以 cm/s 和 0.01 rad/s 为单位，而 ROS 标准使用 m/s 和 rad/s。100 倍的缩放提供了 1 cm/s 和 0.01 rad/s 的分辨率。

### 速度限幅

下位机固件会根据 `max_v_liner_x`、`max_v_angular_z` 等参数对速度进行限幅。即使 ROS 端发送了更大的值，下位机也不会执行超过限制的速度。

这意味着调整 `max_v_liner_x` 是限制机器人最大速度的最直接方法。

---

## 主循环频率

```cpp
// 源码：base_driver.cpp work_loop()
while (ros::ok()) {
    update_param();    // 如果参数有变，发送新参数到下位机
    update_odom();     // 获取里程计 → 发布配置中的 odom 话题
    update_pid_debug();// 如果启用，发布 PID 调试数据
    update_speed();    // 如果有新速度指令，发送到下位机
    update_imu();      // 如果 imu_type=='E'，获取 IMU 数据
    ros::spinOnce();
}
```

主循环以约 1000 Hz 运行。每轮都要调用 `update_odom()`（请求→等待→解析），这意味着串口通信频率约 1000 Hz。在 921600 波特率下这是可行的，但任何延迟都会累积。

---

## 里程计参数：影响定位精度

### `wheel_diameter`（轮径）

**测量方法**：用卡尺测量轮胎外径，单位 mm。

**影响**：如果轮径设为 97mm 但实际是 100mm，那么：
- 机器人以为自己走了 97cm，实际走了 100cm
- 里程计将有 3% 的误差，长距离会累积

> [!TIP]
>
> 用"走直线测试"验证：让机器人走 1 米，检查里程计话题（默认常见配置是 `/wheel_odom`）报告的距离是否为 1.00m。如果不准，微调 wheel_diameter。

### `wheel_track`（轮距）

**测量方法**：左右轮中心线之间的距离，单位 mm。

**影响**：轮距影响旋转里程计。如果轮距不准，机器人转弯时的角度变化会计算错误。

### `encoder_resolution`（编码器分辨率）

**含义**：电机转动一圈产生的脉冲数（含 4 倍频）。

**常见值**：

- 11 线编码器 × 4 倍频 = 44（小型电机）
- 390 线编码器 × 4 倍频 = 1560（精密电机）
- 990 线编码器 × 4 倍频 = 3960（高精度电机）

如果不确定，可以手动转动轮子一圈，观察里程计话题（默认常见配置是 `/wheel_odom`）中的变化量。

---

## PID 参数详解

> [!IMPORTANT]
>
> 这里的 PID 是**下位机（电机控制器板）上的 PID**，不是 ROS 层的。它控制的是电机转速，不是机器人位姿。

### `kp` — 比例增益

- **作用**：速度误差 × kp = 控制输出。误差越大，修正力越强
- **太小**：电机响应迟缓，达不到目标速度
- **太大**：电机抖动、过冲
- **默认值**：320（`src/abot_base/abot_bringup/include/abot_bringup/data_holder.h`）

### `ki` — 积分增益

- **作用**：累积误差 × ki = 额外修正。消除稳态误差（例如上坡时速度持续偏低）
- **太小**：无法消除稳态偏差
- **太大**：积分饱和，响应迟缓且振荡
- **默认值**：2700（`src/abot_base/abot_bringup/include/abot_bringup/data_holder.h`）

### `kd` — 微分增益

- **作用**：误差变化率 × kd = 阻尼。抑制超调
- **太小**：超调严重
- **太大**：对噪声敏感，电机抖动
- **默认值**：0（未使用微分项）

### `ko` — 输出缩放因子

- **作用**：PID 输出 ÷ ko = PWM 占空比。控制输出信号的量程
- **太小**：输出信号过大，电机满载
- **太大**：输出信号过小，电机无力
- **默认值**：10

### `do_pid_interval` — PID 控制周期

- **含义**：下位机每 do_pid_interval 毫秒执行一次 PID 计算
- **默认值**：10ms（100 Hz）
- **影响**：太小则计算负担重，太大则响应不及时

---

## `cmd_last_time` — 命令超时保护

- **含义**：下位机收到最后一个速度指令后，超过 cmd_last_time 毫秒未收到新指令，则自动停车
- **默认值**：250ms
- **作用**：防止 ROS 崩溃或通信中断时机器人失控
- **调整建议**：
  - 太短（< 100ms）：可能导致 move_base 正常控制时意外停车
  - 太长（> 1000ms）：安全隐患增大

---

## `dynamic_reconfigure` 在线调参

```bash
# 1. 启动 rqt_reconfigure
rosrun rqt_reconfigure rqt_reconfigure

# 2. 在左侧树中找到 /abot_driver

# 3. 调整参数，立即生效

# 4. 或使用命令行
rosrun dynamic_reconfigure dynparam set /abot_driver kp 200
rosrun dynamic_reconfigure dynparam set /abot_driver max_v_liner_x 60
```

> [!WARNING]
>
> dynamic_reconfigure 修改会立即写回当前进程内的 `Robot_parameter`，并通过 `ID_SET_ROBOT_PARAMTER` 下发到下位机；但节点重启后，`read_param()` 又会把一组硬编码默认值写回内存，因此这些修改不会可靠持久化。当前仓库里仍然存在这个覆盖行为。

---

## PID 调参指南

### 步骤 1：确认机械参数

用卡尺测量并设置正确的 `wheel_diameter`、`wheel_track`、`encoder_resolution`。

### 步骤 2：设置 `ki=0`, `kd=0`，调 `kp`

1. 将 `ki` 和 `kd` 设为 0
2. 从小 `kp` 开始（例如 50）
3. 发送恒定速度指令：`rostopic pub /cmd_vel geometry_msgs/Twist "linear: {x: 0.1}"`
4. 观察电机是否平稳转动
5. 逐步增大 `kp` 直到电机开始抖动
6. 回退到抖动前值的 70%

### 步骤 3：加入 `ki` 消除稳态误差

1. 保持 `kp` 不变
2. 从小 `ki` 开始（例如 100）
3. 观察电机是否能在负载变化时保持目标速度
4. 逐步增大 `ki` 直到响应变得迟缓
5. 回退到迟缓前值的 50%

### 步骤 4：按需加入 `kd`

1. 如果有明显超调，加入小量 `kd`（例如 5-20）
2. kd 对噪声敏感，不宜过大

### 步骤 5：验证

1. 发送速度阶跃指令，观察响应曲线
2. 如果启用了 `out_pid_debug_enable`，可以订阅 PID 调试话题查看实时数据

---

## 发现的问题：read_param() 硬编码覆盖

```cpp
// 源码：base_driver.cpp read_param()
void BaseDriver::read_param()
{
    frame->interact(ID_GET_ROBOT_PARAMTER);  // 从下位机读取参数

    // 然后立刻用硬编码值覆盖！
    Data_holder::get()->parameter.wheel_diameter = 97;
    Data_holder::get()->parameter.wheel_track = 225;
    Data_holder::get()->parameter.encoder_resolution = 3960;
    Data_holder::get()->parameter.kp = 320;
    Data_holder::get()->parameter.ki = 2700;
    Data_holder::get()->parameter.kd = 0;
    Data_holder::get()->parameter.ko = 10;
    // ...
}
```

> [!CAUTION]
>
> 这段代码先从下位机读取参数，然后立即用硬编码值覆盖。这意味着：
>
> 1. 下位机中存储的参数被忽略
> 2. 通过 `dynamic_reconfigure` 修改的参数在节点重启后会被覆盖
> 3. 要永久修改参数，必须同时修改此处源码并重新编译

**建议修复**：删除硬编码覆盖，改为从 ROS 参数服务器读取默认值。

---

## 源码参考

| 文件 | 关键函数 |
|------|----------|
| `src/abot_base/abot_bringup/src/base_driver.cpp` | `cmd_vel_callback()`, `work_loop()`, `update_odom()`, `update_speed()`, `read_param()` |
| `src/abot_base/abot_bringup/src/base_driver_config.cpp` | `init()`, `dynamic_callback()` |
| `src/abot_base/abot_bringup/include/abot_bringup/data_holder.h` | `Robot_parameter`, `Robot_velocity`, `Robot_odom` |
| `src/abot_base/abot_bringup/CMakeLists.txt` | dynamic_reconfigure 配置入口（生成 `abot_driverConfig`） |
| `src/abot_base/abot_bringup/params/base_params.yaml` | 串口与话题基础参数 |
