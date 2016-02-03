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
#include "./include/PrintFunctions.h"
#include "./include/RootClasses.h"
#include "./include/ToyModel.h"

// --------------------------------------------------------------------------------------------
void PlotModel()
{

    Float_t eta[3], phi[3], pt[3];

    TH1F* h_dphi = new TH1F("h_dphi", ";#phi_{trig} - #phi_{assoc};counts", 100, 0., 2*pi);

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Grab phase space trees from ToyModel.C output. 
    TFile* f_trees = new TFile("./rootFiles/ToyModel.root");

    // Phase space: trigger.
    TTree* t_trig = (TTree*)f_trees->Get("t_trig;1");
    t_trig->SetBranchAddress("eta", &eta[0]);
    t_trig->SetBranchAddress("phi", &phi[0]);
    t_trig->SetBranchAddress("pt", &pt[0]);

    // Phase space: associated.
    TTree* t_assoc = (TTree*)f_trees->Get("t_assoc;1");
    t_assoc->SetBranchAddress("eta", &eta[1]);
    t_assoc->SetBranchAddress("phi", &phi[1]);
    t_assoc->SetBranchAddress("pt", &pt[1]);

    // Phase space: background.
    TTree* t_bkg = (TTree*)f_trees->Get("t_bkg;1");
    t_bkg->Branch("eta", &eta[2]);
    t_bkg->Branch("phi", &phi[2]);
    t_bkg->Branch("pt", &pt[2]);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    // Loop over tree entries.
    const Int_t nEntries = t_trig->GetEntries();
    for (Int_t i_event = 0; i_event < nEntries; i_event++)
    {
        PrintEventStatus(i_event, nEntries, 10);

        // Store ith event variables in eta[], phi[], pt[].
        t_trig->GetEntry(i_event);
        t_assoc->GetEntry(i_event);
        t_bkg->GetEntry(i_event);

        // Fill histograms with desired data.
        Float_t deltaPhi = dphi(phi[0], phi[1]);
        h_dphi->Fill(deltaPhi);
    }

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TCanvas* c_dphi = (TCanvas*)DrawHistogram(h_dphi);



}
