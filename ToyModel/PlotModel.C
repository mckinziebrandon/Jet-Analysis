/********************************************************************************************
* Filename:     PlotModel.C                                                                 *
* Date Created: January 28, 2016                                                            *
* Author:       Brandon McKinzie                                                            *
* Description:  Plot differences in trigger and associated:                                 *
*                   1. deltaPhi                                                             *
*                   2. deltaEta                                                             *
*                   3. deltaEta vs. deltaPhi                                                *
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <cassert>
#include "./include/PrintFunctions.h"
#include "./include/RootClasses.h"
#include "./include/ToyModel.h"

// --------------------------------------------------------------------------------------------
void PlotModel()
{
    // Set eta/phi/pt index aliases.
    const int trig  = 0;
    const int assoc = 1;
    const int bkg   = 2;

    Float_t eta[3], phi[3], pt[3];

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Grab phase space trees from ToyModel.C output. 
    TFile* f_trees = new TFile("./rootFiles/ToyModel.root");

    // Phase space: trigger.
    TTree* t_trig = (TTree*)f_trees->Get("t_trig;1");
    t_trig->SetBranchAddress("eta", &eta[trig]);
    t_trig->SetBranchAddress("phi", &phi[trig]);
    t_trig->SetBranchAddress("pt", &pt[trig]);

    // Phase space: associated.
    TTree* t_assoc = (TTree*)f_trees->Get("t_assoc;1");
    t_assoc->SetBranchAddress("eta", &eta[assoc]);
    t_assoc->SetBranchAddress("phi", &phi[assoc]);
    t_assoc->SetBranchAddress("pt", &pt[assoc]);

    // Phase space: background.
    TTree* t_bkg = (TTree*)f_trees->Get("t_bkg;1");
    t_bkg->SetBranchAddress("eta", &eta[bkg]);
    t_bkg->SetBranchAddress("phi", &phi[bkg]);
    t_bkg->SetBranchAddress("pt", &pt[bkg]);
    
    TFile* f_out = new TFile("./rootFiles/PlotModel.root", "RECREATE");
    f_out->cd();

    HistName = ";#phi_{trig} - #phi_{assoc+bkg};counts";
    TH1F* h_dphi = new TH1F("h_dphi", HistName.Data(), 100, 0., 2*pi);

    HistName = ";#eta_{trig} - #eta_{assoc+bkg};counts";
    TH1F* h_deta = new TH1F("h_deta", HistName.Data(), 80, -2., 2.);

    HistName = ";#phi_{trig} - #phi_{assoc+bkg};#eta_{trig} - #eta_{assoc+bkg}";
    TH2F* h_dphi_deta = new TH2F("h_dphi_deta", HistName.Data(), 100, 0., 2*pi, 80, -2., 2.);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    // Loop over tree entries.
    assert(t_trig->GetEntries() == t_assoc->GetEntries());
    assert(t_bkg->GetEntries() == 10 * t_trig->GetEntries());
    const Int_t nEntries = t_trig->GetEntries();
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        PrintEventStatus(i_event, nEntries, 10);

        // Store ith event variables in eta[], phi[], pt[].
        t_trig->GetEntry(i_event);
        t_assoc->GetEntry(i_event);

        // Calculate differences in phase space quantities.
        Float_t deltaPhi = dphi(phi[trig], phi[assoc]);
        Float_t deltaEta = eta[trig] - eta[assoc];

        // Fill Histograms with trig-assoc data.
        h_dphi->Fill(deltaPhi);
        h_deta->Fill(deltaEta);
        h_dphi_deta->Fill(deltaPhi, deltaEta);

        // Calculate differences w.r.t. background.
        for (int i_bkg = 0; i_bkg < nBkg; i_bkg++)
        {
            // Each event has nBkg entries in t_bkg.
            t_bkg->GetEntry(nBkg * i_event + i_bkg);
            deltaPhi = dphi(phi[trig], phi[bkg]);
            deltaEta = eta[trig] - eta[bkg];

            // Fill background subtracted quantities. 
            h_dphi->Fill(deltaPhi);
            h_deta->Fill(deltaEta);
            h_dphi_deta->Fill(deltaPhi, deltaEta);
        }
    }


    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TCanvas * c_delta = new TCanvas("c_delta", "canvas delta", 1000, 500);
    c_delta->Divide(3, 1);

    c_delta->cd(1);
    h_dphi->Draw();
    SetDrawOptions(h_dphi);

    c_delta->cd(2);
    h_deta->Draw();
    SetDrawOptions(h_deta);

    c_delta->cd(3);
    h_dphi_deta->Draw("colz");
    SetDrawOptions(h_dphi_deta);

    c_delta->Write();
}
