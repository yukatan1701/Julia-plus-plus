#ifndef __LEXICAL_ANALYZER_H__
#define __LEXICAL_ANALYZER_H__

#include "lexem_vector.h"
#include "lexems.h"
#include "errors.h"
#include <string>
#include <stack>
#include <cctype>
#include <map>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::stack;

enum OPERSTYLE { CHARS, WORD };

class LexicalAnalyzer {
	friend class LexemVector;
private:
	map<string, int> label_table;
	bool isVariableChar(char) const;
	bool isOperatorChar(char) const;
	bool isVariable(const string &) const;
	bool isOperator(const string &) const;
	Lexem *readNumber(const string &, int &) const;
	Lexem *wordToLexem(const string &, int &, int line_num, int lex_num) const;
	OPERATOR getOperatorByName(const string & op) const;
	OPERSTYLE style(const string &) const;
	void parseLexem(LexemVector & lines, const string & input);
	void checkGoto(LexemVector & lv);
	void checkIf(LexemVector & lv);
	void checkWhile(LexemVector & lv);
public:
	LexicalAnalyzer(LexemVector & lv, const string & text);
	void runAnalyzer(LexemVector & lv, const string & text);
	void printLabels() const;
};

#endif
