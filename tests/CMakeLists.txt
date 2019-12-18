option(USE_MOCKSERIAL "Use MOCKSERIAL" OFF)

if(USE_MOCKSERIAL)
set(gptest_SOURCES)
list(APPEND gptest_SOURCES mock_serial_self_tests.cpp)
list(APPEND gptest_SOURCES gp_tests.cpp)
list(APPEND gptest_SOURCES test_libezsp.cpp)
list(APPEND gptest_SOURCES ../example/dummy_db.cpp)
add_executable(gptest ${gptest_SOURCES})

target_include_directories(gptest PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gptest PUBLIC ezsp ezspspi)
endif()
