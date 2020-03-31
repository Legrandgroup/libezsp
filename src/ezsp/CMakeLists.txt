set(ezsp_SOURCES)
list(APPEND ezsp_SOURCES
	ezsp.cpp
	lib-ezsp-main.cpp
	ash-codec.cpp
	bootloader-prompt.cpp
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
	zigbee-tools/green-power-device-db.cpp
)

option(USE_BUILTIN_MIC_PROCESSING "Compute and check MIC on the host rather than in the adapter" OFF)

configure_file(${PROJECT_SOURCE_DIR}/src/ezsp/config.h.in ${PROJECT_SOURCE_DIR}/include/ezsp/config.h)

set(ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/gpd.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/enum-generator.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/gpd.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/zbmessage/zcl.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/zbmessage/gpd-commissioning-command-payload.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/zbmessage/green-power-frame.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/zbmessage/green-power-device.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/byte-manip.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/ezsp.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/export.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/ezsp-protocol/struct/ember-gp-sink-table-options-field.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/ezsp/ezsp-protocol/ezsp-enum.h)
list(APPEND ezsp_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/config.h)

add_library(ezsp SHARED ${ezsp_SOURCES})
include_directories(${PROJECT_SOURCE_DIR}/include)
target_include_directories(ezsp PRIVATE ${PROJECT_SOURCE_DIR}/src)

# The lines below are commented-out because preserving the directory structure for includes is not yet handlede properly by cmake
# See for example: https://github.com/robotology/how-to-export-cpp-library/issues/8
#set_target_properties(ezsp PROPERTIES PUBLIC_HEADER "${ezsp_PUBLIC_HEADERS}")
#install(TARGETS ezsp LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/ezsp)
install(TARGETS ezsp LIBRARY DESTINATION lib)
# Instead we grab all include/ directory
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ezsp DESTINATION include)
