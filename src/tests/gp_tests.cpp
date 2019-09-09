#include "TestHarness.h"
#include <iostream>
#include <iomanip>
#include <stdint.h>

#include "../spi/mock-uart/MockUartDriver.h"
#include "../spi/cppthreads/CppThreadsTimerFactory.h"
#include "../spi/IAsyncDataInputObserver.h"

#include "../spi/GenericLogger.h"
#include "../example/CAppDemo.h"

class GPRecvSensorMeasurementTest : public IAsyncDataInputObserver  {
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] stageTransitionExpectedList A pointer to an external list of expected buffer written to the serial line, that will automatically trigger a stage transition (stage++)
	 *            Because this is a pointer, you can update this vector on the fly during the test, we will use an always up-to-date vector each time bytes are written to the serial port.
	 *            However, this also means you have to keep the vector of vector of uint8_t memory allocated during the whole lifetime of this GPRecvSensorMeasurementTest object or you will have dereference crashes!
	 */
	GPRecvSensorMeasurementTest(const std::vector< std::vector<uint8_t> >* stageTransitionExpectedList = nullptr) : stage(0), nbWriteCalls(0), nbReadCallbacks(0), stageExpectedTransitions(stageTransitionExpectedList) { }

	/**
	 * @brief Write callback function to register to the mock serial interface
	 *
	 * It will be invoked each time a write() is done on the mock serial interface to which it has been registered
	 */
	int onWriteCallback(size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) {
		std::cout << "Request to write " << cnt << " bytes: ";
		for(unsigned int loop=0; loop<cnt; loop++) {
			if (loop!=0)
				std::cout << " ";
			std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned((static_cast<const uint8_t*>(buf))[loop]);
		}
		if (delta != std::chrono::duration<double, std::milli>::max()) {
			std::cout << ". Delta since last write: " << (std::chrono::duration_cast<std::chrono::milliseconds>(delta)).count() << "ms";
			if (this->nbWriteCalls == 0) {
				std::cout << "\n";
				FAILF("First call to write() on mock serial interface should not lead to a valid delta\n");
			}
		}
		std::cout << " (current stage " << std::dec << this->stage << ")\n";
		writtenCnt = cnt;
		bool transitionMatch = false;
		if (this->stageExpectedTransitions && this->stageExpectedTransitions->size() > this->stage) {	/* Do we have an expected read buffer to automatically transition to the next stage? */
			if (compareBufWithVector(buf, cnt, (*this->stageExpectedTransitions)[this->stage])) {
				this->stage++;
				transitionMatch = true;
				std::cout << "Automatic trigger matched, transitionning to stage " << this->stage << "\n";
			}
		}
		if (!transitionMatch) {
			std::cerr << "Warning: Got an unexpected command written to serial port while at stage " << std::dec << this->stage << ":";
			for (uint8_t loop=0; loop<cnt; loop++) {
				std::cerr << " " << std::hex << std::setw(2) << std::setfill('0') << unsigned((static_cast<const unsigned char*>(buf))[loop]);
			}
			std::cerr << "\n";
			FAILF("Unexpected command written to serial port");
		}
		this->nbWriteCalls++;
		return 0;
	}

	/**
	 * @brief Write callback function to register to the mock serial interface
	 *
	 * It will be invoked each time a read() is done on the mock serial interface to which it has been registered
	 */
	void onReadCallback(const unsigned char* dataIn, const size_t dataLen) {
		std::cout << "Got notification of " << std::dec << dataLen << " bytes read: ";
		for(unsigned int loop=0; loop<dataLen; loop++) {
			if (loop!=0)
				std::cout << " ";
			std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned((static_cast<const uint8_t*>(dataIn))[loop]);
		}
		std::cout << "\n";
		this->nbReadCallbacks++;
	}

	/**
	 * @brief Callback invoked on UART received bytes
	 */
	void handleInputData(const unsigned char* dataIn, const size_t dataLen) {
		this->onReadCallback(dataIn, dataLen);
	}
public:
	unsigned int stage;	/*!< Counter for the internal state machine */
	unsigned int nbWriteCalls;	/*!< How many time the onWriteCallback() was executed */
	unsigned int nbReadCallbacks;	/*!< How many time the onReadCallback() was executed */
	const std::vector< std::vector<uint8_t> >* stageExpectedTransitions;	/*!< A pointer to an external list of expected buffer written to the serial line, that will automatically trigger a stage transition (stage++) */
};

TEST_GROUP(gp_tests) {
};

#define UT_WAIT_MS(tms) std::this_thread::sleep_for(std::chrono::milliseconds(tms))
#define UT_FAILF_UNLESS_STAGE(tstage) do {\
	if (serialProcessor.stage != tstage) \
		FAILF("Failed to transition to stage %d", tstage); \
	else \
		std::cout << "ASH transitionned to stage " << tstage << "\n"; \
	} while(0)


