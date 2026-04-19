# 里程计与 EKF 融合

> 轮子告诉我走了多远，IMU 告诉我朝向哪——合在一起更准确

## 第一性原理：为什么需要传感器融合？

任何单一传感器都有局限性：

| 传感器 | 能测量 | 弱点 |
|--------|--------|------|
| 轮式编码器 | 平移距离、平移速度 | 打滑时完全失效；航向角随距离累积漂移 |
| IMU 陀螺仪 | 角速度、短期航向 | 零偏漂移随时间累积 |
| IMU 加速度计 | 重力方向（绝对朝向参考） | 受振动影响大，无法独立测量位置 |

**融合的直觉**：轮速计擅长测位置但航向会漂，IMU 擅长测航向但位置会漂。把它们结合起来，互补弱点，得到比任何一个都好的估计。

---

## 轮式里程计原理

### 工作流程

```
电机编码器 → 脉冲计数 → 速度/位移 → Robot_odom 结构体
                                            │
                                            ▼ 串口返回
                                    abot_driver update_odom()
                                            │
                                            ▼ 缩放 ×0.01
                                  `bdg.odom_topic` (Odometry)
                                  `publish_tf=true` 时额外广播 TF
```

### 编码器到里程的转换

下位机固件根据以下参数将编码器脉冲转换为物理量：

- `wheel_diameter`：轮径，决定每个脉冲对应的线位移
- `encoder_resolution`：编码器分辨率，一圈的脉冲数
- `wheel_track`：轮距，影响旋转里程的计算

一个脉冲对应的位移 = $\pi$ $\times$ `wheel_diameter` / `encoder_resolution`

### 里程计漂移的原因

1. **轮子打滑**：光滑地面、加速过猛时轮子空转，编码器以为在走但实际没有
2. **轮径误差**：`wheel_diameter` 参数不准确，每步都有比例误差
3. **轮距误差**：`wheel_track` 不准，转弯角度计算错误
4. **地面不平**：越过障碍物时编码器无法区分水平和垂直位移

> [!TIP]
>
> 里程计漂移是**累积性的**——走越远，误差越大。这就是为什么需要 SLAM/AMCL 来修正全局位置。

---

## `publish_tf` 参数的作用

`src/abot_base/abot_bringup/params/base_params.yaml` 和 `src/abot_base/abot_bringup/params/base_params_with_imu.yaml` 之间的关键区别：

| 参数 | base_params.yaml | base_params_with_imu.yaml |
|------|------------------|---------------------------|
| `publish_tf` | **true** | **false** |
| `odom_topic` | `wheel_odom` | `wheel_odom` |

### 为什么带 IMU 的配置反而 `publish_tf=false`？

因为这个项目里，是否发布 `odom → base_link` TF 不是由 “有没有 IMU” 决定，而是由**谁来承担 TF 发布职责**决定：

1. **纯底盘里程计模式**：`abot_driver` 自己发布 `/wheel_odom`，同时发布 `odom → base_link` TF（`publish_tf: true`）
2. **带 IMU + EKF 模式**：`abot_driver` 仍然发布 `/wheel_odom` 供 EKF 使用，但不再自己发布 TF（`publish_tf: false`）
3. `robot_pose_ekf `订阅轮式里程计和 IMU，输出融合后的位姿，再由后续节点转换成更适合可视化的格式

> [!NOTE]
>
> 文档里最容易误解的一点是：`publish_tf` 只控制 **`abot_driver` 是否直接广播 TF**，并不影响它继续发布里程计话题。IMU + EKF 模式下，底盘里程计仍然是融合输入，只是 TF 发布职责交给了融合链路。

## robot_pose_ekf 融合策略

### 输入

| 话题 | 消息类型 | 传感器 | 提供信息 |
|------|----------|--------|----------|
| `/wheel_odom` | `nav_msgs/Odometry` | 轮式编码器 | 位置 `(x,y)`、线速度、航向角 |
| `/imu/data` | `sensor_msgs/Imu` | IMU | 姿态（四元数）、角速度 |

### 输出

| 话题 | 消息类型 | 内容 |
|------|----------|------|
| `/robot_pose_ekf/odom` | `geometry_msgs/PoseWithCovarianceStamped` | 融合后的位姿+协方差 |

### 融合算法

`robot_pose_ekf` 使用**扩展卡尔曼滤波（EKF）**：

1. **预测步**：根据运动模型（轮速计预测下一时刻位姿）
2. **更新步**：用 IMU 姿态测量修正预测值
3. **输出**：融合后的最优估计

EKF 的优势在于：它不仅给出最优估计，还给出**协方差**（不确定性度量）。当某个传感器不可靠时，EKF 会自动降低其权重。

---

## `odom_ekf.py` 桥接节点

### 为什么需要这个节点？

`robot_pose_ekf` 输出的是 `geometry_msgs/PoseWithCovarianceStamped`，但 RViz 和不少旧 ROS 工具更习惯直接消费 `nav_msgs/Odometry`。因此仓库里放了一个很小的桥接脚本，把输入话题重新包装成 Odometry。

`odom_ekf.py` 的逻辑本身不绑定固定话题名，它使用的是：

- 订阅：`input`
- 发布：`output`

实际接到什么话题、发到什么话题，要看 launch 文件里的 `remap` 配置。

```text
input (PoseWithCovarianceStamped)
        │
        ▼ odom_ekf.py
output (nav_msgs/Odometry)
```

### 源码

```python
# 源码：src/abot_base/abot_bringup/scripts/odom_ekf.py
def callback(msg):
    odom = Odometry()
    odom.header = msg.header
    odom.header.frame_id = "/odom"
    odom.child_frame_id = "base_link"
    odom.pose = msg.pose
    # twist 部分不填充（EKF 不提供速度估计）
    pub.publish(odom)
```

> [!NOTE]
>
> 这个桥接节点只转换位姿部分，不包含速度信息。如果下游节点需要速度信息，应直接使用 abot_driver 发布的轮式里程计话题（默认是 `/wheel_odom`，具体取决于 `odom_topic` 配置）。

---

## 里程计漂移的应对策略

### 短期（几秒内）

- 确保 IMU 校准正确，EKF 正常融合
- 降低加速度（减少轮子打滑概率）
- 确保 `wheel_diameter` 和 `wheel_track` 参数精确

### 中期（几十秒内）

- AMCL 定位会修正全局漂移（需要先建好地图）
- 定期检查 AMCL 粒子分布是否集中

### 长期（分钟级）

- 完全依赖 AMCL/SLAM 修正
- 如果地图质量好，AMCL 可以将定位误差控制在厘米级

---

## 源码参考

| 文件 | 路径 | 关键函数 |
|------|------|----------|
| 主驱动 | `src/abot_base/abot_bringup/src/base_driver.cpp` | `update_odom()` |
| 参数配置 | `src/abot_base/abot_bringup/params/base_params.yaml` / `src/abot_base/abot_bringup/params/base_params_with_imu.yaml` | `publish_tf`, `odom_topic` |
| EKF 桥接 | `src/abot_base/abot_bringup/scripts/odom_ekf.py` | `callback()` |
| 数据结构 | `src/abot_base/abot_bringup/include/abot_bringup/data_holder.h` | `Robot_odom` |
