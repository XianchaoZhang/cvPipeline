cmake_minimum_required(VERSION 3.0.0)
project(main)
include_directories(D:\\code\\opencv-build\\x64\\MinGW\\install\\include)
set(SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp)
link_directories(D:\\code\\opencv-build\\x64\\MinGW\\install\\x64\\mingw\\lib)
link_libraries(libopencv_img_hash450.dll.a)
#link_libraries()

add_executable(main src/main.cpp)