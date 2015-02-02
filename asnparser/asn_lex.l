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


extern unsigned lineNumber;

extern int LexEcho;

#define YY_NEVER_INTERACTIVE 1

#define YYDEBUG 1

#define YY_HEADER_EXPORT_START_CONDITIONS 1

#define YY_USER_ACTION if (LexEcho) std::cout << yytext << std::flush;

#define YY_DECL int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param, yyscan_t yyscanner, ParserContext* context)

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

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

extern "C" int yywrap(yyscan_t ) { return 1; }
bool dumpUpperCaseToken = false;
bool dumpLowerCaseToken = false;

int IsUpper(const char* text)
{
	for (const char* p = text; *p != NULL; ++p)
		if (!(isupper(*p) || *p == '-' || *p == '_'))
			return FALSE;
	return TRUE;
}
char *yyget_text (yyscan_t yyscanner );

static int TokenSelect(YYSTYPE * yylval, int ctx, int token1, int token2, yyscan_t yyscanner)
{
  if (ctx)
    return token1;
  yylval->sval = new std::string(yyget_text(yyscanner));
  return token2;
}

static int Is_WORD(ParserContext* context, yyscan_t yyscanner)
{
	return (context->InWithSyntaxContext || 
	         (!context->classStack->empty() && context->classStack->top()->HasLiteral(yyget_text(yyscanner)))) 
			&& !context->ParsingConstructedType;
}

static int GetUpperCaseToken(ParserContext* context, const char* text)
{
     int result = 0;
	if (context->DummyParameters && (result = context->DummyParameters->GetIdentifierType(text)) != -1) {
		result = result;goto exit;
	}

	if (context->Module)	{
		if ((result = context->Module->GetIdentifierType(text)) != -1) {
			result = result;goto exit;
		} else 
		if (((context->InWithSyntaxContext && IsUpper(text)) || (!context->classStack->empty() && context->classStack->top()->HasLiteral(text))) && !context->ParsingConstructedType) {
			result = WORD_t;goto exit;
		} else {
			result = TYPEREFERENCE;
		}
	} else
		result = MODULEREFERENCE;
	
exit:
	if (dumpUpperCaseToken)
		printf("GetUpperCaseToken::token=%d, name=%s, value=%s\n", result, SYMBOLS[result - 255], text);
	return result;
}

static int GetLowerCaseToken(ParserContext* context, const char* text)
{
	int result = 0;

	if (context->InOIDContext) {
	    result = OID_IDENTIFIER;goto exit;
	}

	if (context->DummyParameters && (result = context->DummyParameters->GetIdentifierType(text)) != -1) {
		result = result;goto exit;
	}

	if (context->Module && (result = context->Module->GetIdentifierType(text)) != -1) {
		result = result;goto exit;
	}

    result = context->IdentifierTokenContext;

exit:
	if (dumpLowerCaseToken)
		printf("GetLowerCaseToken::token=%d, name=%s, value=%s\n", result, SYMBOLS[result - 255], text);
	return result;
}

%}
%option bison-locations 
%option bison-bridge
%option reentrant 
%option yylineno
%option outfile="asn_lex.cxx"
%option header-file="asn_lex.hxx"
%option stack
%option never-interactive

%x multiline_comment comment_line directive remove_directive

%%

"::="				{ return ASSIGNMENT; }

