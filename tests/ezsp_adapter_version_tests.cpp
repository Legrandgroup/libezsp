#include <iostream>
#include <iomanip>
#include <sstream>

#include "ezsp/ezsp-adapter-version.h"
#include "TestHarness.h"

using NSEZSP::EzspAdapterVersion;

TEST_GROUP(ezsp_adapter_version_tests) {
};

TEST(ezsp_adapter_version_tests, equality_stack_version_only) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6440);
	vB.setEzspVersionInfo(0x6440);
	if (vA != vB)
		FAILF("Failed getting equality for identical versions");
	if (vA == vB) {
	}
	else
		FAILF("Failed getting equality for identical versions");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, difference_stack_version_only) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6440);
	vB.setEzspVersionInfo(0x6540);
	if (vA == vB)
		FAILF("Failed getting difference for different versions");
	if (vA != vB) {
	}
	else
		FAILF("Failed getting difference for different versions");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, equality_ezsp_version_only) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6440);
	vB.setEzspVersionInfo(0x6440);
	if (vA != vB)
		FAILF("Failed getting equality for identical versions");
	if (vA == vB) {
	}
	else
		FAILF("Failed getting equality for identical versions");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, difference_ezsp_version_only) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6440, 7, 2);
	vB.setEzspVersionInfo(0x6750, 8, 2);
	if (vA == vB)
		FAILF("Failed getting difference for different versions");
	if (vA != vB) {
	}
	else
		FAILF("Failed getting difference for different versions");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, difference_stack_version_vs_ezsp) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6440);
	vB.setEzspVersionInfo(0x6440, 7, 2);
	if (vA == vB)
		FAILF("Failed getting difference for different versions");
	if (vA != vB) {
	}
	else
		FAILF("Failed getting difference for different versions");
	if (vB > vA) {
	}
	else
		FAILF("Failed getting expected vB>vA");
	if (vB <= vA)
		FAILF("Failed because got unexpected vB<=vA");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, difference_ezsp_protocol_only) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6750, 7, 2);
	vB.setEzspVersionInfo(0x6750, 8, 2);
	
	if (vA == vB)
		FAILF("Failed getting difference for different versions");
	if (vA != vB) {
	}
	else
		FAILF("Failed getting difference for different versions");
	if (vB > vA) {
	}
	else
		FAILF("Failed getting expected vB>vA");
	if (vB <= vA)
		FAILF("Failed because got unexpected vB<=vA");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, ezsp_version_only_rewrite) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6750, 7, 2);
	vB.setEzspVersionInfo(0x6750, 8, 2);
	if (vA == vB)
		FAILF("Failed getting difference for different versions");
	
	vA.setEzspVersionInfo(0x6750, 8, 2);
	if (vA != vB)
		FAILF("Failed getting equality for identical versions");
	
	vB.setEzspVersionInfo(0x6440);
	if (vA == vB)
		FAILF("Failed getting difference for different versions");
	if (vA != vB) {
	}
	else
		FAILF("Failed getting difference for different versions");
	
	vA.setEzspVersionInfo(0x6440);
	if (vA != vB) {
		FAILF("Failed getting equality for identical versions");
	}
	if (vA == vB) {
	}
	else
		FAILF("Failed getting equality for identical versions");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, equality_with_xncp_version_unknown_manufacturer) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6750, 8, 2);
	vB.setEzspVersionInfo(0x6750, 8, 2);
	if (vA != vB)
		FAILF("Failed getting equality for identical versions");
	
	vA.setXncpData(0x0001, 0x1234);
	vB.setXncpData(0x0001, 0x4567);
	if (vA != vB)
		FAILF("Failed getting equality for identical versions (different unknown manufacturers)");
	
	vA.setXncpData(0x0001, 0x1234);
	vB.setXncpData(0x0002, 0x4567);
	if (vA != vB)
		FAILF("Failed getting equality for identical versions (different unknown manufacturers)");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, difference_with_xncp_version_unknown_manufacturer) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6320, 7, 2);
	vB.setEzspVersionInfo(0x6440, 7, 2);
	if (vA == vB)
		FAILF("Failed getting difference for different versions");
	
	vA.setXncpData(0x0001, 0x1234);
	vB.setXncpData(0x0001, 0x4567);
	if (vA == vB)
		FAILF("Failed getting difference for different versions (different unknown manufacturers)");
	if (vA != vB) {
	}
	else
		FAILF("Failed getting difference for different versions (different unknown manufacturers)");
	
	vA.setXncpData(0x0001, 0x1234);
	vB.setXncpData(0x0002, 0x4567);
	if (vA == vB)
		FAILF("Failed getting difference for different versions (different unknown manufacturers)");
	if (vA != vB) {
	}
	else
		FAILF("Failed getting difference for different versions (different unknown manufacturers)");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, comparison_xncp_version_manufacturer_legrand_twice) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6320, 7, 2);
	vB.setEzspVersionInfo(0x6440, 7, 2);
	if (vA == vB) 	/* Version are different */
		FAILF("Failed getting difference for different versions");
	
	/* But when we set the XNCP info with the same manufacturer data 0x1234, they become equal */
	vA.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2345);
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2345);
	
	if (vA != vB)
		FAILF("Failed getting equality for identical versions (Legrand manufacturer on both)");
	
	/* If only the hardware version changes, we should still have equality */
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x1345);
	if (vA != vB)
		FAILF("Failed getting equality for identical versions (Legrand manufacturer on both)");
	
	/* However, if firmware version changes, there should be a difference */
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2123);
	if (vA == vB)
		FAILF("Failed getting difference on XNCP firmware version (Legrand manufacturer on both)");
	
	vA.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2345);
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2123);
	if (vA>vB) {
	}
	else
		FAILF("Got unexpected comparison result on XNCP info (firmware versions)");
	
	if (vA<=vB)
		FAILF("Got unexpected comparison result on XNCP info (firmware versions)");
	if (vA>vB) {
	}
	else
		FAILF("Got unexpected comparison result on XNCP info (firmware versions)");
	
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2345);
	if (vA != vB)
		FAILF("Failed getting equality for identical versions (Legrand manufacturer on both)");
	if (vA<=vB) { /* Equality will make us match also on <= */
	}
	else
		FAILF("Failed getting equality for identical versions (Legrand manufacturer on both)");
	if (vA>=vB) { /* Equality will make us match also on >= */
	}
	else
		FAILF("Failed getting equality for identical versions (Legrand manufacturer on both)");
	
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2344); /* We compare to vA 0x2345 */
	if (vA<=vB)
		FAILF("Got unexpected comparison result on XNCP info revision (firmware versions)");
	if (vA>vB) {
	}
	else
		FAILF("Got unexpected comparison result on XNCP info revision (firmware versions)");
	
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2346); /* We compare to vA 0x2345 */
	if (vA>=vB)
		FAILF("Got unexpected comparison result on XNCP info revision (firmware versions)");
	if (vA<vB) {
	}
	else
		FAILF("Got unexpected comparison result on XNCP info revision (firmware versions)");
	
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2335); /* We compare to vA 0x2345 */
	if (vA<=vB)
		FAILF("Got unexpected comparison result on XNCP info minor (firmware versions)");
	if (vA>vB) {
	}
	else
		FAILF("Got unexpected comparison result on XNCP info minor (firmware versions)");
	
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2353); /* We compare to vA 0x2345 */
	if (vA>=vB)
		FAILF("Got unexpected comparison result on XNCP info minor (firmware versions)");
	if (vA<vB) {
	}
	else
		FAILF("Got unexpected comparison result on XNCP info minor (firmware versions)");
	
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2256); /* We compare to vA 0x2345 */
	if (vA<=vB)
		FAILF("Got unexpected comparison result on XNCP info major (firmware versions)");
	if (vA>vB) {
	}
	else
		FAILF("Got unexpected comparison result on XNCP info major (firmware versions)");
	
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x2434); /* We compare to vA 0x2345 */
	if (vA>=vB)
		FAILF("Got unexpected comparison result on XNCP info major (firmware versions)");
	if (vA<vB) {
	}
	else
		FAILF("Got unexpected comparison result on XNCP info major (firmware versions)");

	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, comparison_xncp_version_manufacturer_legrand_twice_different_protocol_or_stack) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6320, 7, 2);
	vB.setEzspVersionInfo(0x6440, 7, 2);
	vA.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x1234);
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x1234);
	/* Even if stack is different, we only care about Legrand XNCP version, so vA and vB are equivalent */
	
	if (vA != vB) {
		FAILF("Failed getting equality for identical versions (Legrand manufacturer on both)");
	}
	
	/* However, if protocol version are not the same, even on two Legrand devices, we should not show equality */
	vB.setEzspVersionInfo(0x6750, 8, 2);
	if (vA == vB) {
		FAILF("Failed getting difference on EZSP protocols (Legrand manufacturer on both)");
	}
	
	/* Same thing with stack version: even on two Legrand devices, we should not show equality */
	vA.setEzspVersionInfo(0x6750, 8, 3);
	if (vA == vB) {
		FAILF("Failed getting difference on stack versions (Legrand manufacturer on both)");
	}
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, comparison_xncp_version_manufacturer_legrand_and_unknown) {
	EzspAdapterVersion vA;
	EzspAdapterVersion vB;
	vA.setEzspVersionInfo(0x6440, 7, 2);
	vB.setEzspVersionInfo(0x6320, 7, 2);
	vA.setXncpData(0x0001, 0x1234);
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x1345);
	/* Stack from vA is more recent than from vB */
	/* Even if manufacturer Legrand on vB and XNCP info is higher on vB, we should consider vA>vB */
	if (vA <= vB) {
		FAILF("Failed comparing a Legrand adapter with a non-Legrand by excluding XNCP");
	}
	
	if (vA > vB) {
	}
	else
		FAILF("Failed comparing a Legrand adapter with a non-Legrand by excluding XNCP");
	
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x1234);
	if (vA == vB) {
		FAILF("Failed comparing a Legrand adapter with a non-Legrand by excluding XNCP");
	}
	
	vA.setEzspVersionInfo(0x6320, 7, 2);
	vB.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x1345);
	
	if (vA != vB) {
		FAILF("Failed comparing a Legrand adapter with a non-Legrand by excluding XNCP");
	}
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, to_ostream_no_xncp) {
	EzspAdapterVersion v;
	v.setEzspVersionInfo(0x6440, 7, 2);
	
	std::stringstream s;
	
	s << v;
	std::string result = s.str();
	std::cout << "Got <<()'s output: \"" << result << "\"\n";
	
	if (result.find("EZSPv7 running stack type 2 (mesh)") == std::string::npos) {
		FAILF("Failed matching EZSP protocol and stack type from operator<<()'s output on EzspAdapterVersion");
	}
	if (result.find("stack v6.4.4.0") == std::string::npos) {
		FAILF("Failed matching stack version from operator<<()'s output on EzspAdapterVersion");
	}
	
	s = std::stringstream();	/* Flush s */
	s << std::hex << v;
	if (result != s.str())
		FAILF("Discrepancy when switching ostream to hex");
	
	s = std::stringstream();	/* Flush s */
	s << std::dec << v;
	if (result != s.str())
		FAILF("Discrepancy when switching ostream to dec");
	
	s = std::stringstream();	/* Flush s */
	s << std::setfill('A') << v;
	if (result != s.str())
		FAILF("Discrepancy when switching ostream to setfill('A')");
	
	s = std::stringstream();	/* Flush s */
	s << std::setw(16) << v;
	if (result != s.str())
		FAILF("Discrepancy when switching ostream to setw(16)");
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, to_ostream_xncp_manufacturer_not_legrand) {
	EzspAdapterVersion v;
	v.setEzspVersionInfo(0x6440, 7, 2);
	v.setXncpData(0x0001, 0x1234);
	
	std::stringstream s;
	
	s << v;
	std::string result = s.str();
	std::cout << "Got <<()'s output: \"" << result << "\"\n";
	
	if (result.find("Manufacturer: 0x0001") == std::string::npos) {
		FAILF("Failed matching manufacturer from operator<<()'s output on EzspAdapterVersion");
	}
	if (result.find("EZSPv7 running stack type 2 (mesh)") == std::string::npos) {
		FAILF("Failed matching EZSP protocol and stack type from operator<<()'s output on EzspAdapterVersion");
	}
	if (result.find("stack v6.4.4.0") == std::string::npos) {
		FAILF("Failed matching stack version from operator<<()'s output on EzspAdapterVersion");
	}
	
	NOTIFYPASS();
}

