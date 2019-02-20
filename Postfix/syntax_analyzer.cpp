#include "syntax_analyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(const LexicalAnalyzer & la, LexemVector & lv) {
	buildPostfix(lv);
}

bool SyntaxAnalyzer::checkPriority(const stack<Operator *> & op_stack, const Operator *op) const {
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
		if (lv.lines[i].empty())
			continue;
		vector<Lexem *> postfix;
		for (Lexem *cur : lv.lines[i]) {
			LEXEMTYPE lextype = cur->getLexemType();
			if (lextype == NUM || lextype == VAR) {
				postfix.push_back(cur);
			} else {
				Operator *op = static_cast<Operator *>(cur);
				OPERATOR optype = op->getType();
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
