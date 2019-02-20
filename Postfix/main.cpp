#include <iostream>
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"
#include "executer.h"

using std::cin;
using std::cout;
using std::endl;

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
		cout << BLUE << "Infix:\n" << RESET << code << endl;
		SyntaxAnalyzer sa(la, code);
		cout << BLUE << "Postfix:\n" << RESET << code << endl;
		cout << YELLOW << "-----------EXECUTION-----------" << RESET << endl;
		Executer ex(sa, code);
		cout << YELLOW << endl << "-------------------------------" << RESET << endl;
		la.printLabels();
		ex.printVariables();
		ex.printArrays();
	} catch (ERROR_CODES er_code) {
		writeMessage(er_code);
	}
	return 0;
}
