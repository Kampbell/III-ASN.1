/*
 * main.h
 *
 * PWLib application header file for asnparser
 *
 * ASN.1 compiler to produce C++ classes.
 *
 * Copyright (c) 1997-1999 Equivalence Pty. Ltd.
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
 * The Original Code is ASN Parser.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors: 
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 *
 * $Log: main.h,v $
 * Revision 1.5  2011/08/09 18:12:43  arunasr
 * Genesys fixes: 3.0 release candidate
 *
 *  /main/12 2009/10/13 15:51:28 BST arunasr
 *     UNCTime added; compiler warnings cleanup
 * Revision 1.13  2006/05/12 20:53:28  arunasr
 * UTCTime sorted
 *
 * Revision 1.12  2005/09/14 17:34:58  arunasr
 * Parsing of Enumerated and Integer field default values as names fixed
 *
 * Revision 1.11  2005/09/14 10:05:12  arunasr
 * Value parsing corrected in BIT STRING context
 * Generation of named bits corrected
 *
 * Revision 1.10  2005/05/24 10:12:04  arunasr
 * Imported original changes from SF
 *
 * Revision 1.3.2.1  2005/05/23 14:57:55  arunasr
 * Exported object set code to be generated if exported,
 * even if not used in the current module
 * "using" directive should be generated for an imported object set
 *
 * Flattening of CHOICE corrected
 *
 * Portable code generation
 *
 * Quick and dirty fix for fields that have default values:
 *   code generated as for optional fields
 *
 * Fixes for ABSTRACT-SYNTAX
 *
 * Handling of tag modes (Explicit) fixed
 *
 * Got rid of .inl files
 *
 * Revision 1.3  2002/07/02 02:03:26  mangelo
 * Remove Pwlib dependency
 *
 * Revision 1.2  2001/09/07 22:39:17  mangelo
 * add Log keyword substitution
 *
 *
 * May 3, 2001 Huang-Ming Huang
 *   Fixed the problem with my wrong interpretation to varaible constraint.
 *
 * March, 2001 Huang-Ming Huang
 *            Add support for Information Object Class and generate code that follows
 *            X/Open ASN.1/C++ interface.
 *
 */

#ifndef _MAIN_H
#define _MAIN_H

#if defined(_MSC_VER) && (_MSC_VER <=1200)
#pragma warning(disable:4786)
#endif
#include <stdio.h>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <stack>
#include <string>
#include <iostream>
#include <boost/smart_ptr.hpp>
#include <boost/cstdint.hpp>

using std::string;
using std::vector;
using std::list;
using std::map;
using std::ostream;
using std::auto_ptr;
using std::stack;
using boost::shared_ptr;

extern unsigned lineNumber;
extern string  fileName;
extern FILE * yyin;
extern FILE * idin;
extern int yyparse();
extern int idparse();

void yyerror(char * str);
void iderror(char * str);

/////////////////////////////////////////
//
//  standard error output from parser
//

enum StdErrorType { Warning, Fatal };

class StdError {
public:
  StdError(StdErrorType ne) : e(ne) { }
  //StdError(StdErrorType ne, unsigned ln) : e(ne), l(ln) { }
  friend ostream & operator<<(ostream & out, const StdError & e);

protected:
  StdErrorType e;
};


/////////////////////////////////////////
//
//  intermediate structures from parser
//

typedef vector<string> StringList;

class NamedNumber ////: public PObject
{
public:
  NamedNumber(string * nam);
  NamedNumber(string * nam, int num);
  NamedNumber(string * nam, const string & ref);
  friend ostream& operator << (ostream &, const NamedNumber& );

  void SetAutoNumber(const NamedNumber & prev);
  const string & GetName() const { return name; }
  int GetNumber() const { return number; }

protected:
  string name;
  string reference;
  int number;
  bool autonumber;
};

class Printable {
public:
  virtual void PrintOn(ostream &) const=0;
};

inline ostream& operator << (ostream& os, const Printable& obj)
{
  obj.PrintOn(os);
  return os;
}

//PLIST(NamedNumberList, NamedNumber);
typedef boost::shared_ptr<NamedNumber> NamedNumberPtr;
typedef list<NamedNumberPtr> NamedNumberList;


// Types

class TypeBase;
typedef boost::shared_ptr<TypeBase> TypePtr;
typedef vector<TypePtr> TypesVector;

class Tag : public Printable
{
public:
  enum Type {
    Universal,
      Application,
      ContextSpecific,
      Private
  };
  enum UniversalTags {
    IllegalUniversalTag,
      UniversalBoolean,
      UniversalInteger,
      UniversalBitString,
      UniversalOctetString,
      UniversalNull,
      UniversalObjectId,
      UniversalObjectDescriptor,
      UniversalExternalType,
      UniversalReal,
      UniversalEnumeration,
      UniversalEmbeddedPDV,
      UniversalSequence = 16,
      UniversalSet,
      UniversalNumericString,
      UniversalPrintableString,
      UniversalTeletexString,
      UniversalVideotexString,
      UniversalIA5String,
      UniversalUTCTime,
      UniversalGeneralisedTime,
      UniversalGraphicString,
      UniversalVisibleString,
      UniversalGeneralString,
      UniversalUniversalString,
      UniversalBMPString = 30
  };
  enum Mode {
    Implicit,
      Explicit,
      Automatic
    };
    Tag(unsigned tagNum, Mode m);
    Tag() : type(Universal), number(0), mode(Implicit) {}

  bool isDefined() const { return (type|number)!=0; }

  void PrintOn(ostream &) const;
  bool operator == (const Tag& other) const {
    return type == other.type && number == other.number && mode == other.mode;
  }

  bool operator != (const Tag& other) const {
    return !(*this == other);
  }

  Type type;
  unsigned number;
  Mode mode;

  static const char * classNames[];
  static const char * modeNames[];
};

class ValueSet;
class Constraint;
class Parameter;
class SizeConstraintElement;
class FromConstraintElement;
class SubTypeConstraintElement;

typedef boost::shared_ptr<ValueSet> ValueSetPtr;
typedef boost::shared_ptr<Constraint> ConstraintPtr;

class ConstraintElementBase : public Printable
{
public:
  ConstraintElementBase();
  ~ConstraintElementBase();

  void SetExclusions(boost::shared_ptr<ConstraintElementBase> excl) { exclusions = excl; }

  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual void GetConstraint(string& ) const {}
  virtual bool ReferencesType(const TypeBase & type) const;

  virtual ValueSetPtr GetValueSetFromValueField(const string& field) const;
  virtual ValueSetPtr GetValueSetFromValueSetField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const;

  virtual bool HasPERInvisibleConstraint(const Parameter&) const { return false;}
  virtual void GenerateObjectSetInstanceCode(const string& , ostream& ) const{}
  virtual void GenerateObjSetAccessCode(ostream& ){}
  virtual bool GetCharacterSet(string& characterSet) const;
  virtual const SizeConstraintElement* GetSizeConstraint() const;
  virtual const FromConstraintElement* GetFromConstraint() const;
  virtual const SubTypeConstraintElement* GetSubTypeConstraint() const;

  // do not set const here - it is different function!!!
  virtual void PrintOn(ostream&) {}

protected:
  boost::shared_ptr<ConstraintElementBase> exclusions;
};

typedef boost::shared_ptr<ConstraintElementBase> ConstraintElementPtr;
typedef vector<ConstraintElementPtr> ConstraintElementVector;

class Constraint : public Printable
{
public:
  Constraint(bool extend) : extendable(extend){};
  Constraint(ConstraintElementPtr& elmt);
  Constraint(auto_ptr<ConstraintElementVector> std,
    bool extend,
    auto_ptr<ConstraintElementVector> ext =
        auto_ptr<ConstraintElementVector>());

  Constraint(const Constraint& other);

  void PrintOn(ostream &) const;
  void PrintElements(ostream &) const;

  bool IsExtendable() const { return extendable; }
  void GetConstraint(string& str) const ;
  void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  bool ReferencesType(const TypeBase & type) const;

  const ConstraintElementVector& GetStandardElements() const { return standard; }
  const ConstraintElementVector& GetExtensionElements() const { return extensions; }
  ConstraintElementVector& GetStandardElements() { return standard; }
  ConstraintElementVector& GetExtensionElements() { return extensions; }

  ValueSetPtr GetValueSetFromValueField(const string& field) const;
  ValueSetPtr GetValueSetFromValueSetField(const string& field) const;
  ConstraintPtr GetObjectSetFromObjectField(const string& field) const;
  ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const;
  const SizeConstraintElement* GetSizeConstraint() const;
  const FromConstraintElement* GetFromConstraint() const;
  const SubTypeConstraintElement* GetSubTypeConstraint() const;

  void GetCharacterSet(string& characterSet) const;

  virtual auto_ptr<Constraint> Clone() const;
  bool HasPERInvisibleConstraint(const Parameter&) const;
  void GenerateObjectSetInstanceCode(const string& prefix, ostream& cxx) const;
  void GenerateObjSetAccessCode(ostream& );

protected:
  ConstraintElementVector standard;
  bool                  extendable;
  ConstraintElementVector extensions;
};

typedef boost::shared_ptr<Constraint> ConstraintPtr;
typedef vector<ConstraintPtr> ConstraintList;

class ConstrainAllConstraintElement : public ConstraintElementBase
{
public:
  ConstrainAllConstraintElement(ConstraintElementPtr excl);
  void PrintOn(ostream &) const{}
};



class ElementListConstraintElement : public ConstraintElementBase
{
public:
  ElementListConstraintElement();
  ElementListConstraintElement(auto_ptr<ConstraintElementVector> list);
  void PrintOn(ostream &) const;

  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual void GetConstraint(string& str) const;
  virtual bool ReferencesType(const TypeBase & type) const;
  const ConstraintElementVector& GetElements() const { return elements; }

  virtual ValueSetPtr GetValueSetFromValueField(const string& field) const;
  virtual ValueSetPtr GetValueSetFromValueSetField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const;
  virtual bool HasPERInvisibleConstraint(const Parameter&) const;
  virtual void GenerateObjectSetInstanceCode(const string& prefix, ostream& cxx) const;
  virtual void GenerateObjSetAccessCode(ostream& );

  virtual const SizeConstraintElement* GetSizeConstraint() const;
  virtual const FromConstraintElement* GetFromConstraint() const;

  void AppendElements(
    ConstraintElementVector::const_iterator first,
    ConstraintElementVector::const_iterator last
    );
protected:
  ConstraintElementVector elements;
};


class ValueBase;
typedef boost::shared_ptr<ValueBase> ValuePtr;

