%expect 14

%{

/*
 * asn_grammar.y
 *
 * ASN grammar file
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
 * $Log: asn_grammar.y,v $
 * Revision 1.4  2011/08/09 18:12:43  arunasr
 * Genesys fixes: 3.0 release candidate
 *
 *  /main/4 2009/10/13 15:51:26 BST arunasr
 *     UNCTime added; compiler warnings cleanup
 * Revision 1.4  2006/05/12 20:49:49  arunasr
 * UTCTime added
 *
 * Revision 1.3  2005/09/14 10:02:59  arunasr
 * BSTRING and HSTRING parsing corrected in BIT STRING context
 * Generation of named bits corrected
 *
 * Revision 1.2  2004/02/06 16:13:05  arunasr
 * Module exports uncommented so all exported types can be  generatedd
 * (otherwise only used types are generated)
 *
 * Revision 1.1.1.1  2002/11/05 14:07:03  arunasr
 * no message
 *
 * Revision 1.3  2002/07/02 02:03:25  mangelo
 * Remove Pwlib dependency
 *
 * Revision 1.2  2001/09/07 22:38:10  mangelo
 * add Log keyword substitution
 *
 *
 * March, 2001. Huang-Ming Huang
 *            Add support for Information Object Class.
 *
 */

#undef malloc
#undef calloc
#undef realloc
#undef free
#include <sstream>

#include "main.h"
#include "asn_grammar.hxx"



static int UnnamedFieldCount = 1;


static std::string * ConcatNames(std::string * s1, char c, std::string * s2)
{
  *s1 += c;*s1 += *s2;delete s2;return s1;
}

#ifdef _MSC_VER
#pragma warning(disable:4701)
#pragma warning(disable:4127)
#endif

#define YYERROR_VERBOSE 1
#define YYDEBUG 1
#define YYPRINT(a,b,c)

#ifdef REENTRANT_PARSER
extern int yylex(YYSTYPE* yylval_param, ParserContext* context, Environment* env);
extern int yyerror(ParserContext* context, Environment* env, const char* msg);
#else 
extern int yyparse();
void yyerror(const char* str);
extern void yyrestart(FILE*);
extern FILE * yyin;
extern FILE * idin;
extern int yydebug;
extern int iddebug;


#endif

%}
%define api.pure	full
%lex-param			{ParserContext* context}
%parse-param		{ParserContext* context}
%param				{Environment *env}

%token MODULEREFERENCE
%token TYPEREFERENCE
%token OBJECTCLASSREFERENCE
%token VALUEREFERENCE
%token OBJECTREFERENCE
%token OBJECTSETREFERENCE

%token PARAMETERIZEDTYPEREFERENCE
%token PARAMETERIZEDOBJECTCLASSREFERENCE
%token PARAMETERIZEDVALUEREFERENCE
%token PARAMETERIZEDOBJECTREFERENCE
%token PARAMETERIZEDOBJECTSETREFERENCE

%token VALUESET_BRACE
%token OBJECT_BRACE
%token OBJECTSET_BRACE

%token IDENTIFIER
%token BIT_IDENTIFIER
%token OID_IDENTIFIER
%token IMPORT_IDENTIFIER
%token fieldreference
%token FieldReference
%token TYPEFIELDREFERENCE
%token FIXEDTYPEVALUEFIELDREFERENCE
%token VARIABLETYPEVALUEFIELDREFERENCE
%token FIXEDTYPEVALUESETFIELDREFERENCE
%token VARIABLETYPEVALUESETFIELDREFERENCE
%token OBJECTFIELDREFERENCE
%token OBJECTSETFIELDREFERENCE
%token INTEGER

%token CSTRING
%token BSTRING
%token HSTRING
%token BS_BSTRING
%token BS_HSTRING


%token ABSENT
%token ABSTRACT_SYNTAX
%token ALL
%token ANY
%token APPLICATION
%token ASSIGNMENT
%token AUTOMATIC
%token BEGIN_t
%token BIT
%token BMPString
%token BOOLEAN_t
%token BY
%token CHARACTER
%token CHOICE
%token CLASS
%token COMPONENT
%token COMPONENTS
%token CONSTRAINED
%token DEFAULT
%token DEFINED
%token DEFINITIONS
%token EMBEDDED
%token END
%token ENUMERATED
%token EXCEPT
%token EXPLICIT
%token EXPORTS
%token EXTERNAL
%token FALSE_t
%token FROM
%token GeneralString
%token GraphicString
%token IA5String
%token TYPE_IDENTIFIER
%token IDENTIFIER_t
%token IMPLICIT
%token IMPORTS
%token INCLUDES
%token INSTANCE
%token INTEGER_t
%token INTERSECTION
%token ISO646String
%token MACRO
%token MAX
%token MIN
%token MINUS_INFINITY
%token NOTATION
%token NULL_VALUE
%token NULL_TYPE
%token NumericString
%token OBJECT
%token OCTET
%token OF_t
%token OPTIONAL_t
%token PDV
%token PLUS_INFINITY
%token PRESENT
%token PrintableString
%token PRIVATE
%token REAL
%token SEQUENCE
%token SET
%token SIZE_t
%token STRING
%token SYNTAX
%token T61String
%token TAGS
%token TeletexString
%token TRUE_t
%token TYPE_t
%token UNION
%token UNIQUE
%token UNIVERSAL
%token UniversalString
%token VideotexString
%token VisibleString
%token GeneralizedTime
%token UTCTime
%token VALUE
%token WITH

%token ObjectDescriptor_t
%token WORD_t
%token OID_INTEGER

%type <ival> INTEGER
%type <ival> TagDefault
%type <ival> SignedNumber
%type <ival> Class ClassNumber
%type <ival> PresenceConstraint
%type <modd> ModuleDefinition


%type <sval> CSTRING
%type <sval> BSTRING
%type <sval> HSTRING
%type <sval> BS_BSTRING
%type <sval> BS_HSTRING
%type <sval> IDENTIFIER
%type <sval> BIT_IDENTIFIER
%type <sval> OID_IDENTIFIER
%type <sval> IMPORT_IDENTIFIER
%type <sval> TYPEREFERENCE
%type <sval> MODULEREFERENCE
%type <sval> OBJECTCLASSREFERENCE
%type <sval> VALUEREFERENCE
%type <sval> fieldreference FieldReference
%type <sval> TYPEFIELDREFERENCE
%type <sval> VALUEFIELDREFERENCE
%type <sval> FIXEDTYPEVALUEFIELDREFERENCE
%type <sval> VARIABLETYPEVALUEFIELDREFERENCE
%type <sval> VALUESETFIELDREFERENCE
%type <sval> FIXEDTYPEVALUESETFIELDREFERENCE
%type <sval> VARIABLETYPEVALUESETFIELDREFERENCE
%type <sval> OBJECTFIELDREFERENCE
%type <sval> OBJECTSETFIELDREFERENCE
%type <sval> OBJECTREFERENCE
%type <sval> OBJECTSETREFERENCE
%type <sval> DefinitiveObjIdComponent
%type <sval> DefinitiveNameAndNumberForm
%type <sval> GlobalModuleReference
%type <sval> Reference
%type <sval> ExternalTypeReference ExternalValueReference
%type <sval> ObjIdComponent
%type <sval> NumberForm
%type <sval> SimpleDefinedType
%type <sval> CharsDefn
%type <sval> SimpleDefinedValue
%type <sval> FieldName PrimitiveFieldName
%type <sval> ExternalObjectClassReference
%type <sval> UsefulObjectClassReference
%type <sval> OID_INTEGER

%type <sval> PARAMETERIZEDTYPEREFERENCE
%type <sval> PARAMETERIZEDOBJECTCLASSREFERENCE
%type <sval> PARAMETERIZEDVALUEREFERENCE
%type <sval> PARAMETERIZEDOBJECTREFERENCE
%type <sval> PARAMETERIZEDOBJECTSETREFERENCE

%type <slst> DefinitiveIdentifier
%type <slst> DefinitiveObjIdComponentList
%type <slst> ObjIdComponentList
%type <slst> BitIdentifierList
%type <slst> CharSyms

%type <tval> Type BuiltinType ReferencedType NamedType
%type <tval> DefinedType
%type <tval> ConstrainedType
%type <tval> TypeWithConstraint
%type <tval> BitStringType
%type <tval> BooleanType
%type <tval> CharacterStringType
%type <tval> RestrictedCharacterStringType
%type <tval> UnrestrictedCharacterStringType
%type <tval> ChoiceType AlternativeTypeLists
%type <tval> EmbeddedPDVType
%type <tval> EnumeratedType Enumerations
%type <tval> ExternalType

%type <tval> AnyType
%type <tval> IntegerType
%type <tval> NullType
%type <tval> ObjectClassFieldType
%type <ocft> SimpleObjectClassFieldType
%type <tval> ObjectIdentifierType
%type <tval> OctetStringType
%type <tval> RealType
%type <tval> SequenceType ComponentTypeLists
%type <tptr> ComponentType
%type <tval> SequenceOfType
%type <tval> SetType
%type <tval> SetOfType
%type <tval> TaggedType
%type <tval> ParameterizedType
%type <tval> SelectionType
%type <tval> UsefulType
%type <tval> TypeFromObject
%type <tval> ContainedSubtype


%type <tlst> AlternativeTypeList
%type <tlst> ComponentTypeList

%type <symb> Symbol
%type <syml> SymbolList

%type <vval> Value BuiltinValue
%type <vval> AssignedIdentifier
%type <vval> DefinedValue
%type <vval> ObjectIdentifierValue
%type <vval> OctetStringValue
%type <vval> BitStringValue
%type <vval> ExceptionSpec
%type <vval> ExceptionIdentification
%type <vval> LowerEndpoint LowerEndValue UpperEndpoint UpperEndValue
%type <vval> ReferencedValue
%type <vval> BooleanValue
%type <vval> CharacterStringValue RestrictedCharacterStringValue
%type <vval> CharacterStringList Quadruple Tuple
%type <vval> ChoiceValue
%type <vval> NullValue
%type <vval> RealValue NumericRealValue SpecialRealValue
%type <vval> SequenceValue NamedValue
/*!!!! %type <vval> SequenceOfValue */
%type <vval> ParameterizedValue
%type <vval> ValueFromObject

