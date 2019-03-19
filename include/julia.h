#ifndef __JULIA_H__
#define __JULIA_H__

#include <iostream>
#include <string>
#include <fstream>
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"
#include "executer.h"
#include "errors.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ifstream;

class Julia {
	void process(const string & text) const;
public:
	void run(string & code) const;
	void run(ifstream & file) const;
};

#endif
