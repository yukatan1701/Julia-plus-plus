#ifndef __LEXEM_VECTOR_H__
#define __LEXEM_VECTOR_H__

#include <vector>
#include <iostream>
#include "lexems.h"

using std::ostream;
using std::istream;
using std::vector;

class LexemVector {
	friend class LexicalAnalyzer;
	friend class SyntaxAnalyzer;
	friend class Executer;
private:
	vector< vector<Lexem *> > lines;
public:
	LexemVector();
	void free();
	~LexemVector();
	friend ostream& operator<<(ostream &os, const LexemVector& lv);
};

#endif
