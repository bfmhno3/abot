# ROS 命令行与调试工具

> 把 ROS 图“看见、问清、动起来”的终端入口

## 为什么要学命令行工具？

如果把 ROS 看成一个由许多节点组成的分布式系统，那么命令行工具就是你观察和操纵这个系统的控制台。

它们主要解决四类问题：

1. **启动基础设施**：系统有没有真正跑起来？
2. **启动节点**：某个功能包到底有没有成功执行？
3. **观察通信**：谁在发消息？谁在收消息？频率对不对？
4. **检查状态**：参数、服务、消息定义到底是什么？

在没有 ROS 运行环境的前提下，理解这些命令的职责边界也很重要，因为它能帮助你读懂教程、日志、launch 文件和源码里的调用关系。

---

## 一张总图：这些命令分别管什么？

> [!NOTE]
> 下面说的“ROS 图”对应英文里常见的 **ROS graph**。为了阅读顺一点，本文有时也会把它叫作“图谱”，两者说的是同一层东西。

```text
                roscore
                   │
     ┌─────────────┼─────────────┐
     ▼             ▼             ▼
  rosrun       roslaunch      rosparam
  启动单点      启动系统       查/改参数
     │             │
     └──────┬──────┘
            ▼
      正在运行的 ROS 图
            │
   ┌────────┼────────┬────────┐
   ▼        ▼        ▼        ▼
rostopic  rosnode  rosservice rosmsg/rossrv
看话题    看节点    调服务     看消息定义
```

> [!TIP]
> 初学时最容易把这些命令混成一团。记住一个简单分工：`roscore` 管基础设施，`rosrun/roslaunch` 管启动，`rostopic/rosnode/rosservice` 管运行态观察，`rosparam` 管配置，`rosmsg/rossrv` 管接口定义。

---

## 先建立一个运行时心智模型

很多人学 ROS 命令行时会把命令一个个背下来，但不知道它们为什么存在。更好的理解方式是先把运行时系统拆成三层：

### 第 1 层：ROS 图注册层

这里回答的问题是：

- 系统里有哪些节点？
- 谁在发布哪个 topic？
- 谁提供了哪个 service？
- 参数服务器上有哪些配置？

`roscore`、`rosnode`、`rosparam`、`rosservice list` 这类命令，主要都在帮你看这一层。

### 第 2 层：实际通信层

这里回答的问题是：

- `/odom` 里到底有没有数据？
- `/scan_filtered` 的频率是不是正常？
- `/cmd_vel` 现在是谁在发？

`rostopic echo`、`rostopic hz`、`rostopic info` 主要都在帮你看这一层。

### 第 3 层：接口定义层

这里回答的问题是：

- 一个消息到底有哪些字段？
- 一个服务的 request / response 长什么样？

`rosmsg` 和 `rossrv` 就是在看这一层。

> [!NOTE]
> 很多排障之所以绕远路，是因为把这三层混在了一起。比如：`rostopic echo` 没数据，不一定说明节点没启动；也可能是节点启动了，但上游传感器没工作，或者 remap 后订错了 topic。

---

## 核心基础设施命令

### `roscore`

`roscore` 用来启动 ROS1 的核心运行基础设施。对学习者来说，最重要的不是把它拆成多少进程，而是知道它提供了三类能力：

- **Master**：保存谁发布了什么、谁订阅了什么
- **Parameter Server**：保存系统参数
- **rosout 日志通道**：汇总节点日志

常见用法：

```bash
roscore
```

### 可以把 `roscore` 想成什么？

你可以把它想成：

> 一个让“整张 ROS 图可以成立”的基础设施集合。

没有它时，进程在操作系统层面也许仍然能被启动，但通常无法作为一个正常工作的 ROS 节点参与 ROS 图：

- 节点之间没法通过 ROS 规则完成注册
- 话题与服务关系没法被协调
- 参数服务器也不存在

也就是说，`roscore` 不负责替你处理具体数据流，但它负责让“谁和谁应该连起来”这件事可被组织起来。

> [!NOTE]
> 更准确的理解是：`roscore` 启动 ROS 图赖以存在的一组核心服务。不要把它死记成“永远固定的三个独立进程”，因为学习重点是职责，而不是实现细节的绝对化表述。

