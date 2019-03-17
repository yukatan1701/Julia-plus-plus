#include "executer.h"

// TODO: clear tables
Executer::Executer(const SyntaxAnalyzer & sa, LexemVector & lv) {
	map<string, Variable *> *var_table = new map<string, Variable *>;
	var_tables.push(var_table);
	global_var_table = var_tables.top();
	map<string, vector<int> *> *arr_table = new map<string, vector<int> *>;
	arr_tables.push(arr_table);
	global_arr_table = arr_tables.top();

	findEntryPoint(lv);
	func_table = sa.func_table;
	//cout << entry_point << endl;
	evaluatePostfix(lv);
}

void Executer::findEntryPoint(const LexemVector & lv) {
	entry_point = 0;
	for (int i = 0; i < lv.lines.size(); i++) {
		if (!lv.lines[i].empty() && lv.lines[i][0]->getLexemType() == OPER) {
			Operator *op = static_cast<Operator *>(lv.lines[i][0]);
			if (op->getType() == GLOBAL) {
				entry_point = i + 1;
				break;
			}
		}
	}
}

bool Executer::isFastOperator(Operator *op) {
	if (op->getType() == ENDIF)
		return true;
	if (op->getType() == NEWLINE) {
		cout << endl;
		return true;	
	}
	if (op->getType() == SPACE) {
		cout << " ";
		return true;	
	}
	return false;
}

void Executer::processArray(Lexem *val1, Lexem *val2,
							int j, int cur_line_size) {
	map<string, vector<int> *> & arr_table = *(arr_tables.top());
	Variable *var = static_cast<Variable *>(val1);
	Number *num = static_cast<Number *>(val2);
	int pos = num->getValue();
	if (j == cur_line_size - 1) {
		if (pos < 0)
			throw NEGATIVE_SIZE;
		if (arr_table.find(var->getName()) != arr_table.end())
			throw ARRAY_REDEFINITION;
		arr_table[var->getName()] = new vector<int>(pos);
	} else {
		if (arr_table.find(var->getName()) == arr_table.end()
			&& (*global_arr_table).find(var->getName()) == global_arr_table->end())	
			throw ARRAY_NOT_FOUND;
		vector<int> *array;
		if (arr_table.find(var->getName()) != arr_table.end())
			array = arr_table[var->getName()];
		else
			array = (*global_arr_table)[var->getName()];
		if (pos < 0)
			throw NEGATIVE_INDEX;
		int & cell = array->at(pos);
		Lexem *ref = new Reference(var->getName(), cell);
		values.push(ref);
		temporary.push_back(ref);
	}
}

void Executer::addToStack(stack<int> args, int count) {
	for (int i = 0; i < count; i++) {
		//cout << args.top() << " ";
		Lexem *num = new Number(args.top());
		args.pop();
		temporary.push_back(num);
		values.push(num);
	}
	//cout << endl;
}

void Executer::createTables() {
	//cout << "Creating tables..." << endl;
	map<string, Variable *> *var_table = new map<string, Variable *>;
	var_tables.push(var_table);
	map<string, vector<int> *> *arr_table = new map<string, vector<int> *>;
	arr_tables.push(arr_table);
}

void Executer::loadVariables(map<string, Variable *> & var_table, Function *func) {
	int argc = func->getArgc();
	set<string> args = func->getArgs();
	if (args.empty())
		return;
	for (auto it = args.cbegin(); it != args.cend(); it++) {
		Number *num = static_cast<Number *>(values.top());
		Variable *var = new Variable(*it, num->getValue());
		values.pop();
		var_table[*it] = var;
	}
}

void Executer::deleteTables() {	
	map<string, Variable *> *var_table = var_tables.top();
	for (auto it = var_table->cbegin(); it != var_table->cend(); it++)
		delete it->second;
	delete var_table;
	var_tables.pop();
	map<string, vector<int> *> *arr_table = arr_tables.top();
	for (auto it = arr_table->cbegin(); it != arr_table->cend(); it++)
		delete it->second;
	delete arr_table;
	arr_tables.pop();
}

void print_stack(const stack<Lexem *> & old_st) {
	stack<Lexem *> st = old_st;
	cout << "Stack: ";
	while (!st.empty()) {
		st.top()->print();
		st.pop();
	}
	cout << endl;
}

void Executer::processVariable(Variable *var) {
	map<string, Variable *> & var_table = *(var_tables.top());
	if (var_table.find(var->getName()) == var_table.end()) {
		if (global_var_table->find(var->getName()) == global_var_table->end()) {
			var = new Variable(var->getName(), var->getValue());
			var_table.insert(pair<string, Variable *>(var->getName(), var));
			values.push(var_table[var->getName()]);
		} else {
			values.push((*global_var_table)[var->getName()]);
		}
	} else {
		values.push(var_table[var->getName()]);
	}
}

int Executer::doAssign(Lexem *val1, Lexem *val2) {
	map<string, Variable *> & var_table = *(var_tables.top());
	int left = val1->getValue(), right = val2->getValue();
	if (val1->getLexemType() == VAR) {
		const Variable *v_left = static_cast<const Variable *>(val1);
		if (var_table.find(v_left->getName()) != var_table.end())
			var_table[v_left->getName()]->setValue(right);
		else
			(*global_var_table)[v_left->getName()]->setValue(right);
	} else if (val1->getLexemType() == REF) {
		const Reference *r_left = static_cast<const Reference *>(val1);
		int & dest = r_left->getReference();
		dest = right;
	}
	return right;
}

