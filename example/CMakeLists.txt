option(USE_RARITAN "Use RARITAN environment" ON)
option(USE_CPPTHREADS "Use CPPTHREAD" OFF)

if(USE_CPPTHREADS)
set(USE_RARITAN OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DUSE_CPPTHREADS")
endif()
if(USE_RARITAN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DUSE_RARITAN")
endif()

add_executable(mainEzspTest mainEzspTest.cpp)

target_include_directories(mainEzspTest PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(mainEzspTest PUBLIC ezsp ezspspi)
