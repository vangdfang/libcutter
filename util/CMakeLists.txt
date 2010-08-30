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

    add_executable (jsdrive_relative jsdrive_relative.cpp)
    target_link_libraries (jsdrive_relative cutter pthread )
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

add_executable (test_speed test_speed.cpp)
target_link_libraries (test_speed cutter)

add_executable (test_endian test_endian.cpp)

add_executable (test_serial test_serial.cpp)
target_link_libraries (test_serial cutter)

add_executable (test_btea test_btea.cpp)
target_link_libraries (test_btea cutter)

add_executable (interpreter interpreter.cpp)
target_link_libraries (interpreter cutter)

add_executable (interpreter_cv interpreter_cv.cpp)
target_link_libraries (interpreter_cv cutter)

if(HAS_OPENCV)
    add_executable (jsdrive_cv jsdrive_cv.cpp)
    target_link_libraries (jsdrive_cv cutter pthread cv highgui)
endif(HAS_OPENCV)

