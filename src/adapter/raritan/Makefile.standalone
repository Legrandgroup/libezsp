CXXFLAGS = -W -Wall -pedantic 
#LIBCGICC_CXXFLAGS = $(pkg-config --cflags cgicc)
LDFLAGS = 
#LIBCGICC_LDFLAGS = $(pkg-config --libs cgicc) -lcgicc

EXEC = test
OBJECTFILES = demo_pushing_observer.o GenericAsyncDataInputObservable.o RaritanUARTInputObservable.o
CLEANFILES = $(OBJECTFILES) $(EXEC)

all: $(EXEC)

$(EXEC): $(OBJECTFILES)
	$(CXX) $(OBJECTFILES) $(LDFLAGS) $(LIBCGICC_LDFLAGS) -o $(EXEC)

test.o: test.cpp
	$(CXX) $(CXXFLAGS) $(LIBCGICC_CXXFLAGS) -c $< -o $@

clean:
	rm -f $(CLEANFILES)
