# TODO

## 已确认问题

- [ ] 统一 `/shoot` 执行协议；当前 `shoot_control.cpp` 只接受 `"shoot"` / `"stopshoot"`，但上游 `mission_node` 发 `"1"`，
  `cam_track` 发 `"shoot!"`，整条链路默认不兼容。
- [ ] 把 `/dev/shoot` 串口设备改成参数化配置，避免只能绑定固定 Linux 设备路径。
- [ ] 去掉 `sprintf(portname, "/dev/shoot")` 这类不必要的 C 风格写法，改用更直接、更安全的实现。
- [ ] 整理或重写 `control_center.cpp`；当前文件已确认存在参数读取拷贝错误、订阅类型与回调签名不一致、目标点变量混用、语音触发过于粗暴等问题，不能视为稳定实现。
- [ ] 清理 `package.xml` 中的占位元数据（如 `<license>TODO</license>`、占位 maintainer 邮箱）。

## 后续可补充

- [ ] 为发射串口协议补充最小测试工具或仿真桩，降低现场排障成本。
