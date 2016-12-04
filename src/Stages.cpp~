 /*~~~~~~~~~~~~~~~~~SPEAKER DIARIZATION SYSTEM~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~AUTHOR - SAKSHI VERMA AND PRATEEK KOTHA~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~LAST UPDATED - AUGUST 4, 2016 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~UPDATION - AUGUST 16, 2016~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include "Includes.h" 

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~Global declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	string		speechPath ;
	int		featDim ;
	vvc 		speechData ;
	int 		initCluster ;
	int 		initMixture ;
	string 		fileName ;
	int 		numFeats ;
	vvvc 		segmentData ;
	vvvc 		clusterData ;
	vi 		clusterMixtures ;
	int 		segmentationItr ; 
	int 		chunkSize;
	vi 		gmmIds ;
	string 		sdatapath;
	bool 		stop = false;
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~Read whole speech data in vector of vector of double speechData~~~~*/

void readData(string speechPath, vvc &data, string fileName){
	ifstream infile((speechPath+fileName).c_str());
	string s;
	stringstream ss;
	int c ;
	vc feature;
	int me = 0;
	while(getline(infile, s)){
		ss << s;
		double num;
		c = 0;
		fr(i, 0, featDim){
			ss >> num;
			feature.push_back(num);
		}
		data.push_back(feature);
		feature.clear();
		cs(ss);
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~Divide equally initial data amongst all the clusters~~~~~~~~~~~~~~~~~
~~~~~ After dividing there is no concept of boundaries between the segments~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~Uniform segmentation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void initClusters(int &k, vvvc &data){
	numFeats = speechData.size();
	int clusterFeatures = numFeats/k ;
	int count = 0;
	vvc temp ;
	fr(i, 0, k){
		do{
			temp.push_back(speechData[count++]);
		}while(count%clusterFeatures != 0);
		data.push_back(temp);
		temp.clear();
	}
	int rem = numFeats - count;
	if(rem > 0.9*clusterFeatures){
		while(count < numFeats){
			temp.push_back(speechData[count++]);
			k++;
		}
		data.push_back(temp);
	}
	else{
		while(count < numFeats){
			data[k-1].push_back(speechData[count++]);
		}
	}
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~Initialize all the segments of size as chunksize~~~~~~~~~~~~~~~~~*/
void initSegments( vvvc &data, int clusterFeatures){
        int count = 0;
        vvc temp ;
		int numFeats = speechData.size();
        while(numFeats - count > clusterFeatures){
                do{
						temp.push_back(speechData[count++]);
                }while(count%clusterFeatures != 0);
                data.push_back(temp);
                temp.clear();
        }
        int rem = numFeats - count;
        if(rem > 0.9*clusterFeatures){
                while(count < numFeats){
                        temp.push_back(speechData[count++]);
                }
                data.push_back(temp);
        }
        else{
                while(count < numFeats){
                        data[data.size()-1].push_back(speechData[count++]);
                }
        }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~Initializing K clusters randomly by dividing the data in random fashion~~~~~~
~~~~An option to uniform segmentation, Not used for resuts~~~~~~~~~~~~~~~~~~~~*/

void randomInit(int k, vvc &data, vvvc &randomData){
	vi lines(data.size(), 0);
	fr(i, 0, lines.size()){
		lines[i] = i;
	}
	int chunkSize = data.size()/k;
	srand(time(0));
	random_shuffle(lines.begin(), lines.end());
	int count = 0;
	vc feat;
	vvc chunk;
	fr(i, 0, k){
		do{
			chunk.push_back(data[lines[count++]]); 
		}while(count%chunkSize!=0);
		randomData.push_back(chunk);
		chunk.clear();
	}
	int rem = data.size() - count;
	if(rem > 0){
		while(count < data.size()){
			randomData[k-1].push_back(data[lines[count++]]);
		}
	}
}
/*~~~~~~~~~~~~~~~~Initializing mixture clusters~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void initClusterMixtures(int k){
	fr(i, 0, k)
		clusterMixtures.push_back(initMixture);
}

/*~~~~~~~~~~~~~~~~~~~~Initializing gmmIds~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void initGMMId(map<int, Ptr<EM>> &gmms){
	int id = -1;
	fr(i, 0, segmentData.size()){
		gmmIds.push_back(id);
	}
}

/*~~~~Make a matrix for gmm data seeing gmmIds. E.x With gmm id 10 mat will~~~~~
~~~~~~have all the segments data that belongs to id 10~~~~~~~~~~~~~~~~~~~~~~~~*/

Mat makeData(int clusterId, vvvc &clusterData, vi &gmmIds){
        vvc iData;		
        fr(i, 0, gmmIds.size()){
                if(gmmIds[i] == clusterId){
                        fr(j, 0, clusterData[i].size()){
                                iData.push_back(clusterData[i][j]);
                        }
                }
        }
        return d2vecToMat(iData);
}

/*~~~~~~~~~~~~~~Compute llh for testing purpose~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
double computeLLH(Ptr<EM> gmm, Mat data){
        double llh = 0;
        fr(i, 0, data.size().height){
                llh += gmm->predict2(data.row(i), noArray())[0];
        }
		/*normalized llh is used*/
        return llh /= data.size().height;
}

/*~~~~~~~~~~~~~~~~~Compute llh for BIC~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
double computeLLHforBIC(Ptr<EM> gmm, Mat data){
        double llh = 0;
        fr(i, 0, data.size().height){
                llh += gmm->predict2(data.row(i), noArray())[0];
        }
        return llh;
}

/*~~~~~~~~~~~~~~~~~~~~~~~Train a GMM~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

Ptr<EM> oneGMM(Mat &data, double &llh, int nc){
	Mat labels, logLikelihoods;
	double l1;
	Ptr<EM> gmmmodel = cv::ml::EM::create();
	gmmmodel->setClustersNumber(nc);
    gmmmodel->setCovarianceMatrixType(EM::COV_MAT_DIAGONAL);
	gmmmodel->setTermCriteria(TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,
	 5, 0.01));
	gmmmodel->trainEM( data, logLikelihoods, labels, noArray());
	fr(i, 0, data.size().height){
		llh = llh + logLikelihoods.at<double>(i);
	}
	/*Return normalized log likelihood for the sample trained on*/
	llh = llh / data.size().height;
	return gmmmodel;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void gmmTrain(map<int, Ptr<EM> > &gmms, vvvc &data){
	int size = data.size() ;
	double llh;
	int gmmNo = 0;
	fr(i, 0, size){
		Mat oneClusterData = d2vecToMat(data[i]);
		Ptr<EM> gmm = oneGMM(oneClusterData, llh, clusterMixtures[i]);
		//cout << "Cluster " << i << ": " << llh << endl;
		gmms.insert(make_pair(gmmNo, gmm));
		gmmNo++ ;
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~Polling and resegmentation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void segmentationAndPolling(map<int, Ptr<EM> > &gmms){
	vc testFea ;
	double ll, max_ll;
	int gmmId ;
	Mat prob ;
	int bestGmm = -1;
	//cout << segmentData.size() << endl;
	fr(i, 0, segmentData.size()){
		vi pollResults(initCluster, 0);
		fr(j, 0, segmentData[i].size()){
			testFea = segmentData[i][j];
			max_ll = INT_MIN ;
			bestGmm = -1;
			tr(gmms, it){
				ll = it->second -> predict2(testFea, prob)[0] ;
				if(ll > max_ll){
					max_ll = ll ;
					bestGmm = it -> first;
				}
			}
			pollResults[bestGmm]++;
		}
		gmmIds[i] = max_element(pollResults.begin(), pollResults.end()) - 
		pollResults.begin();
	}
	updateMap(gmms, gmmIds);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~Get unique gmm ids that are present at current iteration~~~~~~~~~~*/

vi getUniqueIds(map<int, Ptr<EM>> &gmms){
	vi uniqueSet;
	tr(gmms, it){
		if(find(uniqueSet.begin(), uniqueSet.end(), it->first) == 
		uniqueSet.end()){
			uniqueSet.push_back(it->first);
		}
	}
	return uniqueSet;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~update gmms by training after resegmenting as data changes for each gmm~~~*/

void makeGMMs(map<int, Ptr<EM>> &gmms, vvvc &segmentData, vi &clusterMixs){
	vi uniqueSet = getUniqueIds(gmms);
	double llh;
	tr(uniqueSet, it){
		Mat data = makeData(*it, segmentData, gmmIds);
		Ptr<EM> gmm = oneGMM(data, llh, clusterMixs[*it]);
		gmms[*it] = gmm;
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~Update data structures after merging two clusters~~~~~~~~~~~~~~~~~*/
void updateDts(int s1, int s2, map<int, Ptr<EM>> &gmms, vi &gmmIds, 
				vi &clusterMixs){
	fr(i, 0, gmmIds.size()){
		if(gmmIds[i] == s2){
			gmmIds[i] = s1;
		}
	}
	clusterMixs[s1] += clusterMixs[s2];
	updateMap(gmms, gmmIds);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~Agglomerative clustering~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void aggloClust(vvvc &clusterData, map<int, Ptr<EM>> &gmms, vi &gmmIds,
				 vi &clusterMixs){
	double llh1, llh2, llh3;
	vi ids = getUniqueIds(gmms);
	printv(ids, int);
	map<double, pair<int, int>> likelihoods;
	cout << "Comparing all Clusters\n";
	fr(ii, 0, ids.size()){
		fr(jj, ii+1, ids.size()){
		int i = ids[ii], j = ids[jj];
		//cout << i << " " << j << ": ";
			Mat probs;
			Mat data1 = makeData(i, clusterData, gmmIds);
			Mat data2 = makeData(j, clusterData, gmmIds);
			llh1 = computeLLH(gmms[i], data1);
			llh2 = computeLLH(gmms[j], data2);
			Mat data;
			vconcat(data1, data2, data);
			Ptr<EM> gmm = oneGMM(data, llh3, clusterMixtures[i] +
							clusterMixtures[j]);

			llh3 = computeLLH(gmm, data);
			if(llh3-max(llh1,llh2) > 0) cout << "+";
			else cout << "-";
			/*cout << llh1 << " " << llh2 << " " << llh3 << " " << 
				llh3-llh2-llh1 << endl; */
			//cout << llh1 << " " << llh2 << " " << llh3 << " " << 
				//llh3-max(llh1, llh2) << endl; 
			/*likelihoods.insert(pair<double, pair<int, int>>(llh3-llh2-llh1,
				 pair<int, int>(i, j)));*/
			/* For BIC calculation the likelihoods are all normalized, and for
				and for comparison between original two gmm and new gmm built,
				max of normalized likelihoods of both going to be merged gmms is
				used */
			likelihoods.insert(pair<double, pair<int, int>>(llh3-max(llh1, llh2)
				, pair<int, int>(i, j)));
			
		}
	}
	auto best = likelihoods.end();
	best--;
	if(best -> first < 0){
		stop = true;
		return;
	}
	int s1 = best->second.first;
	int s2 = best->second.second;
	cout << "\nClusters getting merged:\n";
	cout << s1 << " " << s2 << endl;
	updateDts(s1, s2, gmms, gmmIds, clusterMixs);
	//Mat data = makeData(s1, clusterData, gmmIds);
	//double llh4;
	//Ptr<EM> gmm = oneGMM(data, llh4, clusterMixtures[s1]);
	//gmms[s1] = gmm;
	cout << "Modified GMMIds after merging\n";
	printv(gmmIds, int);
	cout << "New number of cluster Mixtures\n";
	printv(clusterMixs, int);
	

}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~This segment collect the speaker data corresponding to speaker given in
~~~~~~~~rttm file and make same number of speakers and same corresponding data 
~~~~~~~~as in rttm file. This is just used for testing purpose that on the best 
~~~~~~~~result that could be obtained, what is the SER.~~~~~~~~~~~~~~~~~~~~~~~*/

map<int, Ptr<EM>> doSpeakerThing(string sdatapath, vvc &speechData){
	ifstream sfile(sdatapath);
	cout << sdatapath << endl;
	string s;
	stringstream ss;
	int sid, sf, ef;
	map<int, vvc> sdata;
	while(getline(sfile, s)){
		cs(ss);
		ss.str(s);
		ss >> sid >> sf >> ef;
		fr(i, sf, ef){
			sdata[sid].push_back(speechData[i]);
		}
	}
	//`printv(sdata[0][0], double);
	cout << "xadf" << endl;
	map<int, Ptr<EM>> sgmms;
	double llh;
	vi mvec = {15, 5, 15, 40, 10, 10, 10, 15, 15};
	int  i = 0;
	tr(sdata, it){
		cout << it->first << " " << sdata[it->first].size() << endl;
		Mat data = d2vecToMat(sdata[it->first]);
		Ptr<EM> gmm = oneGMM(data, llh, 5);
		sgmms.insert(pair<int, Ptr<EM>>(it->first, gmm));
	}
	return sgmms;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~Assign every feature a gmmId for feature	-	wise polling~~~~~~~~~~*/

	void makeFeatureId(vi &featId){
		int l;
		for(int i = 0; i < segmentData.size()-1; i++){
			l = 0;
			while(l < chunkSize){
				featId.push_back(gmmIds[i]);
				l++;
			}
		}
		/* for the last segment as it may have more than chunk size */
		int rem = segmentData[segmentData.size()-1].size();
		while(rem-->0){
			featId.push_back(gmmIds[gmmIds.size()-1]);
		}
		cout << featId.size() << endl;
		
	}
	

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~Making the rttm file from the given scp file~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~and mapping file of one to one correspondence between speech~~~~~~~~~~~
~~~~~~~~~region and actual file(with speech/non-speech region)~~~~~~~~~~~~~~~~~~
~~~~~~~~~NOT USED IN OBTAINING RESULT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	void makeNewRttm(string mapFile, string scpFile, string fileName, 
						string meeting, map<int, Ptr<EM>> &gmms){
		cout << mapFile << " " << scpFile << " " << fileName << " " << meeting 
				<< endl;
		int num = speechData.size();
		vi featId;
		makeFeatureId(featId);
		ifstream infile((mapFile).c_str());
		map<int, int> speechMap;
		string s;
		stringstream ss;
		int sline, mline ;
		getline(infile, s);
		while(getline(infile, s)){
			//cout << "myline " << s <<endl;
			ss << s;
			ss >> sline >> mline ;
			//cout << mline <<" "<< sline << endl;
			speechMap.insert(pair<int, int>(sline, mline));
			cs(ss);
		}
		cs(ss);	
		string sul;
		int start, end;
		infile.close();
		infile.open(scpFile);
		ofstream outfile(("../result/"+meeting+".rttm").c_str());
		
		tr(gmms, it){
			outfile << "SPKR-INFO " << meeting << " 1 <NA> <NA> <NA> adult_male "
			<< it -> first<< " <NA>" << endl;
		}
		while(getline(infile, s)){
			ss << s;
			ss >> start >> end;
			int a = speechMap[start];
			int b = speechMap[end]; 
			cs(ss);
			//cout << featId.size() << " " << a << " " << b << endl;
			vi pollFea(gmmIds.size(), 0);
			for(int i = a; i <= b; i++){
				pollFea[featId[i-1]]++;
			}
			int pos = max_element(pollFea.begin(), pollFea.end()) - 
						pollFea.begin(); 
			outfile<< setprecision(10) <<"SPEAKER " << meeting << " 1 " 
					<< start/100.0 << " " <<(end-start+1)/100.0 << " <NA> <NA> " 
					<< pos << " <NA>"<<endl;
			pollFea.clear();
 		}
		infile.close();
		outfile.close();

	}


/*~~~~~~~~Here, rttm is made from mapping file and fixed duration~~~~~~~~~~~~~~~
~~~~~~~~of chunksize/100 is used except last one. Used in obtaining~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~ the results.~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	void makeResultFileRTTM(string mapFile, string meeting, 
							map<int, Ptr<EM> > &gmms, vi gmmIds){
		ifstream infile(mapFile);
		ofstream outfile(("../result/"+meeting+".segrttm").c_str());
		ofstream ibscp(("../result/"+meeting+".ibscp").c_str());
		tr(gmms, it){
			outfile << "SPKR-INFO " << meeting << " 1 <NA> <NA> <NA> adult_male "
			<< it -> first<< " <NA>" << endl;
		}
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
		int totalSeg = segmentData.size();
		int readSeg = 0;
		int readS = 1;
		auto start = mapping.begin();
		auto it = mapping.begin();
		auto prevIt = it;
		while(it -> second < speechData.size()){
			count = 1;
			start = it ;
			while(count<= chunkSize && it -> second <= speechData.size()){
				prevIt = it;
				it++ ;
				if(it == mapping.end()) break;
				if((prevIt -> first)+1 != it -> first ){
					/*discontinuos case*/
					outfile <<setprecision(10) <<"SPEAKER " << meeting << " 1 " << (start-> first)/100.0 << " " << ((prevIt -> second) - 
							(start-> second) +1)/100.0<<" <NA> <NA> "  << gmmIds[readSeg] << " <NA>" << endl;
					ibscp << gmmIds[readSeg] << "[" << (start->first) << "," << (prevIt->first) << "]" << endl;
					start = it;
				}
				count++;
			}
			outfile << setprecision(10) <<"SPEAKER " << meeting << " 1 " << (start -> first)/100.0 << " " << ((prevIt-> second) -
						 (start-> second) +1)/100.0 <<" <NA> <NA> "  << gmmIds[readSeg] << " <NA>" << endl ;
			ibscp << gmmIds[readSeg] << "[" << (start->first) << "," << (prevIt->first) << "]" << endl;
			if(it == mapping.end()) break;
			readSeg++;
		}
		outfile.close();
		ibscp.close();
		
	}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	void makeFeatureRttm(string mapFile, string meeting, 
							map<int, Ptr<EM> > &gmms){
		vi featId(speechData.size(), -1);
		int l = 0;
		vc fea;
		int bestGmm;
		double ll;
		double maxll;
		tr(speechData, it){
			fea.clear();
			fea = *it;
			maxll = INT_MIN;
			tr(gmms, it1){
				ll = it1->second -> predict2(fea, noArray())[0] ;
				if(ll > maxll){
					maxll = ll;
					bestGmm = it1 -> first;
				}
			}
			featId[l] = bestGmm;
			l++;
		}
		ifstream infile(mapFile);
		ofstream outfile(("../result/"+meeting+".fearttm").c_str());
		tr(gmms, it){
			outfile << "SPKR-INFO " << meeting << " 1 <NA> <NA> <NA> adult_male "
			<< it -> first<< " <NA>" << endl;
		}
		string s;
		stringstream ss;
		int sline = -1, srline = -1;
		//map<int, int> mapping;
		getline(infile, s);
		int readLine = 0;
		while(getline(infile, s)){
			cs(ss);
			ss << s;	
			ss >> sline >> srline;
			//mapping.insert(make_pair(sline, srline));
			outfile << setprecision(10) <<"SPEAKER " << meeting << " 1 " << (sline)/100.0 << " " << "0.01" <<" <NA> <NA> "  << featId[readLine] << " <NA>" << endl ;
			readLine++;
		
		}
	}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~Indicates how many features/segments each gmm is acquiring~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~Mainly used after pollling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	void gmmStrength(vi &gmmIds, map<int, Ptr<EM>> &gmms){
		map<int, int> strength;
		tr(gmmIds, it){
			strength[*it]++;
		}
		tr(strength, it){
			cout << it->second << " ";
		}
		cout << endl;
	}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~Write Proto File to use for viterbi realignment~~~~~~~~~~~~~~~~~~~*/
void writeProtoFile(map<int, Ptr<EM>> &gmms, string protoFileName, 
						string macroFile, vi clusterMixs){
        ofstream protoFile(protoFileName);
        protoFile << "~o\n<STREAMINFO> 1 " << featDim << "<VECSIZE> " << featDim
					 << "\n<NULLD><USER><DIAGC>\n ~h \"proto\"\n<BEGINHMM>\n<NUMSTATES> "
					 << (gmms.size()+2) << "\n"; 
        int stateCount = 2;
		/*cout << "a" << endl;*/
        tr(gmms, it){
                Mat weights = it->second->getWeights();
                Mat means = it->second->getMeans();
                vector<Mat> covs;
                it->second->getCovs(covs);
		/*cout << covs[0] << endl; */
                protoFile << "<STATE> " << (stateCount++) << "\n<NUMMIXES> " << 
							clusterMixs[it->first] << "\n";
		/*cout << "b" << endl; */
		/*cout << clusterMixs[it->first] << " " << means.size().height << endl;
		*/
			fr(i, 0, clusterMixs[it->first]){
				protoFile << "<MIXTURE> " << (i+1) << " " 
						<< weights.at<double>(i) << "\n<MEAN> " << 
						featDim << "\n";
				fr(j, 0, featDim){
					protoFile << means.at<double>(i, j) << " ";
				}
			
				/*cout << "c" << endl; */
				protoFile << "\n<VARIANCE> " << featDim << "\n";
				long double gconst = 1;
				fr(j, 0, featDim){
					protoFile << covs[i].at<double>(j, j) << " ";
					gconst = gconst * covs[i].at<double>(j, j);
				}
			
				/*cout << "d" << endl; */
				protoFile << "\n<GCONST> 7.386e+1" << "\n";
			}
        } 

	/*cout << "e" << endl; */
	protoFile << "<TRANSP> " << (gmms.size()+2) << "\n";
	double prob = 1/(gmms.size()+1.0);
	fr(i, 0, gmms.size()+1){
		protoFile << "0 ";
		fr(j, 0, gmms.size()+1){
			protoFile << prob << " ";
		}
		protoFile << "\n";
	}
	/*cout << "f" << endl;	*/
	fr(j, 0, gmms.size()+2) protoFile << "0 ";
	protoFile << "\n<ENDHMM>";
	protoFile.close();

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~Creating macros file-----------------------------*/
	
        cout << "Creatring macros file..";
		ofstream outfile;
		outfile.open(macroFile);
        ifstream infile(protoFileName);
        int lines3 = 3;
        string s;
		while(getline(infile, s) && lines3> 0){
                outfile<<s<<"\n";
                lines3--;
        }
        infile.close();
        outfile.close();
        cout << "Completed." << endl;

}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



/*~~~~~~~~~~~~~~~~~~~~~~~~~~Main function~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int main(int argc, char* argv[]){

/*~~~~~~~~~~~~~~~~~~~Command line arguments~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	
	featDim 					=		ston(argv[1]);	//Dimensionality of the feature vectors
	initCluster 					=		ston(argv[2]);	//Initial No.of clusters
	initMixture					=		ston(argv[3]);	//Initial No.of mixtures
	int segmentationItrOneTime			=		ston(argv[4]);	//Initial segmentation and polling iters
	segmentationItr					=		ston(argv[5]);	//iters fr segmentataion and polling while clustering
	string meeting 					=		argv[6];	//meeting file name
	fileName 					=		argv[7];	//.speech file where features are present
	string scpFile					=		argv[8]; 	//.nonoverscp file where speech regions are mentioned
	string mapFile					=		argv[9];	//.map file where mappings for speech regions are mentioned
	sdatapath					=		argv[10];	//.speakerData file that contains exact speaker timings from rttm for checking 
	chunkSize					=		ston(argv[11]);
//print congifuration of the system
cout << "-----------------------";
cout << "\nNo. of states: " << initCluster;
cout << "\nNo.of initial mixtures: " << initMixture;
cout << "\nIterations: " << segmentationItrOneTime << ", " << segmentationItr;
cout << "\nFilename: " << meeting;
cout << "\nChunk Size: " << chunkSize;
cout << "\n----------------------" << endl << endl << endl;
/*~~~~~~~~~~~~~~~~~~~~~~~~~~Uniform flat start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	readData(speechPath, speechData, fileName);
	initClusters(initCluster, clusterData);
	map<int, Ptr<EM> > gmms;
	vc likelihoods ;
	initClusterMixtures(initCluster);

	gmmTrain(gmms, clusterData);
	clusterData.shrink_to_fit();

/*~~~~~~~~~~~~~~~~~~~~~~~~Preparing initial Segments~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	int sizeForSeg = speechData.size()/chunkSize;
	initSegments(segmentData, chunkSize);

/*~~~~~~~~~~~~~~~~~~~ Initializing gmmId for each segment~~~~~~~~~~~~~~~~~~~~~*/
	initGMMId(gmms);

/*~~~~~This is testing segmentation by taking speaker data and no of speakers~~~
~~~~~~from rttm itself. Just for testing. Not to be used for actual eval~~~~~~*/


	/*map<int, Ptr<EM>> sgmms = doSpeakerThing(sdatapath, speechData);
	fr(i, 0, 1){
		segmentationAndPolling(sgmms);
		//makeGMMs(sgmms, segmentData, clusterMixtures);
		printv(gmmIds, int);
	}*/
	

/*~~~~~~~~~~~~~~~~~~~Initial segmentation and Pollling~~~~~~~~~~~~~~~~~~~~~~~~*/
	cout << "Initial Segmentation and Polling\n";
	fr(i, 0, segmentationItrOneTime){
			cout << "Iteration " << i << endl;
			segmentationAndPolling(gmms);
			gmmStrength(gmmIds, gmms);
			makeGMMs(gmms, segmentData, clusterMixtures);
			//cout << "GMMs after polling: ";
			//tr(gmms, it)
				//cout << it->first << " "; 
			//cout << endl;
	}
/*~~~~~~~~~~~~~~~Segmentation and clustering iterations~~~~~~~~~~~~~~~~~~~~~~~*/
	int br = 0;
	while(true){
		fr(i, 0, segmentationItr){
			cout << "Iteration " << i << endl;
			segmentationAndPolling(gmms);
			gmmStrength(gmmIds, gmms);
			makeGMMs(gmms, segmentData, clusterMixtures);
			//cout << "GMMs after polling: ";
			//tr(gmms, it)
				//cout << it->first << " "; 
			//cout << endl;
		}
		cout << endl;
		cout << "Final gmmIds :\n";
		//gmmStrength(gmmIds, gmms);
		aggloClust(segmentData, gmms, gmmIds, clusterMixtures);
		//makeGMMs(gmms, segmentData, clusterMixtures);
		if(stop == true)
			break;
	}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~Make rttm file~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	makeNewRttm(mapFile, scpFile, fileName, meeting, gmms);
	makeResultFileRTTM(mapFile, meeting, gmms, gmmIds);
	makeFeatureRttm(mapFile, meeting, gmms);
/*~~~~~~~~~~~~~~~~~~~~~~~~~Viterbi Realignment process~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~Poll frame by frame wise ~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//writeProtoFile(gmms, protoFile, macroFile, clusterMixtures);
}
