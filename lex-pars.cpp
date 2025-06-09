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

llvm::Value *number_node::codegen() {
  return ConstantInt::get(Type::getInt32Ty(TheContext), num, true);
}

id_node::id_node(string value) : id(value) {}

void id_node::print() { cout << id; }

int id_node::evaluate() {
  cout << "id_node: " << id << " = " << idTable[id] << endl;
  return idTable[id];
}

llvm::Value *id_node::codegen() {
  llvm::Value *V = NamedValues[id];
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

llvm::Value *plus_node::codegen() {
  llvm::Value *L = left->codegen();
  llvm::Value *R = right->codegen();
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

llvm::Value *times_node::codegen() {
  llvm::Value *L = left->codegen();
  llvm::Value *R = right->codegen();
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

llvm::Value *minus_node::codegen() {
  llvm::Value *L = left->codegen();
  llvm::Value *R = right->codegen();
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

llvm::Value *divided_node::codegen() {
  llvm::Value *L = left->codegen();
  llvm::Value *R = right->codegen();
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
  llvm::Value *Val = exp->codegen();
  if (!Val)
    return;

  llvm::Value *Variable = NamedValues[id];
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

llvm::FunctionCallee printfFunc = TheModule->getOrInsertFunction(
    "printf",
    llvm::FunctionType::get(
        llvm::IntegerType::getInt32Ty(TheContext),
        llvm::PointerType::get(llvm::Type::getInt8Ty(TheContext), 0), true));

void print_stmt::codegen() {
  llvm::Value *val = NamedValues[id];
  llvm::Value *load =
      Builder.CreateLoad(llvm::Type::getInt32Ty(TheContext), val, id);
  llvm::Value *formatStr = Builder.CreateGlobalStringPtr("%d\n");
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
  llvm::Value *CondV = exp->codegen();
  if (!CondV)
    return;

  CondV = Builder.CreateICmpNE(
      CondV,
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(TheContext), 0, true),
      "ifcond");

  llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();

  llvm::BasicBlock *ThenBB =
      llvm::BasicBlock::Create(TheContext, "then", TheFunction);
  llvm::BasicBlock *ElseBB =
      llvm::BasicBlock::Create(TheContext, "else", TheFunction);
  llvm::BasicBlock *MergeBB =
      llvm::BasicBlock::Create(TheContext, "ifcont", TheFunction);

  Builder.CreateCondBr(CondV, ThenBB, ElseBB);

  Builder.SetInsertPoint(ThenBB);
  stmt_true->codegen();
  Builder.CreateBr(MergeBB);

  Builder.SetInsertPoint(ElseBB);
  stmt_false->codegen();
  Builder.CreateBr(MergeBB);

  Builder.SetInsertPoint(MergeBB);
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
  llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();

  llvm::BasicBlock *CondBB =
      llvm::BasicBlock::Create(TheContext, "whilecond", TheFunction);
  llvm::BasicBlock *LoopBB =
      llvm::BasicBlock::Create(TheContext, "whileloop", TheFunction);
  llvm::BasicBlock *AfterBB =
      llvm::BasicBlock::Create(TheContext, "afterloop", TheFunction);

  Builder.CreateBr(CondBB);

  Builder.SetInsertPoint(CondBB);
  llvm::Value *CondV = exp->codegen();
  if (!CondV)
    return;
  CondV = Builder.CreateICmpNE(
      CondV,
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(TheContext), 0, true),
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
  llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();

  if (init)
    init->codegen();

  llvm::BasicBlock *CondBB =
      llvm::BasicBlock::Create(TheContext, "forcond", TheFunction);
  llvm::BasicBlock *LoopBB =
      llvm::BasicBlock::Create(TheContext, "forloop", TheFunction);
  llvm::BasicBlock *AfterBB =
      llvm::BasicBlock::Create(TheContext, "afterfor", TheFunction);

  Builder.CreateBr(CondBB);

  Builder.SetInsertPoint(CondBB);

  llvm::Value *CondV = cond->codegen();
  if (!CondV)
    return;

  CondV = Builder.CreateICmpNE(
      CondV,
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(TheContext), 0, true),
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

  std::error_code EC;
  llvm::raw_fd_ostream dest("output.ll", EC, llvm::sys::fs::OF_None);
  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message() << "\n";
    return;
  }
  TheModule->print(dest, nullptr);
  dest.close();
}

map<string, int> idTable;
