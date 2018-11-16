CXXFLAGS = -W -Wall -pedantic -std=c++11 -Wno-unused-parameter -g # -Weffc++
#LIBCGICC_CXXFLAGS = $(pkg-config --cflags cgicc)
LDFLAGS = -L/home/seb/_DEV/serial/compiled/lib -lserial -lpthread
#LIBCGICC_LDFLAGS = $(pkg-config --libs cgicc) -lcgicc

EXEC = testEzsp
OBJECTFILES = mainEzspTestLinux.o \
			  mainEzspTest.o \
			  ../domain/ezsp-dongle.o \
			  ../domain/ash.o \
			  ../domain/zbmessage/zigbee-message.o \
			  ../domain/zbmessage/zclheader.o \
			  ../domain/zbmessage/zclframecontrol.o \
			  ../domain/zbmessage/apsoption.o \
			  ../domain/zbmessage/aps.o \
			  ../domain/ezsp-protocol/ezsp-enum.o \
			  ../domain/ezsp-protocol/get-network-parameters-response.o \
			  ../domain/ezsp-protocol/struct/ember-network-parameters.o \
			  ../domain/ezsp-protocol/struct/ember-key-struct.o \
			  ../domain/ezsp-protocol/struct/ember-child-data-struct.o \
			  ../domain/zigbee-tools/zigbee-networking.o \
			  ../domain/zigbee-tools/zigbee-messaging.o \
			  ../spi/GenericAsyncDataInputObservable.o \
			  ../spi/serial/SerialUartDriver.o \
			  ../spi/serial/SerialTimerFactory.o \
			  ../spi/serial/SerialTimer.o

CLEANFILES = $(OBJECTFILES) $(EXEC)
INC = -I../spi -I../domain -I../domain/zbmessage -I/home/seb/_DEV/serial/compiled/include

all: $(EXEC)

$(EXEC): $(OBJECTFILES)
	$(CXX) $(OBJECTFILES) $(LDFLAGS) $(LIBCGICC_LDFLAGS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LIBCGICC_CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CLEANFILES)
