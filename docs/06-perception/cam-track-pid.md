# 视觉追踪 PID 控制

> 视觉控制的本质不是“会看”，而是“看见后能稳定地动”

## 第一性原理

感知层给出的目标信息，本身不会让机器人动起来。

机器人真正运动，需要把“目标偏差”转成“速度命令”。

这个转换过程，就是视觉闭环控制。

在 abot 的 `cam_track` 与 `mission_node` 里，核心思想都很接近：

1. 得到目标偏差
2. 用比例项把偏差映射成角速度
3. 对速度做限幅
4. 当偏差足够小，就停止转动并执行下一步动作（例如射击）

本质上，它们都在做这件事：

> “偏得越多，转得越快；偏差足够小时，就认为已经对准。”

---

## 为什么这里几乎只有 P，没有真正 PID

虽然参数名写的是 `PID_Control_P`，但实际代码里基本只用了比例项 P。

### `mission_node`

```cpp
vel_x = -PID_Control_P * offset_x;
```

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:277`

### `cam_track`

```cpp
yaw_vel = satfunc(PID_Control_P * dyaw, Max_yaw_vel);
```

位置：

- `src/cam_track/include/cam_track/cam_track_node.hpp:214-215`

这意味着：

- 没有 I：不会主动消除长期残差
- 没有 D：不会显式抑制变化率
- 只有 P：最简单、最直接、最容易现场调

为什么比赛里常这样做？

因为：

- 上层目标通常很简单：把目标转到中心
- 比赛时间很紧
- 简单 P 控制调得更快
- 前提是视觉数据足够稳定

---

## `cam_track` 的控制逻辑

源码：

- `src/cam_track/include/cam_track/cam_track_node.hpp`

### 输入来源

- `/tf`：读取 `ar_marker_i` 的位置
- `/odom`：读取当前角速度 `angle_vel`
- `/move_base/status`：判断是否到达导航点

对应位置：

- `src/cam_track/include/cam_track/cam_track_node.hpp:34-37`

### 控制变量

关键参数来自 `PID.yaml`：

```yaml
PID_Control_P : 0.5
Max_yaw_vel : 0.2
yaw_th : 0.02
vel_th : 0.0008
```

含义：

| 参数 | 含义 |
|------|------|
| `PID_Control_P` | 偏差到角速度的比例增益 |
| `Max_yaw_vel` | 最大允许角速度 |
| `yaw_th` | 允许的朝向误差阈值 |
| `vel_th` | 认为机器人已经基本停稳的角速度阈值 |

### 闭环过程

1. `calaryaw(i)` 计算机器人指向标签的目标方向
2. `dyaw = des_yaw - base_yaw`
3. 若 `abs(dyaw) > yaw_th`：继续转
4. 若 `abs(dyaw) <= yaw_th`：停止
5. 若此时 `abs(angle_vel) < vel_th`：发射击命令

关键代码：

- `src/cam_track/include/cam_track/cam_track_node.hpp:179-199`
- `src/cam_track/include/cam_track/cam_track_node.hpp:212-248`

### 为什么还要判断 `vel_th`

这是一个很实用的小技巧。

如果只判断 `dyaw` 足够小，就立刻射击，可能会出现：

- 实际车体还在惯性旋转
- 视觉上刚好经过目标中心，但底盘没真正停稳
- 射击时机偏早

所以它又加了一个条件：

> 只有当角速度也足够小，才算真正完成对准。

这使得控制逻辑更像“稳住后再开火”，而不是“刚擦到目标就开火”。

---

## `mission_node` 的控制逻辑

源码：

- `src/user_demo/include/user_demo/mission_node.hpp`

### 输入来源

- `/ar_pose_marker`：读取标签位姿估计
- `/move_base/result`：判断导航到点
- `/abot/pose`：读取当前位置与朝向

对应位置：

- `src/user_demo/include/user_demo/mission_node.hpp:52-60`

### 关键参数

来自 `mission.yaml`：

```yaml
Yaw_th: 0.035
Track_max_vel_x: 0.6
Track_thres_vel_x: 0.3
```

含义：

| 参数 | 含义 |
|------|------|
| `Yaw_th` | 允许的横向误差阈值 |
| `Track_max_vel_x` | 对准时最大角速度幅值 |
| `Track_thres_vel_x` | 小于该值时直接认为不必动 |

### 饱和函数的作用

`mission_node` 里定义了：

```cpp
satfunc(vel_x, track_max_vel_x, track_thres_vel_x);
```

定义位置：

- `src/user_demo/include/user_demo/mission_node.hpp:397-411`

这个函数本身的设计意图很清楚：

1. **死区处理**
   - 偏差太小时直接输出 0
2. **限幅处理**
   - 偏差太大时也不允许无限增大速度

但当前实现里有一个关键问题：

> [!WARNING]
> `satfunc()` 的返回值没有重新赋回 `vel_x`，所以这套死区/限幅逻辑在当前代码中并没有真正生效。

对应位置：

- `src/user_demo/include/user_demo/mission_node.hpp:277-279`

所以更准确的说法应该是：

- 代码**想要**通过 `satfunc()` 做视觉控制整形
- 但当前版本并没有真正把这个整形结果用起来

这也是很典型的竞赛代码现象：

> 设计思路是对的，但最后一行接线没接上。

因此如果你现场看到 `mission_node` 的控制表现并不像“有死区、有饱和”，不要惊讶——按当前源码，它确实可能没有体现出文档里常见的那种整形效果。

---

## 这两个控制器的差别

| 维度 | `cam_track` | `mission_node` |
|------|-----------|--------------|
| 偏差来源 | 空间角度 `dyaw` | 标签位姿估计中的横向位移 `offset_x` |
| 数据来源 | TF | `/ar_pose_marker` |
| 停止条件 | `abs(dyaw) <= yaw_th` 且角速度足够小 | `offset_x` 落入阈值区间 |
| 恢复策略 | TF 不可用时返回 | `shootToar()` 里写了恢复旋转分支，但 `marker_found` 当前未可靠清零 |
| 应用风格 | 更像几何指向 | 更像比赛业务控制 |

所以你不能简单说哪个 “更高级”。

更准确地说：

- `cam_track` 更像空间控制版本
- `mission_node` 更像比赛业务版本

---

## 参数怎么影响实际表现

### `PID_Control_P` 太小

现象：

- 对准很慢
- 看起来反应迟钝
- 容易错过节奏

### `PID_Control_P` 太大

现象：

- 快速左右摆
- 过冲明显
- 靠近目标时不稳

### `Max_yaw_vel` / `Track_max_vel_x` 太小

现象：

- 大偏差时也转不过来
- 恢复慢
- 比赛耗时长

### `Max_yaw_vel` / `Track_max_vel_x` 太大

现象：

- 转得猛
- 容易冲过头
- 需要更小的 P 才能稳

### `yaw_th` / `Yaw_th` 太小

现象：

- 要求过于苛刻
- 一直修正
- 明明已经差不多了还不开火

### `yaw_th` / `Yaw_th` 太大

现象：

- 很快就判定对准成功
- 但精度差
- 容易打偏

### `vel_th` 太大

现象：

- 车还没完全停稳就触发射击

### `vel_th` 太小

现象：

- 永远等不到“完全静止”
- 控制流程拖沓

---

## 比赛里最实用的调参顺序

### `cam_track`

建议顺序：

1. 先看 TF 是否稳定
2. 再调 `PID_Control_P`
3. 再调 `Max_yaw_vel`
4. 最后调 `yaw_th` 和 `vel_th`

为什么？

因为如果 TF 本身就在跳，后面怎么调都只是掩盖症状。

### `mission_node`

建议顺序：

1. 先看 `/ar_pose_marker` 是否稳定
2. 调 `PID_Control_P`
3. 调 `Track_max_vel_x`
4. 调 `Track_thres_vel_x`
5. 最后调 `Yaw_th`

为什么？

因为 `Track_thres_vel_x` 实际上决定了“死区多大”，它对抖动和不开火的影响很大。

---

## 常见失败现象与原因

### 现象 1：对准时左右来回抖

优先怀疑：

- `PID_Control_P` 太大
- 死区太小
- 视觉输入抖动

### 现象 2：明明目标在前面，但车反应很慢

优先怀疑：

- `PID_Control_P` 太小
- 最大角速度限制太小

### 现象 3：看起来已经对准了，却一直不开火

优先怀疑：

- `yaw_th` 太小
- `vel_th` 太小
- 视觉结果在阈值边缘抖动

### 现象 4：一丢标签就卡住或行为奇怪

优先怀疑：

- 丢失目标后的恢复逻辑过弱
- TF/话题刷新不稳
- 当前目标 ID 配置错了

---

## 一个很值得记住的工程事实

视觉闭环最怕的不是 “控制器不好”，而是：

> 输入量本身就不稳定。

如果：

- `offset_x` 一会儿大一会儿小
- `dyaw` 因 TF 跳变而忽左忽右

那么上层 P 控制一定会显得 “很笨”。

所以真正正确的调试顺序是：

1. 先验证视觉输入稳不稳
2. 再验证阈值合理不合理
3. 最后才调 P

这和很多人直觉相反，但对比赛非常重要。

---

## 额外提醒：当前仓库存在射击命令不统一问题

> [!WARNING]
> 即便视觉控制已经收敛，最终动作仍可能因为接口不一致而失败。

当前仓库里：

- `cam_track` 发 `"shoot!"`
- `mission_node` 发 `"1"`
- 执行层期望 `"shoot"`

所以如果你以后在赛前联调时发现：

- 对准没问题
- 视觉也没问题
- 但就是没发射

不要只盯 PID，也要检查最后一级控制协议是否一致。
