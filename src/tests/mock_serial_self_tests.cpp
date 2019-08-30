#include "TestHarness.h"
#include <iostream>
#include <iomanip>

#include "../spi/mock-uart/MockUartDriver.h"

/**
 * @brief Write callback class to test the mock serial interface
 */
class GenericUartIOTestProcessor : public IAsyncDataInputObserver {
public:
	GenericUartIOTestProcessor() : nbWriteCalls(0), nbReadCallbacks(0) { }

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
		if (cnt != 4)
			FAILF("Expected 4 bytes to write\n");

		if (delta != std::chrono::duration<double, std::milli>::max()) {
			std::cout << ". Delta since last write: " << (std::chrono::duration_cast<std::chrono::milliseconds>(delta)).count() << "ms";
			if (this->nbWriteCalls == 0) {
				std::cout << "\n";
				FAILF("First call to write() on mock serial interface should not lead to a valid delta\n");
			}
		}
		std::cout << "\n";
		writtenCnt = cnt;
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
		if (dataLen != 3)
			FAILF("Expected reading 3 bytes\n");

		std::cout << "\n";
		this->nbReadCallbacks++;
	}

	/**
	 * @brief Callback invoked on UART received bytes
	 */
	void handleInputData(const unsigned char* dataIn, const size_t dataLen) {
		this->onReadCallback(dataIn, dataLen);
	}
private:
	unsigned int nbWriteCalls;	/*!< How many time the onWriteCallback() was executed */
	unsigned int nbReadCallbacks;	/*!< How many time the onWriteCallback() was executed */
};

TEST_GROUP(mock_serial_tests) {
};

TEST(mock_serial_tests, mock_serial_open) {
	GenericUartIOTestProcessor serialProcessor;
	auto wcb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	MockUartDriver uartDriver(wcb);
	if (uartDriver.open("/dev/ttyUSB0", 57600) != 0) {
		FAILF("Failed opening mock serial port");
	}
	NOTIFYPASS();
}

TEST(mock_serial_tests, mock_serial_write) {
	GenericUartIOTestProcessor serialProcessor;
	auto wcb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	MockUartDriver uartDriver(wcb);
	if (uartDriver.open("/dev/ttyUSB0", 57600) != 0) {
		FAILF("Failed opening mock serial port");
	}
	size_t writtenResult;
	uint8_t wBuf[] = {0x00, 0x01, 0xff, 0xfe};
	if (uartDriver.write(writtenResult, wBuf, sizeof(wBuf)) != 0) {
		FAILF("Received a non-0 return code when calling write() on mock serial interface");
	}
	if (uartDriver.getWrittenBytesCount() != sizeof(wBuf)) {
		FAILF("Expected %lu bytes counted by write counter", sizeof(wBuf));
	}
	if (uartDriver.write(writtenResult, wBuf, sizeof(wBuf)) != 0) {
		FAILF("Received a non-0 return code when calling write() on mock serial interface");
	}
	if (uartDriver.getWrittenBytesCount() != sizeof(wBuf)*2) {
		FAILF("Expected %lu bytes counted by write counter", sizeof(wBuf)*2);
	}
	NOTIFYPASS();
}

