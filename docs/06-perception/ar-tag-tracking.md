# AR 标签检测与追踪

> 不是 “看见一个二维码”，而是 “把一个视觉标记变成可用于控制的空间基准”

## 第一性原理

AR 标签（这里主要是 `ar_track_alvar`）的核心价值不是识别图案本身，而是：

- 知道 **标签是谁**（ID）
- 知道 **标签在哪里**（位置）
- 知道 **标签朝向如何**（姿态）

一旦这三件事稳定，机器人就可以据此做：

- 对准
- 接近
- 射击
- 区域判断
- 任务切换

这就是为什么 AR 标签特别适合比赛：

> 它不只是 “看见目标”，而是天然提供一个可计算的坐标参考。

---

## 在 abot 里，AR 结果有两种消费方式

这是仓库里最重要的结构点之一。

### 路线 A：话题方式

`mission_node` 订阅：

- `/ar_pose_marker`

源码位置：

- `src/user_demo/include/user_demo/mission_node.hpp:53`
- `src/user_demo/include/user_demo/mission_node.hpp:261`

在 `tag_cb()` 里，它会：

1. 遍历当前帧检测到的所有标签
2. 找到 `id == current_target_id` 的那个标签
3. 读取 `marker.pose.pose.position.x`
4. 计算：

```cpp
vel_x = -PID_Control_P * offset_x;
```

也就是说，`mission_node` 并不关心完整 3D 姿态，它更关心：

> 当前目标标签位姿估计中的横向位移有多大。

这套逻辑更像 “业务型视觉”：

- 我要打哪个标签？
- 当前横向偏差是否足够小？
- 可以不可以发射？

### 路线 B：TF 方式

`cam_track` 监听：

- `/tf`
- `map -> ar_marker_0/1/2/3`
- `map -> base`

源码位置：

- `src/cam_track/include/cam_track/cam_track_node.hpp:35`
- `src/cam_track/include/cam_track/cam_track_node.hpp:87`
- `src/cam_track/include/cam_track/cam_track_node.hpp:158`

它的做法不是直接读标签消息，而是：

1. 在 TF 里拿到标签在地图中的位置
2. 在 TF 里拿到机器人底盘在地图中的位置
3. 计算机器人朝向目标标签所需的目标 yaw
4. 比较目标 yaw 与当前 `base_yaw`
5. 输出转向速度

所以 `cam_track` 更像“空间型视觉”：

> 我不关心标签消息长什么样，我只关心目标在空间里到底在哪。

---

## mission_node 里的 AR 对准逻辑

### 关键参数

来自 `src/user_demo/param/mission.yaml`：

```yaml
Yaw_th: 0.035
Ar_0_id: 5
Ar_1_id: 0
Ar_2_id: 5
Track_max_vel_x: 0.6
Track_thres_vel_x: 0.3
```

对应代码读取位置：

- `src/user_demo/include/user_demo/mission_node.hpp:31-50`

### 工作机制

当机器人已经导航到 A/B/C 之类的射击点后：

1. `status_cb()` 检测 `/move_base/result` 是否到达成功
2. `execCallback()` 进入射击状态
3. `shootToar(i)` 选择当前应对准的 AR 标签 ID
4. 若 `marker_found == true`：
   - 用 `offset_x` 算角速度
   - 发布到 `/cmd_vel`
5. 当偏差进入阈值区间：
   - `offset_x` 落入 `[-yaw_th, yaw_th]`
   - 等待约 `0.8~0.9s`
   - 连发两次射击命令

关键逻辑位置：

- `src/user_demo/include/user_demo/mission_node.hpp:112`
- `src/user_demo/include/user_demo/mission_node.hpp:127`
- `src/user_demo/include/user_demo/mission_node.hpp:166`
- `src/user_demo/include/user_demo/mission_node.hpp:200`

### P 控制本质

这里虽然参数名写着 PID，但代码实际只用了 P 项：

```cpp
vel_x = -PID_Control_P * offset_x;
```

这意味着：

- 偏差大 → 转得快
- 偏差小 → 转得慢
- 没有积分消除长期静差
- 没有微分抑制快速变化

优点：

- 实现简单
- 调试快
- 比赛中很常见

缺点：

- 对噪声敏感
- 容易在阈值附近抖
- 若识别不稳，会出现忽左忽右

---

## `cam_track` 里的 AR 追踪逻辑

### 关键参数

来自 `src/cam_track/param/PID.yaml`：

```yaml
PID_Control_P : 0.5
Max_yaw_vel : 0.2
yaw_th : 0.02
A_x : 1.5
A_y : -0.53
B_x : 1.5
B_y : -2.2
vel_th : 0.0008
```

读取位置：

- `src/cam_track/include/cam_track/cam_track_node.hpp:25-32`

### 控制流程

`cam_track` 的逻辑是：