class SingleValueConstraintElement : public ConstraintElementBase
{
public:
  SingleValueConstraintElement(const ValuePtr& val);
  ~SingleValueConstraintElement();
  void PrintOn(ostream &) const;

  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual void GetConstraint(string& str) const;

  const ValuePtr& GetValue() const { return value; }
  virtual bool HasPERInvisibleConstraint(const Parameter&) const;
  virtual bool GetCharacterSet(string& characterSet) const;

protected:
  const ValuePtr value;
private:
  SingleValueConstraintElement& operator = (const SingleValueConstraintElement&);
};


class ValueRangeConstraintElement : public ConstraintElementBase
{
public:
  ValueRangeConstraintElement(ValuePtr lowerBound, ValuePtr upperBound);
  ~ValueRangeConstraintElement();
  void PrintOn(ostream &) const;
  void GenerateRange(ostream& strm);
  virtual void GetConstraint(string& str) const;

  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual bool HasPERInvisibleConstraint(const Parameter&) const;
  virtual bool GetCharacterSet(string& characterSet) const;
protected:
  ValuePtr lower;
  ValuePtr upper;
};



class SubTypeConstraintElement : public ConstraintElementBase
{
  //PCLASSINFO(SubTypeConstraintElement, ConstraintElementBase);
public:
  SubTypeConstraintElement(TypePtr typ);
  ~SubTypeConstraintElement();
  void PrintOn(ostream &) const;
  void GenerateCplusplus(const string &, ostream &, ostream &, ostream &) const;
  virtual bool ReferencesType(const TypeBase & type) const;
  virtual bool HasPERInvisibleConstraint(const Parameter&) const;
  virtual void GetConstraint(string& str) const;
  virtual const SubTypeConstraintElement* GetSubTypeConstraint() const;
  string GetSubTypeName() const;
  const TypePtr GetSubType() const { return subtype;}
protected:
  TypePtr subtype;
};


class NestedConstraintConstraintElement : public ConstraintElementBase
{
public:
  NestedConstraintConstraintElement(ConstraintPtr con);
  ~NestedConstraintConstraintElement();

  virtual bool ReferencesType(const TypeBase & type) const;
  virtual bool HasPERInvisibleConstraint(const Parameter&) const;

protected:
  ConstraintPtr constraint;
};


class SizeConstraintElement : public NestedConstraintConstraintElement
{
public:
  SizeConstraintElement(ConstraintPtr constraint);
  virtual void GetConstraint(string& str) const;
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual const SizeConstraintElement* GetSizeConstraint() const;
};


class FromConstraintElement : public NestedConstraintConstraintElement
{
public:
  FromConstraintElement(ConstraintPtr constraint);
  virtual void GetConstraint(string& str) const;
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual const FromConstraintElement* GetFromConstraint() const;
  string GetCharacterSet(const char* canonicalSet, int canonicalSetSize) const;
  int GetRange(ostream& cxx) const;
};


class WithComponentConstraintElement : public NestedConstraintConstraintElement
{
public:
  WithComponentConstraintElement(string name, ConstraintPtr constraint, int presence);
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual bool HasPERInvisibleConstraint(const Parameter&) const { return false; }

  enum {
    Present,
      Absent,
      Optional,
      Default
  };

protected:
  string name;
  int     presence;
};


class InnerTypeConstraintElement : public ElementListConstraintElement
{
public:
  InnerTypeConstraintElement(auto_ptr<ConstraintElementVector> list, bool partial);

  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;

protected:
  bool partial;
};

class ActualParameter;
typedef boost::shared_ptr<ActualParameter> ActualParameterPtr;
typedef vector<ActualParameterPtr> ActualParameterList;
typedef boost::shared_ptr<ActualParameterList> ActualParameterListPtr;

class UserDefinedConstraintElement : public ConstraintElementBase
{
public:
  UserDefinedConstraintElement(ActualParameterListPtr list);
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(const string & fn, ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual bool HasPERInvisibleConstraint(const Parameter&) const { return true; }
protected:
  ActualParameterListPtr parameters;
};

class DefinedObjectSet;
typedef boost::shared_ptr<DefinedObjectSet> DefinedObjectSetPtr;
class TableConstraint
{
public:
  TableConstraint(boost::shared_ptr<DefinedObjectSet> objSet,
      auto_ptr<StringList> atNotations = auto_ptr<StringList>());
  ~TableConstraint();
  bool ReferenceType(const TypeBase& type);
  string GetObjectSetIdentifier() const;
  const StringList* GetAtNotations() const { return atNotations.get();}
private:
  boost::shared_ptr<DefinedObjectSet> objSet;
  auto_ptr<StringList> atNotations;
};

/////////////////////////////////////////////

class Parameter : public Printable
{
public:
  Parameter(const string& name);
  Parameter(const string& name, int type);
  Parameter(const Parameter& other);
  int GetIdentifierType();
  const string& GetName() const;
  void PrintOn(ostream& strm) const;
  virtual bool IsTypeParameter() const { return true;}
  virtual bool ReferencedBy(const TypeBase&) const ;
  virtual ActualParameterPtr MakeActualParameter() const;
protected:
  string name;
  int identifierType;
};

class ValueParameter : public Parameter
{
public:
  ValueParameter(TypePtr governor, const string& nam );
  ValueParameter(const ValueParameter& other);
  ~ValueParameter();
  TypeBase* GetGovernor() { return governor.get();}
  void PrintOn(ostream& strm) const;
  virtual bool IsTypeParameter() const { return false;}
  virtual bool ReferencedBy(const TypeBase&) const { return false;}
  virtual ActualParameterPtr MakeActualParameter() const;
protected:
  TypePtr governor;
};

class DefinedObjectClass;
typedef boost::shared_ptr<DefinedObjectClass> DefinedObjectClassPtr;
class ObjectParameter : public Parameter
{
public:
  ObjectParameter(DefinedObjectClassPtr governor,
    const string& name);
  ~ObjectParameter();
  DefinedObjectClass* GetGovernor() { return governor.get();}
  void PrintOn(ostream& strm) const;
  virtual bool IsTypeParameter() const { return false;}
  virtual bool ReferencedBy(const TypeBase&) const ;
  virtual ActualParameterPtr MakeActualParameter() const;
protected:
  DefinedObjectClassPtr governor;
};

typedef boost::shared_ptr<Parameter> ParameterPtr;
typedef vector<ParameterPtr> ParameterListRep;

class ParameterList : public Printable
{
public:
  void Append(ParameterPtr param);
  bool IsEmpty() const { return rep.empty();}
  int GetIdentifierType(const char* identifier);
  Parameter* GetParameter(const char* identifier);
  void GenerateCplusplus(string& templatePrefix, string& classNameString);
  void PrintOn(ostream& strm) const;
  boost::shared_ptr<ParameterList> GetReferencedParameters(const TypeBase& type) const;
  ActualParameterListPtr MakeActualParameters() const;
  void swap(ParameterList& other) { rep.swap(other.rep); }
  ParameterListRep rep;
};

typedef boost::shared_ptr<ParameterList> ParameterListPtr;

////////////////////////////////////////////
class ModuleDefinition;

class TypeBase : public Printable
{
public:
  void PrintOn(ostream &) const;

  void BeginParseValue() const;
  void EndParseValue() const;
  void BeginParseValueSet() const;
  void EndParseValueSet() const;

  const string & GetName() const { return name; }
  void SetName(const string& name);
  void SetAsValueSetType() { isValueSetType = true;}
  const string& GetIdentifier() const { return identifier; }
  void SetTag(Tag::Type cls, unsigned num, Tag::Mode mode);
  void SetTag(const Tag& _tag) { tag = _tag; }
  const Tag & GetTag() const { return tag; }
  const Tag & GetDefaultTag() const { return defaultTag; }
  bool HasNonStandardTag() const { return tag.isDefined() && tag != defaultTag; }
  void SetParameters(ParameterList& list);
  void AddConstraint(ConstraintPtr constraint) { constraints.push_back(constraint); }
  bool HasConstraints() const ;
  void MoveConstraints(TypeBase& from);
  void CopyConstraints(const TypeBase& from);
  virtual bool HasParameters() const { return !parameters.IsEmpty(); }
  bool IsOptional() const { return isOptional; }
  void SetOptional() { isOptional = true; }
  bool HasDefaultValue() const { return defaultValue.get()!=0; }
  ValuePtr GetDefaultValue() const { return defaultValue; }
  void SetDefaultValue(ValuePtr value);
  const string & GetTemplatePrefix() const { return templatePrefix; }
  const string & GetClassNameString() const { return classNameString; }
  void SetOuterClassName(const string& oname) { outerClassName = oname; }
  void SetTemplatePrefix(const string& tname) { templatePrefix = tname; }
  bool IsValueSetType() const { return isValueSetType;}

  virtual void AdjustIdentifier(bool);
  virtual void FlattenUsedTypes();
  virtual TypePtr FlattenThisType(TypePtr& self, const TypeBase & parent);
  virtual bool IsChoice() const;
  virtual bool IsParameterizedType() const;
  virtual bool IsPrimitiveType() const;
  virtual bool IsSequenceOfType() const { return false;}
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual void GenerateForwardDecls(ostream & hdr);
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual const char * GetAncestorClass() const = 0;
  virtual string GetTypeName() const;
  virtual bool CanReferenceType() const;
  virtual bool ReferencesType(const TypeBase & type) const;
  virtual const string& GetCModuleName() const;
  virtual bool IsParameterisedImport() const;
  virtual bool CanBeFwdDeclared(bool isComponent = false) const;
  virtual bool FwdDeclareMe(ostream & hdr);
  virtual void GenerateInfo(const TypeBase* type, ostream & hdr, ostream& cxx);

  bool IsGenerated() const { return isGenerated; }
  virtual void BeginGenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  void EndGenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  void GenerateTags(ostream & strm) const;
  void GenerateCplusplusConstraints(const string & prefix, ostream & hdr, ostream & cxx, ostream & inl) const;

  const ConstraintList& GetConstraints() const { return constraints;}
  virtual void BeginParseThisTypeValue() const {}
  virtual void EndParseThisTypeValue() const {}
  virtual void ResolveReference() const {}

  virtual string GetPrimitiveType(const string& myName) const;

  virtual void RemovePERInvisibleConstraint(const ParameterPtr&);
  void RemovePERInvisibleConstraints();
  virtual bool UseType(const TypeBase& ) const { return false; }

  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
  virtual bool NeedGenInfo() const ;
  TypePtr SeqOfFlattenThisType(const TypeBase & parent, TypePtr thisPtr);

