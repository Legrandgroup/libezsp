#include "TestHarness.h"
#include <iostream>
#include <iomanip>

#include "../spi/mock-uart/MockUartDriver.h"

/**
 * @brief Write callback class to test the mock serial interface
 */
class GenericWriteTestProcessor {
public:
	GenericWriteTestProcessor() : nbWriteCalls(0) { }

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
private:
	unsigned int nbWriteCalls;	/*!< How many time the onWriteCallback() was executed */
};

TEST_GROUP(mock_serial_tests) {
};

TEST(mock_serial_tests, mock_serial_open) {
	GenericWriteTestProcessor serialProcessor;
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
	GenericWriteTestProcessor serialProcessor;
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

#ifndef USE_CPPUTEST
void unit_tests_mock_serial() {
	mock_serial_open();
	mock_serial_write();
}
#endif	// USE_CPPUTEST
