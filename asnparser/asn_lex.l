%option bison-bridge
%option reentrant 
%option stack
%option bison-locations 
%option yylineno
%option outfile="asn_lex.cxx"
%option header-file="asn_lex.h"
%option never-interactive 

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
#include "asn_grammar.tab.h"
#include <ctype.h>

extern unsigned lineNumber;
extern int LexEcho;
extern const char *const yyname[];

#define YY_NEVER_INTERACTIVE 1

#define YY_HEADER_EXPORT_START_CONDITIONS 1
//#define YY_USER_ACTION if (LexEcho) std::cout << yytext << std::flush;

extern int yylex (YYSTYPE*,YYLTYPE*  ,yyscan_t, ParserContext*);
#define YY_DECL int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param, yyscan_t yyscanner, ParserContext* context)

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

extern "C" int yywrap(yyscan_t ) { return 1; }
bool dumpUpperCaseToken = false;
bool dumpLowerCaseToken = false;

int isUpper(const char* text)
{
	for (const char* p = text; *p != 0; ++p)
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

static int is_WORD(ParserContext* context, yyscan_t yyscanner)
{
	return (context->inWithSyntaxContext || 
	         (!context->classStack->empty() && context->classStack->top()->hasLiteral(yyget_text(yyscanner)))) 
			&& !context->parsingConstructedType;
}

static int getUpperCaseToken(ParserContext* context, const char* text)
{
     int result = 0;
	if (context->dummyParameters && (result = context->dummyParameters->getIdentifierType(text)) != -1) {
		result = result;goto exit;
	}

	if (context->module)	{
		if ((result = context->module->getIdentifierType(text)) != -1) {
			goto exit;
		} else 
		if (((context->inWithSyntaxContext && isUpper(text)) || 
		(!context->classStack->empty() && context->classStack->top()->hasLiteral(text))) && !context->parsingConstructedType) {
			result = WORD_t;
			goto exit;
		} else {
			for(auto module : modules) {
				if (module->getName() == text) {
				   result = MODULEREFERENCE;break;
				}
			}
			if (result == -1)
				result = TYPEREFERENCE;
		}
	} else
		result = MODULEREFERENCE;
	
exit:
	if (dumpUpperCaseToken)
		printf("getUpperCaseToken::token=%d, name=%s, value=%s\n", result, yyname[result - 255], text);
	return result;
}

static int getLowerCaseToken(ParserContext* context, const char* text)
{
	int result = 0;

	if (true) {
	if (context->dummyParameters && (result = context->dummyParameters->getIdentifierType(text)) != -1)
		return result;

	if (context->inOIDLiteral && context->inOIDContext)
	    return OID_IDENTIFIER;

	if (context->module && (result = context->module->getIdentifierType(text)) != -1)
		return result;

    return context->identifierTokenContext;
	} else {
	if (context->inOIDLiteral && context->inOIDContext) {
	    result = OID_IDENTIFIER;
	} else

	if (context->dummyParameters && (result = context->dummyParameters->getIdentifierType(text)) != -1) {
	} else

	if (context->module && (result = context->module->getIdentifierType(text)) != -1) {
	} else

    result = context->identifierTokenContext;

exit:
	if (dumpLowerCaseToken)
		printf("getLowerCaseToken::token=%d, name=%s, value=%s\n", result, yyname[result - 255], text);
	return result;
	}
}

%}

%x multiline_comment comment_line directive remove_directive

digit [0-9]
integer {digit}+
real ({digit}+"."{digit}+)
exp ({integer}|{real})[eE]-?{integer}
lower [a-z]
upper [A-Z]

%%

