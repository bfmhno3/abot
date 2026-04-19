# 发射机构控制

> 真正把 “打靶” 从软件命令变成硬件动作的最后一跳

## 第一性原理

上层节点无论多聪明，最终都必须把“发射”这个动作变成底层设备能理解的协议。

在 abot 里，这件事由 `shoot_cmd` 包中的 `shoot_control.cpp` 负责。

它的职责非常简单：

1. 订阅 `/shoot`
2. 根据字符串命令判断是发射还是停止
3. 向串口 `/dev/shoot` 发送固定字节帧

也就是说，它本质上是一个**命令适配器**：

```text
上层业务字符串  →  串口字节协议  →  发射机构硬件动作
```

源码位置：

- `src/shoot_cmd/src/shoot_control.cpp`

---

## 节点如何工作

### 串口初始化

主函数里会：

- 初始化 ROS 节点 `shoot_control`
- 订阅 `/shoot`
- 打开 `/dev/shoot`
- 以 `9600` 波特率通信

关键代码：

- `src/shoot_cmd/src/shoot_control.cpp:64-76`

其中：

```cpp
sprintf(portname,"/dev/shoot");
sc.init(portname, 9600);
```

说明这个节点依赖一个固定的 Linux 串口设备。

对于你现在的 Windows 学习环境，这个节点当然跑不起来，但这不影响你理解：

- 上层命令是怎么进来的
- 串口帧是怎么发出去的
- 为什么协议不一致会直接导致比赛失败

---

## `/shoot` 话题接受什么命令

核心回调：

```cpp
void shoot_cb(const std_msgs::String::ConstPtr& msg)
{
    if (msg->data == "shoot")
    {
        sc.shoot();
    }
    else if (msg->data == "stopshoot")
    {
        sc.stop_shoot();
    }
}
```

位置：

- `src/shoot_cmd/src/shoot_control.cpp:52-61`

这说明当前执行层认可的协议只有两个：

| 字符串 | 含义 |
|--------|------|
| `shoot` | 发射 |
| `stopshoot` | 停止发射 |

这很重要，因为它定义了整个比赛系统最后一级的“真协议”。

---

## 发射与停止对应的串口帧

### 发射帧

```cpp
char buf[8] = {0x55, 0x01, 0x12, 0x00, 0x00, 0x00, 0x01, 0x69};
```

位置：

- `src/shoot_cmd/src/shoot_control.cpp:38-42`

### 停止帧

```cpp
char buf[8] = {0x55, 0x01, 0x11, 0x00, 0x00, 0x00, 0x01, 0x68};
```

位置：

- `src/shoot_cmd/src/shoot_control.cpp:44-48`

这两个帧非常像一个小型串口协议：

- `0x55` 像帧头
- 后面几个字节像命令字和载荷
- 最后一个字节像校验/结束相关字段

在竞赛系统里，这种设计很常见：

> 上位机只负责发很少几种指令，下位机收到后自己去驱动电机/舵机/发射机构。

---

## 为什么这个节点看起来很简单，却很关键

因为它是整条打靶链路里**最末端、也是最绝对的一层**。

假设上层都对了：

- 导航到了点
- AR 对准成功
- `/shoot` 确实有消息

但只要这里：

- 串口设备名错了
- 波特率错了
- 协议字符串不一致
- 硬件帧不对

结果依然是：

> 场上不发射。

所以它虽然代码量很少，却是比赛链路里最不该忽视的地方之一。

---

## 当前仓库里的协议不一致问题

这是整个项目里最值得警惕的问题之一。

### `mission_node` 发的不是 `shoot`

它发的是：

- `"1"`

位置：

- `src/user_demo/include/user_demo/mission_node.hpp:133`
- `src/user_demo/include/user_demo/mission_node.hpp:169`
- `src/user_demo/include/user_demo/mission_node.hpp:205`

### `cam_track` 发的也不是 `shoot`

它发的是：

- `"shoot!"`

位置：

- `src/cam_track/include/cam_track/cam_track_node.hpp:242-244`

### 但 `shoot_control` 只认：

- `"shoot"`
- `"stopshoot"`

位置：

- `src/shoot_cmd/src/shoot_control.cpp:52-60`

所以：

> [!WARNING]
> 当前仓库里，上层多个节点和执行层的 `/shoot` 字符串协议并不统一。

这会直接导致一个典型比赛症状：

- 上层逻辑以为“我已经发射了”
- 实际发射机构完全没收到正确命令

---

## 为什么仓库里还会并存另一个 shoot.py

在 `abot_bringup` 里还有一个 Python 脚本：

- `src/abot_base/abot_bringup/scripts/shoot.py`

它的行为很不同：

- 也是订阅 `/shoot`
- 但**不看字符串内容**
- 只要收到消息，就直接发“发射帧”，随后发“停止帧”

关键位置：

- `src/abot_base/abot_bringup/scripts/shoot.py:17-26`

这意味着仓库里至少存在过两种执行层思路：

### 方案 A：严格协议型

`shoot_control.cpp`

- 只有 `shoot` / `stopshoot` 才执行
- 接口更清晰
- 但要求上层统一协议

### 方案 B：宽松触发型

`shoot.py`

- 收到任何 `/shoot` 消息都打
- 更宽容
- 但语义更模糊

这很像项目演化过程中留下来的两套方案。

从工程角度看，这并不罕见，但从比赛角度看，危险在于：

> 你必须搞清楚赛场上到底启的是哪一套。

---

## 如果你在比赛中遇到“对准了但不打”，最该排查什么

按优先级建议看：

1. `/shoot` 上到底发了什么字符串
2. 当前运行的是 `shoot_control.cpp` 还是 `shoot.py`
3. `/dev/shoot` 是否存在
4. 波特率和硬件协议是否匹配
5. 串口帧发出去后硬件是否响应

这是一个非常典型的“接口链路”排查问题，而不是“算法问题”。

---

## 控制中心 control_center 是什么

在 `shoot_cmd` 里还有一个 `control_center.cpp`：

- `src/shoot_cmd/src/control_center.cpp`

它看起来是另一套更高层的比赛控制尝试，包含：

- 航点 A/B/C/D/E
- 语音触发 `/snowman/ask`
- `/move_base/status`
- `/ar_pose_marker`

但从代码状态看，它明显不如 `mission_node` 成熟：

- 参数读取有混写
- 变量名引用错误
- 逻辑未完整闭合

所以更合理的理解是：

> `control_center` 像一份实验性/未完成的替代任务控制器，而不是当前主流程。

这也再次说明：

- 这个仓库并不是只有一条单一演化路线
- 竞赛项目里“并存旧方案”和“半完成方案”很常见

---

## 一句话总结

> `shoot_control` 是比赛任务真正落到硬件上的最后一跳，它不是复杂模块，但它的协议必须和上层完全一致。

如果说 `mission_node` 负责“决定什么时候打”，那 `shoot_control` 负责“保证这一下真的打出去”。