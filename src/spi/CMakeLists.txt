set(ezspspi_SOURCES )
list(APPEND ezspspi_SOURCES GenericAsyncDataInputObservable.cpp)
list(APPEND ezspspi_SOURCES TimerFactory.cpp)
list(APPEND ezspspi_SOURCES UartDriverBuilder.cpp)
list(APPEND ezspspi_SOURCES Logger.cpp)
set(ezspspi_LIBS )

option(USE_RARITAN "Use RARITAN environment" ON)
option(USE_CPPTHREADS "Use CPPTHREAD" OFF)
option(USE_SERIALCPP "Use SERIALCPP" OFF)
option(USE_MOCKSERIAL "Use MOCKSERIAL" OFF)
if(USE_CPPTHREADS)
set(USE_RARITAN OFF)
set(USE_SERIALCPP ON)
endif()
if(USE_MOCKSERIAL)
set(USE_SERIALCPP OFF)
endif()

if(USE_CPPTHREADS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_CPPTHREADS")
list(APPEND ezspspi_SOURCES 
	console/ConsoleLogger.cpp
	cppthreads/CppThreadsTimer.cpp
)
list(APPEND ezspspi_LIBS pthread)
endif()

if(USE_SERIALCPP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_SERIALCPP")
list(APPEND ezspspi_SOURCES serial/SerialUartDriver.cpp)
list(APPEND ezspspi_LIBS serialcpp)
endif()

if(USE_MOCKSERIAL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_MOCKSERIAL")
list(APPEND ezspspi_SOURCES mock-uart/MockUartDriver.cpp)
endif()

if(USE_RARITAN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_RARITAN")
list(APPEND ezspspi_SOURCES 
	raritan/RaritanUartDriver.cpp
	raritan/RaritanTimer.cpp
	raritan/RaritanLogger.cpp
)
endif()

add_library(ezspspi ${ezspspi_SOURCES})
target_include_directories(ezspspi PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ezspspi PUBLIC ${ezspspi_LIBS})

install(TARGETS ezspspi DESTINATION lib)
