#include "lex-pars.h"
#include <iostream>
#include <list>
#include <map>
#include <stdlib.h>
#include <string>

using namespace std;
using namespace llvm;

LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
unique_ptr<Module> TheModule = make_unique<Module>("main", TheContext);
map<string, Value *> NamedValues;

operator_node::operator_node(exp_node *L, exp_node *R) {
  left = L;
  right = R;
}

number_node::number_node(int value) { num = value; }

void number_node::print() { cout << num; }

int number_node::evaluate() {
  cout << "number_node: operand = " << num << endl;
  return num;
}

Value *number_node::codegen() {
  return ConstantInt::get(Type::getInt32Ty(TheContext), num, true);
}

id_node::id_node(string value) : id(value) {}

void id_node::print() { cout << id; }

int id_node::evaluate() {
  cout << "id_node: " << id << " = " << idTable[id] << endl;
  return idTable[id];
}

Value *id_node::codegen() {
  Value *V = NamedValues[id];
  return Builder.CreateLoad(Type::getInt32Ty(TheContext), V, id.c_str());
}

plus_node::plus_node(exp_node *L, exp_node *R) : operator_node(L, R) {}

void plus_node::print() {
  cout << "(";
  left->print();
  cout << " + ";
  right->print();
  cout << ")";
}

Value *plus_node::codegen() {
  Value *L = left->codegen();
  Value *R = right->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateAdd(L, R, "addtmp");
}

int plus_node::evaluate() {
  int left_num, right_num;

  left_num = left->evaluate();
  right_num = right->evaluate();

  num = left_num + right_num;
  cout << "plus_node: " << left_num << " + " << right_num << " = " << num
       << "\n";
  return (num);
}

times_node::times_node(exp_node *L, exp_node *R) : operator_node(L, R) {}

void times_node::print() {
  cout << "(";
  left->print();
  cout << " * ";
  right->print();
  cout << ")";
}

int times_node::evaluate() {
  int left_num, right_num;

  left_num = left->evaluate();
  right_num = right->evaluate();

  num = left_num * right_num;
  cout << "times_node: " << left_num << " * " << right_num << " = " << num
       << "\n";
  return (num);
}

Value *times_node::codegen() {
  Value *L = left->codegen();
  Value *R = right->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateMul(L, R, "multmp");
}

minus_node::minus_node(exp_node *L, exp_node *R) : operator_node(L, R) {}

void minus_node::print() {
  cout << "(";
  left->print();
  cout << "-";
  right->print();
  cout << ")";
}

int minus_node::evaluate() {
  int left_num, right_num;

  left_num = left->evaluate();
  right_num = right->evaluate();

  num = left_num - right_num;
  cout << "minus_node: " << left_num << "-" << right_num << "=" << num << "\n";

  return num;
}

Value *minus_node::codegen() {
  Value *L = left->codegen();
  Value *R = right->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateSub(L, R, "subtmp");
}

divided_node::divided_node(exp_node *L, exp_node *R) : operator_node(L, R) {}

void divided_node::print() {
  cout << "(";
  left->print();
  cout << "-";
  right->print();
  cout << ")";
}

int divided_node::evaluate() {
  int left_num, right_num;

  left_num = left->evaluate();
  right_num = right->evaluate();

  num = left_num / right_num;
  cout << "divided_node: " << left_num << "/" << right_num << "=" << num
       << "\n";

  return num;
}

Value *divided_node::codegen() {
  Value *L = left->codegen();
  Value *R = right->codegen();
  if (!L || !R)
    return nullptr;
  return Builder.CreateSDiv(L, R, "divtmp");
}

assignment_stmt::assignment_stmt(string name, exp_node *expression)
    : id(name), exp(expression) {}

void assignment_stmt::print() {
  cout << id << " = ";
  exp->print();
  cout << endl;
}

void assignment_stmt::evaluate() {
  int result = exp->evaluate();
  cout << "assignment_node: " << id << " = " << result << endl << endl;
  idTable[id] = result;
}

void assignment_stmt::codegen() {
  Value *Val = exp->codegen();
  if (!Val)
    return;

  Value *Variable = NamedValues[id];
  if (!Variable) {
    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                     TheFunction->getEntryBlock().begin());
    Variable = TmpB.CreateAlloca(Type::getInt32Ty(TheContext), 0, id.c_str());
    NamedValues[id] = Variable;
  }

  Builder.CreateStore(Val, Variable);
}

print_stmt::print_stmt(string name) : id(name) {}

void print_stmt::print() { cout << "print " << id << endl; }

