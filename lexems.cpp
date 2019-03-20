#include "lexems.h"

void Plusplus::print() const {
	if (opertype == PLUSPLUS)
		cout << "++";
	else
		cout << "--";
	if (pos == POST)
		cout << "[POST] ";
	else
		cout << "[PRE] ";
}

Function::Function(string name, set<string> args, int line) {
	Function::name = name;
	Function::args = args;
	Function::line = line;
}

void Function::print() const {
	string stype;
	if (type == INT)
		stype = "int";
	else
		stype = "void";
	cout << "[" << stype << " " << name << "(" << args.size() << ")" << "] ";
}

Number::Number(int val = 0) {
	value = val;
}

Operator::Operator(OPERATOR type) {
	opertype = type;
}

bool Operator::isBinary() const {
	const string &name = OPERTEXT[opertype];
	if (name == "print" || name == "println" || name == ",")
		return false;
	return true;
}

void Operator::print() const {
	cout << OPERTEXT[opertype] << " ";
}

int Operator::priority() const { 
	return PRIORITY[opertype];
}

bool Operator::goodForUnary() const {
	string name = OPERTEXT[opertype];
	for (string oper: PRE_UNARY) {
		if (oper == name)
			return true;
	}
	return false;
}

int Operator::getValue(const Lexem * l_left, const Lexem * l_right) const {
	int result = 0;
	int left = l_left->getValue(), right = l_right->getValue();
	switch (opertype) {
		case PLUS:
			return left + right;
		case MINUS:
			return left - right;
		case MULT:
			return left * right;
		case BITOR:
			return left | right;
		case XOR:
			return left ^ right;
		case BITAND:
			return left & right;
		case AND:
			return left && right;
		case OR:
			return left || right;
		case DIV:
			return left / right;
		case MOD:
			return left % right;
		case SHL:
			return left << right;
		case SHR:
			return left >> right;
		case EQ:
			return left == right;
		case NEQ:
			return left != right;
		case LEQ:
			return left <= right;
		case LT:
			return left < right;
		case GEQ:
			return left >= right;
		case GT:
			return left > right;
		case PRINTLN:
			cout << left << endl;
			return 0;
		case PRINT:
			cout << left;
			return 0;
		case NEWLINE:
			cout << endl;
			return 0;
		default:
			return 0;
	}
	return result;
}

