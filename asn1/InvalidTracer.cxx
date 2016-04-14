/*
 * InvalidTracer.cxx
 *
 * Copyright (c) 2001 Institute for Information Industry, Taiwan, Republic of China
 * (http://www.iii.org.tw/iiia/ewelcome.htm)
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is III ASN.1 Tool
 *
 * The Initial Developer of the Original Code is Institute for Information Industry.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): Huang-Ming Huang
 *
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors: 
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 */

#include "ios_helper.h"

namespace ASN1 {

/**
 * The InvalidTracer class defines the operations to find the first component of a constructed
 * ASN1::AbstactData object which doesn't meet its own constraint.
 *
 * @see trace_invalid()
*/
class InvalidTracer : public ConstVisitor
{
public:
  friend ASN1_STD ostream& operator << (ASN1_STD ostream& os, const InvalidTracer& tracer)
  {
#if defined(HP_aCC_RW)
    ostrstream &nc = const_cast<ostrstream&>(tracer.strm);
    os << nc.str();
    nc.rdbuf()->freeze(0);
#else
    os << tracer.strm.str();
#endif

    return os;
  }

private:
  using ConstVisitor::do_visit;
  bool do_visit(const Null& value);
  bool do_visit(const BOOLEAN& value);
  bool do_visit(const REAL& value);
  bool do_visit(const INTEGER& value);
  bool do_visit(const ENUMERATED& value);
  bool do_visit(const RELATIVE_OID& value);
  bool do_visit(const OBJECT_IDENTIFIER& value);
  bool do_visit(const BIT_STRING& value) ;
  bool do_visit(const OCTET_STRING& value);
  bool do_visit(const AbstractString& value);
  bool do_visit(const BMPString& value);
  bool do_visit(const CHOICE& value);
  bool do_visit(const OpenData& value);
  bool do_visit(const UTCTime& value);
  bool do_visit(const GeneralizedTime& value);
  bool do_visit(const SEQUENCE_OF_Base& value);

