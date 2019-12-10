lib-y=ezsp

ezsp_SOURCES+=domain/ash.cpp
ezsp_SOURCES+=domain/custom-aes.cpp
ezsp_SOURCES+=domain/ezsp-dongle.cpp

ezsp_HEADERS+=domain/ezsp-dongle-observer.h

ezsp_SOURCES+=domain/ezsp-protocol/ezsp-enum.cpp
ezsp_SOURCES+=domain/ezsp-protocol/get-network-parameters-response.cpp
ezsp_SOURCES+=domain/ezsp-protocol/struct/ember-child-data-struct.cpp
ezsp_SOURCES+=domain/ezsp-protocol/struct/ember-gp-address-struct.cpp
ezsp_SOURCES+=domain/ezsp-protocol/struct/ember-gp-proxy-table-entry-struct.cpp
ezsp_SOURCES+=domain/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.cpp
ezsp_SOURCES+=domain/ezsp-protocol/struct/ember-gp-sink-table-options-field.cpp
ezsp_SOURCES+=domain/ezsp-protocol/struct/ember-key-struct.cpp
ezsp_SOURCES+=domain/ezsp-protocol/struct/ember-network-parameters.cpp
ezsp_SOURCES+=domain/ezsp-protocol/struct/ember-process-gp-pairing-parameter.cpp

ezsp_SOURCES+=domain/zbmessage/aps.cpp
ezsp_SOURCES+=domain/zbmessage/apsoption.cpp
ezsp_SOURCES+=domain/zbmessage/gpd-commissioning-command-payload.cpp
ezsp_SOURCES+=domain/zbmessage/gp-pairing-command-option-struct.cpp
ezsp_SOURCES+=domain/zbmessage/green-power-device.cpp
ezsp_SOURCES+=domain/zbmessage/green-power-frame.cpp
ezsp_SOURCES+=domain/zbmessage/green-power-sink-table-entry.cpp
ezsp_SOURCES+=domain/zbmessage/zclframecontrol.cpp
ezsp_SOURCES+=domain/zbmessage/zclheader.cpp
ezsp_SOURCES+=domain/zbmessage/zdp-enum.cpp
ezsp_SOURCES+=domain/zbmessage/zigbee-message.cpp

ezsp_SOURCES+=domain/zigbee-tools/green-power-sink.cpp
ezsp_SOURCES+=domain/zigbee-tools/zigbee-messaging.cpp
ezsp_SOURCES+=domain/zigbee-tools/zigbee-networking.cpp

ezsp_SOURCES+=spi/GenericAsyncDataInputObservable.cpp

ezsp_SOURCES-$(USE_RARITAN)+=spi/raritan/RaritanEventLoop.cpp
ezsp_SOURCES-$(USE_RARITAN)+=spi/raritan/RaritanTimer.cpp
ezsp_SOURCES-$(USE_RARITAN)+=spi/raritan/RaritanTimerFactory.cpp

ezsp_SOURCES-$(USE_CPPTHREADS)+=spi/cppthreads/CppThreadsTimer.cpp
ezsp_SOURCES-$(USE_CPPTHREADS)+=spi/cppthreads/CppThreadsTimerFactory.cpp
ezsp_SOURCES-$(USE_SERIALCPP)+=spi/console/ConsoleLogger.cpp
ezsp_LIBS-$(USE_CPPTHREADS)+=pthread

ezsp_SOURCES-$(USE_RARITAN)+=spi/raritan/RaritanUartDriver.cpp
ezsp_SOURCES-$(USE_SERIALCPP)+=spi/serial/SerialUartDriver.cpp
ezsp_SOURCES-$(DUMMY)+=spi/mock-uart/MockUartDriver.cpp

bin-$(TEST)+=mainEzspTest

mainEzspTest_SOURCES+=example/CAppDemo.cpp
mainEzspTest_SOURCES+=example/mainEzspTest.cpp
mainEzspTest_SOURCES+=example/dummy_db.cpp
mainEzspTest_LIBRARY+=ezsp
mainEzspTest_LIBRARY+=serialcpp
