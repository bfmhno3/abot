cmake_minimum_required(VERSION 3.10.2)
project(abot_model)

find_package(catkin REQUIRED)

catkin_package()

find_package(roslaunch)

foreach (dir config launch meshes urdf)
    install(DIRECTORY ${dir}/
        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/${dir})
endforeach (dir)