%type <vlst> ComponentValueList

%type <nval> NamedBit
%type <nval> EnumerationItem
%type <nval> NamedNumber

%type <nlst> NamedBitList
%type <nlst> Enumeration
%type <nlst> NamedNumberList

%type <elmt> IntersectionElements
%type <elmt> Elements
%type <elmt> Exclusions
%type <elmt> SubtypeElements
%type <elmt> ObjectSetElements
%type <elmt> ValueRange
%type <elmt> PermittedAlphabet
%type <elmt> InnerTypeConstraints
%type <elmt> MultipleTypeConstraints
%type <elmt> SizeConstraint
%type <elmt> UserDefinedConstraintParameterList
%type <elmt> NamedConstraint

%type <elst> ElementSetSpec Unions Intersections TypeConstraints

%type <cons> Constraint
%type <cons> ConstraintSpec
%type <cons> ElementSetSpecs ObjectSetSpec
/* %type <cons> GeneralConstraint */
%type <cons> UserDefinedConstraint
%type <tcons> TableConstraint
%type <tcons> SimpleTableConstraint
%type <tcons> ComponentRelationConstraint
%type <tagv> Tag

%type <fspc> FieldSpec
%type <fspc> TypeFieldSpec
%type <fspc> FixedTypeValueFieldSpec
%type <fspc> VariableTypeValueFieldSpec
%type <fspc> FixedTypeValueSetFieldSpec
%type <fspc> VariableTypeValueSetFieldSpec
%type <fspc> ObjectFieldSpec
%type <fspc> ObjectSetFieldSpec

%type <flst> FieldSpecs

%type <objc> ObjectClass ObjectClassDefn
%type <dobj> DefinedObjectClass
%type <tgrp> WithSyntaxSpec SyntaxList TokenOrGroupSpecs
%type <togs> TokenOrGroupSpec RequiredToken OptionalGroup
%type <sval> Literal
%type <sett> Setting

%type <sval> ExternalObjectReference
%type <objt> Object
%type <objt> ObjectDefn
%type <objt> DefinedObject
%type <objt> ObjectFromObject
%type <objt> ReferencedObject ReferencedObjectDot
%type <objt> ParameterizedObject ObjectParameter

%type <fldl> FieldSettings
%type <fldl> DefaultSyntax
%type <bldr> DefinedSyntax
%type <bldr> DefinedSyntaxTokens
%type <tken> DefinedSyntaxToken

%type <sval> WORD_t

%type <vset> ValueSet
%type <vset> ValueSetFromObject ValueSetFromObjects

%type <sval> ExternalObjectSetReference
%type <cons> ObjectSet
%type <dos> DefinedObjectSet ParameterizedObjectSet
%type <osce> ReferencedObjects ReferencedObjectsDot
%type <osce> ObjectSetFromObject ObjectSetFromObjects
%type <osce> ObjectSetParameter

%type <para> Parameter
%type <plst> ParameterList Parameters
%type <apar> ActualParameter
%type <aplt> ActualParameterList ActualParameters
%type <apar> UserDefinedConstraintParameter
%type <aplt> UserDefinedConstraintParameters

%type <sval> AtNotation ComponentIdList
%type <slst> AtNotations

%union {
  boost::int64_t       ival;
  std::string	        * sval;
  StringList			* slst;
  TypeBase				* tval;
  TypePtr				* tptr;
  TypesVector			* tlst;
  ValueBase				* vval;
  ValuesList            * vlst;
  NamedNumber	        * nval;
  NamedNumberList       * nlst;
  Constraint            * cons;
  ConstraintElementVector * elst;
  ConstraintElementBase * elmt;
  FieldSpec             * fspc;
  FieldSpecsList		* flst;
  ObjectClassBase		* objc;
  DefinedObjectClass    * dobj;
  TokenGroup            * tgrp;
  TokenOrGroupSpec      * togs;
  Setting				* sett;
  InformationObject     * objt;
  InformationObjectSet  * objs;
  FieldSettingList      * fldl;
  DefaultSyntaxBuilder  * bldr;
  DefinedSyntaxToken    * tken;
  ValueSet              * vset;
  ObjectSetConstraintElement* osce;
  Symbol                * symb;
  SymbolList            * syml;
  Parameter             * para;
  ParameterList         * plst;
  ActualParameter       * apar;
  ActualParameterList   * aplt;
  TableConstraint       * tcons;
  ObjectClassFieldType  * ocft;
  DefinedObjectSet      * dos;
  ModuleDefinition*		modd;

  struct {
    Tag::Type tagClass;
    unsigned tagNumber;
  } tagv;
}
%printer { if ($$ != NULL) fprintf (yyoutput, "'%s'", $$->c_str()); } <sval>
%printer { if ($$ != NULL) fprintf (yyoutput, "'%s'", $$->GetName().c_str()); } <tval> <vval> <objt> <para> <symb> <fspc> <objc> <ocft>


%%

ModuleDefinitionList
	: ModuleDefinitionList ModuleDefinition
      {  }
	| ModuleDefinition
      {  }
;
ModuleDefinition
	: MODULEREFERENCE DefinitiveIdentifier DEFINITIONS TagDefault ASSIGNMENT BEGIN_t
		{
			context->Module = FindModule($1->c_str());
			if ($2) {
				context->Module->SetDefinitiveObjId(*$2); delete $2;
			}
		}
		ModuleBody END
		{
			context->Module->ResolveObjectClassReferences();
			// FIXME context->Module = NULL;
		}
;
DefinitiveIdentifier
  : '{' DefinitiveObjIdComponentList '}'
	{
	  $$ = $2;
	}
  | /* empty */
	{
	  $$ = NULL;
	}
  ;

DefinitiveObjIdComponentList
  : DefinitiveObjIdComponent
      {
		$$ = new StringList;
		$$->push_back($1->c_str());
		delete $1;
      }
  | DefinitiveObjIdComponent DefinitiveObjIdComponentList
      {
		$2->insert($2->begin(), std::string(*$1));
		$$ = $2;
      }
  ;

DefinitiveObjIdComponent
  : IDENTIFIER
  | INTEGER
      {
		char buf[10];
		sprintf(buf, "%u", static_cast<unsigned>($1));
		$$ = new std::string(buf);
      }
  | DefinitiveNameAndNumberForm
  ;

DefinitiveNameAndNumberForm
  : IDENTIFIER '(' INTEGER ')'
      {
		delete $1;
		char buf[10];
		sprintf(buf, "%u", static_cast<unsigned>($3));
		$$ = new std::string(buf);
      }
  ;

TagDefault
  : EXPLICIT TAGS
      {
		$$ = Tag::Explicit;
      }
  | IMPLICIT TAGS
      {
		$$ = Tag::Implicit;
      }
  | AUTOMATIC TAGS
      {
		$$ = Tag::Automatic;
      }
  | /* empty */
      {
		$$ = Tag::Explicit;
      }
  ;


/*************************************/

ModuleBody
  : Exports Imports AssignmentList
  | /* empty */
  ;


Exports
  : EXPORTS SymbolsExported ';'
  | /* empty */
  ;


SymbolsExported
  : SymbolList
      {
	context->Module->SetExports(*$1);
  delete $1;
      }
  | /* empty */
      {
	context->Module->SetExportAll();
      }
  ;


Imports
  : IMPORTS SymbolsImported ';'
  | /* empty */
  ;


SymbolsImported
  : SymbolsFromModuleList
  | /* empty */
  ;

SymbolsFromModuleList
  : SymbolsFromModule
  | SymbolsFromModuleList SymbolsFromModule
  ;


SymbolsFromModule
  : SymbolList FROM GlobalModuleReference
      {
		if (!context->HasObjectTypeMacro) {
		  context->HasObjectTypeMacro = FindWithName(*$1,"OBJECT-TYPE").get() != NULL;
		  if (context->HasObjectTypeMacro)
			std::cerr << "Info: including OBJECT-TYPE macro" << std::endl;
		}
		context->Module->AddImport(ImportModulePtr(new ImportModule($3, $1)));
      }
  ;


GlobalModuleReference
  : MODULEREFERENCE
      {
		context->InOIDContext = 1;
      }
		AssignedIdentifier
      {
		context->InOIDContext = 0;
		delete $3;
      }
  ;


AssignedIdentifier
  : DefinedValue
  | ObjectIdentifierValue
  | /* empty */
      {
	$$ = NULL;
      }
  ;


SymbolList
  : Symbol
      {
	$$ = new SymbolList;
	$$->push_back(SymbolPtr($1));
      }
  | Symbol ',' SymbolList
      {
	$3->push_back(SymbolPtr($1));
	$$ = $3;
      }
  ;


Symbol
  : 
    TYPEREFERENCE 
      {
	$$ = new TypeReference(*$1, false); delete $1;
      }
  | VALUEREFERENCE 
      {
    $$ = new ValueReference(*$1, false); delete $1;
	  }
  | OBJECTCLASSREFERENCE 
      {
    $$ = new ObjectClassReference(*$1, false); delete $1;
	  }
  | OBJECTREFERENCE 
      {
    $$ = new ObjectReference(*$1, false); delete $1;
	  }
  | OBJECTSETREFERENCE 
      {
    $$ = new ObjectSetReference(*$1, false); delete $1;
	  }
  | PARAMETERIZEDTYPEREFERENCE '{' '}'
      {
	$$ = new TypeReference(*$1, true); delete $1;
      }
  | PARAMETERIZEDVALUEREFERENCE '{' '}'
      {
    $$ = new ValueReference(*$1, true); delete $1;
	  }
  | PARAMETERIZEDOBJECTCLASSREFERENCE '{' '}'
      {
    $$ = new ObjectClassReference(*$1, true); delete $1;
	  }
  | PARAMETERIZEDOBJECTREFERENCE '{' '}'
      {
    $$ = new ObjectReference(*$1, true); delete $1;
	  }
  | PARAMETERIZEDOBJECTSETREFERENCE '{' '}'
      {
    $$ = new ObjectSetReference(*$1, true); delete $1;
	  }
