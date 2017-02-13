cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_library(decode-mp3-ogg-jni SHARED
            decode-mp3-ogg-jni.cpp)

target_link_libraries(decode-mp3-ogg-jni
                      android
                      log
                      OpenSLES)
