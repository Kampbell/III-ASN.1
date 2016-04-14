#ifndef TESTTest_INCLUDED
#define TESTTest_INCLUDED

#include "Poco/Foundation.h"
#include "CppUnit/TestCase.h"
#include "asn1.h"

namespace ASN1 {

	class TESTTest : public CppUnit::TestCase	{
	private:
	  CoderEnv env;

	public:
		TESTTest(const std::string& name);
		~TESTTest();

		void testPrimitiveType();
		void testIterator();

		void setUp();
		void tearDown();

		static CppUnit::Test* suite();
	};
}
#endif

