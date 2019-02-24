#include "TestHarness.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

TEST_GROUP(node_tests) {
};

TEST(node_tests, dummy_test) {
	
	if (1==2)
		FAILF("Test failed\n");
	NOTIFYPASS();
}

#ifdef 0
TEST(node_tests, dummy_optional_test) {
	const char* expected_result=NULL;

	if (result != NULL) != 0)
		FAILF("Test failed, got:\n\"%p\", expected:\n\"%s\"\n", result, "NULL");

	NOTIFYPASS();
}
#endif


#ifndef USE_CPPUTEST
void unit_tests_node() {
#ifdef IPV6_SUPPORT
	dummy_optional_test();
#endif
	dummy_test();
}
#endif	// USE_CPPUTEST
