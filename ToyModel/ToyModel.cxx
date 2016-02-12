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
    /* ------------------------------------------------------------ *
     * Object Declarations.                                         *
     * ------------------------------------------------------------ */

    EventModel* event = new EventModel();
    Float_t eta, phi, pt;

    TFile* f_out = new TFile("./rootFiles/ToyModel.root", "RECREATE");

    // Create file for object output tests.
    std::ofstream f_debug("./debug/debug_ToyModel.txt");

    // Phase space: trigger.
    TTree* t_trig = new TTree("t_trig", "Trigger attributes");
    t_trig->Branch("eta", &eta);
    t_trig->Branch("phi", &phi);
    t_trig->Branch("pt", &pt);

    // Phase space: associated.
    TTree* t_assoc = new TTree("t_assoc", "Associated attributes");
    t_assoc->Branch("eta", &eta);
    t_assoc->Branch("phi", &phi);
    t_assoc->Branch("pt", &pt);

    // Phase space: background.
    TTree* t_bkg = new TTree("t_bkg", "Background attributes");
    t_bkg->Branch("eta", &eta);
    t_bkg->Branch("phi", &phi);
    t_bkg->Branch("pt", &pt);

    /* ---------------------------------------------------- *
     * Data Generation/Simulation.                          *       
     * ---------------------------------------------------- */

    // Simulate nEvents with randomly generated tracks. 
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        if (i_event % (nEvents / 10) == 0)
            std::cout << (Float_t)(i_event * 100)/nEvents << " Percent Complete." << std::endl;

        // Generate trigger eta, pt, and v2(pt).
        pt  = event->GetTrackPt();
        phi = event->GetTriggerPhi(pt); 
        eta = event->GetRandEta();
        t_trig->Fill();

        // Generate associated eta, phi pt.
        eta = -eta;
        phi = event->GetAssocPhi(phi, sigma_dphi);
        pt  = event->GetTrackPt();
        t_assoc->Fill();

        // Generate background eta, phi pt.
        #pragma omp parallel for
        for (Int_t i_bkg = 0; i_bkg < nBkg / 2; i_bkg++)
        {
            // Generate trigger eta and phi.
            pt  = event->GetTrackPt();
            phi = event->GetTriggerPhi(pt);
            eta = event->GetRandEta();
            t_bkg->Fill();

            pt  = event->GetTrackPt();
            phi = event->GetTriggerPhi(pt);
            eta = event->GetRandEta();
            t_bkg->Fill();
        }
    }
    

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    f_out->cd();
    t_trig->Write("", TObject::kOverwrite);
    t_assoc->Write("", TObject::kOverwrite);
    t_bkg->Write("", TObject::kOverwrite);

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

    delete t_trig;
    delete t_assoc;
    delete t_bkg;
    delete f_out;

}

#ifndef __CINT__
int main() 
{
    ToyModel();
    return 0;
}
#endif