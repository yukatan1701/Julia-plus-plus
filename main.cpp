#include "julia.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cstring>

using std::string;
using std::ifstream;
using std::cout;
using std::endl;

enum ARG_ERR { NOFILE };

void parseArgs(string & filename, bool & debug, int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			if (i + 1 >= argc)
				throw NOFILE;
			filename.assign(argv[i + 1]);
		} else if (strcmp(argv[i], "-d") == 0) {
			debug = true;
		}
	}
}

string readCode()
{
	string code, line;
	while (getline(cin, line))
		code += line + ";";
	return code;
}

int main(int argc, char *argv[])
{
	Julia julia;
	string filename;
	bool debug_mode = false;
	try {
		parseArgs(filename, debug_mode, argc, argv);
	} catch(ARG_ERR err) {
		cout << "Invalid filename." << endl;
		return -1;
	}
	if (filename.length() > 0) {
		ifstream code(filename);
		if (!code.is_open()) {
			cout << "Failed to open code." << endl;
			return -1;
		}
		julia.run(code, debug_mode);
	} else {
		string code = readCode();
		julia.run(code, debug_mode);
	}
	return 0;
}
