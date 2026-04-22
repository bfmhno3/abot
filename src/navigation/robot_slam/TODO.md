# TODO

## 已确认问题

- [ ] 修复 `navigation_multi_goals.py` 的缩进与脚本风格问题；当前文件混用 tab / 空格，存在 Python 2 环境下可读性差、移植性差的问题。
- [ ] 去掉 `raw_input()` 这种交互式阻塞启动方式，改成 launch / 参数驱动或明确的服务触发，避免节点无法自动化启动。
- [ ] 清理 `reload(sys)` 与 `sys.setdefaultencoding('utf-8')` 这类 Python 2 时代写法，避免后续迁移与维护困难。
- [ ] 修复 `catkin_package(LIBRARIES robot_slam)` 与实际未定义同名库之间的不一致，避免构建元数据误导。
- [ ] 清理 `package.xml` 中的占位元数据（如 `<license>TODO</license>`、占位 maintainer 邮箱）。
- [ ] 修正文案与日志中的拼写错误（如 `arrived to traget point`），避免影响调试输出与用户理解。

## 后续可补充

- [ ] 评估多航点脚本是否应拆成“区域识别状态机 + move_base action 封装”，降低全局变量耦合。
