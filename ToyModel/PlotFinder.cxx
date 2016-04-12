/********************************************************************************************
* Filename:     PlotFinder.cxx
* Date Created: April 1, 2016                                                            *
* Author:       Brandon McKinzie                                                            *
* Description:           *
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <cassert>
#include "./include/EventGenerator.h"

// --------------------------------------------------------------------------------------------
void PlotFinder(Int_t nEvents = 1000)
{

    Float_t nJets, jetPt, jetEta, jetPhi;
    Float_t pt, eta, phi;
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
    TDirectory* dirJets	 = (TDirectory*) fTrees->Get("jetPlots;1");

    TTree* tAssoc = (TTree*) dirTrees->Get("tAssoc;1");
    tAssoc->SetBranchAddress("eta", &eta);
    tAssoc->SetBranchAddress("phi", &phi);
    tAssoc->SetBranchAddress("pt", &pt);

    // Per-event jet information.
    TTree* tNJets = (TTree*) dirJets->Get("tNJets;1");
    TTree* tJets= (TTree*) dirJets->Get("tJetInfo;1");
    tNJets->SetBranchAddress("n", &nJets);
    tJets->SetBranchAddress("pt", &jetPt);
    tJets->SetBranchAddress("eta", &jetEta);
    tJets->SetBranchAddress("phi", &jetPhi);

    TH1* hDelta[3];
    hDelta[0] = new TH1F("hDeltaPt", "", 101, 0, 101);
    hDelta[1] = new TH1F("hDeltaEta", "", 21, -2, 2);
    hDelta[2] = new TH1F("hDeltaPhi", "", 35, 0, 3.5);
    TH1* hNJets = new TH1F("hNJets", "", 10, -0.5, 5.0);
    hNJets->SetBit(TH1::kCanRebin);
    hDelta[0]->SetBit(TH1::kCanRebin);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    cout << "tNJets has " << tNJets->GetEntries() << " entries." << endl;
    cout << "tJets has " << tJets->GetEntries() << " entries." << endl;

    // Loop over tree entries.
    Int_t jetCounter = 0;
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        // Store ith event variables in eta[], phi[], pt[].
        tAssoc->GetEntry(i_event);
        tNJets->GetEntry(i_event);
        hNJets->Fill(nJets);

        for (int i = 0; i < (int) nJets; i++) {
            tJets->GetEntry(jetCounter++);
            hDelta[0]->Fill(jetPt - 100);
            hDelta[1]->Fill(jetEta - eta);
            hDelta[2]->Fill(EventGenerator::dphi(jetPhi, phi));
        }
    }


    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    fileName = "./rootFiles/PlotFinder_";
    fileName += nEvents;
    fileName += ".root";
    TFile* f_out = new TFile(fileName.Data(), "RECREATE");
    f_out->cd();
    
    TCanvas* jetCanvas = new TCanvas("jetCanvas", "jetCanvas", 1000, 500);
    jetCanvas->Divide(4, 1);

    // njets
    jetCanvas->cd(1);
    hNJets->Draw();
    SetDrawOptions(hNJets, 32, (TString) "N_{jets} Per Event", (TString) "Counts");
    hNJets->SetFillStyle(3003);

    // Delta pt
    jetCanvas->cd(2)->SetLogy(1);
    jetCanvas->cd(2);
    hDelta[0]->Draw();
    SetDrawOptions(hDelta[0], 32, (TString) "#Delta pt (reconstructed - embedded)", (TString) "dN/d#Delta pt");
    hDelta[0]->SetFillStyle(3003);

    // delta eta
    jetCanvas->cd(3);
    hDelta[1]->Draw();
    SetDrawOptions(hDelta[1], 32, (TString) "#Delta#eta", (TString) "Counts");
    hDelta[1]->SetFillStyle(3003);

    // delta phi
    jetCanvas->cd(4);
    hDelta[2]->Draw();
    SetDrawOptions(hDelta[2], 32, (TString) "#Delta#phi", (TString) "Counts");
    hDelta[2]->SetFillStyle(3003);

    /*
    TLegend * leg_dphi = new TLegend(0.6, 0.8, 0.9, 0.9);
    leg_dphi->AddEntry(hDeltaPt, "#pt_{jet,reco} - #pt_{jet,embed}", "l");
    leg_dphi->SetBorderSize(0);
    leg_dphi->SetTextSize(0.05);
    leg_dphi->Draw();
    */

    jetCanvas->Write();
}

#ifndef __CINT__
int main(int argc, char* argv[]) 
{
    if (argc == 2) 
        PlotFinder(std::atoi(argv[1]));
    else
        PlotFinder();
    return 0;
}
#endif


