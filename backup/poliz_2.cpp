#include <iostream>
#include <string>
#include <vector>
#include <stack>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::stack;

enum LEXEMTYPE { NUM, OPER };

class Lexem {
public:
	Lexem() {}
	virtual LEXEMTYPE getLexemType() const = 0;
	virtual void print() const = 0;
};

class Number: public Lexem {
	int value;
public:
	Number(int val);
	int getValue() const { return value; };
	LEXEMTYPE getLexemType() const { return NUM; };
	void print() const { cout << value << " "; };
};

Number::Number(int val = 0) {
	value = val;
}

enum OPERATOR {
	PLUS, MINUS, MULTIPLY, LBRACKET, RBRACKET
};

int PRIORITY[] = {
	1, 1, 2, 3, 3
};

class Oper: public Lexem {
	OPERATOR opertype;
public:
	Oper(OPERATOR type);
	OPERATOR getType() { return opertype; };
	int getValue(const Number & n_left, const Number & n_right) const;
	LEXEMTYPE getLexemType() const { return OPER; };
	int priority() const { return PRIORITY[opertype]; }
	void print() const;
};

OPERATOR getOperatorByChar(char op) {
	OPERATOR opertype;
	switch(op) {
		case '+':
			opertype = PLUS;
			break;
		case '-':
			opertype = MINUS;
			break;
		case '*':
			opertype = MULTIPLY;
			break;
		case '(':
			opertype = LBRACKET;
			break;
		case ')':
			opertype = RBRACKET;
			break;
		default:
			opertype = PLUS;
	}
	return opertype;
}

Oper::Oper(OPERATOR type) {
	opertype = type;
}

void Oper::print() const{
	switch (opertype) {
		case PLUS:
			cout << '+';
			break;
		case MINUS:
			cout << '-';
			break;
		case MULTIPLY:
			cout << '*';
			break;
		case LBRACKET:
			cout << '(';
			break;
		case RBRACKET:
			cout << ')';
			break;
	}
	cout << " ";
}

int Oper::getValue(const Number & n_left, const Number & n_right) const {
	int result = 0;
	int left = n_left.getValue(), right = n_right.getValue();
	switch (opertype) {
		case PLUS:
			result = right + left;
			break;
		case MINUS:
			result = right - left;
			break;
		case MULTIPLY:
			result = right * left;
			break;
	}
	return result;
}


vector<Lexem *> parseLexem(const string & input) {
	vector<Lexem *> v_lexem;
	for (int i = 0; i < input.size(); ) {
		Lexem *cur_lex;
		if (input[i] >= '0' && input[i] <= '9') {
			int value = 0;
			while (input[i] >= '0' && input[i] <= '9') {
				value = value * 10 + (input[i] - '0');
				i++;
			}
			cur_lex = new Number(value);
		} else {
			cur_lex = new Oper(getOperatorByChar(input[i]));
			i++;
		}
		v_lexem.push_back(cur_lex);
	}
	return v_lexem;
}

Lexem *popFromTop(stack<Oper> & op_stack) {
	OPERATOR type = op_stack.top().getType();
	op_stack.pop();
	return new Oper(type);
}

vector<Lexem *> buildPoliz(vector<Lexem *> infix) {
	vector<Lexem *> poliz;
	stack<Oper> op_stack;
	for (Lexem *cur : infix) {
		if (cur->getLexemType() == NUM) {
			poliz.push_back(cur);
		} else {
			Oper *op = static_cast<Oper *>(cur);
			OPERATOR optype = op->getType();
			if (optype == LBRACKET) {
				op_stack.push(*op);
			} else if (optype == RBRACKET) {
				if (!op_stack.empty()) {
					while (!op_stack.empty() && op_stack.top().getType() != LBRACKET) {
						poliz.push_back(popFromTop(op_stack));
					}
					op_stack.pop();
				}
			} else {
				if (!op_stack.empty()) {
					while(!op_stack.empty() 
						  && op->priority() <= op_stack.top().priority()
						  && op_stack.top().getType() != LBRACKET) {
						poliz.push_back(popFromTop(op_stack));
					}
				}
				op_stack.push(*op);
			}
		}
		//cout << "Stack size: " << op_stack.size() << endl;
		//cout << "Poliz size: " << poliz.size() << endl;
	}
	while (!op_stack.empty()) {
		poliz.push_back(popFromTop(op_stack));
	}
	return poliz;
}

int evaluatePoliz(vector<Lexem *> poliz) {
	if (poliz.size() == 0)
		return 0;
	stack<int> values;
	for (Lexem *cur : poliz) {
		if (cur->getLexemType() == NUM) {
			Number *num = static_cast<Number *>(cur);
			values.push(num->getValue());
		} else {
			int val1 = values.top();
			values.pop();
			int val2 = values.top();
			values.pop();
			Oper *op = static_cast<Oper *>(cur);
			values.push(op->getValue(Number(val1), Number(val2)));
		}
	}
	return values.top();
}

int main() {
	string input;
	cin >> input;
	vector<Lexem *> parsed = parseLexem(input);
	vector<Lexem *> poliz = buildPoliz(parsed);
	for (auto cur : poliz) {
		cur->print();
	}
	cout << endl << evaluatePoliz(poliz) << endl;
	for (Lexem * elem : parsed) {
		delete elem;
	}
	for (Lexem *elem : poliz) {
		delete elem;
	}
	return 0;
}
