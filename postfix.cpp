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
using std::ostream;
using std::istream;

enum LEXEMTYPE { NUM, OPER, VAR };

enum ERROR_CODES { LOST_SEMICOLON, WRONG_VAR };

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
	int getValue() const { return value; };
	LEXEMTYPE getLexemType() const { return NUM; };
	void print() const { cout << value << " "; };
	Lexem *copy() const;
};

Number::Number(int val = 0) {
	value = val;
}

Lexem * Number::copy() const {
	return new Number(value);
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

enum OPERSTYLE { CHARS, WORD };

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
	Lexem *copy() const;
};

Lexem * Variable::copy() const {
	return new Variable(name, value);
}

class Operator: public Lexem {
	OPERATOR opertype;
public:
	Operator(OPERATOR type);
	OPERATOR getType() const { return opertype; };
	int getValue() const { return 0; }
	int getValue(const Lexem * l_left, const Lexem * l_right, 
	             map<string, Variable *> & var_table) const;
	LEXEMTYPE getLexemType() const { return OPER; };
	int priority() const { return PRIORITY[opertype]; }
	void print() const;
	Lexem *copy() const;
};

Lexem * Operator::copy() const {
	return new Operator(opertype);
}

OPERATOR getOperatorByName(string op) {
	int oper_pos;
	for (oper_pos = 0; oper_pos < OPERTEXT.size(); oper_pos++) {
		if (OPERTEXT[oper_pos] == op)
			break;
	}
	return (OPERATOR) oper_pos;
}

Operator::Operator(OPERATOR type) {
	opertype = type;
}

