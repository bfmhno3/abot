# Mahony 互补滤波器

> 用 PI 控制器融合陀螺仪和加速度计

## 第一性原理：重力是最可靠的参考

当机器人静止时，加速度计测量到的就是重力方向。重力永远向下——这是地球表面最稳定的"绝对参考"。

Mahony 滤波器的核心思想是：

1. 用当前姿态估计，计算出"期望的重力方向"
2. 与加速度计实际测量的重力方向比较，得到**误差**
3. 用这个误差修正陀螺仪的角速度（P 项）和累积偏差（I 项）
4. 用修正后的角速度积分更新姿态

---

## 算法步骤详解

### 步骤 1：归一化加速度计

```cpp
// 源码：src/imu_filter/src/Mahony_filter.cpp
float norm = sqrt(ax*ax + ay*ay + az*az);
ax /= norm;  ay /= norm;  az /= norm;
```

归一化后，加速度计向量变成单位向量，只表示方向，不受大小影响。

### 步骤 2：计算期望重力方向

从当前四元数姿态估计中，计算期望的重力方向（即姿态估计认为"向下"的方向）：

```cpp
// 四元数 q = {q0, q1, q2, q3}
// 期望重力方向 = q × [0,0,0,1] × q*（四元数旋转）
halfvx = q1*q3 - q0*q2;
halfvy = q0*q1 + q2*q3;
halfvz = q0*q0 - 0.5f + q3*q3;
```

### 步骤 3：计算误差

叉积误差 = 测量重力方向 × 期望重力方向：

```cpp
halfex = ay * halfvz - az * halfvy;
halfey = az * halfvx - ax * halfvz;
halfez = ax * halfvy - ay * halfvx;
```

这个误差的物理含义：从期望方向到测量方向需要旋转的轴和角度。

### 步骤 4：积分项（消除陀螺仪零偏）

```cpp
if (twoKi > 0.0f) {
    ex_int += twoKi * halfex * (1.0f / sampleFreq);
    ey_int += twoKi * halfey * (1.0f / sampleFreq);
    ez_int += twoKi * halfez * (1.0f / sampleFreq);
    gx += ex_int;
    gy += ey_int;
    gz += ez_int;
}
```

积分项累积历史误差，即使加速度计暂时不可用，已累积的修正量仍能抵消陀螺仪零偏。

### 步骤 5：比例修正

```cpp
gx += twoKp * halfex;
gy += twoKp * halfey;
gz += twoKp * halfez;
```

将误差的 P 分量加到陀螺仪角速度上，立即修正当前测量。

### 步骤 6：四元数积分

```cpp
// 一阶龙格-库塔积分
q0 += (-q1*gx - q2*gy - q3*gz) * halfT;
q1 += ( q0*gx + q2*gz - q3*gy) * halfT;
q2 += ( q0*gy - q1*gz + q3*gx) * halfT;
q3 += ( q0*gz + q1*gy - q2*gx) * halfT;
```

其中 `halfT = 0.5 / sampleFreq`。

### 步骤 7：归一化四元数

```cpp
norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
q0 /= norm; q1 /= norm; q2 /= norm; q3 /= norm;
```

---

## 参数详解

### `twoKp` — 比例增益

- **默认值**：0.01
- **范围**：0 $\sim$ 0.1
- **含义**：控制加速度计修正的响应速度
- **调大**：更快地修正姿态误差，但对振动更敏感
- **调小**：更平滑，但修正缓慢，可能导致姿态滞后

> [!TIP]
>
> 典型取值范围：0.01~0.05。机器人运动平稳时可以取较大值；运动剧烈、振动大时应取较小值。

### `twoKi` — 积分增益

- **默认值**：0.002
- **范围**：0 $\sim$ 0.01
- **含义**：消除陀螺仪零偏的累积
- **调大**：更快消除零偏，但可能过冲
- **调小**：更稳定，但零偏消除慢
- **设为 0**：完全禁用积分项，退化为纯互补滤波

> [!NOTE]
>
> `twoKi` 的值通常远小于 `twoKp`。推荐比例：`twoKi` $\approx$ `twoKp` / 5 $\sim$ `twoKp` / 10。

### `sampleFreq` — 采样频率

- **默认值**：400 Hz
- **影响**：影响积分步长和滤波器的时间常数
- **必须与实际数据频率匹配**：如果 IMU 数据以 100Hz 发布，`sampleFreq` 应设为 100

---

## 动态调参方法

```bash
# 启动 rqt_reconfigure
rosrun rqt_reconfigure rqt_reconfigure

# 找到 /Mahony_filter 节点
# 调整 twoKp 和 twoKi
```

### 调参流程

1. 先将 `twoKi` 设为 0，只调 `twoKp`
2. 从 `twoKp=0.01` 开始，观察 `/ypr` 话题输出
3. 快速旋转机器人后静止，观察姿态恢复速度
4. 如果恢复太慢，增大 `twoKp`；如果震荡，减小 `twoKp`
5. 确定合适的 `twoKp` 后，加入 `twoKi`
6. 从 `twoKi=0.001` 开始，观察稳态漂移是否消除
7. 逐步增大 `twoKi` 直到漂移消失且无过冲

---

## 输出单位

Mahony 滤波器的 `/ypr` 话题输出**角度（度）**：

```cpp
// 源码中转换为度
yaw = atan2(2*q1*q2 - 2*q0*q3, 2*q0*q0 + 2*q1*q1 - 1) * 57.29578;
```

其中 57.29578 = 180 / $\pi$。

> [!WARNING]
>
> 这与 Madgwick 滤波器不同！Madgwick 输出弧度。如果在代码中混用两个滤波器的输出，注意单位转换。

---

## 源码参考

| 文件 | 路径 |
|------|------|
| 滤波器实现 | `src/imu_filter/src/Mahony_filter.cpp` |
| 动态参数定义 | `src/imu_filter/cfg/MyStuff.cfg` |
| README | `src/imu_filter/README.md` |