### 初学者最常见的误区

#### 误区 1：`roscore` 一启动，机器人就“跑起来了”

不是。

`roscore` 只是把基础设施搭好，真正的业务节点还需要你用 `rosrun` 或 `roslaunch` 启动。

#### 误区 2：有 `roscore` 就说明系统正常

也不是。

`roscore` 正常，只能说明“ROS 图注册中心还活着”；不代表：

- 驱动节点已经启动
- 传感器数据正常
- 参数已经正确加载
- TF 树完整

---

## 启动节点的两种方式

### `rosrun`：启动单个节点

当你已经知道包名和可执行文件名时，`rosrun` 最直接。

```bash
rosrun abot_bringup abot_driver
```

它适合：

- 单独调试一个节点
- 快速验证某个可执行文件能否启动
- 不依赖复杂参数和 launch 结构的场景

在概念上，`rosrun` 做的事可以理解成：

1. 找到包路径
2. 找到对应可执行文件
3. 让操作系统启动这个进程

### 为什么 `rosrun` 在学习阶段特别重要？

因为它会逼你把“一个节点到底是什么”看清楚。

很多初学者一开始总是直接 `roslaunch`，结果容易把所有问题都混成“大系统没起来”。而 `rosrun` 让你只面对一个节点，因此你更容易分辨：

- 是程序本身崩了
- 还是参数没给对
- 还是 launch 拼装层出了问题

### `roslaunch`：启动一个系统

当你需要同时启动多个节点、加载 YAML 参数、应用命名空间和 remap 时，通常使用 `roslaunch`。

```bash
roslaunch abot_bringup robot_with_imu.launch
```

它适合：

- 一键拉起整套系统
- 自动加载参数
- 嵌套子 launch 文件
- 管理复杂启动结构

> [!TIP]
> 简单记忆：`rosrun` 像“手动开一个程序”，`roslaunch` 像“加载一套系统配置”。
>
> 学习和调试时，手动先起 `roscore` 很常见，因为这样更容易分清“基础设施是否正常”和“业务节点是否正常”。但也要知道：如果当前没有 master，`roslaunch` 通常会自动把它拉起来。

### `rosrun` 和 `roslaunch` 的本质区别

很多人会把两者理解成“一个简单、一个高级”，但更关键的区别是它们服务的对象不同：

| 命令 | 服务对象 | 核心价值 |
|------|----------|----------|
| `rosrun` | 单个节点 | 直接、简单、便于单点调试 |
| `roslaunch` | 节点集合 | 声明式地拼装整套系统 |

你可以把 `roslaunch` 理解成“系统级入口”，而 `rosrun` 更接近“组件级入口”。

### 在 abot 里怎么用这个区分？

- 你想验证底盘驱动本身能不能起：优先想 `rosrun abot_bringup abot_driver`
- 你想把底盘、模型、雷达一起拉起来：优先想 `roslaunch abot_bringup robot_with_imu.launch`

这类区分会直接影响你的排障效率。

---

## 运行态观察：话题、节点、服务

### `rostopic`

`rostopic` 面向**话题**。它最常用来回答四个问题：

- 现在有哪些话题？
- 某个话题里到底在传什么？
- 发布频率正常吗？
- 谁在发布、谁在订阅？

常见命令：

```bash
rostopic list
rostopic echo /odom
rostopic hz /scan_filtered
rostopic info /cmd_vel
```

在 abot 里，最值得先看的话题包括：

| 话题 | 为什么重要 |
|------|------------|
| `/cmd_vel` | 导航与任务层最终给到底盘的速度指令 |
| `/odom` | 底盘里程计输出 |
| `/scan_filtered` | 导航真正使用的雷达输入 |
| `/imu/data_raw` | IMU 原始输出 |
| `/shoot` | 上层任务给发射机构的命令 |

### 读懂 `rostopic` 的四个典型动作

#### `rostopic list`

看系统“暴露出了哪些通信管道”。

它适合回答的是“有没有这个话题”，不适合回答“数据是不是正常”。

#### `rostopic echo <topic>`

直接看某条话题里的实时消息内容。

它最适合回答：

- 到底有没有数据
- 数值长什么样
- 某些字段是不是明显异常

