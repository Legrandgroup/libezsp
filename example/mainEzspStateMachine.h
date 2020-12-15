/**
 * @file mainEzspStateMachine.h
 *
 * @brief State machine for mainEzspTest Sample code
 */

#include <iostream>
#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>	// For abs()

#include "spi/TimerBuilder.h"
#include "spi/ILogger.h"

#include <ezsp/ezsp.h>
#include <ezsp/byte-manip.h>

namespace NSMAIN {

/**
 * @brief Convert an ASCII character representing one hexadecimal digit ([0-9a-fA-F]) to its value (0-15)
 *
 * @param[in] hDigit The input printable ASCII character
 * @param[out] byte The value of the hexadecimal digit as a uint8_t nibble (0-15)
 * @return true if the conversion succeeded
**/
static bool hexDigitToNibble(const char hDigit, uint8_t& byte) {
	if (hDigit>='a' && hDigit<='f') {
		byte = static_cast<uint8_t>(hDigit + 10 - 'a');
	}
	else if (hDigit>='A' && hDigit<='F') {
		byte = static_cast<uint8_t>(hDigit + 10 - 'A');
	}
	else if (hDigit>='0' && hDigit<='9') {
		byte = static_cast<uint8_t>(hDigit - '0');
	}
	else {
		return false;
	}
	return true;
}

enum MainState {
	INIT_PENDING,
	REMOVE_ALL_GPD,
	REMOVE_SPECIFIC_GPD,
	ADD_GPD,
	COMMISSION_GPD,
	SCAN_CHANNELS,
	GET_NW_KEY,
	RUN,
	FW_UPGRADE,
};

class MainStateMachine {
public:
	/**
	 * Constructor
	 *
	 * @param timerBuilder An TimerBuilder used to generate ITimer objects
	 * @param libEzspHandle The CLibEzspMain instance to use to communicate with the EZSP adapter
	 * @param openGpCommissionning Do we open GP commissionning at EZSP adapter initialization?
	 * @param authorizeChannelRequestAnswerTimeout During how many second (after startup), we will anwser to a channel request
	 * @param openZigbeeCommissionning Do we open the Zigbee network at EZSP adapter initialization?
	 * @param gpRemoveAllDevices A flag to remove all GP devices from monitoring
	 * @param gpDevicesToAdd A list of GP devices to add to the previous monitoring
	 * @param gpDevicesToRemove A list of source IDs for GP devices to remove from previous monitoring
	 * @param displayNetworkKey Display the Zigbee network key before switching to run state
	 * @param switchToFirmwareUpgradeMode Do we immediately put the EZSP adapter into firmware upgrade mode
	 */
	MainStateMachine(NSSPI::TimerBuilder& timerBuilder,
	                 NSEZSP::CEzsp& libEzspHandle,
	                 bool openGpCommissionning=false,
	                 uint8_t authorizeChannelRequestAnswerTimeout=0,
	                 bool openZigbeeCommissionning=false,
	                 bool gpRemoveAllDevices=false,
	                 const std::vector<NSEZSP::CGpDevice>& gpDevicesToAdd= {},
	                 const std::vector<uint32_t>& gpDevicesToRemove= {},
	                 bool displayNetworkKey=false,
	                 bool switchToFirmwareUpgradeMode=false):
		initFailures(0),
		timerBuilder(timerBuilder),
		libEzsp(libEzspHandle),
		openGpCommissionningAtStartup(openGpCommissionning),
		channelRequestAnswerTimeoutAtStartup(authorizeChannelRequestAnswerTimeout),
		openZigbeeCommissionningAtStartup(openZigbeeCommissionning),
		removeAllGPDAtStartup(gpRemoveAllDevices),
		gpdAddList(gpDevicesToAdd),
		gpdRemoveList(gpDevicesToRemove),
		displayNetworkKey(displayNetworkKey),
		channelRequestAnswerTimer(this->timerBuilder.create()),
		currentState(MainState::INIT_PENDING),
		startFirmwareUpgrade(switchToFirmwareUpgradeMode) {
		/* If the EZSP adapter's application is corrupted, we will never boot the application
		In such cases, if we don't specifically handle that scenarion we would catch an ASH timeout but the default libezsp behaviour is to run the application, which would hang.
		Therefore, if we have been instructed to perform a firmware upgrade, and only in that case, we will ask libezsp to select the bootloader's firmware upgrade option directly as soon as we hit the ASH timeout
		*/
		if (this->startFirmwareUpgrade) {
			this->libEzsp.forceFirmwareUpgradeOnInitTimeout();
		}
	}

