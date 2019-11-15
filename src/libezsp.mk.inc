LIBEZSP_COMMON_SRC = \
                     $(SRC_DOMAIN_PATH)/ezsp-dongle.cpp \
                     $(SRC_DOMAIN_PATH)/ash.cpp \
                     $(SRC_DOMAIN_PATH)/custom-aes.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/green-power-frame.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/green-power-sink-table-entry.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/gpd-commissioning-command-payload.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/zigbee-message.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/zclheader.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/zclframecontrol.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/apsoption.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/aps.cpp \
                     $(SRC_DOMAIN_PATH)/zbmessage/zdp-enum.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/ezsp-enum.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/get-network-parameters-response.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-network-parameters.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-key-struct.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-child-data-struct.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-gp-address-struct.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-gp-sink-table-options-field.cpp \
                     $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-process-gp-pairing-parameter.cpp \
                     $(SRC_DOMAIN_PATH)/zigbee-tools/zigbee-networking.cpp \
                     $(SRC_DOMAIN_PATH)/zigbee-tools/zigbee-messaging.cpp \
                     $(SRC_DOMAIN_PATH)/zigbee-tools/green-power-sink.cpp \
                     $(SRC_DOMAIN_PATH)/zigbee-tools/green-power-sink-table.cpp \

LIBEZSP_LINUX_SPI_SRC = \
                        $(SRC_SPI_PATH)/GenericAsyncDataInputObservable.cpp \
                        $(SRC_SPI_PATH)/console/ConsoleLogger.cpp \
                        $(SRC_SPI_PATH)/cppthreads/CppThreadsTimerFactory.cpp \
                        $(SRC_SPI_PATH)/cppthreads/CppThreadsTimer.cpp \

LIBEZSP_LINUX_SERIALCPP_SRC = $(LIBEZSP_COMMON_SRC) \
                              $(LIBEZSP_LINUX_SPI_SRC) \
                              $(SRC_SPI_PATH)/serial/SerialUartDriver.cpp \

LIBEZSP_LINUX_MOCKSERIAL_SRC = $(LIBEZSP_COMMON_SRC) \
                               $(LIBEZSP_LINUX_SPI_SRC) \
                               $(SRC_SPI_PATH)/mock-uart/MockUartDriver.cpp \

LIBEZSP_COMMON_INC = -I$(SRC_SPI_PATH) -I$(SRC_DOMAIN_PATH) -I$(SRC_DOMAIN_PATH)/zbmessage

export LIBEZSP_COMMON_SRC
export LIBEZSP_LINUX_SERIALCPP_SRC
export LIBEZSP_LINUX_MOCKSERIAL_SRC
export LIBEZSP_COMMON_INC