  const ParameterList& GetParameters() const { return parameters; }
  virtual void GenerateDecoder(ostream&){}

  enum RemoveResult
  {
    OK,
      MAY_NOT,
      FORBIDDEN
  };
  virtual RemoveResult CanRemoveType(const TypeBase&) { return OK; }
  virtual bool RemoveThisType(const TypeBase& type) { return GetName() == type.GetName();}
  virtual bool IsRemovedType() const { return false;}
  ModuleDefinition* GetModule() const { return module;}
protected:
  TypeBase(unsigned tagNum, ModuleDefinition* md);
  TypeBase(TypeBase& copy);

  void PrintStart(ostream &) const;
  void PrintFinish(ostream &) const;
  const char* GetClass() const;

  Tag				tag;
  Tag				defaultTag;
  string			name; // The ASN.1 Type name
  string			identifier; // The converted C Type name
  ConstraintList	constraints;
  bool				isOptional;
  ValuePtr			defaultValue;
  bool				isGenerated;
  ParameterList		parameters;
  string			templatePrefix;
  string			classNameString;
  string			shortClassNameString;
  string			outerClassName;
  bool				isValueSetType;
  ModuleDefinition* module;
};



class DefinedType : public TypeBase
{
public:
  DefinedType(const string& name);
  DefinedType(TypePtr refType);
  DefinedType(TypePtr refType, TypePtr& bType);
  DefinedType(TypePtr refType, const string & name);
  DefinedType(TypePtr refType, const TypeBase & parent);

  void PrintOn(ostream &) const;

  virtual bool IsChoice() const;
  virtual bool IsParameterizedType() const;
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual const char * GetAncestorClass() const;
  virtual string GetTypeName() const;
  virtual bool CanReferenceType() const;
  virtual bool ReferencesType(const TypeBase & type) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual bool CanBeFwdDeclared(bool isComponent = false) const ;

  virtual void BeginParseThisTypeValue() const;
  virtual void EndParseThisTypeValue() const;
  virtual void ResolveReference() const;
  virtual const string& GetCModuleName() const;
  virtual bool UseType(const TypeBase& type) const ;
  virtual bool NeedGenInfo() const;
  virtual RemoveResult CanRemoveType(const TypeBase&) ;
  virtual bool RemoveThisType(const TypeBase&);
  virtual string GetPrimitiveType(const string& myName) const;
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);
  virtual TypePtr FlattenThisType(TypePtr& self, const TypeBase & parent);

protected:
  void ConstructFromType(TypePtr& refType, const string & name);
  string referenceName;
  mutable TypePtr baseType;
  mutable bool unresolved;
};


class ParameterizedType : public DefinedType
{
public:
  ParameterizedType(const string& name, ActualParameterList& args);
  ParameterizedType(TypePtr& refType,
    const TypeBase& parent,
    ActualParameterList& args);

  void PrintOn(ostream &) const;

  virtual bool IsParameterizedType() const;
  virtual string GetTypeName() const;
  virtual bool ReferencesType(const TypeBase & type) const;
  virtual bool UseType(const TypeBase& type) const ;
  virtual RemoveResult CanRemoveType(const TypeBase&);
protected:
  ActualParameterList arguments;
};


class SelectionType : public TypeBase
{
public:
  SelectionType(const string& name, TypePtr base);
  ~SelectionType();

  void PrintOn(ostream &) const;

  virtual void FlattenUsedTypes();
  virtual TypePtr FlattenThisType(TypePtr& self, const TypeBase & parent);
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual const char * GetAncestorClass() const;
  virtual bool CanReferenceType() const;
  virtual bool ReferencesType(const TypeBase & type) const;
  virtual bool UseType(const TypeBase& type) const ;

protected:
  string selection;
  TypePtr baseType;
};


class BooleanType : public TypeBase
{
public:
  BooleanType();
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual const char * GetAncestorClass() const;
  virtual string GetPrimitiveType(const string& myName) const { return "bool";}
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
};


class IntegerType : public TypeBase
{
public:
  IntegerType();
  IntegerType(NamedNumberList&);
  virtual const char * GetAncestorClass() const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  string GetTypeName() const;
  virtual string GetPrimitiveType(const string& myName) const {
    if(!myName.empty())
      return myName + "::value_type::int_type";
    else
      return "int_type";
  }

  virtual bool CanReferenceType() const;
  virtual bool ReferencesType(const TypeBase & type) const;

  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual bool NeedGenInfo() const;
  virtual void GenerateInfo(const TypeBase* type, ostream& , ostream&);
  virtual TypePtr FlattenThisType(TypePtr& self, const TypeBase & parent);

  virtual void
    BeginParseThisTypeValue() const,
    EndParseThisTypeValue() const;

protected:
  NamedNumberList allowedValues;
};


class EnumeratedType : public TypeBase
{
public:
  EnumeratedType(NamedNumberList& enums, bool extend, NamedNumberList* ext);
  void PrintOn(ostream &) const;
  virtual TypePtr FlattenThisType(TypePtr& self, const TypeBase & parent);
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual const char * GetAncestorClass() const;
  virtual string GetPrimitiveType(const string& myName) const {
    if(!myName.empty())
      return myName + "::value_type::NamedNumber";
  else
    return "NamedNumber";
  }
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
  virtual bool NeedGenInfo() const { return true;}
  bool IsPrimitiveType() const;
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);

  virtual void
    BeginParseThisTypeValue() const,
    EndParseThisTypeValue() const;

protected:
  NamedNumberList enumerations;
  size_t numEnums;
  bool extendable;
  int maxEnumValue;
};


class RealType : public TypeBase
{
public:
  RealType();
  virtual const char * GetAncestorClass() const;
  virtual string GetPrimitiveType(const string& myName) const { return "double";}
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
};


class BitStringType : public TypeBase
{
public:
  BitStringType();
  BitStringType(NamedNumberList&);
  virtual const char * GetAncestorClass() const;
  string GetTypeName() const;
  //virtual string GetPrimitiveType(const string& myName) const { return TypeBase::GetPrimitiveType();}
  virtual bool NeedGenInfo() const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);

  virtual int GetToken() const;
  virtual void BeginParseThisTypeValue() const;
  virtual void EndParseThisTypeValue() const;

protected:
  NamedNumberList allowedBits;
};


class OctetStringType : public TypeBase
{
public:
  OctetStringType();
  virtual const char * GetAncestorClass() const;
  string GetTypeName() const;
  virtual string GetPrimitiveType(const string& myName) const { return "const ASN1_STD vector<char>&";}
  virtual const char* GetConstrainedType() const;
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);
};


class NullType : public TypeBase
{
public:
  NullType();
  virtual const char * GetAncestorClass() const;
  virtual void BeginParseThisTypeValue() const;
  virtual void EndParseThisTypeValue() const;
};


class SequenceType : public TypeBase
{
  void PrintOn(ostream &) const;
public:
  SequenceType(TypesVector* std,
    bool extendable,
    TypesVector * extensions,
    unsigned tagNum = Tag::UniversalSequence);
  virtual void FlattenUsedTypes();
  virtual TypePtr FlattenThisType(TypePtr& self, const TypeBase & parent);
  virtual bool IsPrimitiveType() const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual const char * GetAncestorClass() const;
  virtual bool CanReferenceType() const;
  virtual bool ReferencesType(const TypeBase & type) const;

  void GenerateComponent(TypeBase& field, ostream & hdr, ostream & cxx, ostream& inl, int id);
  virtual bool CanBeFwdDeclared(bool isComponent ) const ;
  virtual void RemovePERInvisibleConstraint(const ParameterPtr&);
  virtual bool UseType(const TypeBase& type) const ;
  virtual bool NeedGenInfo() const { return true; }
  virtual void GenerateForwardDecls(ostream & hdr);
  virtual RemoveResult CanRemoveType(const TypeBase&) ;
  virtual bool RemoveThisType(const TypeBase&);
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);
protected:
  TypesVector fields;
  size_t numFields;
  mutable vector<bool> needFwdDeclare;
  bool extendable;
  mutable bool detectingLoop;
};


class SequenceOfType : public TypeBase
{
public:
  SequenceOfType(TypePtr base, ConstraintPtr constraint = ConstraintPtr(), unsigned tag = Tag::UniversalSequence);
  ~SequenceOfType();
  void PrintOn(ostream &) const;
  virtual void FlattenUsedTypes();
  virtual TypePtr FlattenThisType(TypePtr& self, const TypeBase & parent);
  virtual bool IsPrimitiveType() const;
  virtual void GenerateForwardDecls(ostream & hdr);
  virtual const char * GetAncestorClass() const;
  virtual bool CanReferenceType() const;
  virtual bool ReferencesType(const TypeBase & type) const;
  virtual string GetTypeName() const;
  virtual bool FwdDeclareMe(ostream & hdr);
  virtual bool IsSequenceOfType() const { return true;}
  virtual void RemovePERInvisibleConstraint(const ParameterPtr&);
  virtual bool UseType(const TypeBase& type) const ;
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
  void SetNonTypedef(bool v) { nonTypedef = v;}
  virtual RemoveResult CanRemoveType(const TypeBase&) ;
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);
protected:
  TypePtr baseType;
  bool nonTypedef;
};


class SetType : public SequenceType
{
public:
  SetType();
  SetType(SequenceType& seq);
  virtual const char * GetAncestorClass() const;
};


class SetOfType : public SequenceOfType
{
public:
  SetOfType(TypePtr base, ConstraintPtr constraint = ConstraintPtr());
  virtual string GetTypeName() const;
};


class ChoiceType : public SequenceType
{
public:
  ChoiceType(TypesVector * std = NULL,
    bool extendable = false,
    TypesVector * extensions = NULL);
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual bool IsPrimitiveType() const;
  virtual bool IsChoice() const;
  virtual TypePtr FlattenThisType(TypePtr& self, const TypeBase & parent);
  virtual const char * GetAncestorClass() const;
  void GenerateComponent(TypeBase& field, ostream & hdr, ostream & cxx, ostream& inl, int id);
  virtual RemoveResult CanRemoveType(const TypeBase&) ;
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);
private:
  vector<TypeBase*> sortedFields;
};


class EmbeddedPDVType : public TypeBase
{
public:
  EmbeddedPDVType();
  virtual const char * GetAncestorClass() const;
};


class ExternalType : public TypeBase
{
public:
  ExternalType();
  virtual const char * GetAncestorClass() const;
};


class AnyType : public TypeBase
{
public:
  AnyType(const string& ident);
  void PrintOn(ostream & strm) const;
  virtual const char * GetAncestorClass() const;
protected:
  string identifier;
};


