#include "lexical_analyzer.h"

extern const vector<string> OPERTEXT;

const char DELIMITER = ' ';
const char TAB = '\t';

LexicalAnalyzer::LexicalAnalyzer(LexemVector & lv) {
	runAnalyzer(lv);
}

OPERATOR LexicalAnalyzer::getOperatorByName(const string & op) const {
	int oper_pos;
	for (oper_pos = 0; oper_pos < OPERTEXT.size(); oper_pos++) {
		if (OPERTEXT[oper_pos] == op)
			break;
	}
	return (OPERATOR) oper_pos;
}

bool LexicalAnalyzer::isVariableChar(char ch) const {
	return ch == '_' || isalnum(ch);
}

bool LexicalAnalyzer::isOperatorChar(char ch) const {
	for (auto oper : OPERTEXT) {
		if (oper.find(ch) != std::string::npos)
			return true;
	}
	return isalpha(ch);
}

Lexem * LexicalAnalyzer::readNumber(const string & input, int & i) const {
	int value = 0;
	while (isdigit(input[i]))
		value = value * 10 + (input[i++] - '0');
	return new Number(value);
}

bool LexicalAnalyzer::isOperator(const string & word) const {
	if (word.empty())
		return false;
	for (auto elem : OPERTEXT) {
		if (elem == word)
			return true;
	}
	return false;
}

bool LexicalAnalyzer::isVariable(const string & word) const {
	if (word.empty())
		return false;
	for (int i = 0; i < word.size(); i++) {
		if (!isVariableChar(word[i]))
			return false;
	}
	return true;
}

OPERSTYLE LexicalAnalyzer::style(const string & word) const {
	if (isalpha(word[word.size() - 1]))
		return WORD;
	return CHARS;
}

Lexem * LexicalAnalyzer::wordToLexem(const string & input, int & i) const {
	string word;
	if (isOperatorChar(input[i])) {
		bool is_word = isalpha(input[i]);
		bool is_prev_op = false;
		OPERSTYLE last_style = is_word ? WORD : CHARS;
		while (isOperatorChar(input[i])) {
			word.push_back(input[i++]);
			if (last_style != style(word)) {
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
		cout << word << endl;
		throw WRONG_VAR;
	}
	while (isVariableChar(input[i]))
		word.push_back(input[i++]);
	if (isVariable(word))
		return new Variable(word, 0);
	cout << word << endl;
	throw WRONG_VAR;
}

void LexicalAnalyzer::parseLexem(LexemVector & lv, const string & input) {
	vector<Lexem *> line;
	for (int i = 0; i < input.size(); ) {
		char ch = input[i];
		if (input[i] == DELIMITER || input[i] == TAB) {
			ch = input[++i];
			continue;
		}
		Lexem *cur_lex;
		if (isdigit(ch)) {
			cur_lex = readNumber(input, i);
		} else {
			cur_lex = wordToLexem(input, i);
			// add label
			if (ch == '#') {
				break;
			}
			if (ch == ':') {
				Lexem *last_lex = line[line.size() - 1];
				if (last_lex->getLexemType() != VAR) {
					throw WRONG_LABEL;
				}
				Variable *last_var = static_cast<Variable *>(last_lex);
				label_table[last_var->getName()] = lv.lines.size() - 1;
				line.pop_back();
				delete last_lex;
				delete cur_lex;
				continue;
			}
		}
		line.push_back(cur_lex);
	}
	lv.lines[lv.lines.size() - 1] = line;	
	vector<Lexem *> empty;
	lv.lines.push_back(empty);
}

void LexicalAnalyzer::checkGoto(LexemVector & lv) {
	for (int j = 0; j < lv.lines.size(); j++) {
		vector<Lexem *> & line = lv.lines[j];
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

void LexicalAnalyzer::checkIf(LexemVector & lv) {
	stack<If *> ifstack;
	for (int j = 0; j < lv.lines.size(); j++) {
		vector<Lexem *> & line = lv.lines[j];
		Lexem *lexem;
		int n = line.size();
		for (int i = 0; i < n; i++) {
			lexem = line[i];
			if (lexem->getLexemType() == OPER) {
				Operator *op = static_cast<Operator *>(lexem);
				OPERATOR type = op->getType();
				if (type == IF) {
					If *ifop = new If();
					delete op;
					line[i] = ifop;
					ifstack.push(ifop);
				} else if (type == THEN) {
					delete op;
					line.pop_back();
				} else if (type == ELSE) {
					ifstack.top()->setElse(j + 1);
					delete op;
					line.clear();
				} else if (type == ENDIF) {
					If *top = ifstack.top();
					if (top->getElse() > -1) {
						Goto *else_goto = new Goto(j);
						lv.lines[top->getElse() - 1].push_back(else_goto);
					}
					ifstack.pop();
				}
			}
		}
	}
}

void LexicalAnalyzer::checkWhile(LexemVector & lv) {
	stack<If *> whstack;
	stack<int> whline;
	for (int j = 0; j < lv.lines.size(); j++) {
		vector<Lexem *> & line = lv.lines[j];
		Lexem *lexem;
		int n = line.size();
		for (int i = 0; i < n; i++) {
			lexem = line[i];
			if (lexem->getLexemType() == OPER) {
				Operator *op = static_cast<Operator *>(lexem);
				OPERATOR type = op->getType();
				if (type == WHILE) {
					If *ifop = new If();
					delete op;
					line[i] = ifop;
					whstack.push(ifop);
					whline.push(j);
				} else if (type == THEN) {
					delete op;
					line.pop_back();
				} else if (type == ENDWHILE) {
					If *top = whstack.top();
					int whi = whline.top();
					Goto *gt = new Goto(whi);
					top->setElse(j + 1);
					whstack.pop();
					whline.pop();
					delete op;
					line[i] = gt;
				}
			}
		}
	}
}

void LexicalAnalyzer::runAnalyzer(LexemVector & lv) {
	string input;
	while (getline(cin, input))
		parseLexem(lv, input);
	checkGoto(lv);
	checkIf(lv);
	checkWhile(lv);
}

void LexicalAnalyzer::printLabels() const {
	cout << endl << BLUE << "Label\t" << RESET << "| " 
	     << BLUE << "Address" << RESET << endl;
	cout << "--------+--------" << endl;
	for (auto it = label_table.cbegin(); it != label_table.cend(); it++) {
		cout << it->first << "\t| " << it->second << endl;
	}
	if (label_table.cbegin() == label_table.cend())
		cout << "<empty>" << endl;
}
