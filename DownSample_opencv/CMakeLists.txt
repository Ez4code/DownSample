cmake_minimum_required(VERSION 3.21)
project(DownSample)

set(CMAKE_CXX_STANDARD 11)
set(INCLUDE "../include")
include_directories( ${INCLUDE} )

find_package( OpenCV REQUIRED )


add_executable(DownSample TestBench.cpp)
target_link_libraries( DownSample ${OpenCV_LIBS} )
