#include "julia.h"

void Julia::run(string & code, bool debug_mode) const
{
	process(code, debug_mode);
}

void Julia::run(ifstream & file, bool debug_mode) const
{
	string code, line;
	while (getline(file, line))
		code += line + ";";
	process(code, debug_mode);
	file.close();
}


void Julia::process(const string & text, bool debug_mode) const
{
	LexemVector code;
	try {
		LexicalAnalyzer la(code, text);
		if (debug_mode)
			cout << BLUE << "Infix:\n" << RESET << code << endl;
		SyntaxAnalyzer sa(la, code);
		if (debug_mode) {
			cout << BLUE << "Postfix:\n" << RESET << code << endl;
			cout << YELLOW << "-----------EXECUTION-----------" << RESET << endl;
		}
		Executer ex(sa, code);
		if (debug_mode) {
			cout << YELLOW << endl << "-------------------------------" 
				<< RESET << endl;
			la.printLabels();
			ex.printVariables();
			ex.printArrays();
		}
	} catch (Error err) {
		err.print();
	}
}
