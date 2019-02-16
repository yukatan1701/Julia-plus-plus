#include "lexems.h"

Number::Number(int val = 0) {
	value = val;
}

Lexem * Number::copy() const {
	return new Number(value);
}

Lexem * Operator::copy() const {
	return new Operator(opertype);
}

Operator::Operator(OPERATOR type) {
	opertype = type;
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
			if (l_left->getLexemType() == VAR) {
				const Variable *v_left = static_cast<const Variable *>(l_left);
				var_table[v_left->getName()]->setValue(right);
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
	}
	return result;
}

