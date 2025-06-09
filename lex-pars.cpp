#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include "lex-pars.h"

using namespace std;

operator_node::operator_node(exp_node *L, exp_node *R) {
	left    = L;
	right   = R;
}

number_node::number_node(float value) {
	num = value;
}

void number_node:: print() {
	cout << num;
}

float number_node::evaluate() { 
	cout << "number_node: operand = " << num << endl;
	return num; }

id_node::id_node(string value) : id(value) {}

void id_node:: print() {
	cout << id;
}

float id_node::evaluate() { 
	cout << "id_node: " << id << " = " << idTable[id] << endl;
	return idTable[id]; 
}

plus_node::plus_node(exp_node *L, exp_node *R) : operator_node(L,R) {
}

void plus_node::print() {
	cout << "(";
	left->print();
	cout << " + ";
	right->print();
	cout << ")";
}

float plus_node::evaluate() {
	float left_num, right_num;

	left_num  = left->evaluate();
	right_num = right->evaluate();

	num = left_num + right_num;
	cout << "plus_node: " << left_num << " + " << right_num << " = " << num << "\n";
	return (num);
}

times_node::times_node(exp_node *L, exp_node *R) : operator_node(L,R) {
}

void times_node:: print() {
	cout << "(";
	left->print();
	cout << " * ";
	right->print();
	cout << ")";
}

float times_node::evaluate() {
	float left_num, right_num;

	left_num = left->evaluate();
	right_num = right->evaluate();

	num = left_num * right_num;
	cout << "times_node: " << left_num << " * " << right_num << " = " << num << "\n";
	return (num);
}

minus_node::minus_node(exp_node *L, exp_node *R) : operator_node(L, R) {
}

void minus_node::print(){
	cout<<"(";
	left->print();
	cout<<"-";
	right->print();
	cout<<")";
}

float minus_node::evaluate(){
	float left_num,right_num;

	left_num = left->evaluate();
	right_num = right->evaluate();

	num = left_num - right_num;
	cout<<"minus_node: "<<left_num<<"-"<<right_num<<"="<<num<<"\n";

	return num;
}

divided_node::divided_node(exp_node *L, exp_node *R) : operator_node(L, R) {
}

void divided_node::print(){
	cout<<"(";
	left->print();
	cout<<"-";
	right->print();
	cout<<")";
}

float divided_node::evaluate(){
	float left_num,right_num;

	left_num = left->evaluate();
	right_num = right->evaluate();

	num = left_num / right_num;
	cout<<"divided_node: "<<left_num<<"/"<<right_num<<"="<<num<<"\n";

	return num;
}

assignment_stmt::assignment_stmt(string name, exp_node *expression)
	: id(name), exp(expression) {}

	void assignment_stmt::print() {
		cout << id << " = ";
		exp->print();
		cout << endl;
	}

void assignment_stmt::evaluate() {
	float result = exp->evaluate();
	cout << "assignment_node: " << id << " = " << result << endl << endl;
	idTable[id] = result;
}

print_stmt::print_stmt (string name) : id(name) { }

void print_stmt::print() {
	cout << "print " << id <<endl;
}

void print_stmt::evaluate() {
	cout << "print_node: " << id << " = " << idTable[id] << endl << endl;
}

if_else_stmt::if_else_stmt(exp_node *expression, statement *statement_true, statement *statement_false)
	:  exp(expression), stmt_true(statement_true), stmt_false(statement_false) {}

void if_else_stmt::print() {
	cout << "if ";
	exp->print();
	cout << "\n";
	stmt_true->print();
	cout << "else\n";
	stmt_false->print();
}

void if_else_stmt::evaluate() {
    if(exp->evaluate()) {
		stmt_true->evaluate();
	} else {
		stmt_false->evaluate();
    }
	cout << "if else node" << endl << endl;
}

while_stmt::while_stmt(exp_node *expression, statement *statement)
	:  exp(expression), stmt(statement) {}

void while_stmt::print() {
	cout << "while (";
	exp->print();
	cout << " )\n";
	stmt->print();
}

void while_stmt::evaluate() {
	while(exp->evaluate()) {
		stmt->evaluate();
	}
	cout << "while node" << endl << endl;
}

pgm::pgm(list<statement *> *stmtList) : stmts(stmtList) {}

void pgm::evaluate() {
	list<statement *>::iterator stmtIter;
	for (stmtIter = stmts->begin(); stmtIter != stmts->end();
			stmtIter++) {
		(*stmtIter)->print();
		(*stmtIter)->evaluate();
	}
}

void pgm::print() {
	list<statement *>::iterator stmtIter;
        cout<<"\nGoing to print AST\n"; 
        for (stmtIter = stmts->begin(); stmtIter != stmts->end();
                        stmtIter++) {
                (*stmtIter)->print();
	}
        cout<<"\nAST printed \n";
}

map<string, float> idTable;
