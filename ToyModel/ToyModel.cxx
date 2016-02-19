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


// --------------------------------------------------------------------------------------------
void ToyModel()
{
    using std::cout;
    using std::flush;
    /* ------------------------------------------------------------ *
     * Object Declarations.                                         *
     * ------------------------------------------------------------ */

    // Setup EventModel with desired parameters.
    Bool_t bkg_on   = true;
    Bool_t V2_on    = true;
    EventModel* event = new EventModel();

    // Create file for object output tests.
    std::ofstream f_debug("./debug/debug_ToyModel.txt");

    /* ---------------------------------------------------- *
     * Data Generation/Simulation.                          *       
     * ---------------------------------------------------- */

    // Simulate nEvents with randomly generated tracks. 
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        if (i_event % (nEvents / 20) == 0)
            cout << "\n" << (Float_t)(i_event * 100)/nEvents << " Percent Complete." << flush;
        else if (i_event % (nEvents / 200) == 0)
            cout << " ." << flush;

        // Generate trigger eta, pt, and v2(pt).
        event->GenerateTrigger();

        // Generate associated eta, phi pt.
        event->GenerateJet();

        // Generate background eta, phi pt.
        #pragma omp parallel for
        for (Int_t i_bkg = 0; i_bkg < nBkg; i_bkg++)
            event->GenerateBackground();
    }

    event->Write();
    

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    /*
    // Create list of generic histogram identifiers for plotting.
    TString histNames[] = {"phi", "eta", "pt", "eta:phi"};

    // Draw [3] [trigger] histograms from data in [t_trig].
    TCanvas* c_trigger = (TCanvas*)DrawHists(4, TString("trigger"), histNames, t_trig);
    c_trigger->Write();
    c_trigger->Close();

    // Draw [3] [associated] histograms from data in [t_assoc].
    TCanvas* c_associated = (TCanvas*)DrawHists(4, TString("associated"), histNames, t_assoc);
    c_associated->Write();
    c_associated->Close();

    // Draw [3] [background] histograms from data in [t_bkg].
    TCanvas* c_bkg = (TCanvas*)DrawHists(4, TString("background"), histNames, t_bkg);
    c_bkg->Write();
    c_bkg->Close();
    */
}

#ifndef __CINT__
int main() 
{
    ToyModel();
    return 0;
}
#endif
