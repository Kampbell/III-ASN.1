/*
 * asn1test.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "test1.h"
#include "myhttp.h"
#ifdef ASN1_HAS_IOSTREAM
#include <iostream>
#endif

#include "asn1test.h"

extern "C" void SMC1Init();

#ifdef __sun
template <>
const typename ASN1::SEQUENCE_OF<ASN1::INTEGER, ASN1::EmptyConstraint>::InfoType
  ASN1::SEQUENCE_OF<ASN1::INTEGER, ASN1::EmptyConstraint>::theInfo =
{
  ASN1::SEQUENCE_OF_Base::create,
  0x10,
  0,
  ASN1::EmptyConstraint::constraint_type,
  ASN1::EmptyConstraint::lower_bound,
  ASN1::EmptyConstraint::upper_bound,
  &ASN1::INTEGER::theInfo
};
#endif


bool CoderTest(const char* str, CoderEnv& env, const AbstractData& v1, AbstractData& v2,
         const char* encodedStrm, int lineno)
{
  printf("%s\n", str);
  bool result = true;
  std::vector<char> strm;
  printf("encode ");
  if (encode(v1, &env, std::back_inserter(strm)) && std::equal(strm.begin(), strm.end(), encodedStrm))
    printf("success \n");
  else
  {
    std::vector<char>::const_iterator pc = std::mismatch(strm.begin(), strm.end(), encodedStrm).first;
    result = false;
    printf("FAIL !!!! \n");
    printf("The encoded sequence is wrong in the %dth byte : 0x%02x <-> 0x%02x\n"
      , pc-strm.begin(), (unsigned char)*pc, (unsigned char)encodedStrm[pc-strm.begin()]);

    printf("expected  dump is:");
    for(const char *pch = encodedStrm;*pch;++pch)
      printf(" %02x", ((unsigned)*pch)&0xff);
    printf("\n");

    printf("resulting dump is:");
    for(int i = 0;i < strm.size();++i)
      printf(" %02x", ((unsigned)strm[i])&0xff);
    printf("\n");
  }

  result = decode(encodedStrm, encodedStrm+strm.size(), &env, v2);
  if (result && v1 == v2)
    printf( "decode success \n");
  else if (!result)
  {
    printf("decode error\n");
  }
  else
  {
    result = false;
    printf( "decode result doesn't match the original value\n");
  }
  return result;
}


#ifdef ASN1_HAS_IOSTREAM
void ValueNotationTests()
{
  CoderEnv env;
  env.set_encodingRule(CoderEnv::avn);

  INTEGER a(5),b;
  TEST("INTEGER ValueNotation", env ,a, b , "5");

  a = 0 - a;
  TEST("negative INTEGER ValueNotation", env ,a, b , "-5");

  Null null1, null2;
  TEST("NULL ValueNotation", env ,null1, null2 ,"NULL");

  ASN1::BOOLEAN bool1(true), bool2;
  TEST("TRUE ValueNotation", env ,bool1, bool2 ,"TRUE");

  bool1 = false;
  TEST("FALSE ValueNotation", env ,bool1, bool2 ,"FALSE");

  BIT_STRING bs1, bs2;
  bs1.resize(10);
  bs1.set(5);
  bs1.set(7);
  bs1.set(9);
  TEST("BIT_STRING ValueNotation", env ,bs1, bs2 ,"\'0000010101\'B");

  char buf[] = "\x00\x10\x20\x30\x40\x50\x60\x70\x80\x90" ;
  OCTET_STRING os1(buf, buf+10), os2;
  TEST("OCTET_STRING ValueNotation", env ,os1, os2 ,"\'00 10 20 30 40 50 60 70 80 90\'H");

  IA5String is1 = "abcdef", is2;
  TEST("IA5String ValueNotation", env ,is1, is2 ,"\"abcdef\"");

  BMPString bmps1(L"abc"), bmps2;
  TEST("BMPString ValueNotation", env ,bmps1, bmps2 ,"\"abc\"");

  GeneralizedTime t1(2001, 3, 6, 3, 04, 05 ,0, 0, true), t2;
  TEST("GeneralizedTime ValueNotation", env ,t1, t2 ,"\"20010306030405Z\"");

  MyHTTP::RejectProblem rp1, rp2;
  rp1.set_invoke_mistypedArgument();
  TEST("RejectProblem ValueNotation", env, rp1, rp2, "invoke-mistypedArgument");
}
#endif

void testPERCoder()
{
  CoderEnv env;
  env.set_encodingRule(CoderEnv::per_Basic_Aligned);

  INTEGER i1(3), i2;
  TEST("vale INTEGER ::= 3 (PER)", env ,i1, i2,"\x01\x03");

  i1 = -3;
  TEST("vale INTEGER ::= -3 (PER)", env ,i1, i2,"\x01\xfd");

  i1 = 127;
  TEST("vale INTEGER ::= 127 (PER)", env ,i1, i2,"\x01\x7f");

  i1 = -128;
  TEST("vale INTEGER ::= -128 (PER)", env ,i1, i2,"\x01\x80");

  i1 = 128;
  TEST("vale INTEGER ::= 128 (PER)", env ,i1, i2,"\x02\x00\x80");

  i1 = 256;
  TEST("vale INTEGER ::= 256 (PER)", env ,i1, i2,"\x02\x01\x00");

  ASN1::Constrained_INTEGER<FixedConstraint, 3, 6> i3 = 3, i4;
  TEST("vale INTEGER(3..6) ::= 3 (PER)", env ,i3, i4,"\x00");

  ASN1::Constrained_INTEGER<FixedConstraint, 4000, 4255> i5 = 4002, i6;
  TEST("vale INTEGER(4000..4255) ::= 4002 (PER)", env ,i5, i6,"\x02");

  ASN1::Constrained_INTEGER<ExtendableConstraint, 3, 6> i7 = 5, i8;
  TEST("vale INTEGER(3..6,...) ::= 5 (PER)", env ,i7, i8,"\x40");

  i7 = 8;
  TEST("vale INTEGER(3..6,...) ::= 8 (PER)", env ,i7, i8,"\x80\x01\x08");

  ASN1::Constrained_INTEGER<ExtendableConstraint, 1, 65538> i9 = 1, i10;
  TEST("vale INTEGER(1..65538,...) ::= 1 (PER)", env ,i9, i10,"\x00\x00");

  i9 = 257;
  TEST("vale INTEGER(1..65538,...) ::= 257 (PER)", env ,i9, i10,"\x20\x01\x00");

  i9 = 65538;
  TEST("vale INTEGER(1..65538,...) ::= 65538 (PER)", env ,i9, i10,"\x40\x01\x00\x01");

  i9 = 65539;
  TEST("vale INTEGER(1..65538,...) ::= 65539 (PER)", env ,i9, i10,"\x80\x03\x01\x00\x03");

  ASN1::NumericString ns1("12345"), ns2;
  TEST("vale NumericString ::= \"12345\" (PER)", env ,ns1, ns2,"\x05\x23\x45\x60");

  using namespace MyHTTP;

  IA5Str01  ia501str1("012345"), ia501str2;
  TEST("vale IA5String (SIZE (6)) ::= \"012345\" (PER)",
    env ,ia501str1, ia501str2,"\x30\x31\x32\x33\x34\x35");

  IA5Str02  ia502str1("0123456"), ia502str2;
  TEST("vale IA5String (SIZE (5..20)) ::= \"0123456\" (PER)",
    env ,ia502str1, ia502str2,"\x20\x30\x31\x32\x33\x34\x35\x36");

  IA5Str03  ia503str1("abc"), ia503str2;
  TEST("vale IA5String (SIZE (0..8)) ::= \"abc\" (PER)",
    env ,ia503str1, ia503str2,"\x30\x61\x62\x63");

  IA5Str04  ia504str1("ABCDEFGH"), ia504str2;
  TEST("vale IA5String ::= \"ABCDEFGH\" (PER)",
    env ,ia504str1, ia504str2,"\x08\x41\x42\x43\x44\x45\x46\x47\x48");

  IA5Str05  ia505str1("abc"), ia505str2, ia505str3("abcdefgh");
  TEST("vale IA5String (SIZE (0..7, ..., 8)) ::= \"abc\" (PER)",
    env ,ia505str1, ia505str2,"\x30\x61\x62\x63");
/*
  TEST("vale IA5String (SIZE (0..7, ..., 8)) ::= \"abcdefgh\" (PER)",
    env ,ia505str3, ia505str2,"\x08\x61\x62\x63\x64\x65\x66\x67\x68");
*/
  IA5Str08 ia508str1("*2601#"), ia508str2;
  TEST("vale IA5String (FROM (\"0123456789#*,\")) ::= \"*2601#\" (PER)",
    env ,ia508str1, ia508str2,"\x06\x15\x93\x40");

  IA5Str09 ia509str1("*2601#"), ia509str2;
  TEST("vale IA5String (FROM (\"0123456789#*,ABCD\")) ::= \"*2601#\" (PER)",
    env ,ia509str1, ia509str2,"\x06\x2a\x32\x36\x30\x31\x23");

  IA5Str10 ia510str1("TAG"), ia510str2;
  TEST("vale IA5String (FROM (\"ACGT\")^SIZE(3)) ::= \"TAG\" (PER)",
    env ,ia510str1, ia510str2,"\xC8");

  IA5Str11 ia511str1("TFFT"), ia511str2;
  TEST("vale IA5String (FROM (\"TF\")) (SIZE(0..16)) ::= \"TFFT\" (PER)",
    env ,ia511str1, ia511str2,"\x24\x80");

}



