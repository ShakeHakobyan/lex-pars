#include <iostream>
#include <list>
#include <map>
#include <stdlib.h>
#include <string>

using namespace std;

class exp_node {
public:
  int num;
  virtual void print() = 0;
  virtual int evaluate() = 0;
};

class operator_node : public exp_node {
public:
  exp_node *left;
  exp_node *right;
  operator_node(exp_node *L, exp_node *R);
};

class number_node : public exp_node {

public:
  number_node(int value);
  void print();
  int evaluate();
};

class id_node : public exp_node {
protected:
  string id;

public:
  id_node(string value);
  void print();
  int evaluate();
};

class plus_node : public operator_node {
public:
  plus_node(exp_node *L, exp_node *R);
  void print();
  int evaluate();
};

class times_node : public operator_node {
public:
  times_node(exp_node *L, exp_node *R);
  void print();
  int evaluate();
};

class minus_node : public operator_node {
public:
  minus_node(exp_node *L, exp_node *R);
  void print();
  int evaluate();
};

class divided_node : public operator_node {
public:
  divided_node(exp_node *L, exp_node *R);
  void print();
  int evaluate();
};

class statement {
public:
  virtual void print() {}
  virtual void evaluate() = 0;
};

class assignment_stmt : public statement {
protected:
  string id;
  exp_node *exp;

public:
  assignment_stmt(string name, exp_node *expression);
  void print();
  void evaluate();
};

class print_stmt : public statement {
protected:
  string id;

public:
  print_stmt(string id);
  void print();
  void evaluate();
};

class if_else_stmt : public statement {
protected:
  exp_node *exp;
  statement *stmt_true;
  statement *stmt_false;

public:
  if_else_stmt(exp_node *expression, statement *statement_true,
               statement *statement_false);
  void print();
  void evaluate();
};

class while_stmt : public statement {
protected:
  exp_node *exp;
  statement *stmt;

public:
  while_stmt(exp_node *expression, statement *statement);
  void print();
  void evaluate();
};

class for_stmt : public statement {
protected:
  statement *init;
  statement *step;
  exp_node *cond;
  statement *stmt;

public:
  for_stmt(statement *initial, statement *step, exp_node *condition,
           statement *statement);
  void print();
  void evaluate();
};

class stmtlist : public statement {
protected:
  list<statement *> *stmts;

public:
  stmtlist(list<statement *> *stmtlist);
  void evaluate();
  void print();
};

class pgm {
protected:
  stmtlist *stmts;

public:
  pgm(list<statement *> *stmtlist);
  void evaluate();
  void print();
};

extern map<string, int> idTable;
extern pgm *root;
