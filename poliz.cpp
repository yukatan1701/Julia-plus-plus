#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <cctype>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::stack;
using std::map;
using std::set;
using std::pair;

enum LEXEMTYPE { NUM, OPER, VAR };

enum ERROR_CODES { LOST_SEMICOLON, WRONG_VAR };

class Lexem {
public:
	Lexem() {}
	virtual LEXEMTYPE getLexemType() const = 0;
	virtual int getValue() const = 0;
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

const vector<string> OPERTEXT { ";", "=", "or", "and", "|", 
                                "^", "&", "==", "!=", "<", 
								"<=", ">", ">=", "<<", ">>", 
								"+", "-", "*", "/", "%",
								"(", ")" };

enum OPERATOR {
	EOL, ASSIGN, OR, AND, BITOR, 
	XOR, BITAND, EQ, NEQ, LT,
	LEQ, GT, GEQ, SHL, SHR,
	PLUS, MINUS, MULT, DIV, MOD,
	LBRACKET, RBRACKET
};

int PRIORITY[] = {
	0, 1, 2, 3, 4, 
	5, 6, 7, 7, 8,
	8, 8, 8, 9, 9,
	10, 10, 11, 11, 11,
	12, 12
};

const char DELIMITER = ' ';
const char NEWLINE = '\n';
const char SEMICOLON = ';';

class Variable: public Lexem {
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

class Oper: public Lexem {
	OPERATOR opertype;
public:
	Oper(OPERATOR type);
	OPERATOR getType() const { return opertype; };
	int getValue() const { return 0; }
	int getValue(const Lexem * l_left, const Lexem * l_right, 
	             map<string, Variable *> & var_table) const;
	LEXEMTYPE getLexemType() const { return OPER; };
	int priority() const { return PRIORITY[opertype]; }
	void print() const;
};

OPERATOR getOperatorByName(string op) {
	int oper_pos;
	for (oper_pos = 0; oper_pos < OPERTEXT.size(); oper_pos++) {
		if (OPERTEXT[oper_pos] == op)
			break;
	}
	return (OPERATOR) oper_pos;
}

Oper::Oper(OPERATOR type) {
	opertype = type;
}

void Oper::print() const{
	cout << OPERTEXT[opertype] << " ";
}

int Oper::getValue(const Lexem * l_left, const Lexem * l_right, 
                   map<string, Variable *> & var_table) const {
	int result = 0;

	int left = l_left->getValue(), right = l_right->getValue();
	switch (opertype) {
		case ASSIGN:
			result = right;
			if (l_left->getLexemType() == VAR) {
				const Variable *v_left = static_cast<const Variable *>(l_left);
				var_table[v_left->getName()]->setValue(right);
			}
			break;
		case PLUS:
			result = left + right;
			break;
		case MINUS:
			result = left - right;
			break;
		case MULT:
			result = left * right;
			break;
		case BITOR:
			result = left | right;
			break;
		case XOR:
			result = left ^ right;
			break;
		case BITAND:
			result = left & right;
			break;
	}
	return result;
}

Lexem *popFromTop(stack<Oper> & op_stack) {
	OPERATOR type = op_stack.top().getType();
	op_stack.pop();
	return new Oper(type);
}

enum ACCOCIATE { LEFT, RIGHT };

bool checkPriority(const stack<Oper> & op_stack, const Oper *op) {
	ACCOCIATE pos = LEFT;
	if (op->getType() == ASSIGN)
		pos = RIGHT;
	if (pos == LEFT)
		return op->priority() <= op_stack.top().priority();
	return op->priority() < op_stack.top().priority();
}

vector<Lexem *> buildPoliz(vector<Lexem *> infix) {
	vector<Lexem *> poliz;
	stack<Oper> op_stack;
	for (Lexem *cur : infix) {
		LEXEMTYPE lextype = cur->getLexemType();
		if (lextype == NUM || lextype == VAR) {
			poliz.push_back(cur);
		} else {
			Oper *op = static_cast<Oper *>(cur);
			OPERATOR optype = op->getType();
			if (optype == EOL) {
				while (!op_stack.empty())
					poliz.push_back(popFromTop(op_stack));
				poliz.push_back(op);
			} else if (optype == LBRACKET) {
				op_stack.push(*op);
			} else if (optype == RBRACKET) {
				if (!op_stack.empty()) {
					while (!op_stack.empty() && 
						   op_stack.top().getType() != LBRACKET) {
						poliz.push_back(popFromTop(op_stack));
					}
					op_stack.pop();
				}
			} else {
				if (!op_stack.empty()) {
					while(!op_stack.empty() 
						  && checkPriority(op_stack, op)
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
	return poliz;
}

void printTable(const map<string, Variable *> & var_table) {
	cout << endl << "Var\t" << "| Value:" << endl;
	cout << "----------------" << endl;
	for (auto it = var_table.cbegin(); it != var_table.cend(); it++) {
		cout << it->first << "\t| " << it->second->getValue() << endl;
	}
}

map<string, Variable *> evaluatePoliz(vector<Lexem *> poliz) {
	map<string, Variable *> var_table;
	if (poliz.size() == 0)
		return var_table;
	stack<Lexem *> values;
	for (Lexem *cur : poliz) {
		LEXEMTYPE lextype = cur->getLexemType();
		if (lextype == NUM) {
			values.push(cur);
		} else if (lextype == VAR) {
			Variable *var = static_cast<Variable *>(cur);
			var_table.insert(pair<string, Variable *>(var->getName(), var));
			values.push(var_table.find(var->getName())->second);
		} else {
			Oper *op = static_cast<Oper *>(cur);
			if (op->getType() == EOL) {
				// TODO: free stack
				continue;
			}
			const Lexem *val2 = values.top();
			values.pop();
			const Lexem *val1 = values.top();
			values.pop();
			int result = op->getValue(val1, val2, var_table);
			values.push(new Number(result));
		}
	}
	return var_table;
}

bool isVariableChar(char ch) {
	return ch == '_' || isalnum(ch);
}

bool isOperatorChar(char ch) {
	for (auto oper : OPERTEXT) {
		if (oper.find(ch) != std::string::npos && ch != SEMICOLON)
			return true;
	}
	return isalpha(ch);
}

bool skipSpace(const string & input, int & i, bool & has_eol) {
	if (input[i] == NEWLINE) {
		if (!has_eol) {
			throw LOST_SEMICOLON;
		} else
			has_eol = false;
		i++;
		return true;
	}
	if (input[i] == DELIMITER) {
		i++;
		return true;
	}
	return false;
}

Lexem *readNumber(const string & input, int & i) {
	int value = 0;
	while (isdigit(input[i]))
		value = value * 10 + (input[i++] - '0');
	return new Number(value);
}

bool isOperator(string word) {
	for (auto elem : OPERTEXT) {
		if (elem == word)
			return true;
	}
	return false;
}

bool isVariable(string word) {
	for (int i = 0; i < word.size(); i++) {
		if (!isVariableChar(word[i]))
			return false;
	}
	return true;
}

Lexem *wordToLexem(const string & input, int & i) {
	string word;	
	if (input[i] == SEMICOLON) {
		i++;
		return new Oper(getOperatorByName(";"));
	}
	if (isOperatorChar(input[i])) {
		while (isOperatorChar(input[i]))
			word.push_back(input[i++]);
		if (isOperator(word)) 
			return new Oper(getOperatorByName(word));
		if (isVariable(word))
			return new Variable(word, 0);
		cout << word;
		throw WRONG_VAR;
	}
	while (isVariableChar(input[i]))
		word.push_back(input[i++]);
	if (isVariable(word))
		return new Variable(word, 0);
	cout << word;
	throw WRONG_VAR;
}

vector<Lexem *> parseLexem(const string & input) {
	vector<Lexem *> v_lexem;
	bool has_eol = false;
	for (int i = 0; i < input.size(); ) {
		char ch = input[i];
		if (skipSpace(input, i, has_eol))
			continue;
		Lexem *cur_lex;
		if (isdigit(ch)) {
			cur_lex = readNumber(input, i);
		} else {
			cur_lex = wordToLexem(input, i);
			if (ch == SEMICOLON)
				has_eol = true;
		}
		v_lexem.push_back(cur_lex);
	}
	return v_lexem;
}

string getCode() {
	string text; 
	getline(cin, text, (char) EOF);
	return text;
}

void writeMessage(ERROR_CODES er_code) {
	cout << "Error "<< er_code << ": ";
	switch (er_code) {
		case LOST_SEMICOLON:
			cout << "lost semicolon.";
			break;
		case WRONG_VAR:
			cout << "wrong variable name.";
			break;
	}
	cout << endl;
}

int main() {
	string input = getCode();
	//cout << input;
	vector<Lexem *> parsed;
	try {
		parsed = parseLexem(input);
		for (auto elem : parsed)
			elem->print();
		cout << endl;
	} catch (ERROR_CODES er_code) {
		writeMessage(er_code);
	}
	vector<Lexem *> poliz = buildPoliz(parsed);
	for (auto cur : poliz) {
		cur->print();
	}
	cout << endl;
	map<string, Variable *> var_table = evaluatePoliz(poliz);
	printTable(var_table);
	/*for (Lexem * elem : parsed) {
		if (elem)
			delete elem;
	}
	for (Lexem *elem : poliz) {
		if (elem)
			delete elem;
	}*/
	return 0;
}