;

AssignmentList: Assignment
  | AssignmentList Assignment
  ;


Assignment
  : TypeAssignment
  | ValueAssignment
  | ValueSetTypeAssignment
  | ObjectClassAssignment
  | ObjectAssignment
  | ObjectSetAssignment
  | ParameterizedAssignment
  ;


ValueSetTypeAssignment
  : TYPEREFERENCE Type
      {
	$2->SetName(*$1); delete $1;
    context->ValueTypeContext.reset($2);
	$2->BeginParseValueSet();
      }
    ASSIGNMENT ValueSet
      {
	context->Module->AddType($5->MakeValueSetType());
	delete $5;
      }
  ;




/********/

TypeAssignment
  : TYPEREFERENCE ASSIGNMENT Type
      {
	$3->SetName(*$1); delete $1;
	context->Module->AddType(TypePtr($3));
      }
  ;


Type
  : ConstrainedType
  | ReferencedType
  | BuiltinType
  ;


BuiltinType
  : BitStringType
  | BooleanType
  | CharacterStringType
  | ChoiceType
  | EmbeddedPDVType
  | EnumeratedType
  | ExternalType
  | AnyType
  | InstanceOfType
    { }
  | IntegerType
  | NullType
  | ObjectClassFieldType
  | ObjectIdentifierType
  | OctetStringType
  | RealType
  | SequenceType
  | SequenceOfType
  | SetType
  | SetOfType
  | TaggedType
  ;


ReferencedType
  : DefinedType
  | UsefulType
  | SelectionType
  | TypeFromObject
  | ValueSetFromObjects
    {
  $$= $1->MakeValueSetType().get();
  delete $1;
    }
  ;


DefinedType
  : SimpleDefinedType
      {
	$$ = new DefinedType(*$1);
	delete $1;
      }
  | ParameterizedType
/*| ParameterizedValueSetType	synonym for ParameterizedType */
  ;


ExternalTypeReference
  : MODULEREFERENCE '.' TYPEREFERENCE
      {
	*$1 += *$3;
	delete $3;
      }
  ;

BitStringType
  : BIT STRING
      {
	$$ = new BitStringType;
      }
  | BIT STRING '{' NamedBitList '}'
      {
	$$ = new BitStringType(*$4);
	delete $4;
      }
  ;


NamedBitList
  : NamedBit
      {
	$$ = new NamedNumberList;
	$$->push_back(NamedNumberPtr($1));
      }
  | NamedBitList ',' NamedBit
      {
	$1->push_back(NamedNumberPtr($3));
      }
  ;

NamedBit
  : IDENTIFIER '(' INTEGER ')'
      {
	$$ = new NamedNumber($1, (int)$3);
      }
  | IDENTIFIER '(' DefinedValue ')'
      {
	$$ = new NamedNumber($1, ((DefinedValue*)$3)->GetReference());
	delete $3;
      }
  ;


BooleanType
  : BOOLEAN_t
      {
	$$ = new BooleanType;
      }
  ;


CharacterStringType
  : RestrictedCharacterStringType
  | UnrestrictedCharacterStringType
  ;

RestrictedCharacterStringType
  : BMPString
      {
	$$ = new BMPStringType;
      }
  | GeneralString
      {
	$$ = new GeneralStringType;
      }
  | GraphicString
      {
	$$ = new GraphicStringType;
      }
  | IA5String
      {
	$$ = new IA5StringType;
      }
  | ISO646String
      {
	$$ = new ISO646StringType;
      }
  | NumericString
      {
	$$ = new NumericStringType;
      }
  | PrintableString
      {
	$$ = new PrintableStringType;
      }
  | TeletexString
      {
	$$ = new TeletexStringType;
      }
  | T61String
      {
	$$ = new T61StringType;
      }
  | UniversalString
      {
	$$ = new UniversalStringType;
      }
  | VideotexString
      {
	$$ = new VideotexStringType;
      }
  | VisibleString
      {
	$$ = new VisibleStringType;
      }
  ;


UnrestrictedCharacterStringType
  : CHARACTER STRING
      {
	$$ = new UnrestrictedCharacterStringType;
      }
  ;


ChoiceType
  : CHOICE '{'
      {
		context->ParsingConstructedType++;
	  }
    AlternativeTypeLists '}'
      {
	$$ = $4;
    context->ParsingConstructedType--;
      }
  ;

AlternativeTypeLists
  : AlternativeTypeList
      {
	$$ = new ChoiceType($1);
      }
  | AlternativeTypeList ',' ExtensionAndException
      {
	$$ = new ChoiceType($1, true);
      }
  | AlternativeTypeList ',' ExtensionAndException  ','  AlternativeTypeList
      {
	$$ = new ChoiceType($1, true, $5);
      }
  ;

AlternativeTypeList
  : NamedType
      {
	$$ = new TypesVector;
	$$->push_back(TypePtr($1));
      }
  | AlternativeTypeList ',' NamedType
      {
	$1->push_back(TypePtr($3));
      }
  ;


ExtensionAndException
  : '.' '.' '.' ExceptionSpec
  ;


NamedType
  : IDENTIFIER Type
      {
	$2->SetName(*$1); delete $1;
	$$ = $2;
      }
  | Type	     /* ITU-T Rec. X.680 Appendix H.1 */
      {
	std::cerr << StdError(Warning) << "unnamed field." << std::endl;
	std::stringstream strm;
	strm << "_unnamed" << UnnamedFieldCount++<< std::ends;
	$1->SetName(strm.str());
      }
/*| SelectionType    /* Unnecessary as have rule in Type for this */
  ;


EmbeddedPDVType
  : EMBEDDED PDV
      {
	$$ = new EmbeddedPDVType;
      }
  ;


EnumeratedType
  : ENUMERATED '{' Enumerations '}'
      {
	$$ = $3;
      }
  ;

Enumerations
  : Enumeration
      {
	$$ = new EnumeratedType(*$1, false, NULL);
	delete $1;
      }
  | Enumeration  ',' '.' '.' '.'
      {
	$$ = new EnumeratedType(*$1, true, NULL);
	delete $1;
      }
  | Enumeration  ',' '.' '.' '.' ',' Enumeration
      {
	$$ = new EnumeratedType(*$1, true, $7);
	delete $1;
      }
  ;

Enumeration
  : EnumerationItem
      {
	$$ = new NamedNumberList;
	$$->push_back(NamedNumberPtr($1));
      }
  | Enumeration ',' EnumerationItem
      {
	$3->SetAutoNumber(*($1->back()));
	$1->push_back(NamedNumberPtr($3));
	$$ = $1;
      }
  ;

EnumerationItem
  : IDENTIFIER
      {
	$$ = new NamedNumber($1);
      }
  | NamedNumber
  ;

ExternalType
  : EXTERNAL
      {
	$$ = new ExternalType;
      }
  ;

AnyType
  : ANY
      {
	$$ = new AnyType(NULL);
      }
  | ANY DEFINED BY IDENTIFIER
      {
	$$ = new AnyType(*$4);
	delete $4;
      }
  ;


InstanceOfType
  : INSTANCE OF_t DefinedObjectClass
  ;


IntegerType
  : INTEGER_t
      {
	$$ = new IntegerType;
      }
  | INTEGER_t '{' NamedNumberList '}'
      {
	$$ = new IntegerType(*$3); delete $3;
      }
  ;


NullType
  : NULL_TYPE
      {
	$$ = new NullType;
      }
  ;


ObjectClassFieldType
  : SimpleObjectClassFieldType
      {
  $$  = $1;
	  }
  | SimpleObjectClassFieldType '(' TableConstraint ExceptionSpec ')'
	  {
  $1->AddTableConstraint(boost::shared_ptr<TableConstraint>($3));
  $$ = $1;
	  }
  | SimpleObjectClassFieldType '(' ConstraintSpec ExceptionSpec ')'
      {
  $1->AddConstraint(ConstraintPtr($3));
  $$ = $1;
	  }
  ;
SimpleObjectClassFieldType
  : DefinedObjectClass
      {
    context->InformationFromObjectContext = $1;
	  }
	'.' FieldName
      {
	$$ = new ObjectClassFieldType(ObjectClassBasePtr($1), *$4); delete $4;
	context->InformationFromObjectContext = NULL;
      }
  ;


ObjectIdentifierType
  : OBJECT IDENTIFIER_t
      {
	$$ = new ObjectIdentifierType;
      }
  ;

OctetStringType
  : OCTET STRING
      {
	$$ = new OctetStringType;
      }
  ;


RealType
  : REAL
      {
	$$ = new RealType;
      }
  ;


SequenceType
  : SequenceAndBrace ComponentTypeLists '}'
	  {
	$$ = $2;
    context->ParsingConstructedType--;
      }
  | SequenceAndBrace  '}'
      {
	$$ = new SequenceType(NULL, false, NULL);
    context->ParsingConstructedType--;
      }
  | SequenceAndBrace ExtensionAndException '}'
      {
	$$ = new SequenceType(NULL, true, NULL);
    context->ParsingConstructedType--;
      }
  ;

SequenceAndBrace
  : SEQUENCE
	  {
    context->ParsingConstructedType++;
	  }
    '{'
  ;

ComponentTypeLists
  : ComponentTypeList
      {
	$$ = new SequenceType($1, false, NULL);
      }
  | ComponentTypeList ',' ExtensionAndException
      {
	$$ = new SequenceType($1, true, NULL);
      }
  | ComponentTypeList ',' ExtensionAndException ',' ComponentTypeList
      {
	$$ = new SequenceType($1, true, $5);
      }
  | ExtensionAndException ',' ComponentTypeList
      {
	$$ = new SequenceType(NULL, true, $3);
      }
  ;

ComponentTypeList
  : ComponentType
      {
	$$ = new TypesVector;
	$$->push_back(*$1);
	delete $1;
      }
  | ComponentTypeList ',' ComponentType
      {
	$1->push_back(*($3));
	delete $3;
      }
  ;

