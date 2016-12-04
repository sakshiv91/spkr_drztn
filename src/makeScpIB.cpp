#include "Includes.h"

void makeIBScp(string mapFile, string meeting, int chunkSize){
		ifstream infile(mapFile);
		ofstream outfile(("../speechData/ibscps/"+meeting+".IBScp").c_str());
		string s;
		stringstream ss;
		int sline = -1, srline = -1;
		map<int, int> mapping;
		getline(infile, s);
		while(getline(infile, s)){
			cs(ss);
			ss << s;	
			ss >> sline >> srline;
			mapping.insert(make_pair(sline, srline));
		}
		int count ;
		auto start = mapping.begin();
		auto it = mapping.begin();
		auto prevIt = it;
		while(it -> second < mapping.size()){
			count = 1;
			start = it ;
			while(count<= chunkSize && it -> second <= mapping.size()){
				prevIt = it;
				it++ ;
				if(it == mapping.end()) break;
				if((prevIt -> first)+1 != it -> first ){
					/*discontinuos case*/
					outfile <<meeting << ".fea[" << (start-> first)<< "," << ((prevIt -> second) - 
							(start-> second) +1)<<"]" << endl;
					start = it;
				}
				count++;
			}
			outfile << meeting << ".fea[" << (start -> first) << "," << ((prevIt-> second) -
						 (start-> second) +1)<<"]" << endl ;
			if(it == mapping.end()) break;
		}
		outfile.close();
	}
int main(int argc, char* argv[]){
/*~~~~~~~~~~~~~~~~~~~Command line arguments~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	string mapFile = argv[1];
	string meeting = argv[2];
	int chunkSize  = ston(argv[3]);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	makeIBScp(mapFile, meeting, chunkSize);
	cout << "IBScp File written to: ../speechData/ibscps/" << meeting << ".IBScp" << endl;
	return 0;
}

