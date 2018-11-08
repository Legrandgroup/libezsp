CXXFLAGS = -W -Wall -pedantic -std=c++11 -Weffc++
#LIBCGICC_CXXFLAGS = $(pkg-config --cflags cgicc)
LDFLAGS = 
#LIBCGICC_LDFLAGS = $(pkg-config --libs cgicc) -lcgicc

EXEC = testEzsp
OBJECTFILES = mainEzspTest.o ../domain/ezsp-dongle.cpp
CLEANFILES = $(OBJECTFILES) $(EXEC)
INC = -I../spi -I../domain

all: $(EXEC)

$(EXEC): $(OBJECTFILES)
	$(CXX) $(OBJECTFILES) $(LDFLAGS) $(LIBCGICC_LDFLAGS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LIBCGICC_CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(CLEANFILES)