class StringTypeBase : public TypeBase
{
public:
  StringTypeBase(int tag);
  virtual string GetTypeName() const;
  virtual string GetPrimitiveType(const string& myName) const { return "const ASN1_STD string&";}
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual bool NeedGenInfo() const;
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);
  virtual const char* GetCanonicalSetString() const { return NULL;};
protected:
  const char* canonicalSet;
  const char* canonicalSetRep;
  int canonicalSetSize;
};


class BMPStringType : public StringTypeBase
{
public:
  BMPStringType();
  virtual const char * GetAncestorClass() const;
  virtual string GetPrimitiveType(const string& myName) const { return "const ASN1_STD wstring&";}
  virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);
};


class GeneralStringType : public StringTypeBase
{
public:
  GeneralStringType();
  virtual const char * GetAncestorClass() const;
};


class GraphicStringType : public StringTypeBase
{
public:
  GraphicStringType();
  virtual const char * GetAncestorClass() const;
};


class IA5StringType : public StringTypeBase
{
public:
  IA5StringType();
  virtual const char * GetAncestorClass() const;
};


class ISO646StringType : public StringTypeBase
{
public:
  ISO646StringType();
  virtual const char * GetAncestorClass() const;
};


class NumericStringType : public StringTypeBase
{
public:
  NumericStringType();
  virtual const char * GetAncestorClass() const;
};


class PrintableStringType : public StringTypeBase
{
public:
  PrintableStringType();
  virtual const char * GetAncestorClass() const;
};


class TeletexStringType : public StringTypeBase
{
public:
  TeletexStringType();
  virtual const char * GetAncestorClass() const;
};


class T61StringType : public StringTypeBase
{
public:
  T61StringType();
  virtual const char * GetAncestorClass() const;
};


class UniversalStringType : public StringTypeBase
{
public:
  UniversalStringType();
  virtual const char * GetAncestorClass() const;
};


class VideotexStringType : public StringTypeBase
{
public:
  VideotexStringType();
  virtual const char * GetAncestorClass() const;
};


class VisibleStringType : public StringTypeBase
{
public:
  VisibleStringType();
  virtual const char * GetAncestorClass() const;
};


class UnrestrictedCharacterStringType : public StringTypeBase
{
public:
  UnrestrictedCharacterStringType();
  virtual const char * GetAncestorClass() const;
};


class GeneralizedTimeType : public TypeBase
{
public:
  GeneralizedTimeType();
  virtual const char * GetAncestorClass() const;
  virtual string GetPrimitiveType() const { return "const char*";}
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
};


class UTCTimeType : public TypeBase
{
public:
  UTCTimeType();
  virtual const char * GetAncestorClass() const;
  virtual string GetPrimitiveType() const { return "const char*";}
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
};


class ObjectDescriptorType : public TypeBase
{
public:
  ObjectDescriptorType();
  virtual const char * GetAncestorClass() const;
};


class ObjectIdentifierType : public TypeBase
{
public:
  ObjectIdentifierType();
  virtual const char * GetAncestorClass() const;
  virtual void BeginParseThisTypeValue() const;
  virtual void EndParseThisTypeValue() const;
  virtual void GenerateConstructors(ostream & hdr, ostream & cxx, ostream & inl);
};



class ObjectClassBase;
typedef boost::shared_ptr<ObjectClassBase> ObjectClassBasePtr;

class ObjectClassFieldType : public TypeBase
{
public:
  ObjectClassFieldType(ObjectClassBasePtr  objclass, const string& field);
  ~ObjectClassFieldType();
  virtual const char * GetAncestorClass() const;
  void PrintOn(ostream &) const;
  virtual bool CanReferenceType() const;
  virtual bool ReferencesType(const TypeBase & type) const;
  TypeBase* GetFieldType() ;
  const TypeBase* GetFieldType() const ;
  virtual string GetTypeName() const;
  void AddTableConstraint(boost::shared_ptr<TableConstraint> constraint);
  void GenerateDecoder(ostream&);
  virtual void GenerateInfo(const TypeBase* type, ostream& hdr, ostream& cxx);
  string GetConstrainedTypeName() const;
protected:
  ObjectClassBasePtr asnObjectClass;
  string asnObjectClassField;
  boost::shared_ptr<TableConstraint> tableConstraint;
};


class ImportedType : public TypeBase
{
public:
  ImportedType(const TypePtr& ref);
  ImportedType(const string& name, bool parameterised);
  virtual const char * GetAncestorClass() const;
  virtual void AdjustIdentifier(bool usingNamespace);
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual bool IsParameterisedImport() const;

  virtual void BeginParseThisTypeValue() const;
  virtual void EndParseThisTypeValue() const;

  void SetModuleName(const string& name) ;
  virtual const string& GetCModuleName() const { return cModuleName; }
  const string& GetModuleName() const { return moduleName; }
  bool IsPrimitiveType() const;
protected:
  string modulePrefix;
  bool    parameterised;
  const TypePtr reference;
  string moduleName;
  string cModuleName;
private:
  ImportedType& operator = (const ImportedType&);
};


class InformationObject;
typedef boost::shared_ptr<InformationObject> InformationObjectPtr;


class TypeFromObject : public TypeBase
{
public:
  TypeFromObject(InformationObjectPtr  obj, const string& fld);
  ~TypeFromObject();
  virtual const char * GetAncestorClass() const;
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
protected:
  InformationObjectPtr refObj;
  string field;
};

class RemovedType : public TypeBase
{
public:
  RemovedType(const TypeBase& type);
  virtual bool IsRemovedType() const { return true;}
  virtual const char * GetAncestorClass() const;
};

// Values

class ValueBase : public Printable
{
public:
  void SetValueName(const string& name);
  const string & GetName() const { return valueName; }

  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual void GenerateConst(ostream & hdr, ostream & cxx) const;
  virtual bool IsPERInvisibleConstraint(const Parameter&) const { return false;}

protected:
  void PrintBase(ostream &) const;
  string valueName;
};

typedef vector<ValuePtr> ValuesList;

class DefinedValue : public ValueBase
{
  //PCLASSINFO(DefinedValue, ValueBase);
public:
  DefinedValue(const string& name);
  DefinedValue(const ValuePtr&);
  DefinedValue(const string& name, const ValuePtr& );
  void PrintOn(ostream &) const;
  const string & GetReference() const { return referenceName; }
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual bool IsPERInvisibleConstraint(const Parameter& param) const { return param.GetName() == referenceName;}
protected:
  string referenceName;
  mutable ValuePtr actualValue;
  mutable bool unresolved;
};

class ImportedValue : public DefinedValue
{
public:
  ImportedValue(const string& modName, const string& name, const ValuePtr& v)
    : DefinedValue(name, v), moduleName(modName){}
  const string& GetModuleName() const { return moduleName; }
private:
  string moduleName;
};


class BooleanValue : public ValueBase
{
public:
  BooleanValue(bool newVal);
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
protected:
  bool value;
};


class IntegerValue : public ValueBase
{
public:
  IntegerValue(boost::int64_t newVal);
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
  virtual void GenerateConst(ostream & hdr, ostream & cxx) const;

  #if (__SIZEOF_LONG__ != 8)
  operator boost::int64_t() const { return value; }
  #endif
  operator long() const { return (long)value; }

protected:
  boost::int64_t value;
};


class RealValue : public ValueBase
{
public:
  RealValue(double newVal);
  void PrintOn(ostream &) const {}
protected:
  double value;
};


class OctetStringValue : public ValueBase
{
public:
  OctetStringValue() { }
  OctetStringValue(const string& newVal);
  void PrintOn(ostream &) const {}
protected:
  vector<char> value;
};


class BitStringValue : public ValueBase
{
public:
  BitStringValue() { }
  BitStringValue(const string& newVal);
  BitStringValue(StringList * newVal);
  void PrintOn(ostream &) const;
protected:
  string value;
};


class NullValue : public ValueBase
{
public:
  void PrintOn(ostream &) const {}
};


class CharacterValue : public ValueBase
{
public:
  CharacterValue(char c);
  CharacterValue(char t1, char t2);
  CharacterValue(char q1, char q2, char q3, char q4);
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
  unsigned GetValue() const { return value;}
protected:
  unsigned value;
};


class CharacterStringValue : public ValueBase
{
public:
  CharacterStringValue() { }
  CharacterStringValue(const string& newVal);
  CharacterStringValue(StringList& newVal);
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
  void GetValue(string& v) const;
protected:
  string value;
};


class ObjectIdentifierValue : public ValueBase
{
public:
  ObjectIdentifierValue(const string& newVal);
  ObjectIdentifierValue(StringList& newVal);
  void GenerateCplusplus(ostream &hdr, ostream & cxx, ostream & inl) const;
  void GenerateConst(ostream & hdr, ostream & cxx) const;
  void PrintOn(ostream &) const;
protected:
  StringList value;
};


class MinValue : public ValueBase
{
public:
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
};


class MaxValue : public ValueBase
{
public:
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
};


class SequenceValue : public ValueBase
{
public:
  SequenceValue(ValuesList * list = NULL);
  void PrintOn(ostream &) const;
protected:
  ValuesList values;
};

class ChoiceValue : public ValueBase
{
public:
  ChoiceValue(const TypePtr& typ, const string& fieldName, ValuePtr val)
    : type(typ), fieldname(fieldName), value(val) { }
  void PrintOn(ostream &) const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl) const;
protected:
  const TypePtr type;
  string fieldname;
  ValuePtr value;
private:
  ChoiceValue& operator = (const ChoiceValue&);
};

// ValueSet
class ValueSet : public Printable
{
public:
  virtual void Union(ValueSetPtr&)=0;
  virtual void Intersect(ValueSetPtr&)=0;
  virtual TypePtr MakeValueSetType()=0;
  virtual TypePtr GetType()=0;
  virtual ConstraintElementVector* GetElements()=0;
  virtual void ResolveReference() const {}
};

class ValueSetDefn : public ValueSet
{
public:
  ValueSetDefn();
  ValueSetDefn(TypePtr type, ConstraintPtr cons);
  ~ValueSetDefn();

  void Union(ValueSetPtr&);
  void Intersect(ValueSetPtr&);
  TypePtr MakeValueSetType();
  void PrintOn(ostream & ) const;

  TypePtr GetType() { return type;}
  ConstraintElementVector* GetElements() { return elements.get();}
  void ResolveReference() const;
protected:
  TypePtr type;
  auto_ptr<ConstraintElementVector> elements;
  bool extendable;
};