int Executer::callFunction(stack<int> & args, int old_i, int j) {
	Lexem *last = values.top();
	if (last->getLexemType() == OPER) {
		Operator *oper = static_cast<Operator *>(last);
		if (oper->getType() != LBRACKET) {
			// TODO: syntax error
		} else {
		}
	} else if (last->getLexemType() == VAR || last->getLexemType() == NUM) {
		args.push(last->getValue());
		values.pop();
	}
	// TODO: check
	values.pop();
	Variable *func_name = static_cast<Variable *>(values.top());
	Function *func = func_table[func_name->getName()];
	values.pop();
	stack_sizes.push(values.size());
	addToStack(args, func->getArgc());
	returnAddresses.push(make_pair(old_i, j));
	return func->getLine();
}

bool Executer::doReturn(stack<Function *> & functions, int & i, int & new_j) {
	Function *cur_func = functions.top();
	if (cur_func->getType() == INT) {
		Lexem *ans = values.top();
		values.pop();
		while (values.size() != stack_sizes.top())
			values.pop();
		stack_sizes.pop();
		Number *eax = new Number(ans->getValue());
		temporary.push_back(eax);
		values.push(eax);
	}
	functions.pop();
	//printInnerVars(var_table);
	//printInnerArrs(arr_table);
	deleteTables();
	pair<int, int> addr = returnAddresses.top();
	returnAddresses.pop();
	i = addr.first;
	new_j = addr.second + 1;
	return true;
}

void Executer::evaluatePostfix(LexemVector & lv) {
	stack<Function *> functions;
	int new_j = 0;
	bool is_return = false;
	for (int i = entry_point; i < lv.lines.size(); ) {	
		stack<int> args;
		int j = 0;
		if (is_return) {
			is_return = false;
			j = new_j;
		}
		if (lv.lines[i].empty()) {
			i++;
			continue;
		}
		int old_i = i;
		const vector<Lexem *> & cur_line = lv.lines[i++];
		for ( ; j < lv.lines[old_i].size(); j++) {
			map<string, Variable *> & var_table = *(var_tables.top());
			map<string, vector<int> *> & arr_table = *(arr_tables.top());
			Lexem *cur = cur_line[j];
			//cur->print();
			//cout << endl;
			LEXEMTYPE lextype = cur->getLexemType();
			if (lextype == FUNC) {
				Function *cur_func = static_cast<Function *>(cur);
				functions.push(cur_func);
				createTables();
				loadVariables(*(var_tables.top()), cur_func);
				break;
			} else if (lextype == NUM) {
				values.push(cur);
			} else if (lextype == VAR) {
				processVariable(static_cast<Variable *>(cur));
			} else {
				Operator *op = static_cast<Operator *>(cur);
				if (op->getType() == GOTO) {
					Goto *gt = static_cast<Goto *>(op);
					i = gt->getAddress();
					break;
				}
				if (isFastOperator(op)) {
					continue;
				}
				if (op->getType() == RETURN) {
					is_return = doReturn(functions, i, new_j);
					break;
				}
				if (op->getType() == LBRACKET) {
					values.push(op);
					continue;
				}
				if (op->getType() == RBRACKET) {
					i = callFunction(args, old_i, j);
					break;
				}
				Lexem *val2 = values.top();
				values.pop();
				int result = 0;
				if (op->getType() == COMMA) {
					if (val2->getLexemType() == VAR || val2->getLexemType() == NUM) {
						args.push(val2->getValue());
						continue;
					}
					else // TODO: WRONG ARGUMENT
						;
				}
				if (op->getType() == IF) {
					If *opif = static_cast<If *>(op);
					int cond = val2->getValue();
					if (cond == 0)
						i = opif->getElse();
					break;
				}
				if (!op->isBinary()) {
					result = op->getValue(val2, val2);
					continue;
				}
				Lexem *val1 = values.top();
				values.pop();
				if (op->getType() == LSQUARE) {
					processArray(val1, val2, j, cur_line.size());
					continue;
				}
				if (op->getType() == ASSIGN)
					result = doAssign(val1, val2);
				else
					result = op->getValue(val1, val2);
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
	for (auto it = global_arr_table->cbegin(); it != global_arr_table->cend(); it++)
		delete it->second;
}

void Executer::printInnerVars(const map<string, Variable *> & table) const {
	cout << endl << BLUE << "Var\t" << RESET << "| " 
	     << BLUE << "Value" << RESET << endl;
	cout << "--------+--------" << endl;
	for (auto it = table.cbegin(); it != table.cend(); it++) {
		cout << it->first << "\t| " << it->second->getValue() << endl;
	}
	if (table.cbegin() == table.cend())
		cout << "<empty>" << endl;
}

void Executer::printVariables() const {
	printInnerVars(*global_var_table);
}

void Executer::printInnerArrs(const map<string, vector<int> *> & table) const {
	cout << endl << BLUE << "Arrays: " << RESET << endl;
	cout << "------------------" << endl;
	for (auto it = table.cbegin(); it != table.cend(); it++) {
		vector<int> & arr = *(it->second);
		cout << it->first << "[" << arr.size() << "]" << ": ";
		for (auto num : arr)
			cout << num << " ";
		cout << endl;
	}

}

void Executer::printArrays() const {
	printInnerArrs(*global_arr_table);
}
