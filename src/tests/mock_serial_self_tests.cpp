#include "TestHarness.h"
#include <iostream>

#include "../spi/mock-uart/MockUartDriver.h"

class GenericSerialProcessor {
public:
	GenericSerialProcessor() : step(0) { }
	int onWriteCallback(size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) {
		std::cout << "Request to write " << writtenCnt << " bytes: " << /*hexdump(buf, cnt) << */ "delta since last write: " << 0 /*delta*/ << "ms)\n";
		return 0;
	}
private:
	unsigned int step;
};

TEST_GROUP(mock_serial_tests) {
};

TEST(mock_serial_tests, mock_serial_open) {
	GenericSerialProcessor serialProcessor;
	auto cb = [&serialProcessor](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return serialProcessor.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	MockUartDriver uartDriver(cb);
    if (uartDriver.open("/dev/ttyUSB0", 57600) != 0) {
        FAILF("Failed opening mock serial port");
    }
    NOTIFYPASS();
}


#ifndef USE_CPPUTEST
void unit_tests_mock_serial() {
	mock_serial_open();
}
#endif	// USE_CPPUTEST