class ObjectSetConstraintElement;
class ValueSetFromObject : public ValueSet
{
public:
  ValueSetFromObject(InformationObjectPtr obj, const string& fld);
  ~ValueSetFromObject();
  void Union(ValueSetPtr&);
  void Intersect(ValueSetPtr&);
  TypePtr MakeValueSetType();
  void PrintOn(ostream & ) const;

  TypePtr GetType() ;
  ConstraintElementVector* GetElements() ;
protected:
  ValueSetPtr GetRepresentation();

  InformationObjectPtr object;
  string field;
  ValueSetPtr rep;
};

typedef boost::shared_ptr<ObjectSetConstraintElement> ObjectSetConstraintElementPtr;

class ValueSetFromObjects : public ValueSet
{
public:
  ValueSetFromObjects(ObjectSetConstraintElementPtr objSet,
    const string& fld);
  ~ValueSetFromObjects();
  void Union(ValueSetPtr&);
  void Intersect(ValueSetPtr&);
  TypePtr MakeValueSetType();
  void PrintOn(ostream & ) const;

  TypePtr GetType();
  ConstraintElementVector* GetElements();
protected:
  ValueSetPtr GetRepresentation();

  ObjectSetConstraintElementPtr objectSet;
  string field;
  ValueSetPtr rep;
};

// object class
class FieldSetting;

typedef boost::shared_ptr<FieldSetting> FieldSettingPtr;
typedef vector<FieldSettingPtr> FieldSettingList;

class FieldSpec;
//PLIST(FieldSpecsList, FieldSpec);
typedef boost::shared_ptr<FieldSpec> FieldSpecPtr;
typedef vector<FieldSpecPtr> FieldSpecsList;

class FieldSpec : public Printable
{
public:
  FieldSpec(const string& nam, bool optional = false);
  virtual ~FieldSpec();

  bool IsOptional() const { return isOptional; }
  virtual bool HasDefault() const =0;
  const string& GetName() const { return name; }
  const string& GetIdentifier() const { return identifier;}
  virtual string GetField() const = 0;

  virtual void EstablishFieldRelation(FieldSpecsList* ){}
  virtual void BeginParseSetting(FieldSettingList* ) const {}
  virtual void EndParseSetting() const {}

  virtual int GetToken() const = 0;
  void PrintOn(ostream &) const;

  virtual void ResolveReference() const {};

  // used only for FixedTypeValueField and FixedTypeValueSetField
  virtual TypeBase* GetFieldType() { return NULL;}
  virtual const TypeBase* GetFieldType() const { return NULL; }

  //

  virtual bool GetKey(TypePtr& , string& ) { return false;}
  virtual void FwdDeclare(ostream& ) const {}
  virtual void Generate_info_type_constructor(ostream&) const {}
  virtual void Generate_info_type_memfun(ostream& ) const {}
  virtual void Generate_info_type_mem(ostream& ) const {}

  virtual void Generate_value_type(ostream& ) const {}
  virtual void GenerateTypeField(const string& templatePrefix,
    const string& classNameString,
    const TypeBase* keyType,
    const string& objClassName,
    ostream& hdr, ostream& cxx, ostream& inl) const;

protected:

  string name;
  string identifier;
  bool isOptional;
};

class TypeFieldSpec : public FieldSpec
{
public:
  TypeFieldSpec(const string& nam, bool optional = false, TypePtr defaultType= TypePtr());
  ~TypeFieldSpec();
  virtual bool HasDefault() const;
  string GetField() const;
  string GetDefault() const ;
  TypePtr GetDefaultType();
  virtual int GetToken() const;
  void PrintOn(ostream &) const;
  virtual void ResolveReference() const;
  virtual void Generate_info_type_constructor(ostream&) const;
  virtual void Generate_info_type_memfun(ostream& hdr) const;
  virtual void Generate_info_type_mem(ostream& ) const;
  virtual void Generate_value_type(ostream& hdr) const;
  virtual void GenerateTypeField(const string& templatePrefix,
    const string& classNameString,
    const TypeBase* keyType,
    const string& objClassName,
    ostream& hdr, ostream& cxx, ostream& inl) const;
protected:
  TypePtr type;
};

class FixedTypeValueFieldSpec : public FieldSpec
{
public:
  FixedTypeValueFieldSpec(const string& nam, TypePtr t, bool optional = false,
    bool unique = false);
  ~FixedTypeValueFieldSpec();

  virtual bool HasDefault() const;
  void SetDefault(ValuePtr value);
  string GetField() const;
  virtual void BeginParseSetting(FieldSettingList*)  const ;
  virtual void EndParseSetting() const ;
  virtual int GetToken() const;

  void PrintOn(ostream &) const;
  virtual void ResolveReference() const;

  virtual bool GetKey(TypePtr& keyType, string& keyName);
  virtual TypeBase* GetFieldType() ;
  virtual const TypeBase* GetFieldType() const;

protected:
  bool isUnique;
  TypePtr  type;
  ValuePtr defaultValue;
};

class FixedTypeValueSetFieldSpec : public FieldSpec
{
public:
  FixedTypeValueSetFieldSpec(const string& nam, TypePtr t, bool optional = false);
  ~FixedTypeValueSetFieldSpec();

  virtual bool HasDefault() const;
  void SetDefault(ValueSetPtr valueSet){ defaultValueSet = valueSet; }
  string GetField() const { return type->GetName();}
  virtual void BeginParseSetting(FieldSettingList*) const ;
  virtual void EndParseSetting() const ;
  virtual int GetToken() const;

  void PrintOn(ostream &) const;
  virtual void ResolveReference() const;

  virtual TypeBase* GetFieldType() ;
  virtual const TypeBase* GetFieldType() const ;

protected:
  TypePtr  type;
  ValueSetPtr defaultValueSet;
};

class VariableTypeValueFieldSpec : public FieldSpec
{
public:
  VariableTypeValueFieldSpec(const string& nam,
    const string& fieldname,
    bool optional = false);
  ~VariableTypeValueFieldSpec();

  virtual bool HasDefault() const;
  string GetField() const { return fieldName;}
  void SetDefault(ValuePtr value){ defaultValue = value;}
  virtual void EstablishFieldRelation(FieldSpecsList* specs);

  virtual void BeginParseSetting(FieldSettingList*) const ;
  virtual void EndParseSetting() const ;
  virtual int GetToken() const;

  void PrintOn(ostream &) const;
  virtual void ResolveReference() const;

protected:
  string fieldName;
  ValuePtr defaultValue;
  TypePtr defaultType;
};

class VariableTypeValueSetFieldSpec : public FieldSpec
{
public:
  VariableTypeValueSetFieldSpec(const string& nam,
    const string& fieldname,
    bool optional = false);
  ~VariableTypeValueSetFieldSpec();

  virtual bool HasDefault() const;
  string GetField() const { return fieldName;}
  void SetDefault(ValueSetPtr valueSet){ defaultValueSet = valueSet; }
  virtual void EstablishFieldRelation(FieldSpecsList* specs);

  virtual void BeginParseSetting(FieldSettingList*) const ;
  virtual void EndParseSetting() const ;
  virtual int GetToken() const;

  void PrintOn(ostream &) const;
  virtual void ResolveReference() const;

protected:
  string fieldName;
  ValueSetPtr defaultValueSet;
};

class DefinedObjectClass;
typedef boost::shared_ptr<DefinedObjectClass> DefinedObjectClassPtr;
class ObjectFieldSpec : public FieldSpec
{
public:
  ObjectFieldSpec(const string& nam,
    DefinedObjectClass* oclass,
    bool optional = false);
  ~ObjectFieldSpec() ;

  virtual bool HasDefault() const;
  string GetField() const;
  void SetDefault(InformationObjectPtr dftObj);
  virtual void BeginParseSetting(FieldSettingList*) const ;
  virtual void EndParseSetting() const ;
  virtual int GetToken() const;

  void PrintOn(ostream &) const;
  virtual void ResolveReference() const;
protected:
  DefinedObjectClass* objectClass;
  InformationObjectPtr obj;
};

class ObjectSetFieldSpec : public FieldSpec
{
public:
  ObjectSetFieldSpec(const string& nam,
        DefinedObjectClassPtr oclass,
                                bool optional = false);
  ~ObjectSetFieldSpec();

  virtual bool HasDefault() const;
  void SetDefault(ConstraintPtr dftObjSet){ objSet = dftObjSet;}
  string GetField() const;

  virtual void BeginParseSetting(FieldSettingList*) const ;
  virtual void EndParseSetting() const ;
  virtual int GetToken() const;

  void PrintOn(ostream &) const;
  virtual void ResolveReference() const;
  virtual void FwdDeclare(ostream& hdr) const ;
  virtual void Generate_info_type_constructor(ostream& ) const;
  virtual void Generate_info_type_memfun(ostream& hdr) const ;
  virtual void Generate_info_type_mem(ostream& ) const;
  virtual void Generate_value_type(ostream& hdr) const ;
  virtual void GenerateTypeField(const string& templatePrefix,
    const string& classNameString,
    const TypeBase* keyType,
    const string& objClassName,
    ostream& hdr,
    ostream& cxx,
    ostream& inl) const;
protected:
  DefinedObjectClassPtr objectClass;
  ConstraintPtr objSet;
};


class DefinedSyntaxToken;

class TokenOrGroupSpec : public Printable
{
public:
  virtual ~TokenOrGroupSpec(){};
  virtual bool ValidateField(FieldSpecsList* )=0;
  virtual bool HasLiteral(const string& str) const = 0;
  enum MakeDefaultSyntaxResult
  {
    FAIL, // indicate the make process fail
      CONSUMED_AND_EXHAUSTED,
      CONSUMED_AND_NOT_EXHAUSTED,
      NOT_CONSUMED
  };
  virtual MakeDefaultSyntaxResult  MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* setting)=0;
  virtual void PreMakeDefaultSyntax(FieldSettingList* settings)=0;
  virtual void CancelMakeDefaultSyntax() const=0;
  virtual void Reset(){}
};

class Literal : public TokenOrGroupSpec
{
public:
  Literal(const string& str)
    : name(str ){ }
  Literal (const char* str): name(str){}
  virtual ~Literal(){};
  void PrintOn(ostream & strm) const ;
  virtual bool ValidateField(FieldSpecsList* ){ return true; }
  virtual MakeDefaultSyntaxResult  MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* setting);
  virtual bool HasLiteral(const string& str) const { return str == name; }
  virtual void PreMakeDefaultSyntax(FieldSettingList*){};
  virtual void CancelMakeDefaultSyntax() const {}

protected:
  string name;
};

