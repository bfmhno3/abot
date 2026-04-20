cmake_minimum_required(VERSION 3.10.2)
project(abot_imu)

find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    std_msgs
    geometry_msgs
    message_generation
    tf
)

add_message_files(FILES
    RawImu.msg
)
generate_messages(DEPENDENCIES std_msgs geometry_msgs)

catkin_package(CATKIN_DEPENDS message_runtime std_msgs geometry_msgs)

add_executable(abot_imu
    src/abot_imu.cpp
    src/abot_imu_node.cpp
)
target_compile_features(abot_imu PRIVATE cxx_std_11)
target_include_directories(abot_imu PRIVATE
    include
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(abot_imu ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(abot_imu PRIVATE
    ${catkin_LIBRARIES}
)

