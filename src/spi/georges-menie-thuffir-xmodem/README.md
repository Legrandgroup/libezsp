This implementation of the XModem protocol has been copied over from https://github.com/Thuffir/xmodem
It is based on an initial implementation from Georges Menie, modified by [Thuffir](https://github.com/Thuffir) (github pseudo) to add support for XModem-1k (and YModem)

In order to compile this code, just issue the following command, from the directory containing these sources:
```
gcc xmodem.c main.c -o xmodem
```

In order to upgrade the dongle's firmware, you will need a properly compiled EZSP NCP firmware for Silabs targets (a gbl file).
In the examples below, you firmware is called `RB01924_xncp-led-EFR32MG13P632F512GM32-usb-dongle-stack-6.7.5.0+000010.gbl`

First, put your dongle in firmware upgrade mode.
In order to do this, you will need to run the mainEzspTest executable generated during the compilation of the libezsp sources (see [here](../../../README.md) for instructions on how to compile the libezsp sources).

Once you have a working mainEzspTest executable, just run it with the -w option:
```
cd ~/libezsp
LD_LIBRARY_PATH=$HOME/serial ./example/mainEzspTest -r '*' -w -d
```

You should see the dongle switch to firmware upgrade mode by observing a message similar to the following one in the last lines dumped to the console:
```
EZSP adapter is now ready to receive a firmware image (.gbl) via X-modem
```

You then have around 20s to initiate the firmware upload using X-modem.

In order to do so, use the xmodem executable generated above, by running something like:
```
./xmodem ~/Downloads/RB01924_xncp-led-EFR32MG13P632F512GM32-usb-dongle-stack-6.7.5.0+000010.gbl /dev/ttyUSB0
```

This will upgrade the firmware of a dongle (using the file provided as argument: RB01924_xncp-led-EFR32MG13P632F512GM32-usb-dongle-stack-6.7.5.0+000010.gbl, and assuming your dongle is recognized as `/dev/ttyUSB0`).

The source code for the xmodem exacutable was released by Geogres Menie as under the BSD license.

Specifications for XMODEM can be found [here](https://www.menie.org/georges/embedded/xmodem_specification.html)

Another implementation, released under MIT, has [been done by zonque (Daniel Mack)](https://gist.github.com/zonque/0ae2dc8cedbcdbd9b933)
