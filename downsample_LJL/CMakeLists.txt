cmake_minimum_required(VERSION 3.15)
project(Fast_detect3)

set(CMAKE_CXX_STANDARD 11)
set(INCLUDE
        ../include
        )

include_directories( ${INCLUDE} )

set(SOURCES
        main.cpp
        fast.cpp
        detect.cpp
        )

add_executable(fast_detect ${SOURCES})

FIND_PACKAGE(OpenCV REQUIRED)
IF (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    target_link_libraries(fast_detect ${OpenCV_LIBS})
ELSE(OpenCV_FOUND)
    MESSAGE(FATAL_ERROR "OpenCV library not found")
ENDIF (OpenCV_FOUND)