ComponentType
  : NamedType
      {
    $$ = new TypePtr($1);
      }
  | NamedType OPTIONAL_t
      {
	$1->SetOptional();
	$$ = new TypePtr($1);
      }
  | NamedType
      {
   	context->ValueTypeContext.reset($1);
    $1->BeginParseValue();
	  }
	DEFAULT Value
      {
   $1->SetDefaultValue(ValuePtr($4));
   $$ = new TypePtr(context->ValueTypeContext);
   $1->EndParseValue();
      }
  | COMPONENTS OF_t Type
      {
    $$ = new TypePtr($3);
      }
  ;


SequenceOfType
  : SEQUENCE OF_t Type
      {
	$$ = new SequenceOfType(TypePtr($3));
      }
  ;


SetType
  : SetAndBrace ComponentTypeLists '}'
      {
	$$ = new SetType(*(SequenceType*)$2); delete $2;
    context->ParsingConstructedType--;
      }
  | SetAndBrace  '}'
      {
	$$ = new SetType;
    context->ParsingConstructedType--;
      }
  ;

SetAndBrace
  : SET
      {
    context->ParsingConstructedType++;
	  }
    '{'
  ;


SetOfType
  : SET OF_t Type
      {
	$$ = new SetOfType(TypePtr($3));
      }
  ;


TaggedType
  : Tag Type
      {
	$2->SetTag($1.tagClass, $1.tagNumber, context->Module->GetDefaultTagMode());
	$$ = $2;
      }
  | Tag IMPLICIT Type
      {
	$3->SetTag($1.tagClass, $1.tagNumber, Tag::Implicit);
	$$ = $3;
      }
  | Tag EXPLICIT Type
      {
	$3->SetTag($1.tagClass, $1.tagNumber, Tag::Explicit);
	$$ = $3;
      }
  ;

Tag
  : '[' Class ClassNumber ']'
      {
	$$.tagClass = (Tag::Type)$2;
	$$.tagNumber = (int)$3;
      }
  ;

ClassNumber
  : INTEGER
  | DefinedValue
      {
    IntegerValue* val = dynamic_cast<IntegerValue*>($1);
	if (val) {
	  $$ = *val;
	  delete $1;
	}
	else
	  std::cerr << StdError(Fatal) << "incorrect value type." << std::endl;
      }
  ;

Class
  : UNIVERSAL
      {
	$$ = Tag::Universal;
      }
  | APPLICATION
      {
	$$ = Tag::Application;
      }
  | PRIVATE
      {
	$$ = Tag::Private;
      }
  | /* empty */
      {
	$$ = Tag::ContextSpecific;
      }
  ;


SelectionType
  : IDENTIFIER '<' Type
      {
	$$ = new SelectionType(*$1, TypePtr($3)); delete $1;
      }
  ;


UsefulType
  : GeneralizedTime
      {
	$$ = new GeneralizedTimeType;
      }
  | UTCTime
      {
	$$ = new UTCTimeType;
      }
  | ObjectDescriptor_t
      {
	$$ = new ObjectDescriptorType;
      }
  ;


TypeFromObject
  : ReferencedObjectDot TYPEFIELDREFERENCE
    {
	  $$ = new TypeFromObject(InformationObjectPtr($1), *$2); delete $2;
	  context->InformationFromObjectContext = NULL;
	}
  ;


ValueSetFromObjects
  : ReferencedObjectsDot FIXEDTYPEVALUEFIELDREFERENCE
   {
	  $$ = new ValueSetFromObjects(ObjectSetConstraintElementPtr($1), *$2); delete $2;
	  context->InformationFromObjectContext = NULL;
   }
  | ReferencedObjectsDot FIXEDTYPEVALUESETFIELDREFERENCE
   {
	  $$ = new ValueSetFromObjects(ObjectSetConstraintElementPtr($1), *$2); delete $2;
	  context->InformationFromObjectContext = NULL;
   }
  | ValueSetFromObject
  ;

ObjectSetFromObjects
  : ReferencedObjectsDot OBJECTFIELDREFERENCE
    {
	  $$ = new ObjectSetFromObjects(ObjectSetConstraintElementPtr($1), *$2); delete $2;
	  context->InformationFromObjectContext = NULL;
	}
  | ReferencedObjectsDot OBJECTSETFIELDREFERENCE
    {
	  $$ = new ObjectSetFromObjects(ObjectSetConstraintElementPtr($1), *$2); delete $2;
	  context->InformationFromObjectContext = NULL;
	}
  | ObjectSetFromObject
  ;


ReferencedObjects
  : DefinedObjectSet
      {
  $$ = $1;
	  }
  | ObjectSetFromObjects
      {
  $$ = $1;
	  }
  | ParameterizedObjectSet
      {
  $$ = $1;
	  }
  ;

ReferencedObject
  : DefinedObject
  | ObjectFromObject
  | ParameterizedObject
  ;

ParameterizedObject
  : PARAMETERIZEDOBJECTREFERENCE ActualParameterList
      {
	  }
  | MODULEREFERENCE '.' PARAMETERIZEDOBJECTREFERENCE ActualParameterList
      {
	  }
  ;


/********/

ConstrainedType
  : Type Constraint
      {
	$1->AddConstraint(ConstraintPtr($2));
      }
  | TypeWithConstraint
  ;


TypeWithConstraint
  : SET Constraint OF_t Type
      {
	$$ = new SetOfType(TypePtr($4), ConstraintPtr($2));
      }
  | SET SizeConstraint OF_t Type
      {
	ConstraintElementPtr cons($2);
	$$ = new SetOfType(TypePtr($4), ConstraintPtr(new Constraint(cons)));
      }
  | SEQUENCE Constraint OF_t Type
      {
	$$ = new SequenceOfType(TypePtr($4), ConstraintPtr($2));
      }
  | SEQUENCE SizeConstraint OF_t Type
      {
	ConstraintElementPtr cons($2);
	$$ = new SequenceOfType(TypePtr($4), ConstraintPtr(new Constraint(cons)));
      }
  ;

Constraint
  : '(' ConstraintSpec ExceptionSpec ')'
      {
	$$ = $2;
      }
  ;

ConstraintSpec
  : ElementSetSpecs
  | UserDefinedConstraint /* GeneralConstraint */
  ;


ExceptionSpec
  : '!' ExceptionIdentification
      {
	$$ = $2;
      }
  | /* empty */
      {
	$$ = NULL;
      }
  ;


ExceptionIdentification
  : SignedNumber
      {
	$$ = new IntegerValue($1);
      }
  | DefinedValue
  | Type
      {
   	context->ValueTypeContext.reset($1);
    $1->BeginParseValue();
	  }
    ':' Value
      {
	$1->EndParseValue();
	$$ = $4;
      }
  ;


ElementSetSpecs
  : ElementSetSpec
      {
	$$ = new Constraint(std::auto_ptr<ConstraintElementVector>($1), false);
      }
  | ElementSetSpec  ',' '.' '.' '.'
      {
	$$ = new Constraint(std::auto_ptr<ConstraintElementVector>($1), true);
      }
  | '.' '.' '.' ',' ElementSetSpec
      {
	$$ = new Constraint(std::auto_ptr<ConstraintElementVector>(),
						true,
						std::auto_ptr<ConstraintElementVector>($5));
      }
  | ElementSetSpec  ',' '.' '.' '.' ',' ElementSetSpec
      {
	$$ = new Constraint(std::auto_ptr<ConstraintElementVector>($1),
						true,
						std::auto_ptr<ConstraintElementVector>($7));
      }
  ;


ElementSetSpec
  : Unions
  | ALL Exclusions
      {
    $$ = new ConstraintElementVector;
	$$->push_back(ConstraintElementPtr(
				new ConstrainAllConstraintElement(
					ConstraintElementPtr($2))));
      }
  ;


Unions
  : Intersections
      {
	if ($1->size() == 1)
	  $$= $1;
	else
	{
	  $$ = new ConstraintElementVector;
	  $$->push_back(ConstraintElementPtr(new ElementListConstraintElement(
												std::auto_ptr<ConstraintElementVector>($1))));
	}
      }
  | Unions UnionMark Intersections
      {
    if ($3->size() == 1)
	{
	  $$->push_back( (*$3)[0]);
	  delete $3;
	}
	else
	  $$->push_back(ConstraintElementPtr(new ElementListConstraintElement(
				std::auto_ptr<ConstraintElementVector>($3))));
      }
  ;

Intersections
  : IntersectionElements
      {
	$$ = new ConstraintElementVector;
	$$->push_back(ConstraintElementPtr($1));

      }
  | Intersections IntersectionMark IntersectionElements
      {
   	$$->push_back(ConstraintElementPtr($3));
      }
  ;

IntersectionElements
  : Elements
  | Elements Exclusions
      {
	$1->SetExclusions(ConstraintElementPtr($2));
      }
  ;

Exclusions
  : EXCEPT Elements
      {
	$$ = $2;
      }
  ;

UnionMark
  : '|'
  | UNION
  ;

IntersectionMark
  : '^'
  | INTERSECTION
  ;

Elements
  : SubtypeElements
  | ObjectSetElements
  | '(' ElementSetSpec ')'
      {
	$$ = new ElementListConstraintElement(std::auto_ptr<ConstraintElementVector>($2));
      }
  ;


SubtypeElements
  : Value
      {
	$$ = new SingleValueConstraintElement(ValuePtr($1));
      }
  | ContainedSubtype
      {
	$$ = new SubTypeConstraintElement(TypePtr($1));
      }
  | ValueRange
  | PermittedAlphabet
  | SizeConstraint
/*| TypeConstraint  This is really Type and causes ambiguity with ContainedSubtype */
  | InnerTypeConstraints
  ;

ValueRange
  : LowerEndpoint '.' '.' UpperEndpoint
      {
	$$ = new ValueRangeConstraintElement(ValuePtr($1), ValuePtr($4));
      }
  ;

LowerEndpoint
  : LowerEndValue
  | LowerEndValue '<'
  ;

