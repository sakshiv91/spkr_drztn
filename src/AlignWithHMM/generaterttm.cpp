#include <set>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <iterator>
#include <cmath>
#include <iomanip>
using namespace std;
#define tr(con, it) for(auto it = con.begin(); it != con.end(); it++)

void clearstream(stringstream& ss){
	ss.str("");
	ss.clear();
}

int ston(string s){
	stringstream ss;
	ss << s;
	int num;
	ss >> num;
	return num;
}

string ntos(int num){
	stringstream ss;
	ss << num;
	return ss.str();
}

int main(int argc, char *argv[]){
	string recout = argv[1], mapf = argv[2], filename = argv[3];
	int tc = ston(argv[4]);
	ifstream recoutfile(recout.c_str()), mapfile(mapf.c_str());
	ofstream outfile("result.rttm");
	string s;
	stringstream ss;
	map<int, int> mapping;
	int first, second;
	while(getline(mapfile, s)){
		clearstream(ss);
		ss << s;
		ss >> first >> second;
		mapping.insert(pair<int, int>(second, first));
	}
	getline(recoutfile, s);
	getline(recoutfile, s);
	getline(recoutfile, s);
	long long int sframe, eframe;
	string ul;
	long double likelihood;		
	int i, stateName, sf, ef;
	int lines = 0;
	/*while(getline(totclus, s)){
		lines++;
	}*/
	for(int it = 2; it < tc+2; it++){
		outfile << "SPKR-INFO " << filename << " 1 <NA> <NA> <NA> adult_male " << it << " <NA>\n";
	}
	while(getline(recoutfile, s)){
		int index1 = s.find("[");
		int index2 = s.find("]");
		string state = s.substr(index1+1, index2-index1-1);
		stateName = ston(state);
		stringstream ss(s);
		if(i ==0)
			ss >> sframe >> eframe >> ul >> likelihood >> ul;
		else 
			ss >> sframe >> eframe >> ul >> likelihood;
		i++;
		sf = ceil(sframe/(double)100000);
		ef = ceil(eframe/(double)100000);
	//	cout << mapping[sf] << " " << (mapping[ef] - mapping[sf])/(double)100<< endl;
		outfile << "SPEAKER " <<  filename << " 1 " << setprecision(6) << mapping[sf]/(double)100 << " "  << setprecision(3)<< (mapping[ef] - mapping[sf])/(double)100 << " <NA> <NA> " << stateName << " <NA>\n";
	}
	
	outfile.close();
	return 0;
}


















