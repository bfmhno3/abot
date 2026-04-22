# TODO

## 已确认问题

- [ ] 修复 `read_param()` 先读下位机参数、随后又被硬编码默认值覆盖的问题，避免真实参数与 dynamic_reconfigure 修改在节点重启后失效。
- [ ] 把串口、轮径、轮距、PID、IMU 类型等默认值改为从 ROS 参数或配置文件读取，而不是在 `base_driver.cpp` 中写死。
- [ ] 检查 `BaseDriver::~BaseDriver()` 中对 `instance` 的释放逻辑，避免析构时再次 `delete instance` 导致自删/重复释放风险。
- [ ] 评估 `work_loop()` 以 1000 Hz 持续同步请求串口数据的设计，确认是否存在通信拥塞、阻塞累积或 CPU 占用过高的问题。
- [ ] 为 `cmd_vel_callback()` 中的高频 `ROS_INFO_STREAM` 增加降频或调试开关，避免运行时刷屏影响调试与性能。
- [ ] 清理 `package.xml` 中的占位元数据（如 `<license>TODO</license>`、占位 maintainer 邮箱）。

## 后续可补充

- [ ] 审查 `abot_bringup` 的 launch/params 默认值与实际硬件是否一致。
- [ ] 审查 `shoot.py` 与 `shoot_control.cpp` 的职责边界，避免执行链路重复。