UpperEndpoint
  : UpperEndValue
  | '<' UpperEndValue
      {
	$$ = $2;
      }
  ;

LowerEndValue
  : Value
  | MIN
      {
	$$ = new MinValue;
      }
  ;

UpperEndValue
  : Value
  | MAX
      {
	$$ = new MaxValue;
      }
  ;

PermittedAlphabet
  : FROM Constraint
      {
	$$ = new FromConstraintElement(ConstraintPtr($2));
      }
  ;

ContainedSubtype
  : INCLUDES Type
      {
	$$ = $2;
      }
/*| Type	 Actual grammar has INCLUDES keyword optional but this is
		 horribly ambiguous, so only support  a few specific Type
		 definitions */
  | ConstrainedType
  | BuiltinType
  | ReferencedType
  /*
  | DefinedType
  | UsefulType
  */
  ;


SizeConstraint
  : SIZE_t Constraint
      {
	$$ = new SizeConstraintElement(ConstraintPtr($2));
      }
  ;


InnerTypeConstraints
  : WITH COMPONENT Constraint
      {
	$$ = new WithComponentConstraintElement(NULL, ConstraintPtr($3), WithComponentConstraintElement::Default);
      }
  | WITH COMPONENTS MultipleTypeConstraints
      {
	$$ = $3;
      }
  ;

MultipleTypeConstraints
  : '{' TypeConstraints '}'			/* FullSpecification */
      {
	$$ = new InnerTypeConstraintElement(std::auto_ptr<ConstraintElementVector>($2), false);
      }
  | '{'  '.' '.' '.' ',' TypeConstraints '}'	/* PartialSpecification */
      {
	$$ = new InnerTypeConstraintElement(std::auto_ptr<ConstraintElementVector>($6), true);
      }
  ;

TypeConstraints
  : NamedConstraint
      {
    $$ = new ConstraintElementVector;
	$$->push_back(ConstraintElementPtr($1));
      }
  | NamedConstraint ',' TypeConstraints
      {
	$3->push_back(ConstraintElementPtr($1));
	$$ = $3;
      }
  ;

NamedConstraint
  : IDENTIFIER PresenceConstraint
      {
	$$ = new WithComponentConstraintElement(*$1, ConstraintPtr(), (int)$2); delete $1;
      }
  | IDENTIFIER Constraint PresenceConstraint
      {
	$$ = new WithComponentConstraintElement(*$1, ConstraintPtr($2), (int)$3); delete $1;
      }
  ;

PresenceConstraint
  : PRESENT
      {
	$$ = WithComponentConstraintElement::Present;
      }
  | ABSENT
      {
	$$ = WithComponentConstraintElement::Absent;
      }
  | OPTIONAL_t
      {
	$$ = WithComponentConstraintElement::Optional;
      }
  | /* empty */
      {
	$$ = WithComponentConstraintElement::Default;
      }
  ;


/*
GeneralConstraint
  : UserDefinedConstraint
  | TableConstraint
  ;
*/

UserDefinedConstraint
  : CONSTRAINED BY '{' UserDefinedConstraintParameterList '}'
    {
	  ConstraintElementPtr cons($4);
      $$ = new Constraint(cons);
    }
  ;

UserDefinedConstraintParameterList
  : /* empty */
      {
	$$ = new UserDefinedConstraintElement(ActualParameterListPtr());
      }
  | UserDefinedConstraintParameters
      {
	$$ = new UserDefinedConstraintElement(ActualParameterListPtr($1));
      }
  ;

UserDefinedConstraintParameters
  : UserDefinedConstraintParameter ',' UserDefinedConstraintParameters
      {
	$3->push_back(ActualParameterPtr($1));
	$$ = $3;
      }
  | UserDefinedConstraintParameter
      {
	$$ = new ActualParameterList;
	$$->push_back(ActualParameterPtr($1));
      }
  ;

UserDefinedConstraintParameter
  : Governor ':' ActualParameter
      {
	$$ = $3;
      }
  | ActualParameter
  ;

Governor
  : Type
      {
    delete $1;
      }
  | DefinedObjectClass
      {
    delete $1;
	  }

TableConstraint
  : SimpleTableConstraint
  | ComponentRelationConstraint
  ;

SimpleTableConstraint
  : '{' DefinedObjectSet '}' /* '{' ObjectSet '}'*/
      {
    $$ = new TableConstraint(DefinedObjectSetPtr($2));
	  }
  ;

ComponentRelationConstraint
  : '{' DefinedObjectSet '}' '{' AtNotations '}'
      {
	$$ = new TableConstraint(DefinedObjectSetPtr($2), std::auto_ptr<StringList>($5));
	  }
  ;

AtNotations
  : AtNotations ',' AtNotation
      {
  $1->push_back(*$3);
  delete $3;
	  }
  | AtNotation
      {
  $$ = new StringList;
  $$->push_back(*$1);
  delete $1;
	  }
  ;

AtNotation
  : '@' ComponentIdList
      {
  $$ = $2;
	  }
  | '@' '.' ComponentIdList
      {
  $3->insert(0, ".");
  $$ = $3;
	  }
  ;

ComponentIdList
  : ComponentIdList '.' IDENTIFIER
      {
  *($1) += '.' ;
  *($1) += *($3);
  $$ = $1;
  delete $3;
	  }
  | IDENTIFIER
  ;


/********/
ObjectClassAssignment
  : OBJECTCLASSREFERENCE ASSIGNMENT ObjectClass
    {
		$3->SetName(*$1); delete $1;
		context->Module->AddObjectClass(ObjectClassBasePtr($3));
	}
  ;


ObjectAssignment
  : OBJECTREFERENCE DefinedObjectClass
    {
	   $2->BeginParseObject();
    }
  ASSIGNMENT Object
    {
	   $2->EndParseObject();
       context->classStack->pop();
	   $5->SetName(*$1); delete $1;
       $5->SetObjectClass($2);
       context->Module->AddInformationObject(InformationObjectPtr($5));
	}
  ;

ObjectSetAssignment
  : OBJECTSETREFERENCE DefinedObjectClass
    {
	   $2->BeginParseObjectSet();
	}
  ASSIGNMENT ObjectSet
    {
	   context->Module->AddInformationObjectSet(InformationObjectSetPtr(
			new InformationObjectSetDefn(*$1,
			                             ObjectClassBasePtr($2),
			                             ConstraintPtr($5))));
	   delete $1;
       context->classStack->pop();
	}
  ;


ObjectClass
  : DefinedObjectClass
  {
		$$ = $1;
  }
  | ObjectClassDefn
  {
		$$ = $1;
  }
/*  | ParameterizedObjectClass */
  ;

DefinedObjectClass
  : OBJECTCLASSREFERENCE
    {
  $$ = new DefinedObjectClass(*$1);
  delete $1;
	}
  | ExternalObjectClassReference
    {
  $$ = new DefinedObjectClass(*$1);
  delete $1;
	}
  | UsefulObjectClassReference
    {
  $$ = new DefinedObjectClass(*$1);
  delete $1;
	}
  ;

ExternalObjectClassReference
  : MODULEREFERENCE '.' OBJECTCLASSREFERENCE
      {
	$$ = ConcatNames($1, '.', $3);
      }
  ;

UsefulObjectClassReference
  : TYPE_IDENTIFIER
      {
	$$ = new std::string("TYPE-IDENTIFIER");
      }
  | ABSTRACT_SYNTAX
      {
	$$ = new std::string("ABSTRACT-SYNTAX");
      }
  ;

ObjectClassDefn
  : CLASS  '{'  FieldSpecs '}' WithSyntaxSpec
    {
	    ObjectClassDefn* ocd = new ObjectClassDefn;
		$$ = ocd;
		ocd->SetFieldSpecs(std::auto_ptr<FieldSpecsList>($3));
		ocd->SetWithSyntaxSpec(TokenGroupPtr($5));
		context->InWithSyntaxContext = false;
    }
  ;

FieldSpecs
  : FieldSpecs ',' FieldSpec
	{
		$1->push_back(FieldSpecPtr($3));
		$$=$1;
	}
  | FieldSpec
	{
		$$ = new FieldSpecsList;
		$$->push_back(FieldSpecPtr($1));
	}
  ;

FieldSpec
  : TypeFieldSpec
  | FixedTypeValueFieldSpec
  | VariableTypeValueFieldSpec
  | FixedTypeValueSetFieldSpec
  | VariableTypeValueSetFieldSpec
  | ObjectFieldSpec
  | ObjectSetFieldSpec
  ;

TypeFieldSpec
  : FieldReference
    {
		$$ = new TypeFieldSpec(*$1); delete $1;
	}
  | FieldReference OPTIONAL_t
    {
		$$ = new TypeFieldSpec(*$1, true); delete $1;

    }
  | FieldReference DEFAULT Type
    {
		$$ = new TypeFieldSpec(*$1, false, TypePtr($3)); delete $1;
    }
  ;


FixedTypeValueFieldSpec
  : fieldreference Type  /* Unique ValueOptionalitySpec */
    {
	  $$ = new FixedTypeValueFieldSpec(*$1, TypePtr($2));
	  delete $1;
	}
  | fieldreference Type UNIQUE
    {
	  $$ = new FixedTypeValueFieldSpec(*$1, TypePtr($2), false, true);
  	  delete $1;
	}
  | fieldreference Type OPTIONAL_t
    {
	  $$ = new FixedTypeValueFieldSpec(*$1, TypePtr($2), true);
  	  delete $1;
	}
  | fieldreference Type
    {
      context->ValueTypeContext.reset($2);
  	  $2->BeginParseValue();
	}
	DEFAULT Value
	{
	  FixedTypeValueFieldSpec* spec = new FixedTypeValueFieldSpec(*$1, context->ValueTypeContext);
  	  $2->EndParseValue();
	  spec->SetDefault(ValuePtr($5));
	  $$ = spec;
  	  delete $1;
	}
  | fieldreference Type UNIQUE OPTIONAL_t
    {
	  $$ = new FixedTypeValueFieldSpec(*$1, TypePtr($2), true, true);
  	  delete $1;
	}
  | fieldreference Type UNIQUE
    {
	  context->ValueTypeContext.reset($2);
	  $2->BeginParseValue();
	}
	DEFAULT Value
	{
	  TypePtr t = context->ValueTypeContext;
	  $2->EndParseValue();
	  FixedTypeValueFieldSpec* spec = new FixedTypeValueFieldSpec(*$1, t, false, true);
	  spec->SetDefault(ValuePtr($6));
	  $$ = spec;
  	  delete $1;
	}
 ;



