#include "lexem_vector.h"

LexemVector::LexemVector() {
	vector<Lexem *> code_end;
	lines.push_back(code_end);
}

ostream& operator<<(ostream &os, const LexemVector& lv) {
	for (auto line : lv.lines) {
		for (auto elem : line)
			elem->print();
		cout << "; ";
	}
	cout << endl;
}

void LexemVector::free() {
	for (auto line : lines) {
		for (auto elem : line)
			delete elem;
		line.clear();
	}
}

LexemVector::~LexemVector() {
	//cout << "Destructor." << endl;
	free();
}
