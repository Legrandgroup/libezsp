option(USE_RARITAN "Use RARITAN environment" ON)
option(USE_CPPTHREADS "Use CPPTHREAD" OFF)

if(USE_CPPTHREADS)
set(USE_RARITAN OFF)
endif()

add_executable(mainEzspTest mainEzspTest.cpp)

target_include_directories(mainEzspTest PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(mainEzspTest PUBLIC ezsp ezspspi)
