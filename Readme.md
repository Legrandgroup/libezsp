# libezsp

C++ library to send/receive wireless traffic to/from a UART transceiver using the EZSP protocol from Silicon Labs.
A sample demo program is located in src/test/

## For users

In order to compile for Linux, you will first need a working installation of [libserialcpp](https://github.com/Legrandgroup/serial)
If you compile in the Raritan environment, you will just have to clone this code into a subfolder or the source code root, move into this folder, and issue the following command:
```
./Build
```

## For developpers

### Source code organisation

This repository is organised following DDD conventions

* `src/domain` contains most of the library code (EZSP/ASH encoding/decoding and protocol sequence implementation)
* `src/spi` contains the adapters for platform-specific implementations of timers and serial port access
* `src/spi/serial` contains the concrete implementations of adapters for Linux (using libserialcpp and std::thread)
* `src/spi/raritan` contains the concrete implementations of adapters for the Raritan framework (using selectors and an event-driven main loop)
* `src/test` contains a sample demo program to read and report sensor values from a zigbee network
