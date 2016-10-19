#ifndef TRAINING_H
#define TRAINING_H

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include "accsample.h"

namespace WiiC {
	enum LogType {
		LOG_NONE,
		LOG_ACC
	};
}

using namespace std;
using namespace WiiC;

/**
 * Class Training to save the individual training of a gesture  
 */
class Training
{
public:
	/**
	 * Constructor void
	 */
	Training() { logType = LOG_ACC; }
	Training(LogType l) { logType = l; }
	~Training();

	void loadTraining(ifstream&);
	void save(ofstream&) const;
	void addSample(Sample*);
	void clear();

	inline const Sample* sampleAt(unsigned int i) const { 
		if(i < samples.size())
			return samples[i]; 
		else {
			cout << "[Error]: requested out of array bound index in training." << endl;
			return 0;
		}
	}

	inline Sample* sampleAt(unsigned int i) { 
		if(i < samples.size())
			return samples[i]; 
		else {
			cout << "[Error]: requested out of array bound index in training." << endl;
			return 0;
		}
	}
	
	inline void setLogType(LogType l) { logType = l; }
	inline LogType getLogType() const { return logType; }

	inline unsigned int size() const { return samples.size(); }
	
protected:
	void loadAccTraining(ifstream& infile);

private:
	vector<Sample*> samples;
	LogType logType;
};

#endif