"ABSENT"			{ return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,ABSENT,WORD_t, yyscanner); }              
"ABSTRACT-SYNTAX"   { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,ABSTRACT_SYNTAX,WORD_t, yyscanner); }  
"ALL"               { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,ALL,WORD_t, yyscanner); }  
"ANY"               { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,ANY,WORD_t, yyscanner); }  
"APPLICATION"       { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,APPLICATION,WORD_t, yyscanner); }  
"AUTOMATIC"         { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,AUTOMATIC,WORD_t, yyscanner); }  
"BEGIN"             { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,BEGIN_t,WORD_t, yyscanner); }  
"BIT"               { return BIT; }
"BMPString"         { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,BMPString,WORD_t, yyscanner); }  
"BOOLEAN"           { return BOOLEAN_t; }
"BY"                { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,BY,WORD_t, yyscanner); }  
"CHARACTER"         { return CHARACTER; }
"CHOICE"            { return CHOICE; }
"CLASS"             { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,CLASS,WORD_t, yyscanner); }  
"COMPONENT"         { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,COMPONENT,WORD_t, yyscanner); }  
"COMPONENTS"        { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,COMPONENTS,WORD_t, yyscanner); }  
"CONSTRAINED"       { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,CONSTRAINED,WORD_t, yyscanner); }  
"DEFAULT"           { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,DEFAULT,WORD_t, yyscanner); }  
"DEFINED"           { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,DEFINED,WORD_t, yyscanner); }  
"DEFINITIONS"       { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,DEFINITIONS,WORD_t, yyscanner); }  
"EMBEDDED"          { return EMBEDDED; }
"END"               { return END; }
"ENUMERATED"        { return ENUMERATED; }
"EXCEPT"            { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,EXCEPT,WORD_t, yyscanner); }  
"EXPLICIT"          { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,EXPLICIT,WORD_t, yyscanner); }  
"EXPORTS"           { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,EXPORTS,WORD_t, yyscanner); }  
"EXTERNAL"          { return EXTERNAL; }
"FALSE"             { return FALSE_t; }
"FROM"              { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,FROM,WORD_t, yyscanner); }  
"GeneralString"     { return GeneralString; }
"GraphicString"     { return GraphicString; }
"UTCTime"			{ return UTCTime; }
"GeneralizedTime"   { return GeneralizedTime; }
"IA5String"         { return IA5String; }
"TYPE-IDENTIFIER"   { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,TYPE_IDENTIFIER,WORD_t, yyscanner); }  
"IDENTIFIER"        { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,IDENTIFIER_t,WORD_t, yyscanner); }  
"IMPLICIT"          { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,IMPLICIT,WORD_t, yyscanner); }  
"IMPORTS"           { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,IMPORTS,WORD_t, yyscanner); }  
"INCLUDES"          { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,INCLUDES,WORD_t, yyscanner); }  
"INSTANCE"          { return INSTANCE; }
"INTEGER"           { return INTEGER_t; }
"INTERSECTION"      { return INTERSECTION; }
"ISO646String"      { return ISO646String; }
"MACRO"				{ return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,MACRO,WORD_t, yyscanner); }  
"MAX"               { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,MAX,WORD_t, yyscanner); }  
"MIN"               { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,MIN,WORD_t, yyscanner); }  
"MINUS-INFINITY"    { return MINUS_INFINITY; }
"NOTATION"			{ return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,NOTATION,WORD_t, yyscanner); }  
"NULL"              { return context->NullTokenContext; }
"NumericString"     { return NumericString; }
"OBJECT"            { return OBJECT; }
"OCTET"             { return OCTET; }
"OF"                { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,OF_t,WORD_t, yyscanner); }  
"OPTIONAL"          { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,OPTIONAL_t, WORD_t, yyscanner); }  
"PDV"               { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,PDV,WORD_t, yyscanner); }  
"PLUS-INFINITY"     { return PLUS_INFINITY; }
"PRESENT"           { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,PRESENT,WORD_t, yyscanner); }  
"PrintableString"   { return PrintableString; }
"PRIVATE"           { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,PRIVATE,WORD_t, yyscanner); }  
"REAL"              { return REAL; }
"SEQUENCE"          { return SEQUENCE; }
"SET"               { return SET; }
"SIZE"              { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,SIZE_t,WORD_t, yyscanner); }  
"STRING"            { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,STRING,WORD_t, yyscanner); }  
"SYNTAX"            { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,SYNTAX,WORD_t, yyscanner); }  
"T61String"         { return T61String; }
"TAGS"              { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,TAGS,WORD_t, yyscanner); }  
"TeletexString"     { return TeletexString; }
"TRUE"              { return TRUE_t; }
"TYPE"				{ return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,TYPE_t,WORD_t, yyscanner); }  
"UNION"             { return UNION; }
"UNIQUE"            { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,UNIQUE,WORD_t, yyscanner); }  
"UNIVERSAL"         { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,UNIVERSAL,WORD_t, yyscanner); }  
"UniversalString"   { return UniversalString; }
"VALUE"				{ return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,VALUE,WORD_t, yyscanner); }  
"VideotexString"    { return VideotexString; }
"VisibleString"     { return VisibleString; }
"WITH"              { return TokenSelect(yylval, !Is_WORD(context, yyscanner) ,WITH,WORD_t, yyscanner); }  
                                                              

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