VariableTypeValueFieldSpec
  : fieldreference FieldReference /*  : valuefieldreference FieldName */
    {
		$$ = new VariableTypeValueFieldSpec(*$1, *$2); delete $1; delete $2;
	}
  | fieldreference FieldReference OPTIONAL_t /*  | valuefieldreference FieldName OPTIONAL_t */
    {
		$$ = new VariableTypeValueFieldSpec(*$1, *$2, true); delete $1; delete $2;
	}
/*  | valuefieldreference FieldName DEFAULT Value */
  ;

FixedTypeValueSetFieldSpec
  : FieldReference Type
      {
	$$ = new FixedTypeValueSetFieldSpec(*$1, TypePtr($2));
	delete $1;
	  }
  | FieldReference Type OPTIONAL_t
      {
	$$ = new FixedTypeValueSetFieldSpec(*$1, TypePtr($2), true);
	delete $1;
	  }
  | FieldReference Type
      {
    context->ValueTypeContext.reset($2);
	$2->BeginParseValueSet();
	  }
    DEFAULT ValueSet
	  {
	FixedTypeValueSetFieldSpec* spec = new FixedTypeValueSetFieldSpec(*$1, context->ValueTypeContext);
	$2->EndParseValueSet();
	spec->SetDefault(ValueSetPtr($5));
	$$ = spec;
	  }
  ;


VariableTypeValueSetFieldSpec
   : FieldReference FieldReference /* : VALUESETFIELDREFERENCE FieldName */
       {
	 $$ = new VariableTypeValueSetFieldSpec(*$1, *$2); delete $1; delete $2;
	   }
   | FieldReference FieldReference OPTIONAL_t /* | VALUESETFIELDREFERENCE FieldName OPTIONAL_t */
       {
	 $$ = new VariableTypeValueSetFieldSpec(*$1, *$2, true); delete $1; delete $2;
	   }
/*  | FieldReference FieldName DEFAULT ValueSet */
  ;

ObjectFieldSpec
  : fieldreference DefinedObjectClass
      {
	$$ = new  ObjectFieldSpec(*$1, $2); delete $1;
	  }
  | fieldreference DefinedObjectClass OPTIONAL_t
      {
	$$ = new  ObjectFieldSpec(*$1, $2, true); delete $1;
	  }
  | fieldreference DefinedObjectClass
      {
	$2->BeginParseObject();
	  }
    DEFAULT Object
      {
	$2->EndParseObject();
    context->classStack->pop();
	ObjectFieldSpec* spec = new  ObjectFieldSpec(*$1, $2); delete $1;
	spec->SetDefault(InformationObjectPtr($5));
	$$ = spec;
	  }
  ;


ObjectSetFieldSpec
  : FieldReference DefinedObjectClass
    {
		$$ = new  ObjectSetFieldSpec(*$1, DefinedObjectClassPtr($2)); delete $1;
	}
  | FieldReference DefinedObjectClass OPTIONAL_t
    {
		$$ = new  ObjectSetFieldSpec(*$1, DefinedObjectClassPtr($2), true); delete $1;
	}
  | FieldReference DefinedObjectClass
    {
	    $2->BeginParseObjectSet();
	}
  DEFAULT ObjectSet
    {
	    ObjectSetFieldSpec* spec = new  ObjectSetFieldSpec(*$1, DefinedObjectClassPtr($2));
	    delete $1;
		spec->SetDefault(ConstraintPtr($5));
		$$= spec;
    	context->classStack->pop();
	}
  ;


WithSyntaxSpec
  : WITH SYNTAX
    {
      context->InWithSyntaxContext = true;
	}
   SyntaxList
	{
	  $$ = $4;
      context->InWithSyntaxContext = false;
	}
  | /* empty */
    {
	  $$ = NULL;
	}
  ;

SyntaxList
  : '{' TokenOrGroupSpecs '}'
    {
		$$ = $2;
    }
  | '{' '}'
    {
	    $$ = NULL;
	}
  ;

TokenOrGroupSpecs
  : TokenOrGroupSpecs TokenOrGroupSpec
	{
		$$ = $1;
		$$->AddToken(TokenOrGroupSpecPtr($2));

	}
  | TokenOrGroupSpec
    {
		$$ = new TokenGroup;
		$$->AddToken(TokenOrGroupSpecPtr($1));
    }
  ;

TokenOrGroupSpec
  : RequiredToken
  | OptionalGroup
  ;

OptionalGroup
  : '[' TokenOrGroupSpecs ']'
  {
	$2->SetOptional();
    $$ = $2;
  }
  ;

RequiredToken
  : Literal
    {
		$$= new Literal(*$1); delete $1;
    }
  | FieldReference
    {
		$$ = new PrimitiveFieldName(*$1); delete $1;
	}
  | fieldreference
    {
		$$ = new PrimitiveFieldName(*$1); delete $1;
	}
  ;

Literal
  : WORD_t
	{
		$$ = $1;
	}
  | ','
    {
	    $$ = new std::string(",");
	}
  ;


DefinedObject
  : OBJECTREFERENCE
    {
  $$ = new DefinedObject(*$1);
  delete $1;
	}
  | ExternalObjectReference
    {
  $$ = new DefinedObject(*$1);
  delete $1;
	}
  ;


ExternalObjectReference
  : MODULEREFERENCE '.' OBJECTREFERENCE
    {
  $$ = ConcatNames($1, '.', $3);
	}
  ;

/*
ParameterizedObjectClass
  : DefinedObjectClass ActualParameterList
    { }
  ;
*/

DefinedObjectSet
  : OBJECTSETREFERENCE
    {
		$$ = new DefinedObjectSet(*$1);
		delete $1;
	}
  | ExternalObjectSetReference
    {
		$$ = new DefinedObjectSet(*$1);
		delete $1;
	}
  ;

ExternalObjectSetReference
  : MODULEREFERENCE '.' OBJECTSETREFERENCE
      {
	$$ = ConcatNames($1, '.', $3);
	  }
  ;

ParameterizedObjectSet
  : PARAMETERIZEDOBJECTSETREFERENCE ActualParameterList
      {
	$$ = new ParameterizedObjectSet(*$1, ActualParameterListPtr($2));  delete $1;
	  }
  | MODULEREFERENCE '.' PARAMETERIZEDOBJECTSETREFERENCE ActualParameterList
      {
    std::string* str = ConcatNames($1, '.', $3);
	$$ = new ParameterizedObjectSet(*str, ActualParameterListPtr($4));
	delete str;
	  }
  ;

FieldName
  : FieldName '.' PrimitiveFieldName
      {
	$$ = ConcatNames($1, '.', $3);
      }
  | PrimitiveFieldName
  ;


PrimitiveFieldName
  : TYPEFIELDREFERENCE
  | VALUEFIELDREFERENCE
  | VALUESETFIELDREFERENCE
  | OBJECTFIELDREFERENCE
  | OBJECTSETFIELDREFERENCE
  ;



Object
  : DefinedObject
  | ObjectDefn
  | ObjectFromObject
/*
  | ParameterizedObject
    { }
*/
  ;


ObjectDefn
  : DefinedSyntax
    {
		$$ = new DefaultObjectDefn($1->GetDefaultSyntax());
		delete $1;
	}
  | DefaultSyntax
    {
		$$ = new DefaultObjectDefn(std::auto_ptr<FieldSettingList>($1));
	}
;

DefaultSyntax
  : OBJECT_BRACE FieldSettings '}'
      {
    $$ = $2;
    if (context->InObjectSetContext)
	  context->classStack->top()->PreParseObject();
	  }
  ;

FieldSettings
  : FieldSettings ',' PrimitiveFieldName
      {
	context->classStack->top()->GetField(*$3)->BeginParseSetting($1);
	  }
	Setting
	  {
	$$ = $1;
	context->classStack->top()->GetField(*$3)->EndParseSetting();
    $1->push_back(FieldSettingPtr(new FieldSetting(*$3, std::auto_ptr<Setting>($5))));
    delete $3;
	  }
  | PrimitiveFieldName
	  {
	context->classStack->top()->GetField(*$1)->BeginParseSetting(NULL);
	  }
	Setting
	  {
	$$ = new FieldSettingList;
	context->classStack->top()->GetField(*$1)->EndParseSetting();
	$$->push_back(FieldSettingPtr(new FieldSetting(*$1, std::auto_ptr<Setting>($3))));
	delete $1;
	  }
  ;


DefinedSyntax
  : OBJECT_BRACE DefinedSyntaxTokens '}'
	  {
	$$ = $2;
	$$->ResetTokenGroup();
    if (context->InObjectSetContext)
	  context->classStack->top()->PreParseObject();
	  }
  ;

DefinedSyntaxTokens
  : DefinedSyntaxTokens DefinedSyntaxToken
	{
		$$ = $1;
		$$->AddToken($2);
	}
  | /* empty */
  {
		$$ = new DefaultSyntaxBuilder(context->classStack->top()->GetWithSyntax());
  }
  ;

DefinedSyntaxToken
  : Literal
  {
	$$ = new LiteralToken(*$1); delete $1;
  }
  | Setting
  {
	$$ = new SettingToken(std::auto_ptr<Setting>($1));
  }
  ;


Setting
  : Type
	{
		$$ = new TypeSetting(TypePtr($1));
	}
  | Value
    {
	    if (context->ValueTypeContext.get() == NULL)
		  std::cerr << StdError(Fatal) << "Parsing Error\n";
		$$ = new ValueSetting(context->ValueTypeContext,ValuePtr($1));
	}
  | ValueSet
    {
		$$ = new ValueSetSetting(ValueSetPtr($1));
	}
  | Object
    {
		$$ = new ObjectSetting(InformationObjectPtr($1),context->classStack->top());
	}
  | ObjectSet
    {
	    $$ = new ObjectSetSetting(ConstraintPtr($1), context->classStack->top());
    	context->classStack->pop();
	}
  ;


