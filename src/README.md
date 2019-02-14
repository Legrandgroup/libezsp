# libezsp developper information

## Source code organisation

This repository is organised following DDD conventions.
This means that the core code of the library (domain) is isolated from the code that allows this library to make use of external resources or library (spi) and from translation code that makes information available from this library to other software (api).

* `src/domain` contains most of the library code (EZSP/ASH encoding/decoding and protocol sequence implementation)
* `src/spi` contains the adapters for platform-specific implementations of timers and serial port access
* `src/spi/serial` contains the concrete implementations of an adapter to read/write on the serial port for Linux and Windows (using libserialcpp)
* `src/spi/cppthreads` contains the concrete implementation of a timer using C++11 threads
* `src/spi/mock-uart` contains a framework to emulate a serial port for unit testing
* `src/spi/raritan` contains the concrete implementations of adapters for the Raritan framework (using selectors and an event-driven main loop)
* `src/example` contains the code for a sample demo program to read and report sensor values from a zigbee network

SPI connectors implement a concrete implementation that complies to a specified interface declared in interface headers (.h header files which filename starts with a capital I).
This allows dependency inversion paradigm, where the connector depends on the library, rather than the library depends on the underlying connectors. Connectors then become interchangeable, and we use this method a lot to abstract the library from the services it uses (a version of these services are using the Raritan framework, another version is independent from the Raritan framework and can run outside of it).

## SPI interface definition

* spi/ITimer.h and spi/ITimerFactory.h
  Abstract interface to which must conforms implementations of classes that handle timed callbacks and factory classes that generate the associated ITimer objects
  Concrete implementation in the raritan framework:
  * spi/raritan/RaritanTimer.{h,cpp}
  * spi/raritan/RaritanTimerFactory.{h,cpp}
  Concrete implementation using C++ threads:
  * spi/cppthreads/CppThreadsTimerFactory.{h,cpp}
  * spi/cppthreads/CppThreadsTimer.{h,cpp}
* spi/IAsyncDataInputObserver.h
  Abstract interface to which must conforms implementations of classes that get asynchronous notifications on incoming data
  This is an exception, it is not a DIP implementation, the concrete implementation lies inside the applicative code (inside the library)
* spi/IUartDriver.h
  Abstract interface to which must conforms concrete implementations of classes that manipulate UARTs
  Concrete implementation in the raritan framework:
  * spi/raritan/RaritanUartDriver.{h,cpp}
  Concrete implementation using libserial:
  * spi/serial/SerialUartDriver.{h,cpp}
  Concrete implementation of a robotized emulated serial port for unit testing:
  * spi/mock-uart/MockUartDriver.{h,cpp}

## Generating doxygen documentation

In order to generate the source code documentation out of the doxygen tags contained inside the code, run the following command:
```
cd ~/libezsp
doxygen Doxyfile.dev
```

The resulting doc will be available via a browser by opening `~/libezsp/doc/html/index.html