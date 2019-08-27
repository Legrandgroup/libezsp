#include "TestHarness.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

TEST_GROUP(gp_tests) {
};

TEST(gp_tests, dummy_test) {
	
	if (1==2)
		FAILF("Test failed\n");
	NOTIFYPASS();
}

#if 0
TEST(node_tests, dummy_optional_test) {
	const char* expected_result=NULL;

	if (result != NULL) != 0)
		FAILF("Test failed, got:\n\"%p\", expected:\n\"%s\"\n", result, "NULL");

	NOTIFYPASS();
}
#endif


#ifndef USE_CPPUTEST
void unit_tests_gp() {
	dummy_test();
}
#endif	// USE_CPPUTEST