TEST(ezsp_adapter_version_tests, to_ostream_xncp_manufacturer_legrand) {
	EzspAdapterVersion v;
	v.setEzspVersionInfo(0x6440, 7, 2);
	v.setXncpData(NSEZSP::EzspAdapterVersion::Manufacturer::LEGRAND, 0x1234);
	
	std::stringstream s;
	
	s << v;
	std::string result = s.str();
	std::cout << "Got <<()'s output: \"" << result << "\"\n";
	
	if (result.find("LEGRAND") == std::string::npos) {
		FAILF("Failed matching manufacturer from operator<<()'s output on EzspAdapterVersion");
	}
	if (result.find("EZSPv7 running stack type 2 (mesh)") == std::string::npos) {
		FAILF("Failed matching EZSP protocol and stack type from operator<<()'s output on EzspAdapterVersion");
	}
	if (result.find("stack v6.4.4.0") == std::string::npos) {
		FAILF("Failed matching stack version from operator<<()'s output on EzspAdapterVersion");
	}
	
	s = std::stringstream();	/* Flush s */
	s << std::hex << v;
	if (result != s.str())
		FAILF("Discrepancy when switching ostream to hex");
	
	s = std::stringstream();	/* Flush s */
	s << std::dec << v;
	if (result != s.str())
		FAILF("Discrepancy when switching ostream to dec");
	
	s = std::stringstream();	/* Flush s */
	s << std::setfill('A') << v;
	if (result != s.str())
		FAILF("Discrepancy when switching ostream to setfill('A')");
	
	s = std::stringstream();	/* Flush s */
	s << std::setw(16) << v;
	if (result != s.str())
		FAILF("Discrepancy when switching ostream to setw(16)");
	
	NOTIFYPASS();
}

#ifndef USE_CPPUTEST
void unit_tests_ezsp_adapter_version() {
	equality_stack_version_only();
	difference_stack_version_only();
	equality_ezsp_version_only();
	difference_ezsp_version_only();
	difference_stack_version_vs_ezsp();
	difference_ezsp_protocol_only();
	ezsp_version_only_rewrite();
	equality_with_xncp_version_unknown_manufacturer();
	difference_with_xncp_version_unknown_manufacturer();
	comparison_xncp_version_manufacturer_legrand_twice();
	comparison_xncp_version_manufacturer_legrand_twice_different_protocol_or_stack();
	comparison_xncp_version_manufacturer_legrand_and_unknown();
	to_ostream_no_xncp();
	to_ostream_xncp_manufacturer_not_legrand();
	to_ostream_xncp_manufacturer_legrand();
}
#endif	// USE_CPPUTEST
