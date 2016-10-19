/*
 *	logger.cpp
 *
 *	This file is part of WiiC, written by:
 *		Gabriele Randelli
 *		Email: randelli@dis.uniroma1.it
 *
 *	Copyright 2010
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <fstream>
#include <unistd.h>
#include <wiicpp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

void acquireGesture(CWii& wii, CWiimote& wiimote, Training* training)
{
	cout << "  - Press PLUS button to acquire the training" << endl;
	cout << "  - Release PLUS button to end the training acquisition" << endl;
	int enbAcq = 1;

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
			enbAcq = 0;		
		}
	}
}

void randomAcquisition(CWii& wii, CWiimote& wiimote, const int samples, vector<string>& files)
{
	// Total iterations
	int totSamples = samples*files.size();
	srand(time(0));
	cout << "=======================================" << endl;
	cout << "You are going to acquire " << samples << endl;
	cout << "samples per " << files.size() << " files "
		 << " = " << totSamples << " total samples" << endl << endl;
	
	// Open output streams
	vector<ofstream*> outs;
	for(int i = 0 ; i < files.size() ; i++) 
		outs.push_back(new ofstream(files[i].c_str(), ios::app));		
		
	// Number of samples per file
	vector<int> fileSamples;
	for(int i = 0 ; i < files.size() ; i++)
		fileSamples.push_back(samples);
	
	for(int i = 0 ; i < totSamples ; i++) {
		// Random generation
		int selection = rand() % files.size() ;

		// Gesture acquisition
		cout << "Selected gesture: " << files[selection] << endl;
		Training* training = new Training(WiiC::LOG_ACC);
		acquireGesture(wii, wiimote, training);
		cout << "Training " << (i+1) << " acquired" << endl;
		
		// Gesture save
		training->save(*(outs[selection]));
		if(training) {
			delete training;
			training = 0;
		}
		
		// Sample decrement and check whether to close the file
		if(!(--fileSamples[selection])) {
			outs[selection]->close(); // Stream close
			if(outs[selection])
				delete outs[selection]; // Stream deallocation
			outs.erase(outs.begin() + selection); // Stream erase
			files.erase(files.begin() + selection); // Filename erase
			fileSamples.erase(fileSamples.begin() + selection); // File sample erase
		}
	}	
	
	cout << "=======================================" << endl;
	cout << "That's it! Bye" << endl;
}

void dataAcquisition(CWii& wii, CWiimote& wiimote, const string logfile)
{
	int enbAcq = 0;
	int count = 0;
	
	string fileacc;
	
	Dataset* dataset = new Dataset();
	Training* training = new Training(WiiC::LOG_ACC);

	cout << "Trainings will be stored in " << logfile << endl;
	cout << "  - Press PLUS button to acquire the training" << endl;
	cout << "  - Release PLUS button to end the training acquisition" << endl;
	cout << "  - Press HOME button to go back to the main menu" << endl;
	enbAcq = 1;

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
			dataset->addTraining(training);
			count++;
			cout << "Training " << count << " acquired" << endl;
			cout << "  - Press PLUS button to acquire the training" << endl;
			cout << "  - Release PLUS button to end the training acquisition" << endl;
			cout << "  - Press HOME button to go back to the main menu" << endl;
			enbAcq = 1;		
			training = new Training(WiiC::LOG_ACC);
		}

		if(enbAcq == 1 && wiimote.Buttons.isJustPressed(CButtons::BUTTON_HOME)) {
			dataset->save(logfile.c_str());
			enbAcq = 0;
		}
	}
	
	if(training)
		delete training;
	training = 0;

	if(dataset)
		delete dataset;
	dataset = 0;		
}


int main(int argc, char** argv)
{
    CWii wii; // Defaults to 4 remotes
    std::vector<CWiimote>::iterator i;
    int reloadWiimotes = 0;
    int numFound;
    int index;
    bool help = false;

	string option; 
	if(argc == 2) {
		option = string(argv[1]);
		if(option == "help") {
			cout << endl << "wiic-logger is a utility to collect accelerometer data in a log." << endl;
			cout << "wiic-logger is part of WiiC (http://wiic.sf.net)" << endl << endl;
			cout << "Usage: wiic-logger <log_filename>" << endl;
			cout << "       wiic-logger RANDOM <num_samples> <filename1> ... <filenameN>" << endl;
			cout << "   <log_filename>: full pathname of the output log file" << endl;
			cout << "   <num_samples>: number of samples for each file" << endl << endl;
			cout << "   <filename1> ... <filenameN>: full pathname of each gesture file" << endl << endl;
		
			return 1;
		}
	}
	else if(argc >= 3) {
		option = string(argv[1]);
		if(option != "RANDOM") {
			cout << endl << "Bad Syntax!" << endl;
			cout << "Type wiic-logger help for a brief documentation." << endl << endl;
		
			return 1;
		}
	}
	else {
		cout << endl << "Bad Syntax!" << endl;
		cout << "Type wiic-logger help for a brief documentation." << endl << endl;
		
		return 1;	
	}

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
		return 1;
	}
	
    // Use a reference to make working with the iterator handy.
    CWiimote & wiimote = wiimotes[0];

    //Rumble for 0.2 seconds as a connection ack
    wiimote.SetRumbleMode(CWiimote::ON);
    usleep(200000);
    wiimote.SetRumbleMode(CWiimote::OFF);
	wiimote.SetMotionSensingMode(CWiimote::ON);

	if(option == "RANDOM") {
		// Num of samples for each file
		int numSamples = atoi(argv[2]);
		
		// Load candidate files
		vector<string> gestures;
		int i = 3;
		while (i < argc) 
			gestures.push_back(string(argv[i++]));
		
		randomAcquisition(wii, wiimote, numSamples, gestures);
	}
	else
		dataAcquisition(wii, wiimote, option);

    return 0;
}
