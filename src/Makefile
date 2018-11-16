include ../../Config.mk

MODULE_DEPS = $(LIBC_COMPONENT) $(LIBSTDCXX_COMPONENT) libpp_base

CXXFLAGS += $(FW_CXXFLAGS_VISIBILITY) -Wno-error=unused-parameter -Wno-error #-Weffc++

mainEzspTestRaritan_MODULE_NAME = test_log

TARGET_PRGS = mainEzspTestRaritan

mainEzspTestRaritan_CPPFLAGS += -Ispi -Idomain -Idomain/zbmessage -Isrc/domain/ezsp-protocol -Itest
mainEzspTestRaritan_SRCS = mainEzspTestRaritan.cpp \
	domain/zbmessage/zigbee-message.cpp \
	domain/zbmessage/zclheader.cpp \
	domain/zbmessage/zclframecontrol.cpp \
	domain/zbmessage/apsoption.cpp \
	domain/zbmessage/aps.cpp \
	domain/zigbee-tools/zigbee-messaging.cpp \
	domain/zigbee-tools/zigbee-networking.cpp \
	domain/ezsp-protocol/ezsp-enum.cpp \
	domain/ezsp-protocol/get-network-parameters-response.cpp \
	domain/ezsp-protocol/struct/ember-key-struct.cpp \
	domain/ezsp-protocol/struct/ember-network-parameters.cpp \
	domain/ezsp-protocol/struct/ember-child-data-struct.cpp \
	test/mainEzspTest.cpp \
	domain/ezsp-dongle.cpp \
	domain/ash.cpp \
	spi/GenericAsyncDataInputObservable.cpp \
	spi/raritan/RaritanUartDriver.cpp \
	spi/raritan/RaritanEventLoop.cpp \
	

#	domain/zbmessage/out-zigbee-message.cpp

mainEzspTestRaritan_LIBS = -lpp_base

include $(PP_BUILD_SYS_DIR)/Top.mk
