include ../../Config.mk

MODULE_DEPS = $(LIBC_COMPONENT) $(LIBSTDCXX_COMPONENT) libpp_base

CXXFLAGS += $(FW_CXXFLAGS_VISIBILITY) -DUSE_RARITAN -g -O0 -Wno-error=unused-parameter -Wno-unused-parameter #-Weffc++
CFLAGS = $(FW_CFLAGS_3RDPARTY)

TARGET_SHARED_LIBNAMES = libezsp

libezsp_MODULE_NAME = test_log

# SRC_PATH should point to the src/ folder containing source code for this library (can be overridden from environment)
SRC_PATH ?= .
SRC_DOMAIN_PATH ?= $(SRC_PATH)/domain
SRC_SPI_PATH ?= $(SRC_PATH)/spi

include libezsp.mk.inc

libezsp_SRCS = $(LIBEZSP_RARITAN_SRC)

libezsp_LIBS = -lpp_base

# Header files to install
libezspincludedir = $(includedir)/include/local/ezsp

libezspinclude_HEADERS = \
domain/zigbee-tools/zigbee-networking.h \
domain/zigbee-tools/green-power-sink.h \
domain/zigbee-tools/zigbee-messaging.h \
domain/green-power-observer.h \
domain/ezsp-dongle-observer.h \
domain/ezsp-dongle.h \
domain/ash.h \
domain/ezsp-protocol/struct/ember-process-gp-pairing-parameter.h \
domain/ezsp-protocol/struct/ember-key-struct.h \
domain/ezsp-protocol/struct/ember-gp-sink-table-options-field.h \
domain/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.h \
domain/ezsp-protocol/struct/ember-child-data-struct.h \
domain/ezsp-protocol/struct/ember-network-parameters.h \
domain/ezsp-protocol/struct/ember-gp-address-struct.h \
domain/ezsp-protocol/get-network-parameters-response.h \
domain/ezsp-protocol/ezsp-enum.h \
domain/zbmessage/green-power-device.h \
domain/zbmessage/green-power-frame.h \
domain/zbmessage/gp-pairing-command-option-struct.h \
domain/zbmessage/aps.h \
domain/zbmessage/zclframecontrol.h \
domain/zbmessage/zclheader.h \
domain/zbmessage/apsoption.h \
domain/zbmessage/green-power-sink-table-entry.h \
domain/zbmessage/gpd-commissioning-command-payload.h \
domain/zbmessage/zdp-enum.h \
domain/zbmessage/zigbee-message.h \
spi/raritan/RaritanLogger.h \
spi/raritan/RaritanTimerFactory.h \
spi/raritan/RaritanTimer.h \
spi/raritan/RaritanEventLoop.h \
spi/raritan/RaritanUartDriver.h \
spi/ILogger.h \
spi/ITimerFactory.h \
spi/ITimer.h \
spi/IUartDriver.h \
spi/GenericLogger.h \
spi/GenericAsyncDataInputObservable.h \
spi/IAsyncDataInputObserver.h \

MKDIR_P = /usr/bin/mkdir -p
INSTALL_HEADER = install -Dc -m 644

install-libezspincludeHEADERS:: $(libezspinclude_HEADERS)
	echo "  Install ezsp headers to $(DESTDIR)$(libezspincludedir)"
	test -z "$(libezspincludedir)" || $(MKDIR_P) "$(DESTDIR)$(libezspincludedir)"
	@list='$^'; test -n "$(libezspincludedir)" || list=; \
	for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  echo "$$d$$p"; \
	done | \
	while read files; do \
	  echo "  $(INSTALL_HEADER) $$files '$(DESTDIR)$(libezspincludedir)'"; \
	  $(INSTALL_HEADER) $$files "$(DESTDIR)$(libezspincludedir)/$${files#source}" || exit $$?; \
	done


uninstall-libezspincludeHEADERS::
	echo "  Uninstall ezsp headers"
	@list='$(libezspinclude_HEADERS)'; test -n "$(libezspincludedir)" || list=; \
	files=`for p in $$list; do echo $$p; done | sed -e 's|^.*/||'`; \
	test -n "$$files" || exit 0; \
	echo "  ( cd '$(DESTDIR)$(libezspincludedir)' && rm -rf $(DESTDIR)$(libezspincludedir) )"; \
	cd "$(DESTDIR)$(libezspincludedir)" && rm -rf "$(DESTDIR)$(libezspincludedir)"

install-local:: install-libezspincludeHEADERS

clean-local:: uninstall-libezspincludeHEADERS

mainEzspTest_MODULE_NAME = test_log

TARGET_PRGS = mainEzspTest

mainEzspTest_CPPFLAGS += -Ispi -Idomain -Idomain/zbmessage -Isrc/domain/ezsp-protocol -Iexample

mainEzspTest_SRCS = example/mainEzspTest.cpp \
	example/CAppDemo.cpp \
	example/dummy_db.cpp \
	$(libezsp_SRCS)

mainEzspTest_LIBS = -lpp_base

include $(PP_BUILD_SYS_DIR)/Top.mk
