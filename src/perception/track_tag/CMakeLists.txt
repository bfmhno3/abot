cmake_minimum_required(VERSION 3.10.2)
project(track_tag)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(ar_track
  src/ar_track.cpp
)
target_compile_features(ar_track PRIVATE cxx_std_11)
target_include_directories(ar_track PRIVATE
    include
    ${catkin_INCLUDE_DIRS}
)
add_dependencies(ar_track
    ${${PROJECT_NAME}_EXPORTED_TARGETS}
    ${catkin_EXPORTED_TARGETS}
)
target_link_libraries(ar_track PRIVATE
  ${catkin_LIBRARIES}
)

