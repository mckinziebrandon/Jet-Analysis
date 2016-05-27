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
void PlotModel(Int_t nEvents = 1000, Float_t R = 0.3) {
    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    cout << "Beginning PlotModel for " << nEvents << " number of Events . . . " << endl;

    // Grab phase space trees from ToyModel.C output. 
    TFile* fTrees           = new TFile(Form("./rootFiles/TM_N%d_R%d.root", nEvents, (int) (R * 10)), "READ");
    TDirectory* dirTrees    = (TDirectory*) fTrees->Get("trees;1");
    TTree* tBkg             = (TTree*) dirTrees->Get("tBkg;1");

    /* ------------------------------------------------------------ *
     * Drawing into the Histograms.                                 *
     * ------------------------------------------------------------ */

    TH2* hPhiEta = new TH2F("hPhiEta", "", 100, 0., 2. * pi, 100, -etaMax, etaMax);
    TH2* hPtEta = new TH2F("hPtEta", "", 100, 0., 20., 100, -etaMax, etaMax);

    tBkg->Draw("pt>>hPt");
    tBkg->Draw("eta>>hEta");
    tBkg->Draw("phi>>hPhi");
    tBkg->Draw("eta:phi>>hPhiEta");
    tBkg->Draw("eta:pt>>hPtEta");

    TH1* hPt    = (TH1F*) gDirectory->Get("hPt");
    TH1* hEta   = (TH1F*) gDirectory->Get("hEta");
    TH1* hPhi   = (TH1F*) gDirectory->Get("hPhi");

    /* ------------------------------------------------------------ *
     * Canvas Drawing                                               *
     * ------------------------------------------------------------ */

    TFile* outFile = new TFile(Form("./rootFiles/PlotModel_%d.root", nEvents), "RECREATE");
    outFile->cd();

    // _________________ Basic Canvas : pT, eta, phi _________________
    TCanvas* cBasic = new TCanvas("cBasic", "cBasic", 1000, 500);
    cBasic->Divide(3, 1); Int_t iPad = 1;

    const std::string xLab = "p_{T} (GeV/c)";
    const std::string yLab = "#frac{1}{2#pi p_{T}N_{evt}}#frac{d^{2}N_{ch}}{dp_{T}d#eta} (GeV/c)^{-2}";
    Draw(hPt,  cBasic, iPad, colors[0], xLab.data(), yLab.data(), "SetLogy");
    Draw(hEta, cBasic, iPad, colors[0], "#eta", "dN/d#eta");
    Draw(hPhi, cBasic, iPad, colors[0], "#varphi", "dN/d#varphi");

    for (int i = 1; i <= 3; i++) cBasic->cd(i)->SetLeftMargin(0.17);
    hPt->GetYaxis()->SetTitleOffset(1.8);
    hEta->GetYaxis()->SetTitleOffset(1.8);
    hPhi->GetYaxis()->SetTitleOffset(1.8);

    cBasic->WaitPrimitive();

    // _________________ {Phi;pT}-Eta Canvas _________________
    TCanvas* cPhiEta = new TCanvas("cPhiEta", "cPhiEta", 700, 700);
    cPhiEta->Divide(2, 1); iPad = 1;

    cPhiEta->cd(iPad++);
    tBkg->Draw("eta:pt", "", "colz");
    //Draw(hPtEta,  cPhiEta, iPad, colors[0], xLab.data(), "#eta", "SetLogx");
    Draw(hPhiEta, cPhiEta, iPad, colors[0], "#phi", "#eta");


    // Write canvases to output ROOT file. 
    cBasic->Write();
    cPhiEta->Write();
}

#ifndef __CINT__
int main(int argc, char* argv[]) 
{
    // Needed for incredibly obscure reason. 
    TApplication theAPP("App", &argc, argv);

    if (argc == 2) 
        PlotModel(std::atoi(argv[1]));
    else
        PlotModel();
    return 0;
}
#endif

