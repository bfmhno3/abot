# TODO

## 已确认问题

- [ ] 去掉 `voice_assistant.cpp` 中硬编码的讯飞 `appid`，改用配置或环境变量，避免敏感配置直接写进源码。
- [ ] 去掉对 `/tmp/cmd` 和 `mplayer` 的硬编码依赖，改成可配置播放器/播放后端，降低环境耦合。
- [ ] 修复时间播报分支里 `string.copy(timeString, sizeof(string), 0)` 的用法问题；这里既没有显式补 `\0`，又错误使用了
  `sizeof(string)`，存在文本截断或未终止字符串风险。
- [ ] 清理 CMake 中对 `libmsc.so` 的裸链接假设，改成更明确的库查找或安装指引，避免必须手工复制到系统库目录。
- [ ] 清理 `package.xml` 中的占位元数据（如 `<license>TODO</license>`、占位 maintainer 邮箱）。

## 后续可补充

- [ ] 评估 `voice_assistant`、`tts_subscribe`、`iat_publish` 三个节点的职责是否需要进一步收敛，减少重复播报链路。
