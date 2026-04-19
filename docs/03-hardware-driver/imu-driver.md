# IMU 驱动与校准

> 从原始传感器数据到可用姿态信息的旅程

## 为什么需要 IMU？

轮式里程计有一个致命弱点：**它只能测量位移，无法感知朝向的绝对准确性**。轮子打滑、地面不平等都会导致航向角漂移。IMU（惯性测量单元）中的陀螺仪可以提供高频的角速度测量，弥补这一缺陷。

`abot` 使用的是 ICM 20602 六轴 IMU（加速度计 + 陀螺仪），部分型号还集成了磁力计。

---

## 数据流

```
ICM 20602 硬件
    │ 9 轴原始数据（加速度+角速度+磁场）
    ▼
abot_driver (imu_type=='E' 时转发)
    │ /raw_imu (abot_imu/RawImu)
    ▼
abot_imu 节点
    │ 校准（去偏置、缩放）
    │ /imu/data_raw (sensor_msgs/Imu)  ──→ Mahony/Madgwick filter
    │ /imu/mag（默认是 geometry_msgs/Vector3Stamped；启用 `imu/use_mag_msg` 后才是 sensor_msgs/MagneticField）
    ▼
robot_pose_ekf (EKF 融合)
```

---

## `RawImu` 自定义消息

```
# 源码：abot_imu/msg/RawImu.msg
Header header
bool accelerometer          # 加速度计是否可用
bool gyroscope              # 陀螺仪是否可用
bool magnetometer            # 磁力计是否可用
geometry_msgs/Vector3 raw_linear_acceleration    # 原始加速度 (m/s²)
geometry_msgs/Vector3 raw_angular_velocity       # 原始角速度 (rad/s)
geometry_msgs/Vector3 raw_magnetic_field         # 原始磁场 (特斯拉)
```

三个 `bool` 字段表示对应传感器是否可用。如果某传感器故障或未连接，对应字段为 `false`。

---

## `abot_imu` 节点的校准流程

### 加速度计和陀螺仪校准

**前提**：机器人必须**静止放置在水平面上**。

1. 节点启动时，如果 `imu/perform_calibration` 为 `true` 或参数服务器中没有已保存的偏置值，则开始校准
2. 采集 `imu/calibration_samples`（默认 500）个样本
3. 计算每个轴的均值作为偏置（bias）
4. **加速度计 Z 轴**特殊处理：把静止时测到的重力分量折算进 bias（源码里是 `acceleration_bias_["z"] = mean_z + GRAVITY`），这样后续发布 `linear_acceleration` 时减去 bias 后，静止状态下更接近 0
5. 将偏置值保存到参数服务器，下次启动时自动加载

```python
# 校准逻辑的伪代码
for i in range(calibration_samples):
    accumulate(raw_accel, raw_gyro)
accel_bias = mean(accumulated_accel)
gyro_bias = mean(accumulated_gyro)
accel_bias.z += GRAVITY  # 让后续 raw_z - bias_z ≈ 0（静止时）
```

### 磁力计校准（硬铁偏移）

磁力计受电机、金属框架等影响，存在硬铁偏移。校准使用 min/max 方法：

```python
# 硬铁偏移 = (max + min) / 2
offset_x = (mag_x_max + mag_x_min) / 2
offset_y = (mag_y_max + mag_y_min) / 2
offset_z = (mag_z_max + mag_z_min) / 2

# 校准后
corrected_x = raw_x - offset_x
```

**校准操作**：在参数服务器中设置 `mag/x/min`、`mag/x/max` 等值。这些值需要通过手动旋转机器人来获取：

1. 启动 IMU 节点
2. 缓慢旋转机器人，使其在所有方向上转动
3. 记录各轴的最大值和最小值
4. 将 `min` / `max` 值写入 `YAML` 配置文件

---

## 关键参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `imu/use_accelerometer` | `true` | 是否使用加速度计 |
| `imu/use_gyroscope` | `true` | 是否使用陀螺仪 |
| `imu/use_magnetometer` | `true` | 是否使用磁力计 |
| `imu/perform_calibration` | — | 是否在启动时执行校准 |
| `imu/calibration_samples` | `500` | 校准采样数量 |
| `imu/accelerometer_bias` | `{x:0, y:0, z:0}` | 加速度计偏置 |
| `imu/gyroscope_bias` | `{x:0, y:0, z:0}` | 陀螺仪偏置 |
| `imu/linear_acc_stdev` | — | 加速度计标准差（影响协方差矩阵） |
| `imu/angular_vel_stdev` | — | 陀螺仪标准差 |
| `imu/magnetic_field_stdev` | — | 磁力计标准差 |
| `mag/x/min`, `mag/x/max` 等 | — | 磁力计硬铁校准参数 |

### 标准差参数的影响

标准差用于构建 IMU 消息的协方差矩阵。EKF 滤波器会使用这些协方差值来权衡不同传感器的信任程度：

- **标准差大** → EKF 认为该传感器不可靠，降低其权重
- **标准差小** → EKF 高度信任该传感器

> [!TIP]
>
> 如果发现 EKF 融合后姿态角震荡，可以尝试增大 IMU 的标准差参数，让 EKF 更信任轮式里程计。

---

## `imu_type` 条件分支

在 `base_driver.cpp` 中：

```cpp
void BaseDriver::update_imu()
{
    if (Data_holder::get()->parameter.imu_type == 'E') {  // ASCII 69
        frame->interact(ID_GET_IMU_DATA);
        // 发布 /raw_imu
    }
}
```

`imu_type == 'E'` 表示使用外部 IMU（External）。当此条件为真时，`abot_driver` 额外通过串口请求 IMU 原始数据并转发到 `/raw_imu` 话题。

如果不使用 IMU，将 `imu_type` 设为 0 即可禁用此功能。

---

### `/imu/mag` 的消息类型

这里有一个容易忽略的实现细节：`abot_imu` 默认把 `/imu/mag` 发布为 `geometry_msgs/Vector3Stamped`。只有把参数 `imu/use_mag_msg` 设为 `true`，才会改为发布 `sensor_msgs/MagneticField`。

这意味着如果你后续接入的滤波器或可视化工具明确要求 `sensor_msgs/MagneticField`，需要先确认 `launch` 或 `YAML` 里是否打开了 `imu/use_mag_msg`。

---

## IMU 校准服务

```
话题：/imu/calibrate_imu
类型：std_srvs/Empty
```

在运行时可以通过调用此服务触发重新校准：

```bash
rosservice call /imu/calibrate_imu
```

> [!WARNING]
>
> 调用校准服务前，必须确保机器人静止。如果在运动中校准，偏置值会错误，导致姿态角持续漂移。

---

## 源码参考

| 文件 | 路径 |
|------|------|
| IMU 驱动节点 | `src/abot_base/abot_imu/src/abot_imu.cpp` |
| IMU 驱动头文件 | `src/abot_base/abot_imu/include/abot_imu/abot_imu.h` |
| RawImu 消息定义 | `src/abot_base/abot_imu/msg/RawImu.msg` |
| IMU 启动文件 | `src/abot_base/abot_imu/launch/imu_ahrs.launch` |
| 磁力计校准脚本 | `src/abot_base/abot_imu/scripts/simple_magnometer_calibration` |
