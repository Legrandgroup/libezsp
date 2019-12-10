/**
 * @file mainEzspTest.cpp
 * 
 * @brief Sample code for driving a dongle in the Raritan framework or using libserialcpp
 */

#include <iostream>

#ifdef USE_SERIALCPP
#include "spi/serial/SerialUartDriver.h"
#include "spi/cppthreads/CppThreadsTimerFactory.h"
#include <string>
#else
#ifdef USE_RARITAN
#include "spi/raritan/RaritanEventLoop.h"
#include "spi/raritan/RaritanUartDriver.h"
#include "spi/raritan/RaritanTimerFactory.h"
#else
#error Compiler directive USE_SERIALCPP or USE_RARITAN is required
#endif	// USE_RARITAN
#endif	// USE_SERIALCPP
#include "spi/GenericLogger.h"
#include "CAppDemo.h"
#include <getopt.h>
#include <string>
#include <iostream>
#include <sstream>
//#include <iomanip>	// For debug

static void writeUsage(const char* progname, FILE *f) {
    fprintf(f,"\n");
    fprintf(f,"%s - sample test program for libezsp\n\n", progname);
    fprintf(f,"Usage: %s [-d] [-u serialport] [-c channel] [-Z] [-C] [-G|[-r *|-r source_id [-r source_id2...]|-s source_id/key [-s source_id2/key...]]\n", progname);
    fprintf(f,"Available switches:\n");
    fprintf(f,"-h (--help)                       : this help\n");
    fprintf(f,"-d (--debug)                      : enable debug logs\n");
    fprintf(f,"-Z (--open-zigbee)                : open the zigbee network at startup (for 60s)\n");
    fprintf(f,"-G (--open-gp-commissionning)     : open the Green Power commissionning session at startup\n");
    fprintf(f,"-C (--authorize-ch-request-answer): Authorize answer to channel request command for x seconds\n");
    fprintf(f,"-u (--serial-port) <port>         : use a specific serial port (default: '/dev/ttyUSB0')\n");
    fprintf(f,"-c (--reset-to-channel) <channel> : force re-creation of a network on the specified channel (discards previously existing network)\n");
    fprintf(f,"-r (--remove-source-id) <id>      : remove a specific device from the monitored list, based on its source-id, use * to remove all (repeated -r options are allowed)\n");
    fprintf(f,"-s (--source-id) <id/key>         : adds a device to the monitored list, based on its source-id & key, id being formatted as a 8-digit hexadecimal string (eg: 'ffae1245'), and key as a 16-byte/32-digit hex string (repeated -s options are allowed)\n");
}

/**
 * @brief Convert an ASCII character representing one hexadecimal digit ([0-9a-fA-F]) to its value (0-15)
 *
 * @param[in] hDigit The input printable ASCII character
 * @param[out] byte The value of the hexadecimal digit as a uint8_t nibble (0-15)
 * @return true if the conversion succeeded
**/
static bool hexDigitToNibble(const char hDigit, uint8_t& byte) {
    if (hDigit>='a' && hDigit<='f')
        byte = static_cast<uint8_t>(hDigit + 10 - 'a');
    else if (hDigit>='A' && hDigit<='F')
        byte = static_cast<uint8_t>(hDigit + 10 - 'A');
    else if (hDigit>='0' && hDigit<='9')
        byte = static_cast<uint8_t>(hDigit - '0');
    else
        return false;
    return true;
}

