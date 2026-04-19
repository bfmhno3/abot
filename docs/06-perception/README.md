# 感知层总览

> 机器人不是 “看见世界”，而是把图像和标签变成可用于决策的结构化信息

## 第一性原理

在 abot 里，感知层负责回答这些问题：

1. **我看到了什么？**
   - 例如：识别到了某个训练好的目标图案
2. **它在画面里的哪里？**
   - 例如：目标在图像左边还是右边
3. **它在机器人坐标系/地图坐标系里的位置是什么？**
   - 例如：某个 AR 标签在 `map` 下的坐标是多少
4. **这些信息应该交给谁用？**
   - 视觉对准控制器
   - 比赛任务状态机
   - 调试显示节点

所以感知层不是单一算法，而是一条完整的数据链：

```text
摄像头 / 深度相机
      │
      ├── AR 标签检测（ar_track_alvar）
      │        │
      │        ├── 发布 /ar_pose_marker
      │        └── 发布 ar_marker_* 相关 TF
      │
      └── 目标图案识别（find_object_2d）
               │
               ├── 发布 objects / objectsStamped / info
               └── 发布 object_* TF
```

上层节点再消费这些结果：

- `cam_track`：读取 `/tf`，根据 AR 标签方向做 P 控制转向
- `mission_node`：读取 `/ar_pose_marker`，根据标签横向偏差做对准与射击
- `print_objects_detected` / `tf_example`：做调试和结果可视化

---

## 本层在 abot 中的三个核心模块

| 模块 | 作用 | 关键文件 |
|------|------|----------|
| AR 标签检测 | 检测标签 ID 与位姿 | `src/user_demo/include/user_demo/mission_node.hpp` 依赖 `/ar_pose_marker`；`src/cam_track/include/cam_track/cam_track_node.hpp` 依赖 `ar_marker_*` TF |
| `find_object_2d` | 识别训练过的目标图案 | `src/abot_find/src/ros/find_object_2d_node.cpp` |
| `cam_track` | 利用视觉结果做闭环对准 | `src/cam_track/include/cam_track/cam_track_node.hpp` |

这三个模块合起来，才构成“感知 → 控制”的完整闭环。

---

## 两种视觉结果表示方式

这是本仓库最容易混淆、但也最值得理解的一点。

### 1. 话题消息型

例如：

- `/ar_pose_marker`
- `objects`
- `objectsStamped`
- `info`
- `object_position`

特点：

- 适合做业务逻辑判断
- 适合状态机读取
- 内容结构明确，可直接打印调试

例如 `mission_node` 就是订阅 `/ar_pose_marker`，再取出当前目标标签的 `pose.position.x` 作为横向偏差来源。

### 2. TF 坐标变换型

例如：

- `ar_marker_0`
- `ar_marker_1`
- `object_5`

特点：

- 更适合表达“空间关系”
- 适合直接问“目标在 map / camera / base 下的位姿是什么”
- 对运动控制和坐标换算更自然

例如 `cam_track` 并不读 `/ar_pose_marker`，而是从 `/tf` 中监听 `map -> ar_marker_i` 与 `map -> base` 的关系，再计算机器人该朝哪个方向转。

> [!TIP]
> 可以这样记：
> - **话题像报表**：告诉你“看见了什么”
> - **TF 像坐标账本**：告诉你“它到底在哪”

---

## 感知层真正服务的不是“识别”，而是“闭环控制”

单纯识别出目标还不够。比赛里真正重要的是：

- 目标有没有被稳定识别
- 偏差是否足够小，能否闭环收敛
- 丢失目标时是否有恢复策略
- 感知结果是否能直接驱动导航/射击逻辑

所以你看这个仓库时，不要只问：

> “识别算法是什么？”

更要问：

> “识别结果最终是怎样变成速度命令和射击命令的？”

这才是竞赛机器人真正有价值的部分。

---

## 本章节文档导航

| 文档 | 用途 |
|------|------|
| [ar-tag-tracking.md](ar-tag-tracking.md) | 理解 AR 标签检测、TF 与话题的两种消费方式 |
| [find-object-2d.md](find-object-2d.md) | 理解目标图案识别、消息输出与 TF 输出 |
| [cam-track-pid.md](cam-track-pid.md) | 理解视觉闭环对准与 P 控制参数 |

---

## 你学习这一层时最值得建立的认知

### 1. 感知结果不一定直接给任务层

很多时候会先经过：

- TF 转换
- 偏差提取
- PID / P 控制
- 状态机逻辑

所以“检测到目标”只是起点，不是终点。

### 2. 同一项目里可以同时存在两套视觉管线

在 abot 里就是这样：

- 一套是 AR 标签
- 一套是 find_object_2d 图案识别

它们不是互相替代，而是应对不同任务。

### 3. 感知错误会被放大成控制错误

例如：

- 目标中心算偏一点 → 机器人一直转不过来
- TF 延迟或不稳定 → `cam_track` 表现抖动
- 标签丢失后没恢复逻辑 → 状态机卡住

所以比赛里 “车不聪明”，很多时候不是控制器太差，而是感知层给它的信息不够稳。