例如看 `/odom` 时，你会更直观地感受到位姿、速度、协方差这些字段是不是在变化。

#### `rostopic hz <topic>`

看发布频率。

它特别适合区分这类问题：

- 话题存在，但更新很慢
- 传感器在断断续续地出数据
- 系统负载过高导致频率掉下来了

#### `rostopic info <topic>`

看发布者和订阅者。

它在排查“为什么这个消息没人用上”时特别有价值，因为它能告诉你：

- 有没有 publisher
- 有没有 subscriber
- 双方是不是连在你以为的那条 topic 上

> [!TIP]
> `rostopic echo` 是“看内容”，`rostopic hz` 是“看节奏”，`rostopic info` 是“看接线关系”。把这三者分开理解，排障会快很多。

### `rosnode`

`rosnode` 面向**节点**，用来确认某个节点是不是活着、连了哪些接口。

```bash
rosnode list
rosnode info /abot_driver
rosnode kill /some_node
```

它适合检查：

- 节点是否真的启动了
- 节点订阅了哪些话题
- 节点发布了哪些话题
- 节点提供了哪些服务

### `rosnode` 和 `rostopic` 有什么区别？

很多学习者会把它们当成同一类工具，其实视角完全不同：

- `rosnode` 是**从进程视角**看系统
- `rostopic` 是**从通信管道视角**看系统

举例：

- 你怀疑 `abot_driver` 根本没起来，就先看 `rosnode list`
- 你怀疑 `/cmd_vel` 虽然存在但没人发，就先看 `rostopic info /cmd_vel`

### `rosservice`

`rosservice` 面向**服务**，也就是同步请求/响应接口。

```bash
rosservice list
rosservice call /imu/calibrate_imu
```

在 abot 里，`imu/calibrate_imu` 就是一个很典型的服务：调用它相当于发出一次“请现在重新校准”的请求。

### 服务为什么值得单独看？

因为它和 topic 的通信哲学完全不同。

- topic 适合持续数据流
- service 适合一次请求、一次响应

如果你看到某个功能是“按一下触发一次动作”，那它通常更像 service；如果你看到某个功能是“持续不断地流数据”，那通常更像 topic。

---

## 配置类命令

### `rosparam`

`rosparam` 用来和参数服务器交互。

```bash
rosparam list
rosparam get /abot_driver/port
rosparam set /abot_driver/max_v_liner_x 80
rosparam load config.yaml
```

你可以把参数服务器理解成一个全局键值表，但要注意两点：

1. **参数被读到什么时候，取决于节点实现**
2. `rosparam set` 并不等于节点就会立刻生效

例如：

- 某些节点只在启动时读取参数
- 某些节点通过 `dynamic_reconfigure` 支持运行时更新

> [!WARNING]
> 很多新手会误以为 `rosparam set` 一改，机器人就一定马上变。实际上是否即时生效，取决于节点代码有没有在运行时重新读取或注册动态回调。

### 参数服务器最容易让人误解的点

#### 误区 1：参数在服务器里，节点就一定会用

不一定。

参数服务器只是“把值放在那里”。节点是否读取、何时读取、是否支持热更新，都取决于节点实现。

#### 误区 2：参数名写对了就说明参数作用域对了

也不一定。

ROS 参数是有命名空间的，`/abot_driver/port` 和 `/port` 就不是一回事。很多配置问题根本不是值错了，而是参数进到了错误的命名空间。

#### 误区 3：launch 里写了参数，就一定覆盖成功

还要看：

- 这个 launch 文件有没有真的被 include 到运行入口里
- 节点最终名字是不是和你以为的一样
- 参数写在了 `<node>` 里面还是外面

这些问题在读 launch 文件时尤其常见。

---

## 接口定义类命令

### `rosmsg` / `rossrv`

这两个命令不负责启动或通信，它们负责查看接口定义。

```bash
rosmsg show geometry_msgs/Twist
rossrv show std_srvs/Empty
```

它们最适合在你读 launch、代码或日志时快速确认：

- 一个话题的数据结构是什么
- 一个服务请求和响应各包含什么字段

例如：

```bash
rosmsg show nav_msgs/Odometry
```

你就能直接看到 `/odom` 里有哪些位姿、速度和协方差字段。

