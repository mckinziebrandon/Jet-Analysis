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
#include "./include/EventGenerator.h"
#include "./include/MyJetFinder.h"
//#include "./include/Printer.h"

TString debugStr;

// ---------------------------------------------------------------------------------
void ToyModel(Int_t nEvents = 1000) {

    using std::cout;
    using std::flush;

    /* ------------------------------------------------ *
     * Object Declarations.            					*
     * ------------------------------------------------ */

    // Setup jetFinder.
    EventGenerator* eventGenerator = new EventGenerator();
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
        debugStr = "";
    	debugStr += (Float_t)(i_event * 100)/nEvents;
    	debugStr += " Percent Complete.";
        Printer::print(debugStr.Data(), i_event, nEvents);

        // Defined event centrality (and thus multiplicity);
        eventGenerator->SetCentrality(2.5); 
        debugStr = "\tNumber of particles generated: ";
        debugStr += eventGenerator->GetMultiplicity(); 
        Printer::print(debugStr.Data(), i_event, nEvents);

        // Generate specified number/types of particles.
        //eventGenerator->Generate("trig"); 
        //eventGenerator->Generate("hJet"); // Pythia hard scattering
        eventGenerator->Generate("bkg", eventGenerator->GetMultiplicity()); 

        // Use ClusterSequence to get store list of jets in this event.
        jetFinder->FindJets(eventGenerator->GetLastEvent());
        debugStr  = "\tNumber of jets found: ";
        debugStr += jetFinder->GetNumJets();
        Printer::print(debugStr.Data(), i_event, nEvents);

        jetFinder->Clear();
    }

    /* ------------------------------------------------ *
     * Save Simulation Data.							*
     * ------------------------------------------------ */

    // Create new ROOT file with generic EventModel objects, and
    // store desired jet histograms in subfolder "jetPlots".
    TString fileName = "./rootFiles/ToyModel_";
    fileName += nEvents;
    fileName += ".root";
    cout << "Writing data to " << fileName.Data();
    eventGenerator->Write(fileName);
    jetFinder->Write(fileName);
}

/* This main function is needed to compile via "make."
 * Runs ToyModel over argv[1] number of events, if specified,
 * else runs for 1000 events. */
#ifndef __CINT__
int main(int argc, char* argv[]) {
    if (argc == 2) {
        Printer::debug = false;
        ToyModel(atoi(argv[1]));
    } else if (argc == 3 && strcmp(argv[2], "debug") == 0){
        Printer::debug = true;
        ToyModel(atoi(argv[1]));
    } else {
        Printer::debug = false;
        ToyModel();
    }
    return 0;
}
#endif

