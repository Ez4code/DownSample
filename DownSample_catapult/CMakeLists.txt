cmake_minimum_required(VERSION 3.21)
project(DownSample)


set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)


set(INCLUDE
        "../include"
        "../include/bmpUtil"
        )
include_directories( ${INCLUDE} )

set(SOURCE
        "TestBench.cpp"
        "class_hier_design.cpp"
        "../include/bmpUtil/bmp_io.cpp"
        "../include/ccs_bmpfile.cpp"
        )

find_package( OpenCV REQUIRED )

add_executable(DownSample ${SOURCE})
target_link_libraries( DownSample ${OpenCV_LIBS} )


