#include "syntax_analyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(const LexicalAnalyzer & la, LexemVector & lv) {
	findFunctions(lv);
	//cout << "Intermediary:" << endl << lv;
	buildPostfix(lv);
}

void SyntaxAnalyzer::findFunctions(LexemVector & lv) {
	for (int i = 0; i < lv.lines.size(); i++) {
		for(int j = 1; j < lv.lines[i].size(); j++) {
			Lexem *cur = lv.lines[i][j];
			Lexem *prev = lv.lines[i][j - 1];
			//function definition
			if (cur->getLexemType() == VAR && prev->getLexemType() == OPER) {
				Operator *op = static_cast<Operator *>(prev);
				Variable *var = static_cast<Variable *>(cur);
				if (op->getType() == FUNCTION) {
					set<string> args;
					for (int k = j + 1; k < lv.lines[i].size(); k++) {
						Lexem *lex = lv.lines[i][k];
						if (lex->getLexemType() == VAR) {
							Variable *arg = static_cast<Variable *>(lex);
							args.insert(arg->getName());
						}
						delete lex;
					}
					lv.lines[i].resize(2);
					Function *func = new Function(var->getName(), args, i);
					func_table.insert(pair<string, Function *>(var->getName(), func));
					delete cur;
					lv.lines[i][j] = (Lexem *) func;
					break;
				}
			}
			//function call
			if (cur->getLexemType() == OPER && prev->getLexemType() == VAR) {

				Operator *op = static_cast<Operator *>(cur);
				Variable *var = static_cast<Variable *>(prev);
				if (op->getType() == LBRACKET) {
					//set<Lexem *> 
				}
			}
		}
	}
}

bool SyntaxAnalyzer::checkPriority(const stack<Operator *> & op_stack, 
                                   const Operator *op) const {
	ACCOCIATE pos = LEFT;
	if (op->getType() == ASSIGN)
		pos = RIGHT;
	if (pos == LEFT)
		return op->priority() <= op_stack.top()->priority();
	return op->priority() < op_stack.top()->priority();
}

void SyntaxAnalyzer::buildPostfix(LexemVector & lv) {
	for (int i = 0; i < lv.lines.size(); i++) {
		stack<Operator *> op_stack;
		stack<char> br_stack;
		if (lv.lines[i].empty())
			continue;
		vector<Lexem *> postfix;
		Lexem *prev = nullptr;
		for (Lexem *cur : lv.lines[i]) {
			LEXEMTYPE lextype = cur->getLexemType();
			if (lextype == FUNC)
				break;
			if (lextype == NUM || lextype == VAR) {
				postfix.push_back(cur);
			} else {
				Operator *op = static_cast<Operator *>(cur);
				OPERATOR optype = op->getType();
				if (optype == LBRACKET) {
					if (prev != nullptr && prev->getLexemType() == VAR)
						br_stack.push('*'); // special bracket for function call
					else
						br_stack.push('(');
				}
				if (optype == LBRACKET || optype == LSQUARE) {
					op_stack.push(op);
				} else if (optype == RBRACKET) {
					if (!op_stack.empty()) {
						while (!op_stack.empty()
							   && op_stack.top()->getType() != LBRACKET) {
							postfix.push_back(op_stack.top());
							op_stack.pop();
						}
						op_stack.pop();
					}
					if (!br_stack.empty()) {
						if (br_stack.top() == '*')
							postfix.push_back(op);
						br_stack.pop();
					}
				} else if (optype == RSQUARE) {	
					if (!op_stack.empty()) {
						while (!op_stack.empty()
							   && op_stack.top()->getType() != LSQUARE) {
							postfix.push_back(op_stack.top());
							op_stack.pop();
						}
						postfix.push_back(op_stack.top());
						op_stack.pop();
					}
				} else if (optype == COMMA) {
					while (!op_stack.empty()
							  && op_stack.top()->getType() != LBRACKET) {
						postfix.push_back(op_stack.top());
						op_stack.pop();
					}
					postfix.push_back(op);
				} else {
					if (!op_stack.empty()) {
						while(!op_stack.empty() 
							  && checkPriority(op_stack, op)
							  && op_stack.top()->getType() != LBRACKET
							  && op_stack.top()->getType() != LSQUARE) {
							postfix.push_back(op_stack.top());
							op_stack.pop();
						}
					}
					op_stack.push(op);
				}
			}
			prev = cur;
		}
		while (!op_stack.empty()) {
			postfix.push_back(op_stack.top());
			op_stack.pop();
		}
		lv.lines[i] = postfix;
		//cout << "Stack size: " << op_stack.size() << endl;
		//cout << "Postfix size: " << postfix.size() << endl;
	}
}