void Operator::print() const{
	cout << OPERTEXT[opertype] << " ";
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

class LexemVector {
	vector<Lexem *> line;
	bool isVariableChar(char) const;
	bool isOperatorChar(char) const;
	bool isVariable(const string &) const;
	bool isOperator(const string &) const;
	bool skipSpace(const string &, int &, bool &) const;
	Lexem *readNumber(const string &, int &) const;
	Lexem *wordToLexem(const string &, int &) const;
	Lexem *popFromTop(stack<Operator> &) const;
	bool checkPriority(const stack<Operator> &, const Operator *) const;
	OPERSTYLE style(const string &) const;
public:
	LexemVector() {};
	LexemVector(string & input) { parseLexem(input); }
	void parseLexem(const string & input);
	LexemVector buildPostfix() const;
	map<string, Variable *> evaluatePostfix() const;
	void push_back(Lexem *lex);
	void free();
	~LexemVector();
	friend ostream& operator<<(ostream &os, const LexemVector& lv);
};

ostream& operator<<(ostream &os, const LexemVector& lv) {
	for (auto elem : lv.line)
		elem->print();
	cout << endl;
}

void LexemVector::push_back(Lexem *lex) {
	line.push_back(lex);
}

void LexemVector::free() {
	for (auto elem : line)
		delete elem;
	line.clear();
}

LexemVector::~LexemVector() {
	cout << "Destructor." << endl;
	free();
}

Lexem * LexemVector::popFromTop(stack<Operator> & op_stack) const {
	OPERATOR type = op_stack.top().getType();
	op_stack.pop();
	return new Operator(type);
}

enum ACCOCIATE { LEFT, RIGHT };

bool LexemVector::checkPriority(const stack<Operator> & op_stack, const Operator *op) const {
	ACCOCIATE pos = LEFT;
	if (op->getType() == ASSIGN)
		pos = RIGHT;
	if (pos == LEFT)
		return op->priority() <= op_stack.top().priority();
	return op->priority() < op_stack.top().priority();
}

LexemVector LexemVector::buildPostfix() const {
	LexemVector postfix;
	stack<Operator> op_stack;
	for (Lexem *cur : line) {
		LEXEMTYPE lextype = cur->getLexemType();
		if (lextype == NUM || lextype == VAR) {
			postfix.push_back(cur->copy());
		} else {
			Operator *op = static_cast<Operator *>(cur);
			OPERATOR optype = op->getType();
			if (optype == EOL) {
				while (!op_stack.empty())
					postfix.push_back(popFromTop(op_stack));
				postfix.push_back(op->copy());
			} else if (optype == LBRACKET) {
				op_stack.push(*op);
			} else if (optype == RBRACKET) {
				if (!op_stack.empty()) {
					while (!op_stack.empty() && 
						   op_stack.top().getType() != LBRACKET) {
						postfix.push_back(popFromTop(op_stack));
					}
					op_stack.pop();
				}
			} else {
				if (!op_stack.empty()) {
					while(!op_stack.empty() 
						  && checkPriority(op_stack, op)
						  && op_stack.top().getType() != LBRACKET) {
						postfix.push_back(popFromTop(op_stack));
					}
				}
				op_stack.push(*op);
			}
		}
		//cout << "Stack size: " << op_stack.size() << endl;
		//cout << "Postfix size: " << postfix.size() << endl;
	}
	return postfix;
}

void printTable(const map<string, Variable *> & var_table) {
	cout << endl << "Var\t" << "| Value:" << endl;
	cout << "----------------" << endl;
	for (auto it = var_table.cbegin(); it != var_table.cend(); it++) {
		cout << it->first << "\t| " << it->second->getValue() << endl;
	}
}

map<string, Variable *> LexemVector::evaluatePostfix() const {
	map<string, Variable *> var_table;
	if (line.size() == 0)
		return var_table;
	stack<Lexem *> values;
	vector<Lexem *> temporary;
	for (Lexem *cur : line) {
		LEXEMTYPE lextype = cur->getLexemType();
		if (lextype == NUM) {
			values.push(cur);
		} else if (lextype == VAR) {
			Variable *var = static_cast<Variable *>(cur);
			var_table.insert(pair<string, Variable *>(var->getName(), var));
			values.push(var_table.find(var->getName())->second);
		} else {
			Operator *op = static_cast<Operator *>(cur);
			if (op->getType() == EOL) {
				// TODO: free stack
				continue;
			}
			const Lexem *val2 = values.top();
			values.pop();
			const Lexem *val1 = values.top();
			values.pop();
			int result = op->getValue(val1, val2, var_table);
			Lexem *res = new Number(result);
			values.push(res);
			temporary.push_back(res);
		}
	}
	for (auto elem : temporary)
		delete elem;
	return var_table;
}

bool LexemVector::isVariableChar(char ch) const {
	return ch == '_' || isalnum(ch);
}

bool LexemVector::isOperatorChar(char ch) const {
	for (auto oper : OPERTEXT) {
		if (oper.find(ch) != std::string::npos && ch != SEMICOLON)
			return true;
	}
	return isalpha(ch);
}

bool LexemVector::skipSpace(const string & input, int & i, bool & has_eol) const {
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

Lexem * LexemVector::readNumber(const string & input, int & i) const {
	int value = 0;
	while (isdigit(input[i]))
		value = value * 10 + (input[i++] - '0');
	return new Number(value);
}

bool LexemVector::isOperator(const string & word) const {
	for (auto elem : OPERTEXT) {
		if (elem == word)
			return true;
	}
	return false;
}

bool LexemVector::isVariable(const string & word) const {
	for (int i = 0; i < word.size(); i++) {
		if (!isVariableChar(word[i]))
			return false;
	}
	return true;
}

OPERSTYLE LexemVector::style(const string & word) const {
	if (isalpha(word[word.size() - 1]))
		return WORD;
	return CHARS;
}

Lexem * LexemVector::wordToLexem(const string & input, int & i) const {
	string word;	
	if (input[i] == SEMICOLON) {
		i++;
		return new Operator(getOperatorByName(";"));
	}
	if (isOperatorChar(input[i])) {
		bool is_word = isalpha(input[i]);
		bool is_prev_op = false;
		OPERSTYLE last_style = is_word ? WORD : CHARS;
		while (isOperatorChar(input[i])) {
			word.push_back(input[i++]);
			//cout << "[" << input[i] << "]" << word << style(word) << endl;
			if (last_style != style(word)) {
				//cout << "OK" << endl;
				word.pop_back();
				i--;
				break;
			}
			last_style = style(word);
			if (!is_word) {
				if (isOperator(word)) {
					is_prev_op = true;
				} else {
					if (is_prev_op) {
						word.pop_back();
						i--;
						break;
					}
				}
			}
		}
		if (isOperator(word)) 
			return new Operator(getOperatorByName(word));
		if (isVariable(word))
			return new Variable(word, 0);
		cout << word << "$";
		throw WRONG_VAR;
	}
	while (isVariableChar(input[i]))
		word.push_back(input[i++]);
	if (isVariable(word))
		return new Variable(word, 0);
	cout << word;
	throw WRONG_VAR;
}

void LexemVector::parseLexem(const string & input) {
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
		line.push_back(cur_lex);
	}
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
	LexemVector line;
	try {
		line.parseLexem(input);
		cout << line;
	} catch (ERROR_CODES er_code) {
		writeMessage(er_code);
	}
	LexemVector postfix = line.buildPostfix();
	line.free();
	cout << postfix;
	map<string, Variable *> var_table = postfix.evaluatePostfix();
	printTable(var_table);
	return 0;
}
