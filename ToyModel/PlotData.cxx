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
#include "TGrid.h"
#include "../TaskBrandon/env.h"
#include "../TaskBrandon/utils.h"
//#include "./include/EventGenerator.h"


// --------------------------------------------------------------------------------------------
void PlotData(Int_t nEvents = 1000) {
    const int trig = 0;
    const int assoc = 1;
    const int bkg = 2;
    Float_t eta[3], phi[3], pt[3];
    TString histName;

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    /*
    // Obtain simulated pt, eta, phi distribution trees. 
    TString fileName = "./rootFiles/ToyModel_";
    fileName += nEvents;
    fileName += ".root";
    TFile* fModel = new TFile(fileName.Data());
    TDirectory* dirModel = (TDirectory*) fModel->Get("trees;1");

    TTree* tModel = (TTree*) dirModel->Get("tBkg;1");
    tModel->SetBranchAddress("eta", &eta[bkg]);
    tModel->SetBranchAddress("phi", &phi[bkg]);
    tModel->SetBranchAddress("pt", &pt[bkg]);
    */

    // Obtain actual data pt, eta, phi distribution trees. 
    fileName = "../Data/AnalysisResults.root";
    TFile* fData = new TFile(fileName.Data(), "READ");
    TDirectory* dirData = (TDirectory*) fData->Get("CorrelationTree;1");
    Float_t trackPt, trackEta, trackPhi;
    TTree *tEvents = (TTree*) dirData->Get("events;1");
    TTree *tTracks = (TTree*) tEvents->GetBranch("tracks");
    tTracks->SetBranchAddress("tracks.fPt", &trackPt);
    /*
    tTracks->SetBranchAddress("tracks.fEta", &trackEta);
    tTracks->SetBranchAddress("tracks.fPhi", &trackPhi);

    TH1* hPt = new TH1F("hPt", "ugh", 100, 0., 10.);
    fData->Close();
    */
    
    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */
    /*

    // Loop over tree entries.
    //int nBkg = (int) tModel->GetEntries() / nEvents;
    int nTracks = (int) tTracks->GetEntries();
    for (Int_t iEvent = 0; iEvent < nEvents; iEvent++)
    {

        tTracks->GetEntry(iEvent);
        cout << trackPt << "\t";
             << trackEta << "\t"
             << trackPhi << endl;
        // Calculate differences w.r.t. background.
        for (int i_bkg = 0; i_bkg < nBkg; i_bkg++)
        {
            // Each event has nBkg entries in tModel.
            tModel->GetEntry(nBkg * i_event + i_bkg);
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
        */



    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */
    /*

    fileName = "./rootFiles/PlotData.root";
    TFile* outFile = new TFile(fileName.Data(), "RECREATE");
    outFile->cd();
    tTracks->Draw("tracks.fPt>>hPt");
    outFile->Write();
    */

    /*
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
    tModel->SetLineColor(colors[col++]);

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
    tModel->Draw("pt");
    cBasic->cd(pad++);
    tModel->Draw("eta");
    cBasic->cd(pad++);
    tModel->Draw("phi");

    cBasic->Write();
    */
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