TEST(gp_tests, gp_recv_sensor_measurement) {
	CppThreadsTimerFactory timerFactory;
	GenericAsyncDataInputObservable uartIncomingDataHandler;
	ConsoleLogger::getInstance().setLogLevel(LOG_LEVEL::DEBUG);	/* Only display logs for debug level info and higher (up to error) */
	std::vector< std::vector<uint8_t> > stageExpectedTransitions;
	GPRecvSensorMeasurementTest serialProcessor(&stageExpectedTransitions);
	auto wcb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	MockUartDriver uartDriver(wcb);
	if (uartDriver.open("/dev/ttyUSB0", 57600) != 0) {
		FAILF("Failed opening mock serial port");
	}

	stageExpectedTransitions.push_back(std::vector<uint8_t>({0x1a, 0xc0, 0x38, 0xbc, 0x7e}));
	std::vector<uint32_t> sourceIdList;
	sourceIdList.push_back(0x0500001U);
	CAppDemo app(uartDriver, timerFactory, true, 11, sourceIdList);	/* Force reset the network channel to 11  */

	UT_WAIT_MS(50);	/* Give 50ms for libezsp's internal process to write to serial */
	UT_FAILF_UNLESS_STAGE(1);

	stageExpectedTransitions.push_back(std::vector<uint8_t>({0x00, 0x42, 0x21, 0xa8, 0x52, 0xcd, 0x6e, 0x7e}));
	uartDriver.scheduleIncomingChunk(MockUartScheduledByteDelivery(std::vector<uint8_t>({0x1a, 0xc1, 0x02, 0x0b, 0x0a, 0x52, 0x7e})));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));	/* Give 100ms for libezsp's internal process to write to serial */

	if (serialProcessor.stage != 2)
		FAILF("Failed to receive ASH get stack version from lib");
	else
		std::cout << "ASH get stack version confirmed\n";

	stageExpectedTransitions.push_back(std::vector<uint8_t>({0x81, 0x60, 0x59, 0x7e}));
	stageExpectedTransitions.push_back(std::vector<uint8_t>({0x7d, 0x31, 0x43, 0x21, 0xa8, 0x53, 0x05, 0xf0, 0x7e}));
	//std::cerr << "Remaining incoming queue:" << uartDriver.scheduledIncomingChunksToString() << "\n";
	uartDriver.scheduleIncomingChunk(MockUartScheduledByteDelivery(std::vector<uint8_t>({0x01, 0x42, 0xa1, 0xa8, 0x53, 0x28, 0x45, 0xd7, 0xcf, 0x00, 0x7e})));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));	/* Give 100ms for libezsp's internal process to write to serial */

	if (serialProcessor.stage != 4)
		FAILF("Failed to transition to stage 4");
	else
		std::cout << "ASH transitionned to stage 4\n";

	stageExpectedTransitions.push_back(std::vector<uint8_t>({0x82, 0x50, 0x3a, 0x7e}));
	stageExpectedTransitions.push_back(std::vector<uint8_t>({0x22, 0x40, 0x21, 0x57, 0x54, 0x79, 0x17, 0x92, 0x59, 0xbf, 0xeb, 0x7e}));
	uartDriver.scheduleIncomingChunk(MockUartScheduledByteDelivery(std::vector<uint8_t>({0x12, 0x43, 0xa1, 0x57, 0x54, 0x2a, 0x45, 0xd7, 0xe7, 0x42, 0x7e})));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));	/* Give 100ms for libezsp's internal process to write to serial */

	if (serialProcessor.stage != 6)
		FAILF("Failed to transition to stage 6");
	else
		std::cout << "ASH transitionned to stage 6\n";

	stageExpectedTransitions.push_back(std::vector<uint8_t>({0x83, 0x40, 0x1b, 0x7e}));
	stageExpectedTransitions.push_back(std::vector<uint8_t>({0x33, 0x41, 0x21, 0x57, 0x54, 0x79, 0x16, 0xb8 , 0x59, 0x9e, 0xf8, 0x7e}));
	uartDriver.scheduleIncomingChunk(MockUartScheduledByteDelivery(std::vector<uint8_t>({0x23, 0x40, 0xa1, 0x57, 0x54, 0x79, 0x23, 0xad , 0x47, 0x7e})));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));	/* Give 100ms for libezsp's internal process to write to serial */

	if (serialProcessor.stage != 8)
		FAILF("Failed to transition to stage 8");
	else
		std::cout << "ASH transitionned to stage 8\n";

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));	/* Give 1s for final timeout (allows all written bytes to be sent by libezsp) */

	uartDriver.destroyAllScheduledIncomingChunks(); /* Destroy all uartDriver currently running thread just in case */

	NOTIFYPASS();
}


#ifndef USE_CPPUTEST
void unit_tests_gp() {
	gp_recv_sensor_measurement();
}
#endif	// USE_CPPUTEST
