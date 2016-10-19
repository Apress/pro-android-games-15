#ifndef DATASET_H
#define DATASET_H

#include "training.h"

/**
 * Class Dataset for manage data from wiimote for a neural net application
 */
class Dataset
{
public:
	Dataset() {}
	Dataset(const string& filename) { loadDataset(filename); }
	~Dataset();

	/** 
	 * Number of trainings in the current dataset 
	 */
	inline int size() const { return trainings.size(); }

	inline const Training* trainingAt(unsigned int i) const { 
		if(i < trainings.size())
			return trainings[i]; 
		else {
			cout << "[Error]: requested out of array bound index in dataset." << endl;
			return 0;
		}
	}

	inline Training* trainingAt(unsigned int i) { 
		if(i < trainings.size())
			return trainings[i]; 
		else {
			cout << "[Error]: requested out of array bound index in dataset." << endl;
			return 0;
		}
	}

	/** 
	 * Add a new training to the dataset 
	 * 
	 * @param training to add to the dataset
	 */
	void addTraining(Training*);

	/**
	 * Load training values from a file
	 *
	 * @param dataset filename
	 */
	void loadDataset(const string& nomefile);
	
	/**
	 * Save training onto dataset
	 * 
	 * @param input file stream
	 * @param training
	 */
	void getDataTraining(ifstream& infile, Training* training);
	
	/**
	 * Save the dataset into a file for training and recognition
	 *
	 * @param file name
	 */
	bool save(const char* file) const;

	/**
	 * Delete all trainings and clear the buffer
	 */
	void clear() ;


private:
	vector<Training*> trainings;
};

#endif
