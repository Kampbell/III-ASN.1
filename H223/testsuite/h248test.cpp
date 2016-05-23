#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <media_gateway_control.h>
#include "asn1test.h"

namespace H248 = MEDIA_GATEWAY_CONTROL;

using namespace std;
const char ip[] = "\xA4\x30\x33\xEE";
const char packetAndProp[] = "\x00\x00\x10\x01";
#ifdef incorrect_orig_message
const char d[]=
  "\x30\x55\xA1\x53\x80\x01\x01\xA1"
  "\x0C\xA0\x0A\x80\x04\xA4\x30\x33"
  "\xEE\x81\x02\x13\x9C\xA2\x40\xA1"
  "\x3E\xA0\x3C\x80\x01\x01"
  "\xA1\x37\x30\x35\x80\x04\x19\x99"
  "\x26\x9E\xA3\x2D\x30\x2B\xA0\x29"
  "\xA0\x27\xA0\x0A\x30\x08\xA0\x03"
  "\x04\x01\x00\x81\x01\x00\xA1\x19"
  "\xA0\x17\xA1\x15\xA0\x13"
  "\xA1\x11\xA0\x0F\x30\x0D\x30\x0B"
  "\x80\x04\x00\x00\x10\x01\xA1\x03"
  "\x04\x01\x01";
#else
const char d[]=
  "\x30\x4d\x30\x4b\x02\x01\x01\x30"
  "\x0a\x04\x04\xa4\x30\x33\xee\x02"
  "\x02\x13\x9c\x30\x3a\x30\x38\x02"
  "\x01\x01\x30\x33\x30\x31\x02\x04"
  "\x19\x99\x26\x9e\x30\x29\x30\x27"
  "\x30\x25\x30\x0a\x30\x08\x30\x03"
  "\x04\x01\x00\x04\x01\x00\x30\x17"
  "\x30\x15\x30\x13\x30\x11\x30\x0f"
  "\x30\x0d\x30\x0b\x04\x04\x00\x00"
  "\x10\x01\x30\x03\x04\x01\x01";
#endif

const char d1[] =
  "\x30\x2c\x30\x2a\x02\x01\x01\x30"
  "\x0a\x04\x04\xa4\x30\x33\xee\x02"
  "\x02\x13\x9c\x30\x19\x02\x01\x01"
  "\x16\x14\x73\x69\x6d\x70\x6c\x65"
  "\x20\x65\x72\x72\x6f\x72\x20\x6d"
  "\x65\x73\x73\x61\x67\x65";

void H248Test()
{

  ////////////////////////////////////////////////
  // Set termination capabilities (Package, Property and parameter values)

  cout << "setting termination capabilities" << endl;

  H248::MegacoMessage myReq, myReq2;
  myReq.set_mess().set_version(1);
  H248::Message& myMsg = myReq.ref_mess();

  ASN1::OCTET_STRING& address = myMsg.set_mId().select_ip4Address().set_address();
  address.assign(&ip[0], &ip[4]);
  myMsg.ref_mId().ref_ip4Address().set_portNumber(5020);
  myMsg.set_messageBody().select_transactions().resize(1);

  H248::TransactionRequest& transReq = myMsg.ref_messageBody().ref_transactions()[0].select_transactionRequest();
  transReq.set_transactionId(1);
  transReq.set_actions().resize(1);

  H248::ActionRequest& actReq = transReq.ref_actions()[0];
  actReq.set_contextId(429467294);
  actReq.set_commandRequests().resize(1);

  H248::CommandRequest& commReq = actReq.ref_commandRequests()[0];
  commReq.set_command().select_addReq().set_terminationID().resize(1);

  H248::AmmRequest& ammReq= commReq.ref_command().ref_addReq();
  H248::TerminationID& termID = ammReq.ref_terminationID()[0];

  termID.set_wildcard().resize(1);
  termID.ref_wildcard()[0].push_back('\x0');
  termID.set_id().push_back('\x0');  //   <--  you didn't assign this field

  ammReq.set_descriptors().resize(1);
  H248::StreamParms& strmParam = ammReq.ref_descriptors()[0].select_mediaDescriptor().set_streams().select_oneStream();
  strmParam.set_localDescriptor().set_propGrps().resize(1);

  strmParam.set_localDescriptor().set_propGrps()[0].resize(1);
  H248::PropertyParm& propParam = strmParam.set_localDescriptor().set_propGrps()[0][0];
  propParam.set_name().assign(&packetAndProp[0], &packetAndProp[4]);
  propParam.set_value().resize(1);
  propParam.ref_value()[0].push_back('\x01');

  ////////////////////////////////////////////////
  ASN1::CoderEnv env;
  env.set_encodingRule(ASN1::CoderEnv::ber);

  TEST("MegacoMessage (BER)", env, myReq, myReq2, d, sizeof(d));

  ////////////////////////////////////////////////
  // Set termination capabilities (Package, Property and parameter values)

  cout << "setting termination capabilities 2" << endl;

  H248::MegacoMessage myReq3, myReq4;
  myReq3.set_mess().set_version(1);
  H248::Message& myMsg1 = myReq3.ref_mess();

  ASN1::OCTET_STRING& address1 = myMsg1.set_mId().select_ip4Address().set_address();
  address1.assign(&ip[0], &ip[4]);
  myMsg1.ref_mId().ref_ip4Address().set_portNumber(5020);
  myMsg1.set_messageBody().select_messageError();

  H248::ErrorDescriptor& msgErr1 = myMsg1.ref_messageBody().ref_messageError ();
  msgErr1.set_errorCode(1);
  msgErr1.set_errorText(std::string("simple error message"));

  ////////////////////////////////////////////////
  env.set_encodingRule(ASN1::CoderEnv::ber);

  TEST("MegacoMessage2 (BER)", env, myReq3, myReq4, d1, sizeof(d1));
}
