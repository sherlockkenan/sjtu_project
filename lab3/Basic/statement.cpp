/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include <cctype>
#include<iostream>
#include "statement.h"
#include"parser.h"
#include"evalstate.h"
#include <cstdlib>

//#include <tokenscanner.h>
#include "../StanfordCPPLib/tokenscanner.h"
using namespace std;


/* Implementation of the Statement class */

Statement::Statement() {
   /* Empty */
}

Statement::~Statement() {
   /* Empty */
}

//-------------------------------------------------------------------------------------
Remstmt::Remstmt(string st){
	var = st;
}

Remstmt::~Remstmt(){

}

void Remstmt::execute(EvalState & state){
}

StatementType Remstmt::get_type(){
	return REM;
}
//-------------------------------------------------------------------------------------


Letstmt::Letstmt(TokenScanner & scanner){

	exp = parseExp(scanner);
}

Letstmt::~Letstmt(){

}

void Letstmt::execute(EvalState & state){
	exp->eval(state);
}


StatementType Letstmt::get_type(){
	return LET;
}

//-------------------------------------------------------------------------------------


Printstmt::Printstmt(TokenScanner & scanner){
	exp = parseExp(scanner);
}

Printstmt::~Printstmt(){

}

void Printstmt::execute(EvalState & state){
	int value = exp->eval(state);
	cout << value << endl;
}


StatementType Printstmt::get_type(){
	return PRINT;
}



//-------------------------------------------------------------------------------------


Inputstmt::Inputstmt(string st){
	var = st;
}

Inputstmt::~Inputstmt(){

}

void Inputstmt::execute(EvalState & state){
	cout << " ? ";
	int num;
	
	
	
	string token;
	getline(cin, token);
		
	if (token[0] != '-' && !isdigit(token[0]))
	{
		cout << "INVALID NUMBER" << endl;
			
		execute(state);
		return;

	}
	else{
		for (int i = 1; i < token.size();++i)
		{
			if (!isdigit(token[i]))
			{
				cout << "INVALID NUMBER" << endl;
				execute(state);
				return;
			}

		}
		num  = atoi(token.c_str());
		state.setValue(var, num);
		//getchar();                                              //读入换行符，防止出错
	}	
	
}


StatementType Inputstmt::get_type(){
	return INPUT;
}



//-------------------------------------------------------------------------------------


Gotostmt::Gotostmt(int num){
	line_num = num;
}

Gotostmt::~Gotostmt(){

}

void Gotostmt::execute(EvalState & state){
}
void Gotostmt::execute(EvalState & state,int&current_line){
	current_line = line_num;
}
StatementType Gotostmt::get_type(){
	return GOTO;
}


//-------------------------------------------------------------------------------------


Ifstmt::Ifstmt(Expression *lexp, Expression *rexp, string cmp,int num){
	Ifstmt::lexp = lexp;
	Ifstmt::rexp = rexp;
	Ifstmt::cmp = cmp;
	line_num = num;
	

}


Ifstmt::~Ifstmt(){

}
void Ifstmt::execute(EvalState & state){};
void Ifstmt::execute(EvalState & state,int&current_line){
	int lvalue = lexp->eval(state);
	int rvalue = rexp->eval(state);

	Gotostmt gotostmt(line_num);

	if (cmp == ">"){
		if (lvalue > rvalue)
		{
			gotostmt.execute(state, current_line);
			return;
		}
	}
	if (cmp == "<"){
		if (lvalue < rvalue)
		{
			gotostmt.execute(state, current_line);
			return;
		}
	}
	if (cmp == "="){
		if (lvalue == rvalue)
		{
			gotostmt.execute(state, current_line);
			return;
		}
	}
	current_line = -2;                                         //表示条件都没满足

}


StatementType Ifstmt::get_type(){
	return IF;
}




//-------------------------------------------------------------------------------------


Endstmt::Endstmt(){
	
}

Endstmt::~Endstmt(){

}

void Endstmt::execute(EvalState & state){

}


StatementType Endstmt::get_type(){
	return END;
}
