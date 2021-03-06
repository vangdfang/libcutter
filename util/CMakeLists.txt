project (libcutter)
cmake_minimum_required(VERSION 2.6.0...3.19)
    if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)
include_directories(${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/include/pub)

find_library(HAS_SDL NAMES SDL2 SDL2_gfx)

IF(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    link_directories("/usr/local/lib")
    include_directories("/usr/local/include")

    # Fix for M1 Macs, where Homebrew uses a different location. See
    # https://apple.stackexchange.com/questions/410825/apple-silicon-port-all-homebrew-packages-under-usr-local-opt-to-opt-homebrew
    link_directories("/opt/homebrew/lib")
    include_directories("/opt/homebrew/include")
ENDIF()

add_executable (enumerate enumerate.cpp ConfigParser.hpp ConfigParser.cpp)
target_link_libraries (enumerate cutter_static)

add_executable (test_speed test_speed.cpp ConfigParser.hpp ConfigParser.cpp)
target_link_libraries (test_speed cutter_static)

add_executable (test_endian test_endian.cpp)

add_executable (test_serial test_serial.cpp)
target_link_libraries (test_serial cutter_static)

add_executable (draw_gcode draw_gcode.cpp gcode.cpp ConfigParser.hpp ConfigParser.cpp)
target_link_libraries (draw_gcode cutter_static)

add_executable (draw_svg draw_svg.cpp ConfigParser.hpp ConfigParser.cpp svg.cpp)
target_link_libraries (draw_svg cutter_static svg jpeg png)

add_executable (test_btea test_btea.cpp)
target_link_libraries (test_btea cutter_static)

add_executable (interpreter interpreter.cpp ConfigParser.hpp ConfigParser.cpp)
target_link_libraries (interpreter cutter_static)

if(HAS_SDL)
    add_executable (draw_gcode_cv draw_gcode_cv.cpp gcode.cpp)
    target_link_libraries (draw_gcode_cv cutter_static jpeg png SDL2 SDL2_gfx)

    add_executable (draw_svg_cv draw_svg_cv.cpp svg.cpp)
    target_link_libraries (draw_svg_cv cutter_static svg jpeg png SDL2 SDL2_gfx)

    add_executable (interpreter_cv interpreter_cv.cpp)
    target_link_libraries (interpreter_cv cutter_static SDL2 SDL2_gfx)
endif(HAS_SDL)

