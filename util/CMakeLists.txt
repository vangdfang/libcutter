project (libcutter)
cmake_minimum_required(VERSION 2.6.4)
    if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)
include_directories(${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/include/pub)

find_library(HAS_OPENCV NAMES cv highgui)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable (jsdrive jsdrive.cpp)
    target_link_libraries (jsdrive cutter pthread)
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

add_executable (test_serial test_serial.cpp)
target_link_libraries (test_serial cutter)

if(HAS_OPENCV)
    add_executable (jsdrive_cv jsdrive_cv.cpp)
    target_link_libraries (jsdrive_cv cutter pthread cv highgui)
endif(HAS_OPENCV)

