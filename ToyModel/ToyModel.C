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


    // --------------------------------------------------------------------------------
    TCanvas* c_trig = new TCanvas("c_trig", "Trigger hadron canvas", 50, 50, 700, 500);
    TPad* p_trig_1  = new TPad("p_trig_1", "Trigger Pad 1", 0.03, 0.03, 0.47, 0.95);
    p_trig_1->SetLeftMargin(0.15);
    p_trig_1->Draw();

    TPad* p_trig_2  = new TPad("p_trig_2", "Trigger Pad 2", 0.52, 0.03, 0.95, 0.95);
    p_trig_2->SetLeftMargin(0.15);
    p_trig_2->Draw();

    p_trig_1->cd();
    t_trig->Draw("eta>>h_eta");
    SetDrawOptions((TH1F*)gDirectory->Get("h_eta"), TString("eta"), TString("counts"));

    p_trig_2->cd();
    t_trig->Draw("phi>>h_phi");
    SetDrawOptions((TH1F*)gDirectory->Get("h_phi"), TString("phi"), TString("counts"));
    // --------------------------------------------------------------------------------

    /*
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
    */
}
