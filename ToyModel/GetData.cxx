/********************************************************************************************
* Filename:     GetData.cxx
* Date Created: April 13, 2016
* Author:       Brandon McKinzie                                                            *
* Description:  Grab data and plot it. 
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
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
    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    TChain* tEvents = new TChain("CorrelationTree/events");
    ChainFiles(tEvents, kTRUE , "", "../Data/");
    SetTree(tEvents);

    // Make histograms with indices corresponding to {0: reco, 1: gen}.
    TH1 *hTrackPt[2], *hTrackEta[2], *hTrackPhi[2]; 
    const std::string index[] = {"reco", "gen"};
    for (int i = 0; i < 2; i++) {
        hTrackPt[i]  = new TH1F(Form("hTrackPt%s", index[i].c_str()), "", numBins, xBins);
        hTrackEta[i] = new TH1F(Form("hTrackEta%s", index[i].c_str()), "", 40, -1., 1.);
        hTrackPhi[i] = new TH1F(Form("hTrackPhi%s", index[i].c_str()), "", 50, 0., 2 * pi);
        hTrackEta[i]->SetBit(TH1::kCanRebin);
        hTrackPhi[i]->SetBit(TH1::kCanRebin);
    }

    // Get 4-D efficiency data. 
    TFile* fEff = new TFile("../TaskBrandon/correction_2010_aod162_hybrid.root", "READ");
    THn* hEff   = (THnD*) fEff->Get("correction;1");

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */

    // Loop over tree entries.
    const Int_t nEvents = tEvents->GetEntries(); 
    cout << "Looping over " << nEvents << " events from data." << endl;
    for (int j = 0; j < nEvents; j++) {
        tEvents->GetEntry(j);
        if (j % 1000 == 0) { cout << "Processing event " << j << " . . . " << endl; }

        for (int i = 0; i < fTracks->GetEntriesFast(); i++) {
            // Obtain reconstructed values. (i.e. non-corrected).
            AliCFParticle* track = (AliCFParticle*) fTracks->UncheckedAt(i);
            if (track->Pt() < 0.5) continue;
            Float_t trackPt  = track->Pt();
            Float_t trackEta = track->Eta();
            Float_t trackPhi = track->Phi();

            // Fill reconstructed (i.e. non-corrected) histograms.
            hTrackPt[0]->Fill(trackPt);
            hTrackEta[0]->Fill(trackEta);
            hTrackPhi[0]->Fill(trackPhi);

            // Obtain 1/efficiency along eta, pt, cent, and z-vtx. 
            Int_t effVars[4];
            effVars[0]  = hEff->GetAxis(0)->FindBin(trackEta);
            effVars[1]  = hEff->GetAxis(1)->FindBin(trackPt); 
            effVars[2]  = hEff->GetAxis(2)->FindBin(fCentrality[0]); 
            effVars[3]  = hEff->GetAxis(3)->FindBin(fVtxZ); 
            Float_t weight = (Float_t) hEff->GetBinContent(effVars);
            
            // Fill efficiency-corrected histograms.
            hTrackPt[1]->Fill(trackPt, weight);
            hTrackEta[1]->Fill(trackEta, weight);
            hTrackPhi[1]->Fill(trackPhi, weight);
        }
    }

    // Apply normalization of 1/2piPtDeltaEta to all. 
    const Float_t norm = 1. / ((Float_t) nEvents);
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

    // Save all histograms in Data.root.
    TFile* outFile = new TFile("./rootFiles/Data.root", "RECREATE");
    outFile->cd();
    for (int i = 0; i < 2; i++) {
        hTrackPt[i]->Write();
        hTrackEta[i]->Write();
        hTrackPhi[i]->Write();
    }

    // Plot all histograms on one canvas. 
    TCanvas * canvas = new TCanvas("canvas", "canvas delta", 1000, 500);
    canvas->Divide(3, 2);
    Int_t iPad = 1; 
    TString yLabel;

    const std::string effCorr[] = {"", "#frac{1}{#varepsilon}"}; 
    for (int i = 0; i < 2; i++) {
        Draw(hTrackPt[i], "p_{T} (GeV/c)", 
                (effCorr[i] + "#frac{1}{2#pi p_{T}}#frac{dN}{dp_{T}}").data(), 
                colors[1], canvas, iPad);
        Draw(hTrackEta[i], "#eta", (effCorr[i] + "#frac{dN}{d#eta}").data(), colors[1], canvas, iPad);
        Draw(hTrackPhi[i], "#phi", (effCorr[i] + "#frac{dN}{d#phi}").data(), colors[1], canvas, iPad);
    }
    canvas->Write();
}

#ifndef __CINT__
int main(int argc, char* argv[]) {
    GetData();
    return 0;
}
#endif

