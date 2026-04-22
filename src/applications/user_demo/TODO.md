# TODO

## 已确认问题

- [ ] 统一 `/shoot` 协议；当前 `mission_node` 向 `/shoot` 发布的是字符串 `"1"`，与执行层 `shoot_control.cpp` 只接受
  `"shoot"` / `"stopshoot"` 不兼容。
- [ ] 修复 `satfunc()` 返回值未重新赋回 `vel_x` 的问题；当前代码虽然调用了限幅/死区函数，但控制整形实际上没有生效。
- [ ] 修复 `marker_found` 没有在“当前帧未找到目标标签”时可靠清零的问题，避免状态残留导致任务状态机误判。
- [ ] 复核任务状态机对 `move_base` 成功状态与视觉回调时序的依赖，避免 `reach_sign` 与视觉状态不同步造成卡死或误射击。
- [ ] 清理 `package.xml` 中的占位元数据（如 `<license>TODO</license>`、占位 maintainer 邮箱）。

## 后续可补充

- [ ] 评估是否把布尔标志位状态机改成显式枚举状态，降低后续继续扩展航点/射击逻辑时的维护难度。