void print_stmt::evaluate() {
  cout << "print_node: " << id << " = " << idTable[id] << endl << endl;
}

FunctionCallee printfFunc = TheModule->getOrInsertFunction(
    "printf",
    FunctionType::get(IntegerType::getInt32Ty(TheContext),
                      PointerType::get(Type::getInt8Ty(TheContext), 0), true));

void print_stmt::codegen() {
  Value *val = NamedValues[id];
  Value *load = Builder.CreateLoad(Type::getInt32Ty(TheContext), val, id);
  Value *formatStr = Builder.CreateGlobalStringPtr("%d\n");
  Builder.CreateCall(printfFunc, {formatStr, load});
}

if_else_stmt::if_else_stmt(exp_node *expression, statement *statement_true,
                           statement *statement_false)
    : exp(expression), stmt_true(statement_true), stmt_false(statement_false) {}

void if_else_stmt::print() {
  cout << "if ";
  exp->print();
  cout << "\n";
  stmt_true->print();
  cout << "else\n";
  stmt_false->print();
}

void if_else_stmt::evaluate() {
  if (exp->evaluate()) {
    stmt_true->evaluate();
  } else {
    stmt_false->evaluate();
  }
  cout << "if else node" << endl << endl;
}

void if_else_stmt::codegen() {
  Value *CondV = exp->codegen();
  if (!CondV)
    return;

  CondV = Builder.CreateICmpNE(
      CondV, ConstantInt::get(Type::getInt32Ty(TheContext), 0, true), "ifcond");

  Function *TheFunction = Builder.GetInsertBlock()->getParent();

  BasicBlock *ThenBB = BasicBlock::Create(TheContext, "then", TheFunction);
  BasicBlock *ElseBB = BasicBlock::Create(TheContext, "else", TheFunction);
  BasicBlock *MergeBB = BasicBlock::Create(TheContext, "ifcont", TheFunction);

  Builder.CreateCondBr(CondV, ThenBB, ElseBB);

  Builder.SetInsertPoint(ThenBB);
  stmt_true->codegen();
  Builder.CreateBr(MergeBB);

  Builder.SetInsertPoint(ElseBB);
  stmt_false->codegen();
  Builder.CreateBr(MergeBB);

  Builder.SetInsertPoint(MergeBB);
}

gt_zero_node::gt_zero_node(exp_node *e) : expr(e) {}

void gt_zero_node::print() {
  cout << "(>0 ";
  expr->print();
  cout << ")";
}

int gt_zero_node::evaluate() {
  int val = expr->evaluate();
  int result = val > 0 ? 1 : 0;
  cout << "gt_zero_node: " << val << " > 0 = " << result << endl;
  return result;
}

Value *gt_zero_node::codegen() {
  Value *val = expr->codegen();
  Value *zero = ConstantInt::get(Type::getInt32Ty(TheContext), 0, true);
  Value *cond = Builder.CreateICmpSGT(val, zero, "gtzero");
  return Builder.CreateZExt(cond, Type::getInt32Ty(TheContext), "boolToInt");
}

lt_zero_node::lt_zero_node(exp_node *e) : expr(e) {}

void lt_zero_node::print() {
  cout << "(<0 ";
  expr->print();
  cout << ")";
}

int lt_zero_node::evaluate() {
  int val = expr->evaluate();
  int result = val < 0 ? 1 : 0;
  cout << "lt_zero_node: " << val << " < 0 = " << result << endl;
  return result;
}

Value *lt_zero_node::codegen() {
  Value *val = expr->codegen();
  Value *zero = ConstantInt::get(Type::getInt32Ty(TheContext), 0, true);
  Value *cond = Builder.CreateICmpSLT(val, zero, "ltzero");
  return Builder.CreateZExt(cond, Type::getInt32Ty(TheContext), "boolToInt");
}

eq_zero_node::eq_zero_node(exp_node *e) : expr(e) {}

void eq_zero_node::print() {
  cout << "(=0 ";
  expr->print();
  cout << ")";
}

int eq_zero_node::evaluate() {
  int val = expr->evaluate();
  int result = val == 0 ? 1 : 0;
  cout << "eq_zero_node: " << val << " == 0 = " << result << endl;
  return result;
}

Value *eq_zero_node::codegen() {
  Value *val = expr->codegen();
  Value *zero = ConstantInt::get(Type::getInt32Ty(TheContext), 0, true);
  Value *cond = Builder.CreateICmpEQ(val, zero, "eqzero");
  return Builder.CreateZExt(cond, Type::getInt32Ty(TheContext), "boolToInt");
}

