#include "dataset.h"

/**
 * Dataset methods
 */
Dataset::~Dataset()
{
	clear();
}

/**
 *  Add a new training to the dataset
 */
void Dataset::addTraining(Training* t)
{
	if(t)
		trainings.push_back(t);
}

/**
 * Load dataset from a file into a class Dataset
 */
void Dataset::loadDataset(const string& nomefile)
{
	int size; 
	string line;
	ifstream infile(nomefile.c_str());

	if(!infile.is_open()) {
		cout<<"[Error] Unable to open the dataset file"<<endl;
	}
	else {
		trainings.clear();
		getline(infile,line);
		istringstream iline(line);
		iline >> size;
		
		for(int i = 0 ; i < size ; i++) {
			// Each training is inserted in the training vector	
			trainings.push_back(new Training());	
			// Each training is filled with its relative values (polymorphism)
			trainings[i]->loadTraining(infile);
		}
	}
	infile.close();
}

void Dataset::clear()
{
	for(int i = 0 ; i < trainings.size() ; i++) {
		if(trainings[i]) {
			delete trainings[i];
			trainings[i] = 0;
		}
	}
	trainings.clear();
}

/**
 * Save data vector for recognition or acquisition into a file
 *
 * @param file name
 */
bool Dataset::save(const char* file) const
{
	// Open file
	ofstream out(file, ios::trunc);
	if(!out.is_open()) { // File does not exist, hence I create it
		cout << "[Error] Unable to open " << file << endl;
		return false;
	}
	
	out << size() << endl;	
	
	// For each training
	for(int i = 0 ; i < size() ; i++)	{
		const Training* training = trainingAt(i);
		if(training)
			training->save(out);
	}
	
	out.close();

	return true;
}


