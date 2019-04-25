#include "julia.h"

void Julia::run(string & code) const
{
	process(code);
}

void Julia::run(ifstream & file) const
{
	string code, line;
	while (getline(file, line))
		code += line + ";";
	process(code);
	file.close();
}


void Julia::process(const string & text) const
{
	LexemVector code;
	try {
		LexicalAnalyzer la(code, text);
		cout << BLUE << "Infix:\n" << RESET << code << endl;
		SyntaxAnalyzer sa(la, code);
		cout << BLUE << "Postfix:\n" << RESET << code << endl;
		cout << YELLOW << "-----------EXECUTION-----------" << RESET << endl;
		Executer ex(sa, code);
		cout << YELLOW << endl << "-------------------------------" 
			<< RESET << endl;
		la.printLabels();
		ex.printVariables();
		ex.printArrays();
	} catch (Error err) {
		err.print();
	}
}