ObjectSet
  : OBJECTSET_BRACE ObjectSetSpec '}'
      {
	$$ = $2;
	context->classStack->top()->EndParseObjectSet();
	  }
  ;

ObjectSetSpec
  : ElementSetSpecs
  | '.''.''.'
    {
	  $$ = new Constraint(std::auto_ptr<ConstraintElementVector>(), true);
	}
  ;


ObjectSetElements
  : Object
      {
	$$ = new SingleObjectConstraintElement(InformationObjectPtr($1));
      }
  | DefinedObjectSet
      {
	$$ = $1;
      }
  | ObjectSetFromObjects
      {
	$$ = $1;
      }
  | ParameterizedObjectSet
      {
	$$ = $1;
      }
  ;

/*!!!
ObjectSetFromObjects
  : ReferencedObjectsDot FieldName
  ;
*/

ReferencedObjectsDot
  : ReferencedObjects
    {
  context->InformationFromObjectContext = $1->GetObjectClass();
    }
  '.'
    {
  $$ = $1;
	}
  ;

ReferencedObjectDot
  : ReferencedObject
    {
  context->InformationFromObjectContext = $1->GetObjectClass();
    }
  '.'
    {
  $$ = $1;
	}
  ;

/********/

ParameterizedAssignment
  : ParameterizedTypeAssignment
  | ParameterizedValueAssignment
  | ParameterizedValueSetTypeAssignment
/*  | ParameterizedObjectClassAssignment */
  | ParameterizedObjectAssignment
  | ParameterizedObjectSetAssignment
  ;

ParameterizedTypeAssignment
  : PARAMETERIZEDTYPEREFERENCE ParameterList
      {
	context->DummyParameters = $2;
      }
    ASSIGNMENT Type
      {
	context->DummyParameters = NULL;
	$5->SetName(*$1); delete $1;
	$5->SetParameters(*$2); delete $2;
	context->Module->AddType(TypePtr($5));
      }
  ;

ParameterizedValueAssignment
  : PARAMETERIZEDVALUEREFERENCE ParameterList Type
      {
	context->DummyParameters = $2;
      }
    ASSIGNMENT Value
      {
	context->DummyParameters = NULL;
	  }
  ;

ParameterizedValueSetTypeAssignment
  : PARAMETERIZEDTYPEREFERENCE ParameterList Type
      {
	context->DummyParameters = $2;
      }
    ASSIGNMENT ValueSet
      {
	context->DummyParameters = NULL;
	  }
  ;

/*
ParameterizedObjectClassAssignment
  : PARAMETERIZEDOBJECTCLASSREFERENCE ParameterList ASSIGNMENT ObjectClass
    { }
  ;
*/

ParameterizedObjectAssignment
  : PARAMETERIZEDOBJECTREFERENCE ParameterList DefinedObjectClass
      {
	context->DummyParameters = $2;
	$3->BeginParseObject();
      }
    ASSIGNMENT Object
      {
	$3->EndParseObject();
   	context->classStack->pop();
	context->DummyParameters = NULL;
	$6->SetName(*$1); delete $1;
    $6->SetObjectClass($3);
	$6->SetParameters(std::auto_ptr<ParameterList>($2));
    context->Module->AddInformationObject(InformationObjectPtr($6));
	  }
  ;

ParameterizedObjectSetAssignment
  : PARAMETERIZEDOBJECTSETREFERENCE ParameterList DefinedObjectClass
      {
	context->DummyParameters = $2;
    $3->BeginParseObjectSet();
      }
    ASSIGNMENT ObjectSet
      {
	context->DummyParameters = NULL;
    context->Module->AddInformationObjectSet(InformationObjectSetPtr(
		new InformationObjectSetDefn(*$1,
		                            ObjectClassBasePtr($3),
		                            ConstraintPtr($6),
		                            ParameterListPtr($2))));
	delete $1;
    context->classStack->pop();
	  }
  ;

ParameterList
  : '{' Parameters '}'
      {
	$$ = $2;
      }
  ;

Parameters
  : Parameters ',' Parameter
      {
	$$ = $1;
	$$->Append(ParameterPtr($3));
      }
  | Parameter
      {
	$$ = new ParameterList;
	$$->Append(ParameterPtr($1));
      }
  ;

Parameter
  : Type ':' Reference
      {
	$$ = new ValueParameter(TypePtr($1),*$3); delete $1;
      }
  | DefinedObjectClass ':' Reference
      {
    $$ = new ObjectParameter(DefinedObjectClassPtr($1),*$3); delete $3;
	  }
  | Reference
      {
    $$ = new Parameter(*$1); delete $1;
	  }
  ;

ParameterizedType
  : PARAMETERIZEDTYPEREFERENCE ActualParameterList
      {
	$$ = new ParameterizedType(*$1, *$2);
	delete $1; delete $2;
      }
  | MODULEREFERENCE '.' PARAMETERIZEDTYPEREFERENCE ActualParameterList
      {
    std::string* str = ConcatNames($1, '.', $3);
    $$ = new ParameterizedType(*str,*$4);
    delete str; delete $4;
	  }
  ;

SimpleDefinedType
  : TYPEREFERENCE
  | ExternalTypeReference
  ;

ActualParameterList
  : '{' ActualParameters '}'
      {
	$$ = $2;
      }
  ;

ActualParameters
  : ActualParameters ',' ActualParameter
      {
	$1->push_back(ActualParameterPtr($3));
	$$ = $1;
      }
  | ActualParameter
      {
	$$ = new ActualParameterList;
	$$->push_back(ActualParameterPtr($1));
      }
  ;

ActualParameter
  : Type
      {
    $$ = new ActualTypeParameter(TypePtr($1));
	  }
  | Value
      {
    $$ = new ActualValueParameter(ValuePtr($1));
	  }
  | '{' DefinedType '}' /* ValueSet */
      {
    $$ = new ActualValueSetParameter(TypePtr($2) );
	  }
/*!!!
  | DefinedObjectClass
    { }
*/
  | ObjectParameter /* Object */
      {
    $$ = new ActualObjectParameter(InformationObjectPtr($1));
	  }
  | '{' ObjectSetParameter '}' /* ObjectSet */
      {
    $$ = new ActualObjectSetParameter(boost::shared_ptr<ObjectSetConstraintElement>($2));
	  }
  ;

ObjectParameter
  : DefinedObject
  | ParameterizedObject
  ;

ObjectSetParameter
  : DefinedObjectSet
      {
  $$ = $1;
	  }
  | ParameterizedObjectSet
      {
  $$ = $1;
	  }
  ;
/********/

ValueAssignment
  : VALUEREFERENCE Type
      {
    context->ValueTypeContext.reset($2);
	$2->BeginParseValue();
      }
    ASSIGNMENT Value
      {
	$2->EndParseValue();
	$5->SetValueName(*$1); delete $1;
	context->Module->AddValue(ValuePtr($5));
      }
  ;


Value
  : BuiltinValue
  | ReferencedValue
  ;


BuiltinValue
  : BitStringValue
  | BooleanValue
  | CharacterStringValue
  | ChoiceValue
/*| EmbeddedPDVValue  synonym to SequenceValue */
/*| EnumeratedValue   synonym to IDENTIFIER    */
/*| ExternalValue     synonym to SequenceValue */
/*| InstanceOfValue   synonym to Value */
  | SignedNumber      /* IntegerValue */
      {
	$$ = new IntegerValue($1);
      }
  | NullValue
/*!!!
  | ObjectClassFieldValue
*/
  | ObjectIdentifierValue
  | OctetStringValue
  | RealValue
  | SequenceValue
/*!!!!
  | SequenceOfValue
*/
/*| SetValue	      synonym to SequenceValue */
/*| SetOfValue	      synonym to SequenceOfValue */
/*| TaggedValue	      synonym to Value */
  ;


DefinedValue
  : VALUEREFERENCE
      {
	$$ = new DefinedValue(*$1); delete $1;
      }
  | ExternalValueReference
      {
	$$ = new DefinedValue(*$1); delete $1;
      }
/*
  | ParameterizedValue
*/
  ;

ExternalValueReference
  : MODULEREFERENCE '.' VALUEREFERENCE
      {
	*$1 += *$3;
	delete $3;
      }
  ;

ObjectIdentifierValue
  : '{' ObjIdComponentList '}'
      {
	$$ = new ObjectIdentifierValue(*$2); delete $2;
      }
/*!!!
  | '{' DefinedValue_OID ObjIdComponentList '}'
      {
	$$ = new ObjectIdentifierValue($2);
      }
*/
  ;


ObjIdComponentList
  : ObjIdComponent
      {
	$$ = new StringList;
	$$->push_back(*$1);
	delete $1;
      }
/*  | ObjIdComponent ObjIdComponentList
      {
	$2->InsertAt(0, $1);
	$$ = $2;
      }
*/
	| ObjIdComponentList ObjIdComponent
      {
	$1->push_back(*$2);
	$$ = $1;
	delete $2;
      }
  ;

ObjIdComponent
  : OID_IDENTIFIER
  | OID_INTEGER
  | OID_IDENTIFIER '(' NumberForm ')'
      {
	delete $1;
	$$ = $3;
      }
  ;

NumberForm
  : OID_INTEGER
  | ExternalValueReference
  | VALUEREFERENCE
  ;


OctetStringValue
  : BSTRING
      {
	$$ = new OctetStringValue(*$1); delete $1;
      }
  | HSTRING
      {
	$$ = new OctetStringValue(*$1); delete $1;
      }
  ;

BitStringValue
  : BS_BSTRING
      {
	$$ = new BitStringValue(*$1); delete $1;
      }
  | BS_HSTRING
      {
	$$ = new BitStringValue(*$1); delete $1;
      }
  | '{' BitIdentifierList '}'
      {
	$$ = new BitStringValue($2);
      }
  ;


