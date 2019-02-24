#include <iostream>
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"
#include "executer.h"

using std::cin;
using std::cout;
using std::endl;

void writeMessage(ERROR_CODES ec) {
	cout << "Error "<< ec << ": ";
	if (ec == WRONG_VAR)
		cout << "wrong variable name.";
	else if (ec == WRONG_LABEL)
		cout << "wrong label definition.";
	else if (ec == GOTO_DEF_ERROR)
		cout << "wrong goto definition.";
	else if (ec == NEGATIVE_INDEX)
		cout << "negative index.";
	else if (ec == NEGATIVE_SIZE)
		cout << "negative array size.";
	else if (ec == ARRAY_REDEFINITION)
		cout << "array alrealy exist.";
	else if (ec == ARRAY_NOT_FOUND)
		cout << "array does not exist.";
	cout << endl << YELLOW << "TERMINATED" << endl;
}

int main() {
	LexemVector code;
	try {
		LexicalAnalyzer la(code);
		cout << BLUE << "Infix:\n" << RESET << code << endl;
		SyntaxAnalyzer sa(la, code);
		cout << BLUE << "Postfix:\n" << RESET << code << endl;
		return 0;
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