"::="				{ return ASSIGNMENT; }
"..."				{ return ELLIPSIS; }
".."				{ return RANGE; }
"ABSENT"			{ return TokenSelect(yylval, !is_WORD(context, yyscanner) ,ABSENT, WORD_t, yyscanner); }              
"ABSTRACT-SYNTAX"   { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,ABSTRACT_SYNTAX, WORD_t, yyscanner); }  
"ALL"               { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,ALL, WORD_t, yyscanner); }  
"ANY"               { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,ANY, WORD_t, yyscanner); }  
"APPLICATION"       { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,APPLICATION, WORD_t, yyscanner); }  
"AUTOMATIC"         { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,AUTOMATIC, WORD_t, yyscanner); }  
"BEGIN"             { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,BEGIN_t, WORD_t, yyscanner); }  
"BIT"               { return BIT; }
"BMPString"         { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,BMPString, WORD_t, yyscanner); }  
"BOOLEAN"           { return BOOLEAN_t; }
"BY"                { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,BY, WORD_t, yyscanner); }  
"CHARACTER"         { return CHARACTER; }
"CHOICE"            { return CHOICE; }
"CLASS"             { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,CLASS, WORD_t, yyscanner); }  
"COMPONENT"         { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,COMPONENT, WORD_t, yyscanner); }  
"COMPONENTS"        { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,COMPONENTS, WORD_t, yyscanner); }  
"CONSTRAINED"       { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,CONSTRAINED, WORD_t, yyscanner); }  
"CONTAINING"		{ return CONTAINING; }
"DEFAULT"           { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,DEFAULT, WORD_t, yyscanner); }  
"DEFINED"           { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,DEFINED, WORD_t, yyscanner); }  
"DEFINITIONS"       { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,DEFINITIONS, WORD_t, yyscanner); }  
"EMBEDDED"          { return EMBEDDED; }
"END"               { return END; }
"ENUMERATED"        { return ENUMERATED; }
"EXCEPT"            { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,EXCEPT, WORD_t, yyscanner); }  
"EXPLICIT"          { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,EXPLICIT, WORD_t, yyscanner); }  
"EXPORTS"           { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,EXPORTS, WORD_t, yyscanner); }  
"EXTENSIBILITY"		{ return EXTENSIBILITY; }
"EXTERNAL"          { return EXTERNAL; }
"FALSE"             { return FALSE_t; }
"FROM"              { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,FROM, WORD_t, yyscanner); }  
"GeneralString"     { return GeneralString; }
"GeneralizedTime"   { return GeneralizedTime; }
"GraphicString"     { return GraphicString; }
"IA5String"         { return IA5String; }
"IDENTIFIER"        { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,IDENTIFIER_t, WORD_t, yyscanner); }  
"IMPLICIT"          { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,IMPLICIT, WORD_t, yyscanner); }  
"IMPLIED"			{ return IMPLIED; }
"IMPORTS"           { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,IMPORTS, WORD_t, yyscanner); }  
"INCLUDES"          { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,INCLUDES, WORD_t, yyscanner); }  
"INSTANCE"          { return INSTANCE; }
"INSTRUCTIONS"		{ return INSTRUCTIONS; }
"INTEGER"           { return INTEGER_t; }
"INTERSECTION"      { return INTERSECTION; }
"ISO646String"      { return ISO646String; }
"MACRO"				{ return TokenSelect(yylval, !is_WORD(context, yyscanner) ,MACRO, WORD_t, yyscanner); }  
"MAX"               { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,MAX, WORD_t, yyscanner); }  
"MIN"               { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,MIN, WORD_t, yyscanner); }  
"MINUS-INFINITY"    { return MINUS_INFINITY; }
"NOT-A-NUMBER"		{ return NOT_A_NUMBER; }
"NOTATION"			{ return TokenSelect(yylval, !is_WORD(context, yyscanner) ,NOTATION, WORD_t, yyscanner); }  
"NULL"              { return context->nullTokenContext; }
"NumericString"     { return NumericString; }
"OBJECT"            { return OBJECT; }
"OCTET"             { return OCTET; }
"OF"                { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,OF_t, WORD_t, yyscanner); }  
"OID-IRI"           { return OID_IRI; }
"OPTIONAL"          { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,OPTIONAL_t, WORD_t, yyscanner); }  
"PATTERN"			{ return PATTERN; }
"PDV"               { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,PDV, WORD_t, yyscanner); }  
"PLUS-INFINITY"     { return PLUS_INFINITY; }
"PRESENT"           { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,PRESENT, WORD_t, yyscanner); }  
"PRIVATE"           { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,PRIVATE, WORD_t, yyscanner); }  
"PrintableString"   { return PrintableString; }
"REAL"              { return REAL_t; }
"REAL"              { return REAL_t; }
"RELATIVE-OID"		{ return RELATIVE_OID; }
"RELATIVE-OID-IRI"	{ return RELATIVE_OID_IRI; }
"SEQUENCE"			{ return SEQUENCE; }
"SET"               { return SET; }
"SETTINGS"			{ return SETTINGS; }
"SIZE"              { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,SIZE_t, WORD_t, yyscanner); }  
"STRING"            { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,STRING, WORD_t, yyscanner); }  
"SYNTAX"            { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,SYNTAX, WORD_t, yyscanner); }  
"T61String"         { return T61String; }
"TAGS"              { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,TAGS, WORD_t, yyscanner); }  
"TRUE"              { return TRUE_t; }
"TYPE"				{ return TokenSelect(yylval, !is_WORD(context, yyscanner) ,TYPE_t, WORD_t, yyscanner); }  
"TYPE-IDENTIFIER"   { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,TYPE_IDENTIFIER, WORD_t, yyscanner); }  
"TeletexString"     { return TeletexString; }
"UNION"             { return UNION; }
"UNIQUE"            { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,UNIQUE, WORD_t, yyscanner); }  
"UNIVERSAL"         { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,UNIVERSAL, WORD_t, yyscanner); }  
"UTCTime"			{ return UTCTime; }
"UTF8String"		{ return TokenSelect(yylval, !is_WORD(context, yyscanner) ,UTF8String, WORD_t, yyscanner); }  
"UniversalString"   { return UniversalString; }
"VALUE"				{ return TokenSelect(yylval, !is_WORD(context, yyscanner) ,VALUE, WORD_t, yyscanner); }  
"VideotexString"    { return VideotexString; }
"VisibleString"     { return VisibleString; }
"WITH"              { return TokenSelect(yylval, !is_WORD(context, yyscanner) ,WITH, WORD_t, yyscanner); }                                                               
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
   addRemoveItem(yytext);
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
	int result =  context->braceTokenContext;
	switch (context->braceTokenContext)
	{
	case OBJECTSET_BRACE:
		context->braceTokenContext = OBJECT_BRACE;
		break;
	case OBJECT_BRACE:
	case OID_BRACE:
	case VALUESET_BRACE:
		context->braceTokenContext = '{';
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

"|"  { return '|'; }

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

{integer}  {
	if (context->inOIDContext) {
//		yylval->sval = new std::string(yytext);
		yylval->ival = atol(yytext);
		return OID_INTEGER;
	} else {
      const char * ptr = yytext;
      yylval->ival = 0;
      while (*ptr != '\0')
        yylval->ival = yylval->ival * 10 + *ptr++ - '0';
      return INTEGER;
	}
  }

{real}  {
	if (context->inOIDContext) {
//		yylval->sval = new std::string(yytext);
		yylval->ival = atol(yytext);
		return OID_INTEGER;
	} else {
      const char * ptr = yytext;
      yylval->ival = 0;
      while (*ptr != '\0') //FIXME real as integer for now
		if (isdigit(*ptr))
			yylval->ival = yylval->ival * 10 + *ptr++ - '0';
      return REAL;
	}
  }

['][01]*[']B {
  yylval->sval = new std::string(yytext);
  return context->identifierTokenContext==BIT_IDENTIFIER ? BS_BSTRING : BSTRING;
}

['][0-9a-zA-Z]*[']H {
  yylval->sval = new std::string(yytext);
  return context->identifierTokenContext==BIT_IDENTIFIER ? BS_HSTRING : HSTRING;
}

({lower}|({lower}[a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {
    yylval->sval = new std::string(yytext);
    return getLowerCaseToken(context, yytext);
  }

"&"({lower}|({lower}[a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {   /* See X.681 section 7.5 */
    yylval->sval = new std::string(yytext);
	int result;
	if (context->informationFromObjectContext && (result = context->informationFromObjectContext->getFieldToken(yytext)))
		return result;
		
    return fieldreference;
  }

"&"({upper}|({upper}[a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {	  /* See X.681 section 7.4 */
    yylval->sval = new std::string(yytext);
	int result;
	if (context->informationFromObjectContext && (result = context->informationFromObjectContext->getFieldToken(yytext)))
		return result;
    return FieldReference;
  }


({upper}|({upper}[a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {	  /* See X.680 section 9.2 */
    yylval->sval = new std::string(yytext);
	return getUpperCaseToken(context, yytext);
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
 *            add support for Information Object Class.
 */
