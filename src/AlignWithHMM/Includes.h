#ifndef INCLUDES_H_
#define INCLUDES_H_


#include<iostream>
#include<fstream>
#include<sstream>
#include<stdlib.h>
#include<iterator>
#include<algorithm>
#include<map>
#include<set>
#include<vector>
#include<string>
#include <opencv2/core/core.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/ml.hpp"
#include <cstdlib>
#include<iomanip>
#include<string.h>

using namespace std;
using namespace cv;
using namespace cv::ml;
#define vc vector< double >
#define vvc vector< vc >
#define vi vector<int>
#define vvvc vector< vvc >
#define tr(con, it) for(auto it = con.begin(); it != con.end(); it++)
#define fr(i, s, e) for(auto i = s; i < e; i++)
#define printv(con, dt) copy(con.begin(), con.end(), ostream_iterator<dt>(cout, " ")); cout << endl;

int ston(string s){
	stringstream ss;
	ss << s;
	int num;
	ss >> num;
	return num;
}
void cs(stringstream &ss){
	ss.str(" ");
	ss.clear();
}
string ntos(int n){
	stringstream ss;
	ss << n;
	string s;
	ss >> s;
	return s;
}

Mat d2vecToMat(vvc &data){
	int n = data.size(), m = data[0].size();
	Mat matt = Mat(n, m, CV_64FC1);
	fr(i, 0, n){
		fr(j, 0, m){
			matt.at<double>(i, j) = data[i][j];
		}
	}
	
	return matt;
}
Mat d1VecToMat(vc &data){
	int n = data.size();
	Mat matt = Mat(1, n, CV_64FC1);
	fr(i, 0, n){
		matt.at<double>(0, i) = data[i];
	}
	
	return matt;
}

void updateMap(map<int, Ptr<EM> > &gmms, vi gmmIds){
	set<int> s;
	tr(gmmIds, it){
		if(s.find(*it) == s.end()){
			s.insert(*it);
		}
	}
	auto it = gmms.begin();
	while(it != gmms.end()){
		if(s.find(it->first) == s.end()){
			auto tempit = it;
			it++;
			gmms.erase(tempit);
			continue;
		}
		it++;
	}
}
#endif

