%{

/*
 * asn_ref_lex.l
 *
 * ASN lexical analysis file for typereference, objectclassreference, valuereference, ...
 *
 * Copyright (c) 2001 Institute for Information Industry
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
 * The Initial Developer of the Original Code is Institute for Information Industry
 *
 * The code is modified by Genesys Telecommunications Labs UK, 2003-2011
 * Contributors: 
 *    Arunas Ruksnaitis <arunas.ruksnaitis@genesyslab.com>
 *    Rustam Mirzaev <rustam.mirzaev@genesyslab.com>
 *
 */

#include "main.h"
#include <iostream>
#include <algorithm>

#define YY_NEVER_INTERACTIVE 1

#include "asn_ref_grammar.tab.h"

#define FALSE 0
#define TRUE  1

#include <ctype.h>
#define yylval idlval
extern unsigned lineNumber;

int InParameterListContext = FALSE;

extern int LexEcho;
#define YY_USER_ACTION if (LexEcho) std::cout << idtext << std::flush;

extern "C" int yywrap() { return 1; }

int braceCount = 0;
int parenthesisCount = 0;

#define YY_DECL int yylex(YYSTYPE* yylval,YYLTYPE * yylloc, ParserContext* context)

%}

%x multiline_comment comment_line brace parenthesis square_bracket

digit [0-9]
integer {digit}+

%%
"::="								{ return ASSIGNMENT; }
"..."								{ return ELLIPSIS; }
"ABSENT"                           { return ABSENT; }
"ABSTRACT-SYNTAX"                  { return ABSTRACT_SYNTAX; }
"ALL"                              { return ALL; }
"ANY"                              { return ANY; }
"APPLICATION"                      { return APPLICATION; }
"AUTOMATIC"                        { return AUTOMATIC; }
"BEGIN"                            { return BEGIN_t; }
"BIT"                              { return BIT; }
"BMPString"                        { return BMPString; }
"BOOLEAN"                          { return BOOLEAN_t; }
"BY"                               { return BY; }
"CHARACTER"                        { return CHARACTER; }
"CHOICE"                           { return CHOICE; }
"CLASS"                            { return CLASS; }
"COMPONENT"                        { return COMPONENT; }
"COMPONENTS"                       { return COMPONENTS; }
"CONSTRAINED"                      { return CONSTRAINED; }
"DEFAULT"                          { return DEFAULT; }
"DEFINED"                          { return DEFINED; }
"DEFINITIONS"                      { return DEFINITIONS; }
"EMBEDDED"                         { return EMBEDDED; }
"END"                              { return END; }
"ENUMERATED"                       { return ENUMERATED; }
"EXCEPT"                           { return EXCEPT; }
"EXPLICIT"                         { return EXPLICIT; }
"EXTENSIBILITY"                    { return EXTENSIBILITY; }
"EXPORTS"                          { return EXPORTS; }
"EXTERNAL"                         { return EXTERNAL; }
"FALSE"                            { return FALSE_t; }
"FROM"                             { return FROM; }
"GeneralString"                    { return GeneralString; }
"GraphicString"                    { return GraphicString; }
"IA5String"                        { return IA5String; }
"TYPE-IDENTIFIER"                  { return TYPE_IDENTIFIER; }
"IDENTIFIER"                       { return IDENTIFIER_t; }
"IMPLICIT"                         { return IMPLICIT; }
"IMPLIED"							{ return IMPLIED; }
"IMPORTS"                          { return IMPORTS; }
"INCLUDES"                         { return INCLUDES; }
"INSTANCE"                         { return INSTANCE; }
"INTEGER"                          { return INTEGER_t; }
"INTERSECTION"                     { return INTERSECTION; }
"ISO646String"                     { return ISO646String; }
"MACRO"								{ return MACRO; }
"MAX"								{ return MAX; }
"MIN"								{ return MIN; }
"MINUS-INFINITY"					{ return MINUS_INFINITY; }
"NOTATION"							{ return NOTATION; }
"NULL"                             { return NULL_t; }
"NumericString"                    { return NumericString; }
"OBJECT"                           { return OBJECT; }
"OCTET"                            { return OCTET; }
"OID-IRI"							{ return OID_IRI; }
"OF"                               { return OF_t; }
"OPTIONAL"                         { return OPTIONAL_t; }
"PATTERN"							{ return PATTERN; }
"PDV"                              { return PDV; }
"PLUS-INFINITY"                    { return PLUS_INFINITY; }
"PRESENT"                          { return PRESENT; }
"PrintableString"                  { return PrintableString; }
"PRIVATE"                          { return PRIVATE; }
"REAL"                             { return REAL; }
"RELATIVE-OID"						{ return RELATIVE_OID; }
"RELATIVE-OID-IRI"					{ return RELATIVE_OID_IRI; }
"SEQUENCE"                         { return SEQUENCE; }
"SET"                              { return SET; }
"SETTINGS"							{ return SETTINGS; }
"SIZE"                             { return SIZE_t; }
"STRING"                           { return STRING; }
"SYNTAX"                           { return SYNTAX; }
"T61String"                        { return T61String; }
"TAGS"                             { return TAGS; }
"TeletexString"                    { return TeletexString; }
"TRUE"                             { return TRUE_t; }
"TYPE"								{ return TYPE_t; }
"UNION"                            { return UNION; }
"UNIQUE"                           { return UNIQUE; }
"UNIVERSAL"                        { return UNIVERSAL; }
"UniversalString"                  { return UniversalString; }
"UTF8String"                        { return UTF8String; }
"VALUE"								{ return VALUE; }
"VideotexString"                   { return VideotexString; }
"VisibleString"                    { return VisibleString; }
"WITH"                             { return WITH; }

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
<comment_line>. {
}
<comment_line><<EOF>> {
    BEGIN(INITIAL);
}

