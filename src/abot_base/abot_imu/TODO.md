# TODO

## 已确认问题

- [ ] 清理 `package.xml` 中的占位元数据（如 `<license>TODO</license>`、占位 maintainer 邮箱）。
- [ ] 明确 IMU 校准参数的持久化策略，确认默认 `perform_calibration` 行为是否符合实际部署预期。
- [ ] 复查磁力计发布类型在 `geometry_msgs/Vector3Stamped` 与 `sensor_msgs/MagneticField` 之间切换时，下游滤波器配置是否会因此失配。

## 后续可补充

- [ ] 补充针对校准流程、偏置保存与重载的回归测试或最小复现说明。
