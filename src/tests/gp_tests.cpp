#include "TestHarness.h"
#include <iostream>

#include "../spi/mock-uart/MockUartDriver.h"

class GPRecvSensorMeasurementTest {
public:
	GPRecvSensorMeasurementTest() : step(0) { }
	int onWriteCallback(size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) {
		std::cout << "Request to write " << writtenCnt << " bytes: " << /*hexdump(buf, cnt) << */ "delta since last write: " << 0 /*delta*/ << "ms)\n";
		return 0;
	}
private:
	unsigned int step;
};

TEST_GROUP(gp_tests) {
};

TEST(gp_tests, dummy_test) {
	
	if (1==2)
		FAILF("Test failed\n");
	NOTIFYPASS();
}

TEST(gp_tests, gp_recv_sensor_measurement) {
	GPRecvSensorMeasurementTest testContext;
	auto cb = [&testContext](size_t& writtenCnt, const void* buf, size_t cnt, std::chrono::duration<double, std::milli> delta) -> int {
		return testContext.onWriteCallback(writtenCnt, buf, cnt, delta);
	};
	MockUartDriver uartDriver(cb);
    if (uartDriver.open("/dev/ttyUSB0", 57600) != 0) {
        std::cerr << "Failed opening serial port. Aborting\n";
    }

	NOTIFYPASS();
}


#ifndef USE_CPPUTEST
void unit_tests_gp() {
	dummy_test();
	gp_recv_sensor_measurement();
}
#endif	// USE_CPPUTEST
