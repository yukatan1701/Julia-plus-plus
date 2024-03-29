#ifndef __LEXEMS_H__
#define __LEXEMS_H__

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>

#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;36m"
#define RESET   "\033[0m"

using std::vector;
using std::map;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::set;

enum LEXEMTYPE { NUM, OPER, VAR, REF, FUNC, UNARY_MINUS };

enum OPERATOR {
	FUNCTION, RETURN, PRINTLN,
	PRINT, NEWLINE, SPACE,
	IF, THEN, ELSE, ENDIF,
	WHILE, ENDWHILE,
	LOCAL,
	COMMA,
	ASSIGN, OR, AND, BITOR, 
	XOR, BITAND, EQ, NEQ, LT,
	LEQ, GT, GEQ, SHL, SHR,
	PLUS, MINUS, MULT, DIV, MOD,
	LSQUARE, RSQUARE,
	LBRACKET, RBRACKET, LABEL, 
	PLUSPLUS, MINMIN,
	GOTO, GLOBAL
};

const vector<OPERATOR> PAIR_OPS {
	FUNCTION, PRINTLN, PRINT,
	IF, WHILE, LOCAL
};

const vector<string> PRE_UNARY {
	"return", "print", "if", "while",
	",", "=", "or", "and", "|", "^",
	"&", "==", "!=", "<", "<=", ">",
	">=", "<<", ">>", "+", "-", "*",
	"/", "%", "[", "("
};

const vector<string> OPERTEXT {
	"function", "return", "println",
	"print", "newline", "space",
	"if", "then", "else", "endif",
	"while", "endwhile",
	"local",
	",",
	"=", "or", "and", "|", 
    "^", "&", "==", "!=", "<", 
	"<=", ">", ">=", "<<", ">>", 
	"+", "-", "*", "/", "%",
	"[", "]",
	"(", ")", ":",
	"++", "--", 
	"goto", "global"
};
								
const int PRIORITY[] = {
	-1, -1, -1,
	-1, -1, -1,
	-1, -1, -1, -1,
	-1, -1,
	-1,
	0,
	1, 2, 3, 4, 
	5, 6, 7, 7, 8,
	8, 8, 8, 9, 9,
	10, 10, 11, 11, 11,
	13, 13,
	13, 13, 14, 
	15, 15,
	16, 16
};

enum FUNCTYPE { VOID, INT };

class Lexem {
	int row;
	int col;
public:
	Lexem() {}
	Lexem(int row, int col) { Lexem::row = row; Lexem::col = col; }
	virtual LEXEMTYPE getLexemType() const = 0;
	virtual int getValue() const = 0;
	virtual void print() const = 0;
	int getRow() { return row; }
	int getCol() { return col; }
	void setPos(int row, int col) { Lexem::row = row; Lexem::col = col; }
	void setPos(Lexem *lex) { row = lex->row; col = lex->col; }
	~Lexem() {}
};

class Function: public Lexem {
	string name;
	FUNCTYPE type;
	int line;
	set<string> args;
public:
	Function(string name, set<string> args, int line);
	FUNCTYPE getType() const { return type; }
	string getName() const { return name; }
	int getArgc() const { return args.size(); }
	set<string> getArgs() const { return args; }
	int getValue() const { return 0; }
	int getLine() const { return line; }
	void setType(FUNCTYPE ftype) { type = ftype; }
	void print() const;
	LEXEMTYPE getLexemType() const { return FUNC; }
};

class Number: public Lexem {
	int value;
public:
	Number(int val);
	int getValue() const { return value; }
	LEXEMTYPE getLexemType() const { return NUM; }
	void print() const { cout << value << " "; }
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
};

class Label: public Variable {
	Label(string l_name, int address): Variable(l_name, address) {}
};

class Reference: public Variable {
	int & arr_val;
public:
	Reference(string name, int & dest) : Variable(name, 0), arr_val(dest) {}
	int getValue() const { return arr_val; }
	int & getReference() const { return arr_val; }
	LEXEMTYPE getLexemType() const { return REF; }
};

class Operator: public Lexem {
protected:
	OPERATOR opertype;
public:
	Operator(OPERATOR type);
	OPERATOR getType() const { return opertype; }
	int getValue() const { return 0; }
	int getValue(const Lexem * l_left, const Lexem * l_right) const;
	LEXEMTYPE getLexemType() const { return OPER; }
	int priority() const;
	virtual bool isBinary() const;
	bool goodForUnary() const;
	void print() const;
};

enum POSITION {PRE, POST};

class Plusplus: public Operator {
	POSITION pos;
public:
	Plusplus(OPERATOR op, POSITION pos) : Operator(op), pos(pos) {}
	POSITION getPosition() const { return pos; }
	void print() const;
};

class UnaryMinus: public Operator {
public:
	UnaryMinus() : Operator(MINUS) {}
	void print() const { cout << "-[U] "; }
	LEXEMTYPE getLexemType() const { return UNARY_MINUS; }
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
	void print() const { cout << "if[" << else_addr << "] "; }
};

#endif
