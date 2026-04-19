# find_object_2d 目标识别

> 不是 “识别图片里有什么”，而是 “把训练过的目标图案变成消息、位姿和可视化结果”

## 第一性原理

`find_object_2d` 的思路不是深度学习分类，而是传统的**局部特征匹配 + 单应矩阵估计**。

你可以把它理解成：

1. 先把模板图片的特征点记下来
2. 再在当前摄像头图像中找相似特征点
3. 如果匹配关系足够好，就估计一个 `homography`（单应矩阵）
4. 再由这个变换推出：
   - 目标在图像里的四个角
   - 目标中心位置
   - 若有深度图，还能推出 3D 位姿和 TF

所以它并不是 “看见一个类别”，而是：

> “我在当前画面里找到了那个已知平面目标，而且知道它怎么投影进来的。”

---

## abot 中的启动方式

启动文件：

- `src/abot_find/launch/find_object_2d.launch`

当前默认参数：

```xml
<arg name="gui" default="true"/>
<arg name="image_topic" default="/usb_cam/image_raw"/>
<arg name="objects_path" default="/home/abot/find-object-melodic-devel/object"/>
<arg name="settings_path" default="~/.ros/find_object_2d.ini"/>
```

对应位置：

- `src/abot_find/launch/find_object_2d.launch:1-5`

这说明当前工程假设：

- 图像来自 `usb_cam`
- 目标模板放在某个对象目录中
- 参数配置保存在 `~/.ros/find_object_2d.ini`

> [!CAUTION]
> 这里的 `objects_path` 是明显的环境相关硬编码路径，需要进行修改。

---

## 主节点做了什么

入口文件：

- `src/abot_find/src/ros/find_object_2d_node.cpp`

它主要做四件事：

### 1. 读取运行模式参数

例如：

- `gui`
- `objects_path`
- `session_path`
- `settings_path`
- `subscribe_depth`

对应代码：

- `src/abot_find/src/ros/find_object_2d_node.cpp:77-83`

### 2. 加载对象模板或 session

逻辑大致是：

- 如果给了 `session_path`，就加载整个 session
- 否则如果给了 `objects_path`，就加载模板对象目录

对应代码：

- `src/abot_find/src/ros/find_object_2d_node.cpp:113-135`

### 3. 连接相机输入与检测逻辑

如果是 GUI 模式，会把相机图像输入接进图形界面与识别模块。
如果是无 GUI 模式，会把 `CameraROS` 的图像信号直接连接到 `FindObjectROS::detect()`。

对应代码：

- `src/abot_find/src/ros/find_object_2d_node.cpp:143-185`

### 4. 把检测结果发布为 ROS 消息和 TF

这个动作主要在 `FindObjectROS.cpp` 中完成。

---

## 输出了哪些东西

`FindObjectROS` 会发布三类消息：

```cpp
pub_ = nh.advertise<std_msgs::Float32MultiArray>("objects", 1);
pubStamped_ = nh.advertise<find_object_2d::ObjectsStamped>("objectsStamped", 1);
pubInfo_ = nh.advertise<find_object_2d::DetectionInfo>("info", 1);
```

对应位置：

- `src/abot_find/src/ros/FindObjectROS.cpp:50-52`

### 1. `objects`

类型：`std_msgs::Float32MultiArray`

特点：

- 最轻量
- 本质上是打包后的数字数组
- 每个目标占 12 个 float
- 内含目标 ID、宽高、单应矩阵参数

### 2. `objectsStamped`

类型：`find_object_2d::ObjectsStamped`

特点：

- 带 header
- 可以和图像进行时间同步
- 更适合后续可视化与 TF 配合

### 3. `info`

类型：`find_object_2d::DetectionInfo`

特点：

- 信息更全
- 除 ID 与尺寸外，还包含：
  - 文件路径
  - `inliers`
  - `outliers`
  - `homography`

如果你在调识别质量，`info` 往往最有用，因为它能帮助你判断：

> 到底是根本没匹配上，还是匹配到了但内点太少。

---

## 它如何发布 TF

这是 `find_object_2d` 在机器人项目里非常有价值的一点。

在 `publish()` 中，如果：

- 有检测结果
- 有深度图
- 深度常数有效

它就会为每个目标生成一个 TF frame：

```cpp
transform.child_frame_id_ = QString("%1_%2%3")
```

