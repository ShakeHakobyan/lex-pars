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

"if"       { printf("<'if' , keyword>\n"); return IF; }
"else"     { printf("<'else' , keyword>\n"); return ELSE; }
"while"    { printf("<'while' , keyword>\n"); return WHILE; }
"for"      { printf("<'for' , keyword>\n"); return FOR; }
"print"    { printf("<'print' , keyword>\n"); return PRINT; }
"fn"       { printf("<'fn' , keyword>\n"); return FN; }

{DIGIT}+   { printf("<num, NUMBER>\n"); yylval.num = atof(yytext); return NUMBER; }
{LETTER}[0-9a-zA-Z]* { printf("<id, ID>\n"); yylval.id = strdup(yytext); return ID; }

[ \t\f\r]  { /* skip whitespace */ }

"\n"       { printf("<'\\n' , NEWLINE>\n"); line_num++; return NEWLINE; }

"+"        { printf("<'+' , PLUS>\n"); return PLUS;  }
"-"        { printf("<'-' , MINUS>\n"); return MINUS;  }
"*"        { printf("<'*' , TIMES>\n"); return TIMES;  }
"/"        { printf("<'/' , DIVIDED>\n"); return DIVIDED;  }
"="        { printf("<'=' , EQUALS>\n"); return EQUALS; }
";"        { printf("<';' , SEMICOLON>\n"); return SEMICOLON; }
","        { printf("<',' , COMMA>\n"); return COMMA; }

"("        { printf("<'(' , LPAREN>\n"); return LPAREN; }
")"        { printf("<')' , RPAREN>\n"); return RPAREN; }
"["        { printf("<'[' , LBRACKET>\n"); return LBRACKET; }
"]"        { printf("<']' , RBRACKET>\n"); return RBRACKET; }
"<"        { printf("<'<' , LANGLE>\n"); return LANGLE; }
">"        { printf("<'>' , RANGLE>\n"); return RANGLE; }

":)"       { printf("<':)' , GT_ZERO>\n"); return GT_ZERO; }
":("       { printf("<':(' , LT_ZERO>\n"); return LT_ZERO; }
":|"       { printf("<':|' , EQ_ZERO>\n"); return EQ_ZERO; }

.          { printf("error token: %s on line %d\n", yytext, line_num); yymore(); }

%%
