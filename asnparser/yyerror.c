#include <stdio.h>
void yyerror(const char* str, ...) {
	fprintf(stderr, "%s near token %s\n", str, 0);
}