1. 先发布导航目标点 A
2. 到点后，对准 `ar_marker_0`
3. 射击后，再发布导航目标点 B
4. 到点后，依次对准 `ar_marker_1/2/3`

它通过 `calaryaw()` 计算“机器人当前位置指向标签位置”的目标方位角：

```cpp
yaw = atan2(ar_pos[1]-base_pos[1], ar_pos[0]-base_pos[0]);
```

然后和 `base_yaw` 相减，得到 `dyaw`，再通过：

```cpp
yaw_vel = satfunc(PID_Control_P * dyaw, Max_yaw_vel);
```

生成角速度。

### 这和 mission_node 最大的区别是什么？

- `mission_node`：基于标签位姿估计中的横向位移
- `cam_track`：基于 TF 空间位置的角度偏差

你可以把它们理解为：

- `mission_node` 更像 “基于标签位姿偏差的业务控制”
- `cam_track` 更像 “空间指向控制”

---

## 话题与 TF 的优缺点对比

| 方式 | 优点 | 缺点 |
|------|------|------|
| `/ar_pose_marker` | 直接、容易写业务逻辑、结构清楚 | 更依赖具体消息格式，不够通用 |
| `ar_marker_*` TF | 更自然表达空间关系，适合几何控制 | 更依赖 TF 树完整性与时间同步 |

如果你是为比赛快速写逻辑：

- 用消息更快

如果你是想做更统一的空间控制：

- 用 TF 更自然

---

## 丢失标签时系统怎么处理

### mission_node

`shootToar()` 里确实写了一个 “未找到标签时的恢复旋转” 分支：

- 会打印 `marker not found`
- 如果 `abs(yaw_cur) > 0.5`，就以 `-yaw_cur * 0.5` 做恢复旋转

相关代码：

- `src/user_demo/include/user_demo/mission_node.hpp:141-150`
- `src/user_demo/include/user_demo/mission_node.hpp:177-185`
- `src/user_demo/include/user_demo/mission_node.hpp:213-221`

但这里有一个非常关键的实现细节：

> [!WARNING]
> `tag_cb()` 中 `marker_found` 在找到目标标签时会被置为 `true`，但当前实现没有在 “本帧未找到目标标签” 时明确重置为 `false`。

相关代码：

- `src/user_demo/include/user_demo/mission_node.hpp:261-283`

这意味着：

- 从 “设计意图” 上看，系统是想在丢失标签后进入恢复旋转
- 但从 “当前实现” 上看，这个恢复分支未必总能按预期触发

所以你在比赛里如果看到它偶尔表现得不像 “标签丢失后主动找回”，不要只怀疑相机，也要回头看这个状态位逻辑。

### `cam_track`

`cam_track` 更多是依赖 TF 持续可用；如果 TF 查不到，就在回调里捕获异常并直接返回。

相关代码：

- `src/cam_track/include/cam_track/cam_track_node.hpp:93-98`
- `src/cam_track/include/cam_track/cam_track_node.hpp:160-167`

---

## 比赛里最该注意的几个现实问题

### 1. 标签识别稳定性决定控制稳定性

如果标签：

- 时有时无
- 位置抖动很大
- 边缘检测不稳定

那上层控制一定会抖。

### 2. 只调 PID，不调检测质量，通常没用

视觉闭环里最常见的误区是：

> 车在抖，于是疯狂调 P。

但真正原因常常是：

- 相机角度不对
- 光照太差
- 标签太远
- 识别更新不稳

### 3. 话题和 TF 两条视觉链不能混着想

因为它们的 “偏差定义” 并不一样：

- 一条是图像/消息偏差
- 一条是空间角度偏差

如果你不区分，很容易误以为两个控制器 “参数应该一样”，其实不是。

---

## 调试时最值得看的东西

1. `/ar_pose_marker`
   - 是否稳定发布
   - ID 是否正确
2. `/tf`
   - 是否存在 `ar_marker_*`
   - `map -> base` 是否正常
3. `/cmd_vel`
   - 对准阶段是否真的在发布角速度
4. 射击前后的状态变量
   - `marker_found`
   - `reach_sign`
   - `current_target_id`

---

## 一个非常关键的工程问题

> [!WARNING]
> 当前仓库中，上层发射击命令的字符串并不统一。

- `cam_track` 发布的是：`"shoot!"`
  - `src/cam_track/include/cam_track/cam_track_node.hpp:242`
- `mission_node` 发布的是：`"1"`
  - `src/user_demo/include/user_demo/mission_node.hpp:133`
- 而 `shoot_control` 期望的是：`"shoot"` / `"stopshoot"`

这说明：

- 感知与控制链条就算工作了
- 最后一跳执行层也可能因为协议不一致而失效

所以比赛里 “明明对准了却没打出去”，不一定是视觉问题，也可能是命令接口根本没对上。
