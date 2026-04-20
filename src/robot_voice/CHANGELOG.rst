^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package robot_voice
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Forthcoming
-----------
* build(robot_voice): 分别配置 C 和 C++ 的编译标准以消除清高
  - 移除了 CMakeLists.txt 中导致警告的全局 -std=c++11 编译选项
  - 使用 CMAKE_CXX_STANDARD 为 C++ 代码独立设置 C++11 标准
  - 使用 CMAKE_C_STANDARD 为 C 代码独立设置 C11 标准
  - 解决 cc1 编译器在处理 linuxrec.c 时抛出的无效命令选项警告
* fix(robot_voice): 修复 linuxrec 编译时 pthread 类型未定义的报错
  - 在 linuxrec.h 中补充了缺失的 <pthread.h> 等标准库头文件，确保头文件自包含
  - 调整了 linuxrec.h 中的头文件包含顺序，确保系统标准库在自定义头文件之前被解析
  - 解决 ROS Melodic 环境下 catkin build 抛出的 "unknown type name" 编译中断问题
* style: 使用 clang-format 和 CLion 格式化所有代码
* style: 使用 clang-format 和 CLion 格式化所有代码
* docs: 添加代码未完成任务文档
* feat: 添加立创鑫诚源代码
* Contributors: bfmhno3
