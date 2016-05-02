/*
 * ASN1test.h
 * 
 * Copyright (c) 2001 Institute for Information Industry, Taiwan, Republic of China 
 * (http://www.iii.org.tw/iiia/ewelcome.htm)
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 */

#ifndef ASN1Test_h_INCLUDED
#define ASN1Test_h_INCLUDED

#include "CppUnit/TestCase.h"
#include <asn1.h>

namespace ASN1 {
	class ASN1Test : public CppUnit::TestCase	{
	private:
	  CoderEnv env;

	public:
		ASN1Test(const std::string& name);
		~ASN1Test();

		void testBERCoder();
		void testPERCoder();
		void testValueNotationTests();
		void testIterator();
		void testABSTRACT_SYNTAX();
		void testEXTERNAL();
		void testTYPE_IDENTIFIER();

		void setUp();
		void tearDown();

		static CppUnit::Test* suite();
	};
}
#endif