class PrimitiveFieldName : public TokenOrGroupSpec
{
public:
  PrimitiveFieldName(const string& fieldname)
    : name(fieldname)
  {}
  PrimitiveFieldName(const char* fieldname) : name(fieldname){}
  PrimitiveFieldName(const PrimitiveFieldName& other){ name = other.name; field = other.field;}
  ~PrimitiveFieldName(){};
  void PrintOn(ostream &) const ;
  virtual bool ValidateField(FieldSpecsList* fields);
  virtual MakeDefaultSyntaxResult  MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* setting) ;
  virtual bool HasLiteral(const string&) const { return false; }
  virtual void PreMakeDefaultSyntax(FieldSettingList* settings);
  virtual void CancelMakeDefaultSyntax() const;
private:
  string name;
  FieldSpec* field;
};

typedef boost::shared_ptr<TokenOrGroupSpec> TokenOrGroupSpecPtr;
typedef vector<TokenOrGroupSpecPtr> TokenOrGroupSpecList;

class TokenGroup : public TokenOrGroupSpec
{
public:
  TokenGroup() : optional(false), cursor(0){}
  TokenGroup(const TokenGroup& other);
  ~TokenGroup(){}
  void AddToken(TokenOrGroupSpecPtr token){ tokenOrGroupSpecList.push_back(token);}
  void SetOptional(){ optional = true;}
  void PrintOn(ostream &) const;
  bool ValidateField(FieldSpecsList* fields);
  MakeDefaultSyntaxResult  MakeDefaultSyntax(DefinedSyntaxToken* token,
    FieldSettingList* setting);
  virtual bool HasLiteral(const string& str) const;
  virtual void PreMakeDefaultSyntax(FieldSettingList* settings);
  virtual void CancelMakeDefaultSyntax() const;

  size_t size() const { return tokenOrGroupSpecList.size(); }
  TokenOrGroupSpec& operator[](size_t i){ return *(tokenOrGroupSpecList[i]);}
  virtual void Reset();
private:
  TokenOrGroupSpecList tokenOrGroupSpecList;
  bool optional;
  size_t cursor;
};

typedef boost::shared_ptr<TokenGroup> TokenGroupPtr;

class DefaultSyntaxBuilder
{
public:
  DefaultSyntaxBuilder(TokenGroupPtr tkGrp);
  ~DefaultSyntaxBuilder();
  void AddToken(DefinedSyntaxToken* token);
  auto_ptr<FieldSettingList> GetDefaultSyntax();
  void ResetTokenGroup();
private:
  TokenGroupPtr tokenGroup;
  auto_ptr<FieldSettingList> setting;
};

class ObjectClassBase : public Printable
{
public:
  ObjectClassBase(){};
  ObjectClassBase(const string& nam){ SetName(nam); }
  virtual ~ObjectClassBase(){};

  void SetName(const string& nam);
  virtual const string& GetName() const { return name;}
  int GetFieldToken(const char* fieldname) const;
  virtual FieldSpec* GetField(const string& fieldName) =0;
  virtual const FieldSpec* GetField(const string& fieldName) const=0;
  virtual TypeBase* GetFieldType(const string& fieldName) { return GetField(fieldName)->GetFieldType(); }
  virtual const TypeBase* GetFieldType(const string& fieldName) const { return GetField(fieldName)->GetFieldType(); }
  virtual bool VerifyDefaultSyntax(FieldSettingList*) const = 0;
  virtual bool HasLiteral(const string& str) const = 0;
  virtual TokenGroupPtr GetWithSyntax() const = 0;
  virtual void PreParseObject() const = 0;
  virtual void BeginParseObject() const = 0;
  virtual void EndParseObject() const =0 ;
  virtual void BeginParseObjectSet() const = 0;
  virtual void EndParseObjectSet() const = 0;
  virtual void ResolveReference() const =0;
  virtual void GenerateCplusplus(ostream& , ostream& , ostream& ){}
  virtual const string& GetKeyName() const =0;
protected:
  string name;
};

typedef vector<ObjectClassBasePtr> ObjectClassesList;
class ObjectClassDefn : public ObjectClassBase
{
public:
  ObjectClassDefn();

  ~ObjectClassDefn();

  void SetFieldSpecs(auto_ptr<FieldSpecsList> list);
  void SetWithSyntaxSpec(TokenGroupPtr list);

  FieldSpec* GetField(const string& fieldName);
  const FieldSpec* GetField(const string& fieldName) const;

  virtual bool VerifyDefaultSyntax(FieldSettingList*) const;
  virtual bool HasLiteral(const string& str) const { return withSyntaxSpec->HasLiteral(str);}
  virtual TokenGroupPtr GetWithSyntax() const;
  virtual void PreParseObject() const ;
  virtual void BeginParseObject() const;
  virtual void EndParseObject() const;
  virtual void BeginParseObjectSet() const;
  virtual void EndParseObjectSet() const;
  void PrintOn(ostream &) const;
  virtual void ResolveReference() const;

  void ResolveKey();
  const string& GetKeyName() const { return keyName; }
  void GenerateCplusplus(ostream& hdr, ostream& cxx, ostream& inl);
protected:
  auto_ptr<FieldSpecsList> fieldSpecs;
  TokenGroupPtr withSyntaxSpec;
  TypePtr keyType;
  string keyName;
};


class DefinedObjectClass : public ObjectClassBase
{
public:
  DefinedObjectClass(ObjectClassBase* ref);
  DefinedObjectClass(const string& nam, ObjectClassBase* ref = NULL);
  ~DefinedObjectClass(){}

  const string& GetName() const { return referenceName;}
  ObjectClassBase* GetReference();
  const ObjectClassBase* GetReference() const;
  FieldSpec* GetField(const string& fieldName);
  const FieldSpec* GetField(const string& fieldName) const;
  virtual bool VerifyDefaultSyntax(FieldSettingList*) const;
  bool HasLiteral(const string& str) const;
  virtual TokenGroupPtr GetWithSyntax() const;
  virtual void PreParseObject() const ;
  virtual void BeginParseObject() const;
  virtual void EndParseObject() const;
  virtual void BeginParseObjectSet() const;
  virtual void EndParseObjectSet() const;
  void PrintOn(ostream& strm) const;
  virtual void ResolveReference() const;
  virtual TypeBase* GetFieldType(const string& fieldName) ;
  virtual const TypeBase* GetFieldType(const string& fieldName) const;
  const string& GetKeyName() const { return reference->GetKeyName(); }
protected:
  string referenceName;
  mutable ObjectClassBase* reference;
};

class ImportedObjectClass : public DefinedObjectClass
{
public:
  ImportedObjectClass(const string& modName,
    const string& nam,
    ObjectClassBase* ref)
    : DefinedObjectClass(nam, ref), moduleName(modName){}
  const string& GetModuleName() const { return moduleName;}
private:
  string moduleName;
};

class Setting : public Printable
{
public:

  enum
  {
    has_type_setting = 0x01,
      has_value_setting = 0x02,
      has_valueSet_setting = 0x04,
      has_object_setting = 0x08,
      has_objectSet_setting = 0x10
  };
  virtual ~Setting(){};
  virtual void GenerateInfo(const string& , ostream&){}
  virtual void GenerateCplusplus(const string&, const string&, ostream& , ostream& , ostream& , unsigned& flag) =0;
  virtual void GenerateInitializationList(ostream& , ostream& , ostream& ){}
  virtual bool IsExtendable() const { return false;}
  virtual void GenerateInstanceCode(const string& , ostream& ) const{}
};

class TypeSetting : public Setting
{
public:
  TypeSetting(TypePtr typeBase){ type = typeBase; }
  ~TypeSetting(){}

  TypePtr GetType() { return type;}
  const TypeBase* GetType() const { return type.get();}
  void PrintOn(ostream & strm) const;
  virtual void GenerateCplusplus(const string& prefix, const string& name, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
  virtual void GenerateInfo(const string& name,ostream& hdr);
protected:
  TypePtr type;
};


class ValueSetting : public Setting
{
public:
  ValueSetting(TypePtr typeBase, ValuePtr valueBase);
  ~ValueSetting();

  ValuePtr GetValue() { return value;}
  const ValuePtr GetValue() const { return value;}
  TypePtr GetType() { return type;}
  const TypeBase* GetType() const { return type.get();}
  void PrintOn(ostream & strm) const;
  virtual void GenerateCplusplus(const string& prefix, const string& name, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
  virtual void GenerateInitializationList(ostream&, ostream&, ostream&);
protected:
  TypePtr  type;
  ValuePtr value;
};

class ValueSetSetting : public Setting
{
public:
  ValueSetSetting(ValueSetPtr set);
  ~ValueSetSetting();

  ValueSetPtr GetValueSet() { return valueSet; }
  const ValueSetPtr GetValueSet() const { return valueSet; }
  void PrintOn(ostream & strm) const;
  virtual void GenerateCplusplus(const string& prefix, const string& name, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
protected:
  ValueSetPtr valueSet;
};

class InformationObject;
typedef boost::shared_ptr<InformationObject> InformationObjectPtr;
class ObjectSetting : public Setting
{
public:
  ObjectSetting(InformationObjectPtr obj, ObjectClassBase* objClass);
  ~ObjectSetting();
  InformationObjectPtr GetObject() { return object; }
  const InformationObject* GetObject() const { return object.get(); }
  void PrintOn(ostream & strm) const ;
  virtual void GenerateCplusplus(const string& prefix, const string& name, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
protected:
  ObjectClassBase* objectClass;
  InformationObjectPtr object;
};

class ObjectSetSetting : public Setting
{
public:
  ObjectSetSetting(ConstraintPtr objSet, ObjectClassBase* objClass)
    : objectClass(objClass), objectSet(objSet) { }
  ~ObjectSetSetting();
  ConstraintPtr GetObjectSet() { return objectSet;}
  const ConstraintPtr GetObjectSet() const { return objectSet; }
  void PrintOn(ostream &) const ;
  virtual void GenerateCplusplus(const string& prefix, const string& name, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
  virtual void GenerateInfo(const string& name,ostream& hdr);
  virtual bool IsExtendable() const { return objectSet->IsExtendable(); }
  void GenerateInstanceCode(const string& prefix, ostream& cxx) const;
protected:
  ObjectClassBase* objectClass;
  ConstraintPtr objectSet;
};

class FieldSetting : public Printable
{
public:
  FieldSetting(const string& fieldname, auto_ptr<Setting> aSetting);
  ~FieldSetting();

  const string& GetName() const { return name;}
  Setting* GetSetting() { return setting.get();}
  const Setting* GetSetting() const { return setting.get(); }
  void PrintOn(ostream &) const;