TEST(mock_serial_tests, mock_serial_immediate_read_once) {
	GenericAsyncDataInputObservable uartIncomingDataHandler;
	GenericUartIOTestProcessor serialProcessor;
	auto wcb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	MockUartDriver uartDriver(wcb);
	if (uartDriver.open("/dev/ttyUSB0", 57600) != 0) {
		FAILF("Failed opening mock serial port");
	}

	MockUartScheduledByteDelivery rBuf1;
	rBuf1.delay = std::chrono::seconds(0);	/* Make bytes available now */
	rBuf1.byteBuffer.push_back(0x00);
	rBuf1.byteBuffer.push_back(0xa0);
	rBuf1.byteBuffer.push_back(0x50);
	auto rcb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	uartIncomingDataHandler.registerObserver(&serialProcessor);
	uartDriver.setIncomingDataHandler(&uartIncomingDataHandler);

	uartDriver.scheduleIncomingChunk(rBuf1);
	if (uartDriver.getScheduledIncomingChunksCount() != 1) {
		FAILF("Expected 1 scheduled incoming chunk");
	}
	if (uartDriver.getScheduledIncomingBytesCount() != rBuf1.byteBuffer.size()) {
		FAILF("Expected %lu scheduled incoming bytes", rBuf1.byteBuffer.size());
	}
	std::cerr << "Waiting for scheduled read bytes...\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(100));	/* 0.1s to make sure the secondary thread delivers the bytes in the background */
	if (uartDriver.getScheduledIncomingChunksCount() != 0) {
		FAILF("Expected 0 scheduled incoming chunks");
	}
	if (uartDriver.getScheduledIncomingBytesCount() != 0) {
		FAILF("Expected 0 scheduled incoming bytes");
	}
	if (uartDriver.getDeliveredIncomingBytesCount() != rBuf1.byteBuffer.size()) {
		FAILF("Expected %lu delivered incoming bytes", rBuf1.byteBuffer.size());
	}

	NOTIFYPASS();
}


TEST(mock_serial_tests, mock_serial_delayed_multiple_read) {
	GenericAsyncDataInputObservable uartIncomingDataHandler;
	GenericUartIOTestProcessor serialProcessor;
	auto wcb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	MockUartDriver uartDriver(wcb);
	if (uartDriver.open("/dev/ttyUSB0", 57600) != 0) {
		FAILF("Failed opening mock serial port");
	}

	MockUartScheduledByteDelivery rBuf1;
	rBuf1.delay = std::chrono::seconds(1);	/* Make bytes available in 1s */
	rBuf1.byteBuffer.push_back(0x00);
	rBuf1.byteBuffer.push_back(0xa0);
	rBuf1.byteBuffer.push_back(0x50);
	auto rcb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	uartIncomingDataHandler.registerObserver(&serialProcessor);
	uartDriver.setIncomingDataHandler(&uartIncomingDataHandler);

	uartDriver.scheduleIncomingChunk(rBuf1);
	std::cerr << "Current scheduled read queue: " << uartDriver.scheduledIncomingChunksToString() << "\n";
	if (uartDriver.getScheduledIncomingChunksCount() != 1) {
		FAILF("Expected 1 scheduled incoming chunk");
	}
	if (uartDriver.getScheduledIncomingBytesCount() != rBuf1.byteBuffer.size()) {
		FAILF("Expected %lu scheduled incoming bytes", rBuf1.byteBuffer.size());
	}
	std::cerr << "Waiting for scheduled read bytes...\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(1100));	/* 1s + 10% just to make sure the secondary thread delivers the bytes in the background */
	/* Sleep a bit more than 1s to all for scheduled incoming bytes to triggered a read */
	std::cerr << "Current scheduled read queue: " << uartDriver.scheduledIncomingChunksToString() << "\n";
	if (uartDriver.getScheduledIncomingChunksCount() != 0) {
		FAILF("Expected 0 scheduled incoming chunks");
	}
	if (uartDriver.getScheduledIncomingBytesCount() != 0) {
		FAILF("Expected 0 scheduled incoming bytes");
	}
	if (uartDriver.getDeliveredIncomingBytesCount() != rBuf1.byteBuffer.size()) {
		FAILF("Expected %lu delivered incoming bytes", rBuf1.byteBuffer.size());
	}

	NOTIFYPASS();
}

#ifndef USE_CPPUTEST
void unit_tests_mock_serial() {
	mock_serial_open();
	mock_serial_write();
	mock_serial_immediate_read_once();
	mock_serial_delayed_multiple_read();
}
#endif	// USE_CPPUTEST
