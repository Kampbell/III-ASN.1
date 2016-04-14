#ifndef ASN1CTest_INCLUDED
#define ASN1CTest_INCLUDED

#include "Poco/Foundation.h"
#include "CppUnit/TestCase.h"
#include "asn1.h"

namespace ASN1 {

	class ASN1CTest : public CppUnit::TestCase	{
	private:
	  CoderEnv env;

	public:
		ASN1CTest(const std::string& name);
		~ASN1CTest();

		void test03_enum_OK();
		void test17_tags_OK();

		void setUp();
		void tearDown();

		static CppUnit::Test* suite();
	};
}
#endif

