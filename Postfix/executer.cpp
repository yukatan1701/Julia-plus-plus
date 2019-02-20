#include "executer.h"

Executer::Executer(const SyntaxAnalyzer & sa, LexemVector & lv) {
	//findArrays(lv);
	evaluatePostfix(lv);
}
/*
void Executer::findArrays(LexemVector & lv) {
	for (auto line : lv) {
		for (int i = 0; i < line.size(); i++) {
			if (line[i]->getLexemType() == OPER) {
				Operator *op = static_cast<Operator *>(line[i]);
				if (op->getType() == LSQUARE) {
					if (i > 0 && line[i - 1]->getLexemType() == VAR) {
						Variable *var = static_cast<Variable *>(line[i - 1]);	
					} else {
						// TODO: syntax error
					}
				}
			}
		}
	}
}
*/
void Executer::evaluatePostfix(LexemVector & lv) {
	vector<Lexem *> temporary;
	for (int i = 0; i < lv.lines.size(); ) {
		stack<Lexem *> values;
		if (lv.lines[i].empty()) {
			i++;
			continue;
		}
		const vector<Lexem *> & cur_line = lv.lines[i++];
		for (int j = 0; j < cur_line.size(); j++) {
			Lexem *cur = cur_line[j];
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
				if (op->getType() == NEWLINE) {
					cout << endl;
					continue;		
				}
				if (op->getType() == SPACE) {
					cout << " ";
					continue;		
				}
				Lexem *val2 = values.top();
				values.pop();
				int result = 0;
				if (op->getType() == IF) {
					If *opif = static_cast<If *>(op);
					int cond = val2->getValue();
					if (cond == 0)
						i = opif->getElse();
					break;
				}
				if (!op->isBinary()) {
					result = op->getValue(val2, val2, var_table);
					continue;
				}
				Lexem *val1 = values.top();
				values.pop();
				if (op->getType() == LSQUARE) {
					// definition
					Variable *var = static_cast<Variable *>(val1);
					Number *num = static_cast<Number *>(val2);
					if (j == cur_line.size() - 1) {
						arr_table[var->getName()] = new vector<int>(num->getValue());
					} else {
						vector<int> *array = arr_table[var->getName()];
						int & cell = array->at(num->getValue());
						Lexem *ref = new Reference(var->getName(), cell);
						values.push(ref);
						temporary.push_back(ref);
					}
					continue;
				}
				result = op->getValue(val1, val2, var_table);
				Lexem *res = new Number(result);
				values.push(res);
				temporary.push_back(res);
			}
		}
	}
	for (auto elem : temporary)
		delete elem;
}

Executer::~Executer() {
	for (auto it = arr_table.cbegin(); it != arr_table.cend(); it++)
		delete it->second;
}

void Executer::printVariables() const {
	cout << endl << BLUE << "Var\t" << RESET << "| " 
	     << BLUE << "Value" << RESET << endl;
	cout << "--------+--------" << endl;
	for (auto it = var_table.cbegin(); it != var_table.cend(); it++) {
		cout << it->first << "\t| " << it->second->getValue() << endl;
	}
	if (var_table.cbegin() == var_table.cend())
		cout << "<empty>" << endl;
}

void Executer::printArrays() const {
	cout << endl << BLUE << "Arrays: " << RESET << endl;
	cout << "------------------" << endl;
	for (auto it = arr_table.cbegin(); it != arr_table.cend(); it++) {
		vector<int> & arr = *(it->second);
		cout << it->first << "[" << arr.size() << "]" << ": ";
		for (auto num : arr)
			cout << num << " ";
		cout << endl;
	}
}
