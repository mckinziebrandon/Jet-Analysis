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
#include "./include/RootClasses.h"
#include "./include/EventModel.h"

// --------------------------------------------------------------------------------------------
void PlotModel(Int_t nEvents = 1000)
{

    Float_t eta[3], phi[3], pt[3];
    TString HistName;

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    TString fileName = "./rootFiles/ToyModel_";
    fileName += nEvents;
    fileName += ".root";

    // Grab phase space trees from ToyModel.C output. 
    TFile* f_trees = new TFile(fileName.Data());
    TDirectory* dir_trees 	= (TDirectory*) f_trees->Get("trees;1");
    TDirectory* dir_jets	= (TDirectory*) f_trees->Get("jetPlots;1");

    // Phase space: trigger.
    TTree* t_trig = (TTree*) dir_trees->Get("tt;1");
    t_trig->SetBranchAddress("eta", &eta[trig]);
    t_trig->SetBranchAddress("phi", &phi[trig]);
    t_trig->SetBranchAddress("pt", &pt[trig]);

    // Phase space: associated.
    TTree* t_assoc = (TTree*) dir_trees->Get("ta;1");
    t_assoc->SetBranchAddress("eta", &eta[assoc]);
    t_assoc->SetBranchAddress("phi", &phi[assoc]);
    t_assoc->SetBranchAddress("pt", &pt[assoc]);

    // Phase space: background.
    TTree* t_bkg = (TTree*) dir_trees->Get("tb;1");
    t_bkg->SetBranchAddress("eta", &eta[bkg]);
    t_bkg->SetBranchAddress("phi", &phi[bkg]);
    t_bkg->SetBranchAddress("pt", &pt[bkg]);
    
    // Per-event jet information.
    Float_t n;
    Float_t ptReco;
    TTree* t_nJetReco = (TTree*) dir_jets->Get("t_nJetReco;1");
    TTree* t_ptJetReco= (TTree*) dir_jets->Get("t_ptJetReco;1");
    t_nJetReco->SetBranchAddress("n", &n);
    t_ptJetReco->SetBranchAddress("pt", &ptReco);
    TH1* h_dpt = new TH1F("h_dpt", "reco - embed pt", 100, 0.0, 100);

    // Create deltaPhi histograms.
    HistName = ";#Delta#phi;dN/d#Delta#phi";
    TH1F* h_dphi     = new TH1F("h_dphi", HistName.Data(), 100, 0., 2*pi);
    TH1F* h_dphi_bkg = new TH1F("h_dphi_bkg", HistName.Data(), 100, 0., 2*pi);

    // Create deltaEta histograms.
    HistName = ";#Delta#eta;dN/d#Delta#eta";
    TH1F* h_deta     = new TH1F("h_deta", HistName.Data(), 80, -2., 2.);
    TH1F* h_deta_bkg = new TH1F("h_deta_bkg", HistName.Data(), 80, -2., 2.);

    // Create 2D dPhidEta histograms.
    HistName = ";#Delta#phi;#Delta#eta";
    TH2F* h_dphi_deta     = new TH2F("h_dphi_deta", HistName.Data(), 100, 0., 2*pi, 80, -2., 2.);
    TH2F* h_dphi_deta_bkg = new TH2F("h_dphi_deta_bkg", HistName.Data(), 100, 0., 2*pi, 80, -2., 2.);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    // Loop over tree entries.
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        // Store ith event variables in eta[], phi[], pt[].
        t_trig->GetEntry(i_event);
        t_assoc->GetEntry(i_event);
        t_nJetReco->GetEntry(i_event);
        t_ptJetReco->GetEntry(i_event);

        for (int i = 0; i < (int) n; i++) {
			Float_t deltaPt = abs(ptReco - 100.0);
			h_dpt->Fill(deltaPt);
        }

        // Calculate differences in phase space quantities.
        Float_t deltaPhi = EventModel::dphi(phi[trig], phi[assoc]);
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
            deltaPhi = EventModel::dphi(phi[trig], phi[bkg]);
            deltaPhi = EventModel::dphi((float) phi[trig], (float) phi[bkg]);
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

    TFile* f_out = new TFile("./rootFiles/PlotModel.root", "RECREATE");
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

    f_out->cd();
    c_delta->Write();

    TCanvas* jetCanvas = new TCanvas("jetCanvas", "jetCanvas", 700, 500);
    jetCanvas->cd();
    h_dpt->Draw();
    jetCanvas->Write();
}

#ifndef __CINT__
int main(int argc, char* argv[]) 
{
    if (argc == 2) 
        PlotModel(std::atoi(argv[1]));
    else
        PlotModel();
    return 0;
}
#endif


