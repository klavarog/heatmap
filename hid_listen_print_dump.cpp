#include <iostream>
#include <fstream>
#include <string>

int main() {
	using namespace std;
	ifstream fin("dump.txt");
	if (fin) {
		while (!fin.eof()) {
			string str;
			getline(fin, str);
			cout << str << endl;
		}
	} else {
		cout << "File dump.txt isn't exists." << endl;
	}
	fin.close();
}