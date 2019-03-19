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

enum LEXEM_TYPE {
	NUM, OP 
};

class Lexem {
protected:
	LEXEM_TYPE lextype;
public:
	Lexem(LEXEM_TYPE type) { lextype = type; };
	LEXEM_TYPE getLexType() const { return lextype; };
	virtual void print() const;
};

void Lexem::print() const {
	cout << '&';
}

class Number: public Lexem {
	int value;
public:
	Number(int val);
	int getValue() { return value; };
	void print() const { cout << value; };
};

Number::Number(int val = 0) : Lexem(NUM) {
	value = val;
}

enum OPERATOR {
	PLUS, MINUS, MULTIPLY, LBRACKET, RBRACKET
};

int PRIORITY[] = {
	0, 0, 1, 2, 2
};

class Oper: public Lexem {
	OPERATOR opertype;
public:
	Oper(char op);
	OPERATOR getType() { return opertype; };
	int getValue(const Number & left, const Number & right);
	void print() const;
};

Oper::Oper(char op) : Lexem(OP) {
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
}

vector<Lexem *> parseLexem(const string & input) {
	vector<Lexem *> v_lexem;
	for (int i = 0; i < input.size(); i++) {
		Lexem *cur_lex;
		if (input[i] >= '0' && input[i] <= '9') {
			cur_lex = new Number(input[i] - '0');
		} else {
			cur_lex = new Oper(input[i]);
		}
		v_lexem.push_back(cur_lex);
	}
	return v_lexem;
}

Lexem *copyFromTop(stack<Oper> & op_stack) {
	OPERATOR type = op_stack.top().getType();
	char type_char;
	switch (type) {
		case PLUS:
			type_char = '+';
			break;
		case MINUS:
			type_char = '-';
			break;
		case MULTIPLY:
			type_char = '*';
			break;
		default:
			type_char = '(';
			break;
	}
	return new Oper(type_char);
}

vector<Lexem *> buildPoliz(vector<Lexem *> infix) {
	vector<Lexem *> poliz;
	stack<Oper> op_stack;
	for (Lexem *cur : infix) {
		//cur->print();
		//cout << endl;
		if (cur->getLexType() == NUM) {
			poliz.push_back(cur);
		} else {
			Oper *op = dynamic_cast<Oper *>(cur);
			OPERATOR optype = op->getType();
			if (optype == LBRACKET) {
				op_stack.push(*op);
			} else if (optype == RBRACKET) {
				if (!op_stack.empty()) {
					while (!op_stack.empty() && op_stack.top().getType() != LBRACKET) {
						//poliz.push_back(dynamic_cast<Lexem *>(&(op_stack.top())));
						poliz.push_back(copyFromTop(op_stack));
						op_stack.pop();
					}
					op_stack.pop();
				}
			} else {
				if (!op_stack.empty()) {
					OPERATOR toptype = op_stack.top().getType();
					//cout << "{" <<  toptype << "}" << endl;
					while(!op_stack.empty() && PRIORITY[optype] <= PRIORITY[toptype] && toptype < LBRACKET) {
						//poliz.push_back(dynamic_cast<Lexem *>(&(op_stack.top())));
						poliz.push_back(copyFromTop(op_stack));
						op_stack.pop();
						if (!op_stack.empty())
							toptype = op_stack.top().getType();
					}
				}
				op_stack.push(*op);
			}
		}
		//cout << "Stack size: " << op_stack.size() << endl;
		//cout << "Poliz size: " << poliz.size() << endl;
	}
	while (!op_stack.empty()) {
		//poliz.push_back(dynamic_cast<Lexem *>(&(op_stack.top())));
		poliz.push_back(copyFromTop(op_stack));
		op_stack.pop();
	}
	return poliz;
}

int evaluatePoliz(vector<Lexem *> poliz) {
	if (poliz.size() == 0)
		return 0;
	stack<int> values;
	for (int i = 0; i < poliz.size(); i++) {
		Lexem *cur = poliz[i];
		if (cur->getLexType() == NUM) {
			Number *num = dynamic_cast<Number *>(cur);
			values.push(num->getValue());
			//cout << num->getValue();
		} else {
			int val1 = values.top();
			values.pop();
			int val2 = values.top();
			values.pop();
			int result = 0;
			Oper *op = dynamic_cast<Oper *>(cur);
			OPERATOR type = op->getType();
			switch (type) {
				case PLUS:
					//cout << "+";
					result = val1 + val2;
					break;
				case MINUS:
					result = val2 - val1;
					//cout << "-";
					break;
				case MULTIPLY:
					result = val1 * val2;
					//cout << "*";
					break;
			}
			//cout << endl;
			values.push(result);
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