	MainStateMachine(const MainStateMachine&) = delete; /* No copy construction allowed */

	MainStateMachine& operator=(MainStateMachine) = delete; /* No assignment allowed */

	/**
	 * @brief Set internal state machine to run mode (waiting for asynchronous sensor reports)
	 */
	void ezspRun() {
		clogI << "Preparation steps finished... switching to run state\n";
		this->currentState = MainState::RUN;
	}

	/**
	 * @brief Perform the trailing action when the library init is finished
	 */
	void ezspInitDone() {
		clogI << "Adapter version: " << this->libEzsp.getAdapterVersion() << "\n";
		if (this->displayNetworkKey) {
			auto processNetworkKey = [this](NSEZSP::EEmberStatus status, const NSEZSP::EmberKeyData& key) {
				if (status == NSEZSP::EEmberStatus::EMBER_SUCCESS) {
					clogI << "Network key: " << NSSPI::Logger::byteSequenceToString(key) << "\n";
				}
				this->ezspRun();
			};
			this->currentState = MainState::GET_NW_KEY;
			this->libEzsp.getNetworkKey(processNetworkKey);
		}
		else {
			this->ezspRun();
		}
	}

	/**
	 * @brief Upgrade the firmware in the EZSP adapter
	 */
	void ezspFirmwareUpgrade() {
		clogI << "Switchover to bootloader for firmware upgrade\n";
		this->currentState = MainState::FW_UPGRADE;
		libEzsp.setFirmwareUpgradeMode();
	}

	/**
	 * @brief Remove all recorded GPD from the EZSP adapter
	 */
	void clearAllGPDevices() {
		clogI << "Applying remove all GPD action\n";
		this->currentState = MainState::REMOVE_ALL_GPD;
		if (!libEzsp.clearAllGPDevices()) {
			clogE << "Failed clearing GP device list\n";
		}
		this->removeAllGPDAtStartup = false;
		this->gpdRemoveList = std::vector<uint32_t>();
	}

	/**
	 * @brief Remove a selected list of GPD from the EZSP adapter
	 *
	 * @param[in] gpdToRemove The list of source IDs of the GPD to remove
	 */
	void removeGPDevices(const std::vector<uint32_t>& gpdToRemove) {
		clogI << "Removing " << gpdToRemove.size() << " provided GPDs\n";
		this->currentState = MainState::REMOVE_SPECIFIC_GPD;
		if (!libEzsp.removeGPDevices(gpdToRemove)) {
			clogE << "Failed removing GPDs\n";
		}
	}

	/**
	 * @brief Add a selected list of GPD from the EZSP adapter
	 *
	 * @param[in] gpdToAdd The list of source IDs of the GPD to add
	 */
	void addGPDevices(const std::vector<NSEZSP::CGpDevice>& gpdToAdd) {
		clogI << "Adding " << gpdToAdd.size() << " provided GPDs\n";
		this->currentState = MainState::ADD_GPD;
		if (!libEzsp.addGPDevices(gpdToAdd)) {
			clogE << "Failed adding GPDs\n";
		}
	}

