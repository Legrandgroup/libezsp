option(USE_RARITAN "Use RARITAN environment" ON)
option(USE_CPPTHREADS "Use CPPTHREAD" OFF)

if(USE_CPPTHREADS)
set(USE_RARITAN OFF)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)
include_directories(src)

file(GLOB SRCS
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(mainEzspTest ${SRCS})

target_include_directories(mainEzspTest PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(mainEzspTest PUBLIC ezsp ezspspi pthread)
