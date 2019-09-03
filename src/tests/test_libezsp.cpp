/*
 * @file test_libezsp.cpp
 *
 * Automated unit tests runner
 */

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>	// For assert()
#include "TestHarness.h"

#ifndef USE_CPPUTEST
void unit_tests_gp();	// Declaration of gp unit test procedure (see gp_tests.cpp)
void unit_tests_mock_serial();	// Declaration of mock serial self tests (see mock_serial_self_tests.cpp)
#endif

int main(int argc, char* argv[]) {

#ifndef USE_CPPUTEST
	printf("*** Self test on mock serial ***\n");
	unit_tests_mock_serial();
	printf("*** Testing GP frames processing ***\n");
	unit_tests_gp();
	printf("\n*** All unit tests passed successfully ***\n");
#else
	return CommandLineTestRunner::RunAllTests(argc, argv);
#endif	// USE_CPPUTEST
	
	return 0;
}