	/**
	 * @brief Scan channels, displaying a result of channel occupation, then switch to run state (wait for sensor reports)
	 */
	void scanChannelsThenRun() {
		this->currentState = MainState::SCAN_CHANNELS;

		clogD << "Now scanning spectrum for clearest channel\n";
		auto processEnergyScanResults = [this](std::map<uint8_t, int8_t> channelToEnergyScan) {
			std::pair<uint8_t, int8_t> electedChannelRssi = {0xFF, 20};
			for (std::pair<uint8_t, int8_t> scannedChannel : channelToEnergyScan) {
				int8_t rssi = scannedChannel.second;
				if (rssi < electedChannelRssi.second) {
					electedChannelRssi = scannedChannel;
				}
			}
			clogI << "Selecting channel " << static_cast<unsigned int>(electedChannelRssi.first) << " with rssi: " << static_cast<int>(electedChannelRssi.second) << " dBm\n";
			//this->setChannel(electedChannelRssi.first);
			/* No other startup operations required... move to run state */
			this->ezspInitDone();
		};

		auto processActiveScanResults = [this](std::map<uint8_t, std::vector<NSEZSP::ZigbeeNetworkScanResult> > channelToZigbeeNetwork) {
			if (channelToZigbeeNetwork.size() == 0) {
				clogI << "Active scan returned no nearby network\n";
			}
			else {
				for (std::pair<uint8_t, std::vector<NSEZSP::ZigbeeNetworkScanResult> > scannedChannel : channelToZigbeeNetwork) {
					uint8_t channel = scannedChannel.first;
					const std::vector<NSEZSP::ZigbeeNetworkScanResult>& zigbeeNetworksOnChannel = scannedChannel.second;
					clogI << static_cast<unsigned int>(zigbeeNetworksOnChannel.size()) << " zigbee network(s) found on channel " << std::dec << static_cast<unsigned int>(channel) << ":\n";
					for (auto it = zigbeeNetworksOnChannel.begin(); it != zigbeeNetworksOnChannel.end(); ++it) {
						clogI << "Network:" << *it << "\n";
					}
				}
			}
			/* No other startup operations required... move to run state */
			this->ezspInitDone();
		};
#ifdef ACTIVE_SCAN
		libEzsp.startActiveScan(processActiveScanResults, 10, 1<<16);  /* This will make the underlying CEzspMain object move away from READY state until scan is finished */
#else
		libEzsp.startEnergyScan(processEnergyScanResults);  /* This will make the underlying CEzspMain object move away from READY state until scan is finished */
#endif
		/* Switching to run state will be performed once scanning is done, in the processEnergyScanResults() callback above */
	}

