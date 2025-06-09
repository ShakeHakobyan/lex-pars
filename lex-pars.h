#ifndef LEX_PARS_H
#define LEX_PARS_H

#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <stdlib.h>
#include <string>

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

using namespace std;
using namespace llvm;

extern LLVMContext TheContext;
extern IRBuilder<> Builder;
extern unique_ptr<Module> TheModule;
extern map<string, Value *> NamedValues;

class exp_node {
public:
  int num;
  virtual void print() = 0;
  virtual int evaluate() = 0;
  virtual Value *codegen() = 0;
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
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class id_node : public exp_node {
protected:
  string id;

public:
  id_node(string value);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class plus_node : public operator_node {
public:
  plus_node(exp_node *L, exp_node *R);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class times_node : public operator_node {
public:
  times_node(exp_node *L, exp_node *R);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class minus_node : public operator_node {
public:
  minus_node(exp_node *L, exp_node *R);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class divided_node : public operator_node {
public:
  divided_node(exp_node *L, exp_node *R);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class gt_zero_node : public exp_node {
protected:
  exp_node *expr;

public:
  gt_zero_node(exp_node *e);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class lt_zero_node : public exp_node {
protected:
  exp_node *expr;

public:
  lt_zero_node(exp_node *e);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class eq_zero_node : public exp_node {
protected:
  exp_node *expr;

public:
  eq_zero_node(exp_node *e);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class statement {
public:
  virtual void print() {}
  virtual void evaluate() = 0;
  virtual void codegen() = 0;
};

class assignment_stmt : public statement {
protected:
  string id;
  exp_node *exp;

public:
  assignment_stmt(string name, exp_node *expression);
  void print() override;
  void evaluate() override;
  void codegen() override;
};

class print_stmt : public statement {
protected:
  string id;

public:
  print_stmt(string id);
  void print() override;
  void evaluate() override;
  void codegen() override;
};

class if_else_stmt : public statement {
protected:
  exp_node *exp;
  statement *stmt_true;
  statement *stmt_false;

public:
  if_else_stmt(exp_node *expression, statement *statement_true,
               statement *statement_false);
  void print() override;
  void evaluate() override;
  void codegen() override;
};

class while_stmt : public statement {
protected:
  exp_node *exp;
  statement *stmt;

public:
  while_stmt(exp_node *expression, statement *statement);
  void print() override;
  void evaluate() override;
  void codegen() override;
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
  void print() override;
  void evaluate() override;
  void codegen() override;
};

class function_decl_stmt : public statement {
protected:
  string name;
  list<char *> *params;
  list<statement *> *body;

public:
  function_decl_stmt(string name, list<char *> *params, list<statement *> *body);
  void print() override;
  void evaluate() override;
  void codegen() override;
};

class function_call_node : public exp_node {
protected:
  string name;
  list<char *> *args;

public:
  function_call_node(string name, list<char *> *args);
  void print() override;
  int evaluate() override;
  Value *codegen() override;
};

class stmtlist : public statement {
protected:
  list<statement *> *stmts;

public:
  stmtlist(list<statement *> *stmtlist);
  void evaluate() override;
  void print() override;
  void codegen() override;
};

class pgm {
protected:
  stmtlist *stmts;

public:
  pgm(list<statement *> *stmtlist);
  void evaluate();
  void print();
  void codegen();
};

extern map<string, int> idTable;
extern pgm *root;

#endif // LEX_PARS_H
