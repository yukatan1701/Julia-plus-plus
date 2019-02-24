#include "lexems.h"

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
	if (name == "print" || name == ",")
		return false;
	return true;
}

void Operator::print() const {
	cout << OPERTEXT[opertype] << " ";
}

int Operator::priority() const { 
	return PRIORITY[opertype];
}

int Operator::getValue(const Lexem * l_left, const Lexem * l_right, 
                   map<string, Variable *> & var_table) const {
	int result = 0;
	int left = l_left->getValue(), right = l_right->getValue();
	switch (opertype) {
		case ASSIGN:
			//l_left->print();
			//l_right->print();
			if (l_left->getLexemType() == VAR) {
				const Variable *v_left = static_cast<const Variable *>(l_left);
				var_table[v_left->getName()]->setValue(right);
			} else if (l_left->getLexemType() == REF) {
				const Reference *r_left = static_cast<const Reference *>(l_left);
				int & dest = r_left->getReference();
				dest = right;
			}
			return right;
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
		case PRINT:
			cout << left;
			return 0;
		case NEWLINE:
			cout << endl;
			return 0;
	}
	return result;
}

