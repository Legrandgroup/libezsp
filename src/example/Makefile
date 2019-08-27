LOCAL_LDFLAGS ?= -L/home/seb/_DEV/serial/compiled/li
LOCAL_INC ?= -I/home/seb/_DEV/serial/compiled/include

CXXFLAGS = -DUSE_SERIALCPP -W -Wall -pedantic -std=c++11 -Wno-unused-parameter -g -Weffc++
LDFLAGS = $(LOCAL_LDFLAGS) -lserialcpp -lpthread

# SRC_PATH should point to the src/ folder containing source code for this library (can be overridden from environment)
SRC_PATH ?= ..
SRC_DOMAIN_PATH ?= $(SRC_PATH)/domain
SRC_SPI_PATH ?= $(SRC_PATH)/spi

include ../libezsp.mk.inc

SRCS = $(SRC_PATH)/example/mainEzspTest.cpp \
       $(SRC_PATH)/example/CAppDemo.cpp \
       $(SRC_PATH)/example/dummy_db.cpp \
       $(LIBEZSP_LINUX_SERIALCPP_SRC) \

OBJECTFILES = $(patsubst %.cpp, %.o, $(SRCS))

EXEC = mainEzspTest

#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

CLEANFILES = $(OBJECTFILES) $(EXEC)
INC = $(LOCAL_INC) $(LIBEZSP_COMMON_INC)

all: $(EXEC)

$(EXEC): $(OBJECTFILES)
	@echo Linking $@
	$(SILENCE)$(CXX) $(OBJECTFILES) $(LDFLAGS) $(LIBCGICC_LDFLAGS) -o $(EXEC)

%.o: %.cpp
	@echo Compiling $<
	$(SILENCE)$(CXX) $(CXXFLAGS) $(LIBCGICC_CXXFLAGS) $(INC) -c $< -o $@

rebuild: clean-all all

clean:
	rm -f $(CLEANFILES)

clean-all: clean
