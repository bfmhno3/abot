# 代价地图详解

> 机器人不是 “看见障碍物就避开”，而是 “把世界转成一张带代价的地图”

## 第一性原理

对于路径规划器来说，环境不是墙、桌子、箱子，而是一张二维栅格：

- 可走区域 → 低代价
- 靠近障碍物 → 中高代价
- 障碍物本体 → 致命代价

这张图就是 **costmap（代价地图）**。

路径规划器真正做的事是：

> 在一张带权值的地图里，找到 “总代价最低” 的路线。

所以：

- 如果 costmap 画错了，规划一定错
- 如果 costmap 太激进，机器人会撞墙
- 如果 costmap 太保守，机器人会觉得哪都过不去

---

## 全局代价地图 vs 局部代价地图

### 全局代价地图（`global_costmap`）

- 坐标系：`map`
- 基于静态地图
- 用于全局路径规划
- 关注“大方向怎么走”

当前配置：

```yaml
global_frame: map
robot_base_frame: base_link
resolution: 0.1
static_map: true
```

### 局部代价地图（`local_costmap`）

- 坐标系：`odom`
- 不用静态地图，只看当前传感器
- 滚动窗口，跟着机器人走
- 用于 DWA 的短期避障

当前配置：

```yaml
global_frame: odom
rolling_window: true
width: 2.0
height: 2.0
resolution: 0.02
```

> [!TIP]
>
> 可以这样记：
>
> - 全局地图回答“我要去哪里”
> - 局部地图回答“我现在脚下能不能这么走”

---

## 机器人足迹（footprint）

当前配置：

```yaml
footprint: [[-0.17,-0.15],[-0.17,0.15],[0.17,0.15],[0.17,-0.15]]
```

这表示机器人在平面上是一个 **34cm × 30cm** 的矩形。

### 为什么 `footprint` 很重要？

规划器不是把机器人当成一个点，而是把它当成一个有宽度的实体。

如果 footprint 设得太小：
- 路径看起来能过
- 实际上车身会擦墙甚至撞墙

如果 footprint 设得太大：
- 很多本来能过的路会被认为过不去

> [!TIP]
>
> `footprint` 最好略大于真实车身尺寸 1~2cm，留一点安全裕量。

---

## `obstacle_range` 与 `raytrace_range`

当前配置：

```yaml
obstacle_range: 3.0
raytrace_range: 3.5
```

### `obstacle_range`

- 含义：多远以内的障碍会被加入 costmap
- 当前值：3.0m

### `raytrace_range`

- 含义：用来“清除”障碍的射线追踪范围
- 当前值：3.5m

为什么清除范围比标记范围更大？

因为当激光穿过空旷区域时，系统要把以前错误标记过的障碍“擦掉”。多给一点范围，可以更彻底清除旧障碍。

---

## `inflation_radius`：最值得关注的参数

当前配置：

```yaml
inflation_radius: 0.1
cost_scaling_factor: 3.0
```

### `inflation_radius` 是什么？

障碍物不只是 “不能撞到的那一格”，它周围还应该有一个缓冲区。这个缓冲区就是 inflation（膨胀层）。

举例：

- 障碍物本体半径 0cm
- 膨胀半径 10cm
- 那么障碍物周围 10cm 内都被标成“高代价区”

这样规划器会自然远离墙和桌角。

### 当前值的问题

> [!CAUTION]
>
> `inflation_radius: 0.1` 对一台 34cm × 30cm 的机器人来说偏小。

这意味着：

- 机器人会认为“离墙 10cm 也很安全”
- 实际比赛中，这通常会导致贴墙走
- 一旦定位轻微误差，就很容易蹭墙

### 建议值

- 安全优先：0.25~0.30
- 速度优先但仍保守：0.20
- 非常狭窄环境：0.15（谨慎）

---

## `cost_scaling_factor`：代价衰减曲线

当前值：

```yaml
cost_scaling_factor: 3.0
```

它决定障碍物周围的代价下降得有多快：

- 数值大：代价很快掉下去，机器人更敢贴近障碍
- 数值小：代价下降慢，机器人更怕障碍

可以把它想象成“障碍物的威慑力衰减速度”。

如果 `inflation_radius` 是 “影响半径”，那么 `cost_scaling_factor` 是 “影响强度曲线”。

---

## `local_costmap` 的滚动窗口

当前配置：

```yaml
rolling_window: true
width: 2.0
height: 2.0
resolution: 0.02
```

这表示局部代价地图：

- 以机器人为中心
- 大小 2m × 2m
- 分辨率 2cm

也就是说，DWA 只关心机器人周围 1 米左右的即时环境。

### 为什么这样设计？

因为局部规划要实时运行。它没必要知道整个赛场，只需要知道“我眼前这一小块地方有没有障碍”。

### 可能的问题

- 窗口太小：高速运动时来不及看到更远障碍
- 窗口太大：计算负担增加

对当前 abot 的最大速度（0.2m/s）来说，2m × 2m 是够用的。

---

## `sensor source：scan_filtered`

当前配置：

```yaml
observation_sources: scan
scan:
  sensor_frame: laser_link
  data_type: LaserScan
  topic: scan_filtered
  marking: true
  clearing: true
```

含义：

- `marking: true`：看到障碍就加进去
- `clearing: true`：看到空地就把旧障碍擦掉

这是正常且推荐的配置。

如果 clearing 关掉，代价地图会越来越“脏”，旧障碍永远不消失。

---

## 代价地图与比赛表现的直接关系

### 现象 1：机器人贴墙走

优先怀疑：

- `inflation_radius` 太小
- `cost_scaling_factor` 太大

### 现象 2：机器人总觉得前面过不去

优先怀疑：

- footprint 太大
- inflation_radius 太大
- 雷达误检太多

### 现象 3：机器人偶尔把空气当障碍

优先怀疑：

- `/scan_filtered` 数据不干净
- 雷达支架或车体反射被扫进去

---

## 推荐的安全型配置

如果你优先想避免撞墙，我建议从这里开始：

```yaml
inflation_radius: 0.25
cost_scaling_factor: 2.5
footprint: [[-0.17,-0.15],[-0.17,0.15],[0.17,0.15],[0.17,-0.15]]
obstacle_range: 3.0
raytrace_range: 3.5
```

这组参数比当前配置更保守，通常会让机器人离墙更远、路径更安全。

---

## 源码参考

| 文件 | 路径 |
|------|------|
| 通用代价地图参数 | `src/robot_slam/params/carto/costmap_common_params.yaml` |
| 全局代价地图 | `src/robot_slam/params/carto/global_costmap_params.yaml` |
| 局部代价地图 | `src/robot_slam/params/carto/local_costmap_params.yaml` |
| move_base 参数加载 | `src/robot_slam/launch/include/move_base.launch.xml` |
