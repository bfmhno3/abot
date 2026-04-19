# Launch 文件与参数系统

> 用一份声明式配置把“启动哪些节点、加载哪些参数、如何组织命名空间”说清楚

## 为什么需要 launch 文件？

当机器人系统只包含一个节点时，`rosrun` 已经够用。

但在真实项目里，通常需要同时完成这些事：

- 启动多个节点
- 从 YAML 批量加载参数
- 给同类节点套不同命名空间
- 把一个节点里写死的接口名 remap 到系统实际使用的话题名
- 把大系统拆成多个子 launch 复用

这就是 `roslaunch` 和 `.launch` 文件存在的意义。

你可以把它理解成：

> `rosrun` 是“手动启动一个程序”，`roslaunch` 是“声明式启动一套系统”。

---

## 先建立一个启动层心智模型

很多人把 launch 文件理解成“一个 XML 脚本文件”，这没错，但还不够。

更准确地说，launch 文件在 ROS1 里承担了三层职责：

1. **进程拼装**：决定要启动哪些节点
2. **运行时配置注入**：决定参数、命名空间、remap 如何作用于这些节点
3. **系统结构表达**：把“这套系统由哪些部件组成”用可读文本写出来

所以 launch 文件不只是“方便启动”，它还是一张非常重要的系统结构图。

如果你学会读 launch 文件，你通常会更快回答这些问题：

- 这个系统是由哪些节点组成的？
- 这些节点分属哪些包？
- 它们依赖哪些 YAML 参数？
- 它们有没有命名空间隔离？
- 哪些接口名字在运行时被改掉了？

---

## launch 文件到底是什么？

ROS1 的 `.launch` 文件本质上是 **XML 格式的系统配置描述**。

它不会替你发明业务逻辑，而是负责描述：

- 要运行哪些节点
- 节点运行时应该叫什么名字
- 参数从哪里来
- 哪些节点属于同一个命名空间
- 哪些接口名需要重映射
- 是否需要包含其他 launch 文件

最外层通常是：

```xml
<launch>
  ...
</launch>
```

### 为什么 launch 文件适合做系统入口？

因为它描述的是“运行时组合关系”，而不是“源码怎么编译”。

这和 `CMakeLists.txt` 的职责完全不同：

- `CMakeLists.txt` 关心的是：怎么把代码编译出来
- `.launch` 关心的是：编译出来以后，这些节点怎么一起运行

这也是为什么 launch 文件读起来更像“系统配置”，而不是“程序源码”。

---

## 先看一个真实例子

文件：`src/abot_base/abot_bringup/launch/robot_with_imu.launch`

```xml
<launch>
  <include file="$(find abot_bringup)/launch/bringup_with_imu.launch"/>
  <include file="$(find abot_bringup)/launch/model.launch"/>
  <include file="$(find abot_bringup)/launch/rplidar.launch"/>
</launch>
```

这个例子说明了一件事：

- 顶层 launch 文件不一定自己把所有节点写完
- 它也可以只是一个“系统拼装入口”
- 真正的节点启动逻辑分散在多个子 launch 文件里

这正是大型系统推荐的组织方式。

### 这个例子体现了什么设计思想？

它体现的是：

> 顶层 launch 负责拼装，子 launch 负责细节。

这样做的好处是：

- 顶层入口更容易一眼看懂系统骨架
- 每个子模块可以单独维护自己的启动逻辑
- 不同场景可以复用相同的子 launch

例如：

- 你可以保留同一个雷达 launch
- 但换不同的底盘、模型、导航入口
- 最终通过 include 重新组合出不同模式的系统

---

## 最常用的核心标签

### `<node>`：启动一个节点

```xml
<node pkg="abot_bringup" type="abot_driver" name="abot_driver" output="screen"/>
```

常见属性：

| 属性 | 含义 |
|------|------|
| `pkg` | 节点所在包名 |
| `type` | 可执行文件名 |
| `name` | 节点运行时名字 |
| `output="screen"` | 把日志直接打印到终端 |
| `respawn="true"` | 节点异常退出后自动拉起 |

> [!TIP]
> `type` 不是“源码文件名”，而是最终可执行目标名。对 C++ 节点来说，它通常对应 `add_executable(...)` 生成的结果；对 Python 节点来说，通常是脚本名。

### `<node>` 真正控制的是什么？

它控制的不是“源码”，而是“运行中的一个 ROS 进程实例”。

所以你在 `<node>` 上写的东西，本质上是在回答：

- 这个进程从哪个包来
- 运行时叫什么名字
- 它在什么命名空间下
- 需要加载哪些参数
- 是否应该崩溃后自动拉起

### `<param>`：设置单个参数

```xml
<param name="use_sim_time" value="false"/>
```

它适合设置少量、简单、单值参数。

### `<rosparam>`：批量加载 YAML 参数

```xml
<node name="abot_driver" pkg="abot_bringup" type="abot_driver" output="screen">
  <rosparam file="$(find abot_bringup)/params/base_params_with_imu.yaml" command="load"/>
</node>
```

这是项目里最常见的参数加载方式。

### 为什么 `<param>` 和 `<rosparam>` 要分开理解？

