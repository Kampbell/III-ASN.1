#include "ASN1TestSuite.h"
#include "TESTTest.h"
#include "ASN1CTest.h"

namespace ASN1 {
CppUnit::Test* ASN1TestSuite::suite() {
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("ASN1TestSuite");

	pSuite->addTest(TESTTest::suite());
	pSuite->addTest(ASN1CTest::suite());

	return pSuite;
}
}
