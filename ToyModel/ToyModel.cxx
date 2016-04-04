/********************************************************************************************
* Filename:     ToyModel.C                                                                  *
* Date Created: January 25, 2016                                                            *
* Author:       Brandon McKinzie                                                            *
* Description:  Simple model event generator for multiple parton scattering.                *
*               Serves as introduction to toy models for me (Brandon) and                   *
*               two-particle correlation study.                                             *
********************************************************************************************/
#include <iostream>
#include <fstream>
#include "./include/RootClasses.h"
#include "./include/EventModel.h"
#include "./include/MyJetFinder.h"

/* Print function that outputs if debug flag = true. */
Bool_t debug = false;
TString debugStr;
void print(const char* str, Int_t i_event, Int_t nEvents) {
	Bool_t fivePercentInterval = (i_event % (nEvents / 20) == 0);
	if (debug && fivePercentInterval) {
		cout << str << endl;
	}
}

// ---------------------------------------------------------------------------------
void ToyModel(Int_t nEvents = 1000) {

    using std::cout;
    using std::flush;

    /* ------------------------------------------------ *
     * Object Declarations.            					*
     * ------------------------------------------------ */

    // Setup jetFinder.
    MyJetFinder* jetFinder = new MyJetFinder();
    // Create file for object output tests.
    std::ofstream f_debug("./debug/debug_ToyModel.txt");

    /* ------------------------------------------------ *
     * Data Generation/Simulation.                   	*
     * ------------------------------------------------ */

    cout << "Beginning ToyModel simulation of " << nEvents << " events." << endl;
    // Simulate nEvents with randomly generated tracks. 
    for (Int_t i_event = 0; i_event < nEvents; i_event++) {

    	// Print progress updates.
    	debugStr = "\n";
    	debugStr += (Float_t)(i_event * 100)/nEvents;
    	debugStr += " Percent Complete.";
    	print(debugStr.Data(), i_event, nEvents);

        // Generate specified number/types of particles.
        jetFinder->Generate("trig");
        jetFinder->Generate("bkg", nBkg);

        // Use ClusterSequence to get store list of jets in this event.
        jetFinder->FindJets();
        debugStr  = "Found ";
        debugStr += jetFinder->GetNumJets();
        debugStr += " jets in this event.";
        print(debugStr.Data(), i_event, nEvents);

        jetFinder->ResetEventVariables();
    }

    /* ------------------------------------------------ *
     * Save Simulation Data.							*
     * ------------------------------------------------ */

    // Create new ROOT file with generic EventModel objects, and
    // store desired jet histograms in subfolder "jetPlots".
    TString fileName = "./rootFiles/ToyModel_";
    fileName += nEvents;
    fileName += ".root";
    jetFinder->Write(fileName);
}

/* This main function is needed to compile via "make."
 * Runs ToyModel over argv[1] number of events, if specified,
 * else runs for 1000 events. */
#ifndef __CINT__
int main(int argc, char* argv[]) {
    if (argc == 2) {
        ToyModel(atoi(argv[1]));
    } else if (argc == 3 && strcmp(argv[2], "debug") == 0){
    	debug = true;
        ToyModel(atoi(argv[1]));
    } else {
        ToyModel();
    }
    return 0;
}
#endif

