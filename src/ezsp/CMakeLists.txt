set(ezsp_SOURCES)
list(APPEND ezsp_SOURCES
	lib-ezsp-main.cpp
	ash.cpp
	bootloader-prompt.cpp
	custom-aes.cpp
	ezsp-dongle.cpp
	ezsp-protocol/ezsp-enum.cpp
	ezsp-protocol/get-network-parameters-response.cpp
	ezsp-protocol/struct/ember-child-data-struct.cpp
	ezsp-protocol/struct/ember-gp-address-struct.cpp
	ezsp-protocol/struct/ember-gp-proxy-table-entry-struct.cpp
	ezsp-protocol/struct/ember-gp-sink-table-entry-struct.cpp
	ezsp-protocol/struct/ember-gp-sink-table-options-field.cpp
	ezsp-protocol/struct/ember-key-struct.cpp
	ezsp-protocol/struct/ember-network-parameters.cpp
	ezsp-protocol/struct/ember-process-gp-pairing-parameter.cpp
	zbmessage/aps.cpp
	zbmessage/apsoption.cpp
	zbmessage/gpd-commissioning-command-payload.cpp
	zbmessage/gp-pairing-command-option-struct.cpp
	zbmessage/green-power-device.cpp
	zbmessage/green-power-frame.cpp
	zbmessage/green-power-sink-table-entry.cpp
	zbmessage/zclframecontrol.cpp
	zbmessage/zclheader.cpp
	zbmessage/zdp-enum.cpp
	zbmessage/zigbee-message.cpp
	zigbee-tools/green-power-sink.cpp
	zigbee-tools/zigbee-messaging.cpp
	zigbee-tools/zigbee-networking.cpp
)

add_library(ezsp ${ezsp_SOURCES})
include_directories(${PROJECT_SOURCE_DIR}/src/ezsp ${PROJECT_SOURCE_DIR}/src/spi)
target_include_directories(ezsp PRIVATE ${PROJECT_SOURCE_DIR}/src)

install(TARGETS ezsp DESTINATION lib)
