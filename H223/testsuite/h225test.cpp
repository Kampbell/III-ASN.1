/*
 * h225test.cpp
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

#include "h323_messages.h"
#include "msc_openlogicalchannel.h"
#include "asn1test.h"

unsigned h225oid[] = {0,0,8,2250,0,4};
char ip[] = "\x8c\x5c\x3d\x60";
char productId [] = "Institute for Information Industry H.323 Gateway";
char versionId[] = "2.0";

#ifdef __sun
template <>
const typename ASN1::SEQUENCE_OF<ASN1::Constrained_OpenData<MULTIMEDIA_SYSTEM_CONTROL::OpenLogicalChannel>, ASN1::EmptyConstraint>::InfoType
  ASN1::SEQUENCE_OF<ASN1::Constrained_OpenData<MULTIMEDIA_SYSTEM_CONTROL::OpenLogicalChannel>, ASN1::EmptyConstraint>::theInfo =
{
  ASN1::SEQUENCE_OF_Base::create,
  0x10,
  0,
  ASN1::EmptyConstraint::constraint_type,
  ASN1::EmptyConstraint::lower_bound,
  ASN1::EmptyConstraint::upper_bound,
  &ASN1::Constrained_OpenData<MULTIMEDIA_SYSTEM_CONTROL::OpenLogicalChannel>::theInfo
};
#endif

#ifdef ASN1_HAS_IOSTREAM

void H225ValueNotationTests()
{
  CoderEnv env;
  env.set_encodingRule(CoderEnv::avn);

  using namespace H323_MESSAGES;
  EndpointType ep;
  VendorIdentifier& vendor = ep.set_vendor();
  H221NonStandard& nsd = vendor.set_vendor();
  nsd.set_t35CountryCode(38);
  nsd.set_t35Extension(0);
  nsd.set_manufacturerCode(21324);
  vendor.set_productId() = productId;
  vendor.set_versionId() = versionId;
  ep.set_gateway();
  ep.set_mc(false);
  ep.set_undefinedNode(false);

  const char eps[] = "{\n"
                    "  vendor {\n"
            "    vendor {\n"
            "      t35CountryCode 38,\n"
            "      t35Extension 0,\n"
            "      manufacturerCode 21324\n"
            "    },\n"
            "    productId '49 6e 73 74 69 74 75 74 65 20 66 6f 72 20 49 6e 66 6f 72 6d 61 74 69 6f 6e 20 49 6e 64 75 73 74 72 79 20 48 2e 33 32 33 20 47 61 74 65 77 61 79'H,\n"
            "    versionId '32 2e 30'H\n"
            "  },\n"
            "  gateway {\n"
            "  },\n"
            "  mc FALSE,\n"
            "  undefinedNode FALSE\n"
            "}";

  std::stringstream buf;
  ASN1::AVNEncoder encoder(buf);
  ep.accept(encoder);

  EndpointType ep2;
  TEST("EndpointType (AVN)" , env ,ep, ep2, eps, sizeof(eps));

  TransportAddress addr, addr2;
  TransportAddress_ipAddress& ipAddress = addr.select_ipAddress();
  ipAddress.set_ip() = "\x8c\x5c\x3d\x60";
  ipAddress.set_port(1051);


  const char addrs[] = "ipAddress : {\n"
             "  ip '8c 5c 3d 60'H,\n"
             "  port 1051\n"
             "}";
  TEST("TransportAddress (AVN)" , env ,addr ,addr2, addrs, sizeof(addrs));

  ASN1::SEQUENCE_OF<H323_MESSAGES::AliasAddress> aliases, aliases2;
  aliases.resize(3);
  aliases[0].select_dialedDigits("61096");
  aliases[1].select_h323_ID(L"angelo");
  aliases[2].select_email_ID("angelo@iii.org.tw");

  const char as[] = "{\n"
            "  dialedDigits : \"61096\",\n"
            "  h323-ID : \"angelo\",\n"
            "  email-ID : \"angelo@iii.org.tw\"\n"
            "}\n";

  TEST("SEQUENCE OF AliasAddress (AVN)" , env ,aliases, aliases2, as, sizeof(as));

}
#else
#define H225ValueNotationTests()
#endif

void H225PERTests()
{
  CoderEnv env;
  env.set_encodingRule(CoderEnv::per_Basic_Aligned);

  char confId [] = "\x07\x97\x8c\xc3\x74\xf0\x81\x44\xad\x4f\xb0\x13\x2e\xcf\x7f\x74";
  char callId [] = "\x0A\xE1\x82\x9A\xA8\x97\x93\x43\xA3\x6E\x60\x7A\xDA\x83\x6E\x2F";
  char fastStart0[] = "\x00\x00\x00\x0D\x00"
    "\x00\x40\x00\x0B\x0E\x00\x01\x00\x8C\x5C\x3D\x60\x27\x19\x00";
  char fastStart1[] = "\x00\x00\x00\x0C\x60\x13\x80\x0B\x0E\x00\x01\x00\x8C\x5C\x3D\x60"
    "\x27\x19\x00";
  char fastStart2[] = "\x40\x01\x00\x06\x04\x01\x00\x4D\x00\x00\x40\x00"
    "\x12\x1E\x00\x01\x00\x8C\x5C\x3D\x60\x27\x18\x00\x8C\x5C\x3D\x60"
    "\x27\x19\x00";
  char fastStart3[] = "\x40\x01\x00\x06\x04\x01\x00\x4C\x60\x13\x80\x12"
    "\x1E\x00\x01\x00\x8C\x5C\x3D\x60\x27\x18\x00\x8C\x5C\x3D\x60\x27"
    "\x19\x00";

  using namespace H323_MESSAGES;

  EndpointType ep, ep1;
  VendorIdentifier& vendor = ep.set_vendor();
  H221NonStandard& nsd = vendor.set_vendor();
  nsd.set_t35CountryCode(38);
  nsd.set_t35Extension(0);
  nsd.set_manufacturerCode(21324);
  vendor.set_productId() = productId;
  vendor.set_versionId() = versionId;
  ep.set_gateway();
  ep.set_mc(false);
  ep.set_undefinedNode(false);

  char exp[] =   "\x28\xC0\x26\x00\x53\x4C\x2F\x49"
  "\x6E\x73\x74\x69\x74\x75\x74\x65"
  "\x20\x66\x6F\x72\x20\x49\x6E\x66"
  "\x6F\x72\x6D\x61\x74\x69\x6F\x6E"
  "\x20\x49\x6E\x64\x75\x73\x74\x72"
  "\x79\x20\x48\x2E\x33\x32\x33\x20"
  "\x47\x61\x74\x65\x77\x61\x79\x02"
  "\x32\x2E\x30\x00";

  TEST("EndpointType (PER)", env ,ep, ep1, exp, sizeof(exp));

  TransportAddress addr;
  TransportAddress_ipAddress& ipAddress = addr.select_ipAddress();
  ipAddress.set_ip() = "\x8c\x5c\x3d\x60";
  ipAddress.set_port(1051);

  ASN1::SEQUENCE_OF<H323_MESSAGES::AliasAddress> aliases;
  aliases.resize(1);
  aliases[0].select_dialedDigits("61096");

  RasMessage rm1;
  RasMessage rasMessage;
  GatekeeperRequest& gkrq = rasMessage.select_gatekeeperRequest();
  gkrq.set_requestSeqNum(1);
  gkrq.set_protocolIdentifier().assign(h225oid, h225oid+6);
  gkrq.set_rasAddress(addr);
  gkrq.set_endpointType(ep);
  gkrq.set_endpointAlias(aliases);

  char exp2[] =
   "\x00\x20\x00\x00\x06\x00\x08\x91"
   "\x4A\x00\x04\x00\x8C\x5C\x3D\x60"
   "\x04\x1B\x28\xC0\x26\x00\x53\x4C"
   "\x2F\x49\x6E\x73\x74\x69\x74\x75"
   "\x74\x65\x20\x66\x6F\x72\x20\x49"
   "\x6E\x66\x6F\x72\x6D\x61\x74\x69"
   "\x6F\x6E\x20\x49\x6E\x64\x75\x73"
   "\x74\x72\x79\x20\x48\x2E\x33\x32"
   "\x33\x20\x47\x61\x74\x65\x77\x61"
   "\x79\x02\x32\x2E\x30\x00\x01\x02"
   "\x00\x94\x3C\x90";
  TEST("RasMessage (PER)", env ,rasMessage,rm1, exp2, sizeof(exp2));

  H323_UserInformation ui, ui2;
  Setup_UUIE& setup = ui.set_h323_uu_pdu().set_h323_message_body().select_setup();
#ifdef ASN1_HAS_IOSTREAM
  setup.set_protocolIdentifier().setFromValueNotation("{ itu-t (0) recommendation(0) h (8) 2250 version (0) 4 }");
#else
  setup.set_protocolIdentifier().assign(h225oid, h225oid+6);
#endif
  setup.set_sourceInfo(ep);
  aliases[0].ref_dialedDigits()= "62160";
  setup.set_destinationAddress(aliases);
  ipAddress.ref_ip() = "\x8c\x5c\x3e\xA0";
  ipAddress.set_port(1720);
  setup.set_destCallSignalAddress(addr);
  setup.set_conferenceID().assign(confId, confId+16);
  setup.set_conferenceGoal().select_create();
  setup.set_callType().select_pointToPoint();

  ipAddress.set_ip() = "\x8c\x5c\x3d\x60";
  ipAddress.set_port(1052);
  setup.set_sourceCallSignalAddress(addr);
  setup.set_callIdentifier().set_guid().assign(callId, callId+16);

#if !defined(ASN1_OS_FAMILY_WINDOWS) // FIXME
  ASN1::SEQUENCE_OF<ASN1::OCTET_STRING>& fastStart = setup.set_fastStart();
    // Push an OCTET STRING by value, NOT efficient
  fastStart.push_back( ASN1::OCTET_STRING(fastStart0, fastStart0 + sizeof(fastStart0)-1));
    // Push an newly constructed OCSTET STRING to the SEQUENCE OF,
    // and the container takes control the lifetime of the newly created object.
  fastStart.push_back( new ASN1::OCTET_STRING(fastStart1, fastStart1 + sizeof(fastStart1)-1));
  fastStart.push_back( new ASN1::OCTET_STRING(fastStart2, fastStart2 + sizeof(fastStart2)-1));
  fastStart.push_back( new ASN1::OCTET_STRING(fastStart3, fastStart3 + sizeof(fastStart3)-1));
  setup.set_fastStart(fastStart);
  
  //  setup.set_presentationIndicator().select_presentationAllowed();
  setup.set_screeningIndicator(ScreeningIndicator::userProvidedNotScreened);

  ASN1::SEQUENCE_OF<ASN1::OCTET_STRING>::iterator it = fastStart.begin();
  ASN1::SEQUENCE_OF<ASN1::OCTET_STRING> fs2(fastStart.begin(), fastStart.end());

  ui.ref_h323_uu_pdu().set_h245Tunneling(true);

  char exp3[] =
    "\x20\x98\x06\x00\x08\x91\x4A\x00"
    "\x04\x28\xC0\x26\x00\x53\x4C\x2F"
    "\x49\x6E\x73\x74\x69\x74\x75\x74"
    "\x65\x20\x66\x6F\x72\x20\x49\x6E"
    "\x66\x6F\x72\x6D\x61\x74\x69\x6F"
    "\x6E\x20\x49\x6E\x64\x75\x73\x74"
    "\x72\x79\x20\x48\x2E\x33\x32\x33"
    "\x20\x47\x61\x74\x65\x77\x61\x79"
    "\x02\x32\x2E\x30\x00\x01\x02\x00"
    "\x95\x49\x30\x8C\x5C\x3E\xA0\x06"
    "\xB8\x00\x07\x97\x8C\xC3\x74\xF0"
    "\x81\x44\xAD\x4F\xB0\x13\x2E\xCF"
    "\x7F\x74\x00\xCD\x1D\x88\x00\x07"
    "\x00\x8C\x5C\x3D\x60\x04\x1C\x11"
    "\x00\x0A\xE1\x82\x9A\xA8\x97\x93"
    "\x43\xA3\x6E\x60\x7A\xDA\x83\x6E"
    "\x2F\x69\x04\x14\x00\x00\x00\x0D"
    "\x00\x00\x40\x00\x0B\x0E\x00\x01"
    "\x00\x8C\x5C\x3D\x60\x27\x19\x00"
    "\x13\x00\x00\x00\x0C\x60\x13\x80"
    "\x0B\x0E\x00\x01\x00\x8C\x5C\x3D"
    "\x60\x27\x19\x00\x1F\x40\x01\x00"
    "\x06\x04\x01\x00\x4D\x00\x00\x40"
    "\x00\x12\x1E\x00\x01\x00\x8C\x5C"
    "\x3D\x60\x27\x18\x00\x8C\x5C\x3D"
    "\x60\x27\x19\x00\x1E\x40\x01\x00"
    "\x06\x04\x01\x00\x4C\x60\x13\x80"
    "\x12\x1E\x00\x01\x00\x8C\x5C\x3D"
    "\x60\x27\x18\x00\x8C\x5C\x3D\x60"
    "\x27\x19\x00\x01\x00\x01\x00\x01"
    "\x00\x01\x00\x01\x00\x10\x80\x01"
    "\x80";
  TEST("H323_UserInformation (PER)", env ,ui, ui2, exp3, sizeof(exp3));
#endif

  RasMessage rm3, rm4;
  RegistrationConfirm& rcf = rm3.select_registrationConfirm();
  rcf.set_requestSeqNum(1);
  rcf.set_protocolIdentifier().assign(h225oid, h225oid+6);
  rcf.set_callSignalAddress().resize(1);
  rcf.ref_callSignalAddress()[0].select_ipAddress().set_ip().assign(ip, ip+4);
  rcf.ref_callSignalAddress()[0].ref_ipAddress().set_port(1720);
  rcf.set_endpointIdentifier(L"Endpoint");
  rcf.set_timeToLive(5);
  rcf.set_genericData().resize(1);
  rcf.ref_genericData()[0].set_id().select_standard(1);

  char exp4[] =
    "\x12\x00\x00\x00\x06\x00\x08\x91"
    "\x4A\x00\x04\x01\x00\x8C\x5C\x3D"
    "\x60\x06\xB8\x0E\x00\x45\x00\x6E"
    "\x00\x64\x00\x70\x00\x6F\x00\x69"
    "\x00\x6E\x00\x74\x20\x8A\x01\x02"
    "\x00\x04\x01\x00\x01\x00\x04\x01"
    "\x00\x00\x01";
  TEST("RegistrationConfirm (PER)", env , rm3, rm4, exp4, sizeof(exp4));

  typedef ASN1::SEQUENCE_OF<ASN1::Constrained_OpenData<MULTIMEDIA_SYSTEM_CONTROL::OpenLogicalChannel> >  FastStart;
  FastStart fastStart_1;
  int size = fastStart_1.size();
  fastStart_1.resize(1);
  ASN1::Constrained_OpenData<MULTIMEDIA_SYSTEM_CONTROL::OpenLogicalChannel>& elem = fastStart_1[0];
}

void H225Test()
{
    H225ValueNotationTests();
    H225PERTests();
}
