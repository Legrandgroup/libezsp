cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(ezsp VERSION 1.0)

configure_file(version.h.in version.h)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O0 -ggdb")

add_subdirectory(src)
add_subdirectory(example)
