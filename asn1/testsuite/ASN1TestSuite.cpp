#include "ASN1TestSuite.h"
#include "ASN1Test.h"

namespace ASN1 {
CppUnit::Test* ASN1TestSuite::suite() {
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("ASN1TestSuite");

	pSuite->addTest(ASN1Test::suite());

	return pSuite;
}
}
