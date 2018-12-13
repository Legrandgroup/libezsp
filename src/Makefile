include ../../Config.mk

MODULE_DEPS = $(LIBC_COMPONENT) $(LIBSTDCXX_COMPONENT) libpp_base

CXXFLAGS += $(FW_CXXFLAGS_VISIBILITY) -DUSE_RARITAN -g -O0 -Wno-error=unused-parameter -Wno-unused-parameter #-Weffc++

TARGET_SHARED_LIBNAMES = libezsp

libezsp_MODULE_NAME = test_log

libezsp_SRCS = domain/zbmessage/zigbee-message.cpp \
	domain/zbmessage/zclheader.cpp \
	domain/zbmessage/zclframecontrol.cpp \
	domain/zbmessage/apsoption.cpp \
	domain/zbmessage/aps.cpp \
	domain/zbmessage/zdp-enum.cpp \
	domain/zigbee-tools/zigbee-messaging.cpp \
	domain/zigbee-tools/zigbee-networking.cpp \
	domain/ezsp-protocol/ezsp-enum.cpp \
	domain/ezsp-protocol/get-network-parameters-response.cpp \
	domain/ezsp-protocol/struct/ember-key-struct.cpp \
	domain/ezsp-protocol/struct/ember-network-parameters.cpp \
	domain/ezsp-protocol/struct/ember-child-data-struct.cpp \
	domain/ezsp-dongle.cpp \
	domain/ash.cpp \
	spi/GenericAsyncDataInputObservable.cpp \
	spi/raritan/RaritanUartDriver.cpp \
	spi/raritan/RaritanTimerFactory.cpp \
	spi/raritan/RaritanTimer.cpp \
	spi/raritan/RaritanEventLoop.cpp \

libezsp_LIBS = -lpp_base

mainEzspTest_MODULE_NAME = test_log

TARGET_PRGS = mainEzspTest

mainEzspTest_CPPFLAGS += -Ispi -Idomain -Idomain/zbmessage -Isrc/domain/ezsp-protocol -Iexample

mainEzspTest_SRCS = example/mainEzspTest.cpp \
	example/CAppDemo.cpp \
	example/dummy_db.cpp \
	$(libezsp_SRCS)

mainEzspTest_LIBS = -lpp_base

include $(PP_BUILD_SYS_DIR)/Top.mk
