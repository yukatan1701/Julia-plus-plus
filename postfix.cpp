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

enum ERROR_CODES { WRONG_VAR, WRONG_LABEL, GOTO_DEF_ERROR };

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

const vector<string> OPERTEXT { "=", "or", "and", "|", 
                                "^", "&", "==", "!=", "<", 
								"<=", ">", ">=", "<<", ">>", 
								"+", "-", "*", "/", "%",
								"(", ")", ":", "goto"};

enum OPERATOR {
	ASSIGN, OR, AND, BITOR, 
	XOR, BITAND, EQ, NEQ, LT,
	LEQ, GT, GEQ, SHL, SHR,
	PLUS, MINUS, MULT, DIV, MOD,
	LBRACKET, RBRACKET, LABEL, GOTO
};

enum OPERSTYLE { CHARS, WORD };

int PRIORITY[] = {
	1, 2, 3, 4, 
	5, 6, 7, 7, 8,
	8, 8, 8, 9, 9,
	10, 10, 11, 11, 11,
	12, 12, 13, 13
};

map<string, int> label_table;

const char DELIMITER = ' ';
const char NEWLINE = '\n';

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
	OPERATOR getType() const { return opertype; };
	int getValue() const { return 0; }
	int getValue(const Lexem * l_left, const Lexem * l_right, 
	             map<string, Variable *> & var_table) const;
	LEXEMTYPE getLexemType() const { return OPER; };
	int priority() const { return PRIORITY[opertype]; }
	virtual bool isBinary() const { return true; }
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
		//case GOTO:
		//	const Variable *label = static_cast<const Variable *>(l_left);
		//	return label_table[label->getName()];
	}
	return result;
}

class LexemVector {
	vector< vector<Lexem *> > lines;
	bool isVariableChar(char) const;
	bool isOperatorChar(char) const;
	bool isVariable(const string &) const;
	bool isOperator(const string &) const;
	Lexem *readNumber(const string &, int &) const;
	Lexem *wordToLexem(const string &, int &) const;
	bool checkPriority(const stack<Operator *> &, const Operator *) const;
	OPERSTYLE style(const string &) const;
public:
	LexemVector() {};
	void parseLexem(const string & input);
	void checkGoto();
	void buildPostfix();
	map<string, Variable *> evaluatePostfix() const;
	void free();
	~LexemVector();
	friend ostream& operator<<(ostream &os, const LexemVector& lv);
};

ostream& operator<<(ostream &os, const LexemVector& lv) {
	for (auto line : lv.lines) {
		for (auto elem : line)
			elem->print();
		cout << "; ";
	}
	cout << endl;
}

void LexemVector::free() {
	for (auto line : lines) {
		for (auto elem : line)
			delete elem;
		line.clear();
	}
}

LexemVector::~LexemVector() {
	cout << "Destructor." << endl;
	free();
}

enum ACCOCIATE { LEFT, RIGHT };

bool LexemVector::checkPriority(const stack<Operator *> & op_stack, const Operator *op) const {
	ACCOCIATE pos = LEFT;
	if (op->getType() == ASSIGN)
		pos = RIGHT;
	if (pos == LEFT)
		return op->priority() <= op_stack.top()->priority();
	return op->priority() < op_stack.top()->priority();
}

void LexemVector::buildPostfix() {
	for (int i = 0; i < lines.size(); i++) {
		stack<Operator *> op_stack;
		if (lines[i].empty())
			continue;
		vector<Lexem *> postfix;
		for (Lexem *cur : lines[i]) {
			LEXEMTYPE lextype = cur->getLexemType();
			if (lextype == NUM || lextype == VAR) {
				postfix.push_back(cur);
			} else {
				Operator *op = static_cast<Operator *>(cur);
				OPERATOR optype = op->getType();
				if (optype == LBRACKET) {
					op_stack.push(op);
				} else if (optype == RBRACKET) {
					if (!op_stack.empty()) {
						while (!op_stack.empty() && 
							   op_stack.top()->getType() != LBRACKET) {
							postfix.push_back(op_stack.top());
							op_stack.pop();
						}
						op_stack.pop();
					}
				} else {
					if (!op_stack.empty()) {
						while(!op_stack.empty() 
							  && checkPriority(op_stack, op)
							  && op_stack.top()->getType() != LBRACKET) {
							postfix.push_back(op_stack.top());
							op_stack.pop();
						}
					}
					op_stack.push(op);
				}
			}
		}
		while (!op_stack.empty()) {
			postfix.push_back(op_stack.top());
			op_stack.pop();
		}
		lines[i] = postfix;
		//cout << "Stack size: " << op_stack.size() << endl;
		//cout << "Postfix size: " << postfix.size() << endl;
	}
}

