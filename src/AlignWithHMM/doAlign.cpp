/*~~~~~~~~~~~~~~~~~~~~~~~~~~~AUTHOR SAKSHI VERMA~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~LAST UPDATED 28 JULY 2016~~~~~~~~~~~~~~~~~~~~~*/
#include "Includes.h"
int main(int argc, char* argv[]){
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~Running HRest-----------------------------*/
	int hRestItr = ston(argv[1]);
	string path = argv[2];
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int folderNum = 1;
	/*cout << "Running HRest.." << endl;
		system(("mkdir "+ path).c_str());
		system(("mkdir "+path+"hmm0").c_str());
		system(("cp proto "+path+"hmm0").c_str());
		system(("cp macros "+path+"hmm0").c_str());

	for(int i =0 ; i< hRestItr; i++){
		cout << "\t Iteration: " << i << endl;
		folderNum++;
		system(("mkdir "+path+"hmm"+ntos(folderNum)).c_str());
		cout << "created folder hmm" << folderNum << ".. "<< endl;
		system(("HRest -m 1 -i 1 -v 0.0000001 -C config -S filelist.scp -H "+path+"hmm"+ntos(folderNum-1)+"/macros -M "+path+"hmm"+ntos(folderNum)+" "+path+"hmm"+ntos(folderNum-1)+"/proto").c_str());
	}
	cout << "Completed." << endl;
*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~Running HVite~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	cout << "Running HVite..";
	system(("HVite -f -a -C config -H macros -H proto -S filelist.scp -l '*' -i recout.mlf -w wordnet -p 0.0 -s 5.0 dict labels").c_str());
	cout << "Completed." << endl;

}


//removed from HVite command: 
