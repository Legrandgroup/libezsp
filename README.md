# libezsp

C++ library to send/receive wireless traffic to/from a UART transceiver using the EZSP protocol from Silicon Labs.
A sample demo program is located in src/test/

## For users

In order to compile for Linux, you will first need a working installation of [libserialcpp](https://github.com/Legrandgroup/serial)
If you compile in the Raritan environment, you will just have to clone this code into a subfolder or the source code root, move into this folder, and issue the following command:
```
./Build
```

Note that the UART device for communication with the transceiver (eg: /dev/ttyUSB0) is hardcoded inside the code (file `src/test/mainEzspTest*.cpp`)

Once the binary has been copied over to the target, you can simply run it.

When launched for the first time, the dongle will first create a network.
Each time the sample binary process is launched, it will open its network for any device to join for a limited period of time. You can thus enter a sensor in the network at this moment by pressing the button on the device.

The sample process will then automatically discover sensors and bind to temperature and humidity, and configure reporting to be sent to the dongle by the sensor.

Finally, the sensor will periodically send updated values of temperature/humidty, they will displayed in real time on the output stream of the sample process.

In order to force the sensor to send a report, you can also press 4 times on the button.

## For developpers

### Source code organisation

This repository is organised following DDD conventions

* `src/domain` contains most of the library code (EZSP/ASH encoding/decoding and protocol sequence implementation)
* `src/spi` contains the adapters for platform-specific implementations of timers and serial port access
* `src/spi/serial` contains the concrete implementations of adapters for Linux (using libserialcpp and std::thread)
* `src/spi/raritan` contains the concrete implementations of adapters for the Raritan framework (using selectors and an event-driven main loop)
* `src/test` contains a sample demo program to read and report sensor values from a zigbee network
