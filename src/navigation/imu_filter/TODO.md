# TODO

## 已确认问题

- [ ] 统一包名；目录名是 `imu_filter`，但 `package.xml` / `CMakeLists.txt` 当前项目名却是 `filter`，容易导致构建、运行和文档定位混乱。
- [ ] 清理 `package.xml` 中的占位元数据（如 `<license>TODO</license>`、占位 maintainer 邮箱）。

## 后续可补充

- [ ] 复核 dynamic_reconfigure 配置、节点命名和 launch 使用方式，确认外部节点不会因为包名不一致而加载失败。
- [ ] 为 Mahony / Madgwick / bias_calculator 三个可执行文件补充最小可运行说明与参数约束。
