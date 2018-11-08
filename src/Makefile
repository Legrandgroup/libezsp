include ../../Config.mk

MODULE_DEPS = $(LIBC_COMPONENT) $(LIBSTDCXX_COMPONENT) libpp_base

ezsp_raritan_MODULE_NAME = test_log

TARGET_PRGS = ezsp-raritan

ezsp_raritan_SRCS = ezsp-raritan.cpp spi/raritan/RaritanUartDriver.cpp spi/raritan/RaritanEventLoop.cpp
#ezsp_raritan_SRCS = testtool.cpp
ezsp_raritan_LIBS = -lpp_base

include $(PP_BUILD_SYS_DIR)/Top.mk
