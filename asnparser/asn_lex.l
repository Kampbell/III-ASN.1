%{

/*
 * asn_lex.l
 *
 * ASN lexical analysis file
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
 * 
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors: 
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 *
 */

#include "main.h"
#include "asn_grammar.hxx"
#include <ctype.h>

#define YY_NEVER_INTERACTIVE 1

extern unsigned lineNumber;

extern int LexEcho;
#define YYDEBUG 1
#define YY_USER_ACTION if (LexEcho) std::cout << yytext << std::flush;
#define YY_DECL int yylex(YYSTYPE* yylval_param, ParserContext* context, Environment* env)

static const char* SYMBOLS[] = {
  "$end", "error", "$undefined", "MODULEREFERENCE",
  "TYPEREFERENCE", "OBJECTCLASSREFERENCE", "VALUEREFERENCE",
  "OBJECTREFERENCE", "OBJECTSETREFERENCE", "PARAMETERIZEDTYPEREFERENCE",
  "PARAMETERIZEDOBJECTCLASSREFERENCE", "PARAMETERIZEDVALUEREFERENCE",
  "PARAMETERIZEDOBJECTREFERENCE", "PARAMETERIZEDOBJECTSETREFERENCE",
  "VALUESET_BRACE", "OBJECT_BRACE", "OBJECTSET_BRACE", "IDENTIFIER",
  "BIT_IDENTIFIER", "OID_IDENTIFIER", "IMPORT_IDENTIFIER",
  "fieldreference", "FieldReference", "TYPEFIELDREFERENCE",
  "FIXEDTYPEVALUEFIELDREFERENCE", "VARIABLETYPEVALUEFIELDREFERENCE",
  "FIXEDTYPEVALUESETFIELDREFERENCE", "VARIABLETYPEVALUESETFIELDREFERENCE",
  "OBJECTFIELDREFERENCE", "OBJECTSETFIELDREFERENCE", "INTEGER", "CSTRING",
  "BSTRING", "HSTRING", "BS_BSTRING", "BS_HSTRING", "ABSENT",
  "ABSTRACT_SYNTAX", "ALL", "ANY", "APPLICATION", "ASSIGNMENT",
  "AUTOMATIC", "BEGIN_t", "BIT", "BMPString", "BOOLEAN_t", "BY",
  "CHARACTER", "CHOICE", "CLASS", "COMPONENT", "COMPONENTS", "CONSTRAINED",
  "DEFAULT", "DEFINED", "DEFINITIONS", "EMBEDDED", "END", "ENUMERATED",
  "EXCEPT", "EXPLICIT", "EXPORTS", "EXTERNAL", "FALSE_t", "FROM",
  "GeneralString", "GraphicString", "IA5String", "TYPE_IDENTIFIER",
  "IDENTIFIER_t", "IMPLICIT", "IMPORTS", "INCLUDES", "INSTANCE",
  "INTEGER_t", "INTERSECTION", "ISO646String", "MACRO", "MAX", "MIN",
  "MINUS_INFINITY", "NOTATION", "NULL_VALUE", "NULL_TYPE", "NumericString",
  "OBJECT", "OCTET", "OF_t", "OPTIONAL_t", "PDV", "PLUS_INFINITY",
  "PRESENT", "PrintableString", "PRIVATE", "REAL", "SEQUENCE", "SET",
  "SIZE_t", "STRING", "SYNTAX", "T61String", "TAGS", "TeletexString",
  "TRUE_t", "TYPE_t", "UNION", "UNIQUE", "UNIVERSAL", "UniversalString",
  "VideotexString", "VisibleString", "GeneralizedTime", "UTCTime", "VALUE",
  "WITH", "ObjectDescriptor_t", "WORD_t", "OID_INTEGER"}; 

extern "C" int yywrap() { return 1; }

int IsUpper(const char* text)
{
	for (const char* p = text; *p != NULL; ++p)
		if (!(isupper(*p) || *p == '-' || *p == '_'))
			return FALSE;
	return TRUE;
}

static int TokenSelect(YYSTYPE * yylval, int ctx, int token1, int token2)
{
  if (ctx)
    return token1;
  yylval->sval = new std::string(yytext);
  return token2;
}

static int Is_WORD(ParserContext* context)
{
	return (context->InWithSyntaxContext || 
	         (!context->classStack->empty() && context->classStack->top()->HasLiteral(yytext))) 
			&& !context->ParsingConstructedType;
}

static int GetUpperCaseToken(ParserContext* context, const char* text)
{
    int result;
	if (context->DummyParameters && (result = context->DummyParameters->GetIdentifierType(text)) != -1) {
		//printf("GetUpperCaseToken::token=%d, name=%s, value=%s\n", result, SYMBOLS[result - 255], text);
		return result;
	}

	if (context->Module)
	{
		result = context->Module->GetIdentifierType(text);
		if (result != -1) {
			//printf("GetUpperCaseToken::token=%d, name=%s, value=%s\n", result, SYMBOLS[result - 255], text);
			if (result == OBJECTCLASSREFERENCE)
				;
			return result;
		} else
		if (((context->InWithSyntaxContext && IsUpper(text)) ||
		   (!context->classStack->empty() && context->classStack->top()->HasLiteral(text))) 
		   && !context->ParsingConstructedType) {
			return WORD_t;
		} else {
			return TYPEREFERENCE;
		}
	} else
		return MODULEREFERENCE;
}

static int GetLowerCaseToken(ParserContext* context, const char* text)
{
	int result = 0;

	if (context->InOIDContext)
	    return OID_IDENTIFIER;

	if (context->DummyParameters && (result = context->DummyParameters->GetIdentifierType(text)) != -1) {
		//printf("GetLowerCaseToken::token=%d, name=%s, value=%s\n", result, SYMBOLS[result - 255], text);
		return result;
	}

	if (context->Module && (result = context->Module->GetIdentifierType(text)) != -1) {
		//printf("GetLowerCaseToken::token=%d, name=%s, value=%s\n", result, SYMBOLS[result - 255], text);
		return result;
	}

    return context->IdentifierTokenContext;
}

%}
%option bison-bridge

