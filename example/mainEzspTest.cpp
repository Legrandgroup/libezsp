/**
 * @file mainEzspTest.cpp
 *
 * @brief Sample code for driving a dongle in the Raritan framework or using libserialcpp
 */

#include <iostream>

#include "spi/TimerBuilder.h"
#include "spi/UartDriverBuilder.h"
#include "spi/Logger.h"
#include <getopt.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#ifdef USE_RARITAN
#include <pp/Selector.h>
#endif
#ifdef USE_CPPTHREADS
#include <thread>
#include <condition_variable>
#include <csignal>
#endif

#include <ezsp/ezsp.h>
#include <ezsp/byte-manip.h>

#include "mainEzspStateMachine.h"

#ifdef USE_CPPTHREADS
static bool stop = false;
static std::condition_variable cv;
static std::mutex m;
#endif

static void writeUsage(const char* progname, FILE *f) {
    fprintf(f,"\n");
    fprintf(f,"%s - sample test program for libezsp\n\n", progname);
    fprintf(f,"Usage: %s [-d] [-u serialport] [-w|[-c channel] [-Z] [-C time] [-G|[-r *|-r source_id [-r source_id2...]] [-s source_id/key [-s source_id2/key...]]]\n", progname);
    fprintf(f,"Available switches:\n");
    fprintf(f,"-h (--help)                               : this help\n");
    fprintf(f,"-d (--debug)                              : enable debug logs\n");
    fprintf(f,"-b (--baudrate) <baudrate>                : baudrate used to communicate over the serial port\n");
    fprintf(f,"-w (--firmware-upgrade)                   : put the adapter in firmware upgrade mode and return when done\n");
    fprintf(f,"-Z (--open-zigbee)                        : open the zigbee network at startup (for 60s)\n");
    fprintf(f,"-G (--open-gp-commissionning)             : open the Green Power commissionning session at startup\n");
    fprintf(f,"-C (--authorize-ch-request-answer) <time> : Allow answers to unauthenticated (maintenance) channel requests for 0<time<255 seconds. Note: responses to MSP authenticated requests is always allowed\n");
    fprintf(f,"-u (--serial-port) <port>                 : use a specific serial port (default: '/dev/ttyUSB0')\n");
    fprintf(f,"-c (--reset-to-channel) <channel>         : force re-creation of a network on the specified channel (discards previously existing network)\n");
    fprintf(f,"-r (--remove-source-id) <source_id>       : remove a specific device from the monitored list, based on its source-id, use * to remove all (repeated -r options are allowed)\n");
    fprintf(f,"-s (--source-id) <source_id/key>          : adds a device to the monitored list, based on its source-id & key, id being formatted as a 8-digit hexadecimal string (eg: 'ffae1245'), and key as a 16-byte/32-digit hex string (repeated -s options are allowed)\n");
}

