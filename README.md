# bfmhno3's ROS1 Robot Notes

<div align="center">

```text
██████╗ ███████╗███╗   ███╗██╗  ██╗███╗   ██╗ ██████╗ ██████╗ 
██╔══██╗██╔════╝████╗ ████║██║  ██║████╗  ██║██╔═══██╗╚════██╗
██████╔╝█████╗  ██╔████╔██║███████║██╔██╗ ██║██║   ██║ █████╔╝
██╔══██╗██╔══╝  ██║╚██╔╝██║██╔══██║██║╚██╗██║██║   ██║ ╚═══██╗
██████╔╝██║     ██║ ╚═╝ ██║██║  ██║██║ ╚████║╚██████╔╝██████╔╝
╚═════╝ ╚═╝     ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚═════╝ 
```

**一个由 `bfmhno3` 整理、面向教育机器人竞赛复盘与学习的 ROS1 项目档案**

<p>
  <img src="https://img.shields.io/badge/ROS-ROS1-22314E?style=for-the-badge&logo=ros&logoColor=white" alt="ROS1">
  <img src="https://img.shields.io/badge/Ubuntu-18.04-E95420?style=for-the-badge&logo=ubuntu&logoColor=white" alt="Ubuntu 18.04">
  <img src="https://img.shields.io/badge/Python-2.7-3776AB?style=for-the-badge&logo=python&logoColor=white" alt="Python 2.7">
  <img src="https://img.shields.io/badge/Catkin-Workspace-0A7E8C?style=for-the-badge" alt="Catkin Workspace">
  <img src="https://img.shields.io/badge/Status-Learning%20Archive-6C63FF?style=for-the-badge" alt="Learning Archive">
</p>

<p>
  <img src="https://img.shields.io/badge/Docs-Chinese-1F6FEB?style=flat-square" alt="Chinese Docs">
  <img src="https://img.shields.io/badge/Focus-Competition%20Tuning-F59E0B?style=flat-square" alt="Competition Tuning">
  <img src="https://img.shields.io/badge/Scope-Driver%20to%20Mission-10B981?style=flat-square" alt="Driver to Mission">
  <img src="https://img.shields.io/badge/Note-Historical%20Code%20Included-8B5CF6?style=flat-square" alt="Historical Code Included">
</p>

[文档入口](docs/README.md) · [系统架构](docs/02-system-architecture/README.md) · [竞赛任务](docs/07-competition-tasks/README.md) · [实战手册](docs/08-competition-playbook/README.md)

</div>

---

> [!WARNING]
> ## 来源与分发风险提示
> 这个仓库中的代码并不是一个已明确授权公开分发的官方开源项目。相关源码在正常情况下需要通过原提供方人员单独获取。  
> 因此，这份仓库更适合被理解为**个人学习整理、竞赛复盘与技术笔记归档**，而不是带有明确再分发许可的公开发行版。  
> **如果有侵权，请联系删除**。

---

## 项目是什么

这是一套基于 **ROS1** 的教育机器人项目源码，面向麦克纳姆轮全向小车这类竞赛平台。仓库里同时包含：

- 底层硬件驱动：电机、里程计、IMU、串口协议
- 中层能力模块：姿态滤波、SLAM、定位、导航
- 上层任务逻辑：AR 标签、视觉追踪、多目标导航、射击控制、语音交互
- 配套学习文档：按“第一性原理 → 实际源码 → 调参与排障”组织

如果你来这里的目标是：

- 看懂比赛车到底怎么跑起来
- 找到哪些参数决定导航 / 追踪 / 发射行为
- 为后续调参、改任务流程、准备比赛做资料沉淀

那这个仓库就是为这件事服务的。

---

## 你会在这里看到什么

<table>
  <tr>
    <td width="33%">
      <h3>底层驱动</h3>
      <p>串口协议、底盘速度控制、里程计、IMU 数据链路。</p>
    </td>
    <td width="33%">
      <h3>导航与定位</h3>
      <p>建图、定位、代价地图、局部规划、EKF 相关链路。</p>
    </td>
    <td width="33%">
      <h3>竞赛任务层</h3>
      <p>任务状态机、多目标导航、视觉追踪、发射与语音交互。</p>
    </td>
  </tr>
</table>

---

## 仓库结构

