%{
#include <stdio.h>
%}

%union {
  char* string;
  int number;
}

%type <string> lit_string
%type <number> lit_number

%token keyword_add
%token lit_string
%token lit_number

%%

program : stmts
        ;

stmts : stmt_list terms;

stmt_list : stmt
          | stmt_list terms stmt
          ;

stmt : add_stmt
     ;

add_stmt : keyword_add lit_string
           {
             printf("add <%s>\n", $2);
           }
         ;

terms : term
      | terms term
      ;

term : ';' { yyerrok; }
     | '\n'
     ;

%%

#include "lex.yy.c"

int yyerror(char* msg)
{
  printf("Error: %s at line number:%d\n", msg, yylineno);
}

void main()
{
  yyparse();
}
