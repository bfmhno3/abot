cmake_minimum_required(VERSION 3.10.2)
project(abot_bringup)

find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    std_msgs
    tf
    dynamic_reconfigure
    abot_imu
)

generate_dynamic_reconfigure_options(
    cfg/abot_parameter.cfg
)

catkin_package()

add_executable(abot_driver
    src/main.cpp
    src/base_driver_config.cpp
    src/base_driver.cpp
    src/data_holder.cpp
    src/simple_dataframe_master.cpp
    src/serial_transport.cpp
)
target_compile_features(abot_driver PRIVATE cxx_std_11)
target_include_directories(abot_driver PRIVATE
    include
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(abot_driver
    ${${PROJECT_NAME}_EXPORTED_TARGETS}
    ${catkin_EXPORTED_TARGETS}
)
target_link_libraries(abot_driver PRIVATE
    ${catkin_LIBRARIES}
)