%x multiline_comment comment_line directive remove_directive

%%

"::="				{ return ASSIGNMENT; }

"ABSENT"			{ return TokenSelect(yylval, !Is_WORD(context),ABSENT,WORD_t); }              
"ABSTRACT-SYNTAX"   { return TokenSelect(yylval, !Is_WORD(context),ABSTRACT_SYNTAX,WORD_t); }
"ALL"               { return TokenSelect(yylval, !Is_WORD(context),ALL,WORD_t); }
"ANY"               { return TokenSelect(yylval, !Is_WORD(context),ANY,WORD_t); }
"APPLICATION"       { return TokenSelect(yylval, !Is_WORD(context),APPLICATION,WORD_t); }
"AUTOMATIC"         { return TokenSelect(yylval, !Is_WORD(context),AUTOMATIC,WORD_t); }
"BEGIN"             { return TokenSelect(yylval, !Is_WORD(context),BEGIN_t,WORD_t); }
"BIT"               { return BIT; }
"BMPString"         { return TokenSelect(yylval, !Is_WORD(context),BMPString,WORD_t); }
"BOOLEAN"           { return BOOLEAN_t; }
"BY"                { return TokenSelect(yylval, !Is_WORD(context),BY,WORD_t); }
"CHARACTER"         { return CHARACTER; }
"CHOICE"            { return CHOICE; }
"CLASS"             { return TokenSelect(yylval, !Is_WORD(context),CLASS,WORD_t); }
"COMPONENT"         { return TokenSelect(yylval, !Is_WORD(context),COMPONENT,WORD_t); }
"COMPONENTS"        { return TokenSelect(yylval, !Is_WORD(context),COMPONENTS,WORD_t); }
"CONSTRAINED"       { return TokenSelect(yylval, !Is_WORD(context),CONSTRAINED,WORD_t); }
"DEFAULT"           { return TokenSelect(yylval, !Is_WORD(context),DEFAULT,WORD_t); }
"DEFINED"           { return TokenSelect(yylval, !Is_WORD(context),DEFINED,WORD_t); }
"DEFINITIONS"       { return TokenSelect(yylval, !Is_WORD(context),DEFINITIONS,WORD_t); }
"EMBEDDED"          { return EMBEDDED; }
"END"               { return END; }
"ENUMERATED"        { return ENUMERATED; }
"EXCEPT"            { return TokenSelect(yylval, !Is_WORD(context),EXCEPT,WORD_t); }
"EXPLICIT"          { return TokenSelect(yylval, !Is_WORD(context),EXPLICIT,WORD_t); }
"EXPORTS"           { return TokenSelect(yylval, !Is_WORD(context),EXPORTS,WORD_t); }
"EXTERNAL"          { return EXTERNAL; }
"FALSE"             { return FALSE_t; }
"FROM"              { return TokenSelect(yylval, !Is_WORD(context),FROM,WORD_t); }
"GeneralString"     { return GeneralString; }
"GraphicString"     { return GraphicString; }
"UTCTime"			{ return UTCTime; }
"GeneralizedTime"   { return GeneralizedTime; }
"IA5String"         { return IA5String; }
"TYPE-IDENTIFIER"   { return TokenSelect(yylval, !Is_WORD(context),TYPE_IDENTIFIER,WORD_t); }
"IDENTIFIER"        { return TokenSelect(yylval, !Is_WORD(context),IDENTIFIER_t,WORD_t); }
"IMPLICIT"          { return TokenSelect(yylval, !Is_WORD(context),IMPLICIT,WORD_t); }
"IMPORTS"           { return TokenSelect(yylval, !Is_WORD(context),IMPORTS,WORD_t); }
"INCLUDES"          { return TokenSelect(yylval, !Is_WORD(context),INCLUDES,WORD_t); }
"INSTANCE"          { return INSTANCE; }
"INTEGER"           { return INTEGER_t; }
"INTERSECTION"      { return INTERSECTION; }
"ISO646String"      { return ISO646String; }
"MACRO"				{ return TokenSelect(yylval, !Is_WORD(context),MACRO,WORD_t); }
"MAX"               { return TokenSelect(yylval, !Is_WORD(context),MAX,WORD_t); }
"MIN"               { return TokenSelect(yylval, !Is_WORD(context),MIN,WORD_t); }
"MINUS-INFINITY"    { return MINUS_INFINITY; }
"NOTATION"			{ return TokenSelect(yylval, !Is_WORD(context),NOTATION,WORD_t); }
"NULL"              { return context->NullTokenContext; }
"NumericString"     { return NumericString; }
"OBJECT"            { return OBJECT; }
"OCTET"             { return OCTET; }
"OF"                { return TokenSelect(yylval, !Is_WORD(context),OF_t,WORD_t); }
"OPTIONAL"          { return TokenSelect(yylval, !Is_WORD(context),OPTIONAL_t, WORD_t); }
"PDV"               { return TokenSelect(yylval, !Is_WORD(context),PDV,WORD_t); }
"PLUS-INFINITY"     { return PLUS_INFINITY; }
"PRESENT"           { return TokenSelect(yylval, !Is_WORD(context),PRESENT,WORD_t); }
"PrintableString"   { return PrintableString; }
"PRIVATE"           { return TokenSelect(yylval, !Is_WORD(context),PRIVATE,WORD_t); }
"REAL"              { return REAL; }
"SEQUENCE"          { return SEQUENCE; }
"SET"               { return SET; }
"SIZE"              { return TokenSelect(yylval, !Is_WORD(context),SIZE_t,WORD_t); }
"STRING"            { return TokenSelect(yylval, !Is_WORD(context),STRING,WORD_t); }
"SYNTAX"            { return TokenSelect(yylval, !Is_WORD(context),SYNTAX,WORD_t); }
"T61String"         { return T61String; }
"TAGS"              { return TokenSelect(yylval, !Is_WORD(context),TAGS,WORD_t); }
"TeletexString"     { return TeletexString; }
"TRUE"              { return TRUE_t; }
"TYPE"				{ return TokenSelect(yylval, !Is_WORD(context),TYPE_t,WORD_t); }
"UNION"             { return UNION; }
"UNIQUE"            { return TokenSelect(yylval, !Is_WORD(context),UNIQUE,WORD_t); }
"UNIVERSAL"         { return TokenSelect(yylval, !Is_WORD(context),UNIVERSAL,WORD_t); }
"UniversalString"   { return UniversalString; }
"VALUE"				{ return TokenSelect(yylval, !Is_WORD(context),VALUE,WORD_t); }
"VideotexString"    { return VideotexString; }
"VisibleString"     { return VisibleString; }
"WITH"              { return TokenSelect(yylval, !Is_WORD(context),WITH,WORD_t); }
                                                              

