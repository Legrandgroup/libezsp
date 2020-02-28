set(ezspspi_SOURCES )
list(APPEND ezspspi_SOURCES TimerBuilder.cpp)
list(APPEND ezspspi_SOURCES GenericAsyncDataInputObservable.cpp)
list(APPEND ezspspi_SOURCES UartDriverBuilder.cpp)
list(APPEND ezspspi_SOURCES Logger.cpp)
list(APPEND ezspspi_SOURCES aes/AesBuilder.cpp)
set(ezspspi_PUBLIC_HEADERS )
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/aes/AesBuilder.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/src/spi/aes/custom/custom-aes.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/aes/IAes.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/ByteBuffer.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/src/spi/console/ConsoleLogger.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/src/spi/cppthreads/CppThreadsTimer.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/GenericAsyncDataInputObservable.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/IAsyncDataInputObserver.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/ILogger.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/ITimer.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/IUartDriver.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/Logger.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/src/spi/raritan/RaritanLogger.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/src/spi/raritan/RaritanTimer.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/src/spi/raritan/RaritanUartDriver.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/src/spi/serial/SerialUartDriver.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/TimerBuilder.h)
list(APPEND ezspspi_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/spi/UartDriverBuilder.h)
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
list(APPEND ezspspi_LIBS pp_base)
endif()

option(USE_AESCUSTOM "Use AESCUSTOM" ON)
if(USE_AESCUSTOM)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_AESCUSTOM")
list(APPEND ezspspi_SOURCES aes/custom/custom-aes.cpp)
endif()


add_library(ezspspi SHARED ${ezspspi_SOURCES})
include_directories(${PROJECT_SOURCE_DIR}/include)
target_include_directories(ezspspi PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ezspspi PUBLIC ${ezspspi_LIBS})

set_target_properties(ezspspi PROPERTIES PUBLIC_HEADER "${ezspspi_PUBLIC_HEADERS}")
install(TARGETS ezspspi LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/spi)
