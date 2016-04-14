#include <iostream>
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "csn/test.h"
#include "csn/MyHTTP.h"

#include "TESTTest.h"
using namespace std;
using namespace TEST;
using namespace ASN1;

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
TESTTest::TESTTest(const string& name) :
	CppUnit::TestCase(name) {
}


TESTTest::~TESTTest() {

}
void TESTTest::testPrimitiveType() {
	{
		vector<char> strm;
		ASN1::BOOLEAN b1(true), b2;
		clog << endl;

		clog << "value BOOLEAN ::= TRUE (BER)" << endl;
		char data[] = { 0x01, 0x01, 0xff };
		enc(env, strm, b1, data);
		dec(env, strm, b2, data);
		assert(b1 == b2);
	}
	{
		vector<char> strm;
		ASN1::BOOLEAN b1(true), b2;
		clog << "value BOOLEAN ::= FALSE (BER)" << endl;
		b1 = false;
		char data[] = { 0x01, 0x01, 0x00 };
		enc(env, strm, b1, data);
		dec(env, strm, b2, data);
		assert(b1 == b2);
	}
	{
		vector<char> strm;
		clog << "value NULL ::= NULL (BER)" << endl;
		Null n1, n2;
		char data[] = { 0x05, 0x00 };
		enc(env, strm, n1, data);
		dec(env, strm, n2, data);
		assert(n1 == n2);
	}
	{
		vector<char> strm;
		clog << "value INTEGER ::= 127 (BER)" << endl;
		ASN1::INTEGER i1, i2;
		i1 = 127;
		char data[] = { 0x02, 0x01, 0x7F };
		enc(env, strm, i1, data);
		dec(env, strm, i2, data);
		assert(i1 == i2);
	}
	{
		vector<char> strm;
		clog << "value INTEGER ::= 128 (BER)" << endl;
		ASN1::INTEGER i1, i2;
		i1 = 128;
		char data[] = { 0x02, 0x02, 0x00, 0x80 };
		enc(env, strm, i1, data);
		dec(env, strm, i2, data);
		assert(i1 == i2);
	}
	{
		vector<char> strm;
		clog << "value INTEGER ::= -27066 (BER)" << endl;
		ASN1::INTEGER i1, i2;
		i1 = -27066;
		char data[] = { 0x02, 0x02, 0x96, 0x46 };
		enc(env, strm, i1, data);
		dec(env, strm, i2, data);
		assert(i1 == i2);
	}
	{
		vector<char> strm;
		clog << "value BIT_STRING ::= '1011011101011'B (BER)" << endl;
		ASN1::BIT_STRING bs1, bs2;
		bs1.resize(13);
		bs1.set(0), bs1.set(2), bs1.set(3), bs1.set(5), bs1.set(6), bs1.set(7),
			bs1.set(9), bs1.set(11), bs1.set(12);

		char data[] = { 0x03, 0x03, 0x03, 0xb7, 0x58 };
		enc(env, strm, bs1, data);
		dec(env, strm, bs2, data);
		assert(bs1 == bs2);
	}
	{
		vector<char> strm;
		clog << "value S3 ::= SEQUENCE {\n  age INTEGER,\n  single BOOLEAN\n} ::= { age 24, single true}" << endl;
		ASN1::INTEGER i1;
		const unsigned tag = i1.getTag();
		ASN1::AbstractData::create(&ASN1::INTEGER::theInfo);
		S3 s3_1, s3_2;
		s3_1.set_age(24);
		s3_1.set_single(true);
		char data[] = { 0x30, 0x06, 0x02, 0x01, 0x18, 0x01, 0x01, 0xFF };
		enc(env, strm, s3_1, data);
		dec(env, strm, s3_2, data);
		assert(s3_1 == s3_2);
	}
	{
		vector<char> strm;
		clog << "value SEQUENCE OF INTEGER ::= {2,6,5} (BER)" << endl;
		S1 s1_1, s1_2;
		s1_1.push_back(new INTEGER(2));
		s1_1.push_back(new INTEGER(6));
		s1_1.push_back(new INTEGER(5));
		char data[] = { 0x30, 0x09, 0x02, 0x01, 0x02, 0x02, 0x01, 0x06, 0x02, 0x01, 0x05 };
		enc(env, strm, s1_1, data);
		dec(env, strm, s1_2, data);
		assert(s1_1 == s1_2);
	}
	{
		vector<char> strm;
		clog << "value Choice1 ::= CHOICE {\n    name VisibleString,\n    nobody NULL\n} ::= name : Perec" << endl;
		Choice1 c1, c2;
		c1.select_name("Perec");
		char data[] = { 0x1A, 0x05, 0x50, 0x65, 0x72, 0x65, 0x63 };
		enc(env, strm, c1, data);
		dec(env, strm, c2, data);
		assert(c1 == c2);
	}
	{
		vector<char> strm;
		clog << "value Choice1 ::= CHOICE {\n    name VisibleString,\n    nobody NULL\n} ::= name : Perec" << endl;
		Choice2 c1, c2;
		c1.select_name("Perec");
		//		char data[] = { 0x1A, 0x05, 0x50, 0x65, 0x72, 0x65, 0x63 };
		char data[] = { 0xA2, 0x07, 0x1A, 0x05, 0x50, 0x65, 0x72, 0x65, 0x63 };
		enc(env, strm, c1, data);
		dec(env, strm, c2, data);
		assert(c1 == c2);
	}
#ifdef FIXME
	{
		vector<char> strm;
		clog << "value Choice1 ::= CHOICE {\n    name VisibleString,\n    nobody NULL\n} ::= name : Perec" << endl;
		Choice3 c1, c2;
		c1.select_name("Perec");
		char data[] = { 0xA2, 0x07, 0x1A, 0x05, 0x50, 0x65, 0x72, 0x65, 0x63 };
		enc(env, strm, c1, data);
		dec(env, strm, c2, data);
		assert(c1 == c2);
	}
#endif
	{
		vector<char> strm;
		clog << "value Choice4 ::= f2 : f4 : 5" << endl;
		Choice4 c1, c2;
		c1.select_f2().select_f4(5);
		char data[] = { 0x02, 0x01, 0x05 };
		enc(env, strm, c1, data);
		dec(env, strm, c2, data);
		assert(c1 == c2);
	}
	{
		vector<char> strm;
		clog << "value S4 ::= { name A, attrib : f2 : f3 TRUE }" << endl;
		S4 s4_1, s4_2;
		s4_1.set_name("A");
		s4_1.set_attrib().select_f2().select_f3(true);
		char data[] = { 0x30, 0x06, 0x1A, 0x01, 0x41, 0x01, 0x01, 0xFF };
		enc(env, strm, s4_1, data);
		dec(env, strm, s4_2, data);
		assert(s4_1 == s4_2);
	}
	{
		vector<char> strm;
		clog << "value GetRequest ::= { header-only TRUE,\n\taccept-types { standards {html, plain-text}),\n\turl \"www.asn1.com\" }" << endl;
		MyHTTP::GetRequest gr1, gr2;
		gr1.set_header_only(true);
		gr1.set_accept_types().set_standards().resize(2);
		gr1.ref_accept_types().ref_standards().set(0);
		gr1.ref_accept_types().ref_standards().set(1);
		gr1.set_url("www.asn1.com");
		char data[] = { 0x30, 0x17, 0x01, 0x01, 0xFF, 0x31, 0x04, 0x03, 0x02, 0x06, 0xc0, 0x1a, 0x0c, 0x77,
			            0x77, 0x77, 0x2E, 0x61, 0x73, 0x6E, 0x31, 0x2E, 0x63, 0x6F, 0x6D };
		enc(env, strm, gr1, data);
		dec(env, strm, gr2, data);
		assert(gr1 == gr2);
	}
}
void TESTTest::testIterator() {
  int ints[] = { 2 , 3, 4 ,5};
  typedef ASN1::SET_OF<INTEGER> IntSet;
  IntSet intSet(ints, ints+sizeof(ints)/sizeof(int));

  IntSet::iterator i = intSet.begin();
  IntSet::iterator j = i+2;
  IntSet::iterator k = ++i;
  INTEGER z;
//FIXME   assert(*i == 2);
//FIXME   assert(*j == 4);
//FIXME   assert(*k == 3);
  //j = 2+i;

  IntSet::reverse_iterator ri = intSet.rbegin();
  IntSet::reverse_iterator rj = ++ri;
  //rj = 2+ri;

  int ints2[] = { 6,7,8,9 };
  intSet.insert(intSet.end(), ints2, ints2+sizeof(ints2)/sizeof(int));

  IntSet::const_iterator ci = intSet.begin();
  IntSet::const_iterator ce = intSet.end();
  assert(ce-ci == 8);

}
void TESTTest::setUp() {
	env.set_encodingRule(CoderEnv::ber);
}
void TESTTest::tearDown() {

}
CppUnit::Test* TESTTest::suite() {
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("TESTTest");

	CppUnit_addTest(pSuite, TESTTest, testPrimitiveType);
	CppUnit_addTest(pSuite, TESTTest, testIterator);

	return pSuite;
}
}
/*
*/