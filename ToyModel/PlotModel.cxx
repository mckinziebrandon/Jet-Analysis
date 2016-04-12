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
#include "./include/EventGenerator.h"

// --------------------------------------------------------------------------------------------
void PlotModel(Int_t nEvents = 1000) {
    const int trig = 0;
    const int assoc = 1;
    const int bkg = 2;
    Float_t eta[3], phi[3], pt[3];
    TString histName;

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    TString fileName = "./rootFiles/ToyModel_";
    fileName += nEvents;
    fileName += ".root";

    // Grab phase space trees from ToyModel.C output. 
    TFile*      fTrees = new TFile(fileName.Data());
    TDirectory* dirTrees = (TDirectory*) fTrees->Get("trees;1");

    // Phase space: trigger.
    TTree* tTrig = (TTree*) dirTrees->Get("tTrig;1");
    tTrig->SetBranchAddress("eta", &eta[trig]);
    tTrig->SetBranchAddress("phi", &phi[trig]);
    tTrig->SetBranchAddress("pt", &pt[trig]);

    // Phase space: associated.
    TTree* tAssoc = (TTree*) dirTrees->Get("tAssoc;1");
    tAssoc->SetBranchAddress("eta", &eta[assoc]);
    tAssoc->SetBranchAddress("phi", &phi[assoc]);
    tAssoc->SetBranchAddress("pt", &pt[assoc]);

    // Phase space: background.
    TTree* tBkg = (TTree*) dirTrees->Get("tBkg;1");
    tBkg->SetBranchAddress("eta", &eta[bkg]);
    tBkg->SetBranchAddress("phi", &phi[bkg]);
    tBkg->SetBranchAddress("pt", &pt[bkg]);
    
    // Create deltaPhi histograms.
    TH1F* hDPhiAll = new TH1F("hDPhiAll", "", 100, 0., 2*pi);
    TH1F* hDPhiBkg = new TH1F("hDPhiBkg", "", 100, 0., 2*pi);

    // Create deltaEta histograms.
    TH1F* hDEtaAll = new TH1F("hDEtaAll", "", 80, -2., 2.);
    TH1F* hDEtaBkg = new TH1F("hDEtaBkg", "", 80, -2., 2.);

    // Create 2D dPhidEta histograms.
    TH2F* hDPhiDEtaAll     = new TH2F("hDPhiDEtaAll", "", 100, 0., 2*pi, 80, -2., 2.);
    TH2F* hDPhiDEtaBkg = new TH2F("hDPhiDEtaBkg", "", 100, 0., 2*pi, 80, -2., 2.);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    // Loop over tree entries.
    int nBkg = (int) tBkg->GetEntries() / nEvents;
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        // Store ith event variables in eta[], phi[], pt[].
        tTrig->GetEntry(i_event);
        tAssoc->GetEntry(i_event);
    
        // Calculate differences in phase space quantities.
        Float_t deltaPhi = EventGenerator::dphi(phi[trig], phi[assoc]);
        Float_t deltaEta = eta[trig] - eta[assoc];

        // Fill Histograms with trig-assoc data.
        hDPhiAll->Fill(deltaPhi);
        hDEtaAll->Fill(deltaEta);
        hDPhiDEtaAll->Fill(deltaPhi, deltaEta);

        // Calculate differences w.r.t. background.
        for (int i_bkg = 0; i_bkg < nBkg; i_bkg++)
        {
            // Each event has nBkg entries in tBkg.
            tBkg->GetEntry(nBkg * i_event + i_bkg);
            deltaPhi = EventGenerator::dphi(phi[trig], phi[bkg]);
            deltaPhi = EventGenerator::dphi((float) phi[trig], (float) phi[bkg]);
            deltaEta = eta[trig] - eta[bkg];

            // Fill background subtracted quantities. 
            hDPhiAll->Fill(deltaPhi);
            hDEtaAll->Fill(deltaEta);
            hDPhiDEtaAll->Fill(deltaPhi, deltaEta);

            hDPhiBkg->Fill(deltaPhi);
            hDEtaBkg->Fill(deltaEta);
            hDPhiDEtaBkg->Fill(deltaPhi, deltaEta);
        }
    }


    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    fileName = "./rootFiles/PlotModel_";
    fileName += nEvents;
    fileName += ".root";
    TFile* outFile = new TFile(fileName.Data(), "RECREATE");
    outFile->cd();

    // __________________ Draw All Delta Phi/Eta Plot(s) ____________________
    TCanvas * cDelta = new TCanvas("cDelta", "canvas delta", 1000, 500);
    cDelta->Divide(3, 1);

    // ----------------------- Fill delta phi canvas. -----------------------
    cDelta->cd(1);
    hDPhiAll->Draw();
    hDPhiBkg->Draw("same");
    SetDrawOptions(hDPhiAll, 36, (TString) "#Delta#phi", (TString) "dN/d#Delta#phi");
    SetDrawOptions(hDPhiBkg, 45, (TString) "#Delta#phi", (TString) "dN/d#Delta#phi");
    hDPhiBkg->SetFillStyle(3003);
    
    TLegend * legDPhi = new TLegend(0.6, 0.8, 0.9, 0.9);
    legDPhi->AddEntry(hDPhiAll, "#phi_{trig} - #phi_{assoc+bkg}", "l");
    legDPhi->AddEntry(hDPhiBkg, "#phi_{trig} - #phi_{bkg}", "lf");
    legDPhi->SetBorderSize(0);
    legDPhi->SetTextSize(0.05);
    legDPhi->Draw();

    // ----------------------- Fill delta eta canvas. -----------------------
    cDelta->cd(2);
    hDEtaAll->Draw();
    hDEtaBkg->Draw("same");
    SetDrawOptions(hDEtaAll, 36, (TString) "#Delta#eta", (TString) "dN/d#Delta#eta");
    SetDrawOptions(hDEtaBkg, 45, (TString) "#Delta#eta", (TString) "dN/d#Delta#eta");
    hDEtaBkg->SetFillStyle(3003);

    TLegend * leg_deta = new TLegend(0.6, 0.8, 0.9, 0.9);
    leg_deta->AddEntry(hDEtaAll, "#eta_{trig} - #eta_{assoc+bkg}", "l");
    leg_deta->AddEntry(hDEtaBkg, "#eta_{trig} - #eta_{bkg}", "lf");
    leg_deta->SetBorderSize(0);
    leg_deta->SetTextSize(0.05);
    leg_deta->Draw();

    // --------------- Fill delta eta vs. delta phi canvas. -----------------
    cDelta->cd(3);
    hDPhiDEtaAll->Draw("colz");
    SetDrawOptions(hDPhiDEtaAll, 36, (TString) "#Delta#phi", (TString) "#Delta#eta");


    outFile->cd();
    cDelta->Write();

    // __________________ Draw All Basic Phase Space Plot(s).  ____________________
    TCanvas * cBasic = new TCanvas("cBasic", "pt eta phi of all particles", 1000, 500);
    cBasic->Divide(3, 3);
    Int_t pad = 1;
    int col = 0;

    tTrig->SetLineColor(colors[col++]);
    tAssoc->SetLineColor(colors[col++]);
    tBkg->SetLineColor(colors[col++]);

    // Trigger 
    cBasic->cd(pad++);
    tTrig->Draw("pt");
    cBasic->cd(pad++);
    tTrig->Draw("eta");
    cBasic->cd(pad++);
    tTrig->Draw("phi");

    // Associated 
    cBasic->cd(pad++);
    tAssoc->Draw("pt");
    cBasic->cd(pad++);
    tAssoc->Draw("eta");
    cBasic->cd(pad++);
    tAssoc->Draw("phi");

    // Background 
    cBasic->cd(pad++);
    tBkg->Draw("pt");
    cBasic->cd(pad++);
    tBkg->Draw("eta");
    cBasic->cd(pad++);
    tBkg->Draw("phi");

    cBasic->Write();
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

