#!/bin/bash
ROS_SETUP=${ROS_SETUP:-/opt/ros/melodic/setup.bash}
WORKSPACE_SETUP=${WORKSPACE_SETUP:-$(dirname "$0")/../../../devel/setup.bash}
. "$ROS_SETUP"
if [ -f "$WORKSPACE_SETUP" ]; then
  . "$WORKSPACE_SETUP"
fi
gnome-terminal --window -e 'bash -c "roscore; exec bash"' \
--tab -e 'bash -c "sleep 2; roslaunch usb_cam usb_cam.launch; exec bash"' \
--tab -e 'bash -c "sleep 2; roslaunch tracker_pkg kcf_tracker.launch; exec bash"' \
--tab -e 'bash -c "sleep 2; roslaunch tracker_pkg follower.launch; exec bash"' \
