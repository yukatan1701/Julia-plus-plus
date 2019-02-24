#ifndef __EXECUTER_H__
#define __EXECUTER_H__

#include <map>
#include <set>
#include "syntax_analyzer.h"
#include "lexems.h"

using std::set;
using std::pair;
using std::map;

class Executer {
	friend class LexemVector;
private:
	map<string, Variable *> var_table;
	map<string, vector<int> *> arr_table;
	void evaluatePostfix(LexemVector & lv);
	void processArray(Lexem *val1, Lexem *val2, int j,
	                  int cur_line_size,
                      vector<Lexem *> & temporary, 
					  stack<Lexem *> & values);
	bool isFastOperator(Operator *op);
public:
	Executer(const SyntaxAnalyzer & sa, LexemVector & lv);
	~Executer();
	void printVariables() const;
	void printArrays() const;
};

#endif
