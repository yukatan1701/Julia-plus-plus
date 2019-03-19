#include "julia.h"
#include <fstream>
#include <string>
#include <iostream>

using std::string;
using std::ifstream;
using std::cout;
using std::endl;

string readCode() {
	string code, line;
	while (getline(cin, line))
		code += line + ";";
	return code;
}

int main(int argc, char *argv[]) {
	Julia julia;
	if (argc > 1) {
		ifstream code(argv[1]);
		if (!code.is_open()) {
			cout << "Failed to open code." << endl;
			return -1;
		}
		julia.run(code);
		code.close();
	} else {
		string code = readCode();
		julia.run(code);
	}
	return 0;
}
