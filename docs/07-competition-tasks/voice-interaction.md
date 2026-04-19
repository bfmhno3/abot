# 语音交互

> 在比赛系统里，语音不只是 “好玩”，它往往承担触发、播报和人机交互入口的角色

## 第一性原理

机器人语音系统通常分三步：

1. **听懂你说了什么**（语音识别）
2. **决定该怎么回应**（文本逻辑）
3. **把回应再说出来**（语音合成）

在 abot 里，这三件事分别对应不同节点：

- `iat_publish.cpp`：负责把识别结果发布成文本
- `tts_subscribe.cpp` / `voice_assistant.cpp`：负责把文本播报出来
- 其他任务控制节点：消费语音结果来触发比赛逻辑

也就是说，这层的本质不是“语音 AI”，而是：

> 把人类口头指令接入 ROS 话题系统。

---

## 当前仓库里最核心的语音节点

### 1. `voice_assistant.cpp`

位置：

- `src/robot_voice/src/voice_assistant.cpp`

它的主要职责是：

- 订阅 `voiceWords`
- 根据收到的文本生成回应文本
- 调用讯飞 TTS 合成 wav 文件
- 用 `mplayer` 播放合成结果

### 2. `tts_subscribe.cpp`

位置：

- `src/robot_voice/src/tts_subscribe.cpp`

从命名和 grep 结果看，它也是订阅 `voiceWords` 做播报，属于另一种 TTS 使用方式。

### 3. `iat_publish.cpp`

位置：

- `src/robot_voice/src/iat_publish.cpp`

它会发布：

- `voiceWords`

这意味着它承担的是 “听写/语音识别结果转 ROS 文本”的角色。

---

## `voiceWords` 话题在系统中的位置

这是语音层最重要的公共接口。

当前可以看到：

- `iat_publish.cpp` 发布 `voiceWords`
- `voice_assistant.cpp` 订阅 `voiceWords`
- `tts_subscribe.cpp` 订阅 `voiceWords`
- `navigation_multi_goals.py` 也会向 `/voiceWords` 发布到点播报

相关位置：

- `src/robot_voice/src/iat_publish.cpp:132`
- `src/robot_voice/src/voice_assistant.cpp:246`
- `src/robot_voice/src/tts_subscribe.cpp:215`
- `src/robot_slam/scripts/navigation_multi_goals.py:37-38`

这说明 `voiceWords` 在这个项目里并不只是“用户输入”，它实际上兼任了：

- 语音识别输出
- 系统播报输入
- 任务状态提示通道

换句话说，它更像一个 “语音文本总线”。

---

## `voice_assistant.cpp` 是怎么工作的

### 1. 订阅文字

主函数里：

```cpp
ros::Subscriber sub = n.subscribe("voiceWords", 1000, voiceWordsCallback);
```

位置：

- `src/robot_voice/src/voice_assistant.cpp:244-247`

### 2. 根据文本决定回复内容

`voiceWordsCallback()` 里针对一些固定句子做了分支：

- `你是谁？`
- `你可以做什么？`
- `现在时间。`

对应位置：

- `src/robot_voice/src/voice_assistant.cpp:157-188`

这说明它不是大模型对话，而是：

> 固定规则问答 + 文本转语音。

### 3. 调用讯飞 TTS 生成 wav

关键函数：

- `text_to_speech()`

位置：

- `src/robot_voice/src/voice_assistant.cpp:58-138`

它会：

1. 登录讯飞 SDK
2. 把文本送进 TTS 会话
3. 获取合成音频
4. 写成 `tts_sample.wav`

### 4. 调用播放器播放

最后：

```cpp
popen("mplayer -quiet -slave -input file=/tmp/cmd 'tts_sample.wav'","r");
```

位置：

- `src/robot_voice/src/voice_assistant.cpp:200-203`

所以这整个节点本质上是：

```text
ROS 文本消息 → 讯飞 TTS → 本地 wav → mplayer 播放
```

---

## 讯飞 SDK 在这里扮演什么角色

`voice_assistant.cpp` 明显依赖讯飞语音云 SDK：

- `QTTSSessionBegin`
- `QTTSTextPut`
- `QTTSAudioGet`
- `MSPLogin`
- `MSPLogout`

位置：

- `src/robot_voice/src/voice_assistant.cpp:79-131`
- `src/robot_voice/src/voice_assistant.cpp:229-236`

登录参数里甚至直接写了：

```cpp
const char* login_params = "appid = 594a7b46, work_dir = .";
```

位置：

- `src/robot_voice/src/voice_assistant.cpp:216`

> [!WARNING]
> 这属于硬编码第三方服务配置。对于教学理解没问题，但从工程和安全角度看，这是很不理想的做法。

---

## 语音在比赛里怎么接任务逻辑

仓库里可以看到另一条语音输入链：

- `control_center.cpp` 订阅 `/snowman/ask`
- 只要收到消息，就直接把 `start_move = true`

位置：

- `src/shoot_cmd/src/control_center.cpp:29`
- `src/shoot_cmd/src/control_center.cpp:75-85`

这说明语音不只是拿来播报，也曾被用作：

- 启动比赛流程
- 人工触发导航开始

不过这里要格外谨慎理解：`control_center.cpp` 当前更像未整理完成的实验脚本，而不是稳定任务节点。文件里还能看到订阅类型与回调签名不一致、参数读取拷贝错误、目标点变量混用等问题，所以更适合把它当成 “曾尝试过的语音触发入口”，而不是推荐直接复用的正式方案。

---

## `voiceWords` 与比赛播报

`navigation_multi_goals.py` 里：

```python
self.arrive_pub = rospy.Publisher('/voiceWords', String, queue_size=10)
```

并且在导航完成时发布：

- `arrived to traget point`

位置：

- `src/robot_slam/scripts/navigation_multi_goals.py:37`
- `src/robot_slam/scripts/navigation_multi_goals.py:88-92`

这说明语音系统在比赛里还承担了一个很实用的角色：

- 把内部状态说给人听
- 方便操作者知道当前流程跑到哪一步

也就是说，它不仅是交互入口，也是 “可听化日志系统”。

---

## 这个模块最值得注意的现实问题

### 1. 强依赖 Linux 本地环境

例如：

- `mplayer`
- `/tmp/cmd`
- 讯飞 SDK 本地库

这些都说明它强依赖部署机环境。

### 2. 不是通用语音助手，而是规则型语音节点

它适合：

- 固定问答
- 状态播报
- 简单触发

不适合：

- 开放域对话
- 复杂语义理解

### 3. 语音结果本质上仍然只是字符串消息

在 ROS 系统里，它最后还是变成了：

- 某个 topic 上的一段文本

所以你调试语音系统时，最核心的其实不是“声音好不好听”，而是：

- 文本有没有正确发布
- 播报节点有没有正确订阅
- 上层任务节点有没有按字符串做出正确响应

---

## 一句话总结

> abot 的语音层不是独立智能体，而是把“听到的话”和“系统想说的话”都接进 ROS 话题系统的一层桥接。

它在比赛里的价值，不在炫技，而在触发、播报和流程可观测性。