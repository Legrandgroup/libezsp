set(ezspfull_SOURCES)

list(APPEND ezspfull_SOURCES
	ezsp/ash.cpp
	ezsp/custom-aes.cpp
	ezsp/ezsp-dongle.cpp
	ezsp/ezsp-protocol/ezsp-enum.cpp
	ezsp/ezsp-protocol/get-network-parameters-response.cpp
	ezsp/ezsp-protocol/struct/ember-child-data-struct.cpp
	ezsp/ezsp-protocol/struct/ember-gp-address-struct.cpp
	ezsp/ezsp-protocol/struct/ember-gp-proxy-table-entry-struct.cpp
	ezsp/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.cpp
	ezsp/ezsp-protocol/struct/ember-gp-sink-table-options-field.cpp
	ezsp/ezsp-protocol/struct/ember-key-struct.cpp
	ezsp/ezsp-protocol/struct/ember-network-parameters.cpp
	ezsp/ezsp-protocol/struct/ember-process-gp-pairing-parameter.cpp
	ezsp/zbmessage/aps.cpp
	ezsp/zbmessage/apsoption.cpp
	ezsp/zbmessage/gpd-commissioning-command-payload.cpp
	ezsp/zbmessage/gp-pairing-command-option-struct.cpp
	ezsp/zbmessage/green-power-device.cpp
	ezsp/zbmessage/green-power-frame.cpp
	ezsp/zbmessage/green-power-sink-table-entry.cpp
	ezsp/zbmessage/zclframecontrol.cpp
	ezsp/zbmessage/zclheader.cpp
	ezsp/zbmessage/zdp-enum.cpp
	ezsp/zbmessage/zigbee-message.cpp
	ezsp/zigbee-tools/green-power-sink.cpp
	ezsp/zigbee-tools/zigbee-messaging.cpp
	ezsp/zigbee-tools/zigbee-networking.cpp
)

list(APPEND ezspfull_SOURCES spi/GenericAsyncDataInputObservable.cpp)
list(APPEND ezspfull_SOURCES spi/TimerBuilder.cpp)
list(APPEND ezspfull_SOURCES spi/UartDriverBuilder.cpp)
list(APPEND ezspfull_SOURCES spi/Logger.cpp)
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
list(APPEND ezspfull_SOURCES 
	spi/console/ConsoleLogger.cpp
	spi/cppthreads/CppThreadsTimer.cpp
)
list(APPEND ezspfull_LIBS pthread)
endif()

if(USE_SERIALCPP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_SERIALCPP")
list(APPEND ezspfull_SOURCES spi/serial/SerialUartDriver.cpp)
list(APPEND ezspfull_LIBS serialcpp)
endif()

if(USE_MOCKSERIAL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_MOCKSERIAL")
list(APPEND ezspfull_SOURCES spi/mock-uart/MockUartDriver.cpp)
endif()

if(USE_RARITAN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_RARITAN")
list(APPEND ezspfull_SOURCES 
	spi/raritan/RaritanUartDriver.cpp
	spi/raritan/RaritanTimer.cpp
	spi/raritan/RaritanEventLoop.cpp
	spi/raritan/RaritanLogger.cpp
)
endif()

add_library(ezspfull ${ezspfull_SOURCES})

include_directories(${PROJECT_SOURCE_DIR}/src/ezsp ${PROJECT_SOURCE_DIR}/src/spi)
target_include_directories(ezspfull PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ezspfull PUBLIC ${ezspfull_LIBS})

install(TARGETS ezspfull DESTINATION lib)
