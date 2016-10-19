#include <wiicpp.h>
#include <MLAlg.h>

void recognize(int argc, char** argv, MLAlg& mlalg)
{
    CWii wii; // Defaults to 4 remotes
    std::vector<CWiimote>::iterator i;
    int numFound;
	Training* training = 0;

	// Load the model
	mlalg.load(argv[3]);
	
	// Load category names, if any
	if(argc > 4) {
		ifstream in(argv[4]);
		vector<string> categories;
		
		string tmp;
		while(in.good()) {
			in >> tmp;
			categories.push_back(tmp);
		}
		in.close();
    	mlalg.setCategoryNames(categories);
	}
	
    vector<int> mask;
    for(int i = 1 ; i <= 8 ; i++) 
	    mask.push_back(i);
	
    cout << "Searching for wiimotes... Turn them on!" << endl;

    //Find the wiimote
    numFound = wii.Find(1);

    // Search for up to five seconds;

    cout << "Found " << numFound << " wiimotes" << endl;
    cout << "Connecting to wiimotes..." << endl;

    // Connect to the wiimote
    std::vector<CWiimote>& wiimotes = wii.Connect();

	cout << "Connected to " << (unsigned int)wiimotes.size() << " wiimotes" << endl;

	if(wiimotes.size() == 0) {
		cout << "Error: no connected Wiimote" << endl;
		return;
	}
	
    // Use a reference to make working with the iterator handy.
    CWiimote & wiimote = wiimotes[0];

    //Rumble for 0.2 seconds as a connection ack
    wiimote.SetRumbleMode(CWiimote::ON);
    usleep(200000);
    wiimote.SetRumbleMode(CWiimote::OFF);
	wiimote.SetMotionSensingMode(CWiimote::ON);	
	
	int enbAcq = 2;

	while(enbAcq) {
		wii.Poll();
		
		//Inizio acquisizione dati
		while(enbAcq && wiimote.Buttons.isPressed(CButtons::BUTTON_PLUS)) {
			float x, y, z;
			wiimote.Accelerometer.GetGravityVector(x,y,z);
			training->addSample(new AccSample(x,y,z));
		  	enbAcq = 2;		
		  	usleep(10000);
			wii.Poll();	
		}
		
		if(enbAcq == 2) {
			enbAcq = 1;		
			cout << "  - Press PLUS button to acquire the training" << endl;
			cout << "  - Release PLUS button to end the training acquisition" << endl;
			MLData newData(mask);
			if(training) {
				newData.loadTraining(training);
				//newData.normalize();
				CvMat *gestureIn = newData.getInputData();
				CvMat *gestureOut = newData.getOutputData();
				mlalg.recognize(gestureIn, gestureOut);
				delete training;
			}
			training = new Training(WiiC::LOG_ACC);
		}
		
		if((wiimote.GetEvent() == CWiimote::EVENT_DISCONNECT) ||					
		(wiimote.GetEvent() == CWiimote::EVENT_UNEXPECTED_DISCONNECT))
				enbAcq = 0;
	}
	
	if(training) delete training;
	training = 0;
}

void train(int argc, char** argv, MLAlg& mlalg, MLData& mldata)
{
    vector<string> vf;
	for(int i = 4 ; i < argc ; i++) 
    	vf.push_back(string(argv[i]));

    if(!mldata.open(vf)) {
		cout << "Unable to parse input files" << endl;
		return;
    }

    //mldata.normalize();
    float tr = 0.75; if (argc>3) tr = atof(argv[3]);
    mldata.generateTrainingAndValidationData(tr);
    CvMat *trainIn = mldata.getTrainingInputData();
    CvMat *trainOut = mldata.getTrainingOutputData();
    CvMat *validateIn = mldata.getValidationInputData();
    CvMat *validateOut = mldata.getValidationOutputData();
    
    cout << "===================================" << endl;
    mlalg.train( trainIn, trainOut );
    
    cout << "===================================" << endl;
    mlalg.validate(validateIn,validateOut);

	string output = string(argv[2]) + string("_model.xml");
	mlalg.save(output.c_str());
	cout << "Trained model saved in " << output << endl;
}