"/*" {
    BEGIN(multiline_comment);
  }
<multiline_comment>"*/"  {
    BEGIN(INITIAL);
  }
<multiline_comment>\n  {
    lineNumber++;
  }
<multiline_comment>. {
}

"--" {
    BEGIN(comment_line);
  }

<comment_line>"--"  {
    BEGIN(INITIAL);
  }

<comment_line>\n  {
    lineNumber++;
    BEGIN(INITIAL);
  }

<comment_line><<EOF>> {
    BEGIN(INITIAL);
}

<comment_line>. {
}

"--<" {
   BEGIN(directive);
}

<directive>">--" {
   BEGIN(INITIAL);
}

<directive>"ASN1.Remove" {
   BEGIN(remove_directive); 
}

<remove_directive>([A-Z]|([A-Z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))+"."+([A-Z]|([A-Z][a-zA-Z0-9_-]*[a-zA-Z0-9_])) {
   AddRemoveItem(yytext);
}

<remove_directive>">--" {
   BEGIN(INITIAL);
}

<remove_directive>. {
}

<directive>. {
}

"@"  { return '@'; }

"{"  {
	int result =  context->BraceTokenContext;
	switch (context->BraceTokenContext)
	{
	case OBJECTSET_BRACE:
		context->BraceTokenContext = OBJECT_BRACE;
		break;
	case OBJECT_BRACE:
	case VALUESET_BRACE:
		context->BraceTokenContext = '{';
		break;
	}
	return result; 
}

"!"  { return '!'; }

"}"  { return '}'; }

"["  { return '['; }

"]"  { return ']'; }

"("  { return '('; }

")"  { return ')'; }

"<"  { return '<'; }

">"  { return '>'; }

"|"  { return '|'; }

";"  { return ';'; }

"+"  { return '+'; }

"-"  { return '-'; }

"*"  { return '*'; }

"/"  { return '/'; }

","  { return ','; }

"."  { return '.'; }

":"  { return ':'; }

"="  { return '='; }

'|'  { return '|'; }

"^"  { return '^'; }


(\"[^\"]*\")(\"[^\"]*\")*  {
    std::string s = yytext;
    size_t pos;
    while ((pos = s.find('\n')) != -1) {
      size_t start = pos;
      while (start > 0 && isspace(s[start-1]))
	    start--;
      while (isspace(s[pos]))
	    pos++;
      s.replace(start, pos - start," ");
      lineNumber++;
    }
    yylval->sval = new std::string(s);
    return CSTRING;
  }

[0-9][0-9]*  {
	if (context->InOIDContext) {
		yylval->sval = new std::string(yytext);
		return OID_INTEGER;
	}
	else {
      const char * ptr = yytext;
      yylval->ival = 0;
      while (*ptr != '\0')
        yylval->ival = yylval->ival * 10 + *ptr++ - '0';
      return INTEGER;
	}
  }

['][01]*[']B {
  yylval->sval = new std::string(yytext);
  return context->IdentifierTokenContext==BIT_IDENTIFIER ? BS_BSTRING : BSTRING;
}

['][0-9a-zA-Z]*[']H {
  yylval->sval = new std::string(yytext);
  return context->IdentifierTokenContext==BIT_IDENTIFIER ? BS_HSTRING : HSTRING;
}

([a-z]|([a-z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {
    yylval->sval = new std::string(yytext);
    return GetLowerCaseToken(context, yytext);
  }

"&"([a-z]|([a-z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {   /* See X.681 section 7.5 */
    yylval->sval = new std::string(yytext);
	int result;
	if (context->InformationFromObjectContext && (result = context->InformationFromObjectContext->GetFieldToken(yytext)))
		return result;
		
    return fieldreference;
  }

"&"([A-Z]|([A-Z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {	  /* See X.681 section 7.4 */
    yylval->sval = new std::string(yytext);
	int result;
	if (context->InformationFromObjectContext && (result = context->InformationFromObjectContext->GetFieldToken(yytext)))
		return result;
    return FieldReference;
  }


([A-Z]|([A-Z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {	  /* See X.680 section 9.2 */
    yylval->sval = new std::string(yytext);
	return GetUpperCaseToken(context, yytext);
  }

[ \t\r]  { }

\n {
    lineNumber++;
  }

.  { std::cerr << StdError(Fatal) << "unknown token " << yytext << std::endl; }


%%
/* 
 * $Log: asn_lex.l,v $
 * Revision 1.6  2011/08/09 18:12:43  arunasr
 * Genesys fixes: 3.0 release candidate
 *
 *  /main/3 2009/10/13 15:51:27 BST arunasr
 *     UNCTime added; compiler warnings cleanup
 * Revision 1.3  2006/05/12 20:50:09  arunasr
 * UTCTime added
 *
 * Revision 1.2  2005/09/14 10:00:32  arunasr
 * BSTRING and HSTRING parsing added
 *
 * Revision 1.1.1.1  2002/11/05 14:07:03  arunasr
 * no message
 *
 * Revision 1.4  2002/07/02 02:03:25  mangelo
 * Remove Pwlib dependency
 *
 * Revision 1.3  2001/09/07 22:36:49  mangelo
 * add Log keyword substitution
 *
 *
 * March, 2001. Huang-Ming Huang
 *            Add support for Information Object Class.
 */
