cmake_minimum_required(VERSION 3.8)

# set the project name and version
project(ezsp VERSION 2.2.0)

configure_file(version.h.in version.h)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

add_definitions("-fvisibility=hidden")
add_definitions("-fvisibility-inlines-hidden")

option(USE_GCOV "Use GCOV coverage profiler" OFF)
if(USE_GCOV)
	add_definitions(-g -O0 --coverage -fprofile-arcs -ftest-coverage -DUNIT_TESTS -fvisibility=default)
	link_libraries(--coverage -lgcov -fprofile-arcs)
endif()

#add_subdirectory(src)
add_subdirectory(src/ezsp)
add_subdirectory(src/spi)
add_subdirectory(example)
add_subdirectory(tests)