```text
abot/
├── LICENSE
├── README.md
├── docs/
│   ├── README.md
│   ├── 01-ros-fundamentals/
│   ├── 02-system-architecture/
│   ├── 03-hardware-driver/
│   ├── 04-orientation-filter/
│   ├── 05-slam-navigation/
│   ├── 06-perception/
│   ├── 07-competition-tasks/
│   └── 08-competition-playbook/
└── src/
    ├── abot_base/
    │   ├── abot_bringup/
    │   ├── abot_imu/
    │   ├── abot_model/
    │   └── lidar_filters/
    ├── imu_filter/
    ├── robot_slam/
    ├── cam_track/
    ├── user_demo/
    ├── shoot_cmd/
    ├── abot_find/
    └── robot_voice/
```

### 重点包速览

| 包 / 目录 | 作用 |
|---|---|
| `src/abot_base/abot_bringup` | 底盘驱动、串口通信、里程计、基础 bringup |
| `src/abot_base/abot_imu` | IMU 原始数据读取与校准 |
| `src/imu_filter` | Mahony / Madgwick 姿态滤波 |
| `src/robot_slam` | 建图、定位、导航、多航点脚本 |
| `src/cam_track` | 视觉追踪与转向控制 |
| `src/user_demo` | 比赛主状态机与航点任务 |
| `src/shoot_cmd` | 发射机构控制与实验性控制中心 |
| `src/robot_voice` | 科大讯飞语音识别 / 语音合成接入 |
| `docs/` | 中文学习文档与竞赛理解手册 |

---

## 推荐阅读路线

### 如果你第一次接触这个项目

1. [`docs/README.md`](docs/README.md)
2. [`docs/01-ros-fundamentals/README.md`](docs/01-ros-fundamentals/README.md)
3. [`docs/02-system-architecture/README.md`](docs/02-system-architecture/README.md)
4. [`docs/03-hardware-driver/README.md`](docs/03-hardware-driver/README.md)
5. [`docs/05-slam-navigation/README.md`](docs/05-slam-navigation/README.md)
6. [`docs/07-competition-tasks/README.md`](docs/07-competition-tasks/README.md)

### 如果你更关心“怎么调参”

- [`docs/03-hardware-driver/motor-control.md`](docs/03-hardware-driver/motor-control.md)
- [`docs/05-slam-navigation/dwa-local-planner.md`](docs/05-slam-navigation/dwa-local-planner.md)
- [`docs/05-slam-navigation/competition-tuning.md`](docs/05-slam-navigation/competition-tuning.md)
- [`docs/08-competition-playbook/README.md`](docs/08-competition-playbook/README.md)

### 如果你更关心“比赛任务怎么串起来”

- [`docs/07-competition-tasks/mission-node.md`](docs/07-competition-tasks/mission-node.md)
- [`docs/07-competition-tasks/multi-goal-navigation.md`](docs/07-competition-tasks/multi-goal-navigation.md)
- [`docs/07-competition-tasks/shoot-control.md`](docs/07-competition-tasks/shoot-control.md)
- [`docs/07-competition-tasks/voice-interaction.md`](docs/07-competition-tasks/voice-interaction.md)

---

## 如何编译

### 0. 克隆仓库并切换分支

```bash
git clone https://github.com/bfmhno3/abot
cd abot
git checkout main
```

### 1. 准备工作空间

这个仓库当前已经是标准的 catkin 工作区形态：

```text
abot/
└── src/
```

也就是说，仓库根目录就是工作空间根目录。

### 2. 进入仓库根目录

```bash
cd /path/to/abot
```

### 3. 初始化 ROS 环境（可选）

通常需要先加载系统的 ROS1 环境：

```bash
source /opt/ros/<your-distro>/setup.bash
```

如果你已经有自己的工作环境脚本，也可以按你自己的习惯来。

如果已经使用鱼香肉丝的快速配置脚本安装好了 ROS1 Melodic，则无需手动进行初始化 ROS 环境。

### 4. 安装依赖

```bash
sudo apt install libalsa-ocaml-dev
sudo apt install ros-melodic-move-base
sudo apt install ros-melodic-ar-track-alvar-msgs
sudo cp /path/to/abot/src/robot_voice/libs/x64/libmsc.so /usr/lib/
```

### 5. 执行编译