while_stmt::while_stmt(exp_node *expression, statement *statement)
    : exp(expression), stmt(statement) {}

void while_stmt::print() {
  cout << "while (";
  exp->print();
  cout << " )\n";
  stmt->print();
}

void while_stmt::evaluate() {
  while (exp->evaluate()) {
    stmt->evaluate();
  }
  cout << "while node" << endl << endl;
}

void while_stmt::codegen() {
  Function *TheFunction = Builder.GetInsertBlock()->getParent();

  BasicBlock *CondBB = BasicBlock::Create(TheContext, "whilecond", TheFunction);
  BasicBlock *LoopBB = BasicBlock::Create(TheContext, "whileloop", TheFunction);
  BasicBlock *AfterBB =
      BasicBlock::Create(TheContext, "afterloop", TheFunction);

  Builder.CreateBr(CondBB);

  Builder.SetInsertPoint(CondBB);
  Value *CondV = exp->codegen();
  if (!CondV)
    return;
  CondV = Builder.CreateICmpNE(
      CondV, ConstantInt::get(Type::getInt32Ty(TheContext), 0, true),
      "whilecond");

  Builder.CreateCondBr(CondV, LoopBB, AfterBB);

  Builder.SetInsertPoint(LoopBB);
  stmt->codegen();

  Builder.CreateBr(CondBB);

  Builder.SetInsertPoint(AfterBB);
}

for_stmt::for_stmt(statement *initial, statement *step, exp_node *condition,
                   statement *statement)
    : init(initial), step(step), cond(condition), stmt(statement) {}

void for_stmt::print() {
  cout << "for (\n    init: ";
  init->print();
  cout << "    step: ";
  step->print();
  cout << "    cond: ";
  cond->print();
  cout << ")";
  stmt->print();
}

void for_stmt::evaluate() {
  init->evaluate();
  while (cond->evaluate()) {
    stmt->evaluate();
    step->evaluate();
  }
  cout << "for node" << endl << endl;
}

void for_stmt::codegen() {
  Function *TheFunction = Builder.GetInsertBlock()->getParent();

  if (init)
    init->codegen();

  BasicBlock *CondBB = BasicBlock::Create(TheContext, "forcond", TheFunction);
  BasicBlock *LoopBB = BasicBlock::Create(TheContext, "forloop", TheFunction);
  BasicBlock *AfterBB = BasicBlock::Create(TheContext, "afterfor", TheFunction);

  Builder.CreateBr(CondBB);

  Builder.SetInsertPoint(CondBB);

  Value *CondV = cond->codegen();
  if (!CondV)
    return;

  CondV = Builder.CreateICmpNE(
      CondV, ConstantInt::get(Type::getInt32Ty(TheContext), 0, true),
      "forcond");

  Builder.CreateCondBr(CondV, LoopBB, AfterBB);

  Builder.SetInsertPoint(LoopBB);

  stmt->codegen();

  if (step)
    step->codegen();

  Builder.CreateBr(CondBB);

  Builder.SetInsertPoint(AfterBB);
}

stmtlist::stmtlist(list<statement *> *stmtList) : stmts(stmtList) {}

void stmtlist::evaluate() {
  for (auto stmt : *stmts) {
    stmt->print();
    stmt->evaluate();
  }
}

void stmtlist::print() {
  cout << "\n[\n";
  for (auto stmt : *stmts) {
    stmt->print();
  }
  cout << "] \n";
}

void stmtlist::codegen() {
  for (auto stmt : *stmts) {
    stmt->codegen();
  }
}

pgm::pgm(list<statement *> *stmtList) : stmts(new stmtlist(stmtList)) {}

void pgm::evaluate() { stmts->evaluate(); }

void pgm::print() {
  cout << "\nGoing to print AST\n";
  stmts->print();
  cout << "\nAST printed \n";
}

void pgm::codegen() {

  FunctionType *FT = FunctionType::get(Type::getInt32Ty(TheContext), false);
  Function *MainF =
      Function::Create(FT, Function::ExternalLinkage, "main", TheModule.get());

  BasicBlock *BB = BasicBlock::Create(TheContext, "entry", MainF);
  Builder.SetInsertPoint(BB);

  stmts->codegen();

  Builder.CreateRet(ConstantInt::get(Type::getInt32Ty(TheContext), 0));

  error_code EC;
  raw_fd_ostream dest("output.ll", EC, sys::fs::OF_None);
  if (EC) {
    errs() << "Could not open file: " << EC.message() << "\n";
    return;
  }
  TheModule->print(dest, nullptr);
  dest.close();
}

map<string, int> idTable;
