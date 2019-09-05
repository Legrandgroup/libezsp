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
	GPRecvSensorMeasurementTest() : stage(0), nbWriteCalls(0), nbReadCallbacks(0) { }

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
		std::cout << "\n";
		writtenCnt = cnt;
		if (this->stage == 0 && compareBufWithVector(buf, cnt, std::vector<uint8_t>({0x1a, 0xc0, 0x38, 0xbc, 0x7e}))) {
			std::cout << "Got a ASH reset command\n";
			this->stage++;
		}
		else if (this->stage == 1 && compareBufWithVector(buf, cnt, std::vector<uint8_t>({0x00, 0x42, 0x21, 0xa8, 0x52, 0xcd, 0x6e, 0x7e}))) {
			std::cout << "Got a ASH get stack version command\n";
			this->stage++;
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
		std::cout << "Got notification of " << dataLen << " bytes read: ";
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
};

TEST_GROUP(gp_tests) {
};

TEST(gp_tests, gp_recv_sensor_measurement) {
	CppThreadsTimerFactory timerFactory;
	GenericAsyncDataInputObservable uartIncomingDataHandler;
	GPRecvSensorMeasurementTest serialProcessor;
	ConsoleLogger::getInstance().setLogLevel(LOG_LEVEL::DEBUG);	/* Only display logs for debug level info and higher (up to error) */
	auto wcb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	MockUartDriver uartDriver(wcb);
	if (uartDriver.open("/dev/ttyUSB0", 57600) != 0) {
		FAILF("Failed opening mock serial port");
	}

//	MockUartScheduledByteDelivery rBuf1;
//	rBuf1.delay = std::chrono::seconds(0);	/* Make bytes available now */
//	rBuf1.byteBuffer.push_back(0x00);
//	rBuf1.byteBuffer.push_back(0xa0);
//	rBuf1.byteBuffer.push_back(0x50);

	std::vector<uint32_t> sourceIdList;
	sourceIdList.push_back(0x0500001U);
	CAppDemo app(uartDriver, timerFactory, true, 11, sourceIdList);	/* Force reset the network channel to 11  */

	/* Note: we have to register our debug (dump serial read) observer after CAppDemo's internal one because observers list is currently implemented as a set and new observer are emplace()d */
	uartDriver.getIncomingDataHandler()->registerObserver(&serialProcessor);	/* Add our own observer to dump (for debug) the (emulated) bytes read from the serial port (that will be simultaneously sent to the read DUT) */

	std::this_thread::sleep_for(std::chrono::milliseconds(50));	/* Give 50ms for libezsp's internal process to write to serial */
	if (serialProcessor.stage != 1)
		FAILF("Failed to receive ASH reset from lib");
	else
		std::cout << "ASH reset confirmed\n";

	{
		MockUartScheduledByteDelivery rBuf;
		rBuf.delay = std::chrono::seconds(0);  /* Make bytes available now */
		rBuf.byteBuffer = std::vector<uint8_t>({0x1a, 0xc1, 0x02, 0x0b, 0x0a, 0x52, 0x7e});
		uartDriver.scheduleIncomingChunk(rBuf);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));	/* Give 100ms for libezsp's internal process to write to serial */

	if (serialProcessor.stage != 2)
		FAILF("Failed to receive ASH get stack version from lib");
	else
		std::cout << "ASH get stack version confirmed\n";

	{
		MockUartScheduledByteDelivery rBuf;
		rBuf.delay = std::chrono::seconds(0);  /* Make bytes available now */
		rBuf.byteBuffer = std::vector<uint8_t>({0x1a, 0xc1, 0x02, 0x0b, 0x0a, 0x52, 0x7e});
		uartDriver.scheduleIncomingChunk(rBuf);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));	/* Give 3000ms for final timeout (allows all written bytes to be generated) */

	uartDriver.destroyAllScheduledIncomingChunks(); /* Destroy all uartDriver currently running thread just in case */

	NOTIFYPASS();
}


#ifndef USE_CPPUTEST
void unit_tests_gp() {
	gp_recv_sensor_measurement();
}
#endif	// USE_CPPUTEST
