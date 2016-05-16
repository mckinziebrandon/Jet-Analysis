/********************************************************************************************
* Filename:     PlotData.C                                                                 *
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
#include "TAlienCollection.h"
#include "AliCFParticle.h"
#include "TGrid.h"
#include "THn.h"
#include "../TaskBrandon/env.h"
#include "../TaskBrandon/utils.h"
#include "./include/RootClasses.h"

TH1* Divide(TH1* h, TGraph* g);

// --------------------------------------------------------------------------------------------
void PlotData() {
    const TString PATH  = "/home/student/jet-radius-analysis/ToyModel/";
    const Int_t nEvents = 1000;
    const Float_t pi    = TMath::Pi();
    Float_t eta, phi, pt;
    TString histName;

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Obtain actual data pt, eta, phi distribution trees. 
    TFile* fData    = new TFile(PATH + "rootFiles/Data.root", "READ");
    TH1* hTrackPt   = (TH1F*) fData->Get("hTrackPtreco;1");
    TH1* hTrackEta  = (TH1F*) fData->Get("hTrackEtareco;1");
    TH1* hTrackPhi  = (TH1F*) fData->Get("hTrackPhireco;1");

    // Grab simulated model data from ToyModel output. 
    TFile* fTrees = new TFile(Form("./rootFiles/ToyModel_%d.root", nEvents));
    TDirectory* dirTrees = (TDirectory*) fTrees->Get("trees;1");
    TTree* tBkg = (TTree*) dirTrees->Get("tBkg;1");
    tBkg->SetBranchAddress("eta", &eta);
    tBkg->SetBranchAddress("phi", &phi);
    tBkg->SetBranchAddress("pt", &pt);
    
    // Create histograms.
    TH1* hModelPt = new TH1F("hModelPt", "", numBins, xBins);
    TH1* hModelEta = new TH1F("hModelEta", "", 40, -1., 1.);
    TH1* hModelPhi = new TH1F("hModelPhi", "", 50, 0., 2*pi);
    hModelEta->SetBit(TH1::kCanRebin);
    hModelPhi->SetBit(TH1::kCanRebin);

    // Get data points from published ALICE and CMS pt distributions. 
    TFile* fileALICE    = new TFile(PATH + "rootFiles/ALICE_pt.root");
    TGraph* graphALICE  = (TGraphAsymmErrors*) fileALICE->Get("ptDistribution;1");
    graphALICE->SetTitle(";p_{T} (GeV/c); #frac{1}{N_{evt}}#frac{1}{2#pi p_{T}}#frac{d^{2}N_{ch}}{dp_{T}d#eta} (GeV/c)^{-2}");

    TFile* fileCMS = new TFile(PATH + "rootFiles/CMS_pt.root");
    TGraph* graphCMS = (TGraphAsymmErrors*) fileCMS->Get("ptDistribution;1");
    graphCMS->SetMarkerStyle(styles[1]);
    graphCMS->SetMarkerColor(colors[2]);

    // Get toy model of soft background. 
    int nBkg = (int) tBkg->GetEntries();
    for (int i_bkg = 0; i_bkg < nBkg; i_bkg++) {
        tBkg->GetEntry(i_bkg);
        if (!pt) continue;
        Float_t normPt = 1 / ((Float_t) nEvents);
        hModelPt->Fill(pt, normPt);
        hModelEta->Fill(eta);
        hModelPhi->Fill(phi);
    }
    hModelPt->Scale(1, "width");

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TFile* outFile = new TFile(PATH + "rootFiles/PlotData.root", "RECREATE");
    outFile->cd();

    // __________________ Draw individual pt/eta/phi plots for data and model.  __________________
    TCanvas * canvas = new TCanvas("canvas", "canvas delta", 1000, 500);
    canvas->Divide(3, 2);
    Int_t iPad = 1; 
    
    const std::string effCorr = "#frac{1}{#varepsilon}";
    const std::string yLabel[] = {"#frac{1}{2#pi p_{T}}#frac{dN}{dp_{T}}", 
                                  "#frac{dN}{d#eta}"
                                  "#frac{dN}{d#phi}" };
    // --------- Draw Data distributions --------- 
    Draw(hTrackPt, "p_{T} (GeV/c)", (effCorr + yLabel[0]).data(), colors[1], canvas, iPad);
    Draw(hTrackEta, "#eta", (effCorr + yLabel[1]).data(), colors[1], canvas, iPad);
    Draw(hTrackPhi, "#phi", (effCorr + yLabel[2]).data(), colors[1], canvas, iPad);
    // --------------------------------------------

    // --------- Draw Model distributions --------- 
    Draw(hModelPt, "p_{T} (GeV/c)", yLabel[0].data(), colors[0], canvas, iPad);
    Draw(hModelEta, "#eta", yLabel[1].data(), colors[0], canvas, iPad);
    Draw(hModelPhi, "#phi", yLabel[2].data(), colors[0], canvas, iPad);
    // ---------------------------------------------

    // __________________ Draw superimposed pt/eta/phi plots data/model.  __________________
    TCanvas * canvasCompare = new TCanvas("canvasCompare", "superimposed canvas", 1000, 800);
    canvasCompare->Divide(1, 2);
    iPad = 1; 

    Draw(graphALICE, canvasCompare, iPad);
    graphCMS->Draw("Psame");
    gPad->SetTicks(1, 1);
    Draw(hModelPt, hTrackPt, canvasCompare, iPad);
    DrawLegend(hModelPt, hTrackPt, graphALICE, graphCMS, canvasCompare, iPad);

    // --------------- Ratio of histograms ---------------------
    TH1* hRatio = new TH1F("hRatio", "", numBins, xBins);
    TH1* hRatioSample = Divide(hTrackPt, graphALICE);
    hRatio->Divide(hModelPt, hTrackPt);
    DrawRatio(hRatio, hRatioSample, canvasCompare);
    gPad->SetTicks(1, 1);

    canvas->Write();
    canvasCompare->Write(); 
}

#ifndef __CINT__
int main(int argc, char* argv[]) {
    PlotData();
    return 0;
}
#endif

TH1* Divide(TH1* h, TGraph* g) {
    TH1* res = new TH1D("res", "", numBins, xBins);
    Double_t* gPoints = g->GetY();
    for (int i = 1; i <= numBins; i++) {
        Double_t hContent = h->GetBinContent(i);
        Double_t gContent = gPoints[i - 1];
        res->SetBinContent(i, hContent / gContent);
    }
    return res;
}

