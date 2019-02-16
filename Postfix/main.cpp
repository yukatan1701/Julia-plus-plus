#include <iostream>
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"
#include "executer.h"

using std::cin;
using std::cout;
using std::endl;

int PRIORITY[] = {
	-1, -1, -1, -1,
	-1, -1,
	1, 2, 3, 4, 
	5, 6, 7, 7, 8,
	8, 8, 8, 9, 9,
	10, 10, 11, 11, 11,
	12, 12, 13, 13, 13
};

void writeMessage(ERROR_CODES er_code) {
	cout << "Error "<< er_code << ": ";
	switch (er_code) {
		case WRONG_VAR:
			cout << "wrong variable name.";
			break;
		case WRONG_LABEL:
			cout << "wrong label definition.";
			break;
		case GOTO_DEF_ERROR:
			cout << "wrong goto definition.";
			break;
	}
	cout << endl;
}

int main() {
	LexemVector code;
	try {
		LexicalAnalyzer la(code);
		cout << "\nInfix:\n" << code;
		la.printLabels();
		SyntaxAnalyzer sa(la, code);
		cout << "\nPostfix:\n" << code;
		Executer ex(sa, code);
		ex.printVariables();
	} catch (ERROR_CODES er_code) {
		writeMessage(er_code);
	}
	return 0;
}
