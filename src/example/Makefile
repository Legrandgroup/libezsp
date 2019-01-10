LOCAL_LDFLAGS ?= -L/home/seb/_DEV/serial/compiled/li
LOCAL_INC ?= -I/home/seb/_DEV/serial/compiled/include

CXXFLAGS = -DUSE_SERIALCPP -W -Wall -pedantic -std=c++11 -Wno-unused-parameter -g # -Weffc++
LDFLAGS = $(LOCAL_LDFLAGS) -lserial -lpthread

EXEC = mainEzspTest
OBJECTFILES = mainEzspTest.o \
			  CAppDemo.o \
			  dummy_db.o \
			  ../domain/ezsp-dongle.o \
			  ../domain/ash.o \
			  ../domain/zbmessage/zigbee-message.o \
			  ../domain/zbmessage/zclheader.o \
			  ../domain/zbmessage/zclframecontrol.o \
			  ../domain/zbmessage/apsoption.o \
			  ../domain/zbmessage/aps.o \
			  ../domain/zbmessage/zdp-enum.o \
			  ../domain/ezsp-protocol/ezsp-enum.o \
			  ../domain/ezsp-protocol/get-network-parameters-response.o \
			  ../domain/ezsp-protocol/struct/ember-network-parameters.o \
			  ../domain/ezsp-protocol/struct/ember-key-struct.o \
			  ../domain/ezsp-protocol/struct/ember-child-data-struct.o \
			  ../domain/zigbee-tools/zigbee-networking.o \
			  ../domain/zigbee-tools/zigbee-messaging.o \
			  ../spi/GenericAsyncDataInputObservable.o \
			  ../spi/serial/SerialUartDriver.o \
			  ../spi/cppthreads/CppThreadsTimerFactory.o \
			  ../spi/cppthreads/CppThreadsTimer.o

CLEANFILES = $(OBJECTFILES) $(EXEC)
INC = $(LOCAL_INC) -I../spi -I../domain -I../domain/zbmessage

all: $(EXEC)

$(EXEC): $(OBJECTFILES)
	$(CXX) $(OBJECTFILES) $(LDFLAGS) $(LIBCGICC_LDFLAGS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LIBCGICC_CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CLEANFILES)
