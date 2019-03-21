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
	"Index out of range",
	"Invalid argument type",
	"Opening bracket is missed",
	"Operand type is incorrect or operand is missed",
	"Separator (space or comma) is missed",
	"Invalid use of increment (decrement)",
	"Invalid using of unary minus",
	"Wrong local variable definition",
	"Too many words on line of 'global'",
	"Too few words for operator",
	"No function definition for operator 'return'",
	"No 'return' definition for previous function",
	"'global' section has already been defined",
	"Function definition after the 'global' section",
	"Entry point not found",
	"The number of parameters does not match the number of arguments"
};

void Error::print() {
	cout << RED << "Error on line " << row + 1 << ", lexem " 
	     << col + 1 << ": " << RESET;
	cout << err_text[code] << "." << endl;
}
