include ../../Config.mk

MODULE_DEPS = $(LIBC_COMPONENT) $(LIBSTDCXX_COMPONENT) libpp_base

CXXFLAGS += $(FW_CXXFLAGS_VISIBILITY) -Wno-error=unused-parameter -Wno-error #-Weffc++

mainEzspTest_MODULE_NAME = test_log

TARGET_PRGS = mainEzspTest

mainEzspTest_CPPFLAGS += -Ispi -Idomain -Idomain/zbmessage
mainEzspTest_SRCS = mainEzspTest.cpp domain/ezsp-dongle.cpp domain/ash.cpp spi/GenericAsyncDataInputObservable.cpp spi/raritan/RaritanUartDriver.cpp spi/raritan/RaritanEventLoop.cpp
#ezsp_raritan_SRCS = testtool.cpp
mainEzspTest_LIBS = -lpp_base

include $(PP_BUILD_SYS_DIR)/Top.mk
