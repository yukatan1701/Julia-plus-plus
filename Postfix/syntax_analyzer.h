#ifndef __SYNTAX_ANALYZER_H__
#define __SYNTAX_ANALYZER_H__

#include "lexem_vector.h"
#include "lexical_analyzer.h"
#include "lexems.h"
#include <stack>

using std::stack;

enum ACCOCIATE { LEFT, RIGHT };

class SyntaxAnalyzer {
	friend class LexemVector;
private:
	bool checkPriority(const stack<Operator *> &, const Operator *) const;
	void buildPostfix(LexemVector & lv);
public:
	SyntaxAnalyzer(const LexicalAnalyzer & la, LexemVector & lv);
};

#endif
