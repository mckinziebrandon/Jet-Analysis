/********************************************************************************************
* Filename:     GetData.cxx
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
#include "THn.h"
#include "../TaskBrandon/env.h"
#include "../TaskBrandon/utils.h"
#include "./include/RootClasses.h"

// --------------------------------------------------------------------------------------------
void GetData() {
    const float pi = TMath::Pi();
    TString histName;

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

    // Make histograms with indices corresponding to {0: reco, 1: gen}.
    TH1 *hTrackPt[2], *hTrackEta[2], *hTrackPhi[2]; 
    TString index[] = {"reco", "gen"};
    for (int i = 0; i < 2; i++) {
        histName  = "hTrackPt"; 
        histName += index[i];
        hTrackPt[i] = new TH1F(histName.Data(), "", numBins, xBins);

        histName  = "hTrackEta";
        histName += index[i];
        hTrackEta[i] = new TH1F(histName.Data(), "", 40, -1., 1.);
        hTrackEta[i]->SetBit(TH1::kCanRebin);

        histName  = "hTrackPhi";
        histName += index[i];
        hTrackPhi[i] = new TH1F(histName.Data(), "", 50, 0., 2 * pi);
        hTrackPhi[i]->SetBit(TH1::kCanRebin);
    }

    // Get 4-D efficiency data. 
    TFile* fEff = new TFile("../TaskBrandon/correction_2010_aod162_hybrid.root", "READ");
    THn* hEff = (THnD*) fEff->Get("correction;1");

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    TH1* hCentrality[2];
    hCentrality[0] = new TH1F("hCentrality0", "Un-cut Centrality dist", 5, 0., 5.);
    hCentrality[0]->SetLineColor(colors[0]);
    hCentrality[0]->SetLineWidth(2);
    hCentrality[1] = new TH1F("hCentrality1", "track-cut Centrality dist", 5, 0., 5.);
    hCentrality[1]->SetLineColor(colors[1]);
    hCentrality[1]->SetLineWidth(2);

    // Loop over tree entries.
    Int_t nEvents = tEvents->GetEntries(); 
    cout << "num entries = " << nEvents << endl;
    for (int j = 0; j < nEvents; j++) {
        tEvents->GetEntry(j);
        if (j % 1000 == 0) { cout << "Processing event " << j << " . . . " << endl; }

        hCentrality[0]->Fill(fCentrality[0]);
        for (int i = 0; i < fTracks->GetEntriesFast(); i++) {
            // Obtain reconstructed values. (i.e. non-corrected)
            AliCFParticle* track = (AliCFParticle*) fTracks->UncheckedAt(i);
            Float_t trackPt  = track->Pt();
            if (trackPt < 0.5) continue;
            Float_t trackEta = track->Eta();
            Float_t trackPhi = track->Phi();

            // Fill reconstructed (i.e. non-corrected) histograms.
            hTrackPt[0]->Fill(trackPt);
            hTrackEta[0]->Fill(trackEta);
            hTrackPhi[0]->Fill(trackPhi);

            // Obtain 1/efficiency along eta, pt, cent, and z-vtx. 
            Float_t weight;
            Int_t effVars[4];
            effVars[0]  = hEff->GetAxis(0)->FindBin(trackEta);
            effVars[1]  = hEff->GetAxis(1)->FindBin(trackPt); 
            effVars[2]  = hEff->GetAxis(2)->FindBin(fCentrality[0]); 
            effVars[3]  = hEff->GetAxis(3)->FindBin(fVtxZ); 
            weight      = (Float_t) hEff->GetBinContent(effVars);
            
            // Fill efficiency-corrected histograms.
            hTrackPt[1]->Fill(trackPt, weight);
            hTrackEta[1]->Fill(trackEta, weight);
            hTrackPhi[1]->Fill(trackPhi, weight);

            hCentrality[1]->Fill(fCentrality[0]);

        }
    }

    // Perform the normalization, but actually correctly now...
    Float_t norm = 1. / ((Float_t) nEvents);
    for (int j = 0; j < 2; j++) {
        hTrackPt[j]->Scale(norm, "width");
        for (int i = 1; i <= numBins; i++) {
            Float_t pt      = hTrackPt[j]->GetBinCenter(i);
            Float_t content = hTrackPt[j]->GetBinContent(i);
            content /= (2. * pi * pt * 1.6);
            hTrackPt[j]->SetBinContent(i, content);
        }
    }
   

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    fileName = "./rootFiles/Data.root";
    TFile* outFile = new TFile(fileName.Data(), "RECREATE");
    outFile->cd();
    for (int i = 0; i < 2; i++) {
        hTrackPt[i]->Write();
        hTrackEta[i]->Write();
        hTrackPhi[i]->Write();
    }
    hCentrality[0]->Write();
    hCentrality[1]->Write();
    hCentrality[1]->Draw();
    //hCentrality[1]->Draw("same");

    TCanvas * canvas = new TCanvas("canvas", "canvas delta", 1000, 500);
    canvas->Divide(3, 2);
    Int_t iPad = 1; // heh
    TString yLabel;
    TString effCorr = "#frac{1}{#varepsilon}"; 

    for (int i = 0; i < 2; i++) {
        yLabel = "";
        if (i == 1) { yLabel = effCorr; } 
        yLabel += "#frac{1}{2#pi p_{T}}";
        yLabel += "#frac{dN}{dp_{T}}";
        Draw(hTrackPt[i], (TString) "p_{T} (GeV/c)", yLabel, colors[1], canvas, iPad);

        yLabel = "";
        if (i == 1) { yLabel += effCorr; }
        yLabel += "#frac{dN}{d#eta}";
        Draw(hTrackEta[i], (TString) "#eta", yLabel, colors[1], canvas, iPad);

        yLabel = "";
        if (i == 1) { yLabel += effCorr; }
        yLabel += "#frac{dN}{d#phi}";
        Draw(hTrackPhi[i], (TString) "#phi", yLabel, colors[1], canvas, iPad);
    }

    canvas->Write();
    outFile->Close();
}

#ifndef __CINT__
int main(int argc, char* argv[]) {
    GetData();
    return 0;
}
#endif

