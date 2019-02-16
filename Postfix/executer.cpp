#include "executer.h"

Executer::Executer(const SyntaxAnalyzer & sa, LexemVector & lv) {
	evaluatePostfix(lv);
}

void Executer::evaluatePostfix(LexemVector & lv) {
	vector<Lexem *> temporary;
	for (int i = 0; i < lv.lines.size(); ) {
		stack<Lexem *> values;
		if (lv.lines[i].empty()) {
			i++;
			continue;
		}
		const vector<Lexem *> & cur_line = lv.lines[i++];
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
				if (op->getType() == GOTO) {
					Goto *gt = static_cast<Goto *>(op);
					i = gt->getAddress();
					break;
				}
				if (op->getType() == ENDIF)
					continue;
				const Lexem *val2 = values.top();
				values.pop();
				int result = 0;
				if (op->getType() == IF) {
					If *opif = static_cast<If *>(op);
					int cond = val2->getValue();
					if (cond == 0)
						i = opif->getElse();
					break;
				}
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
}

void Executer::printVariables() const {
	cout << endl << "Var\t" << "| Value:" << endl;
	cout << "----------------" << endl;
	for (auto it = var_table.cbegin(); it != var_table.cend(); it++) {
		cout << it->first << "\t| " << it->second->getValue() << endl;
	}
}
