%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern YYSTYPE yylval;

char* new_str(const char* p, int len) {
  char *ret_buf;

  if (!p) return NULL;

  ret_buf = malloc(len+1);
  memcpy(ret_buf, p, len);
  ret_buf[len] = '\0';

  return ret_buf;
}
%}


CHAR  [A-Za-z\"\'\.]
CHNUM ({CHAR}|[0-9])
WORD  {CHAR}{CHNUM}*

/*
delim [" "\n\t]
whitespace {delim}+
words {}
*/
%%

add  { return keyword_add; }

[0-9]+ { yylval.number = atoi(yytext); return lit_number; }

{WORD} { yylval.string = new_str(yytext, yyleng); return lit_string; }

[,;:] { return yytext[0]; }

"\n" { return '\n'; }

%%

int yywrap() {
  return 1;
}
