#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {
	ifstream filelist(argv[1]);
	ofstream outfile(argv[2]);
	string line;
	while(getline(filelist, line)) {
		outfile << "./Stages 19 20 5 5 3 " << line << " ../speechData/ascii_speechs/" << line << ".speech ../speechData/nonoverscps/" << line << ".nonoverscp ../speechData/mappings/" << line << ".map 250" << endl;
	}
	cout << "File " << argv[2] << " written." << endl;
	filelist.close();
	outfile.close();
	return 0;
}
