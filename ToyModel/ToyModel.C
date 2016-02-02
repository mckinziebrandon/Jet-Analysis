/********************************************************************************************
* Filename:     ToyModel.C                                                                  *
* Date Created: January 25, 2016                                                            *
* Author:       Leonardo Milano                                                             *
* Edited by:    Brandon McKinzie                                                            *
* Description:  Simple model event generator for multiple parton scattering.                *
*               Serves as introduction to toy models for me (Brandon) and                   *
*               two-particle correlation study.                                             *
********************************************************************************************/
#include <iostream>
#include <fstream>
#include "./include/PrintFunctions.h"
#include "./include/RootClasses.h"
#include "./include/ToyModel.h"

// ---------- Constants ----------
const Int_t nEvents             = 100000;
const Int_t nBkg                = 10;
const Int_t nCanvas             = 3;
const Int_t trig_pt_threshold   = 1;
const Int_t parton_mass         = 0;
const Float_t sigma_dphi       = (pi / 4) / 2;
// -------------------------------

// --------------------------------------------------------------------------------------------
void ToyModel()
{
    Float_t eta, phi;

    TFile* f_out = new TFile("./rootFiles/ToyModel.root", "RECREATE");

    // Create file for object output tests.
    std::ofstream f_debug("./debug/debug_ToyModel.txt");

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

    // Simulate nEvents with randomly generated tracks. 
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        // Print update in 10 percent increments.
        PrintEventStatus(i_event, nEvents, 10);

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

    f_out->cd();
    TString histNames[] = {"phi", "eta", "phi:eta"};

    TCanvas* c_trigger = (TCanvas*)DrawHists(3, TString("trig"), histNames, t_trig);
    c_trigger->Write();
    c_trigger->Close();

    TCanvas* c_associated = (TCanvas*)DrawHists(3, TString("assoc"), histNames, t_assoc);
    c_associated->Write();
    c_associated->Close();

    TCanvas* c_bkg = (TCanvas*)DrawHists(3, TString("bkg"), histNames, t_bkg);
    c_bkg->Write();
    c_bkg->Close();

    f_out->Write();
}


