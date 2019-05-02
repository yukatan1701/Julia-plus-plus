#include "lexem_vector.h"

LexemVector::LexemVector()
{
	vector<Lexem *> code_end;
	lines.push_back(code_end);
}

ostream& operator<<(ostream &os, const LexemVector& lv)
{
	int i = 1;
	for (auto line : lv.lines) {
		cout << i++ << " | ";
		for (auto elem : line)
			elem->print();
		cout << "; " << endl;
	}
	cout << endl;
}

void LexemVector::free()
{
	for (auto line : lines) {
		for (auto elem : line)
			delete elem;
		line.clear();
	}
}

LexemVector::~LexemVector()
{
	//cout << "Destructor." << endl;
	free();
}
