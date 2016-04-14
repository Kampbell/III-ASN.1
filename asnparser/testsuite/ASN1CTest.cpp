#include <iostream>
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "csn/ModuleTestEnum1.h"

#include "ASN1CTest.h"
using namespace std;
using namespace ASN1;
using namespace ModuleTestEnum1;

void enc(CoderEnv& env,  vector<char>& strm, const AbstractData& v1, const char* encodedStrm) {
	bool result = true;
	result = encode(v1, &env, back_inserter(strm)) && equal(strm.begin(), strm.end(), encodedStrm);
	if (!result) {
		vector<char>::const_iterator pc = mismatch(strm.begin(), strm.end(), encodedStrm).first;
		result = false;
		printf("FAIL !!!! \n");
		printf("The encoded sequence is wrong in the %dth byte : 0x%02x <-> 0x%02x\n"
			, pc - strm.begin(), (unsigned char)*pc, (unsigned char)encodedStrm[pc - strm.begin()]);

		printf("expected  dump is:");
		for (const char *pch = encodedStrm; *pch; ++pch)
			printf(" %02x", ((unsigned)*pch) & 0xff);
		printf("\n");

		printf("resulting dump is:");
		for (int i = 0; i < strm.size(); ++i)
			printf(" %02x", ((unsigned)strm[i]) & 0xff);
		printf("\n");
	}
	assert(result);
}
void dec(CoderEnv& env, vector<char>& strm, AbstractData& v2,  const char* encodedStrm) {
	bool result = true;
	result = decode(encodedStrm, encodedStrm+strm.size(), &env, v2);
	assert(result);
}
namespace ASN1 {
ASN1CTest::ASN1CTest(const string& name) :
	CppUnit::TestCase(name) {
}


ASN1CTest::~ASN1CTest() {

}
void ASN1CTest::test03_enum_OK() {
	bool result = true;
	vector<char> strm;
	const char* data;
	{	// Enum1
		strm.clear();
		Enum1 asn1, asn2;
		asn1.set_alpha();
		result = encode(asn1, &env, back_inserter(strm));
		assert(result);
		data = strm.data();
		result = decode(data, data + strm.size(), &env, asn2);
		assert(result);
		assert(asn1 == asn2);
		assert(asn2.is_alpha());
		assert(asn2.asInt() == 5);
	}
	{	// Enum1
		strm.clear();
		Enum1 asn1, asn2;
		asn1.setFromInt(12);
		result = encode(asn1, &env, back_inserter(strm));
		assert(result);
		data = strm.data();
		result = decode(data, data + strm.size(), &env, asn2);
		assert(result);
		assert(asn1 == asn2);
		assert(asn2.extendable() == true);
		assert(!asn2.is_alpha());
		assert(asn2.asInt() == 12);
	}
	{	// Enum2
		strm.clear();
		Enum2 asn1, asn2;
		asn1.set_beta();
		result = encode(asn1, &env, back_inserter(strm));
		assert(result);
		data = strm.data();
		result = decode(data, data + strm.size(), &env, asn2);
		assert(result);
		assert(asn1 == asn2);
		assert(asn2.extendable() == true);
		assert(asn2.is_beta());
		assert(asn2.asInt() == 12);
	}
	{	// Enum2
		strm.clear();
		Enum2 asn1, asn2;
		asn1.set_gamma();
		result = encode(asn1, &env, back_inserter(strm));
		assert(result);
		data = strm.data();
		result = decode(data, data + strm.size(), &env, asn2);
		assert(result);
		assert(asn1 == asn2);
		assert(asn2.extendable() == true);
		assert(asn2.is_gamma());
		assert(asn2.asInt() == 103);
	}
	{	// Enum3
		strm.clear();
		Enum3 asn1, asn2;
		asn1.setFromInt(12);
		result = encode(asn1, &env, back_inserter(strm));
		assert(result);
		data = strm.data();
		result = decode(data, data + strm.size(), &env, asn2);
		assert(result);
		assert(asn1 == asn2);
		assert(asn2.extendable() == true);
		assert(asn2.asInt() == 12);
	}
}
void ASN1CTest::setUp() {
	env.set_encodingRule(CoderEnv::ber);
}
void ASN1CTest::tearDown() {

}
CppUnit::Test* ASN1CTest::suite() {
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("ASN1CTest");

	CppUnit_addTest(pSuite, ASN1CTest, test03_enum_OK);

	return pSuite;
}
}
/*
*/