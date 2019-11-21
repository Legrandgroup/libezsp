/**
 * @file mainEzspTest.cpp
 * 
 * @brief Sample code for driving a dongle in the Raritan framework or using libserialcpp
 */

#include <iostream>

#ifdef USE_SERIALCPP
#include "../spi/serial/SerialUartDriver.h"
#include "../spi/cppthreads/CppThreadsTimerFactory.h"
#include <string>
#else
#ifdef USE_RARITAN
#include "../spi/raritan/RaritanEventLoop.h"
#include "../spi/raritan/RaritanUartDriver.h"
#include "../spi/raritan/RaritanTimerFactory.h"
#else
#error Compiler directive USE_SERIALCPP or USE_RARITAN is required
#endif	// USE_RARITAN
#endif	// USE_SERIALCPP
#include "../spi/GenericLogger.h"
#include "CAppDemo.h"
#include <getopt.h>
#include <string>
#include <iostream>
#include <sstream>

static void writeUsage(const char* progname, FILE *f) {
    fprintf(f,"\n");
    fprintf(f,"%s - sample test program for libezsp\n\n", progname);
    fprintf(f,"Usage: %s [-d] [-r channel] [-s source_id [-s source_id2...]]\n", progname);
    fprintf(f,"Available switches:\n");
    fprintf(f,"-h (--help)                       : this help\n");
    fprintf(f,"-d (--debug)                      : enable debug logs\n");
    fprintf(f,"-Z (--open-zigbee)                : open the zigbee network at startup (for 60s)\n");
    fprintf(f,"-G (--open-gp-commissionning)     : open the Green Power commissionning session at startup\n");
    fprintf(f,"-u (--serial-port) <port>         : use a specific serial port (default: '/dev/ttyUSB0')\n");
    fprintf(f,"-r (--reset-to-channel) <channel> : force re-creation of a network on the specified channel (discards previously existing network)\n");
    fprintf(f,"-s (--source-id) <source-id>      : enables receiving from a device with this source-id, formatted as a 8-digit hexadecimal string (eg: 'ffae1245') (repeated -s options are allowed)\n");
}

int main(int argc, char **argv) {
#ifdef USE_SERIALCPP
    SerialUartDriver uartDriver;
    CppThreadsTimerFactory timerFactory;

    ConsoleLogger::getInstance().setLogLevel(LOG_LEVEL::INFO);	/* Only display logs for debug level info and higher (up to error) */
#endif
#ifdef USE_RARITAN
    RaritanEventLoop eventLoop;
    RaritanUartDriver uartDriver(eventLoop);
    RaritanTimerFactory timerFactory(eventLoop);
#endif

    int optionIndex=0;
    int c;
    bool debugEnabled = false;
    std::vector<CGpDevice> gpDevDataList;
    unsigned int resetToChannel = 0;
    std::string serialPort("/dev/ttyUSB0");
    bool openGpCommissionningAtStartup = false;
    bool openZigbeeNetworkAtStartup = false;

    static struct option longOptions[] = {
        {"reset-to-channel", 1, 0, 'r'},
        {"source-id", 1, 0, 's'},
        {"serial-port", 1, 0, 'u'},
        {"open-zigbee", 0, 0, 'Z'},
        {"open-gp-commissionning", 0, 0, 'G'},
        {"debug", 0, 0, 'd'},
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}
    };
    while ( (c = getopt_long(argc, argv, "dhZGs:u:r:", longOptions, &optionIndex)) != -1) {
        switch (c) {
            case 's':
            {
                std::stringstream sourceId;
                sourceId << std::hex << optarg;
                unsigned int sourceIdValue;
                sourceId >> sourceIdValue;
                if (sourceIdValue<static_cast<uint32_t>(-1)) {	/* Protection against overflow */
                    gpDevDataList.push_back(CGpDevice(sourceIdValue, CGpDevice::UNKNOWN_KEY));
                }
                else {
                    clogE << "Invalid source ID: " << optarg << "\n";
                }

            }
            break;
            case 'u':
                serialPort = optarg;
                break;
            case 'r':
                std::stringstream(optarg) >> resetToChannel;
                break;
            case 'G':
                openGpCommissionningAtStartup = true;
                break;
            case 'Z':
                openZigbeeNetworkAtStartup = true;
                break;
            case 'd':
                debugEnabled = true;
                break;
            case 'h':
                writeUsage(argv[0], stdout);
                exit(0);
            case '?':
            default:
                std::cerr << "Unsupported command-line option. Exitting\n";
                writeUsage(argv[0], stdout);
                exit(1);
        }
    }

#ifdef USE_RARITAN
    RaritanLogger::getInstance().setLogLevel(debugEnabled ? LOG_LEVEL::DEBUG : LOG_LEVEL::INFO);
#endif

    clogI << "Starting ezsp test program (info)\n";

    if (uartDriver.open(serialPort, 57600) != 0) {
        clogE << "Failed opening serial port. Aborting\n";
        return 1;
    }

    CAppDemo app(uartDriver, timerFactory, (resetToChannel!=0), openGpCommissionningAtStartup, openZigbeeNetworkAtStartup, resetToChannel, gpDevDataList);	/* If a channel was provided, reset the network and recreate it on the provided channel */

#ifdef USE_SERIALCPP
    std::string line;
    std::getline(std::cin, line);
#endif
#ifdef USE_RARITAN
    eventLoop.run();
#endif

    return 0;
}
