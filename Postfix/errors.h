#ifndef __ERRORS_H__
#define __ERRORS_H__

#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;36m"
#define RESET   "\033[0m"
#define RED     "\033[1;31m"

enum ERROR_CODES { WRONG_VAR, 
				   WRONG_LABEL, 
				   GOTO_DEF_ERROR,
				   GOTO_NOT_FOUND,
                   NEGATIVE_INDEX, 
				   NEGATIVE_SIZE, 
				   ARRAY_NOT_FOUND,
				   ARRAY_REDEFINITION,
				   INDEX_OUT_OF_RANGE
				 };

class Error {
	ERROR_CODES code;
	int row, col;
public:
	Error(ERROR_CODES err, int row = 0, int col = 0): code(err), row(row), col(col) {}
	void print();
};

#endif