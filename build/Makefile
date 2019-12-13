include build/scripts.mk

package=ezsp
version=1.1

subdir-y+=src

bin-$(TEST)+=mainEzspTest

mainEzspTest_CFLAGS+=-Isrc
mainEzspTest_LDFLAGS+=-Lsrc

mainEzspTest_SOURCES+=example/CAppDemo.cpp
mainEzspTest_SOURCES+=example/mainEzspTest.cpp
mainEzspTest_SOURCES+=example/dummy_db.cpp
mainEzspTest_LIBRARY+=ezsp
mainEzspTest_LIBRARY+=ezpspi
mainEzspTest_LIBRARY+=serialcpp