"@"  { return '@'; }

"{"  {
	BEGIN(brace);
	braceCount++;
	return '{';
}

<brace>"{" { braceCount++; }

<brace>\n {
    lineNumber++;
}

<brace>"}"  {
    if (--braceCount == 0)
	{
		BEGIN(INITIAL);
		return '}';
	}
}

<brace>. { 	/* eat anything inside brace */ }


"["  {
	BEGIN(square_bracket);
	return '[';
}

<square_bracket>\n {
    lineNumber++;
}

<square_bracket>"]"  {
    BEGIN(INITIAL);
	return ']';
}

<square_bracket>. {}

"("  {
    BEGIN(parenthesis);
	parenthesisCount++;
	return '(';
}

<parenthesis>")"  {
    if (--parenthesisCount == 0)
	{
		BEGIN(INITIAL);
		return ')';
	}
}

<parenthesis>"("  {
    parenthesisCount++;
}

<parenthesis>\n {
    lineNumber++;
}

<parenthesis>. { /* eat anything inside parenthesis */
}





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

['][01]*[']B {
   return BSTRING;
}

['][0-9a-zA-Z]*[']H {
   return HSTRING;
}


(\"[^\"]*\")(\"[^\"]*\")*  {
#ifdef __sun
    unsigned lineNum = 0;
    std::count(yytext, yytext+strlen(yytext), '\n', lineNum);
    lineNumber += lineNum;
#else
    lineNumber += std::count(yytext, yytext+strlen(yytext), '\n');
#endif
    return CSTRING;
  }

{integer} {
    const char * ptr = yytext;
    yylval->ival = 0;
    while (*ptr != '\0')
      yylval->ival = yylval->ival * 10 + *ptr++ - '0';
    return INTEGER;
}

"&"([a-zA-Z][a-zA-Z0-9_-]*[a-zA-Z0-9_]) {
    yylval->sval = new std::string(yytext);
    return FIELDREFERENCE;
}

([a-z]|([a-z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {
    yylval->sval = new std::string(yytext);
    return IDENTIFIER;
}


([A-Z]|([A-Z][A-Z0-9_-]*[A-Z0-9_]))  {
    yylval->sval = new std::string(yytext);
	int result;
	if (context->module)
	  if ((result = context->module->getIdentifierType(yytext)) == OBJECTCLASSREFERENCE)
	     return OBJECTCLASSREFERENCE;
	return WOULDBE_OBJECTCLASSREFERENCE;
}

([A-Z]|([A-Z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {
    yylval->sval = new std::string(yytext);
    return TYPEREFERENCE;
}

[ \t\r]  { }

\n {
    lineNumber++;
}


.  { std::cerr << StdError(Fatal) << "unknown token " << yytext << std::endl; }


%%
/*
 * $Log: asn_ref_lex.l,v $
 * Revision 1.5  2011/08/09 18:12:43  arunasr
 * Genesys fixes: 3.0 release candidate
 *
 * Revision 1.1  2002/11/05 14:07:03  arunasr
 * Initial revision
 *
 * Revision 1.3  2002/07/02 02:03:25  mangelo
 * Remove Pwlib dependency
 *
 * Revision 1.2  2001/09/07 22:37:32  mangelo
 * add Log keyword substitution
 *
 */
