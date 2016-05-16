/********************************************************************************************
* Filename:     PlotFinder.cxx
* Date Created: April 1, 2016                                                            *
* Author:       Brandon McKinzie                                                            *
* Description:           *
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include "./include/RootClasses.h"

// --------------------------------------------------------------------------------------------
void PlotFinder(Int_t nEvents = 1000)
{

    const int nJetVariables = 5;
    Float_t nJets, jetPt, jetEta, jetPhi;
    Float_t area, ptSub;
    Float_t pt, eta, phi;
    TString histName;

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Grab phase space trees from ToyModel.C output. 
    TFile*      fTrees = new TFile(Form("./rootFiles/ToyModel_%d.root", nEvents));
    TDirectory* dirJets	 = (TDirectory*) fTrees->Get("jetPlots;1");

    // Per-event jet information.
    TTree* tNJets = (TTree*) dirJets->Get("tNJets;1");
    TTree* tJetInfo= (TTree*) dirJets->Get("tJetInfo;1");
    tNJets->SetBranchAddress("n", &nJets);
    tJetInfo->SetBranchAddress("pt", &jetPt);
    tJetInfo->SetBranchAddress("eta", &jetEta);
    tJetInfo->SetBranchAddress("phi", &jetPhi);
    tJetInfo->SetBranchAddress("area", &area);
    tJetInfo->SetBranchAddress("ptSub", &ptSub);

    TH1* hNJets = new TH1F("hNJets", "", 50, 0.0, 50.0);
    hNJets->SetBit(TH1::kCanRebin);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    TH1* hJetInfo[nJetVariables];
    const TString hJetNames[nJetVariables] = {"hPt", "hEta", "hPhi", "hArea", "hPtSub"};
    const TString hJetLabels[nJetVariables] = {"Jet p_{T}", 
        "Jet #eta", "Jet #varphi", "Jet area A", "Jet p_{T} - #rho A"};
    for (int i = 0; i < nJetVariables; i++) {
        if (i == nJetVariables - 2) hJetInfo[i] = new TH1F(hJetNames[i].Data(), "", 100, 0., 1.);
        else if (i == 2) hJetInfo[i] = new TH1F(hJetNames[i].Data(), "", 100, 0., 10.);
        else if (i == 1) hJetInfo[i] = new TH1F(hJetNames[i].Data(), "", 100, -1., 1.);
        else hJetInfo[i] = new TH1F(hJetNames[i].Data(), hJetLabels[i].Data(), 100, 0., 100.);
        hJetInfo[i]->SetBit(TH1::kCanRebin);
    }

    cout << "tNJets has " << tNJets->GetEntries() << " entries." << endl;
    cout << "tJetInfo has " << tJetInfo->GetEntries() << " entries." << endl;

    // Loop over tree entries.
    Int_t jetCounter = 0;
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        // Store ith event variables in eta[], phi[], pt[].
        tNJets->GetEntry(i_event);
        hNJets->Fill(nJets);

        for (int i = 0; i < (int) nJets; i++) {
            tJetInfo->GetEntry(jetCounter++);
            int jetVar = 0;
            hJetInfo[jetVar++]->Fill(jetPt);
            hJetInfo[jetVar++]->Fill(jetEta);
            hJetInfo[jetVar++]->Fill(jetPhi);
            hJetInfo[jetVar++]->Fill(area);
            hJetInfo[jetVar++]->Fill(ptSub);
        }
    }

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TFile* f_out = new TFile(Form("./rootFiles/PlotFinder_%d.root", nEvents), "RECREATE");
    f_out->cd();
    
    TCanvas* jetCanvas = new TCanvas("jetCanvas", "jetCanvas", 1000, 500);
    jetCanvas->Divide(3, 2);
    Int_t iPad = 1;

    while (iPad <= 5) { Draw(hJetInfo[iPad - 1], hJetLabels[iPad - 1].Data(), "", (Color_t) colors[iPad%4], jetCanvas, iPad); }
    Draw(hNJets, "Number of Jets per Event", "", colors[iPad % 4], jetCanvas, iPad);

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


