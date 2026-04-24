cmake_minimum_required(VERSION 3.10.2)
project(robot_voice)

find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    std_msgs
)

catkin_package()

set(ROBOT_VOICE_COMMON_LIBRARIES
    ${catkin_LIBRARIES}
    libmsc.so
    -ldl
    -pthread
)

add_executable(tts_subscribe
    src/tts_subscribe.cpp
)
target_compile_features(tts_subscribe PRIVATE cxx_std_11)
target_include_directories(tts_subscribe PRIVATE
    ${catkin_INCLUDE_DIRS}
    include
)
target_link_libraries(tts_subscribe PRIVATE
    ${ROBOT_VOICE_COMMON_LIBRARIES}
)

add_executable(iat_publish
    src/iat_publish.cpp
    src/speech_recognizer.c
    src/linuxrec.c
)
target_compile_features(iat_publish PRIVATE cxx_std_11)
set_source_files_properties(
    src/speech_recognizer.c
    src/linuxrec.c
    PROPERTIES LANGUAGE C
)
target_include_directories(iat_publish PRIVATE
    ${catkin_INCLUDE_DIRS}
    include
)
target_link_libraries(iat_publish PRIVATE
    ${ROBOT_VOICE_COMMON_LIBRARIES}
    -lm
    -lrt
    -lasound
)

add_executable(voice_assistant
    src/voice_assistant.cpp
)
target_compile_features(voice_assistant PRIVATE cxx_std_11)
target_include_directories(voice_assistant PRIVATE
    ${catkin_INCLUDE_DIRS}
    include
)
target_link_libraries(voice_assistant PRIVATE
    ${ROBOT_VOICE_COMMON_LIBRARIES}
)
