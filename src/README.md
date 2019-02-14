# libezsp developper information

## Source code organisation

This repository is organised following DDD conventions.
This means that the core code of the library (domain) is isolated from the code that allows this library to make use of external resources or library (spi) and from translation code that makes information available from this library to other software (api).

* `src/domain` contains most of the library code (EZSP/ASH encoding/decoding and protocol sequence implementation)
* `src/spi` contains the adapters for platform-specific implementations of timers and serial port access
* `src/spi/serial` contains the concrete implementations of adapters for Linux (using libserialcpp and std::thread)
* `src/spi/raritan` contains the concrete implementations of adapters for the Raritan framework (using selectors and an event-driven main loop)
* `src/example` contains a sample demo program to read and report sensor values from a zigbee network
