#include "syntax_analyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(const LexicalAnalyzer & la, LexemVector & lv) {
	findFunctions(lv);
	addLexemNumbers(lv);
	buildPostfix(lv);
}

void SyntaxAnalyzer::addLexemNumbers(LexemVector & lv) {
	for (int i = 0; i < lv.lines.size(); i++) {
		for (int j = 0; j < lv.lines[i].size(); j++) {
			Lexem *cur = lv.lines[i][j];
			cur->setPos(i, j);
		}
	}
}

void SyntaxAnalyzer::findFunctions(LexemVector & lv) {
	Function *last_func = nullptr;
	bool has_return = true;
	bool has_global = false;
	for (int i = 0; i < lv.lines.size(); i++) {
		for(int j = 0; j < lv.lines[i].size(); j++) {
			Lexem *cur = lv.lines[i][j];
			if (j == 0 && cur->getLexemType() == OPER) {	
				Operator *op = static_cast<Operator *>(cur);
				if (op->getType() == RETURN) {
					if (last_func == nullptr)
						throw Error(NO_FUNCTION_DEF, i, j);
					if (j == lv.lines[i].size() - 1)
						last_func->setType(VOID);
					else
						last_func->setType(INT);
					has_return = true;
				} else if (op->getType() == GLOBAL) {
					if (has_global)
						throw Error(GLOBAL_REDEFINITION, i, j);
					if (!has_return)
						throw Error(NO_RETURN, i, j);
					has_global = true;
				}
			}
			if (j == 0) continue;
			Lexem *prev = lv.lines[i][j - 1];
			//function definition
			if (cur->getLexemType() == VAR && prev->getLexemType() == OPER) {
				Operator *op = static_cast<Operator *>(prev);
				Variable *var = static_cast<Variable *>(cur);
				if (op->getType() == FUNCTION) {
					if (!has_return)
						throw Error(NO_RETURN, i, j);
					has_return = false;
				}
				if (op->getType() == FUNCTION) {
					if (has_global)
						throw Error(FUNCTION_AFTER_GLOBAL, i, j);
					set<string> args;
					for (int k = j + 1; k < lv.lines[i].size(); k++) {
						Lexem *lex = lv.lines[i][k];
						if (lex->getLexemType() == VAR) {
							Variable *arg = static_cast<Variable *>(lex);
							args.insert(arg->getName());
						}
						delete lex;
					}
					lv.lines[i].resize(1);
					Function *func = new Function(var->getName(), args, i);
					last_func = func;
					func_table.insert(pair<string, Function *>(var->getName(), func));
					delete cur;
					lv.lines[i][j - 1] = (Lexem *) func;
					break;
				} 
			}
		}
	}
	if (last_func != nullptr && !has_global)
		throw Error(NO_ENTRY_POINT, 0, 0);
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

void SyntaxAnalyzer::processPlusplus(LexemVector & lv, stack<Operator *> & op_stack, 
                                     Operator *op, Lexem *prev, int i, int j) {
	Plusplus *oper = nullptr;
	OPERATOR optype = op->getType();
	if (prev != nullptr && (prev->getLexemType() == VAR)) {
		oper = new Plusplus(optype, POST);
	} else if (j < lv.lines[i].size() - 1 && 
	          (lv.lines[i][j + 1]->getLexemType() == NUM ||
	          lv.lines[i][j + 1]->getLexemType() == VAR)) {
		oper = new Plusplus(optype, PRE);						
	} else if (prev != nullptr && prev->getLexemType() == OPER) {
		Operator *op_prev = static_cast<Operator *>(prev);
		if (op_prev->getType() != RSQUARE)
			throw Error(WRONG_INCREMENT_OPERAND, op);
		oper = new Plusplus(optype, POST);
	} else if (j < lv.lines[i].size() - 1 && 
	          (lv.lines[i][j + 1]->getLexemType() == OPER)) {
		Operator *op_next = static_cast<Operator *>(lv.lines[i][j + 1]);
		if (op_next->getType() != LSQUARE)
			throw Error(WRONG_INCREMENT_OPERAND, op);
		oper = new Plusplus(optype, PRE);
	} else
		throw Error(WRONG_INCREMENT_OPERAND, op);
	oper->setPos(op);
	delete lv.lines[i][j];
	lv.lines[i][j] = oper;
	op_stack.push(oper);
}

bool SyntaxAnalyzer::isPairOperator(const Operator *op) {
	OPERATOR optype = op->getType();
	for (OPERATOR type: PAIR_OPS) {
		if (type == optype)
			return true;
	}
	return false;
}

void SyntaxAnalyzer::buildPostfix(LexemVector & lv) {
	for (unsigned int i = 0; i < lv.lines.size(); i++) {
		stack<Operator *> op_stack;
		stack<char> br_stack;
		if (lv.lines[i].empty())
			continue;
		vector<Lexem *> postfix;
		Lexem *prev = nullptr;
		for (unsigned int j = 0; j < lv.lines[i].size(); j++) {
			Lexem *cur = lv.lines[i][j];
			int line_size = lv.lines[i].size();
			LEXEMTYPE lextype = cur->getLexemType();
			if (lextype == FUNC) {
				postfix.push_back(cur);
				break;
			}
			if (lextype == NUM || lextype == VAR) {
				postfix.push_back(cur);
			} else {
				Operator *op = static_cast<Operator *>(cur);
				OPERATOR optype = op->getType();
				if (optype == GLOBAL && line_size > 1)
					throw Error(TOO_MANY_WORDS, op);
				if (isPairOperator(op) && line_size == 1)
					throw Error(TOO_FEW_WORDS, op);
				if (optype == PLUSPLUS || optype == MINMIN) {
					processPlusplus(lv, op_stack, op, prev, i, j);
					continue;
				}
				if (optype == MINUS && (prev == nullptr || prev->getLexemType() == OPER)) {
					Operator *prev_op = static_cast<Operator *>(prev);
					if (prev_op->goodForUnary()) {
						UnaryMinus *umin = new UnaryMinus();
						umin->setPos(op);
						delete lv.lines[i][j];
						lv.lines[i][j] = umin;
						op_stack.push(umin);
						continue;
					}
				}
				if (optype == LBRACKET) {
					if (prev != nullptr && prev->getLexemType() == VAR) {
						br_stack.push('*'); // special bracket for function call
						postfix.push_back(op);
					}
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