	/**
	 * @brief Callback invoked when the underlying EZSP library changes state
	 *
	 * @param i_state The new state of the EZSP library
	 */
	void ezspStateChangeCallback(NSEZSP::CLibEzspState i_state) {
		clogI << "EZSP library change to state " << NSEZSP::CLibEzspPublic::getStateAsString(i_state) << "\n";
		if (i_state == NSEZSP::CLibEzspState::READY) {
			clogI << "EZSP library is ready, entering main state machine with MainState " << static_cast<int>(this->currentState) << "\n";
			if (this->currentState == MainState::INIT_PENDING && this->startFirmwareUpgrade) {
				clogI << "Switching to firmware upgrade mode\n";
				this->ezspFirmwareUpgrade();
			}
			if (this->currentState == MainState::INIT_PENDING && this->removeAllGPDAtStartup) {
				this->clearAllGPDevices();
			}
			else if ((this->currentState == MainState::INIT_PENDING) && (this->gpdRemoveList.size() > 0)) { /* If in REMOVE_ALL_GPD state, no need to remove specific GPs, we have already flushed all */
				std::vector<uint32_t> copyGpdRemoveList;
				std::swap(copyGpdRemoveList, this->gpdRemoveList);
				this->removeGPDevices(copyGpdRemoveList);
			}
			else if ((this->currentState == MainState::INIT_PENDING ||
			          this->currentState == MainState::REMOVE_ALL_GPD ||
			          this->currentState == MainState::REMOVE_SPECIFIC_GPD) &&
			         (this->gpdAddList.size() > 0)) {    /* Once init is done or optional GPD remove has been done... */
				std::vector<NSEZSP::CGpDevice> copyGpdAddList;
				std::swap(copyGpdAddList, this->gpdAddList);
				this->addGPDevices(copyGpdAddList);
			}
			else if ((this->currentState == MainState::INIT_PENDING ||
			          this->currentState == MainState::REMOVE_ALL_GPD ||
			          this->currentState == MainState::REMOVE_SPECIFIC_GPD ||
			          this->currentState == MainState::ADD_GPD) &&
			         (this->channelRequestAnswerTimeoutAtStartup != 0 ||
			          this->openGpCommissionningAtStartup)) {    /* Once init is done or optional GPD remove and addition has been done... */
				if (this->openGpCommissionningAtStartup) {
					clogI << "Opening GP commisionning session\n";
					libEzsp.openCommissioningSession();
				}
				if (this->channelRequestAnswerTimeoutAtStartup != 0) {
					clogI << "Opening GP channel request window for " << std::dec << static_cast<unsigned int>(this->channelRequestAnswerTimeoutAtStartup) << "s\n";
					libEzsp.setAnswerToGpfChannelRqstPolicy(true);
					// start timer
					this->channelRequestAnswerTimer->start(static_cast<uint32_t>(this->channelRequestAnswerTimeoutAtStartup*1000),
					[this](NSSPI::ITimer *timer) {
						clogI << "Closing GP channel request window\n";
						this->libEzsp.setAnswerToGpfChannelRqstPolicy(false);
					}
					                                      );
				}

				this->scanChannelsThenRun();
			}
			// else if (this->openZigbeeCommissionningAtStartup) {
			//     // If requested to do so, open the zigbee network for a specific duration, so new devices can join
			//     zb_nwk.openNetwork(60);

			//     // we retrieve network information and key and eui64 of dongle (can be done before)
			//     dongle.sendCommand(EZSP_GET_NETWORK_PARAMETERS);
			//     dongle.sendCommand(EZSP_GET_EUI64);
			//     dongle.sendCommand(EZSP_GET_KEY, { EMBER_CURRENT_NETWORK_KEY });

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
			//             NSSPI::ByteBuffer payload;
			//             payload.push_back(u16_get_lo_u8(i_id));
			//             payload.push_back(u16_get_hi_u8(i_id));

			//             zb_messaging.SendZDOCommand( i_id, ZDP_ACTIVE_EP, payload );
			//         }
			//     });
			// }
			else {
				clogW << "Ignoring EZSP library state change\n";
			}
			clogI << "Moving to MainState " << static_cast<int>(this->currentState) << "\n";
		}
	}

