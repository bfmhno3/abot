cmake_minimum_required(VERSION 3.10.2)
project(lidar_follower)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  std_msgs
  message_generation
)

add_message_files(
   FILES
   position.msg
 )

generate_messages(
  DEPENDENCIES
   sensor_msgs
   std_msgs
 )

catkin_package(
  CATKIN_DEPENDS message_runtime roscpp rospy sensor_msgs std_msgs
)
