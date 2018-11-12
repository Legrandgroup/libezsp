CXXFLAGS = -W -Wall -pedantic -std=c++11 -Weffc++
#LIBCGICC_CXXFLAGS = $(pkg-config --cflags cgicc)
LDFLAGS = 
#LIBCGICC_LDFLAGS = $(pkg-config --libs cgicc) -lcgicc

EXEC = testEzsp
OBJECTFILES = mainEzspTestLinux.o mainEzspTest.o ../domain/ezsp-dongle.o ../domain/ash.o ../domain/zbmessage/zigbee-message.o ../domain/zbmessage/zclheader.o ../domain/zbmessage/zclframecontrol.o ../domain/zbmessage/apsoption.o ../domain/zbmessage/aps.o ../domain/zbmessage/out-zigbee-message.o
CLEANFILES = $(OBJECTFILES) $(EXEC)
INC = -I../spi -I../domain -I../domain/zbmessage

all: $(EXEC)

$(EXEC): $(OBJECTFILES)
	$(CXX) $(OBJECTFILES) $(LDFLAGS) $(LIBCGICC_LDFLAGS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LIBCGICC_CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CLEANFILES)
