# Cartographer SLAM 建图

> 更现代的 2D SLAM：子图、回环、在线扫描匹配

## 第一性原理

Cartographer 和 Gmapping 的核心区别在于：

- **Gmapping** 更像 “边走边修正的粒子地图”
- **Cartographer** 更像 “先建立很多小地图（submap），再把这些小地图拼起来并整体优化”

这让 Cartographer 在以下场景更有优势：

- 路径更长
- 回环更重要
- 场景更复杂
- 需要更好的全局一致性

---

## abot 中的配置定位

配置文件：

- `src/robot_slam/params/zoo_2Dlidar_location_params.lua`

这是一个 **2D 激光雷达 + 轮式里程计** 的 Cartographer 配置：

| 配置项 | 值 |
|--------|----|
| `use_odometry` | true |
| `use_imu_data` | false |
| `num_laser_scans` | 1 |
| `num_point_clouds` | 0 |

也就是说：

> 它依赖激光雷达做主定位，依赖轮速计做运动先验，但**不使用 IMU**。

---

## 子图（Submap）机制

### `num_range_data = 35`

- 含义：每收集 35 帧激光数据，就构建一个局部子图
- 当前值：**35**

直觉理解：

- 数值小：子图更小，更新更频繁，更细致，但优化任务更多
- 数值大：子图更大，更稳定，但细节响应更慢

35 是一个比较小的子图规模，适合小场地和短路径。

---

## 在线相关性扫描匹配

### `use_online_correlative_scan_matching = true`

这表示 Cartographer 在实时定位时，会在当前位置附近尝试一个小范围搜索，找到最能对齐当前激光扫描的位置。

这能带来两个好处：

1. 降低对里程计精度的依赖
2. 在转角或纹理简单区域更稳

但代价是计算量更高。

### `linear_search_window = 0.1`

- 含义：平移搜索窗口半径 10cm
- 作用：允许 Cartographer 在当前位置附近 ±10cm 搜索更优匹配

如果地图经常有轻微错位，可以适当增大；如果 CPU 紧张，可以减小。

---

## 约束与回环

### `constraint_builder.min_score = 0.65`

- 含义：接受回环约束的最低匹配分数
- 当前值：**0.65**

影响：

- 太低：容易误认为“这里以前来过”，造成错误回环
- 太高：真正的回环也可能不接受

对竞赛小场地来说，0.65 是中等偏保守的值。

### `optimize_every_n_nodes = 35`

- 含义：每累计 35 个节点，就跑一次全局图优化
- 当前值：**35**

这和 `num_range_data=35` 形成了一种节奏：大约每形成一个子图就优化一次。

---

## 为什么 `use_imu_data = false`？

理论上 IMU 可以提升姿态估计，但当前配置里关闭了 IMU：

```lua
use_imu_data = false
```

这通常意味着：

1. 当前 IMU 质量/标定不够稳定
2. 只用 2D 激光雷达 + 轮速计已经够用
3. 不希望把不稳定的 IMU 引入系统复杂度

> [!TIP]
>
> 这也是一个很实用的工程决策：不是“能接 IMU 就一定更强”，而是“如果 IMU 不稳定，它反而会把定位带坏”。

---

## 参数如何影响机器人行为

### 现象 1：长走廊里定位偶尔跳

可能原因：

- `min_score` 太低，错误匹配被接受
- `linear_search_window` 太大，搜索过头

### 现象 2：CPU 占用高，系统卡顿

可能原因：

- 在线相关匹配开启且窗口较大
- 优化频率太高
- 子图太小（优化任务太多）

### 现象 3：回到起点地图接不上

可能原因：

- 回环分数阈值太高
- 场景特征太少
- 轮速计误差太大，先验太差

---

## 与 Gmapping 的取舍

| 维度 | Gmapping | Cartographer |
|------|----------|--------------|
| 建图速度 | 快 | 中等 |
| 回环能力 | 一般 | 更强 |
| 参数复杂度 | 低 | 高 |
| 算力需求 | 低 | 更高 |
| 工程容错 | 高 | 中等 |

对 abot 竞赛来说：

- 如果你只是想赛前快速扫一张图，**Gmapping 更省心**
- 如果你已经熟悉参数，想追求更好的全局一致性，**Cartographer 更值得投入**

---

## 建议的理解方式

先不要试图一开始就 “调懂所有 Lua 参数”。优先理解下面 4 个：

1. `num_range_data`
2. `use_online_correlative_scan_matching`
3. `linear_search_window`
4. `constraint_builder.min_score`

因为这 4 个参数，最直接决定了：

- 子图有多频繁
- 局部匹配有多激进
- 回环有多严格

这三点就是 Cartographer 在比赛场景中的核心性格。

---

## 源码参考

| 文件 | 路径 |
|------|------|
| Cartographer 配置 | `src/robot_slam/params/zoo_2Dlidar_location_params.lua` |
| 启动入口 | `src/robot_slam/launch/nav_cartographer.launch` |