BitIdentifierList
  : BIT_IDENTIFIER
      {
	$$ = new StringList;
	$$->push_back(*$1);
	delete $1;
      }
  | BitIdentifierList ',' BIT_IDENTIFIER
      {
	$1->push_back(*$3);
	delete $3;
      }
  ;


BooleanValue
  : TRUE_t
      {
	$$ = new BooleanValue(true);
      }
  | FALSE_t
      {
	$$ = new BooleanValue(false);
      }
  ;


CharacterStringValue
  : RestrictedCharacterStringValue
/*!!!
  | UnrestrictedCharacterStringValue
*/
  ;

RestrictedCharacterStringValue
  : CSTRING
      {
	$$ = new CharacterStringValue(*$1); delete $1;
      }
  | CharacterStringList
  | Quadruple
  | Tuple
  ;

CharacterStringList
  : '{' CharSyms '}'
      {
	$$ = new CharacterStringValue(*$2); delete $2;
      }
  ;

CharSyms
  : CharsDefn
      {
	$$ = new StringList;
	$$->push_back(*$1);
	delete $1;
      }
  | CharSyms ',' CharsDefn
      {
	$1->push_back(*$3);
	delete $3;
      }
  ;

CharsDefn
  : CSTRING
  | DefinedValue
      {
	std::cerr << StdError(Warning) << "DefinedValue in string unsupported" << *$1 << std::endl;
	$$ = new std::string(((DefinedValue*)($1))->GetReference());
      }
  ;

Quadruple
  :  '{'  INTEGER  ','  INTEGER  ','  INTEGER  ','  INTEGER '}'
      {
	if ($2 != 0 || $4 != 0 || $6 > 255 || $8 > 255)
	  std::cerr << StdError(Warning) << "Illegal value in Character Quadruple" << std::endl;
	$$ = new CharacterValue((unsigned char)$2, (unsigned char)$4, (unsigned char)$6, (unsigned char)$8);
      }
  ;

Tuple
  :  '{' INTEGER ',' INTEGER '}'
      {
	if ($2 > 255 || $4 > 255)
	  std::cerr << StdError(Warning) << "Illegal value in Character Tuple" << std::endl;
	$$ = new CharacterValue((unsigned char)$2, (unsigned char)$4);
      }
  ;


ChoiceValue
  : IDENTIFIER ':' Value
      {
	$$ = new ChoiceValue(context->ValueTypeContext, *$1, ValuePtr($3)); delete $1;
      }
  ;


NullValue
  : NULL_VALUE
      {
	$$ = new NullValue;
      }
  ;


RealValue
  : NumericRealValue
  | SpecialRealValue
  ;

NumericRealValue
  :  '0'
      {
	$$ = new RealValue(0);
      }
/*!!!
  | SequenceValue
*/
  ;

SpecialRealValue
  : PLUS_INFINITY
      {
	$$ = new RealValue(0);
      }
  | MINUS_INFINITY
      {
	$$ = new RealValue(0);
      }
  ;


SequenceValue
  : '{' ComponentValueList '}'
      {
	$$ = new SequenceValue($2);
      }
  | '{'  '}'
      {
	$$ = new SequenceValue;
      }
  ;

ComponentValueList
  : NamedValue
      {
	$$ = new ValuesList;
	$$->push_back(ValuePtr($1));
      }
  | ComponentValueList ',' NamedValue
      {
	$1->push_back(ValuePtr($3));
      }
  ;

NamedValue
  : IDENTIFIER Value
      {
	$2->SetValueName(*$1); delete $1;
	$$ = $2;
      }
  ;


/*!!!!
SequenceOfValue
  : '{' ValueList '}'
      {
	$$ = NULL;
      }
  | '{'  '}'
      {
	$$ = NULL;
      }
  ;

ValueList
  : Value
      { }
  | ValueList ',' Value
      { }
  ;
*/


/*!!!
ObjectClassFieldValue
  : OpenTypeFieldVal
  | Value
  ;

OpenTypeFieldVal
  : Type ':' Value
  ;
*/


ReferencedValue
  : DefinedValue
  | ValueFromObject
  ;

ValueFromObject
  : ReferencedObjectDot VALUEFIELDREFERENCE
    {
	  ValueSetting* setting = (ValueSetting*) $1->GetSetting(*$2);
	  $$ = new DefinedValue(setting->GetValue());
	  delete $1;
	  delete $2;
      context->InformationFromObjectContext = NULL;
	}
  ;

VALUEFIELDREFERENCE
  : FIXEDTYPEVALUEFIELDREFERENCE
  | VARIABLETYPEVALUEFIELDREFERENCE
  ;

ValueSetFromObject
 : ReferencedObjectDot VALUESETFIELDREFERENCE
   {
	  $$ = new ValueSetFromObject(InformationObjectPtr($1), *$2); delete $2;
      context->InformationFromObjectContext = NULL;
   }
  ;

VALUESETFIELDREFERENCE
  : FIXEDTYPEVALUESETFIELDREFERENCE
  | VARIABLETYPEVALUESETFIELDREFERENCE
  ;

ObjectFromObject
  : ReferencedObjectDot OBJECTFIELDREFERENCE
    {
	  $$ = new ObjectFromObject(InformationObjectPtr($1), *$2); delete $2;
	  context->InformationFromObjectContext = NULL;
    }
  ;

ObjectSetFromObject
  : ReferencedObjectDot OBJECTSETFIELDREFERENCE
    {
	  $$ = new ObjectSetFromObject(InformationObjectPtr($1), *$2); delete $2;
	  context->InformationFromObjectContext = NULL;
    }
  ;

/*
ParameterizedValue
  : PARAMETERIZEDVALUEREFERENCE ActualParameterList
    { }
  | MODULEREFERENCE '.' PARAMETERIZEDVALUEREFERENCE ActualParameterList
    { }
  ;
*/

SimpleDefinedValue
  : VALUEREFERENCE
  | ExternalValueReference
  ;




ValueSet
  : VALUESET_BRACE ElementSetSpecs '}'
	{
  if (context->ValueTypeContext.get() == NULL)
     std::cerr << StdError(Warning) << "";
  // $$ = new ValueSetDefn(TypePtr(new DefinedType(context->ValueTypeContext)), ConstraintPtr($2));
  $$ = new ValueSetDefn(context->ValueTypeContext, ConstraintPtr($2));
  context->ValueTypeContext->EndParseValueSet();
	}
  ;



MacroDefinition
  : TYPEREFERENCE MACRO ASSIGNMENT MacroSubstance
      {
	std::cerr << StdError(Warning) << "MACRO unsupported" << std::endl;
      }
  ;

MacroSubstance
  : BEGIN_t
      {
	InMacroContext = true;
      }
    MacroBody END
      {
	InMacroContext = false;
      }
  | TYPEREFERENCE
      {}
  | TYPEREFERENCE '.' TYPEREFERENCE
      {}
  ;

MacroBody
  : TypeProduction ValueProduction /*SupportingProductions*/
  ;

TypeProduction
  : TYPE_t NOTATION ASSIGNMENT MacroAlternativeList
  ;

ValueProduction
  : VALUE NOTATION ASSIGNMENT MacroAlternativeList
  ;


/*
SupportingProductions
  : ProductionList
  | /* empty *//*
  ;

ProductionList
  : Production
  | ProductionList Production
  ;

Production
  : TYPEREFERENCE ASSIGNMENT MacroAlternativeList

  ;
*/

MacroAlternativeList
  : MacroAlternative
  | MacroAlternative '|' MacroAlternativeList
  ;

MacroAlternative
  : SymbolElement
  | SymbolElement MacroAlternative
  ;

SymbolElement
  : SymbolDefn
  | EmbeddedDefinitions
  ;

SymbolDefn
  : CSTRING
      {}
  | TYPEREFERENCE
      {}
  | TYPEREFERENCE ASSIGNMENT
      {}
  ;

EmbeddedDefinitions
  : '<' EmbeddedDefinitionList '>'
  ;

EmbeddedDefinitionList
  : EmbeddedDefinition
  | EmbeddedDefinitionList EmbeddedDefinition
  ;

EmbeddedDefinition
  : LocalTypeAssignment
  | LocalValueAssignment
  ;

LocalTypeAssignment
  : TYPEREFERENCE ASSIGNMENT Type
      {}
  ;

LocalValueAssignment
  : IDENTIFIER Type ASSIGNMENT Value
      {}
  ;


/********/

/********/

/*!!! Not actually referenced by any other part of grammar
AbsoluteReference
  : '@' GlobalModuleReference '.' ItemSpec
  ;

ItemSpec
  : TYPEREFERENCE
  |  ItemId '.' ComponentId
  ;

ItemId
  : ItemSpec
  ;

ComponentId
  : IDENTIFIER
  | INTEGER
  | '*'
  ;
*/


Reference
  : TYPEREFERENCE
  | VALUEREFERENCE
  | OBJECTCLASSREFERENCE
  | OBJECTREFERENCE
  | OBJECTSETREFERENCE
  | PARAMETERIZEDTYPEREFERENCE
  | PARAMETERIZEDVALUEREFERENCE
  | PARAMETERIZEDOBJECTCLASSREFERENCE
  | PARAMETERIZEDOBJECTREFERENCE
  | PARAMETERIZEDOBJECTSETREFERENCE
  | IDENTIFIER
  ;


NamedNumberList
  : NamedNumber
      {
	$$ = new NamedNumberList;
	$$->push_back(NamedNumberPtr($1));
      }
  | NamedNumberList ',' NamedNumber
      {
	$1->push_back(NamedNumberPtr($3));
      }
  ;

NamedNumber
  : IDENTIFIER '(' SignedNumber ')'
      {
	$$ = new NamedNumber($1, (int)$3);
      }
  | IDENTIFIER '(' DefinedValue ')'
      {
	$$ = new NamedNumber($1, ((DefinedValue*)$3)->GetReference());
	delete $3;
      }
  ;


SignedNumber
  :  INTEGER
  | '-' INTEGER
      {
	$$ = -$2;
      }
  ;

/** End of File ****/
