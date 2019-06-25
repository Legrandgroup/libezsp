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
#include "GenericLogger.h"
#include "CAppDemo.h"
#include <getopt.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

static void writeUsage(const char* progname, FILE *f) {
    fprintf(f,"\n");
    fprintf(f,"%s - sample test program for libezsp\n\n", progname);
    fprintf(f,"Usage: %s [-d] [-r channel] [-s source_id [-s source_id2...]]\n", progname);
    fprintf(f,"Available switches:\n");
    fprintf(f,"-h (--help)                       : this help\n");
    fprintf(f,"-d (--debug)                      : enable debug logs\n");
    fprintf(f,"-r (--reset-to-channel) <channel> : force re-creation of a network on the specified channel (discards previously existing network)\n");
    fprintf(f,"-s (--source-id) <source-id>      : enables receiving from a device with this source-id (repeated -s options are allowed)\n");
}

int main(int argc, char **argv) {
#ifdef USE_SERIALCPP
    SerialUartDriver uartDriver;
    CppThreadsTimerFactory timerFactory;
#endif
#ifdef USE_RARITAN
    RaritanEventLoop eventLoop;
    RaritanUartDriver uartDriver(eventLoop);
    RaritanTimerFactory timerFactory(eventLoop);
#endif

    int optionIndex=0;
    int c;
    bool debugEnabled = false;
    std::vector<std::string> sourceIdList;
    unsigned int resetToChannel = 0;
    std::string serialPort("/dev/ttyUSB0");

    static struct option longOptions[] = {
        {"reset-to-channel", 1, 0, 'r'},
        {"source-id", 1, 0, 's'},
        {"serial-port", 1, 0, 'u'},
        {"debug", 0, 0, 'd'},
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}
    };
    while ( (c = getopt_long(argc, argv, "dhs:u:r:", longOptions, &optionIndex)) != -1) {
		switch (c) {
			case 's':
				sourceIdList.push_back(std::string(optarg));
				break;
			case 'u':
				serialPort = optarg;
				break;
			case 'r':
				stringstream(optarg) >> resetToChannel;
				break;
			case 'd':
				debugEnabled = true;
				break;
			case 'h':
				writeUsage(argv[0], stdout);
				exit(0);
			case '?':
			default:
				std::cerr << "Unknown command-line option\n";
				writeUsage(argv[0], stdout);
				exit(1);
		}
    }

#ifdef USE_RARITAN
    RaritanLogger::getInstance().setLogLevel(debugEnabled ? LOG_LEVEL::DEBUG : LOG_LEVEL::INFO);
#endif

    clogI << "Starting ezsp test program (info)\n";

    uartDriver.open(serialPort, 57600);

    CAppDemo app(uartDriver, timerFactory, (resetToChannel!=0), resetToChannel);	/* If a channel was provided, reset the network and recreate it on the provided channel */

#ifdef USE_SERIALCPP
    std::string line;
    std::getline(std::cin, line);
#endif
#ifdef USE_RARITAN
    eventLoop.run();
#endif

    return 0;
}
