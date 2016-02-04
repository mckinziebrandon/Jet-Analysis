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

    TH1F* h_dphi[2];
    TH1F* h_deta[2];
    TH2F* h_dphi_deta[2];

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

    TString strType[2];
    strType[0] = "assoc";
    strType[1] = "assoc+bkg";

    TString HistName;

    // Loop over tree entries.
    const Int_t nEntries = t_trig->GetEntries();
    for (Int_t i_event = 0; i_event < nEntries; i_event++)
    {
        PrintEventStatus(i_event, nEntries, 10);

        // Store ith event variables in eta[], phi[], pt[].
        t_trig->GetEntry(i_event);
        t_assoc->GetEntry(i_event);

        for (Int_t i_type = 0; i_type < 2; i_type++)
        {
            if (i_event == 0)
            {
                HistName = ";#phi_{trig} - #phi_{";
                HistName += strType[i_type];
                HistName += "};counts";
                h_dphi[i_type] = new TH1F("h_dphi", HistName.Data(), 100, 0., 2*pi);

                HistName = ";#eta_{trig} - #eta_{";
                HistName += strType[i_type];
                HistName += "};counts";
                h_deta[i_type] = new TH1F("h_deta", HistName.Data(), 100, -2., 2.);

                HistName = ";#phi_{trig} - #phi_{ ";
                HistName += strType[i_type];
                HistName += "};#eta_{trig}-#eta_{";
                HistName += strType[i_type];
                HistName += "}";
                h_dphi_deta[i_type] = new TH2F("h_dphi_deta", HistName.Data(), 100, 0., 2*pi, 100, -2., 2.);
            }
            
            // Fill histograms with {trig, assoc} data.
            Float_t deltaPhi = dphi(phi[0], phi[1]);
            //Float_t deltaEta = deta(eta[0], eta[1]);
            Float_t deltaEta = eta[0]-eta[1];

            h_dphi[i_type]->Fill(deltaPhi);
            h_deta[i_type]->Fill(deltaEta);
            h_dphi_deta[i_type]->Fill(deltaPhi, deltaEta);
        
            if (i_type == 1)
            {
                for (int j = 0; j < 10; j++)
                {
                    t_bkg->GetEntry(10*i_event+j);

                    // Fill histograms with {trig, bkg} data.
                    deltaPhi = dphi(phi[0], phi[2]);
                    //deltaEta = deta(eta[0], eta[2]);
                    deltaEta = eta[0]-eta[2];
                    h_dphi[i_type]->Fill(deltaPhi);
                    h_deta[i_type]->Fill(deltaEta);
                    h_dphi_deta[i_type]->Fill(deltaPhi, deltaEta);
               
                }
            }
        }
    }

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

   //TCanvas* c_dphi = (TCanvas*)DrawHistogram(h_dphi);

    TCanvas * c_delta = new TCanvas("c_delta", "canvas delta", 1000, 500);
    c_delta->Divide(3, 2);

    for (int i = 0; i < 2; i++)
    {
        c_delta->cd(1 + 3*i);
        h_dphi[i]->Draw();
        SetDrawOptions(h_dphi[i]);

        c_delta->cd(2 + 3*i);
        h_deta[i]->Draw();
        SetDrawOptions(h_deta[i]);

        c_delta->cd(3 + 3*i);
        h_dphi_deta[i]->Draw("colz");
        SetDrawOptions(h_dphi_deta[i]);
    }
}
