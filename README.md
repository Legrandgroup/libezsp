# libezsp
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=libezsp&metric=alert_status)](https://sonarcloud.io/dashboard?id=libezsp)

C++ library to send/receive wireless traffic to/from a UART transceiver using the EZSP protocol from Silicon Labs.
The code for a sample demo program is located in `src/example/mainEzspTest.cpp` and `src/example/CAppDemo.cpp`, this code is designed to work seamlessly on Linux within or outside of the Raritan framework.

## Compiling

### In the Raritan environment

If you compile in the Raritan environment, you will just have to clone this code into a subfolder or the source code root, move into this folder, and issue the following command:
```
./Build && cp ./build-for-target/example/mainEzspTest ../install_root/bin
```

By default, only the shared library is copied over to the target rootfs, this is why we also manually copy the example binary (mainEzspTest) to `../install_root/bin/mainEzspTest`
This binary will be directly be accessible on the target, and in the default search path, so that it can run from a terminal on the target, by issueing:
```
mainEzspTest --help
```

### Using libserialcpp and C++11 threads under Linux

In order to compile for Linux, you will first need a working installation of [libserialcpp](https://github.com/Legrandgroup/serial).

Let's assume the source code for libserialcpp is checked out in directory `~/serial` (this will be the assumption in all lines below).
In order to compile libserialcpp, type the following commands:
```
cd ~/serial
make -f Makefile.linux-nocmake all
```

This should result in a binary shared library built as file `~/serial/libserialcpp.so`

Now, we have to compile libezsp pointing it to the libserialcpp library we have just generated (in the example below, we assume the sources for libezsp are located in directory `~/libezsp`).
Issue the following commands in order to compile libezsp:
```
cd ~/libezsp
LDFLAGS=-L$HOME/serial cmake -DCMAKE_CXX_FLAGS=-isystem\ $HOME/serial/include/ -DUSE_CPPTHREADS=ON
make
```

Additional environment variables tell the compiler that libserialcpp.so can be found in `$HOME/serial` and headers are in `$HOME/serial/include` (this should be the default after the libserialcpp compilation steps above).

In order to run the sample code under Linux, issue the following command in a terminal:
```
cd ~/libezsp
LD_LIBRARY_PATH=$HOME/serial ./example/mainEzspTest -C 11 -c 26 -r '*' -s '0x01510004/0123456789abcdef0123456789abcdef' -d
```

The example above will open Green Power commissionning mode for 11s, and use the Zigbee channel 26 (for both Zigbee and Green Power transmission and reception).

The `-r` switch will flush any pre-existing known source ID & associated keys, and `-s` will manually add decoding support for a Green Power device with source ID 0x01510004 (and provides its associated 128-bit AES key).

### Execution

Note that the UART device for communication with the transceiver (eg: `/dev/ttyUSB0`) is hardcoded inside the code (file `src/example/mainEzspTest.cpp`)

We then library is run, it will first try to communicate with the dongle over the serial link provided above.

Once this is done, and when launched for the first time, the library will instruct the dongle to first create a network on the specified channel.
Each time the sample binary process is subsequently run, it will listen to sensor reports on that channel.
When the sensor sends periodically updated values of temperature/humidty via Green Power radio frames, the dongle will receive these, the library will handle this incoming traffic and values will displayed in real time on the output stream of the sample binary.

In order to force the sensor to send a report, you can also press on the button.

## For developpers

The developper-specific information can be found in [src/README.md](src/README.md)