void validation(int argc, char** argv, MLAlg& mlalg, MLData& mldata)
{
	// Load the model
	mlalg.load(argv[3]);
	
	// Load category names, if any
	if(argc > 5) {
		ifstream in("categories");
		vector<string> categories;
		
		string tmp;
		while(in.good()) {
			in >> tmp;
			categories.push_back(tmp);
		}
		in.close();
    	mlalg.setCategoryNames(categories);
	}	
	
    vector<string> vf;
	for(int i = 4 ; i < argc ; i++) 
    	vf.push_back(string(argv[i]));

    if(!mldata.open(vf)) {
		cout << "Unable to parse input files" << endl;
		return;
    }

    //mldata.normalize();
    float tr = 0.0;
    mldata.generateTrainingAndValidationData(tr);
    CvMat *validateIn = mldata.getValidationInputData();
    CvMat *validateOut = mldata.getValidationOutputData();
    
    cout << "===================================" << endl;
    mlalg.validate(validateIn,validateOut);
}

void convert(int argc, char** argv, MLAlg& mlalg)
{	
    vector<int> mask;
    for(int i = 1 ; i <= 8 ; i++) 
	    mask.push_back(i);
	
    vector<string> vf;
	for(int i = 2 ; i < argc ; i++) 
    	vf.push_back(string(argv[i]));

	for(int i = 0 ; i < vf.size() ; i++) {
		Dataset dataset(vf[i]);
		string outputFile = vf[i] + "_features" ;
		for(int j = 0 ; j < dataset.size() ; j++) {
			Training* t = dataset.trainingAt(j);
			MLData newData(mask);
			if(t) {
				newData.loadTraining(t);
				newData.save(outputFile,true);
			}
		}
		cout << vf[i] << " saved in " << outputFile << endl;
	}
}

int main(int argc, char **argv) {

    if (argc<3) {
		cout << "wiic-ml allows to recognize a set of gestures using OpenCV machine learning library." << endl;
		cout << "wiic-ml is part of WiiC (http://wiic.sf.net)" << endl << endl;
		cout << "Usage:  " << argv[0] << " train <KNN|Bayes|SVN|DT|ANN|Boost|RT> <training_data_ratio> <data_file1> ... <data_fileN>" << endl;
		cout << "        " << argv[0] << " val <KNN|Bayes|SVN|DT|ANN|Boost|RT> <model_file> <val_file1> ... <val_fileN>" << endl;
		cout << "        " << argv[0] << " rec <KNN|Bayes|SVN|DT|ANN|Boost|RT> <model_file> [category_names]" << endl;
		cout << "        " << argv[0] << " convert <data_file1> ... <data_fileN>" << endl;
		cout << "e.g. " << argv[0] << " train KNN 0.75 file1.log file2.log" << endl;
		cout << "e.g. " << argv[0] << " val KNN KNN_model.xml file1.log file2.log" << endl;
		cout << "e.g. " << argv[0] << " rec KNN KNN_model.xml [category_names.txt]" << endl;
		cout << "e.g. " << argv[0] << " convert file1.log file2.log" << endl;
		cout << "======= Supported Algorithms =======" << endl;
		cout << "KNN: k-nearest neighbor" << endl;
		cout << "Bayes: normal Bayes classifier" << endl;
		cout << "SVN: support vector machines" << endl;
		cout << "DT: decision trees" << endl;
		cout << "ANN: artificial neural networks" << endl;
		cout << "Boost: two-class discrete AdaBoost" << endl;
		cout << "RT: random trees" << endl;
		return 1;
    }
    
    vector<int> mask;
    for(int i = 1 ; i <= 8 ; i++) 
	    mask.push_back(i);

    MLData mldata(mask);
    MLAlg mlalg;
    mlalg.setType(string(argv[2]));
    
	// Mode parsing
	string cmd = string(argv[1]);
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
	if(cmd == string("rec")) {
		recognize(argc, argv, mlalg);
	}
	else if(cmd == "train") {
		train(argc, argv, mlalg, mldata);
	}
	else if(cmd == "val") {
		validation(argc, argv, mlalg, mldata);
	}
	else if(cmd == "convert") {
		convert(argc, argv, mlalg);
	}
	else {
		cout << "Error - Unsupported option" << endl;
	}
		    
    return 0;
}
