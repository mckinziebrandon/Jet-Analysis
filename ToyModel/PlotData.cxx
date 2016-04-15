/********************************************************************************************
* Filename:     PlotData.C                                                                 *
* Date Created: April 13, 2016
* Author:       Brandon McKinzie                                                            *
* Description:  Grab data and plot it. 
********************************************************************************************/
#include <iostream>
#include <fstream>
#include "TChain.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "AliEventPoolManager.h"
#include "AliCFParticle.h"
#include "TGrid.h"
#include "../TaskBrandon/env.h"
#include "../TaskBrandon/utils.h"
#include "./include/RootClasses.h"
//#include "./include/EventGenerator.h"

// --------------------------------------------------------------------------------------------
void PlotData(Int_t nEvents = 1000) {
    const int trig = 0;
    const int assoc = 1;
    const int bkg = 2;
    Float_t eta[3], phi[3], pt[3];
    const float pi = TMath::Pi();
    TString histName;
    Float_t trackPt, trackEta, trackPhi;

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Obtain actual data pt, eta, phi distribution trees. 
    TString     fileName = "../Data/AnalysisResults.root";
    TFile*      fData   = new TFile(fileName.Data(), "READ");
    TDirectory* dirData = (TDirectory*) fData->Get("CorrelationTree;1");

    TChain* tEvents = new TChain("CorrelationTree/events");
    ChainFiles(tEvents, kTRUE , "", "../Data/");
    SetTree(tEvents);

    TH1* hTrackPt = new TH1F("hTrackPt", "", 100, 0, 10);
    TH1* hTrackEta = new TH1F("hTrackEta", "", 40, -1., 1.);
    TH1* hTrackPhi = new TH1F("hTrackPhi", "", 50, 0., 2*pi);
    TH1* hCentrality = new TH1F("hCentrality", "", 50, 0., 5.);
    hTrackPt->SetBit(TH1::kCanRebin);
    hTrackEta->SetBit(TH1::kCanRebin);
    hTrackPhi->SetBit(TH1::kCanRebin);
    hCentrality->SetBit(TH1::kCanRebin);

    // Grab phase space trees from ToyModel.C output. 
    fileName = "./rootFiles/ToyModel_1000.root";
    TFile*      fTrees = new TFile(fileName.Data());
    TDirectory* dirTrees = (TDirectory*) fTrees->Get("trees;1");

    // Phase space: background.
    TTree* tBkg = (TTree*) dirTrees->Get("tBkg;1");
    tBkg->SetBranchAddress("eta", &eta[bkg]);
    tBkg->SetBranchAddress("phi", &phi[bkg]);
    tBkg->SetBranchAddress("pt", &pt[bkg]);
    
    // Create histograms.
    TH1* hModelPt = new TH1F("hModelPt", "", 100, 0, 10);
    TH1* hModelEta = new TH1F("hModelEta", "", 40, -1., 1.);
    TH1* hModelPhi = new TH1F("hModelPhi", "", 50, 0., 2*pi);
    hModelPt->SetBit(TH1::kCanRebin);
    hModelEta->SetBit(TH1::kCanRebin);
    hModelPhi->SetBit(TH1::kCanRebin);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    // Loop over tree entries.

    cout << "num entries = " << tEvents->GetEntries() << endl;
    for (int j = 0; j < tEvents->GetEntries(); j++) {
        if (j % 1000 == 0) { cout << "Procesing event " << j << " . . . " << endl; }

        tEvents->GetEntry(j);
        cout << "fCentrality[0] " << fCentrality[0] << endl;

        for (int i = 0; i < fTracks->GetEntriesFast(); i++){
            AliCFParticle* track = (AliCFParticle*) fTracks->UncheckedAt(i);
            hTrackPt->Fill(track->Pt());
            hTrackEta->Fill(track->Eta());
            hTrackPhi->Fill(track->Phi());

            /*
            Int_t effVars[4];
            effVars[0] = fEfficiencyCorrectionAssociated->GetAxis(0)->FindBin(eta[j]);
            effVars[1] = fEfficiencyCorrectionAssociated->GetAxis(1)->FindBin(vars[1]); //pt
            effVars[2] = fEfficiencyCorrectionAssociated->GetAxis(2)->FindBin(vars[3]); //centrality
            effVars[3] = fEfficiencyCorrectionAssociated->GetAxis(3)->FindBin(vars[5]); //zVtx
            useWeight *= fEfficiencyCorrectionAssociated->GetBinContent(effVars);
            */
        }
    }
 
    // Calculate differences w.r.t. background.
    int nBkg = (int) tBkg->GetEntries();
    for (int i_bkg = 0; i_bkg < nBkg; i_bkg++) {
        // Each event has nBkg entries in tBkg.
        tBkg->GetEntry(i_bkg);
        hModelPt->Fill(pt[bkg]);
        hModelEta->Fill(eta[bkg]);
        hModelPhi->Fill(phi[bkg]);
    }
   

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    fileName = "./rootFiles/PlotData.root";
    TFile* outFile = new TFile(fileName.Data(), "RECREATE");
    outFile->cd();
    hTrackPt->Write();
    hTrackEta->Write();
    hTrackPhi->Write();
    outFile->Close();

    // __________________ Draw All Delta Phi/Eta Plot(s) ____________________
    TCanvas * cDelta = new TCanvas("cDelta", "canvas delta", 1000, 500);
    cDelta->Divide(3, 2);

    // ----------------------- Fill delta phi canvas. -----------------------
    cDelta->cd(1)->SetLogy(1);
    cDelta->cd(1);
    hTrackPt->Draw();
    SetDrawOptions(hTrackPt, colors[1], (TString) "p_{T} (GeV/c)", (TString) "dN/dp_{T}");

    // ----------------------- Fill delta eta canvas. -----------------------
    cDelta->cd(2);
    hTrackEta->Draw();
    SetDrawOptions(hTrackEta, colors[1], (TString) "#eta", (TString) "dN/d#eta");

    // --------------- Fill delta eta vs. delta phi canvas. -----------------
    cDelta->cd(3);
    hTrackPhi->Draw();
    SetDrawOptions(hTrackPhi, colors[1], (TString) "#phi", (TString) "dN/d#phi");


    cDelta->cd(4)->SetLogy(4);
    cDelta->cd(4);
    hModelPt->Draw();
    SetDrawOptions(hModelPt, colors[0], (TString) "p_{T} (GeV/c)", (TString) "dN/dp_{T}");

    cDelta->cd(5);
    hModelEta->Draw();
    SetDrawOptions(hModelEta, colors[0], (TString) "#eta", (TString) "dN/d#eta");

    cDelta->cd(6);
    hModelPhi->Draw();
    SetDrawOptions(hModelPhi, colors[0], (TString) "#phi", (TString) "dN/d#phi");
}

#ifndef __CINT__
int main(int argc, char* argv[]) 
{
    if (argc == 2) 
        PlotData(std::atoi(argv[1]));
    else
        PlotData();
    return 0;
}
#endif

