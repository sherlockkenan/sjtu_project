/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
//#include <error.h>
#include "../StanfordCPPLib/error.h"
using namespace std;

Program::Program() {
	
}

Program::~Program() {
   // Replace this stub with your own code
}

void Program::clear() {
	// Replace this stub with your own code
	sourse_line.clear();
	parsed_line.clear();
}

void Program::addSourceLine(int lineNumber, string line) {
	// Replace this stub with your own code
	map<int, string >::iterator l_it;
	l_it = sourse_line.find(lineNumber);
	if (l_it != sourse_line.end())
		sourse_line[lineNumber] = line;                                              //更新
	else
	    sourse_line.insert(pair<int, string>(lineNumber, line));
}

void Program::removeSourceLine(int lineNumber) {
	// Replace this stub with your own code
	map<int, string >::iterator l_it1;
	map<int, Statement*>::iterator l_it2;
	l_it1 = sourse_line.find(lineNumber);
	l_it2 = parsed_line.find(lineNumber);
	if (l_it1 != sourse_line.end())
	{
		sourse_line.erase(l_it1);
		parsed_line.erase(l_it2);
	}
}

string Program::getSourceLine(int lineNumber) {
	// Replace this stub with your own code
	map<int, string >::iterator l_it;
	l_it = sourse_line.find(lineNumber);
	if (l_it == sourse_line.end())
		error("LINE NUMBER ERROR");
	else
		return sourse_line[lineNumber];
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
	// Replace this stub with your own code
	map<int, Statement* >::iterator l_it;
	l_it = parsed_line.find(lineNumber);
	if (l_it != parsed_line.end())
		parsed_line[lineNumber] = stmt;                                     //更新
	else
		parsed_line.insert(pair<int, Statement*>(lineNumber, stmt));
}
Statement *Program::getParsedStatement(int lineNumber) {
	// Replace this stub with your own code
	map<int, Statement* >::iterator l_it;
	l_it = parsed_line.find(lineNumber);
	if (l_it == parsed_line.end())
		error("LINE NUMBER ERROR");
	else
		return parsed_line[lineNumber];
}

int Program::getFirstLineNumber() {
	map<int, Statement* >::iterator l_it;
	l_it = parsed_line.begin();
	if (l_it == parsed_line.end())return -1;
	return l_it->first;
}

int Program::getNextLineNumber(int lineNumber) {
	map<int, Statement* >::iterator l_it;
	l_it = parsed_line.find(lineNumber);
	if (l_it == parsed_line.end()||++l_it == parsed_line.end() )
		return -1;
	else 
		return l_it->first;
}
