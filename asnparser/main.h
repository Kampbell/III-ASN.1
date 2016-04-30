// *INDENT-OFF*
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
 */

#ifndef _MAIN_H
#define _MAIN_H

#if defined(_MSC_VER)
#if _MSC_VER <= 1200
#pragma warning(disable:4786)
#pragma warning(disable:4127)
#endif
#pragma warning(disable:4100)
#pragma warning(disable:4701)
#pragma warning(disable:4127)
#pragma warning(disable:4390)
#pragma warning(disable:4102)
#pragma warning(disable:4244)
#pragma warning(disable:4018)	// '<' : incompatibilité signed/unsigned
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
using std::clog;
using std::endl;
using std::streambuf;
using std::fill_n;
using std::iostream;
using std::istream;
using std::ostream;
using std::ostreambuf_iterator;
using boost::shared_ptr;

#define REENTRANT_PARSER 1

extern unsigned lineNumber;
extern string  fileName;

#ifdef REENTRANT_PARSER
#else
extern FILE * idin;
extern int idparse();
void iderror(const char* str);
#endif
#define FALSE 0
#define TRUE  1


/////////////////////////////////////////
//
//  standard error output from parser
//

enum StdErrorType { Information, Warning, Error, Fatal };

class StdError {
  public:
	StdError(StdErrorType ne) : e(ne) { }
	//StdError(StdErrorType ne, unsigned ln) : e(ne), l(ln) { }
	friend ostream& operator<<(ostream& out, const StdError& e);

  protected:
	StdErrorType e;
};


/////////////////////////////////////////
//
//  intermediate structures from parser
//


typedef vector<string> StringList;

class NamedNumber { ////: public PObject
  public:
	NamedNumber(string * nam);
	NamedNumber(string * nam, int num);
	NamedNumber(string * nam, const string& ref);
	friend ostream& operator << (ostream&, const NamedNumber& );

	void setAutoNumber(const NamedNumber& prev);
	const string& getName() const { return name; }
	int getNumber() const { return number; }

  protected:
	string name;
	string reference;
	int number;
	bool autonumber;
};

class Printable {
  public:
	virtual void printOn(ostream&) const = 0;
};

inline ostream& operator << (ostream& os, const Printable& obj) {
	obj.printOn(os);
	return os;
}

//PLIST(NamedNumberList, NamedNumber);
typedef shared_ptr<NamedNumber> NamedNumberPtr;
typedef list<NamedNumberPtr> NamedNumberList;

enum class Arc {
	ITU_T = 0,
	ISO,
	joint_iso_itu_t
};


// Types

class TypeBase;
typedef shared_ptr<TypeBase> TypePtr;
typedef vector<TypePtr> TypesVector;

class Tag : public Printable {
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
		UniversalUTF8String,
		UniversalRelativeOID,
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

	bool isDefined() const {
		return (type|number)!= 0;
	}