	/**
	 * @brief Extract one cluster report from the beginning payload buffer
	 *
	 * @note The buffer may contain trailing data, that won't be parsed. @p usedBytes can be used to find out what remains after the cluster report
	 *
	 * @param[in] payload The payload (byte buffer) out of which we will parse the leading bytes
	 * @param[out] usedBytes The number of bytes successfully dissected at the beginning of the buffer
	 *
	 * @return true If a cluster report could be parsed, in such case @p usedBytes will contain the number of bytes decoded to generate the cluster report
	 */
	static bool extractClusterReport( const NSSPI::ByteBuffer& payload, uint8_t& usedBytes ) {
		size_t payloadSize = payload.size();

		if (payloadSize < 5) {
			clogE << "Attribute reporting frame is too short: " << payloadSize << " bytes\n";
			usedBytes = 0;
			return false;
		}

		/* Cluster IDs are defined in the ZCL specification (Zigbee Alliance document 07-5123-06) */
		uint16_t clusterId = NSEZSP::dble_u8_to_u16(payload.at(1), payload.at(0));
		/* Attribute IDs are also defined in the ZCL specs */
		uint16_t attributeId = NSEZSP::dble_u8_to_u16(payload.at(3), payload.at(2));
		uint8_t type = payload.at(4);

		switch (clusterId) {
		case 0x0000: /* Basic */
			if ((attributeId == 0x4000 /* SWBuildID */) && (type == NSEZSP::ZCL_CHAR_STRING_ATTRIBUTE_TYPE)) {
				if (payloadSize < 6) {
					clogE << "Firmware version string is too short: " << payloadSize << " bytes\n";
				}
				else {
					uint8_t strLength = payload.at(5);
					if (payloadSize < 6+strLength) {
						clogE << "String frame is too short: " << payloadSize << " bytes\n";
						usedBytes = 0;
						return false;
					}
					std::string fwVersion(payload.begin()+6, payload.begin()+6+strLength);
					std::cout << "Firmware version is \"" << fwVersion << "\"\n";
					usedBytes = 6 + strLength;
					return true;
				}
			}
			else {
				clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
			}
			break;
		case 0x000F: /* Binary input */
			if ((attributeId == 0x0055 /* PresentValue */) && (type == NSEZSP::ZCL_BOOLEAN_ATTRIBUTE_TYPE)) {
				if (payloadSize < 6) {
					clogE << "Binary input frame is too short: " << payloadSize << " bytes\n";
				}
				else {
					uint8_t value = payload.at(5);
					std::cout << "Door is " << (value?"closed":"open") << "\n";
					usedBytes = 6;
					return true;
				}
			}
			else {
				clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
			}
			break;
		case 0x0402: /* Temperature Measurement */
			if ((attributeId == 0x0000 /* MeasuredValue */) && (type == NSEZSP::ZCL_INT16S_ATTRIBUTE_TYPE)) {
				if (payloadSize < 7) {
					clogE << "Temperature frame is too short: " << payloadSize << " bytes\n";
					usedBytes = 0;
					return false;
				}
				else {
					int16_t value = static_cast<int16_t>(NSEZSP::dble_u8_to_u16(payload.at(6), payload.at(5)));
					std::cout << "Temperature: " << value/100 << "." << std::setw(2) << std::setfill('0') << abs(value%100) << "°C\n"; /* Note: we padd with 0 and only use the absolute value for the decimals in order to properly display the extracted decimal part */
					usedBytes = 7;
					return true;
				}
			}
			else {
				clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
			}
			break;
		case 0x0405: /* Humidity */
			if ((attributeId == 0x0000 /* MeasuredValue */) && (type == NSEZSP::ZCL_INT16U_ATTRIBUTE_TYPE)) {
				if (payloadSize < 7) {
					clogE << "Humidity frame is too short: " << payloadSize << " bytes\n";
					usedBytes = 0;
					return false;
				}
				else {
					int16_t value = static_cast<int16_t>(NSEZSP::dble_u8_to_u16(payload.at(6), payload.at(5)));
					std::cout << "Humidity: " << value/100 << "." << std::setw(2) << std::setfill('0') << value%100 << "%\n";
					usedBytes = 7;
					return true;
				}
			}
			else {
				clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
			}
			break;
		case 0x0001: /* Power Configuration */
			if ((attributeId == 0x0020 /* BatteryVoltage */) && (type == NSEZSP::ZCL_INT8U_ATTRIBUTE_TYPE)) {
				if (payloadSize < 6) {
					clogE << "Battery level frame is too short: " << payloadSize << " bytes\n";
				}
				else {
					uint8_t value = static_cast<uint8_t>(payload.at(5));
					std::cout << "Battery level: " << value/10 << "." << std::setw(1) << std::setfill('0') << value%10 << "V\n";
					usedBytes = 6;
					return true;
				}
			}
			else {
				clogE << "Wrong type: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<unsigned int>(type) << "\n";
			}
			break;
		default:
			clogE << "Unknown cluster ID: 0x" << std::hex << std::setw(4) << std::setfill('0') << clusterId << "\n";
		}
		clogE << "Payload parsing error\n";
		usedBytes = 0;
		return false;
	}

