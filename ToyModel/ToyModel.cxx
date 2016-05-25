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
#include <typeinfo>
#include "./include/RootClasses.h"
#include "./include/EventGenerator.h"
#include "./include/EventFunctions.h"
#include "./include/Printer.h"
#include "./include/MyJetFinder.h"

bool isFloat(const char* arg);

// ---------------------------------------------------------------------------------
void ToyModel(Int_t nEvents=1000, Float_t R=0.3) {
    using std::cout;
    using std::endl;

    cout << " ===================================================================\n"
         << "                    BEGINNING TOYMODEL SIMULATION.                  \n" 
         << " NUMBER OF EVENTS: " << nEvents << "\n"
         << " INPUT JET RADIUS: " << R << "\n"
         << " IN DEBUGGING MODE: " << std::boolalpha << (Printer::debug == true) << "\n"
         << " =================================================================== " 
         << endl;

    /* ------------------------------------------------ *
     * Object Declarations.            					*
     * ------------------------------------------------ */

    // Setup jetFinder.
    EventGenerator* eventGenerator = new EventGenerator();
    MyJetFinder* jetFinder = new MyJetFinder(R);

    /* ------------------------------------------------ *
     * Data Generation/Simulation.                   	*
     * ------------------------------------------------ */

    // Simulate nEvents with randomly generated tracks. 
    for (Int_t i_event = 0; i_event < nEvents; i_event++) {
    	// Print progress updates.
        Printer::print("Percent Complete: ", i_event,  nEvents);

        // Defined event centrality (and thus multiplicity);
        eventGenerator->SetCentrality(2.5); // Cent = 2.5% is lowest available data point. 
        Printer::print("\tNumber of particles generated: ", eventGenerator->GetMultiplicity());

        // Generate specified number/types of particles.
        eventGenerator->Generate("bkg", (Int_t) eventGenerator->GetMultiplicity()); 
        Printer::print("\tNumber of reconstructed particles: ", eventGenerator->GetRecoMult());

        // Use ClusterSequence to get list of jets in this event.
        jetFinder->FindJets(eventGenerator->GetLastEvent());
        Printer::print("\tNumber of jets found: ", jetFinder->GetNumJets());

        jetFinder->Clear();
    }

    /* ------------------------------------------------ *
     * Save Simulation Data.							*
     * ------------------------------------------------ */

    // Create new ROOT file with generic EventModel objects, and
    // store desired jet histograms in subfolder "jetPlots".
    const std::string fileName = Form("./rootFiles/TM_N%d_R%d.root", nEvents, (int) (R * 10));
    cout << "Writing data to " << fileName;
    eventGenerator->Write(fileName.data());
    jetFinder->Write(fileName.data());
}

/* This main function is needed to compile via "make."
 * Runs ToyModel over argv[1] number of events, if specified,
 * else runs for 1000 events. */
#ifndef __CINT__
int main(int argc, char* argv[]) {
    // Case 1: User only provides desired number of events. 
    if (argc == 2) {
        Printer::debug = false;
        ToyModel(atoi(argv[1]));
    // Case 2: User provides number of events and a radius.
    } else if (argc == 3 && isFloat(argv[2])) {
        Printer::debug = false;
        ToyModel(atoi(argv[1]), atof(argv[2]));
    // Case 3: User provides number of events and debug flag.
    } else if (argc == 3 && strcmp(argv[2], "debug") == 0){
        Printer::debug = true;
        ToyModel(atoi(argv[1]));
    // Case 4: User specifies no additional arguments. 
    } else {
        Printer::debug = false;
        ToyModel();
    }
    return 0;
}
#endif

bool isFloat(const char* arg) {
    return typeid(atof(arg)) == typeid(double);
}

