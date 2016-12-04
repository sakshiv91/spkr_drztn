#include <Includes.h>

void gmmTrain(vector<Ptr<EM>> &gmms, vvvc &data, double &negLogLikelihood){
	int size = data.size();
	//how to know how many mixtures to train with?
	Mat clusterData;
	fr(i, 0, size){
		vecToMat(clusterData, data[i]);
		Ptr<EM> gmm = oneGMM(clusterData, negLogLikelihood, noOfMixs);
		gmms.push_back(gmm);
	}
}

Ptr<EM> oneGMM(Mat &data, double &llh, int nc){
	Mat labels, logLikelihoods;
	double l1;
	Ptr<EM> gmmmodel = cv::ml::EM::create();
	gmmmodel->setClustersNumber(nc);
    	gmmmodel->setCovarianceMatrixType(EM::COV_MAT_DIAGONAL);
	gmmmodel->setTermCriteria(TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 500, 0.01));
	gmmmodel->trainEM( data, logLikelihoods, labels, noArray());
	fr(i, 0, data.size().height){
		llh = llh + logLikelihoods.at<double>(i);
	}
	llh = llh / data.size().height; // normalizing the negloglikelihood value with size of the data
	return gmmmodelstate1;
}