### 为什么这两个命令很重要？

因为很多“看不懂运行现象”的根源，其实是你对消息结构理解错了。

比如：

- 你以为 `/cmd_vel` 里只有线速度，其实还有角速度
- 你以为 `/odom` 只是位置，其实还带速度和协方差
- 你以为某个 service 没返回值，结果它其实有 response 字段，只是你没看接口定义

所以 `rosmsg` / `rossrv` 的价值并不是“查文档”，而是帮你把抽象接口落实成字段级理解。

---

## 和 catkin 的关系：放在这一篇里只讲入口

你会在很多教程里同时看到 ROS CLI 和 catkin 命令，但它们不是一类工具。

- 上面这些命令主要面向**运行与调试**
- `catkin_make` / `catkin build` 主要面向**构建工作空间**

所以在本章里，catkin 只需要记住入口认知：

```bash
cd ~/abot
catkin_make
source devel/setup.bash
```

如果你想系统理解工作空间结构、`package.xml`、`CMakeLists.txt`、`catkin_make` 与 `catkin build` 的差异，请看后续子文档：[catkin-workspace-and-build.md](catkin-workspace-and-build.md)。

---

## 调试时最常见的命令组合

### 1. 怀疑节点没起来

```bash
rosnode list
rosnode info /abot_driver
```

判断思路：

1. 先确认节点名有没有出现在系统里
2. 再确认它发布/订阅了哪些接口
3. 如果节点存在但接口异常，再继续排查参数或上游依赖

### 2. 怀疑话题没有数据

```bash
rostopic list
rostopic echo /odom
rostopic hz /scan_filtered
```

判断思路：

1. 先看话题是不是存在
2. 再看有没有消息内容
3. 最后看频率是否正常

### 3. 怀疑参数没加载进去

```bash
rosparam list
rosparam get /abot_driver/port
```

判断思路：

1. 先确认参数是否存在
2. 再确认它是不是在正确命名空间下
3. 最后再判断节点是否真的消费了这个参数

### 4. 怀疑接口理解错了

```bash
rosmsg show geometry_msgs/Twist
rossrv show std_srvs/Empty
```

判断思路：

1. 先别猜字段
2. 直接看消息 / 服务定义
3. 再回头理解 launch、日志和代码中的行为

---

## 在 abot 中，先记住这几个命令最有用

| 目标 | 推荐命令 |
|------|----------|
| 看系统里有哪些节点 | `rosnode list` |
| 看某个节点连了什么 | `rosnode info /node_name` |
| 看速度指令有没有发出来 | `rostopic echo /cmd_vel` |
| 看雷达数据是否正常 | `rostopic echo /scan_filtered` / `rostopic hz /scan_filtered` |
| 看底盘有没有发布里程计 | `rostopic echo /odom` |
| 看参数是否正确加载 | `rosparam get /param_name` |
| 查消息字段 | `rosmsg show <msg>` |

---

## 一个学习者最该建立的命令行认知

### 1. 命令不是越多越厉害，而是视角要对

- 查节点状态，用 `rosnode`
- 查话题内容，用 `rostopic`
- 查参数，用 `rosparam`
- 查接口定义，用 `rosmsg` / `rossrv`

选错视角，排障就会一直打转。

### 2. “系统没起来”不是一个问题，而是一组问题

它可能分别意味着：

- `roscore` 没启动
- 节点没启动
- 节点启动了但参数错了
- topic 有名字但没数据
- topic 有数据但没人订阅
- service 在，但调用逻辑错了

CLI 的价值就是把这团混乱拆开。

### 3. 学 ROS CLI，本质上是在学怎么观察系统

你并不是在背命令本身，而是在训练自己：

- 从节点视角观察
- 从 topic 视角观察
- 从参数视角观察
- 从接口定义视角观察

一旦这套观察方式建立起来，你读 abot 的 launch、驱动、导航和任务逻辑时就会顺很多。

---

## 相关文档

- [launch-files-and-parameters.md](launch-files-and-parameters.md) — 理解 launch 文件、参数加载、命名空间和替换参数
- [catkin-workspace-and-build.md](catkin-workspace-and-build.md) — 理解工作空间结构、构建流程与 catkin 工具链
- [README.md](README.md) — 返回本章节总览
