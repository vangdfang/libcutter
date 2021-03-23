project (libcutter)
cmake_minimum_required(VERSION 2.6.0...3.19)
    if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)
include_directories(${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/include/pub)

find_library(HAS_SDL NAMES SDL SDL_gfx)

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

add_executable (draw_gcode draw_gcode.cpp gcode.cpp)
target_link_libraries (draw_gcode cutter)

add_executable (draw_svg draw_svg.cpp)
target_link_libraries (draw_svg cutter svg jpeg png)

add_executable (test_btea test_btea.cpp)
target_link_libraries (test_btea cutter)

add_executable (interpreter interpreter.cpp)
target_link_libraries (interpreter cutter)

if(HAS_SDL)
    add_executable (draw_gcode_cv draw_gcode_cv.cpp gcode.cpp)
    target_link_libraries (draw_gcode_cv cutter jpeg png)

    add_executable (draw_svg_cv draw_svg_cv.cpp)
    target_link_libraries (draw_svg_cv cutter svg jpeg png)

    add_executable (interpreter_cv interpreter_cv.cpp)
    target_link_libraries (interpreter_cv cutter)

    add_executable (jsdrive_cv jsdrive_cv.cpp)
    target_link_libraries (jsdrive_cv cutter pthread)
endif(HAS_SDL)

