include ../../Config.mk

MODULE_DEPS = $(LIBC_COMPONENT) $(LIBSTDCXX_COMPONENT) libpp_base

CXXFLAGS += $(FW_CXXFLAGS_VISIBILITY) -DUSE_RARITAN -g -O0 -Wno-error=unused-parameter -Wno-unused-parameter #-Weffc++
CFLAGS = $(FW_CFLAGS_3RDPARTY)

TARGET_SHARED_LIBNAMES = libezsp

libezsp_MODULE_NAME = test_log

libezsp_SRCS = domain/zbmessage/zigbee-message.cpp \
	domain/zbmessage/zclheader.cpp \
	domain/zbmessage/zclframecontrol.cpp \
	domain/zbmessage/apsoption.cpp \
	domain/zbmessage/aps.cpp \
	domain/zbmessage/zdp-enum.cpp \
	domain/zbmessage/green-power-frame.cpp \
	domain/zigbee-tools/green-power-sink.cpp \
	domain/zbmessage/green-power-sink-table-entry.cpp \
	domain/zigbee-tools/zigbee-messaging.cpp \
	domain/zigbee-tools/zigbee-networking.cpp \
	domain/zigbee-tools/green-power-sink-table.cpp \
	domain/ezsp-protocol/ezsp-enum.cpp \
	domain/ezsp-protocol/get-network-parameters-response.cpp \
	domain/ezsp-protocol/struct/ember-key-struct.cpp \
	domain/ezsp-protocol/struct/ember-network-parameters.cpp \
	domain/ezsp-protocol/struct/ember-child-data-struct.cpp \
	domain/ezsp-protocol/struct/ember-gp-address-struct.cpp \
	domain/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.cpp \
	domain/ezsp-dongle.cpp \
	domain/ash.cpp \
	spi/GenericAsyncDataInputObservable.cpp \
	spi/raritan/RaritanUartDriver.cpp \
	spi/raritan/RaritanTimerFactory.cpp \
	spi/raritan/RaritanTimer.cpp \
	spi/raritan/RaritanEventLoop.cpp \
	spi/raritan/RaritanLogger.cpp \

libezsp_LIBS = -lpp_base

# Header files to install
libezspincludedir = $(includedir)/include/local/ezsp

libezspinclude_HEADERS = domain/zbmessage/zdp-enum.h \
domain/ezsp-protocol/ezsp-enum.h \
domain/ezsp-protocol/get-network-parameters-response.h \
domain/byte-manip.h \
domain/ash.h \
domain/ezsp-dongle.h \
domain/ezsp-dongle-observer.h \
domain/zigbee-tools/zigbee-networking.h \
domain/zigbee-tools/zigbee-messaging.h \
domain/zbmessage/green-power-frame.h \
domain/zigbee-tools/green-power-sink.h \
domain/zigbee-tools/green-power-sink-table.h \
domain/zbmessage/green-power-sink-table-entry.h \
domain/green-power-observer.h \
domain/zbmessage/zigbee-message.h \
domain/zbmessage/aps.h \
domain/zbmessage/apsoption.h \
domain/zbmessage/zclheader.h \
domain/zbmessage/zclframecontrol.h \
domain/ezsp-protocol/struct/ember-key-struct.h \
domain/ezsp-protocol/struct/ember-network-parameters.h \
domain/ezsp-protocol/struct/ember-child-data-struct.h \
domain/ezsp-protocol/struct/ember-gp-address-struct.h \
domain/ezsp-protocol/struct/ember-gp-sink-table-entry-struct.h \
spi/raritan/RaritanTimerFactory.h \
spi/raritan/RaritanEventLoop.h \
spi/raritan/RaritanUartDriver.h \
spi/ITimerFactory.h \
spi/ITimerFactory.h \
spi/IUartDriver.h \
spi/ITimer.h \
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
