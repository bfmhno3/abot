cmake_minimum_required(VERSION 3.10.2)
project(abot_base)

find_package(catkin REQUIRED COMPONENTS
  actionlib
  geometry_msgs
  move_base_msgs
  roscpp
  rospy
  tf
  visualization_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES abot_base
#  CATKIN_DEPENDS actionlib geometry_msgs move_base_msgs roscpp tf visualization_msgs
#  DEPENDS system_lib
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