int main(int argc, char **argv) {
    NSSPI::IUartDriver *uartDriver = NSSPI::UartDriverBuilder::getInstance();
    NSSPI::TimerBuilder timerBuilder;
    int optionIndex=0;
    int c;
    bool debugEnabled = false;
    bool switchToFirmwareUpgradeMode = false;
    std::vector<NSEZSP::CGpDevice> gpAddedDevDataList;
    std::vector<uint32_t> gpRemovedDevDataList;
    bool removeAllGpDevs = false;
    unsigned int resetToChannel = 0;
    std::string serialPort("/dev/ttyUSB0");
    bool openGpCommissionningAtStartup = false;
    bool openZigbeeNetworkAtStartup = false;
    uint8_t authorizeChRqstAnswerTimeout = 0U;
    int baudrate = 115200;

    static struct option longOptions[] = {
        {"reset-to-channel", 1, nullptr, 'c'},
        {"source-id", 1, nullptr, 's'},
        {"baudrate", 1, nullptr, 'b'},
        {"remove-source-id", 1, nullptr, 'r'},
        {"serial-port", 1, nullptr, 'u'},
        {"open-zigbee", 0, nullptr, 'Z'},
        {"open-gp-commissionning", 0, nullptr, 'G'},
        {"authorize-ch-request-answer", 1, nullptr, 'C'},
        {"firmware-upgrade", 0, nullptr, 'w'},
        {"debug", 0, nullptr, 'd'},
        {"help", 0, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };
    while ( (c = getopt_long(argc, argv, "dhwZGs:b:r:u:c:C:", longOptions, &optionIndex)) != -1) {
        switch (c) {
            case 's':
            {
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
                        if (gpDevKeyStr.length() != 2*16) {   /* 2 hex digits per byte (16=EMBER_KEY_DATA_BYTE_SIZE) */
                            clogE << "Invalid key length: " << gpDevKeyStr << " (should be 16-bytes long).\n";
                            return 1;
                        }
                        else {
                            NSEZSP::EmberKeyData keyValue(NSEZSP::CGpDevice::UNKNOWN_KEY);
                            if (gpDevKeyStr != "") {
                                std::vector<uint8_t> argAsBytes;
                                for (unsigned int i = 0; i<16; i++) {
                                    uint8_t hiNibble;
                                    if (!NSMAIN::hexDigitToNibble(gpDevKeyStr[i*2], hiNibble)) {
                                        clogE << "Invalid character '" << gpDevKeyStr[i*2] << "' at position " << i*2+1 << " in key " << gpDevKeyStr << "\n"; /* Note: 1st char is identified by a position=1 and not index 0 for readability */
                                        return 1;
                                    }
                                    uint8_t loNibble;
                                    if (!NSMAIN::hexDigitToNibble(gpDevKeyStr[i*2+1], loNibble)) {
                                        clogE << "Invalid character '" << gpDevKeyStr[i*2+1] << "' at position " << i*2+2 << " in key " << gpDevKeyStr << "\n"; /* Note: 1st char is identified by a position=1 and not index 0 for readability */
                                        return 1;
                                    }
                                    keyValue.at(i) = (static_cast<uint8_t>(hiNibble << 4) | loNibble);
                                }
                            }
                            gpAddedDevDataList.push_back(NSEZSP::CGpDevice(sourceIdValue, keyValue));
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
                    if (gpRemovedDevDataList.size()) {
                        std::cerr << "-r * option cannot be used if another -r was used\n";
                        return 1;
                    }
                    removeAllGpDevs = true;
                }
                else {
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
            case 'b':
                baudrate = strtol(optarg, NULL, 10);
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
                authorizeChRqstAnswerTimeout = std::stoi(optarg);
                break;
            case 'Z':
                openZigbeeNetworkAtStartup = true;
                break;
            case 'w':
                switchToFirmwareUpgradeMode = true;
                break;
            case 'd':
                debugEnabled = true;
                break;
            case 'h':
                writeUsage(argv[0], stdout);
                return 0;
            case '?':
            default:
                std::cerr << "Unsupported command-line option. Exitting\n";
                writeUsage(argv[0], stdout);
                return 1;
        }
    }

    NSSPI::Logger::getInstance()->setLogLevel(debugEnabled ? NSSPI::LOG_LEVEL::DEBUG : NSSPI::LOG_LEVEL::INFO);

    clogI << "Starting ezsp test program (info)\n";

    if (uartDriver->open(serialPort, baudrate) != 0) {
        clogE << "Failed opening serial port. Aborting\n";
        return 1;
    }

#ifdef USE_CPPTHREADS
    auto sighandler = [](int signal) {
      stop = true;
      cv.notify_one();
    };
    std::signal(SIGINT, sighandler);
#endif
    NSEZSP::CEzsp lib_main(uartDriver, timerBuilder, resetToChannel);	/* If a channel was provided, reset the network and recreate it on the provided channel */
    NSMAIN::MainStateMachine fsm(timerBuilder, lib_main, openGpCommissionningAtStartup, authorizeChRqstAnswerTimeout, openZigbeeNetworkAtStartup, removeAllGpDevs, gpAddedDevDataList, gpRemovedDevDataList, switchToFirmwareUpgradeMode);
    auto clibobs = [&fsm, &lib_main](NSEZSP::CLibEzspState i_state) {
        bool terminate = false; /* Shall we terminate the current process? */
        bool failure = false;   /* Shall we exit with a failure? */
        if (i_state == NSEZSP::CLibEzspState::IN_XMODEM_XFR) {
            terminate = true;
            failure = false;
        }
        try {
                fsm.ezspStateChangeCallback(i_state);
        } catch (const std::exception& e) {
            clogE << "Aborting\n";
            terminate = true;
            failure = true;
        }
        if (terminate) {
    #ifdef USE_RARITAN
            exit(failure?1:0);
    #endif
    #ifdef USE_CPPTHREADS
            stop = true;
            cv.notify_one();
    #endif
        }
    };
    lib_main.registerLibraryStateCallback(clibobs);

    auto gprecvobs = [&fsm](NSEZSP::CGpFrame &i_gpf) {
            fsm.onReceivedGPFrame(i_gpf);
    };
    lib_main.registerGPFrameRecvCallback(gprecvobs);

    // Sample incoming greenpower sourceId callback
    // auto cgpidobs = [](uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status) {
    //     clogI << "greenpower sourcedId: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(i_gpd_id) <<
    //              ", known: " << (i_gpd_known?"true":"false") << ", key status: " <<  std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_gpd_key_status) <<
    //              "\n";
    // };
    // lib_main.registerGPSourceIdCallback(cgpidobs);
    lib_main.start();

#ifdef USE_RARITAN
    pp::Selector& eventSelector(*pp::SelectorSingleton::getInstance());
    eventSelector.run();
#endif
#ifdef USE_CPPTHREADS
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{return stop;});
#endif
    clogI << "goodbye" << std::endl;
    return 0;
}