默认前缀是：

- `object`

所以最后会得到类似：

- `object_1`
- `object_1_b`
- `object_2`

对应代码：

- `src/abot_find/src/ros/FindObjectROS.cpp:39-45`
- `src/abot_find/src/ros/FindObjectROS.cpp:115-118`
- `src/abot_find/src/ros/FindObjectROS.cpp:195-198`

这意味着：

> `find_object_2d` 不只是给你一个“识别结果列表”，它还能把目标变成 TF 树中的一个实体。

这对机器人控制很重要，因为一旦目标进了 TF：

- 你可以查 `camera -> object_5`
- 也可以做更复杂的坐标变换
- 上层控制器就能按“空间目标”来写，而不是只按“图像中心点”来写

---

## 2D 图像位置是怎么提取出来的

`print_objects_detected_node.cpp` 演示了一个非常实用的下游用法。

它订阅：

- `objects`
- `objectsStamped`
- `image`

然后：

1. 读取每个目标的 12 维 homography 数据
2. 还原目标四个角在图像中的位置
3. 计算中心点
4. 发布 `geometry_msgs::Point object_position`
5. 并在图像上画框、画中心点、标注坐标

相关位置：

- `src/abot_find/src/ros/print_objects_detected_node.cpp:48-112`
- `src/abot_find/src/ros/print_objects_detected_node.cpp:114-165`
- `src/abot_find/src/ros/print_objects_detected_node.cpp:181-191`

其中一个关键动作是：

```cpp
obj_pos.x = (tmp_left_x + tmp_right_x)/2;
obj_pos.y = (...四角 y 均值...);
obj_pos.z = id;
pos_pub.publish(obj_pos);
```

这说明下游节点经常并不需要完整姿态，只需要：

- 目标中心 x
- 目标中心 y
- 目标 ID

也就是说，`find_object_2d` 的输出可以继续被“压缩”为更简单的业务控制输入。

---

## tf_example 在教你什么

`tf_example_node.cpp` 是一个很好的教学样例。

它会：

1. 订阅 `objectsStamped`
2. 根据目标 ID 推导对应 TF frame 名称
3. 用 `tf::TransformListener` 查询目标在目标坐标系中的位姿

相关位置：

- `src/abot_find/src/ros/tf_example_node.cpp:43`
- `src/abot_find/src/ros/tf_example_node.cpp:48-95`

这个例子非常重要，因为它告诉你：

> `find_object_2d` 的真正价值，并不只是在图像框出目标，而是在 ROS 里把目标“坐标化”。

---

## GUI 模式 vs 无 GUI 模式

### GUI 模式

优点：

- 可以直接看识别结果
- 方便添加训练对象
- 适合调试与教学

缺点：

- 依赖图形界面
- 比赛部署时未必需要

### 无 GUI 模式

优点：

- 更适合实际运行
- 更轻量

缺点：

- 调试信息不直观

对你当前的学习场景来说，GUI 模式虽然跑不起来，但你仍然可以从源码理解：

- 它的输入是什么
- 它的输出是什么
- 它怎么被集成进机器人系统

这已经足够有价值。

---

## 比赛视角下，这个模块适合做什么

适合：

- 识别训练过的平面目标
- 识别特定图案/贴纸/标识牌
- 在视觉任务里提供目标中心位置
- 配合深度图给出 3D 目标位姿

不适合：

- 任意类别通用识别
- 无模板新目标识别
- 复杂光照和大角度变化下的鲁棒场景理解

换句话说，它更像：

> “一个比赛里很实用的模板识别器”，而不是“现代通用视觉 AI”。

---

## 你最该注意的工程现实

### 1. 对模板质量有依赖

如果模板图片：

- 特征点太少
- 纹理太弱
- 重复图案太多

识别就会不稳。

### 2. 对相机与深度条件有依赖

只有在深度有效时，它才更容易生成可靠 TF。

如果目标太近、太远、深度图噪声大，`FindObjectROS` 会报 invalid depth。

### 3. 识别成功不等于控制成功

即使识别成功：

- 如果你没有后续控制器消费 `objects` / TF
- 或者消费逻辑太脆弱

比赛效果仍然不佳。

所以不要把视觉任务只理解成“有没有识别出来”，更要看：

> 识别结果是否足够稳定地流向控制闭环。