	/**
	 * @brief Extract multiple concatenated cluster reports from the beginning payload buffer
	 *
	 * @param[in] payload The payload (byte buffer) we will parse
	 *
	 * @return true If the buffer only contains 0, 1 or more concatenated cluster report(s) that were succesfully parsed, false otherwise (also returned when there are undecoded trailing bytes)
	 */
	static bool extractMultiClusterReport( NSSPI::ByteBuffer payload ) {
		uint8_t usedBytes = 0;
		bool validBuffer = true;

		while (payload.size()>0 && validBuffer) {
			validBuffer = extractClusterReport(payload, usedBytes);
			if (validBuffer) {
				payload.erase(payload.begin(), payload.begin()+static_cast<int>(usedBytes));
			}
		}
		return validBuffer;
	}

	/**
	 * @brief Handler to be invoked when a new green power frame is received
	 *
	 * It will take the appropriate actions
	 *
	 * @warning Currently, this method only handles green power attribute single or multi report frames
	 *
	 * @param[in] i_gpf The frame received
	 */
	void onReceivedGPFrame(NSEZSP::CGpFrame &i_gpf) {
		switch(i_gpf.getCommandId()) {
		case 0xa0: {	// Attribute reporting
			uint8_t usedBytes;
			if (!extractClusterReport(i_gpf.getPayload(), usedBytes)) {
				clogE << "Failed decoding attribute reporting payload: ";
				for (auto i : i_gpf.getPayload()) {
					clogE << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i) << " ";
				}
			}
		}
		break;

		case 0xa2: {	// Multi-Cluster Reporting
			if (!extractMultiClusterReport(i_gpf.getPayload())) {
				clogE << "Failed to fully decode multi-cluster reporting payload: ";
				for (auto i : i_gpf.getPayload()) {
					clogE << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i) << " ";
				}
			}
		}
		break;

		case 0x20: { // Off Command (see Table 54 "Payloadless GPDF commands sent by GPD" in Green Power Specifications v1.1.1 (14-0563-18))
			std::cout << "Received Off Command\n";
		}
		break;

		case 0x21: { // On Command (see Table 54 "Payloadless GPDF commands sent by GPD" in Green Power Specifications v1.1.1 (14-0563-18))
			std::cout << "Received On Command\n";
		}
		break;

		default:
			clogW << "Unknown command ID: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(i_gpf.getCommandId()) << "\n";
			break;
		}
	}

private:
	unsigned int initFailures;  /*!< How many failed init cycles we have done so far */
	NSSPI::TimerBuilder &timerBuilder;    /*!< A builder to create timer instances */
	NSEZSP::CEzsp& libEzsp;  /*!< The CEzsp instance to use to communicate with the EZSP adapter */
	bool openGpCommissionningAtStartup; /*!< Do we open GP commissionning at EZSP adapter initialization? */
	uint8_t channelRequestAnswerTimeoutAtStartup;   /*!< During how many second (after startup), we will anwser to a channel request */
	bool openZigbeeCommissionningAtStartup; /*!< Do we open the Zigbee network at EZSP adapter initialization? */
	bool removeAllGPDAtStartup; /*!< A flag to remove all GP devices from monitoring */
	std::vector<NSEZSP::CGpDevice> gpdAddList; /*!< A list of GP devices to add to the previous monitoring */
	std::vector<uint32_t> gpdRemoveList; /*!< A list of source IDs for GP devices to remove from previous monitoring */
	bool displayNetworkKey; /*!< Do we display the Zigbee network key before swithcing to run state? */
	std::unique_ptr<NSSPI::ITimer> channelRequestAnswerTimer;   /*!< A timer to temporarily allow channel request */
	MainState currentState; /*!< Our current state (for the internal state machine) */
	bool startFirmwareUpgrade; /*!< Do we immediately put the EZSP adapter into firmware upgrade mode at startup */
};

} // namespace NSMAIN
