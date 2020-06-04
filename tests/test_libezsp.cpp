/*
 * @file test_libezsp.cpp
 *
 * Automated unit tests runner
 */

#include <cstdio>
#include <cstdlib>

#include <cassert>	// For assert()
#include "TestHarness.h"

#ifndef USE_CPPUTEST
void unit_tests_gp();	// Declaration of gp unit test procedure (see gp_tests.cpp)
void unit_tests_mock_serial();	// Declaration of mock serial self tests (see mock_serial_self_tests.cpp)
void unit_tests_logger_bytes_to_string();	// Declaration of logger bytes to string tests (see logger_bytes_to_string_tests.cpp)
void unit_tests_green_power_frame();	// Declaration of green power frame decoder tests (see green_power_frame_tests.cpp)
void unit_tests_ezsp_adapter_version();	// Declaration of EZSP adapter tests (see ezsp_adapter_version_tests.cpp)
#endif

int main(int argc, char* argv[]) {

#ifndef USE_CPPUTEST
	printf("*** Self test on mock serial ***\n");
	unit_tests_mock_serial();
	printf("*** Testing bytes container to string converter ***\n");
	unit_tests_logger_bytes_to_string();
	printf("*** Testing GP frames decoder and MIC check ***\n");
	unit_tests_green_power_frame();
	printf("*** Testing GP frames processing ***\n");
	unit_tests_gp();
	printf("*** Testing EZSP version processing ***\n");
	unit_tests_ezsp_adapter_version();
	printf("\n*** All unit tests passed successfully ***\n");
#else
	return CommandLineTestRunner::RunAllTests(argc, argv);
#endif	// USE_CPPUTEST

	return 0;
}
