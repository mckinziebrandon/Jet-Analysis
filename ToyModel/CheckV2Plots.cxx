/********************************************************************************************
* Filename:     CheckV2Plots.cxx
* Date Created: February 17, 2016
* Author:       Brandon McKinzie                                                            *
* Description:  hi
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <cassert>
#include "./include/RootClasses.h"
#include "./include/EventModel.h"

// --------------------------------------------------------------------------------------------
void CheckV2Plots()
{
    const int nobkg = 0;    const int noV2  = 0;
    const int bkg   = 1;    const int V2    = 1; 

    Float_t eta[3], phi[3], pt[3];
    TString HistName;

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

    // Create deltaPhi histograms.
    TH1F* h_dphi[2][2];
    for (int i = 0; i < 2; i++) 
    {
        for (int j = 0; j < 2; j++) 
        {
            HistName  = "h_dphi_";
            HistName += i;
            HistName += "_";
            HistName += j;
            h_dphi[i][j] = new TH1F(HistName.Data(), HistName.Data(), 100, 0., 2*pi);
        }
    }

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    // Loop over tree entries.
    EventModel* event = new EventModel();
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        // Store ith event variables in eta[], phi[], pt[].
        t_trig->GetEntry(i_event);
        t_assoc->GetEntry(i_event);

        // Calculate differences in phase space quantities.
        Float_t deltaPhi = event->dphi(phi[trig], phi[assoc]);
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
            deltaPhi = event->dphi(phi[trig], phi[bkg]);
            deltaEta = eta[trig] - eta[bkg];

            // Fill background subtracted quantities. 
            h_dphi->Fill(deltaPhi);
            h_deta->Fill(deltaEta);
            //h_dphi_deta->Fill(deltaPhi, deltaEta);

            h_dphi_bkg->Fill(deltaPhi);
            h_deta_bkg->Fill(deltaEta);
            h_dphi_deta_bkg->Fill(deltaPhi, deltaEta);
        }
    }


    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    f_out->cd();
    TCanvas * c_delta = new TCanvas("c_delta", "canvas delta", 1000, 500);
    c_delta->Divide(2, 2);

    // __________________ Draw Delta Phi Plot(s) __________________
    c_delta->cd(1);
    h_dphi->Draw();
    h_dphi_bkg->Draw("same");
    SetDrawOptions(h_dphi);
    SetDrawOptions(h_dphi_bkg);
    h_dphi_bkg->SetLineColor(kRed+2);
    h_dphi_bkg->SetFillColor(kRed+2);
    h_dphi_bkg->SetFillStyle(3003);

    TLegend * leg_dphi = new TLegend(0.6, 0.8, 0.9, 0.9);
    leg_dphi->AddEntry(h_dphi, "#phi_{trig} - #phi_{assoc+bkg}", "l");
    leg_dphi->AddEntry(h_dphi_bkg, "#phi_{trig} - #phi_{bkg}", "lf");
    leg_dphi->SetBorderSize(0);
    leg_dphi->SetTextSize(0.05);
    leg_dphi->Draw();

    // __________________ Draw Delta Eta Plot(s) __________________
    c_delta->cd(2);
    h_deta->Draw();
    h_deta_bkg->Draw("same");
    SetDrawOptions(h_deta);
    SetDrawOptions(h_deta_bkg);
    h_deta_bkg->SetLineColor(kRed+2);
    h_deta_bkg->SetFillColor(kRed+2);
    h_deta_bkg->SetFillStyle(3003);

    TLegend * leg_deta = new TLegend(0.6, 0.8, 0.9, 0.9);
    leg_deta->AddEntry(h_deta, "#eta_{trig} - #eta_{assoc+bkg}", "l");
    leg_deta->AddEntry(h_deta_bkg, "#eta_{trig} - #eta_{bkg}", "lf");
    leg_deta->SetBorderSize(0);
    leg_deta->SetTextSize(0.05);
    leg_deta->Draw();

    // ______________ Draw Delta Eta vs. Delta Phi Plot(s) ______________
    c_delta->cd(3);
    h_dphi_deta->Draw("colz");

    c_delta->cd(4);
    h_dphi_deta_bkg->Draw("colz");


    c_delta->Write();

}

#ifndef __CINT__
int main() 
{
    PlotModel();
    return 0;
}
#endif