因为它们适合的粒度不同：

- `<param>` 适合少量、临时、直观的单值配置
- `<rosparam>` 适合整批结构化参数，尤其是 YAML 文件

在真实项目里，如果底盘、雷达、导航的参数很多，几乎总是用 `<rosparam>` 更可维护。

### `<arg>`：为 launch 文件定义可传入变量

```xml
<arg name="map_name" default="my_lab.yaml"/>
```

命令行覆盖方式：

```bash
roslaunch robot_slam navigation.launch map_name:=arena.yaml
```

### `<include>`：复用其他 launch 文件

```xml
<include file="$(find robot_slam)/launch/include/move_base.launch.xml"/>
```

它的作用类似“模块化拆分”。

### `<remap>`：不改代码，改接口名

```xml
<remap from="scan" to="scan_filtered"/>
```

这让节点代码和系统实际接线方式解耦。

### `<group>`：把一批配置放进同一作用域

```xml
<group ns="robot1">
  ...
</group>
```

它最常用于：

- 给一批节点统一套命名空间
- 和 `if` / `unless` 配合做条件挂载

---

## 参数到底会进到哪里？

这是 launch 新手最容易混乱的地方。

### 1. `<node>` 外面的参数

```xml
<param name="foo" value="1"/>
```

它会写进全局命名空间或当前作用域命名空间。

### 2. `<node>` 里面的参数

```xml
<node name="abot_driver" ...>
  <param name="port" value="/dev/abot"/>
</node>
```

它通常会进入该节点的**私有命名空间**。

也就是说，在最常见的“相对参数名”写法下，节点里通常以 `~port` 的方式读取，最终路径类似：

```text
/abot_driver/port
```

> [!NOTE]
> 所以“参数写在哪一层”非常重要。看起来都是 `param`，但最终所在命名空间可能完全不同。

### 为什么这个问题这么容易出错？

因为 launch 文件表面上看只是 XML 层级，但在 ROS 里，这个层级会直接影响参数最终路径。

很多配置错误并不是“值写错了”，而是：

- 参数落在了全局命名空间
- 但节点却在读私有命名空间

或者反过来：

- 你以为自己改了 `/abot_driver/port`
- 实际上你改的是 `/port`

这两种情况，运行现象可能非常像，但根因完全不同。

### 一个实用判断方法

如果某个参数明显是“这个节点自己的内部配置”，那么它大概率应该跟在 `<node>` 里面。

比如：

- 串口端口
- 波特率
- 传感器阈值
- PID 参数

而那些更偏“系统级开关”的东西，才更可能放在外层作用域里。

---

## 替换参数 `$(...)`：让 launch 文件不要写死

XML 本身是静态的，但 `roslaunch` 在正式解析前会先处理 `$(...)` 这种替换参数。

最常用的几种：

| 写法 | 作用 |
|------|------|
| `$(find pkg)` | 找到某个包的路径 |
| `$(arg name)` | 读取 `<arg>` 的值 |
| `$(env VAR)` | 读取环境变量 |
| `$(optenv VAR default)` | 读取环境变量，若无则用默认值 |
| `$(anon name)` | 生成匿名唯一名字 |
| `$(dirname)` | 当前 launch 文件所在目录 |
| `$(eval ...)` | 执行简单 Python 表达式 |

示例：

```xml
<arg name="map_name" default="my_lab.yaml"/>
<node name="map_server"
      pkg="map_server"
      type="map_server"
      args="$(find robot_slam)/maps/$(arg map_name)"/>
```

这里的意思就是：

1. 先找到 `robot_slam` 包路径
2. 再拼接 `maps/` 目录
3. 最后把 `map_name` 参数值接进去

### 这些替换参数到底在解决什么问题？

它们解决的是：

> 不要把运行环境、文件路径、场景变量硬编码在 launch 文件里。

如果没有这些替换参数，你就不得不把很多路径、文件名、变量值写死。一旦：

- 包目录变了
- 地图名变了
- 启动场景变了
- 环境变量变了

你的 launch 文件就要跟着重写。

### `$(find pkg)` 为什么这么常见？

因为 launch 文件经常需要引用：

- YAML 参数文件
- 其他 launch 文件
- 地图文件
- URDF / xacro 文件

这些资源都属于某个包，而 `$(find pkg)` 让你不用手写绝对路径。

### `$(arg name)` 为什么重要？

因为它让 launch 文件可以从“固定脚本”变成“可配置模板”。

同一份 launch，你可以通过改参数来切换：

- 地图名
- 是否开 RViz
- 是否启用 GUI
- 是否进入仿真模式

这也是大型系统里 launch 复用能力的来源之一。

---

## 条件启动：`if` 和 `unless`

ROS1 的 launch 没有传统编程语言里的 `if/else` 语句，但很多标签都支持条件属性。

```xml
<node pkg="rviz" type="rviz" name="rviz" if="$(arg use_gui)"/>
<node pkg="fake_pkg" type="headless_node" name="headless" unless="$(arg use_gui)"/>
```

适用场景：

- 仿真模式 vs 真机模式
- 带界面 vs 无界面
- 调试版 vs 比赛版

