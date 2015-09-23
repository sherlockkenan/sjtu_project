/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface specified in Chapter 17, which is an
 * excellent model for the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include "evalstate.h"
#include "exp.h"
//#include<tokenscanner.h>
#include"../StanfordCPPLib/tokenscanner.h"

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

enum StatementType { REM, LET, PRINT, INPUT, GOTO, IF, END };

class Statement {

public:

/*
 * Constructor: Statement
 * ----------------------
 * The base class constructor is empty.  Each subclass must provide
 * its own constructor.
 */

   Statement();

/*
 * Destructor: ~Statement
 * Usage: delete stmt;
 * -------------------
 * The destructor deallocates the storage for this expression.
 * It must be declared virtual to ensure that the correct subclass
 * destructor is called when deleting a statement.
 */

   virtual ~Statement();

/*
 * Method: execute
 * Usage: stmt->execute(state);
 * ----------------------------
 * This method executes a BASIC statement.  Each of the subclasses
 * defines its own execute method that implements the necessary
 * operations.  As was true for the expression evaluator, this
 * method takes an EvalState object for looking up variables or
 * controlling the operation of the interpreter.
 */

   virtual void execute(EvalState & state) = 0;
   virtual StatementType get_type() = 0;

};

/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */


class Remstmt : public Statement{

public:
	Remstmt(string str);
	~Remstmt();

	
	void execute(EvalState & state);
	StatementType get_type();
private:
	
	string var;


};



//--------------------------------------------------------------------------------



class Letstmt : public Statement{

public:
	Letstmt(TokenScanner& scanner);
	~Letstmt();


	void execute(EvalState & state);
	StatementType get_type();
private:
	
	Expression *exp;


};



//-----------------------------------------------------------------------------------------


class Printstmt : public Statement{

public:
	Printstmt(TokenScanner& scanner);
	~Printstmt();


	void execute(EvalState & state);
	StatementType get_type();
private:

	Expression *exp;


};

//------------------------------------------------------------------------------------------

class Inputstmt : public Statement{

public:
	Inputstmt(string st);
	~Inputstmt();


	void execute(EvalState & state);
	StatementType get_type();
private:
	
	string var;

};

//--------------------------------------------------------------------------------------------

class Gotostmt : public Statement{

public:
	Gotostmt(int num);
	~Gotostmt();


	void execute(EvalState & state);
	void execute(EvalState & state,int&current_line);
	StatementType get_type();
private:
	
	int line_num;


};

//---------------------------------------------------------------------------------------------

class Ifstmt : public Statement{

public:
	Ifstmt(Expression *lexp, Expression *rexp, string cmp, int num);
	~Ifstmt();


	void execute(EvalState & state);
	void execute(EvalState & state,int&current_line);
	StatementType get_type();
private:

	Expression *lexp;
	Expression *rexp;
	string cmp;
	int line_num;


};

//------------------------------------------------------------------------------------------------

class Endstmt : public Statement{

public:
	Endstmt();
	~Endstmt();


	void execute(EvalState & state);
	StatementType get_type();
private:


};

#endif
