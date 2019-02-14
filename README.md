# libezsp

C++ library to send/receive wireless traffic to/from a UART transceiver using the EZSP protocol from Silicon Labs.
A sample demo program is located in `src/example/mainEzspTest.cpp` and `src/example/CAppDemo.cpp`

## Compiling

### In the Raritan environment

If you compile in the Raritan environment, you will just have to clone this code into a subfolder or the source code root, move into this folder, and issue the following command:
```
./Build
```

The newly generated binary will be located in `../install_root/bin/mainEzspTest` (so, on the qemu installation, it will directly be accessible by running (no the target):
```
mainEzspTest
```

### Using libserialcpp and C++11 threads under Linux

In order to compile for Linux, you will first need a working installation of [libserialcpp](https://github.com/Legrandgroup/serial)
Let's assumethe code for libserialcpp is checked out in directory `~/serial` (this will be the assumption in all lines below)
In order to compile libserialcpp, type the following commands:
```
cd ~/serial
make -f Makefile.linux-nocmake all
```

This should result in a shared library build as `~/serial/libserialcpp.so`

Now, we have to compile libezsp using libserialcpp we have just generated (in the example below, we assume the sources are located in directory `~/libezsp`).
Issue the following commands in order to compile libezsp:
```
cd ~/libezsp/src/example
LOCAL_LDFLAGS=-L$HOME/serial LOCAL_INC=-I$HOME/serial/include make
```

This will tell the compiler that libserialcpp.so can be found in $HOME/serial and geaders are in $HOME/serial/include (this should be the default after the libserialcpp compilation steps above)

In order to run the sample code, just issue:
```
LD_LIBRARY_PATH=$HOME/serial mainEzspTest
```

### Execution

Note that the UART device for communication with the transceiver (eg: /dev/ttyUSB0) is hardcoded inside the code (file `src/example/mainEzspTest.cpp`)

Once the binary has been copied over to the target, you can simply run it.

When launched for the first time, the dongle will first create a network.
Each time the sample binary process is launched, it will open its network for any device to join for a limited period of time. You can thus enter a sensor in the network at this moment by pressing the button on the device.

The sample process will then automatically discover sensors and bind to temperature and humidity, and configure reporting to be sent to the dongle by the sensor.

Finally, the sensor will periodically send updated values of temperature/humidty, they will displayed in real time on the output stream of the sample process.

In order to force the sensor to send a report, you can also press 4 times on the button.

## For developpers

The developper-specific information can be found in [src/README.md]
