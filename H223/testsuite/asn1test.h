/*
 * asn1test.h
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
#ifndef ASN1TEST_H
#define ASN1TEST_H
#include <stdio.h>
#include "asn1.h"


#define TEST(str, env, v1, v2, encodedStrm, len) CoderTest(str, env, v1, v2, encodedStrm, len, __LINE__)

using namespace ASN1;
extern bool CoderTest(const char* str,
            CoderEnv& env,
                      const AbstractData& v1,
                      AbstractData& v2,
                      const char* encodedStrm,
                      int len,
                      int lineno);

#endif