  bool IsExtendable() const;
  void GenerateCplusplus(const string& prefix, ostream & hdr, ostream & cxx, ostream & inl, unsigned& flag);
  void GenerateInitializationList(ostream & hdr, ostream & cxx, ostream & inl);
  void GenerateInfo(ostream& hdr);
  void GenerateInstanceCode(const string& prefix, ostream& cxx) const;
protected:
  string name;
  string identifier;
   auto_ptr<Setting> setting;
};


class InformationObject : public Printable
{
public:
  InformationObject() : parameters(NULL){}
  virtual ~InformationObject();
  void SetName(const string& str){ name = str; }
  const string& GetName() const { return name; }
  const string& GetClassName() const;
  void PrintBase(ostream &) const;
  virtual bool SetObjectClass(const ObjectClassBase* definedClass) = 0;
  virtual const ObjectClassBase* GetObjectClass() const = 0;
  virtual const Setting* GetSetting(const string& fieldname) const = 0;
  void SetParameters(auto_ptr<ParameterList> list);
  virtual void GenerateCplusplus(ostream&, ostream &, ostream &) {}
  virtual bool IsExtendable() const =0;
  virtual void GenerateInstanceCode(ostream& cxx) const =0;
protected:
  virtual bool VerifyObjectDefinition() = 0 ;
  string name;
  auto_ptr<ParameterList> parameters;
};

typedef vector<InformationObjectPtr> InformationObjectList;

class DefinedObject : public InformationObject
{
public:
  DefinedObject(const string& name, const InformationObject* ref=NULL);
  bool VerifyObjectDefinition();
  const InformationObject* GetReference() const ;
  bool SetObjectClass(const ObjectClassBase* ){ return true;}
  const ObjectClassBase* GetObjectClass() const ;
  void PrintOn(ostream &) const;
  virtual const Setting* GetSetting(const string& fieldname) const;
  virtual bool IsExtendable() const { return reference->IsExtendable(); }
  virtual void GenerateInstanceCode(ostream& cxx) const;
protected:
  string referenceName;
  mutable const InformationObject* reference;
};

class ImportedObject : public DefinedObject
{
public:
  ImportedObject(const string& modName, const string& name, const InformationObject* ref)
    : DefinedObject(name, ref), moduleName(modName) {}
  const string& GetModuleName() const { return moduleName; }
  virtual void GenerateInstanceCode(ostream& cxx) const;
private:
  string moduleName;
};

class DefaultObjectDefn : public InformationObject
{
public:
  DefaultObjectDefn(auto_ptr<FieldSettingList> list) : settings(list){}
  ~DefaultObjectDefn(){ }
  bool VerifyObjectDefinition();

  bool SetObjectClass(const ObjectClassBase* definedClass);
  const ObjectClassBase* GetObjectClass() const { return referenceClass;}

  void PrintOn(ostream &) const;
  virtual const Setting* GetSetting(const string& fieldname) const;
  virtual void GenerateCplusplus(ostream& hdr , ostream & cxx, ostream & inl);
  virtual bool IsExtendable() const ;
  virtual void GenerateInstanceCode(ostream& cxx) const;
protected:
  auto_ptr<FieldSettingList> settings;
  const ObjectClassBase* referenceClass;
};

class ObjectFromObject : public InformationObject
{
public:
  ObjectFromObject(InformationObjectPtr referenceObj, const string& fld);
  ~ObjectFromObject();
  void PrintOn(ostream &) const;
  virtual bool SetObjectClass(const ObjectClassBase* ){return true;}
  virtual const ObjectClassBase* GetObjectClass() const ;
  virtual const Setting* GetSetting(const string& fieldname) const;
  virtual bool IsExtendable() const { return refObj->IsExtendable(); }
  virtual void GenerateInstanceCode(ostream& cxx) const;
protected:
  virtual bool VerifyObjectDefinition();
  InformationObjectPtr refObj;
  string field;
};

class DefinedSyntaxToken
{
public:
  virtual ~DefinedSyntaxToken(){};
  virtual bool MatchLiteral(const string&) { return false;}
  virtual FieldSettingPtr MatchSetting(const string&) { return FieldSettingPtr();}
  virtual bool IsEndSyntaxToken() { return false;}
};

class LiteralToken : public DefinedSyntaxToken
{
public:
  LiteralToken(const string& tokenName) : name(tokenName){ }
  virtual bool MatchLiteral(const string& literal) { return literal == name;}
protected:
  string name;
};

class SettingToken : public DefinedSyntaxToken
{
public:
  SettingToken(auto_ptr<Setting> set) : setting(set) {}
  ~SettingToken() { }
  virtual FieldSettingPtr MatchSetting(const string&) ;
protected:
  auto_ptr<Setting> setting;
};

class EndSyntaxToken : public DefinedSyntaxToken
{
public:
  EndSyntaxToken(){}
  virtual bool IsEndSyntaxToken() { return true;}
};



class InformationObjectSet : public Printable
{
public:
  virtual ~InformationObjectSet(){}

  virtual const string& GetName() const = 0 ;
  virtual const ObjectClassBase* GetObjectClass() const = 0 ;
  virtual bool IsExtendable() const = 0;

  virtual ValueSetPtr GetValueSetFromValueField(const string& field) const =0;
  virtual ValueSetPtr GetValueSetFromValueSetField(const string& field) const=0;
  virtual ConstraintPtr GetObjectSetFromObjectField(const string& field) const=0;
  virtual ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const=0;
  virtual bool HasParameters() const =0;
  virtual void GenerateInstanceCode(ostream& cxx) const =0;
  virtual void GenerateType(ostream& hdr, ostream& cxx, ostream& inl) const=0;
};

class InformationObjectSetDefn : public InformationObjectSet
{
public:
  InformationObjectSetDefn(const string& nam,
    ObjectClassBasePtr objClass,
    ConstraintPtr set,
    ParameterListPtr list = ParameterListPtr() );
  ~InformationObjectSetDefn();

  const string& GetName() const { return name;}
  const ObjectClassBase* GetObjectClass() const ;

  virtual bool IsExtendable() const { return rep->IsExtendable(); }
  ValueSetPtr GetValueSetFromValueField(const string& field) const;
  ValueSetPtr GetValueSetFromValueSetField(const string& field) const;
  ConstraintPtr GetObjectSetFromObjectField(const string& field) const;
  ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const;
  bool HasParameters() const { return parameters.get() != NULL; }
  void GenerateInstanceCode(ostream& cxx) const;
  void PrintOn(ostream &) const;
  virtual void GenerateType(ostream& hdr, ostream& cxx, ostream& inl) const;
  bool GenerateTypeConstructor(ostream& cxx) const;
protected:
  string name;
  ObjectClassBasePtr objectClass;
  ConstraintPtr rep;
  ParameterListPtr parameters;
};

class ImportedObjectSet :  public InformationObjectSet
{
public:
  ImportedObjectSet(const string& modName, const InformationObjectSet* objSet) : reference(objSet), moduleName(modName){}
  virtual const string& GetName() const { return reference->GetName(); }
  virtual const ObjectClassBase* GetObjectClass() const { return reference->GetObjectClass(); }
  const string& GetModuleName() const { return moduleName; }

  virtual bool IsExtendable() const { return reference->IsExtendable(); }
  virtual ValueSetPtr GetValueSetFromValueField(const string& field) const { return reference->GetValueSetFromValueField(field); }
  virtual ValueSetPtr GetValueSetFromValueSetField(const string& field) const { return reference->GetValueSetFromValueSetField(field); }
  virtual ConstraintPtr GetObjectSetFromObjectField(const string& field) const { return reference->GetObjectSetFromObjectField(field); }
  virtual ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const { return reference->GetObjectSetFromObjectSetField(field); }
  virtual bool HasParameters() const { return reference->HasParameters(); }
  void GenerateInstanceCode(ostream& ) const{}
  virtual void GenerateType(ostream& , ostream&, ostream&) const {}
  void PrintOn(ostream &) const {}
private:
  const InformationObjectSet* reference;
  string moduleName;
};

typedef boost::shared_ptr<InformationObjectSet> InformationObjectSetPtr;
typedef vector<InformationObjectSetPtr> InformationObjectSetList;
class ObjectSetConstraintElement : public ConstraintElementBase
{
public:
  virtual const ObjectClassBase* GetObjectClass() const = 0 ;
  virtual string GetName() const = 0;
  virtual void GenerateObjSetAccessCode(ostream& ) {}
};

class DefinedObjectSet : public ObjectSetConstraintElement
{
public:
  DefinedObjectSet(const string& ref) ;

  virtual const ObjectClassBase* GetObjectClass() const ;
  virtual string GetName() const { return referenceName;}

  virtual ValueSetPtr GetValueSetFromValueField(const string& field) const;
  virtual ValueSetPtr GetValueSetFromValueSetField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const;
  virtual bool HasPERInvisibleConstraint(const Parameter& param) const;

  virtual const InformationObjectSet* GetReference() const;

  void PrintOn(ostream &) const;
protected:
  string referenceName;
  mutable const InformationObjectSet* reference;
};

class ParameterizedObjectSet : public DefinedObjectSet
{
public:
  ParameterizedObjectSet(const string& ref, ActualParameterListPtr args) ;
  ~ParameterizedObjectSet();
  virtual string GetName() const ;

  void PrintOn(ostream &) const;
protected:
  ActualParameterListPtr arguments;
};


class ObjectSetFromObject : public ObjectSetConstraintElement
{
public:
  ObjectSetFromObject(InformationObjectPtr obj, const string& fld);
  ~ObjectSetFromObject();

  virtual const ObjectClassBase* GetObjectClass() const ;
  virtual string GetName() const;

  virtual ValueSetPtr GetValueSetFromValueField(const string& field) const;
  virtual ValueSetPtr GetValueSetFromValueSetField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const;
  virtual bool HasPERInvisibleConstraint(const Parameter& param) const;

  void PrintOn(ostream &) const;
protected:
  Constraint* GetRepresentation() const;
  InformationObjectPtr refObj;
  string field;
  mutable ConstraintPtr rep;
};

class ObjectSetFromObjects : public ObjectSetConstraintElement
{
public:
  ObjectSetFromObjects(ObjectSetConstraintElementPtr objSet, const string& fld);
  ~ObjectSetFromObjects();

  virtual const ObjectClassBase* GetObjectClass() const ;
  virtual string GetName() const;

  virtual ValueSetPtr GetValueSetFromValueField(const string& field) const;
  virtual ValueSetPtr GetValueSetFromValueSetField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const;

