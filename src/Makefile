lib-y=ezsp

ezsp_CFLAGS+=-I.

ezsp_SOURCES+=ezsp/ash.cpp
ezsp_SOURCES+=ezsp/custom-aes.cpp
ezsp_SOURCES+=ezsp/ezsp-dongle.cpp

ezsp_HEADERS+=ezsp/ezsp-dongle-observer.h

ezsp_SOURCES+=ezsp/ezsp-protocol/ezsp-enum.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/get-network-parameters-response.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/struct/ember-child-data-struct.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/struct/ember-gp-address-struct.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/struct/ember-gp-proxy-table-entry-struct.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/struct/ember-gp-sink-table-options-field.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/struct/ember-key-struct.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/struct/ember-network-parameters.cpp
ezsp_SOURCES+=ezsp/ezsp-protocol/struct/ember-process-gp-pairing-parameter.cpp

ezsp_SOURCES+=ezsp/zbmessage/aps.cpp
ezsp_SOURCES+=ezsp/zbmessage/apsoption.cpp
ezsp_SOURCES+=ezsp/zbmessage/gpd-commissioning-command-payload.cpp
ezsp_SOURCES+=ezsp/zbmessage/gp-pairing-command-option-struct.cpp
ezsp_SOURCES+=ezsp/zbmessage/green-power-device.cpp
ezsp_SOURCES+=ezsp/zbmessage/green-power-frame.cpp
ezsp_SOURCES+=ezsp/zbmessage/green-power-sink-table-entry.cpp
ezsp_SOURCES+=ezsp/zbmessage/zclframecontrol.cpp
ezsp_SOURCES+=ezsp/zbmessage/zclheader.cpp
ezsp_SOURCES+=ezsp/zbmessage/zdp-enum.cpp
ezsp_SOURCES+=ezsp/zbmessage/zigbee-message.cpp

ezsp_SOURCES+=ezsp/zigbee-tools/green-power-sink.cpp
ezsp_SOURCES+=ezsp/zigbee-tools/zigbee-messaging.cpp
ezsp_SOURCES+=ezsp/zigbee-tools/zigbee-networking.cpp

lib-y+=ezpspi

ezpspi_SOURCES+=spi/GenericAsyncDataInputObservable.cpp

ezpspi_SOURCES-$(USE_RARITAN)+=spi/raritan/RaritanEventLoop.cpp
ezpspi_SOURCES-$(USE_RARITAN)+=spi/raritan/RaritanTimer.cpp
ezpspi_SOURCES-$(USE_RARITAN)+=spi/raritan/RaritanTimerFactory.cpp

ezpspi_SOURCES-$(USE_CPPTHREADS)+=spi/cppthreads/CppThreadsTimer.cpp
ezpspi_SOURCES-$(USE_CPPTHREADS)+=spi/cppthreads/CppThreadsTimerFactory.cpp
ezpspi_SOURCES-$(USE_SERIALCPP)+=spi/console/ConsoleLogger.cpp
ezpspi_LIBS-$(USE_CPPTHREADS)+=pthread

ezpspi_SOURCES-$(USE_RARITAN)+=spi/raritan/RaritanUartDriver.cpp
ezpspi_SOURCES-$(USE_SERIALCPP)+=spi/serial/SerialUartDriver.cpp
ezpspi_SOURCES-$(DUMMY)+=spi/mock-uart/MockUartDriver.cpp
