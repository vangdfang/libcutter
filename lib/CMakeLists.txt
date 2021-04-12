project (libcutter)
cmake_minimum_required(VERSION 3.8...3.19)
    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)
include_directories(${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/include/pub)

find_package(SWIG)
if(SWIG_FOUND)
    include(${SWIG_USE_FILE})

    find_package(PythonLibs)
    if(PYTHONLIBS_FOUND)
        include_directories(${PYTHON_INCLUDE_PATH})

        set(CMAKE_SWIG_FLAGS "")

        set_source_files_properties(cutter.i PROPERTIES CPLUSPLUS ON)
    endif(PYTHONLIBS_FOUND)
endif(SWIG_FOUND)

find_library(HAS_SDL NAMES SDL SDL_gfx)

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

if(HAS_SDL)
    list(APPEND cutter_files device_sim.cpp)
endif(HAS_SDL)

# Build static version for our default tools, to make them easier to use. It's
# technically better not to specify `SHARED` or `STATIC` at all, but we
# generally want a reusable SHARED binary for app developers and a STATIC binary
# to make our tools easy to share. This is the most straightforward way to do
# this.
#
# https://cgold.readthedocs.io/en/latest/tutorials/libraries/static-shared.html
add_library (cutter SHARED ${cutter_files})
add_library (cutter_static STATIC ${cutter_files})
if(HAS_SDL)
    target_link_libraries (cutter SDL SDL_gfx)
endif(HAS_SDL)

if(PYTHONLIBS_FOUND)
    swig_add_library(pycutter TYPE SHARED LANGUAGE python SOURCES cutter.i)
    swig_link_libraries(pycutter ${PYTHON_LIBRARIES})
    if(HAS_SDL)
        target_link_libraries(pycutter SDL SDL_gfx)
    endif(HAS_SDL)
endif(PYTHONLIBS_FOUND)
