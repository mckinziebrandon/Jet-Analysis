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
#include "THn.h"
#include "../TaskBrandon/env.h"
#include "../TaskBrandon/utils.h"
#include "./include/RootClasses.h"

TH1* Divide(TH1* h, TGraph* g);
void NormalizeCMS(TGraph* g);

// --------------------------------------------------------------------------------------------
void PlotData() {
    const int trig = 0;
    const int assoc = 1;
    const int bkg = 2;
    Float_t eta[3], phi[3], pt[3];
    const float pi = TMath::Pi();
    TString histName;

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Obtain actual data pt, eta, phi distribution trees. 
    TString     fileName = "rootFiles/Data.root";
    TFile*      fData   = new TFile(fileName.Data(), "READ");

    // Make histograms with indices corresponding to {0: reco, 1: gen}.
    TH1 *hTrackPt = (TH1F*) fData->Get("hTrackPtgen;1");
    TH1 *hTrackEta = (TH1F*) fData->Get("hTrackEtagen;1");
    TH1 *hTrackPhi = (TH1F*) fData->Get("hTrackPhigen;1");

    // Grab phase space trees from ToyModel output. 
    const int nEvents = 2000;
    fileName = "./rootFiles/ToyModel_2000.root";
    TFile*      fTrees = new TFile(fileName.Data());
    TDirectory* dirTrees = (TDirectory*) fTrees->Get("trees;1");

    // Phase space: background.
    TTree* tBkg = (TTree*) dirTrees->Get("tBkg;1");
    tBkg->SetBranchAddress("eta", &eta[bkg]);
    tBkg->SetBranchAddress("phi", &phi[bkg]);
    tBkg->SetBranchAddress("pt", &pt[bkg]);
    
    // Create histograms.
    TH1* hModelPt = new TH1F("hModelPt", "", numBins, xBins);
    TH1* hModelEta = new TH1F("hModelEta", "", 40, -1., 1.);
    TH1* hModelPhi = new TH1F("hModelPhi", "", 50, 0., 2*pi);
    hModelEta->SetBit(TH1::kCanRebin);
    hModelPhi->SetBit(TH1::kCanRebin);

    TFile*  fileALICE  = new TFile("/home/student/jet-radius-analysis/ToyModel/rootFiles/ALICE_pt.root");
    TGraph* graphALICE = (TGraphAsymmErrors*) fileALICE->Get("ptDistribution;1");
    graphALICE->SetTitle(";p_{T} (GeV/c); #frac{1}{N_{evt}}#frac{1}{2#pi p_{T}}#frac{d^{2}N_{ch}}{dp_{T}d#eta} (GeV/c)^{-2}");

    const Int_t n_ALICE_Events = 53573;
    TFile*  fileCMS  = new TFile("/home/student/jet-radius-analysis/ToyModel/rootFiles/CMS_pt.root");
    TGraph* graphCMS = (TGraphAsymmErrors*) fileCMS->Get("ptDistribution;1");
    NormalizeCMS(graphCMS);

    // Get toy model of soft background. 
    int nBkg = (int) tBkg->GetEntries();
    for (int i_bkg = 0; i_bkg < nBkg; i_bkg++) {
        // Each event has nBkg entries in tBkg.
        tBkg->GetEntry(i_bkg);
        if (!pt[bkg]) continue;
        Float_t normPt = 1 / ((Float_t) nEvents);
        hModelPt->Fill(pt[bkg], normPt);
        hModelEta->Fill(eta[bkg]);
        hModelPhi->Fill(phi[bkg]);
    }
    hModelPt->Scale(1, "width");

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    fileName = "./rootFiles/PlotData.root";
    TFile* outFile = new TFile(fileName.Data(), "RECREATE");
    outFile->cd();

    // __________________ Draw individual pt/eta/phi plots for data and model.  __________________
    TCanvas * canvas = new TCanvas("canvas", "canvas delta", 1000, 500);
    canvas->Divide(3, 2);
    Int_t iPad = 1; // heh
    TString yLabel;
    TString effCorr = "#frac{1}{#varepsilon}"; 

    // --------- Draw Data distributions --------- 
    yLabel = "#frac{1}{#varepsilon}#frac{1}{2#pi p_{T}}";
    yLabel += "#frac{dN}{dp_{T}}";
    Draw(hTrackPt, (TString) "p_{T} (GeV/c)", yLabel, colors[1], canvas, iPad);

    yLabel =  "#frac{1}{#varepsilon}#frac{dN}{d#eta}";
    Draw(hTrackEta, (TString) "#eta", yLabel, colors[1], canvas, iPad);

    yLabel = "#frac{1}{#varepsilon}#frac{dN}{d#phi}";
    Draw(hTrackPhi, (TString) "#phi", yLabel, colors[1], canvas, iPad);
    // --------------------------------------------

    // --------- Draw Model distributions --------- 
    Draw(hModelPt, (TString) "p_{T} (GeV/c)", (TString) "#frac{1}{2#pi p_{T}}#frac{dN}{dp_{T}}", colors[0], canvas, iPad);
    Draw(hModelEta, (TString) "#eta", (TString) "dN/d#eta", colors[0], canvas, iPad);
    Draw(hModelPhi, (TString) "#phi", (TString) "dN/d#phi", colors[0], canvas, iPad);
    // ---------------------------------------------

    canvas->Write();

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
    TGraph* AliceOverCms = GetAliceOverCms(hTrackPt, graphCMS);
    hRatio->Divide(hModelPt, hTrackPt);
    DrawRatio(hRatio, hRatioSample, canvasCompare);
    gPad->SetTicks(1, 1);

    canvasCompare->Write(); 
    outFile->Close();
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

void NormalizeCMS(TGraph* g) {
    const Float_t n_Coll_CMS = 1660.; // pm 130
    for (int i = 0; i < g->GetN(); i++) {
        Double_t pt, _;
        g->GetPoint(i, pt, _);
        cout << "pt = " << pt << endl;
        //g->GetY()[i] *= n_Coll_CMS;
    }

    g->SetMarkerStyle(styles[1]);
    g->SetMarkerColor(colors[2]);
}


