#include <iostream>

using namespace std;

int main() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 8; j++) {
			if (j == 3)
				j = 6;
			cout << i << " " << j << endl;
		}
	}
	return 0;
}
