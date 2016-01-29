/********************************************************************************************
* Filename:     ToyModel.C                                                                  *
* Date Created: January 25, 2016                                                            *
* Author:       Leonardo Milano                                                             *
* Edited by:    Brandon McKinzie                                                            *
* Description:  Simple model event generator for multiple parton scattering.                *
*               Serves as introduction to toy models for me (Brandon) and                   *
*               two-particle correlation study.                                             *
********************************************************************************************/
#include "./include/RootClasses.h"
#include "./lib/ToyModel.cpp"
#include <iostream>
#include <fstream>

// ---------- Constants ----------
const Int_t nEvents             = 100000;
const Int_t nBkg                = 10;
const Int_t trig_pt_threshold   = 1;
const Int_t parton_mass         = 0;
const Double_t sigma_dphi       = (pi / 4) / 2;
// -------------------------------

// --------------------------------------------------------------------------------------------
void ToyModel()
{
    // Create file for object output tests.
    std::ofstream f_debug("./debug/debug_ToyModel.txt");

    Double_t eta, phi;
    // Phase space: trigger.
    TTree* t_trig = new TTree("t_trig", "Trigger attributes");
    t_trig->Branch("eta", &eta);
    t_trig->Branch("phi", &phi);

    // Phase space: associated.
    TTree* t_assoc = new TTree("t_assoc", "Associated attributes");
    t_assoc->Branch("eta", &eta);
    t_assoc->Branch("phi", &phi);

    // Phase space: background.
    TTree* t_bkg = new TTree("t_bkg", "Background attributes");
    t_bkg->Branch("eta", &eta);
    t_bkg->Branch("phi", &phi);

    // Will randomly generate uniform eta and phi of simulated jets. 
    TRandom3* rand = new TRandom3();

    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {

        if (i_event % (nEvents / 10) == 0)
        {
            Double_t percent_complete = Double_t(i_event) / Double_t(nEvents) * 100.;
            Printf("Beginning event %i. Simulation is %.2lf%% complete.", i_event, percent_complete);
        }

        // Generate trigger eta and phi.
        eta = rand->Uniform(-1.00, 1.00);
        phi = 2*pi*rand->Rndm();
        t_trig->Fill();

        // Generate associated eta and phi.
        eta = rand->Uniform(-1., 1.);
        phi = GetAssocPhi(phi, sigma_dphi, rand);
        t_assoc->Fill();

        for (Int_t i_bkg = 0; i_bkg < nBkg; i_bkg++)
        {
            // Generate trigger eta and phi.
            eta = -1.00 + 2.00*rand->Rndm();
            phi = 2*pi*rand->Rndm();
            t_bkg->Fill();
        }
    }

    // --------------------------------------------------------------------------------
    TCanvas* c_trig = new TCanvas("c_trig", "Trigger hadron canvas", 20, 20, 700, 500);
    c_trig->Divide(2, 1);

    c_trig->cd(1);
    t_trig->Draw("eta");

    c_trig->cd(2);
    t_trig->Draw("phi");
    // --------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------
    TCanvas* c_assoc = new TCanvas("c_assoc", "Assoc hadron canvas", 20, 200, 700, 500);
    c_assoc->Divide(2, 1);

    c_assoc->cd(1);
    t_assoc->Draw("eta");

    c_assoc->cd(2);
    t_assoc->Draw("phi");
    // --------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------
    TCanvas* c_bkg = new TCanvas("c_bkg", "Background hadron canvas", 60, 200, 700, 500);
    c_bkg->Divide(2, 1);

    c_bkg->cd(1);
    t_bkg->Draw("eta");

    c_bkg->cd(2);
    t_bkg->Draw("phi");
}
