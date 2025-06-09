%{
#include <iostream>
#include <string>
#include <stdlib.h>
#include <map>
#include <list>
#include "lex-pars.h"

using namespace std;

 pgm *root;

 int line_num = 1;

int yylex();
void yyerror(char * s);

%}

%start program

%union {
  int num;
  char *id;
  exp_node *expnode;
  list<statement *> *stmts;
  statement *st;
  pgm *prog;
}

%error-verbose

%token <num> NUMBER
%token <id> ID
%token <if> IF
%token <else> ELSE
%token <while> WHILE
%token <while> FOR
%token <print> PRINT
%token NEWLINE
%token PLUS
%token MINUS
%token TIMES
%token DIVIDED
%token <cmp> EQUALS 
%token LPAREN RPAREN
%token LBRACKET RBRACKET
%token SEMICOLON

%type <expnode> exp 
%type <stmts> stmtlist
%type <st> stmt
%type <prog> program

%left PLUS MINUS
%left TIMES DIVIDED

%%

program : stmtlist { $$ = new pgm($1); root = $$; }
;

stmtlist : stmtlist NEWLINE  
	   {
             $$ = $1;
           }
         | stmtlist stmt
            { 
              $$ = $1;
              $1->push_back($2);
            }
         | stmtlist error NEWLINE
	   { 
             $$ = $1;
             yyclearin; } 
         |  
           { $$ = new list<statement *>(); }  
;

stmt: ID EQUALS exp SEMICOLON { 
  $$ = new assignment_stmt($1, $3);
 }
       
| PRINT ID SEMICOLON {
  $$ = new print_stmt($2);
 }

| IF LPAREN exp RPAREN stmt ELSE  stmt {
   $$= new if_else_stmt($3, $5, $7);
 }

| WHILE LPAREN exp RPAREN stmt {
   $$= new while_stmt($3, $5);
 }

 | FOR LPAREN stmt stmt exp RPAREN stmt {
    $$= new for_stmt($3, $4, $5, $7);
  }
  | LBRACKET stmtlist RBRACKET {
    $$ = new stmtlist($2);
  }

 | exp SEMICOLON {
    $$ = new assignment_stmt("_", $1); printf("ova senc kod grum\n");
 }

| stmt NEWLINE

 ;

exp:	exp PLUS exp {
	  $$ = new plus_node($1, $3); }

	|  exp MINUS exp {
      $$ = new minus_node($1, $3); }
    |	exp TIMES exp {
	  $$ = new times_node($1, $3); }

	|   exp DIVIDED exp {
        $$ = new divided_node($1, $3); }

	|	NUMBER {
	  $$ = new number_node($1); }

	|       ID {
  	  $$ = new id_node($1); }

  	|  LPAREN exp RPAREN {
        $$ = $2;
     }

;
 
%%
int main()
{
  yyparse();
  root->evaluate();
  root->print();
}

void yyerror(char * s)
{
  fprintf(stderr, "line %d: %s\n", line_num, s);
}

