/*
 * File: Basic.cpp
 * ---------------
 * Name: ¿ÂÑ§º²
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
//#include <error.h>
//#include <tokenscanner.h>

#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

//#include <simpio.h>
//#include <strlib.h>
#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
   EvalState state;
   Program program;
   //cout << "Stub implementation of BASIC" << endl;
   while (true) {
      try {
         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
         cout<< ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
  /* TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);
   Expression *exp = parseExp(scanner);
   int value = exp->eval(state);
   cout << value << endl;
   delete exp;*/

   TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);
   string token = scanner.nextToken();
   TokenType type = scanner.getTokenType(token);
   if (type == WORD){
	   if (token == "RUN")
	   {
		   int current_line=program.getFirstLineNumber();
		   while (current_line>=0)
		   {
			   int next_line = program.getNextLineNumber(current_line);
			   Statement* stat=program.getParsedStatement(current_line);
			   StatementType type = stat->get_type();
			   if (type == END)
				   break;
			   if (type == GOTO)
			   {
				   ((Gotostmt*)stat)->execute(state,current_line);
				   
				   continue;
			   }
			   if (type == IF)
			   {
				   ((Ifstmt*)stat)->execute(state, current_line);
				   if (current_line != -2)
			  	       continue;
			   }
			  
			   stat->execute(state);
			   current_line = next_line;
		   }
		  
	   }
	   if (token == "QUIT")
	   {
		   exit(0);
	   }
	   if (token == "CLEAR")
	   {
		   program.clear();
		   EvalState state1;
		   state = state1;
		   
	   }
	   if (token == "HELP")
	   {
	   }
	   if (token == "LIST")
	   {
		   int current_line = program.getFirstLineNumber();
		   while (current_line != -1)
		   {
			   cout << program.getSourceLine(current_line) << endl;
			   current_line = program.getNextLineNumber(current_line);
		   }
	   }
	   if (token == "INPUT"){
		   string st = scanner.nextToken();
		   if (scanner.hasMoreTokens())
			   error("SYNTAX ERROR");
		   Inputstmt inputst(st);
		   inputst.execute(state);
		  
	   }
	   if (token == "PRINT"){
		   Printstmt printst(scanner);
		   printst.execute(state);
	   }

	   if (token == "LET")
	   {
		   Letstmt lets(scanner);
		   lets.execute(state);
		 
	   }
	   if (token != "RUN"&&token != "CLEAR"&&token != "HELP"&&token != "LIST"&&token != "QUIT"&&token != "PRINT"&&token != "LET"&&token != "INPUT")
		   error("SYNTAX ERROR");
   }


   else if (type == NUMBER){
	   int line_num = atoi(token.c_str());
	   
	   string token2 = scanner.nextToken();
	   if (token2 == "")
	   {
		   program.removeSourceLine(line_num);
		   return;
	   }
	   if (token2 == "REM")
	   {
		   string str, temp;
		   while (scanner.hasMoreTokens())
		   {
			   temp = scanner.nextToken();
			   str += temp;
		   }
		   Statement* remst = new Remstmt(str);
		   program.setParsedStatement(line_num, remst);
		   program.addSourceLine(line_num, line);
		   return;
	   }

	   if (token2 == "LET")
	   {
		   Statement *letst = new Letstmt(scanner);
		   program.setParsedStatement(line_num, letst);
		   program.addSourceLine(line_num, line);
		   return;
	   }

	   if (token2 == "PRINT")
	   {
		   Statement *printst = new Printstmt(scanner);
		   program.setParsedStatement(line_num, printst);
		   program.addSourceLine(line_num, line);
		   return;
	   }

	   if (token2 == "INPUT")
	   {
		   string st = scanner.nextToken();
		   if (scanner.hasMoreTokens())
			   error("SYNTAX ERROR");
		   Statement* inputstat = new Inputstmt(st);
		   program.setParsedStatement(line_num, inputstat);
		   program.addSourceLine(line_num, line);
		   return;
	   }
	   if (token2 == "GOTO")
	   {
		   string token_tmp = scanner.nextToken();
		   TokenType type = scanner.getTokenType(token_tmp);
		   if (type != NUMBER)
			   error("SYNTAX ERROR");
		   if (scanner.hasMoreTokens())
			   error("SYNTAX ERROR");
		   int linenum = atoi(token_tmp.c_str());
		   Statement* gotostat = new Gotostmt(linenum);
		   program.setParsedStatement(line_num, gotostat);
		   program.addSourceLine(line_num, line);
		   return;
	   }


	   if (token2 == "IF")
	   {
		   TokenScanner Lscanner;
		   TokenScanner Rscanner;
		   string left, right, tmp, cmp;
		   while (scanner.hasMoreTokens())
		   {
			   tmp = scanner.nextToken();
			   if (tmp == "=" || tmp == ">" || tmp == "<")
			   {
				   cmp = tmp;
				   break;
			   }

			   else left = left + tmp;
		   }
		   if (cmp != "="&&cmp != "<"&&cmp != ">")
			   error("SYNTAX ERROR");

		   while (scanner.hasMoreTokens())
		   {
			   tmp = scanner.nextToken();
			   if (tmp == "THEN")
				   break;
			   else right = right + tmp;
		   }
		   if (tmp != "THEN")
			   error("SYNTAX ERROR");

		   string token_tmp = scanner.nextToken();
		   TokenType type = scanner.getTokenType(token_tmp);
		   if (type != NUMBER)
			   error("SYNTAX ERROR");
		   if (scanner.hasMoreTokens())
			   error("SYNTAX ERROR");
		   int linenum = atoi(token_tmp.c_str());

		   Lscanner.setInput(left);
		   Rscanner.setInput(right);
		   Expression *lexp = parseExp(Lscanner);
		   Expression *rexp = parseExp(Rscanner);

		   Statement* ifstat = new Ifstmt(lexp, rexp, cmp, linenum);
		   program.setParsedStatement(line_num, ifstat);
		   program.addSourceLine(line_num, line);
		   return;
	   }
	   if (token2 == "END")
	   {
		   Statement* endstat=new Endstmt();
		   program.setParsedStatement(line_num, endstat);
		   program.addSourceLine(line_num, line);
		   return;
	   }

	   
	    error("SYNTAX ERROR");
	  

   }
    else   error("SYNTAX ERROR");
}
