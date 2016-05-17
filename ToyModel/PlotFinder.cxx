/********************************************************************************************
* Filename:     PlotFinder.cxx                                                              *
* Date Created: April 1, 2016                                                               *
* Author:       Brandon McKinzie                                                            *
* Description:  Given raw output of ToyModel, turn into pretty plots.                       *
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include "./include/RootClasses.h"
#include "TApplication.h"

// --------------------------------------------------------------------------------------------
void PlotFinder(Int_t nEvents = 1000) {
    using std::cout;
    using std::endl;
    using std::string;
    
    cout << "Plotting jet quantities from most recent Toy Model run of " 
         << nEvents << " number of events...." << endl;

    const Float_t pi = (Float_t) TMath::Pi();
    const int nJetVariables = 5;
    const string hJetNames[nJetVariables] = {"hPt", "hEta", "hPhi", "hArea", "hPtSub"};
    const string hJetLabels[nJetVariables] = { "p_{T}^{jet,reco}", "#eta^{jet}", "#varphi^{jet}", 
                                                "Area", "p_{T}^{jet,reco} - #rho A"};

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Grab phase space trees from ToyModel.C output. 
    TFile*      fTrees = new TFile(Form("./rootFiles/ToyModel_%d.root", nEvents), "READ");
    TDirectory* dirJets	 = (TDirectory*) fTrees->Get("jetPlots;1");

    // Per-event jet information.
    Float_t nJets, rho, sigma;
    TTree* tEventJetInfo = (TTree*) dirJets->Get("tEventJetInfo;1");
    tEventJetInfo->SetBranchAddress("n", &nJets);
    tEventJetInfo->SetBranchAddress("rho", &rho);
    tEventJetInfo->SetBranchAddress("sigma", &sigma);

    // Per-jet information.
    Float_t pt, eta, phi, area, ptSub;
    TTree* tJetInfo= (TTree*) dirJets->Get("tJetInfo;1");
    tJetInfo->SetBranchAddress("pt", &pt);
    tJetInfo->SetBranchAddress("eta", &eta);
    tJetInfo->SetBranchAddress("phi", &phi);
    tJetInfo->SetBranchAddress("area", &area);
    tJetInfo->SetBranchAddress("ptSub", &ptSub);

    /* ---------------------------------------------------- *
     * Histogram Creation.                                     *
     * ---------------------------------------------------- */

    // n, rho, sigma (event variables)
    TH1* hNJets = new TH1F("hNJets", "Number of Jets per Event", 50, 20., 60.);
    TH1* hRho   = new TH1F("hRho", ";#rho^{event}", 100, 100., 150.);
    TH1* hSigma = new TH1F("hSigma", ";#sigma^{event}", 100, 5., 30.);
    hNJets->SetBit(TH1::kCanRebin);
    hRho->SetBit(TH1::kCanRebin);
    hSigma->SetBit(TH1::kCanRebin);

    // pT, eta, phi.
    Int_t iName = 0;
    TH1* hPt    = new TH1F(hJetNames[iName].data(), hJetLabels[iName++].data(), 100, 0., 100.);
    TH1* hEta   = new TH1F(hJetNames[iName].data(), hJetLabels[iName++].data(), 100, -0.8, 0.8);
    TH1* hPhi   = new TH1F(hJetNames[iName].data(), hJetLabels[iName++].data(), 100, 0., 2. * pi);

    // Area and pTSub. 
    TH1* hArea  = new TH1F(hJetNames[iName].data(), hJetLabels[iName++].data(), 100, 0., 0.5);
    TH1* hPtSub = new TH1F(hJetNames[iName].data(), hJetLabels[iName++].data(), 100, -30., 80.);
    hArea->SetBit(TH1::kCanRebin);
    hPtSub->SetBit(TH1::kCanRebin);

    // 2-D plot of pT v area, as recommended by Peter. 
    TH2* hPtArea = new TH2D("hPtArea", ";p_{T}^{jet};Area", 100, 0., 100., 100, 0., 0.5);

    /* ---------------------------------------------------- *
     * Data Processing.                                     *
     * ---------------------------------------------------- */
    // Loop over tree entries.
    cout << "tEventJetInfo has " << tEventJetInfo->GetEntries() << " entries." << endl;
    cout << "tJetInfo has " << tJetInfo->GetEntries() << " entries." << endl;
    Int_t jetCounter = 0;
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        // Could probably just use draw() methods of TNtuple...
        tEventJetInfo->GetEntry(i_event);
        hNJets->Fill(nJets);
        hRho->Fill(rho);
        hSigma->Fill(sigma);

        // Fill individual jet variables over nJets in this event.
        for (int i = 0; i < (int) nJets; i++) {
            tJetInfo->GetEntry(jetCounter++);
            // Area cut suggested by Peter. 
            if (area > 0.15) {
                hPt->Fill(pt);
                hEta->Fill(eta);
                hPhi->Fill(phi);
                hArea->Fill(area);
                hPtSub->Fill(ptSub);
                hPtArea->Fill(pt, area);
            }
        }
    }

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TFile* f_out = new TFile(Form("./rootFiles/PlotFinder_%d.root", nEvents), "RECREATE");
    f_out->cd();
    
    // ___________ 1. Plot the basic jet pt, eta, phi variables. ___________ 
    TCanvas* cJetBasic = new TCanvas("cJetBasic", "cJetBasic", 1000, 500);
    cJetBasic->Divide(3, 1);
    Int_t iPad = 1;
    Draw(hPt, hJetLabels[0].data(), "", colors[iPad % 4], cJetBasic, iPad, true); 
    Draw(hEta, hJetLabels[1].data(), "", colors[iPad % 4], cJetBasic, iPad, true); 
    Draw(hPhi, hJetLabels[2].data(), "", colors[iPad % 4], cJetBasic, iPad, true); 

    // ___________ 2. Plot the jet area and ptSub variables. ___________ 
    TCanvas* cJetInfo = new TCanvas("cJetInfo", "cJetInfo", 800, 500);
    cJetInfo->Divide(2, 1);
    iPad = 1;
    Draw(hArea, hJetLabels[3].data(), "", colors[iPad % 4], cJetInfo, iPad, false);
    Draw(hPtSub, hJetLabels[4].data(), "", colors[iPad % 4], cJetInfo, iPad, false);

    // ___________ 2. Plot the jet area and ptSub variables. ___________ 
    TCanvas* cEventInfo = new TCanvas("cEventInfo", "cEventInfo", 1000, 500);
    cEventInfo->Divide(3, 1);
    iPad = 1;
    Draw(hNJets, "nJets (per event)", "", colors[iPad % 4], cEventInfo, iPad, false);
    Draw(hRho, "#rho (per event)", "", colors[iPad % 4], cEventInfo, iPad, false);
    Draw(hSigma, "#sigma (per event)", "", colors[iPad % 4], cEventInfo, iPad, false);

    TCanvas* cPtArea = new TCanvas("cPtArea", "cPtArea", 600, 600);
    cPtArea->cd(); 
    hPtArea->Draw("colz");

    cJetBasic->Write();
    cJetInfo->Write();
    cEventInfo->Write();
    cPtArea->Write();
}

#ifndef __CINT__
int main(int argc, char* argv[]) 
{
    // Needed for incredibly obscure reason. 
    TApplication theAPP("App", &argc, argv);

    if (argc == 2) 
        PlotFinder(std::atoi(argv[1]));
    else
        PlotFinder();
    return 0;
}
#endif


