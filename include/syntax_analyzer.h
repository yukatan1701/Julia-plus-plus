#ifndef __SYNTAX_ANALYZER_H__
#define __SYNTAX_ANALYZER_H__

#include "lexem_vector.h"
#include "lexical_analyzer.h"
#include "lexems.h"
#include "errors.h"
#include <stack>
#include <map>
#include <set>
#include <string>

using std::stack;
using std::map;
using std::string;
using std::set;
using std::pair;

enum ACCOCIATE { LEFT, RIGHT };

class SyntaxAnalyzer {
	friend class LexemVector;
	friend class Executer;
private:
	map<string, Function *> func_table;
	bool checkPriority(const stack<Operator *> &, const Operator *) const;
	void processPlusplus(LexemVector & lv, stack<Operator *> & op_stack,
		Operator *op, Lexem *prev, int i, int j);
	bool isPairOperator(const Operator *op);
	void findFunctions(LexemVector & lv);
	void addLexemNumbers(LexemVector & lv);
	void buildPostfix(LexemVector & lv);
public:
	SyntaxAnalyzer(const LexicalAnalyzer & la, LexemVector & lv);
};

#endif
