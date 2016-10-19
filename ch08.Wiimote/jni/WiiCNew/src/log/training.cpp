#include "training.h"

Training::~Training()
{
	clear();
}

/***
 * Load data into a single training
 */
void Training::loadTraining(ifstream& infile)
{
	string line;
	getline(infile,line);
	if(line == "acc") {
		loadAccTraining(infile);
		logType = LOG_ACC;
	}
	else
		cout << "[Error]: bad log type." << endl;			
}

void Training::loadAccTraining(ifstream& infile)
{
	istringstream xIn, yIn, zIn;
	string xLine, yLine, zLine;
	float xAcc, yAcc, zAcc;
	
	if(getline(infile,xLine) && getline(infile,yLine) && getline(infile,zLine)) {
		xIn.str(xLine);
		yIn.str(yLine);
		zIn.str(zLine);
		
		while(xIn >> xAcc) {
			yIn >> yAcc;
			zIn >> zAcc;
			samples.push_back(new AccSample(xAcc,yAcc,zAcc));
		}
	}
}

void Training::save(ofstream& out) const
{	
	if(logType == LOG_ACC) {
		out << "acc" << endl;
		
		for(int i = 0 ; i < samples.size() ; i++) {
			AccSample* s = (AccSample*)samples[i];
			out << s->x() << " ";
		}
		out << endl;

		for(int i = 0 ; i < samples.size() ; i++) {
			AccSample* s = (AccSample*)samples[i];
			out << s->y() << " ";
		}
		out << endl;

		for(int i = 0 ; i < samples.size() ; i++) {
			AccSample* s = (AccSample*)samples[i];
			out << s->z() << " ";
		}
		out << endl;		
	}
}

void Training::addSample(Sample* s)
{
	if(s)
		samples.push_back(s);
}

void Training::clear()
{
	for(int i = 0 ; i < samples.size() ; i++) {
		if(samples[i]) {
			delete samples[i];
			samples[i] = 0;
		}
	}
	samples.clear();
}
