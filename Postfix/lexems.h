#ifndef __LEXEMS_H__
#define __LEXEMS_H__

#include <iostream>
#include <map>
#include <string>
#include <vector>

#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;36m"
#define RESET   "\033[0m"

using std::vector;
using std::map;
using std::cin;
using std::cout;
using std::endl;
using std::string;

enum LEXEMTYPE { NUM, OPER, VAR };

enum ERROR_CODES { WRONG_VAR, WRONG_LABEL, GOTO_DEF_ERROR };

const vector<string> OPERTEXT { "print", "newline", "space",
								"if", "then", "else", "endif",
								"while", "endwhile",
								"=", "or", "and", "|", 
                                "^", "&", "==", "!=", "<", 
								"<=", ">", ">=", "<<", ">>", 
								"+", "-", "*", "/", "%",
								"(", ")", ":", "goto"
								};

enum OPERATOR {
	PRINT, NEWLINE, SPACE,
	IF, THEN, ELSE, ENDIF,
	WHILE, ENDWHILE,
	ASSIGN, OR, AND, BITOR, 
	XOR, BITAND, EQ, NEQ, LT,
	LEQ, GT, GEQ, SHL, SHR,
	PLUS, MINUS, MULT, DIV, MOD,
	LBRACKET, RBRACKET, LABEL, GOTO
};

const int PRIORITY[] = {
	-1, -1, -1,
	-1, -1, -1, -1,
	-1, -1,
	1, 2, 3, 4, 
	5, 6, 7, 7, 8,
	8, 8, 8, 9, 9,
	10, 10, 11, 11, 11,
	12, 12, 13, 13, 13
};

class Lexem {
public:
	Lexem() {}
	virtual LEXEMTYPE getLexemType() const = 0;
	virtual int getValue() const = 0;
	virtual void print() const = 0;
	virtual Lexem *copy() const = 0;
};

class Number: public Lexem {
	int value;
public:
	Number(int val);
	int getValue() const { return value; }
	LEXEMTYPE getLexemType() const { return NUM; }
	void print() const { cout << value << " "; }
	Lexem *copy() const;
};

class Variable: public Lexem {
protected:
	int value;
	string name;
public:
	Variable(string var_name, int var_val) { name = var_name; value = var_val; }
	string getName() const { return name; }
	int getValue() const { return value; }
	void setValue(int val) { value = val; }
	LEXEMTYPE getLexemType() const { return VAR; }
	void print() const { cout << "{" << name << "}" << " "; }
	Lexem *copy() const { return new Variable(name, value); }
};

class Label: public Variable {
	Label(string l_name, int address): Variable(l_name, address) {};
};

class Operator: public Lexem {
protected:
	OPERATOR opertype;
public:
	Operator(OPERATOR type);
	OPERATOR getType() const { return opertype; }
	int getValue() const { return 0; }
	int getValue(const Lexem * l_left, const Lexem * l_right, 
	             map<string, Variable *> & var_table) const;
	LEXEMTYPE getLexemType() const { return OPER; }
	int priority() const;
	virtual bool isBinary() const;
	void print() const;
	Lexem *copy() const;
};

class Goto: public Operator {
	int address;
public:
	Goto(int addr) : Operator(GOTO) { address = addr; }
	void print() const { cout << "goto[" << address << "] "; }
	int getAddress() const { return address; }
	bool isBinary() const { return false; }
};

class If: public Operator {
	int else_addr;
public:
	If() : Operator(IF) { else_addr = -1; }
	int getElse() const { return else_addr; }
	void setElse(int addr) { else_addr = addr; }
	void print() const { cout << "if[$] "; }
};

#endif
