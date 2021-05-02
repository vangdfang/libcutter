project (libcutter)
cmake_minimum_required(VERSION 2.6.0...3.19.1)
    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)
include_directories(${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/include/pub)
set(CMAKE_CXX_FLAGS "-g -Wall -fstack-protector-all ")
set(CMAKE_C_FLAGS "-g -Wall -fstack-protector-all ")

# Use C++17 at least
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)
add_subdirectory(util)

