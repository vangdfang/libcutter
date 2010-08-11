project (libcutter)
cmake_minimum_required(VERSION 2.6.4)
    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)
include_directories(${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/include/pub)

find_library(HAS_OPENCV NAMES cv highgui)

set(cutter_files
    serial_port.cpp
    device.cpp
    device_c.cpp
    btea.c
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(REMOVE_AT cutter_files list(FIND serial_port.cpp))
    list(APPEND cutter_files serial_port_win32.cpp)
endif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

if(HAS_OPENCV)
    list(APPEND cutter_files device_cv_sim.cpp)
endif(HAS_OPENCV)

add_library (cutter SHARED ${cutter_files})
if(HAS_OPENCV)
    target_link_libraries (cutter cv highgui)
endif(HAS_OPENCV)