  virtual bool HasPERInvisibleConstraint(const Parameter& param) const;
  void PrintOn(ostream &) const;
  virtual void GenerateObjSetAccessCode(ostream& );
protected:
  ConstraintPtr GetRepresentation() const;
  ObjectSetConstraintElementPtr refObjSet;
  string field;
  mutable ConstraintPtr rep;
};

class SingleObjectConstraintElement : public ConstraintElementBase
{
public:
  SingleObjectConstraintElement(InformationObjectPtr obj);
  ~SingleObjectConstraintElement();

  virtual ValueSetPtr GetValueSetFromValueField(const string& field) const;
  virtual ValueSetPtr GetValueSetFromValueSetField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectField(const string& field) const;
  virtual ConstraintPtr GetObjectSetFromObjectSetField(const string& field) const;
  virtual bool HasPERInvisibleConstraint(const Parameter& param) const;
  virtual void GenerateObjectSetInstanceCode(const string& prefix, ostream& cxx) const;
  void PrintOn(ostream &) const;
protected:
  InformationObjectPtr object;
};

class ObjectSetType : public TypeBase
{
public:
  ObjectSetType(InformationObjectSetPtr objSet);
  virtual const char * GetAncestorClass() const;
  virtual void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  virtual bool HasParameters() const;
private:
  InformationObjectSetPtr objSet;
};

// ImportedSymbol
class ModuleDefinition;
class Symbol : public Printable
{
public:
  Symbol(const string& sym, bool param);
  bool IsParameterisedImport() const { return parameterized;}
  virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to)=0;
  virtual void AppendToModule(const string& , ModuleDefinition* ){}
  virtual void GenerateUsingDirective(const string& , ostream & ) const {}
  virtual bool IsType() const { return false; }
  virtual bool IsValuesOrObjects() const { return false;}
  const string& GetName() const { return name; }

  void PrintOn(ostream& strm) const;
protected:
  string name;
  bool parameterized;
};

class TypeReference : public Symbol
{
public:
  TypeReference(const string& sym, bool param) : Symbol(sym,param){}
  virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
  virtual void AppendToModule(const string& fromName, ModuleDefinition* to);
  virtual void GenerateUsingDirective(const string& moduleName, ostream & strm) const;
  virtual bool IsType() const { return true; }
};

class ValueReference : public Symbol
{
public:
  ValueReference(const string& sym, bool param) : Symbol(sym,param){}
  virtual bool IsValuesOrObjects() const { return true;}
  virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
};

class ObjectClassReference : public Symbol
{
public:
  ObjectClassReference(const string& sym, bool param) : Symbol(sym,param){}
  virtual void GenerateUsingDirective(const string& moduleName, ostream & strm) const ;
  virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
};

class ObjectReference : public Symbol
{
public:
  ObjectReference(const string& sym, bool param) : Symbol(sym,param){}
  virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
  virtual bool IsValuesOrObjects() const { return true;}
};

class ObjectSetReference : public Symbol
{
public:
  ObjectSetReference(const string& sym, bool param) : Symbol(sym,param){}
  virtual void GenerateUsingDirective(const string& moduleName, ostream & strm) const ;
  virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
  virtual bool IsValuesOrObjects() const { return true;}
};
// ActualParameter

class ActualParameter : public Printable
{
public:
  virtual bool ReferencesType(const TypeBase & ) const { return false;}
  virtual bool UseType(const TypeBase & ) const { return false;}
  virtual bool GenerateTemplateArgument(string& ) const { return false;}
};

class ActualTypeParameter : public ActualParameter
{
public:
  ActualTypeParameter(TypePtr type);
  ~ActualTypeParameter();

  bool ReferencesType(const TypeBase & type) const;
  bool UseType(const TypeBase & ) const;
  bool GenerateTemplateArgument(string& name) const;
  void PrintOn(ostream & strm) const;
protected:
  TypePtr param;
};

class ActualValueParameter : public ActualParameter
{
public:
  ActualValueParameter(ValuePtr value);
  ~ActualValueParameter();

  void PrintOn(ostream & strm) const;
protected:
  ValuePtr param;
};

class ActualValueSetParameter : public ActualParameter
{
public:
  ActualValueSetParameter(TypePtr valueSetType);
  ~ActualValueSetParameter();

  bool ReferencesType(const TypeBase & type) const;
  bool UseType(const TypeBase & ) const;
  bool GenerateTemplateArgument(string& ) const;

  void PrintOn(ostream & strm) const;
protected:
  TypePtr param;
};

class ActualObjectParameter : public ActualParameter
{
public:
  ActualObjectParameter(InformationObjectPtr obj);
  ~ActualObjectParameter();
  bool GenerateTemplateArgument(string& name) const;
  virtual bool UseType(const TypeBase & ) const ;
  virtual bool ReferencesType(const TypeBase & type) const;

  void PrintOn(ostream & strm) const;
protected:
  InformationObjectPtr param;
};

class ActualObjectSetParameter : public ActualParameter
{
public:
  ActualObjectSetParameter(boost::shared_ptr<ObjectSetConstraintElement> objectSet);
  ~ActualObjectSetParameter();
  bool GenerateTemplateArgument(string& name) const;
  virtual bool UseType(const TypeBase & ) const ;
  virtual bool ReferencesType(const TypeBase & type) const;

  void PrintOn(ostream & strm) const;
  virtual bool IsTemplateArgument() const { return true;}
protected:
  boost::shared_ptr<ObjectSetConstraintElement> param;
};



typedef boost::shared_ptr<Symbol> SymbolPtr;
typedef vector<SymbolPtr> SymbolList;

class ImportModule : public Printable
{
  //PCLASSINFO(ImportModule, PObject);
public:
  ImportModule(string * name, SymbolList * syms);
  void SetFileName(const string& name) { filename = name; }
  const string& GetFileName() const { return filename;}

  void PrintOn(ostream &) const;

  void GenerateCplusplus(ostream & hdr, ostream & cxx, ostream & inl);
  void GenerateUsingDirectives(ostream & strm) const;
  void Adjust();

  const string& GetName() const { return fullModuleName; }
  bool HasValuesOrObjects() const;
  string GetCModuleName() const;
  const string& GetLowerCaseName() const { return filename; }

protected:
  string   fullModuleName;
  string   shortModuleName;
  SymbolList symbols;
  string   filename;
};

typedef boost::shared_ptr<ImportModule> ImportModulePtr;
typedef vector<ImportModulePtr> ImportsList;
typedef boost::shared_ptr<ModuleDefinition> ModuleDefinitionPtr;
typedef vector<ModuleDefinitionPtr> ModuleList;


typedef map<string, TypePtr> TypeMap;

class ModuleDefinition : public Printable
{
public:
  ModuleDefinition(const string& name, Tag::Mode defTagMode);
  ~ModuleDefinition();

  void PrintOn(ostream &) const;

  Tag::Mode GetDefaultTagMode() const { return defaultTagMode; }

  void AddIdentifier(string* name, int idType);
  void AddImportedIdentifiers(StringList& imports, const string& moduleName);
  int  GetIdentifierType(const string& id);

  void SetDefinitiveObjId(StringList& id);
  void SetExportAll();
  void SetExports(SymbolList& syms);

  void AddImport(ImportModulePtr mod)  { imports.push_back(mod); }
  void AddType(TypePtr type)       ;
  void AddValue(ValuePtr val)      { values.push_back(val); }
  void AddObjectClass(ObjectClassBasePtr oclass) { objectClasses.push_back(oclass); }
  void AddInformationObject(InformationObjectPtr obj) { informationObjects.push_back(obj); }
  void AddInformationObjectSet(InformationObjectSetPtr set) { informationObjectSets.push_back(set); }

  TypePtr FindType(const string & name);
  bool  HasType(const string & name);
  const ValuesList & GetValues() const { return values; }

  ValuePtr FindValue(const string& name);
  ObjectClassBasePtr FindObjectClass(const string & name);
  const InformationObject* FindInformationObject(const string & name);
  const InformationObjectSet* FindInformationObjectSet(const string & name);

  const string & GetName() const { return moduleName; }
  const string & GetCModuleName() const { return cModuleName; }
  const string & GetPrefix()     const { return classNamePrefix; }

  string GetImportModuleName(const string & moduleName);

  int GetIndentLevel() const { return indentLevel; }
  void SetIndentLevel(int delta) { indentLevel += delta; }

  bool UsingInlines() const { return usingInlines; }

  virtual void GenerateCplusplus(const string & modName,
    unsigned numFiles,
    bool verbose);

  void ResolveObjectClassReferences() const;

  void AdjustModuleName(const string & sourcePath, bool isSubModule = false);
  bool ReorderTypes();
  string CreateSubModules(SymbolList& exportedSymbols);
  string GetFileName();
  void AdjustImportedModules();


  bool IsExported(const string& name) const;

  void GenerateClassModule(ostream& hdr, ostream& cxx, ostream& inl);
  void CreateObjectSetTypes();

  void AddToRemoveList(const string& reference);
  void RemoveReferences(bool verbose);
  ImportModule* FindImportedModule(const string& theModuleName);

private:
  ModuleDefinition& operator = (const ModuleDefinition&);
  typedef map<string, string> PStringMap;
  const string            moduleName;
  string                  classNamePrefix;
  bool                     separateClassFiles;
  StringList              definitiveId;
  Tag::Mode                defaultTagMode;
  SymbolList               exports;
  bool                     exportAll;
  ImportsList              imports;
  PStringMap               importNames;
  TypesVector              types;
  TypeMap                  typeMap;
  ValuesList               values;
//    MibList         mibs;
  int                      indentLevel;
  bool                     usingInlines;
  ObjectClassesList        objectClasses;
  InformationObjectList    informationObjects;
  InformationObjectSetList informationObjectSets;
  map<string,int>    identifiers;
  string              shortModuleName;
  string              cModuleName;
  string            path;
  ModuleList             subModules;
  vector<string>     removeList;
};

template <class T>
boost::shared_ptr<T> FindWithName(const vector<boost::shared_ptr<T> >& cont, const string& name)
{
  typedef vector<boost::shared_ptr<T> > Cont;

  typename Cont::const_iterator itr = cont.begin(), last = cont.end();
  for (; itr != last; ++itr)
    if ((*itr)->GetName() == name)
      return *itr;
    return boost::shared_ptr<T>();

}

extern ModuleDefinition * Module;
extern ModuleList Modules;

typedef stack<ObjectClassBase*> ClassStack;
extern ClassStack *classStack;
extern ParameterList * DummyParameters;

ModuleDefinition* FindModule(const char* name);
void AddRemoveItem(const char* item);
#endif
