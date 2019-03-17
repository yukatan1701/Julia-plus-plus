#include <iostream>
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"
#include "executer.h"
#include "errors.h"

using std::cin;
using std::cout;
using std::endl;

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
	} catch (Error err) {
		err.print();
	}
	return 0;
}