> [!CAUTION]
>
> 不建议使用 `catkin_make` 方式编译，建议使用 `catkin build` 的方式进行编译，具体差异可以查看 [Catkin 工作空间与构建流程](./docs/01-ros-fundamentals/catkin-workspace-and-build.md)。

最基础的 catkin 编译方式：

```bash
catkin_make
```

如果你使用的是 `catkin_tools`，也可以按自己的习惯改成：

```bash
catkin build
```

### 5. 编译后加载工作空间环境

```bash
source devel/setup.bash
```

也可以将加载脚本写入 `.bashrc` 让 bash 启动时自动加载：

```bash
source /path/to/abot/devel/setup.bash
```

### 笔者示例

```bash
cd ~
git clone https://github.com/bfmhno3/abot
cd abot
git checkout main
sudo apt install libalsa-ocaml-dev
sudo apt install ros-melodic-move-base
sudo apt install ros-melodic-ar-track-alvar-msgs
sudo cp ~/abot/src/robot_voice/libs/x64/libmsc.so /usr/lib/
catkin build
```

第一次编译成功之后，应该得到类似以下输出：

<details>

```bash
---------------------------------------------------
Profile:                     default
Extending:             [env] /opt/ros/melodic
Workspace:                   /home/joe/abot
---------------------------------------------------
Build Space:        [exists] /home/joe/abot/build
Devel Space:        [exists] /home/joe/abot/devel
Install Space:      [unused] /home/joe/abot/install
Log Space:         [missing] /home/joe/abot/logs
Source Space:       [exists] /home/joe/abot/src
DESTDIR:            [unused] None
---------------------------------------------------
Devel Space Layout:          linked
Install Space Layout:        None
---------------------------------------------------
Additional CMake Args:       None
Additional Make Args:        None
Additional catkin Make Args: None
Internal Make Job Server:    True
Cache Job Environments:      False
---------------------------------------------------
Whitelisted Packages:        None
Blacklisted Packages:        None
---------------------------------------------------
Workspace configuration appears valid.

NOTE: Forcing CMake to run for each package.
---------------------------------------------------
[build] Found '11' packages in 0.0 seconds.                                                                                                 
[build] Updating package table.                                                                                                             
Starting  >>> catkin_tools_prebuild                                                                                                         
Finished  <<< catkin_tools_prebuild                [ 1.4 seconds ]                                                                          
Starting  >>> abot_imu                                                                                                                      
Starting  >>> abot_model                                                                                                                    
Starting  >>> cam_track                                                                                                                     
Starting  >>> filter                                                                                                                        
Finished  <<< abot_model                           [ 1.9 seconds ]                                                                          
Starting  >>> find_object_2d                                                                                                                

____________________________________________________________________________________________________________________________________________
Warnings   << find_object_2d:cmake /home/joe/abot/logs/find_object_2d/build.cmake.000.log                                                   
CMake Warning (dev) at /usr/src/googletest/CMakeLists.txt:3 (project):
  Policy CMP0048 is not set: project() command manages VERSION variables.
  Run "cmake --help-policy CMP0048" for policy details.  Use the cmake_policy
  command to set the policy and suppress this warning.

  The following variable(s) would be set to empty:

    PROJECT_VERSION
    PROJECT_VERSION_MAJOR
    PROJECT_VERSION_MINOR
    PROJECT_VERSION_PATCH
This warning is for project developers.  Use -Wno-dev to suppress it.

CMake Warning (dev) at /usr/src/googletest/googlemock/CMakeLists.txt:40 (project):
  Policy CMP0048 is not set: project() command manages VERSION variables.
  Run "cmake --help-policy CMP0048" for policy details.  Use the cmake_policy
  command to set the policy and suppress this warning.

  The following variable(s) would be set to empty:

    PROJECT_VERSION
    PROJECT_VERSION_MAJOR
    PROJECT_VERSION_MINOR
    PROJECT_VERSION_PATCH
This warning is for project developers.  Use -Wno-dev to suppress it.

CMake Warning (dev) at /usr/src/googletest/googletest/CMakeLists.txt:47 (project):
  Policy CMP0048 is not set: project() command manages VERSION variables.
  Run "cmake --help-policy CMP0048" for policy details.  Use the cmake_policy
  command to set the policy and suppress this warning.

  The following variable(s) would be set to empty:

    PROJECT_VERSION
    PROJECT_VERSION_MAJOR
    PROJECT_VERSION_MINOR
    PROJECT_VERSION_PATCH
This warning is for project developers.  Use -Wno-dev to suppress it.

cd /home/joe/abot/build/find_object_2d; catkin build --get-env find_object_2d | catkin env -si  /usr/bin/cmake /home/joe/abot/src/abot_find --no-warn-unused-cli -DCATKIN_DEVEL_PREFIX=/home/joe/abot/devel/.private/find_object_2d -DCMAKE_INSTALL_PREFIX=/home/joe/abot/install; cd -
............................................................................................................................................
Finished  <<< cam_track                            [ 5.7 seconds ]                                                                          
Finished  <<< abot_imu                             [ 6.9 seconds ]                                                                          
Finished  <<< filter                               [ 10.5 seconds ]                                                                         
Starting  >>> lidar_filters                                                                                                                 
Finished  <<< lidar_filters                        [ 2.1 seconds ]                                                                          
Starting  >>> robot_slam                                                                                                                    
Finished  <<< robot_slam                           [ 5.7 seconds ]                                                                          
Starting  >>> robot_voice                                                                                                                   
Starting  >>> shoot_cmd                                                                                                                     
Starting  >>> user_demo                                                                                                                     
Finished  <<< shoot_cmd                            [ 4.1 seconds ]                                                                          
Starting  >>> abot_bringup                                                                                                                  
____________________________________________________________________________________________________________________________________________
Warnings   << robot_voice:make /home/joe/abot/logs/robot_voice/build.make.000.log                                                           
cc1: warning: command line option ‘-std=c++11’ is valid for C++/ObjC++ but not for C
cc1: warning: command line option ‘-std=c++11’ is valid for C++/ObjC++ but not for C
cd /home/joe/abot/build/robot_voice; catkin build --get-env robot_voice | catkin env -si  /usr/bin/make --jobserver-fds=6,7 -j; cd -
............................................................................................................................................
Finished  <<< robot_voice                          [ 5.4 seconds ]                                                                          
Finished  <<< find_object_2d                       [ 26.5 seconds ]                                                                         
Finished  <<< user_demo                            [ 5.4 seconds ]                                                                          
Finished  <<< abot_bringup                         [ 6.2 seconds ]                                                                          
[build] Summary: All 12 packages succeeded!                                                                                                 
[build]   Ignored:   None.                                                                                                                  
[build]   Warnings:  2 packages succeeded with warnings.                                                                                    
[build]   Abandoned: None.                                                                                                                  
[build]   Failed:    None.                                                                                                                  
[build] Runtime: 34.4 seconds total.                                                                                                        
[build] Note: Workspace packages have changed, please re-source setup files to use them.
```

