/**
 * @file mainEzspTest.cpp
 * 
 * @brief Sample code for driving a dongle in the Raritan framework or using libserialcpp
 */

#include <iostream>

#include "spi/TimerBuilder.h"
#include "spi/UartDriverBuilder.h"
#include "spi/Logger.h"
#include "ezsp/lib-ezsp-main.h"
#include <getopt.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
//#include <iomanip>	// For debug
#ifdef USE_RARITAN
#include <pp/Selector.h>
#endif

static void writeUsage(const char* progname, FILE *f) {
    fprintf(f,"\n");
    fprintf(f,"%s - sample test program for libezsp\n\n", progname);
    fprintf(f,"Usage: %s [-d] [-u serialport] [-c channel] [-Z] [-C time] [-G|[-r *|-r source_id [-r source_id2...]] [-s source_id/key [-s source_id2/key...]]\n", progname);
    fprintf(f,"Available switches:\n");
    fprintf(f,"-h (--help)                               : this help\n");
    fprintf(f,"-d (--debug)                              : enable debug logs\n");
    fprintf(f,"-Z (--open-zigbee)                        : open the zigbee network at startup (for 60s)\n");
    fprintf(f,"-G (--open-gp-commissionning)             : open the Green Power commissionning session at startup\n");
    fprintf(f,"-C (--authorize-ch-request-answer) <time> : Authorize answer to channel request command for x seconds (0<x<255)\n");
    fprintf(f,"-u (--serial-port) <port>                 : use a specific serial port (default: '/dev/ttyUSB0')\n");
    fprintf(f,"-c (--reset-to-channel) <channel>         : force re-creation of a network on the specified channel (discards previously existing network)\n");
    fprintf(f,"-r (--remove-source-id) <source_id>       : remove a specific device from the monitored list, based on its source-id, use * to remove all (repeated -r options are allowed)\n");
    fprintf(f,"-s (--source-id) <source_id/key>          : adds a device to the monitored list, based on its source-id & key, id being formatted as a 8-digit hexadecimal string (eg: 'ffae1245'), and key as a 16-byte/32-digit hex string (repeated -s options are allowed)\n");
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

enum MainState {
    INIT_PENDING,
    REMOVE_ALL_GPD,
    REMOVE_SPECIFIC_GPD,
    ADD_GPD,
    COMMISSION_GPD,
    OPEN_ZIGBEE_NWK,
    RUN
};

class MainStateMachine {
public:
    /**
     * Constructor
     *
     * @param timerFactoryUtil An ITimerFactory used to generate ITimer objects
     * @param libEzspHandle The CLibEzspMain instance to use to communicate with the EZSP adapter
     * @param requestReset Do we reset the network and re-create a new one?
     * @param openGpCommissionning Do we open GP commissionning at dongle initialization?
     * @param authorizeChannelRequestAnswerTimeout During how many second (after startup), we will anwser to a channel request
     * @param openZigbeeCommissionning Do we open the Zigbee network at dongle initialization?
     * @param useNetworkChannel The 802.15.4 channel on which to send/receive traffic
     * @param gpRemoveAllDevices A flag to remove all GP devices from monitoring
     * @param gpDevicesToAdd A list of GP devices to add to the previous monitoring
     * @param gpDevicesToRemove A list of source IDs for GP devices to remove from previous monitoring
     */
    MainStateMachine(TimerBuilder& timerBuilder,
                     CLibEzspMain& libEzspHandle,
                     bool requestReset=false,
                     bool openGpCommissionning=false,
                     uint8_t authorizeChannelRequestAnswerTimeout=0,
                     bool openZigbeeCommissionning=false,
                     unsigned int useNetworkChannel=11,
                     bool gpRemoveAllDevices=false,
                     const std::vector<CGpDevice>& gpDevicesToAdd={},
                     const std::vector<uint32_t>& gpDevicesToRemove={}) :
        initFailures(0),
        timerBuilder(timerBuilder),
        libEzsp(libEzspHandle),
        resetAtStartup(requestReset),
        openGpCommissionningAtStartup(openGpCommissionning),
        channelRequestAnswerTimeoutAtStartup(authorizeChannelRequestAnswerTimeout),
        openZigbeeCommissionningAtStartup(openZigbeeCommissionning),
        channel(useNetworkChannel),
        removeAllGPDAtStartup(gpRemoveAllDevices),
        gpdAddList(gpDevicesToAdd),
        gpdRemoveList(gpDevicesToRemove),
        channelRequestAnswerTimer(this->timerBuilder.create()),
        currentState(MainState::INIT_PENDING) { }

    MainStateMachine(const MainStateMachine&) = delete; /* No copy construction allowed */

    MainStateMachine& operator=(MainStateMachine) = delete; /* No assignment allowed */

    /**
     * @brief Set internal state machine to run mode (waiting for asynchronous sensor reports)
     */
    void ezspRun() {
        clogI << "Preparation steps finished... switching to run state\n";
        this->currentState = MainState::RUN;
    }

    void ezspStateChangeCallback(CLibEzspState& i_state) {
        clogI << "EZSP library change to state " << static_cast<int>(i_state) << "\n";
        if (i_state == CLibEzspState::READY) {
            clogI << "EZSP library is ready, entering main state machine with MainState " << static_cast<int>(this->currentState) << "\n";
            if (this->currentState == MainState::INIT_PENDING && this->removeAllGPDAtStartup) {
                clogI << "Applying remove all GPD action\n";
                this->currentState = MainState::REMOVE_ALL_GPD;
                if (!libEzsp.clearAllGPDevices()) {
                    clogE << "Failed clearing GP device list\n";
                }
                this->removeAllGPDAtStartup = false;
                this->gpdRemoveList = std::vector<uint32_t>();
            }
            else if (this->currentState == MainState::INIT_PENDING && this->gpdRemoveList.size()) { /* If in REMOVE_ALL_GPD state, no need to remove specific GPs, we have already flushed all */
                clogI << "Removing " << this->gpdRemoveList.size() << " provided GPDs\n";
                this->currentState = MainState::REMOVE_SPECIFIC_GPD;
                if (!libEzsp.removeGPDevices(this->gpdRemoveList)) {
                    clogE << "Failed removing GPDs\n";
                }
                this->gpdRemoveList = std::vector<uint32_t>();
            }
            else if ((this->currentState == MainState::INIT_PENDING ||
                      this->currentState == MainState::REMOVE_ALL_GPD ||
                      this->currentState == MainState::REMOVE_SPECIFIC_GPD) &&
                     this->gpdAddList.size()) {    /* Once init is done or optional GPD remove has been done... */
                clogI << "Adding " << this->gpdAddList.size() << " provided GPDs\n";
                this->currentState = MainState::ADD_GPD;
                if (!libEzsp.addGPDevices(this->gpdAddList)) {
                    clogE << "Failed adding GPDs\n";
                }
                this->gpdAddList = std::vector<CGpDevice>();
            }
            else if ((this->currentState == MainState::INIT_PENDING ||
                      this->currentState == MainState::REMOVE_ALL_GPD ||
                      this->currentState == MainState::REMOVE_SPECIFIC_GPD ||
                      this->currentState == MainState::ADD_GPD) &&
                     this->openGpCommissionningAtStartup) {    /* Once init is done or optional GPD remove and addition has been done... */
                clogI << "Opening GP commisionning session\n";
                clogE << "Not implemented yet\n";
                exit(1);
                //libEzsp.getSink().openCommissioningSession();  /* FIXME: should not directly invoke methods on sink */
            }
            else if ((this->currentState == MainState::INIT_PENDING ||
                      this->currentState == MainState::REMOVE_ALL_GPD ||
                      this->currentState == MainState::REMOVE_SPECIFIC_GPD ||
                      this->currentState == MainState::ADD_GPD) &&
                     this->channelRequestAnswerTimeoutAtStartup) {    /* Once init is done or optional GPD remove and addition has been done... */
                clogI << "Opening GP channel request window for " << std::dec << static_cast<unsigned int>(this->channelRequestAnswerTimeoutAtStartup) << "s\n";
                libEzsp.setAnswerToGpfChannelRqstPolicy(true);
                // start timer
                this->channelRequestAnswerTimer->start(static_cast<uint16_t>(this->channelRequestAnswerTimeoutAtStartup*1000),
                             [this](ITimer *timer) {
                                 clogI << "Closing GP channel request window\n";
                                 this->libEzsp.setAnswerToGpfChannelRqstPolicy(false);
                             }
                            );
                this->ezspRun();
            }
            // else if (this->openZigbeeCommissionningAtStartup) {
            //     // If requested to do so, open the zigbee network for a specific duration, so new devices can join
            //     zb_nwk.openNetwork(60);

            //     // we retrieve network information and key and eui64 of dongle (can be done before)
            //     dongle.sendCommand(EZSP_GET_NETWORK_PARAMETERS);
            //     dongle.sendCommand(EZSP_GET_EUI64);
            //     std::vector<uint8_t> l_payload;
            //     l_payload.push_back(EMBER_CURRENT_NETWORK_KEY);
            //     dongle.sendCommand(EZSP_GET_KEY, l_payload);

            //     // start discover of existing product inside network
            //     zb_nwk.startDiscoverProduct([&](EmberNodeType i_type, EmberEUI64 i_eui64, EmberNodeId i_id){
            //         clogI << " Is it a new product ";
            //         clogI << "[type : "<< CEzspEnum::EmberNodeTypeToString(i_type) << "]";
            //         clogI << "[eui64 :";
            //         for(uint8_t loop=0; loop<i_eui64.size(); loop++){ clogI << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned(i_eui64[loop]); }
            //         clogI << "]";
            //         clogI << "[id : "<< std::hex << std::setw(4) << std::setfill('0') << unsigned(i_id) << "]";
            //         clogI << " ?" << std::endl;

            //         if( db.addProduct( i_eui64, i_id ) )
            //         {
            //             clogI << "YES !! Retrieve information for binding" << std::endl;

            //             // retrieve information about device, starting by discover list of active endpoint
            //             std::vector<uint8_t> payload;
            //             payload.push_back(u16_get_lo_u8(i_id));
            //             payload.push_back(u16_get_hi_u8(i_id));

            //             zb_messaging.SendZDOCommand( i_id, ZDP_ACTIVE_EP, payload );
            //         }
            //     });
            // }
            else {  /* No preparation step remains... we can swich to normal run state */
                this->ezspRun();
            }
            clogI << "Moving to MainState " << static_cast<int>(this->currentState) << "\n";
        }
	}

private:
    unsigned int initFailures;  /*!< How many failed init cycles we have done so far */
    TimerBuilder &timerBuilder;    /*!< A builder to create timer instances */
    CLibEzspMain& libEzsp;  /*!< The CLibEzspMain instance to use to communicate with the EZSP adapter */
    bool resetAtStartup;    /*!< Do we reset the network and re-create a new one? */
    bool openGpCommissionningAtStartup; /*!< Do we open GP commissionning at dongle initialization? */
    uint8_t channelRequestAnswerTimeoutAtStartup;   /*!< During how many second (after startup), we will anwser to a channel request */
    bool openZigbeeCommissionningAtStartup; /*!< Do we open the Zigbee network at dongle initialization? */
    unsigned int channel; /*!< The 802.15.4 channel on which to send/receive traffic */
    bool removeAllGPDAtStartup; /*!< A flag to remove all GP devices from monitoring */
    std::vector<CGpDevice> gpdAddList; /*!< A list of GP devices to add to the previous monitoring */
    std::vector<uint32_t> gpdRemoveList; /*!< A list of source IDs for GP devices to remove from previous monitoring */
    std::unique_ptr<ITimer> channelRequestAnswerTimer;   /*!< A timer to temporarily allow channel request */
    MainState currentState; /*!< Our current state (for the internal state machine) */
};

int main(int argc, char **argv) {
    IUartDriver *uartDriver = UartDriverBuilder::getUartDriver();
    TimerBuilder timerFactory;
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
                    if (gpRemovedDevDataList.size()) {
                        std::cerr << "-r * option cannot be used if another -r was used\n";
                        exit(1);
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

    Logger::getInstance().setLogLevel(debugEnabled ? LOG_LEVEL::DEBUG : LOG_LEVEL::INFO);

    clogI << "Starting ezsp test program (info)\n";

    if (uartDriver->open(serialPort, 57600) != 0) {
        clogE << "Failed opening serial port. Aborting\n";
        return 1;
    }

    CLibEzspMain lib_main(uartDriver, timerFactory);
    clogI << "Got timeout " << std::dec << static_cast<unsigned int>(authorizeChRqstAnswerTimeout) << "s\n";
    MainStateMachine fsm(timerFactory, lib_main, (resetToChannel!=0), openGpCommissionningAtStartup, authorizeChRqstAnswerTimeout, openZigbeeNetworkAtStartup, resetToChannel, removeAllGpDevs, gpAddedDevDataList, gpRemovedDevDataList);	/* If a channel was provided, reset the network and recreate it on the provided channel */
    auto clibobs = [&fsm, &lib_main](CLibEzspState& i_state) {
        fsm.ezspStateChangeCallback(i_state);
    };
    lib_main.registerLibraryStateCallback(clibobs);

    // lib incomming greenpower sourceId callback
	// auto cgpidobs = [](uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status) {
    //     clogI << "greenpower sourcedId: 0x" << std::hex << std::setw(4) << std::setfill('0') << unsigned(i_gpd_id)  <<
    //                 ", known: " << (i_gpd_known?"true":"false") << ", key status: " <<  std::hex << std::setw(2) << std::setfill('0') << unsigned(i_gpd_key_status) <<
    //                 std::endl;
    // };
    // lib_main.registerGPSourceIdCallback(cgpidobs);


#ifdef USE_CPPTHREADS
    std::string line;
    std::getline(std::cin, line);
#endif
#ifdef USE_RARITAN
    pp::Selector eventSelector(*pp::SelectorSingleton::getInstance())
    eventSelector.run();
#endif

    return 0;
}
