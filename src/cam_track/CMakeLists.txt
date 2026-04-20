cmake_minimum_required(VERSION 3.10.2)
project(cam_track)

find_package(catkin REQUIRED COMPONENTS
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

include_directories(
    include
    ${catkin_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}_node
    src/cam_track_node.cpp
)
target_compile_features(${PROJECT_NAME}_node PRIVATE cxx_std_14)
target_include_directories(${PROJECT_NAME}_node PRIVATE
    include
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(${PROJECT_NAME}_node
    ${${PROJECT_NAME}_EXPORTED_TARGETS}
    ${catkin_EXPORTED_TARGETS}
)
target_link_libraries(${PROJECT_NAME}_node PRIVATE
    ${catkin_LIBRARIES}
)