void printTable(const map<string, Variable *> & var_table) {
	cout << endl << "Var\t" << "| Value:" << endl;
	cout << "----------------" << endl;
	for (auto it = var_table.cbegin(); it != var_table.cend(); it++) {
		cout << it->first << "\t| " << it->second->getValue() << endl;
	}
}

void printLabelTable(const map<string, int> & label_table) {
	cout << endl << "Label\t" << "| Address:" << endl;
	cout << "----------------" << endl;
	for (auto it = label_table.cbegin(); it != label_table.cend(); it++) {
		cout << it->first << "\t| " << it->second << endl;
	}
}

map<string, Variable *> LexemVector::evaluatePostfix() const {
	map<string, Variable *> var_table;
	if (lines.size() == 0)
		return var_table;
	vector<Lexem *> temporary;
	for (int i = 0; i < lines.size(); ) {
		stack<Lexem *> values;
		if (lines[i].empty()) {
			i++;
			continue;
		}
		const vector<Lexem *> & cur_line = lines[i++];
		for (Lexem *cur : cur_line) {
			LEXEMTYPE lextype = cur->getLexemType();
			if (lextype == NUM) {
				values.push(cur);
			} else if (lextype == VAR) {
				Variable *var = static_cast<Variable *>(cur);
				var_table.insert(pair<string, Variable *>(var->getName(), var));
				values.push(var_table.find(var->getName())->second);
			} else {
				Operator *op = static_cast<Operator *>(cur);
			//	op->print();
				/*if (op->getType() == EOL) {
					// TODO: free stack
					continue;
				}*/
				if (op->getType() == GOTO) {
					Goto *gt = static_cast<Goto *>(op);
					i = gt->getAddress();
					break;
				}
				const Lexem *val2 = values.top();
				values.pop();
				int result = 0;
				if (op->isBinary()) {
					const Lexem *val1 = values.top();
					values.pop();
					result = op->getValue(val1, val2, var_table);
				} else {
				
				}
				Lexem *res = new Number(result);
				values.push(res);
				temporary.push_back(res);
			}
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
		if (oper.find(ch) != std::string::npos)
			return true;
	}
	return isalpha(ch);
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
	vector<Lexem *> line;
	for (int i = 0; i < input.size(); ) {
		char ch = input[i];
		while (input[i] == DELIMITER)
			ch = input[++i];
		Lexem *cur_lex;
		if (isdigit(ch)) {
			cur_lex = readNumber(input, i);
		} else {
			cur_lex = wordToLexem(input, i);
			// add label
			if (ch == ':') {
				Lexem *last_lex = line[line.size() - 1];
				if (last_lex->getLexemType() != VAR) {
					throw WRONG_LABEL;
				}
				Variable *last_var = static_cast<Variable *>(last_lex);
				label_table[last_var->getName()] = i;
				line.pop_back();
				delete last_lex;
				delete cur_lex;
				continue;
			}
		}
		line.push_back(cur_lex);
	}
	lines.push_back(line);
}

void LexemVector::checkGoto() {
	for (int j = 0; j < lines.size(); j++) {
		vector<Lexem *> & line = lines[j];
		Lexem *lexem;
		int n = line.size();
		for (int i = 0; i < n; i++) {
			lexem = line[i];
			if (lexem->getLexemType() == OPER) {
				Operator *op = static_cast<Operator *>(lexem);
				if (op->getType() == GOTO) {
					if (i < n - 1 && line[i + 1]->getLexemType() == VAR) {
						Variable *addr = static_cast<Variable *>(line[i + 1]);
						if (label_table.find(addr->getName()) == label_table.end())
							throw GOTO_DEF_ERROR;
						Goto *gt = new Goto(label_table[addr->getName()]);
						delete op;
						delete addr;
						line.resize(n - 2);
						line.push_back(gt);
						break;
					} else
						throw GOTO_DEF_ERROR;
				}
			}
		}
	}
}

void writeMessage(ERROR_CODES er_code) {
	cout << "Error "<< er_code << ": ";
	switch (er_code) {
		case WRONG_VAR:
			cout << "wrong variable name.";
			break;
		case WRONG_LABEL:
			cout << "wrong label definition.";
			break;
		case GOTO_DEF_ERROR:
			cout << "wrong goto definition.";
			break;
	}
	cout << endl;
}

int main() {
	string input;
	LexemVector code;
	try {
		while (getline(cin, input))
			code.parseLexem(input);
		code.checkGoto();
		cout << "\nInfix:\n" << code;
		code.buildPostfix();
		cout << "\nPostfix:\n" << code;
		//code.free();
		printLabelTable(label_table);
		map<string, Variable *> var_table = code.evaluatePostfix();
		printTable(var_table);
	} catch (ERROR_CODES er_code) {
		writeMessage(er_code);
	}
	return 0;
}
