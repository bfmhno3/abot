cmake_minimum_required(VERSION 3.10.2)
project(user_demo)

find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    actionlib_msgs
    geometry_msgs
    std_msgs
    tf
    tf2_ros
    tf_conversions
    nav_msgs
)
find_package(Eigen3 REQUIRED)

catkin_package(
    INCLUDE_DIRS include
    LIBRARIES cam_track
    CATKIN_DEPENDS actionlib_msgs geometry_msgs std_msgs tf tf2_ros
    #  DEPENDS system_lib
)

add_executable(mission_node src/mission_node.cpp)
target_compile_features(mission_node PRIVATE cxx_std_11)
target_include_directories(mission_node PRIVATE
    include
    ${catkin_INCLUDE_DIRS}
)
target_link_libraries(mission_node
    ${catkin_LIBRARIES}
)

