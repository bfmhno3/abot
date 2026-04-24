cmake_minimum_required(VERSION 3.10.2)
project(shoot_cmd)

find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    std_msgs
)

catkin_package(
    CATKIN_DEPENDS roscpp rospy std_msgs
)

add_library(SerialPort
    src/SerialPort.cpp
)
target_compile_features(SerialPort PRIVATE cxx_std_11)
target_include_directories(SerialPort PRIVATE
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(SerialPort ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(SerialPort PRIVATE
    ${catkin_LIBRARIES}
)

add_executable(shoot_control
    src/shoot_control.cpp
)
target_compile_features(shoot_control PRIVATE cxx_std_11)
target_include_directories(shoot_control PRIVATE
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(shoot_control ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(shoot_control PRIVATE
    ${catkin_LIBRARIES}
    SerialPort
)
