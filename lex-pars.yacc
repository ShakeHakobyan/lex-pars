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
  list<char *> *idlist;
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
%token <fn> FN
%token NEWLINE
%token PLUS
%token MINUS
%token TIMES
%token DIVIDED
%token <cmp> EQUALS
%token GT_ZERO LT_ZERO EQ_ZERO
%token LPAREN RPAREN
%token LBRACKET RBRACKET
%token LANGLE RANGLE
%token SEMICOLON COMMA

%type <expnode> exp funccall
%type <stmts> stmtlist
%type <st> stmt fundecl
%type <idlist> params
%type <prog> program

%left PLUS MINUS
%left TIMES DIVIDED

%%

program:
    stmtlist {
      $$ = new pgm($1);
      root = $$;
    }
;

stmtlist:
    stmtlist NEWLINE {
      $$ = $1;
    }
  | stmtlist stmt {
      $$ = $1;
      $1->push_back($2);
    }
  | stmtlist fundecl {
        $$ = $1;
        $1->push_back($2);
      }
  | stmtlist error NEWLINE {
      $$ = $1;
      yyclearin;
    }
  | /* empty */ {
      $$ = new list<statement *>();
    }
;

stmt:
    ID EQUALS exp SEMICOLON {
      $$ = new assignment_stmt($1, $3);
    }
  | PRINT ID SEMICOLON {
      $$ = new print_stmt($2);
    }
  | IF LPAREN exp RPAREN stmt ELSE stmt {
      $$ = new if_else_stmt($3, $5, $7);
    }
  | WHILE LPAREN exp RPAREN stmt {
      $$ = new while_stmt($3, $5);
    }
  | FOR LPAREN stmt stmt exp RPAREN stmt {
      $$ = new for_stmt($3, $4, $5, $7);
    }
  | LBRACKET stmtlist RBRACKET {
      $$ = new stmtlist($2);
    }
  | exp SEMICOLON {
      $$ = new assignment_stmt("_", $1);
      printf("ova senc kod grum\n");
    }
  | stmt NEWLINE {
      $$ = $1;
    }
;

fundecl:
    FN ID LANGLE params RANGLE LBRACKET stmtlist RBRACKET {
      $$ = new function_decl_stmt($2, $4, $7);
    }
;

params:
    ID {
      $$ = new list<char*>();
      $$->push_back(strdup($1));
    }
  | params COMMA ID {
      $$ = $1;
      $$->push_back(strdup($3));
    }
  | /* empty */ {
      $$ = new list<char*>();
    }
;

exp:
    exp PLUS exp {
      $$ = new plus_node($1, $3);
    }
  | exp MINUS exp {
      $$ = new minus_node($1, $3);
    }
  | exp TIMES exp {
      $$ = new times_node($1, $3);
    }
  | exp DIVIDED exp {
      $$ = new divided_node($1, $3);
    }
  | NUMBER {
      $$ = new number_node($1);
    }
  | ID {
      $$ = new id_node($1);
    }
  | LPAREN exp RPAREN {
      $$ = $2;
    }
  | exp GT_ZERO {
      $$ = new gt_zero_node($1);
    }
  | exp LT_ZERO {
      $$ = new lt_zero_node($1);
    }
  | exp EQ_ZERO {
      $$ = new eq_zero_node($1);
    }
  | funccall {
         $$ = $1;
       }
;

funccall:
    ID LANGLE params RANGLE {
      $$ = new function_call_node($1, $3);
    }
;

%%

int main() {
  yyparse();
  root->evaluate();
  root->print();
  //root->codegen();
  return 0;
}

void yyerror(char * s) {
  fprintf(stderr, "line %d: %s\n", line_num, s);
}
