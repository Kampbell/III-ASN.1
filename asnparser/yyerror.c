#include <stdio.h>
#include <stdarg.h>
#include "main.h"

typedef int Yshort;

#include "asn_grammar.tab.h"

#ifdef __cplusplus
#define _C_ "C"
#else
#define _C_
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define inline
#endif /* C99 */
#endif /* C++ */
extern const char *const yyname[];


void yyerror(const char* str, ...) {
	va_list ap;
	int yychar;
	YYSTYPE yylval;
	int yyposn;

	va_start(ap, str);
	yychar = va_arg(ap, int);
	yylval = va_arg(ap, YYSTYPE);
	yyposn = va_arg(ap, int);
	va_end(ap);
	fprintf(stderr, "%s near token %s at position %d\n", str, yyname[yychar - 256], yyposn);
}
//     yyerror("syntax error", yychar, yylval, yyposn);
