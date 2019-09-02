CXXFLAGS = -DUSE_SERIALCPP -W -Wall -pedantic -std=c++11 -Wno-unused-parameter -g -Weffc++
LDFLAGS = $(LOCAL_LDFLAGS) -lpthread

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

# SRC_PATH should point to the src/ folder containing source code for this library (can be overridden from environment)
SRC_PATH ?= ..
SRC_DOMAIN_PATH ?= $(SRC_PATH)/domain
SRC_SPI_PATH ?= $(SRC_PATH)/spi

include ../libezsp.mk.inc

SRCS = $(SRC_PATH)/tests/mock_serial_self_tests.cpp \
       $(SRC_PATH)/tests/gp_tests.cpp \
       $(SRC_PATH)/tests/test_libezsp.cpp \
       $(SRC_PATH)/example/dummy_db.cpp \
       $(SRC_PATH)/example/CAppDemo.cpp \
       $(LIBEZSP_LINUX_MOCKSERIAL_SRC) \

OBJECTFILES = $(patsubst %.cpp, %.o, $(SRCS))

EXEC = test_runner

#Set this to @ to keep the makefile quiet
ifndef SILENCE
	SILENCE = @
endif

# get rid of built-in rules
.SUFFIXES:

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
	@rm -f $(CLEANFILES)
	@rm -f $(EXEC)

clean-all: clean

check: $(EXEC)
	./$<