  bool visitExtensionRoot(const SEQUENCE& value, int index);
  bool visitKnownExtension(const SEQUENCE& value, int index);

#if defined(HP_aCC_RW)
  ASN1_STD ostrstream strm;
#else
  ASN1_STD ostringstream strm;
#endif
};

bool InvalidTracer::do_visit(const Null& value)
{
  return true;
}

bool InvalidTracer::do_visit(const BOOLEAN& value)
{
  return true;
}

bool InvalidTracer::do_visit(const REAL& value)
{
	return true;
}
 bool InvalidTracer::do_visit(const INTEGER& value)
{
  if (value.getLowerLimit() >= 0)
  {
    UINT_TYPE v = static_cast<UINT_TYPE>(value.getValue());
    if (value.getValue() < value.getLowerLimit())
      strm << " The INTEGER value " << v << " is smaller than lower bound " << value.getLowerLimit();
    else if (v > value.getUpperLimit() && !value.extendable())
      strm << " The INTEGER value " << v << " is greater than upper bound " << value.getUpperLimit();
    else
      return true;
  }
  else
  {
    if (value.getValue() < value.getLowerLimit() )
      strm << " The INTEGER value " << value.getValue() << " is smaller than lower bound " << value.getLowerLimit();
    else if (static_cast<UINT_TYPE>(value.getValue()) > value.getUpperLimit() && !value.extendable())
      strm << " The INTEGER value " << value.getValue() << " is greater than upper bound " << value.getUpperLimit();
    else
      return true;
  }
  return false;
}

bool InvalidTracer::do_visit(const ENUMERATED& value)
{
  if (value.asInt() > value.getMaximum())
  {
    strm << " This ENUMERATED has invalid value " << value.asInt();
    return false;
  }
  return true;
}

bool InvalidTracer::do_visit(const RELATIVE_OID& value)
{
  if (value.levels() == 0)
  {
    strm << " This RELATIVE_OID is not assigned";
    return false;
  }
  return true;
}

bool InvalidTracer::do_visit(const OBJECT_IDENTIFIER& value)
{
  if (value.levels() == 0)
  {
    strm << " This OBJECT IDENTIFIER is not assigned";
    return false;
  }
  return true;
}

bool InvalidTracer::do_visit(const BIT_STRING& value)
{
  if (value.size() < static_cast<UINT_TYPE>(value.getLowerLimit()))
    strm << " This BIT STRING has size " << value.size() << " smaller than its lower bound " << value.getLowerLimit();
  else if (value.getConstraintType() == FixedConstraint && value.size() > value.getUpperLimit())
    strm << " This BIT STRING has size " << value.size() << " greater than its upper bound " << value.getUpperLimit();
  else
    return true;

  return false;
}

bool InvalidTracer::do_visit(const OCTET_STRING& value)
{
  if (value.size() < static_cast<UINT_TYPE>(value.getLowerLimit()))
    strm << " This OCTET STRING has size " << value.size() << " smaller than its lower bound " << value.getLowerLimit();
  else if (value.getConstraintType() == FixedConstraint && value.size() > value.getUpperLimit())
    strm << " This OCTET STRING has size " << value.size() << " greater than its upper bound " << value.getUpperLimit();
  else
    return true;

  return false;
}

bool InvalidTracer::do_visit(const AbstractString& value)
{
  int pos;
  if (value.size() < static_cast<UINT_TYPE>(value.getLowerLimit()))
    strm << " This AbstractString has size " << value.size() << " smaller than its lower bound " << value.getLowerLimit();
  else if (value.getConstraintType() == FixedConstraint && value.size() > value.getUpperLimit())
    strm << " This AbstractString has size " << value.size() << " greater than its upper bound " << value.getUpperLimit();
  else if ( (pos = value.find_first_invalid()) != ASN1_STD string::npos)
    strm << " The character '" << value[pos]  << "' is not valid for the string";
  else
    return true;

  return false;
}

bool InvalidTracer::do_visit(const BMPString& value)
{
  unsigned pos;
  if (value.size() < static_cast<UINT_TYPE>(value.getLowerLimit()))
    strm << " This BMPString has size " << value.size() << " smaller than its lower bound " << value.getLowerLimit();
  else if (value.getConstraintType() == FixedConstraint && value.size() > value.getUpperLimit())
    strm << " This BMPString has size " << value.size() << " greater than its upper bound " << value.getUpperLimit();
  else if ( (pos = value.first_illegal_at()) < value.size())
    strm << " The character '" << value[pos]  << "' is not valid for the string";
  else
    return true;

  return false;
}

bool InvalidTracer::do_visit(const CHOICE& value)
{
  if (value.currentSelection() == CHOICE::unselected_)
    strm << " This CHOICE is not selected";
  else if (value.currentSelection() == CHOICE::unknownSelection_)
    strm << " This selection is not understood by this decoder";
  else
  {
    strm << "." << value.getSelectionName();
    return value.getSelection()->accept(*this);
  }
  return false;
}

bool InvalidTracer::do_visit(const OpenData& value)
{
  if (value.has_data())
    return value.get_data().accept(*this);

  if (!value.has_buf())
  {
    strm << " This Open Type does not contain any valid data";
    return false;
  }

  return true;
}

bool InvalidTracer::do_visit(const UTCTime& value)
{
  if (value.isStrictlyValid())
  {
    strm << " This UTCTime is not valid";
    return false;
  }
  return true;
}

bool InvalidTracer::do_visit(const GeneralizedTime& value)
{
  if (value.isStrictlyValid())
  {
    strm << " This GeneralizedTime is not valid";
    return false;
  }
  return true;
}

bool InvalidTracer::do_visit(const SEQUENCE_OF_Base& value)
{
  if (value.size() < static_cast<UINT_TYPE>(value.getLowerLimit()))
  {
    strm << " This SEQUENCE OF has size " << value.size() << " smaller than its lower bound " << value.getLowerLimit();
    return false;
  }
  else if (value.getConstraintType() == FixedConstraint && value.size() > value.getUpperLimit())
  {
    strm << " This SEQUENCE OF has size " << value.size() << " greater than its upper bound " << value.getUpperLimit();
    return false;
  }

  SEQUENCE_OF_Base::const_iterator first = value.begin(), last = value.end();

  for (; first != last; ++first)
  {
    InvalidTracer tracer;
    if (! (*first)->accept(tracer))
    {
      strm << "[" << first- value.begin() << "]" << tracer;
      return false;
    }
  }
  return true;
}

bool InvalidTracer::visitExtensionRoot(const SEQUENCE& value, int index)
{
  InvalidTracer tracer;
  if (!value.getField(index)->accept(tracer))
  {
    strm <<  "." << value.getFieldName(index) << tracer;
    return false;
  }
  return true;
}

bool InvalidTracer::visitKnownExtension(const SEQUENCE& value, int index)
{
  return visitExtensionRoot(value, index);
}

bool trace_invalid(ASN1_STD ostream& os, const char* str, const AbstractData& data)
{
  InvalidTracer tracer;
  if (!data.accept(tracer))
  {
    os << str << tracer << ASN1_STD endl;
    return false;
  }
  return true;
}

}
