cmake_minimum_required(VERSION 3.10.2)
project(filter)

find_package(catkin REQUIRED COMPONENTS
    dynamic_reconfigure
    geometry_msgs
    roscpp
    rospy
    sensor_msgs
    tf
)

generate_dynamic_reconfigure_options(
    cfg/MyStuff.cfg
    cfg/MyStuff2.cfg
)

catkin_package()

add_executable(Mahony_filter
    src/Mahony_filter.cpp
)
target_compile_features(Mahony_filter PRIVATE cxx_std_11)
target_include_directories(Mahony_filter PRIVATE
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(Mahony_filter ${PROJECT_NAME}_gencfg)
target_link_libraries(Mahony_filter PRIVATE
    ${catkin_LIBRARIES}
)

add_executable(Madgwick_filter
    src/Madgwick_filter.cpp
)
target_compile_features(Madgwick_filter PRIVATE cxx_std_11)
target_include_directories(Madgwick_filter PRIVATE
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(Madgwick_filter ${PROJECT_NAME}_gencfg)
target_link_libraries(Madgwick_filter PRIVATE
    ${catkin_LIBRARIES}
)

add_executable(bias_calculator
    src/bias_calculator.cpp
)
target_compile_features(bias_calculator PRIVATE cxx_std_11)
target_include_directories(bias_calculator PRIVATE
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(bias_calculator ${PROJECT_NAME}_gencfg)
target_link_libraries(bias_calculator PRIVATE
    ${catkin_LIBRARIES}
)
