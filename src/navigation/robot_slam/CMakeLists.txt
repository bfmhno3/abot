cmake_minimum_required(VERSION 3.10.2)
project(robot_slam)

find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    std_msgs
    actionlib_msgs
    geometry_msgs
    nav_msgs
    tf
    tf2_ros
    tf_conversions
)

catkin_package(
    INCLUDE_DIRS include
    LIBRARIES robot_slam
    CATKIN_DEPENDS roscpp rospy std_msgs
    #  DEPENDS system_lib
)

add_executable(navigate_node
    src/navigate.cpp
)
target_compile_features(navigate_node PRIVATE cxx_std_11)
target_include_directories(navigate_node PRIVATE
    ${catkin_INCLUDE_DIRS}
    include
)
target_link_libraries(navigate_node PRIVATE
    ${catkin_LIBRARIES}
)

