#include "errors.h"
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::cout;
using std::endl;
using std::vector;

const vector<string> err_text {
	"Invalid variable name",
	"Failed to set label: invalid name",
	"Invalid goto definition",
	"Label not found",
	"Negative array index",
	"Negative array size",
	"Array not found",
	"Array has already been defined",
	"Index out of range"
	};

void Error::print() {
	cout << RED << "Error on line " << row + 1 << ", lexem " 
	     << col + 1 << ": " << RESET;
	cout << err_text[code] << "." << endl;
}
