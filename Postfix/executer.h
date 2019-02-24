#ifndef __EXECUTER_H__
#define __EXECUTER_H__

#include <map>
#include <set>
#include <utility>
#include "syntax_analyzer.h"
#include "lexems.h"

using std::set;
using std::pair;
using std::make_pair;
using std::map;

class Executer {
	friend class LexemVector;
private:
	map<string, Function *> func_table;
	stack<Lexem *> values;
	vector<Lexem *> temporary;
	stack<int> stack_sizes;
	int entry_point;

	map<string, Variable *> * global_var_table;
	map<string, vector<int> *> * global_arr_table;
	stack< map<string, Variable*> *> var_tables;
	stack< map<string, vector<int> *> *> arr_tables;
	stack< pair<int, int> > returnAddresses;

	void createTables();
	void addToStack(stack<int> args, int count);
	void findEntryPoint(const LexemVector & lv);
	void loadVariables(map<string, Variable *> & var_table, Function *func);
	void deleteTables();
	void evaluatePostfix(LexemVector & lv);
	void processArray(map<string, vector<int> *> & arr_table,
	                  Lexem *val1, Lexem *val2, int j,
	                  int cur_line_size,
                      vector<Lexem *> & temporary, 
					  stack<Lexem *> & values);
	bool isFastOperator(Operator *op);
	void printInnerVars(const map<string, Variable *> & table) const;
	void printInnerArrs(const map<string, vector<int> *> & table) const;
public:
	Executer(const SyntaxAnalyzer & sa, LexemVector & lv);
	~Executer();
	void printVariables() const;
	void printArrays() const;
};

#endif
