define HELP

 libsgtree project makefile help
 ------------------------------------

 In order to perform unit tests with cpputest, run the following command:
 CFLAGS="-DUSE_CPPUTEST `pkg-config --cflags cpputest`" LIBS="`pkg-config --libs cpputest`" CPPUTEST_STATIC_LIB=/path/to/libCppUTest.a make CC=g++ <target>
 
 In ordert to compile on native 128-bit platforms, prepend make with the following environment variables (eg for little endian):
 CFLAGS="-DHAS_INT128 -DIS_LITTLE_ENDIAN"
 
 Usage: `make <target>' where <target> is one of:


  --- build commands
  all              build lib and its tests
  test             build tests 
  clean            remove binaries (lib and tests)
  clean-all        remove binaries and object files
  rebuild          clean all and build

endef
export HELP

#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

# get rid of built-in rules
.SUFFIXES:

# SRC_PATH should point to the src/ folder containing source code for this library (can be overridden from environment)
SRC_PATH ?= ..
SRC_DOMAIN_PATH ?= $(SRC_PATH)/domain
SRC_SPI_PATH ?= $(SRC_PATH)/spi

LOCAL_LDFLAGS ?= -L/home/seb/_DEV/serial/compiled/li
LOCAL_INC ?= -I/home/seb/_DEV/serial/compiled/include

CXXFLAGS = -DUSE_SERIALCPP -W -Wall -pedantic -std=c++11 -Wno-unused-parameter -g -Weffc++
LDFLAGS = $(LOCAL_LDFLAGS) -lpthread

EXEC = test_runner
OBJECTFILES = \
			  $(SRC_PATH)/example/dummy_db.o \
			  $(SRC_DOMAIN_PATH)/ezsp-dongle.o \
			  $(SRC_DOMAIN_PATH)/ash.o \
			  $(SRC_DOMAIN_PATH)/zbmessage/green-power-frame.o \
			  $(SRC_DOMAIN_PATH)/zbmessage/green-power-sink-table-entry.o \
			  $(SRC_DOMAIN_PATH)/zbmessage/zigbee-message.o \
			  $(SRC_DOMAIN_PATH)/zbmessage/zclheader.o \
			  $(SRC_DOMAIN_PATH)/zbmessage/zclframecontrol.o \
			  $(SRC_DOMAIN_PATH)/zbmessage/apsoption.o \
			  $(SRC_DOMAIN_PATH)/zbmessage/aps.o \
			  $(SRC_DOMAIN_PATH)/zbmessage/zdp-enum.o \
			  $(SRC_DOMAIN_PATH)/ezsp-protocol/ezsp-enum.o \
			  $(SRC_DOMAIN_PATH)/ezsp-protocol/get-network-parameters-response.o \
			  $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-network-parameters.o \
			  $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-key-struct.o \
			  $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-child-data-struct.o \
			  $(SRC_DOMAIN_PATH)/ezsp-protocol/struct/ember-gp-address-struct.o \
			  $(SRC_DOMAIN_PATH)/zigbee-tools/zigbee-networking.o \
			  $(SRC_DOMAIN_PATH)/zigbee-tools/zigbee-messaging.o \
			  $(SRC_DOMAIN_PATH)/zigbee-tools/green-power-sink.o \
			  $(SRC_DOMAIN_PATH)/zigbee-tools/green-power-sink-table.o \
			  $(SRC_SPI_PATH)/GenericAsyncDataInputObservable.o \
			  $(SRC_SPI_PATH)/mock-uart/MockUartDriver.o \
			  $(SRC_SPI_PATH)/console/ConsoleLogger.o \
			  $(SRC_SPI_PATH)/cppthreads/CppThreadsTimerFactory.o \
			  $(SRC_SPI_PATH)/cppthreads/CppThreadsTimer.o \

TEST_OBJECTFILES = \
			  node_tests.o \
			  test_libezsp.o \

CLEANFILES = $(OBJECTFILES) $(EXEC)
INC = $(LOCAL_INC) -I../spi -I../domain -I../domain/zbmessage

all: $(EXEC)

$(EXEC): $(OBJECTFILES)
	$(CXX) $(OBJECTFILES) $(LDFLAGS) $(LIBCGICC_LDFLAGS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LIBCGICC_CXXFLAGS) $(INC) -c $< -o $@

clean:
	@rm -f $(CLEANFILES)
	@rm -f test_runner

check: test_runner
	./$<

test_runner: $(OBJECTFILES) $(TEST_OBJECTFILES)
	@echo Linking $@
	$(SILENCE)$(CXX) $(LIBS) $(CXXFLAGS) $(LDFLAGS) $(LIBCGICC_LDFLAGS) -o $@ $^