</details>

---

## 运行前你应该先知道的事

1. **这是 ROS1 项目**，不是 ROS2。
2. **很多运行路径默认面向 Linux / Ubuntu 环境**，不是 Windows 原生环境。
3. **仓库里存在历史方案并存的情况**，不是所有节点都处于同一成熟度。
4. **任务层最容易踩的坑往往不是算法，而是接口不统一**，例如 `/shoot` 字符串协议不一致。
5. **文档和源码要结合看**：文档负责帮你建立模型，源码负责给出最终真相。

---

## 这份仓库最适合怎么用

### 用法 A：当学习材料

从 `docs/` 开始读，建立整个系统从底层到比赛层的理解。

### 用法 B：当调参参考

一边看文档，一边对照 `src/` 里的真实参数文件、launch 文件和节点实现。

### 用法 C：当比赛维护手册

把你后续真正踩过的编译坑、运行坑、参数经验、比赛故障案例继续沉淀进文档。

---

## 文档入口

- 总文档入口：[`docs/README.md`](docs/README.md)
- 硬件驱动：[`docs/03-hardware-driver/README.md`](docs/03-hardware-driver/README.md)
- SLAM 与导航：[`docs/05-slam-navigation/README.md`](docs/05-slam-navigation/README.md)
- 竞赛任务：[`docs/07-competition-tasks/README.md`](docs/07-competition-tasks/README.md)
- 竞赛实战手册：[`docs/08-competition-playbook/README.md`](docs/08-competition-playbook/README.md)

---

## License

当前仓库根目录存在 [`LICENSE`](LICENSE)，但如果你准备公开发布整个项目，仍建议结合上面的“来源与分发风险提示”一起评估，不要只看许可证文件是否存在。