	void printOn(ostream&) const;
	bool operator == (const Tag& other) const {
		return type == other.type&& number == other.number&& mode == other.mode;
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

typedef shared_ptr<ValueSet> ValueSetPtr;
typedef shared_ptr<Constraint> ConstraintPtr;

class ConstraintElementBase : public Printable {
  public:
	ConstraintElementBase();
	~ConstraintElementBase();

	void setExclusions(shared_ptr<ConstraintElementBase> excl) {
		exclusions = excl;
	}

	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual void getConstraint(string& ) const {}
	virtual bool referencesType(const TypeBase& type) const;

	virtual ValueSetPtr getValueSetFromValueField(const string& field) const;
	virtual ValueSetPtr getValueSetFromValueSetField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectSetField(const string& field) const;

	virtual bool hasPERInvisibleConstraint(const Parameter&) const {
		return false;
	}
	virtual void generateObjectSetInstanceCode(const string& , ostream& ) const {}
	virtual void generateObjSetAccessCode(ostream& ) {}
	virtual bool getCharacterSet(string& characterSet) const;
	virtual const SizeConstraintElement* getSizeConstraint() const;
	virtual const FromConstraintElement* getFromConstraint() const;
	virtual const SubTypeConstraintElement* getSubTypeConstraint() const;

	// do not set const here - it is different function!!!
	virtual void printOn(ostream&) {}

  protected:
	shared_ptr<ConstraintElementBase> exclusions;
};

typedef shared_ptr<ConstraintElementBase> ConstraintElementPtr;
typedef vector<ConstraintElementPtr> ConstraintElementVector;

class Constraint : public Printable {
public:
	Constraint(bool extend) : extendable(extend) {};
	Constraint(ConstraintElementPtr& elmt);
	Constraint(auto_ptr<ConstraintElementVector> std, bool extend, auto_ptr<ConstraintElementVector> ext = auto_ptr<ConstraintElementVector>());

	Constraint(const Constraint& other);

	void printOn(ostream&) const;
	void PrintElements(ostream&) const;

	bool isExtendable() const {
		return extendable;
	}
	void getConstraint(string& str) const;
	void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	bool referencesType(const TypeBase& type) const;

	const ConstraintElementVector& getStandardElements() const {
		return standard;
	}
	const ConstraintElementVector& getExtensionElements() const {
		return extensions;
	}
	ConstraintElementVector& getStandardElements() {
		return standard;
	}
	ConstraintElementVector& getExtensionElements() {
		return extensions;
	}

	ValueSetPtr getValueSetFromValueField(const string& field) const;
	ValueSetPtr getValueSetFromValueSetField(const string& field) const;
	ConstraintPtr getObjectSetFromObjectField(const string& field) const;
	ConstraintPtr getObjectSetFromObjectSetField(const string& field) const;
	const SizeConstraintElement* getSizeConstraint() const;
	const FromConstraintElement* getFromConstraint() const;
	const SubTypeConstraintElement* getSubTypeConstraint() const;

	void getCharacterSet(string& characterSet) const;

	virtual auto_ptr<Constraint> Clone() const;
	bool hasPERInvisibleConstraint(const Parameter&) const;
	void generateObjectSetInstanceCode(const string& prefix, ostream& cxx) const;
	void generateObjSetAccessCode(ostream&);

protected:
	ConstraintElementVector standard;
	bool                  extendable;
	ConstraintElementVector extensions;
};

typedef shared_ptr<Constraint> ConstraintPtr;
typedef vector<ConstraintPtr> ConstraintList;

class ConstrainAllConstraintElement : public ConstraintElementBase {
  public:
	ConstrainAllConstraintElement(ConstraintElementPtr excl);
	void printOn(ostream&) const {}
};



class ElementListConstraintElement : public ConstraintElementBase {
  public:
	ElementListConstraintElement();
	ElementListConstraintElement(auto_ptr<ConstraintElementVector> list);
	void printOn(ostream&) const;

	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual void getConstraint(string& str) const;
	virtual bool referencesType(const TypeBase& type) const;
	const ConstraintElementVector& getElements() const {
		return elements;
	}

	virtual ValueSetPtr getValueSetFromValueField(const string& field) const;
	virtual ValueSetPtr getValueSetFromValueSetField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectSetField(const string& field) const;
	virtual bool hasPERInvisibleConstraint(const Parameter&) const;
	virtual void generateObjectSetInstanceCode(const string& prefix, ostream& cxx) const;
	virtual void generateObjSetAccessCode(ostream& );

	virtual const SizeConstraintElement* getSizeConstraint() const;
	virtual const FromConstraintElement* getFromConstraint() const;

	void AppendElements(
		ConstraintElementVector::const_iterator first,
		ConstraintElementVector::const_iterator last
	);
  protected:
	ConstraintElementVector elements;
};


class ValueBase;
typedef shared_ptr<ValueBase> ValuePtr;

class SingleValueConstraintElement : public ConstraintElementBase {
  public:
	SingleValueConstraintElement(const ValuePtr& val);
	~SingleValueConstraintElement();
	void printOn(ostream&) const;

	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual void getConstraint(string& str) const;

	const ValuePtr& getValue() const {
		return value;
	}
	virtual bool hasPERInvisibleConstraint(const Parameter&) const;
	virtual bool getCharacterSet(string& characterSet) const;

  protected:
	const ValuePtr value;
  private:
	SingleValueConstraintElement& operator = (const SingleValueConstraintElement&);
};


class ValueRangeConstraintElement : public ConstraintElementBase {
  public:
	ValueRangeConstraintElement(ValuePtr lowerBound, ValuePtr upperBound);
	~ValueRangeConstraintElement();
	void printOn(ostream&) const;
	void generateRange(ostream& strm);
	virtual void getConstraint(string& str) const;

	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual bool hasPERInvisibleConstraint(const Parameter&) const;
	virtual bool getCharacterSet(string& characterSet) const;
  protected:
	ValuePtr lower;
	ValuePtr upper;
};



class SubTypeConstraintElement : public ConstraintElementBase {
	//PCLASSINFO(SubTypeConstraintElement, ConstraintElementBase);
  public:
	SubTypeConstraintElement(TypePtr typ);
	~SubTypeConstraintElement();
	void printOn(ostream&) const;
	void generateCplusplus(const string&, ostream&, ostream&, ostream&) const;
	virtual bool referencesType(const TypeBase& type) const;
	virtual bool hasPERInvisibleConstraint(const Parameter&) const;
	virtual void getConstraint(string& str) const;
	virtual const SubTypeConstraintElement* getSubTypeConstraint() const;
	string getSubTypeName() const;
	const TypePtr getSubType() const {
		return subtype;
	}
  protected:
	TypePtr subtype;
};


class NestedConstraintConstraintElement : public ConstraintElementBase {
  public:
	NestedConstraintConstraintElement(ConstraintPtr con);
	~NestedConstraintConstraintElement();

	virtual bool referencesType(const TypeBase& type) const;
	virtual bool hasPERInvisibleConstraint(const Parameter&) const;

  protected:
	ConstraintPtr constraint;
};


class SizeConstraintElement : public NestedConstraintConstraintElement {
  public:
	SizeConstraintElement(ConstraintPtr constraint);
	virtual void getConstraint(string& str) const;
	void printOn(ostream&) const;
	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual const SizeConstraintElement* getSizeConstraint() const;
};


class FromConstraintElement : public NestedConstraintConstraintElement {
  public:
	FromConstraintElement(ConstraintPtr constraint);
	virtual void getConstraint(string& str) const;
	void printOn(ostream&) const;
	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual const FromConstraintElement* getFromConstraint() const;
	string getCharacterSet(const char* canonicalSet, int canonicalSetSize) const;
	int getRange(ostream& cxx) const;
};


class WithComponentConstraintElement : public NestedConstraintConstraintElement {
  public:
	WithComponentConstraintElement(string name, ConstraintPtr constraint, int presence);
	void printOn(ostream&) const;
	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual bool hasPERInvisibleConstraint(const Parameter&) const {
		return false;
	}

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


class InnerTypeConstraintElement : public ElementListConstraintElement {
  public:
	InnerTypeConstraintElement(auto_ptr<ConstraintElementVector> list, bool partial);

	void printOn(ostream&) const;
	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;

  protected:
	bool partial;
};

class ActualParameter;
typedef shared_ptr<ActualParameter> ActualParameterPtr;
typedef vector<ActualParameterPtr> ActualParameterList;
typedef shared_ptr<ActualParameterList> ActualParameterListPtr;

class UserDefinedConstraintElement : public ConstraintElementBase {
  public:
	UserDefinedConstraintElement(ActualParameterListPtr list);
	void printOn(ostream&) const;
	virtual void generateCplusplus(const string& fn, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual bool hasPERInvisibleConstraint(const Parameter&) const {
		return true;
	}
  protected:
	ActualParameterListPtr parameters;
};

class DefinedObjectSet;
typedef shared_ptr<DefinedObjectSet> DefinedObjectSetPtr;
class TableConstraint {
  public:
	TableConstraint(shared_ptr<DefinedObjectSet> objSet,
					auto_ptr<StringList> atNotations = auto_ptr<StringList>());
	~TableConstraint();
	bool ReferenceType(const TypeBase& type);
	string getObjectSetIdentifier() const;
	const StringList* getAtNotations() const {
		return atNotations.get();
	}
  private:
	shared_ptr<DefinedObjectSet> objSet;
	auto_ptr<StringList> atNotations;
};

/////////////////////////////////////////////

class Parameter : public Printable {
  public:
	Parameter(const string& name);
	Parameter(const string& name, int type);
	Parameter(const Parameter& other);
	int getIdentifierType();
	const string& getName() const;
	void printOn(ostream& strm) const;
	virtual bool isTypeParameter() const {
		return true;
	}
	virtual bool ReferencedBy(const TypeBase&) const;
	virtual ActualParameterPtr MakeActualParameter() const;
  protected:
	string name;
	int identifierType;
};

class ValueParameter : public Parameter {
  public:
	ValueParameter(TypePtr governor, const string& nam );
	ValueParameter(const ValueParameter& other);
	~ValueParameter();
	TypeBase* getGovernor() {
		return governor.get();
	}
	void printOn(ostream& strm) const;
	virtual bool isTypeParameter() const {
		return false;
	}
	virtual bool ReferencedBy(const TypeBase&) const {
		return false;
	}
	virtual ActualParameterPtr MakeActualParameter() const;
  protected:
	TypePtr governor;
};

class DefinedObjectClass;
typedef shared_ptr<DefinedObjectClass> DefinedObjectClassPtr;
class ObjectParameter : public Parameter {
  public:
	ObjectParameter(DefinedObjectClassPtr governor,
					const string& name);
	~ObjectParameter();
	DefinedObjectClass* getGovernor() {
		return governor.get();
	}
	void printOn(ostream& strm) const;
	virtual bool isTypeParameter() const {
		return false;
	}
	virtual bool ReferencedBy(const TypeBase&) const;
	virtual ActualParameterPtr MakeActualParameter() const;
  protected:
	DefinedObjectClassPtr governor;
};

typedef shared_ptr<Parameter> ParameterPtr;
typedef vector<ParameterPtr> ParameterListRep;

class ParameterList : public Printable {
  public:
	void Append(ParameterPtr param);
	bool isEmpty() const {
		return rep.empty();
	}
	int getIdentifierType(const char* identifier);
	Parameter* getParameter(const char* identifier);
	void generateCplusplus(string& templatePrefix, string& classNameString);
	void printOn(ostream& strm) const;
	shared_ptr<ParameterList> getReferencedParameters(const TypeBase& type) const;
	ActualParameterListPtr MakeActualParameters() const;
	void swap(ParameterList& other) {
		rep.swap(other.rep);
	}
	ParameterListRep rep;
};

typedef shared_ptr<ParameterList> ParameterListPtr;

////////////////////////////////////////////
class ModuleDefinition;

class TypeBase : public Printable {
  public:
	void printOn(ostream&) const;

	void beginParseValue() const;
	void endParseValue() const;
	void beginParseValueSet() const;
	void endParseValueSet() const;

	const string& getName() const					{ return name;	}
	const string& getCppName() const				{ return identifier;	}
	void setName(const string& name);
	void setAsValueSetType()						{ isvaluesettype = true;	}
	const string& getIdentifier() const				{ return identifier;	}
	void setTag(Tag::Type cls, unsigned num, Tag::Mode mode);
	void setTag(const Tag& _tag)					{ tag = _tag;	}
	const Tag& getTag() const						{ return tag;	}
	const Tag& getDefaultTag() const				{ return defaultTag;	}
	bool hasNonStandardTag() const					{ return tag.isDefined()&& tag != defaultTag;	}
	void setParameters(ParameterList& list);
	void addConstraint(ConstraintPtr constraint)	{ constraints.push_back(constraint);	}
	bool hasConstraints() const;
	void moveConstraints(TypeBase& from);
	void copyConstraints(const TypeBase& from);
	virtual bool hasParameters() const				{ return !parameters.isEmpty();	}
	bool isOptional() const							{ return isoptional;	}
	void setOptional()								{ isoptional = true;	}
	bool hasDefaultValue() const					{ return defaultValue.get()!= 0;	}
	ValuePtr getDefaultValue() const				{ return defaultValue;	}
	void setDefaultValue(ValuePtr value);
	const string& getTemplatePrefix() const			{ return templatePrefix;	}
	const string& getClassNameString() const		{ return classNameString;	}
	void setOuterClassName(const string& oname)		{ outerClassName = oname;	}
	void setTemplatePrefix(const string& tname)		{ templatePrefix = tname;	}

	bool isValueSetType() const						{ return isvaluesettype;	}
	virtual bool isChoice() const					{ return false; }
	virtual bool isParameterizedType() const		{ return false; }
	virtual bool isPrimitiveType() const			{ return true; }
	virtual bool isSequenceOfType() const			{ return false;	}

	virtual void adjustIdentifier(bool);
	virtual void flattenUsedTypes();
	virtual TypePtr flattenThisType(TypePtr& self, const TypeBase& parent);

	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateForwardDecls(ostream& fwd, ostream& hdr);
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual const char * getAncestorClass() const = 0;
	virtual string getTypeName() const;
	virtual bool canReferenceType() const;
	virtual bool referencesType(const TypeBase& type) const;
	virtual const string& getCModuleName() const;
	virtual bool isParameterisedImport() const;
	virtual bool canBeFwdDeclared(bool isComponent = false) const;
	virtual bool forwardDeclareMe(ostream& hdr);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);

	bool isGenerated() const {
		return isgenerated;
	}
	virtual void beginGenerateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	void endGenerateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	void generateTags(ostream& strm) const;
	void generateCplusplusConstraints(const string& prefix, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;

	const ConstraintList& getConstraints() const {
		return constraints;
	}
	virtual void beginParseThisTypeValue() const {}
	virtual void endParseThisTypeValue() const {}
	virtual void resolveReference() const {}

	virtual string getPrimitiveType(const string& myName) const;

	virtual void RemovePERInvisibleConstraint(const ParameterPtr&);
	void RemovePERInvisibleConstraints();
	virtual bool useType(const TypeBase& ) const {
		return false;
	}

	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual bool needGenInfo() const;
	TypePtr SeqOfflattenThisType(const TypeBase& parent, TypePtr thisPtr);

	const ParameterList& getParameters() const {
		return parameters;
	}
	virtual void generateDecoder(ostream&) {}

	enum RemoveResult {
		OK,
		MAY_NOT,
		FORBIDDEN
	};
	virtual RemoveResult canRemoveType(const TypeBase&) {
		return OK;
	}
	virtual bool removeThisType(const TypeBase& type) {
		return getName() == type.getName();
	}
	virtual bool isRemovedType() const {
		return false;
	}
	ModuleDefinition* getModule() const {
		return module;
	}
  protected:
	TypeBase(unsigned tagNum, ModuleDefinition* md);
	TypeBase(TypeBase& copy);

	void PrintStart(ostream&) const;
	void PrintFinish(ostream&) const;
	const char* getClass() const;

	Tag				tag;
	Tag				defaultTag;
	string			name; // The ASN.1 Type name
	string			identifier; // The converted C Type name
	ConstraintList	constraints;
	bool				isoptional;
	ValuePtr			defaultValue;
	bool				isgenerated;
	ParameterList		parameters;
	string			templatePrefix;
	string			classNameString;
	string			shortClassNameString;
	string			outerClassName;
	bool				isvaluesettype;
	ModuleDefinition* module;
};



class DefinedType : public TypeBase {
  public:
	DefinedType(const string& name);
	DefinedType(TypePtr refType);
	DefinedType(TypePtr refType, TypePtr& bType);
	DefinedType(TypePtr refType, const string& name);
	DefinedType(TypePtr refType, const TypeBase& parent);

	void printOn(ostream&) const;

	virtual bool isChoice() const;
	virtual bool isParameterizedType() const;
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual const char * getAncestorClass() const;
	virtual string getTypeName() const;
	virtual bool canReferenceType() const;
	virtual bool referencesType(const TypeBase& type) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual bool canBeFwdDeclared(bool isComponent = false) const;

	virtual void beginParseThisTypeValue() const;
	virtual void endParseThisTypeValue() const;
	virtual void resolveReference() const;
	virtual const string& getCModuleName() const;
	virtual bool useType(const TypeBase& type) const;
	virtual bool needGenInfo() const;
	virtual RemoveResult canRemoveType(const TypeBase&);
	virtual bool removeThisType(const TypeBase&);
	virtual string getPrimitiveType(const string& myName) const;
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
	virtual TypePtr flattenThisType(TypePtr& self, const TypeBase& parent);
	virtual bool isPrimitiveType() const;

  protected:
	void ConstructFromType(TypePtr& refType, const string& name);
	string referenceName;
	mutable TypePtr baseType;
	mutable bool unresolved;
};


class ParameterizedType : public DefinedType {
  public:
	ParameterizedType(const string& name, ActualParameterList& args);
	ParameterizedType(TypePtr& refType,
					  const TypeBase& parent,
					  ActualParameterList& args);

	void printOn(ostream&) const;

	virtual bool isParameterizedType() const;
	virtual string getTypeName() const;
	virtual bool referencesType(const TypeBase& type) const;
	virtual bool useType(const TypeBase& type) const;
	virtual RemoveResult canRemoveType(const TypeBase&);
  protected:
	ActualParameterList arguments;
};


class SelectionType : public TypeBase {
  public:
	SelectionType(const string& name, TypePtr base);
	~SelectionType();

	void printOn(ostream&) const;

	virtual void flattenUsedTypes();
	virtual TypePtr flattenThisType(TypePtr& self, const TypeBase& parent);
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual const char * getAncestorClass() const;
	virtual bool canReferenceType() const;
	virtual bool referencesType(const TypeBase& type) const;
	virtual bool useType(const TypeBase& type) const;

  protected:
	string selection;
	TypePtr baseType;
};


class BooleanType : public TypeBase {
  public:
	BooleanType();
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual const char * getAncestorClass() const;
	virtual string getPrimitiveType(const string& ) const {
		return "bool";
	}
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
};


class IntegerType : public TypeBase {
  public:
	IntegerType();
	IntegerType(NamedNumberList&);
	virtual const char * getAncestorClass() const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	string getTypeName() const;
	virtual string getPrimitiveType(const string& myName) const {
		if(!myName.empty())
			return myName + "::value_type::int_type";
		else
			return "int_type";
	}

	virtual bool canReferenceType() const;
	virtual bool referencesType(const TypeBase& type) const;

	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual bool needGenInfo() const;
	virtual void generateInfo(const TypeBase* type, ostream& , ostream&);
	virtual TypePtr flattenThisType(TypePtr& self, const TypeBase& parent);

	virtual void
	beginParseThisTypeValue() const,
							endParseThisTypeValue() const;

  protected:
	NamedNumberList allowedValues;
};


class EnumeratedType : public TypeBase {
  public:
	EnumeratedType(NamedNumberList& enums, bool extend, NamedNumberList* ext);
	void printOn(ostream&) const;
	virtual TypePtr flattenThisType(TypePtr& self, const TypeBase& parent);
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual const char * getAncestorClass() const;
	virtual string getPrimitiveType(const string& myName) const {
		if(!myName.empty())
			return myName + "::value_type::NamedNumber";
		else
			return "NamedNumber";
	}
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual bool needGenInfo() const {
		return true;
	}
	bool isPrimitiveType() const;
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);

	virtual void
	beginParseThisTypeValue() const,
							endParseThisTypeValue() const;

  protected:
	NamedNumberList enumerations;
	size_t numEnums;
	bool extendable;
	int maxEnumValue;
};


class RealType : public TypeBase {
  public:
	RealType();
	virtual const char * getAncestorClass() const;
	virtual string getPrimitiveType(const string& ) const {
		return "double";
	}
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
};


class BitStringType : public TypeBase {
  public:
	BitStringType();
	BitStringType(NamedNumberList&);
	virtual const char * getAncestorClass() const;
	string getTypeName() const;
	//virtual string getPrimitiveType(const string& myName) const { return TypeBase::getPrimitiveType();}
	virtual bool needGenInfo() const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);

	virtual int getToken() const;
	virtual void beginParseThisTypeValue() const;
	virtual void endParseThisTypeValue() const;

  protected:
	NamedNumberList allowedBits;
};


class OctetStringType : public TypeBase {
  public:
	OctetStringType();
	virtual const char * getAncestorClass() const;
	string getTypeName() const;
	virtual string getPrimitiveType(const string& ) const {
		return "const ASN1::octets&";
	}
	virtual const char* getConstrainedType() const;
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
};


class NullType : public TypeBase {
  public:
	NullType();
	virtual const char * getAncestorClass() const;
	virtual void beginParseThisTypeValue() const;
	virtual void endParseThisTypeValue() const;
};


class SequenceType : public TypeBase {
	void printOn(ostream&) const;
  public:
	SequenceType(TypesVector* std, bool extendable, TypesVector * extensions, unsigned tagNum = Tag::UniversalSequence);
	virtual void flattenUsedTypes();
	virtual TypePtr flattenThisType(TypePtr& self, const TypeBase& parent);
	virtual bool isPrimitiveType() const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual const char * getAncestorClass() const;
	virtual bool canReferenceType() const;
	virtual bool referencesType(const TypeBase& type) const;

	void generateComponent(TypeBase& field, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, int id);
	virtual bool canBeFwdDeclared(bool isComponent ) const;
	virtual void RemovePERInvisibleConstraint(const ParameterPtr&);
	virtual bool useType(const TypeBase& type) const;
	virtual bool needGenInfo() const {
		return true;
	}
	virtual void generateForwardDecls(ostream& fwd, ostream& hdr);
	virtual RemoveResult canRemoveType(const TypeBase&);
	virtual bool removeThisType(const TypeBase&);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
  protected:
	TypesVector fields;
	size_t numFields;
	mutable vector<bool> needFwdDeclare;
	bool extendable;
	mutable bool detectingLoop;
};


class SequenceOfType : public TypeBase {
  public:
	SequenceOfType(TypePtr base, ConstraintPtr constraint = ConstraintPtr(), unsigned tag = Tag::UniversalSequence);
	~SequenceOfType();
	void printOn(ostream&) const;
	virtual void flattenUsedTypes();
	virtual TypePtr flattenThisType(TypePtr& self, const TypeBase& parent);
	virtual bool isPrimitiveType() const;
	virtual void generateForwardDecls(ostream& fwd, ostream& hdr);
	virtual const char * getAncestorClass() const;
	virtual bool canReferenceType() const;
	virtual bool referencesType(const TypeBase& type) const;
	virtual string getTypeName() const;
	virtual bool forwardDeclareMe(ostream& hdr);
	virtual bool isSequenceOfType() const		{	return true;}
	virtual void RemovePERInvisibleConstraint(const ParameterPtr&);
	virtual bool useType(const TypeBase& type) const;
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	void setNonTypedef(bool v)					{ nonTypedef = v; }
	virtual RemoveResult canRemoveType(const TypeBase&);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
  protected:
	TypePtr baseType;
	bool nonTypedef;
};


class SetType : public SequenceType {
  public:
	SetType();
	SetType(SequenceType& seq);
	virtual const char * getAncestorClass() const;
};


class SetOfType : public SequenceOfType {
  public:
	SetOfType(TypePtr base, ConstraintPtr constraint = ConstraintPtr());
	virtual string getTypeName() const;
};


class ChoiceType : public SequenceType {
  public:
	ChoiceType(TypesVector * std = nullptr, bool extendable = false, TypesVector * extensions = nullptr);
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual bool isPrimitiveType() const;
	virtual bool isChoice() const;
	virtual TypePtr flattenThisType(TypePtr& self, const TypeBase& parent);
	virtual const char * getAncestorClass() const;
	void generateComponent(TypeBase& field, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, int id);
	virtual RemoveResult canRemoveType(const TypeBase&);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
  private:
	vector<TypeBase*> sortedFields;
};


class EmbeddedPDVType : public TypeBase {
  public:
	EmbeddedPDVType();
	virtual const char * getAncestorClass() const;
};


class ExternalType : public TypeBase {
  public:
	ExternalType();
	virtual const char * getAncestorClass() const;
};


class AnyType : public TypeBase {
  public:
	AnyType(const string& ident);
	void printOn(ostream& strm) const;
	virtual const char * getAncestorClass() const;
  protected:
	string identifier;
};


class StringTypeBase : public TypeBase {
  public:
	StringTypeBase(int tag);
	virtual string getTypeName() const;
	virtual string getPrimitiveType(const string& ) const {
		return "const ASN1_STD string&";
	}
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual bool needGenInfo() const;
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
	virtual const char* getCanonicalSetString() const {
		return nullptr;
	};
  protected:
	const char* canonicalSet = nullptr;
	const char* canonicalSetRep = nullptr;
	int canonicalSetSize = 0;
};


class UTF8StringType : public StringTypeBase {
  public:
	UTF8StringType();
	virtual const char * getAncestorClass() const;
	virtual string getPrimitiveType(const string& ) const {
		return "const ASN1_STD wstring&";
	}
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
};


class BMPStringType : public StringTypeBase {
  public:
	BMPStringType();
	virtual const char * getAncestorClass() const;
	virtual string getPrimitiveType(const string& ) const {
		return "const ASN1_STD wstring&";
	}
	virtual void generateOperators(ostream& fwd, ostream& hdr, ostream& cxx, const TypeBase& actualType);
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
};


class GeneralStringType : public StringTypeBase {
  public:
	GeneralStringType();
	virtual const char * getAncestorClass() const;
};


class GraphicStringType : public StringTypeBase {
  public:
	GraphicStringType();
	virtual const char * getAncestorClass() const;
};


class IA5StringType : public StringTypeBase {
  public:
	IA5StringType();
	virtual const char * getAncestorClass() const;
};


class ISO646StringType : public StringTypeBase {
  public:
	ISO646StringType();
	virtual const char * getAncestorClass() const;
};


class NumericStringType : public StringTypeBase {
  public:
	NumericStringType();
	virtual const char * getAncestorClass() const;
};


class PrintableStringType : public StringTypeBase {
  public:
	PrintableStringType();
	virtual const char * getAncestorClass() const;
};


class TeletexStringType : public StringTypeBase {
  public:
	TeletexStringType();
	virtual const char * getAncestorClass() const;
};


class T61StringType : public StringTypeBase {
  public:
	T61StringType();
	virtual const char * getAncestorClass() const;
};


class UniversalStringType : public StringTypeBase {
  public:
	UniversalStringType();
	virtual const char * getAncestorClass() const;
};


class VideotexStringType : public StringTypeBase {
  public:
	VideotexStringType();
	virtual const char * getAncestorClass() const;
};


class VisibleStringType : public StringTypeBase {
  public:
	VisibleStringType();
	virtual const char * getAncestorClass() const;
};


class UnrestrictedCharacterStringType : public StringTypeBase {
  public:
	UnrestrictedCharacterStringType();
	virtual const char * getAncestorClass() const;
};


class GeneralizedTimeType : public TypeBase {
  public:
	GeneralizedTimeType();
	virtual const char * getAncestorClass() const;
	virtual string getPrimitiveType() const {
		return "const char*";
	}
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
};


class UTCTimeType : public TypeBase {
  public:
	UTCTimeType();
	virtual const char * getAncestorClass() const;
	virtual string getPrimitiveType() const {
		return "const char*";
	}
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
};


class ObjectDescriptorType : public TypeBase {
  public:
	ObjectDescriptorType();
	virtual const char * getAncestorClass() const;
};


class RelativeOIDType : public TypeBase {
  public:
	RelativeOIDType();
	virtual const char * getAncestorClass() const;
	virtual void beginParseThisTypeValue() const;
	virtual void endParseThisTypeValue() const;
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
};

class ObjectIdentifierType : public TypeBase {
  public:
	ObjectIdentifierType();
	virtual const char * getAncestorClass() const;
	virtual void beginParseThisTypeValue() const;
	virtual void endParseThisTypeValue() const;
	virtual void generateConstructors(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
};



class ObjectClassBase;
typedef shared_ptr<ObjectClassBase> ObjectClassBasePtr;

class ObjectClassFieldType : public TypeBase {
  public:
	ObjectClassFieldType(ObjectClassBasePtr  objclass, const string& field);
	~ObjectClassFieldType();
	virtual const char * getAncestorClass() const;
	void printOn(ostream&) const;
	virtual bool canReferenceType() const;
	virtual bool referencesType(const TypeBase& type) const;
	TypeBase* getFieldType();
	const TypeBase* getFieldType() const;
	virtual string getTypeName() const;
	void addTableConstraint(shared_ptr<TableConstraint> constraint);
	void generateDecoder(ostream&);
	virtual void generateInfo(const TypeBase* type, ostream& fwd, ostream& hdr, ostream& cxx);
	string getConstrainedTypeName() const;
  protected:
	ObjectClassBasePtr asnObjectClass;
	string asnObjectClassField;
	shared_ptr<TableConstraint> tableConstraint;
};


class ImportedType : public TypeBase {
  public:
	ImportedType(const TypePtr& ref);
	ImportedType(const string& name, bool parameterised);
	virtual const char * getAncestorClass() const;
	virtual void adjustIdentifier(bool usingNamespace);
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual bool isParameterisedImport() const;

	virtual void beginParseThisTypeValue() const;
	virtual void endParseThisTypeValue() const;

	void setModuleName(const string& name);
	virtual const string& getCModuleName() const {
		return cppModuleName;
	}
	const string& getModuleName() const {
		return moduleName;
	}
	bool isPrimitiveType() const;
  protected:
	string modulePrefix;
	bool    parameterised;
	const TypePtr reference;
	string moduleName;
	string cppModuleName;
  private:
	ImportedType& operator = (const ImportedType&);
};


class InformationObject;
typedef shared_ptr<InformationObject> InformationObjectPtr;


class TypeFromObject : public TypeBase {
  public:
	TypeFromObject(InformationObjectPtr  obj, const string& fld);
	~TypeFromObject();
	virtual const char * getAncestorClass() const;
	void printOn(ostream&) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
  protected:
	InformationObjectPtr refObj;
	string field;
};

class RemovedType : public TypeBase {
  public:
	RemovedType(const TypeBase& type);
	virtual bool isRemovedType() const {
		return true;
	}
	virtual const char * getAncestorClass() const;
};

// Values

class ValueBase : public Printable {
  public:
	void setValueName(const string& name);
	const string& getName() const {		return valueName;	}
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual void generateConst(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual bool isPERInvisibleConstraint(const Parameter&) const {		return false;	}

  protected:
	void PrintBase(ostream&) const;
	string valueName;
};

typedef vector<ValuePtr> ValuesList;

class DefinedValue : public ValueBase {
	//PCLASSINFO(DefinedValue, ValueBase);
  public:
	DefinedValue(const string& name);
	DefinedValue(const ValuePtr& value);
	DefinedValue(const string& name, const ValuePtr& value);
	void printOn(ostream&) const;
	const string& getReference() const { return referenceName; }
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual bool isPERInvisibleConstraint(const Parameter& param) const {
		return param.getName() == referenceName;
	}
  protected:
	string referenceName;
	mutable ValuePtr actualValue;
	mutable bool unresolved;
};

class ImportedValue : public DefinedValue {
  public:
	ImportedValue(const string& modName, const string& name, const ValuePtr& v)
		: DefinedValue(name, v), moduleName(modName) {}
	const string& getModuleName() const {
		return moduleName;
	}
  private:
	string moduleName;
};


class BooleanValue : public ValueBase {
  public:
	BooleanValue(bool newVal);
	void printOn(ostream&) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
  protected:
	bool value;
};


class IntegerValue : public ValueBase {
  public:
	IntegerValue(boost::int64_t newVal);
	void printOn(ostream&) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	virtual void generateConst(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;

#if (__SIZEOF_LONG__ != 8)
	operator int64_t() const {
		return value;
	}
#endif
	operator long() const {
		return (long)value;
	}

  protected:
	boost::int64_t value;
};


class RealValue : public ValueBase {
  public:
	RealValue(double newVal);
	void printOn(ostream&) const {}
  protected:
	double value;
};


class OctetStringValue : public ValueBase {
  public:
	OctetStringValue() { }
	OctetStringValue(const string& newVal);
	void printOn(ostream&) const {}
  protected:
	vector<char> value;
};


class BitStringValue : public ValueBase {
  public:
	BitStringValue() { }
	BitStringValue(const string& newVal);
	BitStringValue(StringList * newVal);
	void printOn(ostream&) const;
  protected:
	string value;
};


class NullValue : public ValueBase {
  public:
	void printOn(ostream&) const {}
};


class CharacterValue : public ValueBase {
  public:
	CharacterValue(char c);
	CharacterValue(char t1, char t2);
	CharacterValue(char q1, char q2, char q3, char q4);
	void printOn(ostream&) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	unsigned getValue() const {
		return value;
	}
  protected:
	unsigned value;
};


class CharacterStringValue : public ValueBase {
  public:
	CharacterStringValue() { }
	CharacterStringValue(const string& newVal);
	CharacterStringValue(StringList& newVal);
	void printOn(ostream&) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	void getValue(string& v) const;
  protected:
	string value;
};

class NumberForm {
public:
	NumberForm(int number = -1) : number(number) {}
	NumberForm(const string form) : form(form) { }
	int getNumber() const { return number; }
	const string& getForm() const { return form; }
	friend ostream& operator<< (ostream& os, const NumberForm& numberForm) {
		if (numberForm.number == -1)
		return os;
		
		os << '(';
		if (numberForm.form.empty())
			os << numberForm.number;
		else
			os << numberForm.form;
		os << ')';
		return os;
	}

private:
	int number;
	string form;
};
class ObjIdComponent {
public:
	ObjIdComponent(const char* name) : name(name) {}
	ObjIdComponent(const string& name) : name(name) { if (name == "joint-iso-itu-t") numberForm = NumberForm(2); }
	ObjIdComponent(const NumberForm& numberform) : numberForm(numberform) {}
	ObjIdComponent(const string& name, const NumberForm& numberform) : name(name), numberForm(numberform) {}
	const string& getName() const { return name; }
	const NumberForm& getNumberForm() const { return numberForm; }
	friend ostream& operator<< (ostream& os, const ObjIdComponent& oic) {
		if (!oic.name.empty()) {
			os << oic.name;
			os << oic.numberForm;
		}
		return os;
	}
private:
	string name;
	NumberForm numberForm;
};
typedef vector<ObjIdComponent> ObjIdComponentList;

class ObjectIdentifierValue : public ValueBase {
  public:
//	ObjectIdentifierValue(const string& newVal);
	ObjectIdentifierValue(ObjIdComponentList& newVal);
	virtual void generateCplusplus(ostream& fwd, ostream&hdr, ostream& cxx, ostream& inl) const;
	virtual void generateConst(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	void printOn(ostream&) const;
	const ObjIdComponentList& getComponents() const { return value; }
  protected:
	ObjIdComponentList value;
};


class MinValue : public ValueBase {
  public:
	void printOn(ostream&) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
};


class MaxValue : public ValueBase {
  public:
	void printOn(ostream&) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
};


class SequenceValue : public ValueBase {
  public:
	SequenceValue(ValuesList * list = nullptr);
	void printOn(ostream&) const;
  protected:
	ValuesList values;
};

class ChoiceValue : public ValueBase {
  public:
	ChoiceValue(const TypePtr& typ, const string& fieldName, ValuePtr val)
		: type(typ), fieldname(fieldName), value(val) { }
	void printOn(ostream&) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
  protected:
	const TypePtr type;
	string fieldname;
	ValuePtr value;
  private:
	ChoiceValue& operator = (const ChoiceValue&);
};

// ValueSet
class ValueSet : public Printable {
  public:
	virtual void Union(ValueSetPtr&) = 0;
	virtual void Intersect(ValueSetPtr&) = 0;
	virtual TypePtr MakeValueSetType() = 0;
	virtual TypePtr getType() = 0;
	virtual ConstraintElementVector* getElements() = 0;
	virtual void resolveReference() const {}
};

class ValueSetDefn : public ValueSet {
  public:
	ValueSetDefn();
	ValueSetDefn(TypePtr type, ConstraintPtr cons);
	~ValueSetDefn();

	void Union(ValueSetPtr&);
	void Intersect(ValueSetPtr&);
	TypePtr MakeValueSetType();
	void printOn(ostream& os) const;

	TypePtr getType() {
		return type;
	}
	ConstraintElementVector* getElements() {
		return elements.get();
	}
	void resolveReference() const;
  protected:
	TypePtr type;
	auto_ptr<ConstraintElementVector> elements;
	bool extendable;
};



class ObjectSetConstraintElement;
class ValueSetFromObject : public ValueSet {
  public:
	ValueSetFromObject(InformationObjectPtr obj, const string& fld);
	~ValueSetFromObject();
	void Union(ValueSetPtr&);
	void Intersect(ValueSetPtr&);
	TypePtr MakeValueSetType();
	void printOn(ostream& ) const;

	TypePtr getType();
	ConstraintElementVector* getElements();
  protected:
	ValueSetPtr getRepresentation();

	InformationObjectPtr object;
	string field;
	ValueSetPtr rep;
};

typedef shared_ptr<ObjectSetConstraintElement> ObjectSetConstraintElementPtr;

class ValueSetFromObjects : public ValueSet {
  public:
	ValueSetFromObjects(ObjectSetConstraintElementPtr objSet,
						const string& fld);
	~ValueSetFromObjects();
	void Union(ValueSetPtr&);
	void Intersect(ValueSetPtr&);
	TypePtr MakeValueSetType();
	void printOn(ostream& ) const;

	TypePtr getType();
	ConstraintElementVector* getElements();
  protected:
	ValueSetPtr getRepresentation();

	ObjectSetConstraintElementPtr objectSet;
	string field;
	ValueSetPtr rep;
};

// object class
class FieldSetting;

typedef shared_ptr<FieldSetting> FieldSettingPtr;
typedef vector<FieldSettingPtr> FieldSettingList;

class FieldSpec;
//PLIST(FieldSpecsList, FieldSpec);
typedef shared_ptr<FieldSpec> FieldSpecPtr;
typedef vector<FieldSpecPtr> FieldSpecsList;

class FieldSpec : public Printable {
  public:
	FieldSpec(const string& nam, bool optional = false);
	virtual ~FieldSpec();

	bool isOptional() const {
		return isoptional;
	}
	virtual bool hasDefault() const = 0;
	const string& getName() const {
		return name;
	}
	const string& getIdentifier() const {
		return identifier;
	}
	virtual string getField() const = 0;

	virtual void EstablishFieldRelation(FieldSpecsList* ) {}
	virtual void beginParseSetting(FieldSettingList* ) const {}
	virtual void endParseSetting() const {}

	virtual int getToken() const = 0;
	void printOn(ostream&) const;

	virtual void resolveReference() const {};

	// used only for FixedTypeValueField and FixedTypeValueSetField
	virtual TypeBase* getFieldType() {
		return nullptr;
	}
	virtual const TypeBase* getFieldType() const {
		return nullptr;
	}

	//

	virtual bool getKey(TypePtr& , string& ) {
		return false;
	}
	virtual void FwdDeclare(ostream& ) const {}
	virtual void generate_info_type_constructor(ostream&) const {}
	virtual void generate_info_type_memfun(ostream& ) const {}
	virtual void generate_info_type_mem(ostream& ) const {}

	virtual void generate_value_type(ostream& ) const {}
	virtual void generateTypeField(const string& templatePrefix,
								   const string& classNameString,
								   const TypeBase* keyType,
								   const string& objClassName,
								   ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;

  protected:

	string name;
	string identifier;
	bool isoptional;
};

class TypeFieldSpec : public FieldSpec {
  public:
	TypeFieldSpec(const string& nam, bool optional = false, TypePtr defaultType= TypePtr());
	~TypeFieldSpec();
	virtual bool hasDefault() const;
	string getField() const;
	string getDefault() const;
	TypePtr getDefaultType();
	virtual int getToken() const;
	virtual bool getKey(TypePtr& keyType, string& keyName);

	void printOn(ostream&) const;
	virtual void resolveReference() const;
	virtual void generate_info_type_constructor(ostream&) const;
	virtual void generate_info_type_memfun(ostream& hdr) const;
	virtual void generate_info_type_mem(ostream& ) const;
	virtual void generate_value_type(ostream& hdr) const;
	virtual void generateTypeField(const string& templatePrefix,
								   const string& classNameString,
								   const TypeBase* keyType,
								   const string& objClassName,
								   ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
  protected:
	TypePtr type;
};
class FixedTypeValueFieldSpec;
typedef shared_ptr<FixedTypeValueFieldSpec> FixedTypeValueFieldSpecPtr;
class FixedTypeValueFieldSpec : public FieldSpec {
  public:
	FixedTypeValueFieldSpec(const string& nam, TypePtr t, bool optional = false,
							bool unique = false);
	~FixedTypeValueFieldSpec();

	virtual bool hasDefault() const;
	void setDefault(ValuePtr value);
	string getField() const;
	virtual void beginParseSetting(FieldSettingList*)  const;
	virtual void endParseSetting() const;
	virtual int getToken() const;

	void printOn(ostream&) const;
	virtual void resolveReference() const;

	virtual bool getKey(TypePtr& keyType, string& keyName);
	virtual TypeBase* getFieldType();
	virtual const TypeBase* getFieldType() const;

  protected:
	bool isUnique;
	TypePtr  type;
	ValuePtr defaultValue;
};

class FixedTypeValueSetFieldSpec : public FieldSpec {
  public:
	FixedTypeValueSetFieldSpec(const string& nam, TypePtr t, bool optional = false);
	~FixedTypeValueSetFieldSpec();

	virtual bool hasDefault() const;
	void setDefault(ValueSetPtr valueSet) {
		defaultValueSet = valueSet;
	}
	string getField() const {
		return type->getName();
	}
	virtual void beginParseSetting(FieldSettingList*) const;
	virtual void endParseSetting() const;
	virtual int getToken() const;

	void printOn(ostream&) const;
	virtual void resolveReference() const;

	virtual TypeBase* getFieldType();
	virtual const TypeBase* getFieldType() const;

  protected:
	TypePtr  type;
	ValueSetPtr defaultValueSet;
};

class VariableTypeValueFieldSpec : public FieldSpec {
  public:
	VariableTypeValueFieldSpec(const string& nam,
							   const string& fieldname,
							   bool optional = false);
	~VariableTypeValueFieldSpec();

	virtual bool hasDefault() const;
	string getField() const {
		return fieldName;
	}
	void setDefault(ValuePtr value) {
		defaultValue = value;
	}
	virtual void EstablishFieldRelation(FieldSpecsList* specs);

	virtual void beginParseSetting(FieldSettingList*) const;
	virtual void endParseSetting() const;
	virtual int getToken() const;

	void printOn(ostream&) const;
	virtual void resolveReference() const;

  protected:
	string fieldName;
	ValuePtr defaultValue;
	TypePtr defaultType;
};

class VariableTypeValueSetFieldSpec : public FieldSpec {
  public:
	VariableTypeValueSetFieldSpec(const string& nam,
								  const string& fieldname,
								  bool optional = false);
	~VariableTypeValueSetFieldSpec();

	virtual bool hasDefault() const;
	string getField() const {
		return fieldName;
	}
	void setDefault(ValueSetPtr valueSet) {
		defaultValueSet = valueSet;
	}
	virtual void EstablishFieldRelation(FieldSpecsList* specs);

	virtual void beginParseSetting(FieldSettingList*) const;
	virtual void endParseSetting() const;
	virtual int getToken() const;

	void printOn(ostream&) const;
	virtual void resolveReference() const;

  protected:
	string fieldName;
	ValueSetPtr defaultValueSet;
};

class DefinedObjectClass;
typedef shared_ptr<DefinedObjectClass> DefinedObjectClassPtr;
class ObjectFieldSpec : public FieldSpec {
  public:
	ObjectFieldSpec(const string& nam,
					DefinedObjectClass* oclass,
					bool optional = false);
	~ObjectFieldSpec();

	virtual bool hasDefault() const;
	string getField() const;
	void setDefault(InformationObjectPtr dftObj);
	virtual void beginParseSetting(FieldSettingList*) const;
	virtual void endParseSetting() const;
	virtual int getToken() const;

	void printOn(ostream&) const;
	virtual void resolveReference() const;
  protected:
	DefinedObjectClass* objectClass;
	InformationObjectPtr obj;
};

class ObjectSetFieldSpec : public FieldSpec {
  public:
	ObjectSetFieldSpec(const string& nam,  DefinedObjectClassPtr oclass,  bool optional = false);
	~ObjectSetFieldSpec();

	virtual bool hasDefault() const;
	void setDefault(ConstraintPtr dftObjSet) { objSet = dftObjSet; }
	string getField() const;

	virtual void beginParseSetting(FieldSettingList*) const;
	virtual void endParseSetting() const;
	virtual int getToken() const;

	void printOn(ostream&) const;
	virtual void resolveReference() const;
	virtual void FwdDeclare(ostream& hdr) const;
	virtual void generate_info_type_constructor(ostream& ) const;
	virtual void generate_info_type_memfun(ostream& hdr) const;
	virtual void generate_info_type_mem(ostream& ) const;
	virtual void generate_value_type(ostream& hdr) const;
	virtual void generateTypeField(const string& templatePrefix,
								   const string& classNameString,
								   const TypeBase* keyType,
								   const string& objClassName,
		ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
  protected:
	DefinedObjectClassPtr objectClass;
	ConstraintPtr objSet;
};


class DefinedSyntaxToken;

class TokenOrGroupSpec : public Printable {
  public:
	virtual ~TokenOrGroupSpec() {};
	virtual bool ValidateField(FieldSpecsList* ) = 0;
	virtual bool hasLiteral(const string& str) const = 0;
	enum MakeDefaultSyntaxResult {
		FAIL, // indicate the make process fail
		CONSUMED_AND_EXHAUSTED,
		CONSUMED_AND_NOT_EXHAUSTED,
		NOT_CONSUMED
	};
	virtual MakeDefaultSyntaxResult  MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* setting) = 0;
	virtual void preMakeDefaultSyntax(FieldSettingList* settings) = 0;
	virtual void cancelMakeDefaultSyntax(int no = 0) const = 0;
	virtual void Reset() {}
};

class Literal : public TokenOrGroupSpec {
  public:
	Literal(const string& str)
		: name(str ) { }
	Literal (const char* str): name(str) {}
	virtual ~Literal() {};
	void printOn(ostream& os) const;
	virtual bool ValidateField(FieldSpecsList* ) {
		return true;
	}
	virtual MakeDefaultSyntaxResult  MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* setting);
	virtual bool hasLiteral(const string& str) const {
		return str == name;
	}
	virtual void preMakeDefaultSyntax(FieldSettingList*) {};
	virtual void cancelMakeDefaultSyntax(int no = 0) const {}

  protected:
	string name;
};

class PrimitiveFieldName : public TokenOrGroupSpec {
  public:
	PrimitiveFieldName(const string& fieldname)
		: name(fieldname) {
	}
	PrimitiveFieldName(const char* fieldname) : name(fieldname) {}
	PrimitiveFieldName(const PrimitiveFieldName& other) {
		name = other.name;
		field = other.field;
	}
	~PrimitiveFieldName() {};
	void printOn(ostream&) const;
	virtual bool ValidateField(FieldSpecsList* fields);
	virtual MakeDefaultSyntaxResult  MakeDefaultSyntax(DefinedSyntaxToken* token, FieldSettingList* setting);
	virtual bool hasLiteral(const string&) const {
		return false;
	}
	virtual void preMakeDefaultSyntax(FieldSettingList* settings);
	virtual void cancelMakeDefaultSyntax(int no = 0) const;
  private:
	string name;
	FieldSpec* field;
};

typedef shared_ptr<TokenOrGroupSpec> TokenOrGroupSpecPtr;
typedef vector<TokenOrGroupSpecPtr> TokenOrGroupSpecList;

class TokenGroup : public TokenOrGroupSpec {
  public:
	TokenGroup() : optional(false), cursor(0) {}
	TokenGroup(const TokenGroup& other);
	~TokenGroup() {}
	void addToken(TokenOrGroupSpecPtr token) {
		tokenOrGroupSpecList.push_back(token);
	}
	void setOptional() {
		optional = true;
	}
	void printOn(ostream&) const;
	bool ValidateField(FieldSpecsList* fields);
	MakeDefaultSyntaxResult  MakeDefaultSyntax(DefinedSyntaxToken* token,	FieldSettingList* setting);
	virtual bool hasLiteral(const string& str) const;
	virtual void preMakeDefaultSyntax(FieldSettingList* settings);
	virtual void cancelMakeDefaultSyntax(int no = 0) const;

	size_t size() const {
		return tokenOrGroupSpecList.size();
	}
	TokenOrGroupSpec& operator[](size_t i) {
		return *(tokenOrGroupSpecList[i]);
	}
	virtual void Reset();
  private:
	TokenOrGroupSpecList tokenOrGroupSpecList;
	bool optional;
	size_t cursor;
};

typedef shared_ptr<TokenGroup> TokenGroupPtr;

class DefaultSyntaxBuilder {
  public:
	DefaultSyntaxBuilder(TokenGroupPtr tkGrp);
	~DefaultSyntaxBuilder();
	void addToken(DefinedSyntaxToken* token);
	auto_ptr<FieldSettingList> getDefaultSyntax();
	void ResetTokenGroup();
  private:
	TokenGroupPtr tokenGroup;
	auto_ptr<FieldSettingList> setting;
};

class ObjectClassBase : public Printable {
  public:
	ObjectClassBase() {};
	ObjectClassBase(const string& nam) {		setName(nam);	}
	virtual ~ObjectClassBase() {};

	void setName(const string& nam);
	virtual const string& getName() const {		return name;	}
	virtual const string& getCppName() const {		return cppname;	}
	virtual const string& getReferenceName() const = 0;

	int getFieldToken(const char* fieldname) const;
	virtual FieldSpec* getField(const string& fieldName) = 0;
	virtual const FieldSpec* getField(const string& fieldName) const = 0;
	virtual TypeBase* getFieldType(const string& fieldName) {
		return getField(fieldName)->getFieldType();
	}
	virtual const TypeBase* getFieldType(const string& fieldName) const {
		return getField(fieldName)->getFieldType();
	}
	virtual bool VerifyDefaultSyntax(FieldSettingList*) const = 0;
	virtual bool hasLiteral(const string& str) const = 0;
	virtual TokenGroupPtr getWithSyntax() const = 0;
	virtual void PreParseObject() const = 0;
	virtual void beginParseObject() const = 0;
	virtual void endParseObject() const = 0;
	virtual void beginParseObjectSet() const = 0;
	virtual void endParseObjectSet() const = 0;
	virtual void resolveReference() const = 0;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {}
	virtual const string& getKeyName() const = 0;
  protected:
	string cppname;
	string name;
};

class ObjectClassDefn;
typedef shared_ptr<ObjectClassDefn> ObjectClassDefnPtr;
typedef vector<ObjectClassBasePtr> ObjectClassesList;
class ObjectClassDefn : public ObjectClassBase {
  public:
	ObjectClassDefn();

	~ObjectClassDefn();

	void setFieldSpecs(auto_ptr<FieldSpecsList> list);
	void setWithSyntaxSpec(TokenGroupPtr list);

	FieldSpec* getField(const string& fieldName);
	const FieldSpec* getField(const string& fieldName) const;

	virtual bool VerifyDefaultSyntax(FieldSettingList*) const;
	virtual bool hasLiteral(const string& str) const {
		return withSyntaxSpec->hasLiteral(str);
	}
	virtual TokenGroupPtr getWithSyntax() const;
	virtual void PreParseObject() const;
	virtual void beginParseObject() const;
	virtual void endParseObject() const;
	virtual void beginParseObjectSet() const;
	virtual void endParseObjectSet() const;
	void printOn(ostream&) const;
	virtual void resolveReference() const;

	void ResolveKey();
	const string& getKeyName() const {
		return keyName;
	}
	virtual const string& getReferenceName() const {
		return name;
	}

	void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
  protected:
	auto_ptr<FieldSpecsList> fieldSpecs;
	TokenGroupPtr withSyntaxSpec;
	TypePtr keyType;
	string keyName;
};


class DefinedObjectClass : public ObjectClassBase {
  public:
	DefinedObjectClass(ObjectClassBase* ref);
	DefinedObjectClass(const string& nam, ObjectClassBase* ref = nullptr);
	~DefinedObjectClass() {}


	virtual const string& getReferenceName() const {		return referenceName;	}

	ObjectClassBase* getReference();
	const ObjectClassBase* getReference() const;
	FieldSpec* getField(const string& fieldName);
	const FieldSpec* getField(const string& fieldName) const;
	virtual bool VerifyDefaultSyntax(FieldSettingList*) const;
	bool hasLiteral(const string& str) const;
	virtual TokenGroupPtr getWithSyntax() const;
	virtual void PreParseObject() const;
	virtual void beginParseObject() const;
	virtual void endParseObject() const;
	virtual void beginParseObjectSet() const;
	virtual void endParseObjectSet() const;
	virtual void printOn(ostream& strm) const;
	virtual void resolveReference() const;
	virtual TypeBase* getFieldType(const string& fieldName);
	virtual const TypeBase* getFieldType(const string& fieldName) const;
	const string& getKeyName() const	{ return reference->getKeyName(); }
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
  protected:
	string referenceName;
	mutable ObjectClassBase* reference;
};

class ModuleDefinition;
typedef shared_ptr<ModuleDefinition> ModuleDefinitionPtr;
typedef vector<ModuleDefinitionPtr> ModuleList;

class ImportedObjectClass : public DefinedObjectClass {
  public:
	ImportedObjectClass(const string& modName, const string& nam,  ObjectClassBase* ref);
	const string& getModuleName() const {
		return moduleName;
	}
	const ModuleDefinitionPtr& getModule() const {
		return module;
	}
  private:
	string				moduleName;
	ModuleDefinitionPtr	module;
};

class Setting : public Printable {
  public:

	enum {
		has_type_setting = 0x01,
		has_value_setting = 0x02,
		has_valueSet_setting = 0x04,
		has_object_setting = 0x08,
		has_objectSet_setting = 0x10
	};
	virtual ~Setting() {};
	virtual void generateInfo(const string& , ostream&) {}
	virtual void generateCplusplus(const string& prefix, const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag) = 0;
	virtual void generateInitializationList(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) {}
	virtual bool isExtendable() const { 	return false;	}
	virtual void generateInstanceCode(const string& , ostream& ) const {}
};

class TypeSetting : public Setting {
  public:
	TypeSetting(TypePtr typeBase) {
		type = typeBase;
	}
	~TypeSetting() {}

	TypePtr getType() {
		return type;
	}
	const TypeBase* getType() const {
		return type.get();
	}
	void printOn(ostream& strm) const;
	virtual void generateCplusplus(const string& prefix, const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
	virtual void generateInfo(const string& name,ostream& hdr);
  protected:
	TypePtr type;
};


class ValueSetting : public Setting {
  public:
	ValueSetting(TypePtr typeBase, ValuePtr valueBase);
	~ValueSetting();

	ValuePtr getValue() {
		return value;
	}
	const ValuePtr getValue() const {
		return value;
	}
	TypePtr getType() {
		return type;
	}
	const TypeBase* getType() const {
		return type.get();
	}
	void printOn(ostream& strm) const;
	virtual void generateCplusplus(const string& prefix, const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
	virtual void generateInitializationList(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
  protected:
	TypePtr  type;
	ValuePtr value;
};

class ValueSetSetting : public Setting {
  public:
	ValueSetSetting(ValueSetPtr set);
	~ValueSetSetting();

	ValueSetPtr getValueSet() {
		return valueSet;
	}
	const ValueSetPtr getValueSet() const {
		return valueSet;
	}
	void printOn(ostream& strm) const;
	virtual void generateCplusplus(const string& prefix, const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
  protected:
	ValueSetPtr valueSet;
};

class InformationObject;
typedef shared_ptr<InformationObject> InformationObjectPtr;
class ObjectSetting : public Setting {
  public:
	ObjectSetting(InformationObjectPtr obj, ObjectClassBase* objClass);
	~ObjectSetting();
	InformationObjectPtr getObject() {
		return object;
	}
	const InformationObject* getObject() const {
		return object.get();
	}
	void printOn(ostream& strm) const;
	virtual void generateCplusplus(const string& prefix, const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
  protected:
	ObjectClassBase* objectClass;
	InformationObjectPtr object;
};

class ObjectSetSetting : public Setting {
  public:
	ObjectSetSetting(ConstraintPtr objSet, ObjectClassBase* objClass)
		: objectClass(objClass), objectSet(objSet) { }
	~ObjectSetSetting();
	ConstraintPtr getObjectSet() {
		return objectSet;
	}
	const ConstraintPtr getObjectSet() const {
		return objectSet;
	}
	void printOn(ostream&) const;
	virtual void generateCplusplus(const string& prefix, const string& name, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
	virtual void generateInfo(const string& name,ostream& hdr);
	virtual bool isExtendable() const {
		return objectSet->isExtendable();
	}
	void generateInstanceCode(const string& prefix, ostream& cxx) const;
  protected:
	ObjectClassBase* objectClass;
	ConstraintPtr objectSet;
};

class FieldSetting : public Printable {
  public:
	FieldSetting(const string& fieldname, auto_ptr<Setting> aSetting);
	~FieldSetting();

	const string& getName() const {
		return name;
	}
	Setting* getSetting() {
		return setting.get();
	}
	const Setting* getSetting() const {
		return setting.get();
	}
	void printOn(ostream&) const;

	bool isExtendable() const;
	void generateCplusplus(const string& prefix, ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl, unsigned& flag);
	void generateInitializationList(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	void generateInfo(ostream& hdr);
	void generateInstanceCode(const string& prefix, ostream& cxx) const;
  protected:
	string name;
	string identifier;
	auto_ptr<Setting> setting;
};


class InformationObject : public Printable {
  public:
	InformationObject() : parameters(nullptr) {}
	virtual ~InformationObject();
	void setName(const string& str) {
		name = str;
	}
	const string& getName() const {
		return name;
	}
	const string& getClassName() const;
	void PrintBase(ostream&) const;
	virtual bool setObjectClass(const ObjectClassBase* definedClass) = 0;
	virtual const ObjectClassBase* getObjectClass() const = 0;
	virtual const Setting* getSetting(const string& fieldname) const = 0;
	void setParameters(auto_ptr<ParameterList> list);
	virtual void generateCplusplus(ostream&, ostream&, ostream&) {}
	virtual bool isExtendable() const = 0;
	virtual void generateInstanceCode(ostream& cxx) const = 0;
  protected:
	virtual bool VerifyObjectDefinition() = 0;
	string name;
	auto_ptr<ParameterList> parameters;
};

typedef vector<InformationObjectPtr> InformationObjectList;

class DefinedObject : public InformationObject {
  public:
	DefinedObject(const string& name, const InformationObject* ref=nullptr);
	bool VerifyObjectDefinition();
	const InformationObject* getReference() const;
	bool setObjectClass(const ObjectClassBase* ) {
		return true;
	}
	const ObjectClassBase* getObjectClass() const;
	void printOn(ostream&) const;
	virtual const Setting* getSetting(const string& fieldname) const;
	virtual bool isExtendable() const {
		return reference->isExtendable();
	}
	virtual void generateInstanceCode(ostream& cxx) const;
  protected:
	string referenceName;
	mutable const InformationObject* reference;
};

class ImportedObject : public DefinedObject {
  public:
	ImportedObject(const string& modName, const string& name, const InformationObject* ref)
		: DefinedObject(name, ref), moduleName(modName) {}
	const string& getModuleName() const {
		return moduleName;
	}
	virtual void generateInstanceCode(ostream& cxx) const;
  private:
	string moduleName;
};

class DefaultObjectDefn : public InformationObject {
  public:
	DefaultObjectDefn(auto_ptr<FieldSettingList> list) : settings(list) {}
	~DefaultObjectDefn() { }
	bool VerifyObjectDefinition();

	bool setObjectClass(const ObjectClassBase* definedClass);
	const ObjectClassBase* getObjectClass() const {
		return referenceClass;
	}

	void printOn(ostream&) const;
	virtual const Setting* getSetting(const string& fieldname) const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr , ostream& cxx, ostream& inl);
	virtual bool isExtendable() const;
	virtual void generateInstanceCode(ostream& cxx) const;
  protected:
	auto_ptr<FieldSettingList> settings;
	const ObjectClassBase* referenceClass;
};

class ObjectFromObject : public InformationObject {
  public:
	ObjectFromObject(InformationObjectPtr referenceObj, const string& fld);
	~ObjectFromObject();
	void printOn(ostream&) const;
	virtual bool setObjectClass(const ObjectClassBase* ) {
		return true;
	}
	virtual const ObjectClassBase* getObjectClass() const;
	virtual const Setting* getSetting(const string& fieldname) const;
	virtual bool isExtendable() const {
		return refObj->isExtendable();
	}
	virtual void generateInstanceCode(ostream& cxx) const;
  protected:
	virtual bool VerifyObjectDefinition();
	InformationObjectPtr refObj;
	string field;
};

class DefinedSyntaxToken {
  public:
	virtual ~DefinedSyntaxToken() {};
	virtual bool MatchLiteral(const string&) {
		return false;
	}
	virtual FieldSettingPtr MatchSetting(const string&) {
		return FieldSettingPtr();
	}
	virtual bool isendSyntaxToken() {
		return false;
	}
};

class LiteralToken : public DefinedSyntaxToken {
  public:
	LiteralToken(const string& tokenName) : name(tokenName) { }
	virtual bool MatchLiteral(const string& literal) {
		return literal == name;
	}
  protected:
	string name;
};

class SettingToken : public DefinedSyntaxToken {
  public:
	SettingToken(auto_ptr<Setting> set) : setting(set) {}
	~SettingToken() { }
	virtual FieldSettingPtr MatchSetting(const string&);
  protected:
	auto_ptr<Setting> setting;
};

class endSyntaxToken : public DefinedSyntaxToken {
  public:
	endSyntaxToken() {}
	virtual bool isendSyntaxToken() {
		return true;
	}
};



class InformationObjectSet : public Printable {
  public:
	virtual ~InformationObjectSet() {}

	virtual const string& getName() const = 0;
	virtual const ObjectClassBase* getObjectClass() const = 0;
	virtual bool isExtendable() const = 0;

	virtual ValueSetPtr getValueSetFromValueField(const string& field) const = 0;
	virtual ValueSetPtr getValueSetFromValueSetField(const string& field) const = 0;
	virtual ConstraintPtr getObjectSetFromObjectField(const string& field) const = 0;
	virtual ConstraintPtr getObjectSetFromObjectSetField(const string& field) const = 0;
	virtual bool hasParameters() const = 0;
	virtual void generateInstanceCode(ostream& cxx) const = 0;
	virtual void generateType(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const = 0;
};

class InformationObjectSetDefn : public InformationObjectSet {
  public:
	InformationObjectSetDefn(const string& nam,
							 ObjectClassBasePtr objClass,
							 ConstraintPtr set,
							 ParameterListPtr list = ParameterListPtr() );
	~InformationObjectSetDefn();

	const string& getName() const {
		return name;
	}
	const ObjectClassBase* getObjectClass() const;

	virtual bool isExtendable() const {
		return rep->isExtendable();
	}
	ValueSetPtr getValueSetFromValueField(const string& field) const;
	ValueSetPtr getValueSetFromValueSetField(const string& field) const;
	ConstraintPtr getObjectSetFromObjectField(const string& field) const;
	ConstraintPtr getObjectSetFromObjectSetField(const string& field) const;
	bool hasParameters() const {
		return parameters.get() != nullptr;
	}
	void generateInstanceCode(ostream& cxx) const;
	void printOn(ostream&) const;
	virtual void generateType(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const;
	bool generateTypeConstructor(ostream& cxx) const;
  protected:
	string name;
	ObjectClassBasePtr objectClass;
	ConstraintPtr rep;
	ParameterListPtr parameters;
};

class ImportedObjectSet :  public InformationObjectSet {
  public:
	ImportedObjectSet(const string& modName, const InformationObjectSet* objSet) : reference(objSet), moduleName(modName) {}
	virtual const string& getName() const {
		return reference->getName();
	}
	virtual const ObjectClassBase* getObjectClass() const {
		return reference->getObjectClass();
	}
	const string& getModuleName() const {
		return moduleName;
	}

	virtual bool isExtendable() const {
		return reference->isExtendable();
	}
	virtual ValueSetPtr getValueSetFromValueField(const string& field) const {
		return reference->getValueSetFromValueField(field);
	}
	virtual ValueSetPtr getValueSetFromValueSetField(const string& field) const {
		return reference->getValueSetFromValueSetField(field);
	}
	virtual ConstraintPtr getObjectSetFromObjectField(const string& field) const {
		return reference->getObjectSetFromObjectField(field);
	}
	virtual ConstraintPtr getObjectSetFromObjectSetField(const string& field) const {
		return reference->getObjectSetFromObjectSetField(field);
	}
	virtual bool hasParameters() const {
		return reference->hasParameters();
	}
	void generateInstanceCode(ostream& ) const {}
	virtual void generateType(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl) const {}
	void printOn(ostream&) const {}
  private:
	const InformationObjectSet* reference;
	string moduleName;
};

typedef shared_ptr<InformationObjectSet> InformationObjectSetPtr;
typedef vector<InformationObjectSetPtr> InformationObjectSetList;
class ObjectSetConstraintElement : public ConstraintElementBase {
  public:
	virtual const ObjectClassBase* getObjectClass() const = 0;
	virtual string getName() const = 0;
	virtual void generateObjSetAccessCode(ostream& ) {}
};

class DefinedObjectSet : public ObjectSetConstraintElement {
  public:
	DefinedObjectSet(const string& ref);

	virtual const ObjectClassBase* getObjectClass() const;
	virtual string getName() const {
		return referenceName;
	}

	virtual ValueSetPtr getValueSetFromValueField(const string& field) const;
	virtual ValueSetPtr getValueSetFromValueSetField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectSetField(const string& field) const;
	virtual bool hasPERInvisibleConstraint(const Parameter& param) const;

	virtual const InformationObjectSet* getReference() const;

	void printOn(ostream&) const;
  protected:
	string referenceName;
	mutable const InformationObjectSet* reference;
};

class ParameterizedObjectSet : public DefinedObjectSet {
  public:
	ParameterizedObjectSet(const string& ref, ActualParameterListPtr args);
	~ParameterizedObjectSet();
	virtual string getName() const;

	void printOn(ostream&) const;
  protected:
	ActualParameterListPtr arguments;
};


class ObjectSetFromObject : public ObjectSetConstraintElement {
  public:
	ObjectSetFromObject(InformationObjectPtr obj, const string& fld);
	~ObjectSetFromObject();

	virtual const ObjectClassBase* getObjectClass() const;
	virtual string getName() const;

	virtual ValueSetPtr getValueSetFromValueField(const string& field) const;
	virtual ValueSetPtr getValueSetFromValueSetField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectSetField(const string& field) const;
	virtual bool hasPERInvisibleConstraint(const Parameter& param) const;

	void printOn(ostream&) const;
  protected:
	Constraint* getRepresentation() const;
	InformationObjectPtr refObj;
	string field;
	mutable ConstraintPtr rep;
};

class ObjectSetFromObjects : public ObjectSetConstraintElement {
  public:
	ObjectSetFromObjects(ObjectSetConstraintElementPtr objSet, const string& fld);
	~ObjectSetFromObjects();

	virtual const ObjectClassBase* getObjectClass() const;
	virtual string getName() const;

	virtual ValueSetPtr getValueSetFromValueField(const string& field) const;
	virtual ValueSetPtr getValueSetFromValueSetField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectSetField(const string& field) const;

	virtual bool hasPERInvisibleConstraint(const Parameter& param) const;
	void printOn(ostream&) const;
	virtual void generateObjSetAccessCode(ostream& );
  protected:
	ConstraintPtr getRepresentation() const;
	ObjectSetConstraintElementPtr refObjSet;
	string field;
	mutable ConstraintPtr rep;
};

class SingleObjectConstraintElement : public ConstraintElementBase {
  public:
	SingleObjectConstraintElement(InformationObjectPtr obj);
	~SingleObjectConstraintElement();

	virtual ValueSetPtr getValueSetFromValueField(const string& field) const;
	virtual ValueSetPtr getValueSetFromValueSetField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectField(const string& field) const;
	virtual ConstraintPtr getObjectSetFromObjectSetField(const string& field) const;
	virtual bool hasPERInvisibleConstraint(const Parameter& param) const;
	virtual void generateObjectSetInstanceCode(const string& prefix, ostream& cxx) const;
	void printOn(ostream&) const;
  protected:
	InformationObjectPtr object;
};

class ObjectSetType : public TypeBase {
  public:
	ObjectSetType(InformationObjectSetPtr objSet);
	virtual const char * getAncestorClass() const;
	virtual void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	virtual bool hasParameters() const;
	bool isPrimitiveType() const {	return false; }
  private:
	InformationObjectSetPtr objSet;
};

// ImportedSymbol
class ModuleDefinition;
class Symbol : public Printable {
  public:
	Symbol(const string& sym, bool param);
	bool isParameterisedImport() const {
		return parameterized;
	}
	virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to) = 0;
	virtual void AppendToModule(const string& , ModuleDefinition* ) {}
	virtual void generateUsingDirective(const string& , ostream& ) const {}
	virtual bool isType() const {
		return false;
	}
	virtual bool isValuesOrObjects() const {
		return false;
	}
	const string& getName() const {
		return name;
	}

	void printOn(ostream& strm) const;
  protected:
	string name;
	bool parameterized;
};

class TypeReference : public Symbol {
  public:
	TypeReference(const string& sym, bool param) : Symbol(sym,param) {}
	virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
	virtual void AppendToModule(const string& fromName, ModuleDefinition* to);
	virtual void generateUsingDirective(const string& moduleName, ostream& strm) const;
	virtual bool isType() const {
		return true;
	}
};

class ValueReference : public Symbol {
  public:
	ValueReference(const string& sym, bool param) : Symbol(sym,param) {}
	virtual bool isValuesOrObjects() const {
		return true;
	}
	virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
};

class ObjectClassReference : public Symbol {
  public:
	ObjectClassReference(const string& sym, bool param) : Symbol(sym,param) {}
	virtual void generateUsingDirective(const string& moduleName, ostream& strm) const;
	virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
};

class ObjectReference : public Symbol {
  public:
	ObjectReference(const string& sym, bool param) : Symbol(sym,param) {}
	virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
	virtual bool isValuesOrObjects() const {
		return true;
	}
};

class ObjectSetReference : public Symbol {
  public:
	ObjectSetReference(const string& sym, bool param) : Symbol(sym,param) {}
	virtual void generateUsingDirective(const string& moduleName, ostream& strm) const;
	virtual void AppendToModule(ModuleDefinition* from, ModuleDefinition* to);
	virtual bool isValuesOrObjects() const {
		return true;
	}
};
// ActualParameter

class ActualParameter : public Printable {
  public:
	virtual bool referencesType(const TypeBase& ) const {
		return false;
	}
	virtual bool useType(const TypeBase& ) const {
		return false;
	}
	virtual bool generateTemplateArgument(string& ) const {
		return false;
	}
};

class ActualTypeParameter : public ActualParameter {
  public:
	ActualTypeParameter(TypePtr type);
	~ActualTypeParameter();

	bool referencesType(const TypeBase& type) const;
	bool useType(const TypeBase& ) const;
	bool generateTemplateArgument(string& name) const;
	void printOn(ostream& strm) const;
  protected:
	TypePtr param;
};

class ActualValueParameter : public ActualParameter {
  public:
	ActualValueParameter(ValuePtr value);
	~ActualValueParameter();

	void printOn(ostream& strm) const;
  protected:
	ValuePtr param;
};

class ActualValueSetParameter : public ActualParameter {
  public:
	ActualValueSetParameter(TypePtr valueSetType);
	~ActualValueSetParameter();

	bool referencesType(const TypeBase& type) const;
	bool useType(const TypeBase& ) const;
	bool generateTemplateArgument(string& ) const;

	void printOn(ostream& strm) const;
  protected:
	TypePtr param;
};

class ActualObjectParameter : public ActualParameter {
  public:
	ActualObjectParameter(InformationObjectPtr obj);
	~ActualObjectParameter();
	bool generateTemplateArgument(string& name) const;
	virtual bool useType(const TypeBase& ) const;
	virtual bool referencesType(const TypeBase& type) const;

	void printOn(ostream& strm) const;
  protected:
	InformationObjectPtr param;
};

class ActualObjectSetParameter : public ActualParameter {
  public:
	ActualObjectSetParameter(shared_ptr<ObjectSetConstraintElement> objectSet);
	~ActualObjectSetParameter();
	bool generateTemplateArgument(string& name) const;
	virtual bool useType(const TypeBase& ) const;
	virtual bool referencesType(const TypeBase& type) const;

	void printOn(ostream& strm) const;
	virtual bool isTemplateArgument() const {
		return true;
	}
  protected:
	shared_ptr<ObjectSetConstraintElement> param;
};



typedef shared_ptr<Symbol> SymbolPtr;
typedef vector<SymbolPtr> SymbolList;

class ImportModule : public Printable {
	//PCLASSINFO(ImportModule, PObject);
  public:
	ImportModule(string * name, SymbolList * syms);
	void setFileName(const string& name) {
		filename = name;
	}
	const string& getFileName() const {
		return filename;
	}

	void printOn(ostream&) const;

	void generateCplusplus(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	void generateUsingDirectives(ostream& strm) const;
	void Adjust();

	const string& getName() const {
		return fullModuleName;
	}
	bool hasValuesOrObjects() const;
	string getCModuleName() const;
	const string& getLowerCaseName() const {
		return filename;
	}

  protected:
	string   fullModuleName;
	string   shortModuleName;
	SymbolList symbols;
	string   filename;
};

typedef shared_ptr<ImportModule> ImportModulePtr;
typedef vector<ImportModulePtr> ImportsList;


typedef map<string, TypePtr> TypeMap;

class ModuleDefinition : public Printable {
  public:
	ModuleDefinition(const string& name);
	ModuleDefinition(const string& name, const string& filePath, Tag::Mode defTagMode);
	~ModuleDefinition();

	void printOn(ostream&) const;

	Tag::Mode getDefaultTagMode() const {
		return defaultTagMode;
	}

	void addIdentifier(string* name, int idType);
	void addImportedIdentifiers(StringList& imports, const string& moduleName);
	int  getIdentifierType(const string& id);

	void setDefinitiveObjId(StringList& id);
	void setExportAll();
	void setExports(SymbolList& syms);

	void addImport(ImportModulePtr mod)  {
		imports.push_back(mod);
	}
	void addType(TypePtr type)      ;
	void addValue(ValuePtr val)      {
		values.push_back(val);
	}
	void addObjectClass(ObjectClassBasePtr oclass) {
		objectClasses.push_back(oclass);
	}
	void addInformationObject(InformationObjectPtr obj) {
		informationObjects.push_back(obj);
	}
	void addInformationObjectSet(InformationObjectSetPtr set) {
		informationObjectSets.push_back(set);
	}

	TypePtr findType(const string& name);
	bool  hasType(const string& name);
	const ValuesList& getValues() const {
		return values;
	}

	ValuePtr findValue(const string& name);
	ObjectClassBasePtr findObjectClass(const string& name);
	const InformationObject* findInformationObject(const string& name);
	const InformationObjectSet* findInformationObjectSet(const string& name);

	const string& getName() const {
		return moduleName;
	}
	const string& getCModuleName() const {
		return cppModuleName;
	}
	const string& getPrefix()     const {
		return classNamePrefix;
	}

	string getImportModuleName(const string& moduleName);

	int getIndentLevel() const {
		return indentLevel;
	}
	void setIndentLevel(int delta) {
		indentLevel += delta;
	}

	bool UsingInlines() const {
		return usingInlines;
	}

	virtual void generateCplusplus(const string& modName, unsigned numFiles, bool verbose);

	void ResolveObjectClassReferences() const;

	void AdjustModuleName(const string& sourcePath, bool isSubModule = false);
	bool ReorderTypes();
	string CreateSubModules(SymbolList& exportedSymbols);
	string getFileName();
	void AdjustImportedModules();


	bool isExported(const string& name) const;

	void generateClassModule(ostream& fwd, ostream& hdr, ostream& cxx, ostream& inl);
	void CreateObjectSetTypes();

	void addToRemoveList(const string& reference);
	void RemoveReferences(bool verbose);
	ImportModule* findImportedModule(const string& theModuleName);

	const string& getModulePath() const {
		return modulePath;
	}
	void dump() const;

  private:
	ModuleDefinition& operator = (const ModuleDefinition&);
	typedef map<string, string> PStringMap;
	const string				moduleName;
	const string				modulePath;
	string						classNamePrefix;
	bool						separateClassFiles;
	StringList					definitiveId;
	Tag::Mode					defaultTagMode;
	SymbolList					exports;
	bool						exportAll;
	ImportsList					imports;
	PStringMap					importNames;
	TypesVector					types;
	TypeMap						typeMap;
	ValuesList					values;
	int							indentLevel;
	bool						usingInlines;
	ObjectClassesList			objectClasses;
	InformationObjectList		informationObjects;
	InformationObjectSetList	informationObjectSets;
	map<string,int>				identifiers;
	string						shortModuleName;
	string						cppModuleName;
	string						generatedPath;
	ModuleList					subModules;
	vector<string>				removeList;
};

template <class T>
boost::shared_ptr<T> findWithName(const vector<boost::shared_ptr<T> >& cont, const string& name) {
	typedef vector<boost::shared_ptr<T> > Cont;

	typename Cont::const_iterator itr = cont.begin(), last = cont.end();
	for (; itr != last; ++itr)
		if ((*itr)->getName() == name)
			return *itr;
	return boost::shared_ptr<T>();

}
typedef stack<ObjectClassBase*> ClassStack;

extern ModuleList Modules;

#ifndef REENTRANT_PARSER
extern ModuleDefinition * Module;
extern ParameterList * DummyParameters;
extern ClassStack *classStack;
#endif


ModuleDefinition* findModule(const char* name);
ModuleDefinition* CreateModule(const char* name);
void addRemoveItem(const char* item);

class UsefulModuleDef;
class ModuleDefinition;

/* An opaque pointer. */
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

class ParserContext {
  public:
	ParserContext(FILE* file = nullptr);
	~ParserContext();

	yyscan_t				lexer;
	FILE*					file;
	ModuleDefinition *		Module			= nullptr;
	ClassStack *			classStack		= nullptr;
	ParameterList *			DummyParameters = nullptr;
	TypePtr					ValueTypeContext;
	vector<string>			RemoveList;
	int						IdentifierTokenContext;// = IDENTIFIER; chicken/egg problem
	int						ReferenceTokenContext /* = MODULEREFERENCE */;
	int						NullTokenContext;// = NULL_TYPE; chicken/egg problem
	int						BraceTokenContext				= '{';
	int						InOIDContext					= FALSE;
	int						InMacroContext					= FALSE;
	int						InMIBContext					= FALSE;
	int						InObjectSetContext				= 0;
	int						InWithSyntaxContext				= FALSE;
	int						hasObjectTypeMacro				= FALSE;
	const ObjectClassBase*	InformationFromObjectContext	= nullptr;
	int						ParsingConstructedType			= FALSE;
	int						InTopParser						= TRUE;
};

#endif
/*
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
 * flattening of CHOICE corrected
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
 *            add support for Information Object Class and generate code that follows
 *            X/Open ASN.1/C++ interface.
 *
 */
// *INDENT-ON*