int main(int argc, char **argv) {
#ifdef USE_SERIALCPP
    SerialUartDriver uartDriver;
    CppThreadsTimerFactory timerFactory;

    ConsoleLogger::getInstance().setLogLevel(LOG_LEVEL::DEBUG);	/* Only display logs for debug level info and higher (up to error) */
#endif
#ifdef USE_RARITAN
    RaritanEventLoop eventLoop;
    RaritanUartDriver uartDriver(eventLoop);
    RaritanTimerFactory timerFactory(eventLoop);
#endif

    int optionIndex=0;
    int c;
    bool debugEnabled = false;
    std::vector<CGpDevice> gpAddedDevDataList;
    std::vector<uint32_t> gpRemovedDevDataList;
    bool removeAllGpDevs = false;
    unsigned int resetToChannel = 0;
    std::string serialPort("/dev/ttyUSB0");
    bool openGpCommissionningAtStartup = false;
    bool openZigbeeNetworkAtStartup = false;
    uint8_t authorizeChRqstAnswerTimeout = 0U;

    static struct option longOptions[] = {
        {"reset-to-channel", 1, 0, 'c'},
        {"source-id", 1, 0, 's'},
        {"remove-source-id", 1, 0, 'r'},
        {"serial-port", 1, 0, 'u'},
        {"open-zigbee", 0, 0, 'Z'},
        {"open-gp-commissionning", 0, 0, 'G'},
        {"authorize-ch-request-answer", 1, 0, 'C'},
        {"debug", 0, 0, 'd'},
        {"help", 0, 0, 'h'},
        {0, 0, 0, 0}
    };
    while ( (c = getopt_long(argc, argv, "dhZGs:r:u:c:C:", longOptions, &optionIndex)) != -1) {
        switch (c) {
            case 's':
            {
                if (gpRemovedDevDataList.size() || removeAllGpDevs) {
                    std::cerr << "-s option cannot be used if -r was used\n";
                    exit(1);
                }
                std::istringstream gpDevDataStream(optarg);
                std::string gpDevSourceIdstr;
                if (std::getline(gpDevDataStream, gpDevSourceIdstr, '/')) {
                    std::stringstream gpDevSourceIdStream;
                    gpDevSourceIdStream << std::hex << gpDevSourceIdstr;
                    unsigned int sourceIdValue;
                    gpDevSourceIdStream >> sourceIdValue;
                    if (sourceIdValue < static_cast<uint32_t>(-1)) {	/* Protection against overflow */
                        //std::cerr << "Read source ID part of arg: " << std::hex << std::setw(8) << std::setfill('0') << sourceIdValue << "\n";
                        std::string gpDevKeyStr;
                        gpDevDataStream >> gpDevKeyStr;	/* Read everything after the separator, which should be the key */
                        //std::cerr << "Read key part of arg: " << gpDevKeyStr << "\n";
                        if (gpDevKeyStr.length() != 32) {
                            clogE << "Invalid key length: " << gpDevKeyStr << " (should be 16-bytes long).\n";
                            exit(1);
                        }
                        else {
                            EmberKeyData keyValue(CGpDevice::UNKNOWN_KEY);
                            if (gpDevKeyStr != "") {
                                std::vector<uint8_t> argAsBytes;
                                for (unsigned int i = 0; i < gpDevKeyStr.length(); i += 2) {
                                    uint8_t hiNibble;
                                    if (!hexDigitToNibble(gpDevKeyStr[i], hiNibble)) {
                                        clogE << "Invalid character '" << gpDevKeyStr[i] << "' at position " << i+1 << " in key " << gpDevKeyStr << "\n"; /* Note: 1st char is identified by a position=1 for readability */
                                        exit(1);
                                    }
                                    uint8_t loNibble;
                                    if (!hexDigitToNibble(gpDevKeyStr[i+1], loNibble)) {
                                        clogE << "Invalid character '" << gpDevKeyStr[i+1] << "' at position " << i+2 << " in key " << gpDevKeyStr << "\n"; /* Note: 1st char is identified by a position=1 for readability */
                                        exit(1);
                                    }
                                    argAsBytes.push_back(static_cast<uint8_t>(hiNibble << 4) | loNibble);
                                }
                                //for (uint8_t loop=0; loop<argAsBytes.size(); loop++) { std::cerr << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(argAsBytes[loop]); }
                                //std::cerr << "\n";
                                keyValue = EmberKeyData(argAsBytes);
                            }
                            gpAddedDevDataList.push_back(CGpDevice(sourceIdValue, keyValue));
                        }
                    }
                    else {
                        clogE << "Invalid source ID: " << optarg << "\n";
                    }
                }
            }
            break;
            case 'r':
            {
                std::string gpDevSourceIdstr(optarg);
                if (gpDevSourceIdstr == "*") {  /* Remove all source IDs */
                    if (gpRemovedDevDataList.size() || gpAddedDevDataList.size()) {
                        std::cerr << "-r * option cannot be used if another -r or -s was used\n";
                        exit(1);
                    }
                    removeAllGpDevs = true;
                }
                else {
                    if (gpAddedDevDataList.size() || removeAllGpDevs) {
                        std::cerr << "-r option cannot be used if -r '*' or -s was used\n";
                        exit(1);
                    }
                    std::stringstream gpDevSourceIdStream;
                    gpDevSourceIdStream << std::hex << gpDevSourceIdstr;
                    unsigned int sourceIdValue;
                    gpDevSourceIdStream >> sourceIdValue;
                    if (sourceIdValue < static_cast<uint32_t>(-1)) {	/* Protection against overflow */
                        //std::cerr << "Read source ID part of arg: " << std::hex << std::setw(8) << std::setfill('0') << sourceIdValue << "\n";
                        gpRemovedDevDataList.push_back(sourceIdValue);
                    }
                    else {
                        clogE << "Invalid source ID: " << optarg << "\n";
                    }
                }
            }
            break;
            case 'u':
                serialPort = optarg;
                break;
            case 'c':
                std::stringstream(optarg) >> resetToChannel;
                break;
            case 'G':
                openGpCommissionningAtStartup = true;
                break;
            case 'C':
                std::stringstream(optarg) >> authorizeChRqstAnswerTimeout;
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

    CAppDemo app(uartDriver, timerFactory, (resetToChannel!=0), openGpCommissionningAtStartup, authorizeChRqstAnswerTimeout, openZigbeeNetworkAtStartup, resetToChannel, removeAllGpDevs, gpRemovedDevDataList, gpAddedDevDataList);	/* If a channel was provided, reset the network and recreate it on the provided channel */

#ifdef USE_SERIALCPP
    std::string line;
    std::getline(std::cin, line);
#endif
#ifdef USE_RARITAN
    eventLoop.run();
#endif

    return 0;
}