void testBERCoder()
{
  CoderEnv env;
  env.set_encodingRule(CoderEnv::ber);

  ASN1::BOOLEAN b1(true), b2;

  TEST("value BOOLEAN ::= TRUE (BER)", env ,b1, b2, "\x01\x01\xff");

  b1 = false;
  TEST("value BOOLEAN ::= FALSE (BER)", env ,b1, b2, "\x01\x01\x00");

  Null n1, n2;
  TEST("value NULL ::= NULL (BER)", env ,n1, n2, "\x05\x00");

  INTEGER i1 = -27066, i2;
  TEST("value INTEGER ::= -27066 (BER)", env ,i1, i2,"\x02\x02\x96\x46");

  i1 = 127;
  TEST("value INTEGER ::= 127 (BER)", env ,i1, i2,"\x02\x01\x7F");

  i1 = -128;
  TEST("value INTEGER ::= -128 (BER)", env ,i1, i2,"\x02\x01\x80");

  i1 = 128;
  TEST("value INTEGER ::= 128 (BER)", env ,i1, i2,"\x02\x02\x00\x80");


  BIT_STRING bs1, bs2;
  bs1.resize(13);
  bs1.set(0), bs1.set(2), bs1.set(3), bs1.set(5), bs1.set(6), bs1.set(7),
  bs1.set(9), bs1.set(11), bs1.set(12);

  TEST("value BIT_STRING ::= '1011011101011'B (BER)", env ,bs1, bs2,"\x03\x03\x03\xb7\x58");

  char r6[] = "\x04\x03\x01\x02\x03";
  OCTET_STRING os1(r6+2, r6+5), os2;
  TEST("value OCTET_STRING ::= '01 02 03'H (BER)", env ,os1, os2, r6);

  using namespace TEST1;

  unsigned tag = i1.getTag();
  ASN1::AbstractData::create(&ASN1::INTEGER::theInfo);
  S3 s3_1, s3_2;
  s3_1.set_age(24);
  s3_1.set_single(true);
  TEST("value S3 ::= SEQUENCE {\n  age INTEGER,\n  single BOOLEAN\n} ::= { age 24, single true}",
        env ,s3_1, s3_2, "\x30\x06\x02\x01\x18\x01\x01\xFF");

  S1 s1_1, s1_2;
  s1_1.push_back(new INTEGER(2));
  s1_1.push_back(new INTEGER(6));
  s1_1.push_back(new INTEGER(5));
  TEST("value SEQUENCE OF INTEGER ::= {2,6,5} (BER)", env ,s1_1, s1_2, "\x30\x09\x02\x01\x02\x02\x01\x06\x02\x01\x05");


  Choice1 c1, c2;
  c1.select_name("Perec");
  TEST("value Choice1 ::= CHOICE {\n    name VisibleString,\n    nobody NULL\n} ::= name : Perec", env ,c1, c2, "\x1A\x05\x50\x65\x72\x65\x63");

  Choice2 c3, c4;
  c3.select_name("Perec");
  // was expecting incorrect "\x82\x07\x1A\x05\x50\x65\x72\x65\x63"
  TEST("value Choice2 ::= CHOICE [2] {\n    name VisibleString,\n    nobody NULL\n} ::= name : Perec", env ,c3, c4, "\xA2\x07\x1A\x05\x50\x65\x72\x65\x63");

  Choice3 c5, c6 ;
  c5.select_name("Perec");
  // was expecting incorrect "\xA2\x07\x80\x05\x50\x65\x72\x65\x63"
  TEST("value Choice3 ::= CHOICE [2] {\n    name [0] VisibleString,\n    nobody [1] NULL\n} ::= name : Perec", env ,c5, c6,
       "\xA2\x09\xA0\x07\x1A\x05\x50\x65\x72\x65\x63");

  Choice4 c7, c8;
  c7.select_f2().select_f4(5);
  TEST("value Choice4 ::= f2 : f4 : 5" ,
  env, c7, c8 , "\x02\x01\x05");

  S4 s4_1, s4_2;
  s4_1.set_name("A");
  s4_1.set_attrib().select_f2().select_f3(true);
  TEST("value S4 ::= { name A, attrib : f2 : f3 TRUE }",
  env, s4_1, s4_2, "\x30\x06\x1A\x01\x41\x01\x01\xFF");

  using namespace MyHTTP;


  GetRequest gr1, gr2;
  gr1.set_header_only(true);
  gr1.set_accept_types().set_standards().resize(2);
  gr1.ref_accept_types().ref_standards().set(0);
  gr1.ref_accept_types().ref_standards().set(1);
  gr1.set_url("www.asn1.com");

  // was expecting incorrect : "\x30\x17\x80\x01\xFF\xA2\x04\x80\x02\x06\xc0\x83\x0c\x77\x77\x77\x2E\x61\x73\x6E\x31\x2E\x63\x6F\x6D"
  TEST("value GetRequest ::= { header-only TRUE,\n\taccept-types { standards {html, plain-text}),\n\turl \"www.asn1.com\" }",
    env, gr1, gr2, "\x30\x17\x01\x01\xFF\x31\x04\x03\x02\x06\xc0\x1a\x0c"
    "\x77\x77\x77\x2E\x61\x73\x6E\x31\x2E\x63\x6F\x6D");
}

void IteratorTest()
{
  int ints[] = { 2 , 3, 4 ,5};
  typedef ASN1::SET_OF<INTEGER> IntSet;
  IntSet intSet(ints, ints+sizeof(ints));

  IntSet::iterator i = intSet.begin();
  IntSet::iterator j = i+2;
  IntSet::iterator k = ++i;
  //j = 2+i;

  IntSet::reverse_iterator ri = intSet.rbegin();
  IntSet::reverse_iterator rj = ++ri;
  //rj = 2+ri;

  int ints2[] = { 6,7,8,9 };
  intSet.insert(intSet.end(), ints2, ints2+4);

  IntSet::const_iterator ci = intSet.begin();
  IntSet::const_iterator ce = intSet.end();
  int a = ce-ci;

}


int main(int argc, char* argv[])
{

#ifdef _MPC850
  SMC1Init();
#endif
#ifdef ASN1_HAS_IOSTREAM
  ValueNotationTests();
#endif
  testBERCoder();
  testPERCoder();

  std::string a("abcdefg");
  bool b = (a == "abcdefg");

  return 0;
}
