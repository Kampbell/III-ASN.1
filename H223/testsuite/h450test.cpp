/*
 * h450test.cpp
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

#include "call_transfer_operations.h"
#include "h4501_supplementary_serviceapdu_structure.h"
#include "asn1test.h"

#ifdef ASN1_HAS_IOSTREAM
void EnumerationTest()
{
  Addressing_Data_Elements::ScreeningIndicator indicator1, indicator2;
  indicator1.set_userProvidedVerifiedAndPassed();

  ASN1::CoderEnv env;
  env.set_avn();

  TEST("ScreeningIndicator (AVN)", env, indicator1, indicator2, "userProvidedVerifiedAndPassed", sizeof("userProvidedVerifiedAndPassed"));
}
#endif

void H450Test()
{
#ifdef ASN1_HAS_IOSTREAM
  EnumerationTest();
#endif
  namespace H4501 = H4501_Supplementary_ServiceAPDU_Structure ;
  namespace HGEL = H4501_General_Error_List;
  namespace H4502 = Call_Transfer_Operations;
  namespace ROA = Remote_Operations_Apdus;
  namespace H225 = H323_MESSAGES;

  H4501::Module h4501Module;
  HGEL::Module hgelModule;
  H4502::Module h4502Module(&hgelModule);

  const H4501::OPERATION& callTransferOperations = h4502Module.get_H323CallTransferOperations();

  h4501Module.get_OperationSet().insert(callTransferOperations.begin(),
                      callTransferOperations.end());

  ASN1::CoderEnv env;
    // set encoding rule

  env.set_per_Basic_Aligned();
  env.insert(&h4501Module);

  // CTInitiate.invoke
  H4501::H4501SupplementaryService pdu1, pdu2;
  
  H4501::InvokeIDs invokeIDs;

  pdu1.set_serviceApdu().select_rosApdus().resize(1);
  ROA::ROS<H4501::InvokeIdSet, H4501::OperationSet, H4501::OperationSet>& ros = pdu1.ref_serviceApdu().ref_rosApdus()[0];
  ROA::Invoke<H4501::InvokeIdSet, H4501::OperationSet>& invoke = ros.select_invoke();
  invoke.set_invokeId(1);
  invoke.set_opcode(h4502Module.get_callTransferInitiate().operationCode);
  std::auto_ptr<H4502::CTInitiateArg> arg(new H4502::CTInitiateArg);
  arg->set_callIdentity("1111");
  arg->set_reroutingNumber().set_destinationAddress().resize(1);
  H225::AliasAddress& addr = arg->ref_reroutingNumber().ref_destinationAddress()[0];
  addr.select_h323_ID(L"Huang-Ming Huang");
  invoke.set_argument().grab(arg.release());

  char exp[] =
  "\x00\x01\x10\x00\x01\x00\x01\x09"
  "\x26\x21\x11\x10\x01\x40\x0F\x00"
  "\x48\x00\x75\x00\x61\x00\x6E\x00"
  "\x67\x00\x2D\x00\x4D\x00\x69\x00"
  "\x6E\x00\x67\x00\x20\x00\x48\x00"
  "\x75\x00\x61\x00\x6E\x00\x67";

  TEST("CTInitiate.invoke (PER)", env, pdu1, pdu2, exp, sizeof(exp));
}
