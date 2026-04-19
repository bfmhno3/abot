# TODO

## 已确认问题

- [ ] 统一 `/shoot` 协议；当前 `cam_track` 发布的是 `"shoot!"`，与 `shoot_control.cpp` 只接受 `"shoot"` / `"stopshoot"` 的执行协议不兼容。
- [ ] 清理 `pointToar()` 中遗留的 `//TODO: odom twist angle z < 0.000x`，明确真正的停稳判据并补上实现。
- [ ] 复核 `package.xml` 依赖声明；当前只声明了部分消息/TF 依赖，需确认是否缺少 `roscpp`、`nav_msgs`、`ar_track_alvar_msgs` 等直接依赖，避免 isolated build 失败。
- [ ] 清理 `package.xml` 中的占位元数据（如 `<license>TODO</license>`、占位 maintainer 邮箱）。

## 后续可补充

- [ ] 复核 `cam_track` 的状态推进条件是否会在标签丢失、角速度噪声较大时误触发射击。
