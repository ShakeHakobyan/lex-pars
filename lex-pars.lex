%{
#include <string.h>
#include "lex-pars.h"
#include "y.tab.h"
#define yywrap() 1
#define YY_SKIP_YYWRAP

extern int line_num;

%}

DIGIT [0-9]
LETTER [a-zA-Z]

%%

"char"  { printf("<'char' , keyword>"); return CHAR; }

"false"   { printf("<'false' , keyword>"); return FALSE; }

"while"   { printf("<'while' , keyword>"); return WHILE; }

"if"   { printf("<'if' , keyword>"); return IF; }

"else"   { printf("<'else' , keyword>"); return ELSE; }

"print"   { printf("<'print' , keyword>"); return PRINT; }

{DIGIT}+  { printf("<num, NUMBER>\n"); yylval.num = atof(yytext); return NUMBER; }

{LETTER}[0-9a-zA-Z]* { printf("<id, ID>\n"); yylval.id = strdup(yytext); return ID; }

[ \t\f\r]		 // ignore white space 

"\n"    { printf("<'\\n' , NEWLINE>\n"); line_num++; return NEWLINE; }

"*"	{ printf("<'*' , TIMES>\n"); return TIMES;  }

"="     { printf("<'=' , EQUALS>\n"); return EQUALS; }

";"     { printf("<';' , SEMICOLON>\n"); return SEMICOLON; }

"("     { printf("<'(' , LPAREN>\n"); return LPAREN; }

")"     { printf("<')' , RPAREN>\n"); return RPAREN; }

. { printf("error token: %s on line %d\n", yytext, line_num); yymore(); }

%%


