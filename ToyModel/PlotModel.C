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

    HistName = ";#Delta#phi;dN/d#Delta#phi";
    TH1F* h_dphi = new TH1F("h_dphi", HistName.Data(), 100, 0., 2*pi);

    HistName = ";#Delta#phi;dN/d#Delta#phi";
    TH1F* h_dphi_bkg = new TH1F("h_dphi_bkg", HistName.Data(), 100, 0., 2*pi);

    HistName = ";#Delta#eta;dN/d#Delta#eta";
    TH1F* h_deta = new TH1F("h_deta", HistName.Data(), 80, -2., 2.);

    HistName = ";#Delta#eta;dN/d#Delta#eta";
    TH1F* h_deta_bkg = new TH1F("h_deta_bkg", HistName.Data(), 80, -2., 2.);

    HistName = ";#Delta#phi;#Delta#eta";
    TH2F* h_dphi_deta = new TH2F("h_dphi_deta", HistName.Data(), 100, 0., 2*pi, 80, -2., 2.);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    // Loop over tree entries.
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        if (i_event % (nEvents / 10) == 0) 
            PrintEventStatus(i_event, nEvents, 10);

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
            h_dphi_bkg->Fill(deltaPhi);
            h_deta->Fill(deltaEta);
            h_deta_bkg->Fill(deltaEta);
            h_dphi_deta->Fill(deltaPhi, deltaEta);
        }
    }


    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TCanvas * c_delta = new TCanvas("c_delta", "canvas delta", 1000, 500);
    c_delta->Divide(3, 1);

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
    SetDrawOptions(h_dphi_deta);

    c_delta->Write();

}