### 为什么条件启动很有用？

因为很多系统结构是“同一套骨架，不同场景下挂不同模块”。

例如：

- 调试时你想带 RViz
- 比赛时你想关掉图形界面
- 真机上要起驱动
- 仿真里则换成 fake driver

这些变化如果全靠复制多份 launch 文件，会很快失控；而 `if/unless` 可以让同一份入口覆盖多种场景。

---

## 一个非常重要的误区：launch 不保证严格启动顺序

很多初学者看到 launch 文件从上往下写，就以为节点会严格按这个顺序一个个启动。

这是错误的。

更准确的理解是：

- `roslaunch` 会组织启动过程
- 但不应被当成“严格串行调度器”
- 如果节点 A 依赖节点 B，稳健做法应该是 **A 自己等待依赖就绪**

例如：

- 服务依赖可以在代码里 `wait_for_service`
- 话题依赖可以等待第一帧数据
- TF 依赖可以等待 transform 可用

> [!WARNING]
> 如果你的系统“只有按照某种时序碰巧启动才不报错”，那通常说明问题不在 launch 文件，而在节点自身没有正确处理依赖未就绪状态。

### 为什么这个误区非常危险？

因为它会让你用错误方式修系统：

- 给 launch 文件乱加顺序幻想
- 反复改 include 顺序
- 甚至靠 sleep 或多次重启“碰运气”

这些都不是稳健做法。

真正稳健的系统应该满足：

- 上游没就绪时，下游能等待
- 第一帧数据没来时，节点不崩
- TF 暂时不可用时，逻辑能重试

也就是说，**健壮性主要写在节点逻辑里，而不是写在 launch 顺序里。**

---

## `clear_params`：防止旧参数残留

当你反复调试同一个节点时，参数服务器里可能残留上一次运行写进去的旧值。

这时可以在节点上加：

```xml
<node pkg="some_pkg" type="some_node" name="some_node" clear_params="true"/>
```

它的作用是：

- 在启动该节点前
- 先清掉该节点私有命名空间下的旧参数
- 避免旧配置污染当前运行

### 为什么这个选项很实用？

因为参数服务器是“状态会留存”的。

如果你不停重启 launch、反复改 YAML、切换节点名字，旧参数残留会造成一种非常烦人的现象：

- 你以为当前系统在用新配置
- 但实际上某些旧参数还挂在服务器里
- 于是运行现象和文件内容对不上

这时 `clear_params="true"` 就是在帮你减少“历史状态污染当前结果”。

---

## 在 abot 里，最值得注意的 launch 结构

### 1. 顶层入口常常只负责拼装

如 `robot_with_imu.launch`，主要是 include 多个子 launch，而不是所有内容都写在一个文件里。

### 2. 参数通常来自 YAML

例如 `abot_driver` 会从 `base_params_with_imu.yaml` 读入底盘参数，这比在 launch 里一条条手写更适合维护。

### 3. launch 文件和系统架构是强绑定的

读 launch 文件时，你其实是在看：

- 哪些包会一起启动
- 哪些节点是系统的关键骨架
- 数据从哪些模块流向哪些模块

所以 launch 文件既是启动脚本，也是**系统结构图的文字版**。

### 4. launch 是理解仓库结构的捷径

如果你现在对 abot 仓库还不熟，直接去看所有源码很容易迷路。

但如果你先从 launch 文件入手，你往往能更快抓住：

- 哪些节点属于底盘层
- 哪些属于感知层
- 哪些属于导航层
- 哪些属于任务调度层

---

## 阅读 launch 文件时的推荐顺序

建议你以后拿到一个陌生 launch 文件时，按这个顺序看：

1. **先看 include 了哪些子 launch**
2. **再看启动了哪些 node**
3. **再看这些 node 分别加载了哪些 rosparam**
4. **最后看有没有 remap、namespace、if/unless**

这样更容易先抓住系统骨架，再看细节。

### 为什么这个顺序有效？

因为它符合“先整体、后局部”的阅读规律。

如果你一上来就盯着 `<param>`、`<remap>` 细节，很容易在局部里打转；但如果你先看 include 和 node，你会先知道：

- 系统里到底有哪些模块
- 哪些模块是入口
- 哪些模块只是附属件

这会让后面的细节解释自动更容易理解。

---

## 一个学习者最该建立的 launch 认知

1. launch 文件负责的是“怎么把系统拉起来”，不是“节点内部怎么工作”。
2. 它是节点、参数、命名空间、remap、子模块之间的装配说明书。
3. 如果你还不熟仓库结构，先读 launch 往往比先读源码更容易抓住系统骨架。
4. 很多 launch 疑难问题，本质上不是 XML 语法问题，而是命名空间和参数落点理解错了。

---

## 和本章其他文档的关系

- 如果你想知道常见调试命令怎么用，看：[ros-cli.md](ros-cli.md)
- 如果你想知道工作空间、`package.xml`、`catkin_make` / `catkin build` 是怎么回事，看：[catkin-workspace-and-build.md](catkin-workspace-and-build.md)
- 如果你想回到本章总览，看：[README.md](README.md)
