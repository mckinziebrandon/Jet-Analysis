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
#include "./include/MyJetFinder.h"

// --------------------------------------------------------------------------------------------
void PlotFinder(Int_t nEvents = 1000) {
    using std::cout;
    using std::endl;
    
    cout << "Plotting jet quantities from most recent Toy Model run of " 
         << nEvents << " number of events...." << endl;


    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Grab phase space trees from ToyModel.C output. 
    TFile* fTrees           = new TFile(Form("./rootFiles/ToyModel_%d.root", nEvents), "READ");
    TDirectory* dirJets	    = (TDirectory*) fTrees->Get("jetPlots;1");
    TTree* tEventJetInfo    = (TTree*) dirJets->Get("tEventJetInfo;1");
    TTree* tJetInfo         = (TTree*) dirJets->Get("tJetInfo;1");

    /* ---------------------------------------------------- *
     * Extract desired plots from trees into histograms.    *
     * ---------------------------------------------------- */

    // Event-by-event jet properties. 
    tEventJetInfo->Draw("n>>hNJets");       
    tEventJetInfo->Draw("rho>>hRho");
    tEventJetInfo->Draw("sigma>>hSigma");
    TH1* hNJets = (TH1F*) gDirectory->Get("hNJets");
    TH1* hRho   = (TH1F*) gDirectory->Get("hRho");
    TH1* hSigma = (TH1F*) gDirectory->Get("hSigma");

    // 1D Histograms; WITHOUT area cut. 
    tJetInfo->Draw("area>>hArea", Form("area <= %f", areaMin)); // ish
    tJetInfo->Draw("ptSub>>hPtSub");
    tJetInfo->Draw("pt>>hPt");
    tJetInfo->Draw("eta>>hEta", "ptSub>0.0");
    tJetInfo->Draw("phi>>hPhi");
    TH1* hPt            = (TH1F*) gDirectory->Get("hPt");
    TH1* hEta           = (TH1F*) gDirectory->Get("hEta");
    TH1* hPhi           = (TH1F*) gDirectory->Get("hPhi");
    TH1* hArea  = (TH1F*) gDirectory->Get("hArea");
    TH1* hPtSub = (TH1F*) gDirectory->Get("hPtSub");

    // 2D Histograms; WITHOUT area cut. 
    tJetInfo->Draw("eta:phi>>hPhiEta");
    tJetInfo->Draw("eta:pt>>hPtEta");
    tJetInfo->Draw("eta:ptSub>>hPtSubEta");
    tJetInfo->Draw("pt:area>>hAreaPt");
    tJetInfo->Draw("ptSub:area>>hAreaPtSub");
    TH2* hPhiEta    = (TH2F*) gDirectory->Get("hPhiEta");
    TH2* hPtEta     = (TH2F*) gDirectory->Get("hPtEta");
    TH2* hPtSubEta  = (TH2F*) gDirectory->Get("hPtSubEta");
    TH2* hAreaPt    = (TH2F*) gDirectory->Get("hAreaPt");    
    TH2* hAreaPtSub = (TH2F*) gDirectory->Get("hAreaPtSub");

    // 1D Histograms; WITH area cut. 
    TCut areaCut = Form("area > %f", areaMin);
    tJetInfo->Draw("area>>hAreaCut", areaCut);
    tJetInfo->Draw("ptSub>>hPtSubWithACut", areaCut);
    TH1* hAreaCut       = (TH1F*) gDirectory->Get("hAreaCut");
    TH1* hPtSubWithACut = (TH1F*) gDirectory->Get("hPtSubWithACut");

    // 2D Histograms; WITH area cut. 
    tJetInfo->Draw("eta:phi>>hPhiEtaWithACut", areaCut);
    tJetInfo->Draw("eta:pt>>hPtEtaWithACut", areaCut);
    tJetInfo->Draw("eta:ptSub>>hPtSubEtaWithACut", areaCut);
    TH2* hPhiEtaWithACut    = (TH2F*) gDirectory->Get("hPhiEtaWithACut");    
    TH2* hPtEtaWithACut     = (TH2F*) gDirectory->Get("hPtEtaWithACut");    
    TH2* hPtSubEtaWithACut  = (TH2F*) gDirectory->Get("hPtSubEtaWithACut");

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TFile* f_out = new TFile(Form("./rootFiles/PlotFinder_%d.root", nEvents), "RECREATE");
    f_out->cd();

    //const Float_t pi = (Float_t) TMath::Pi();
    const std::string hJetLabels[5] = { "p_{T}^{jet,reco}", 
                                        "#eta^{jet}", 
                                        "#varphi^{jet}", 
                                        "Area", 
                                        "p_{T}^{jet,reco} - #rho A"};

    
    // =====================================================================
    // ___________ 1. Plot the basic jet pt, eta, phi variables. ___________ 
    TCanvas* cJetBasic = new TCanvas("cJetBasic", "cJetBasic", 1000, 500);
    cJetBasic->Divide(3, 1);
    Int_t iPad   = 1;
    Draw(hPt,  cJetBasic, iPad, colors[iPad % 4], hJetLabels[0].data(), "",   "SetLogy"); 
    Draw(hEta, cJetBasic, iPad, colors[iPad % 4], hJetLabels[1].data());
    Draw(hPhi, cJetBasic, iPad, colors[iPad % 4], hJetLabels[2].data());

    // ================================================================= 
    // ___________ 2. Plot the jet area and ptSub variables. ___________ 
    TCanvas* cJetInfo = new TCanvas("cJetInfo", "cJetInfo", 800, 500);
    cJetInfo->Divide(2, 1); iPad = 1;
    cJetInfo->cd(1)->SetLeftMargin(0.11);
    cJetInfo->cd(1)->SetBottomMargin(0.12);
    cJetInfo->cd(2)->SetBottomMargin(0.12);

    // List of jet legend items for both of these plots. 
    LegendList legList  = MyJetFinder::GetJetParams();
    legList.push_back(std::make_pair((TObject*)0, Form("No. Events: %d", nEvents)));

    // 2.1. Draw distribution of jet areas, after cut on areaMin. 
    Double_t y2 = hArea->GetBinContent(hArea->FindBin(areaMin) + 1);
    Draw(hArea, cJetInfo, iPad, kGreen - 5, hJetLabels[3].data());
    Draw(hAreaCut, cJetInfo, --iPad, kBlue - 5, hJetLabels[3].data());

    legList.push_back(std::make_pair(hArea, "Below Acceptance"));
    legList.push_back(std::make_pair(hAreaCut, "Within Acceptance"));
    DrawLegend(legList, (TPad*) cJetInfo->cd(1));
    legList.pop_back(); legList.pop_back();

    // 2.2. Draw background-subtracted pT plot.
    y2 = hPtSub->GetBinContent(hPtSub->FindBin(0.));
    Draw(hPtSub, cJetInfo, iPad, kGreen - 5, hJetLabels[4].data(), "", "SetLogy");
    Draw(hPtSubWithACut, cJetInfo, --iPad, kBlue - 5, hJetLabels[4].data(), "", "SetLogy");
    DrawLine(Point(0., 0.), Point(0., y2), (TPad*) cJetInfo->cd(2));

    legList.push_back(std::make_pair(hPtSub, "Without Area Cut"));
    legList.push_back(std::make_pair(hPtSubWithACut, "With Area Cut"));
    DrawLegend(legList, (TPad*) cJetInfo->cd(2));
    legList.pop_back(); legList.pop_back();

    // Wait until user double-clicks the canvas before continuing.
    //cJetInfo->WaitPrimitive();

    // ========================================================
    // ___________ 3. Plot the per-event variables. ___________ 
    TCanvas* cEventInfo = new TCanvas("cEventInfo", "cEventInfo", 1000, 500);
    cEventInfo->Divide(3, 1);
    iPad = 1;
    Draw(hNJets,    cEventInfo, iPad, kBlue+3, "nJets (per event)");
    Draw(hRho,      cEventInfo, iPad, kBlue+3, "#rho (per event)");
    Draw(hSigma,    cEventInfo, iPad, kBlue+3, "#sigma (per event)");
    int p = 1; while (p <= 3) DrawLegend(legList, (TPad*) cEventInfo->cd(p++));

    // =========================================================================== 
    // ___________ 4. Plot 2-Dimensional jet area and ptSub variables. ___________ 
    TCanvas* cAreaPt = new TCanvas("cAreaPt", "cAreaPt", 900, 900);
    cAreaPt->Divide(2, 1);
    iPad = 1;

    y2 = hPtSub->GetBinContent(hPtSub->FindBin(0.));
    Draw(hAreaPt, cAreaPt, iPad, kBlue, "Area", "p_{T}", "SetLogz");
    DrawLegend(legList, (TPad*) cAreaPt->cd(iPad - 1));
    DrawLine(Point(areaMin, 0.), Point(areaMin, 50.), (TPad*) cJetInfo->cd(iPad - 1));

    Draw(hAreaPtSub, cAreaPt, iPad, kBlue, "Area", "p_{T} - #rho A", "SetLogz");
    DrawLine(Point(areaMin, 0.), Point(areaMin, 50.), (TPad*) cJetInfo->cd(iPad - 1));
    DrawLegend(legList, (TPad*) cAreaPt->cd(iPad - 1));

    // ====================================================== 
    // ___________ 5. Plot 2-Dimensional phi-eta. ___________ 
    TCanvas* cPhiEta = new TCanvas("cPhiEta", "cPhiEta", 800, 600);
    cPhiEta->Divide(2, 1); iPad = 1;

    Draw(hPhiEta, cPhiEta, iPad, kBlue, "#varphi", "#eta");
    legList.push_back(std::make_pair((TObject*)0, "Without Area Cut"));
    DrawLegend(legList, (TPad*) cPhiEta->cd(iPad - 1));
    legList.pop_back();

    Draw(hPhiEtaWithACut, cPhiEta, iPad, kBlue, "#varphi", "#eta");
    legList.push_back(std::make_pair((TObject*)0, "With Area Cut"));
    DrawLegend(legList, (TPad*) cPhiEta->cd(iPad - 1));
    legList.pop_back();


    // ========================================================== 
    // ___________ 6. Plot 2-Dimensional pt(Sub)-eta. ___________ 
    TCanvas* cPtEta = new TCanvas("cPtEta", "cPtEta", 900, 900);
    cPtEta->Divide(2, 2); iPad = 1;

    Draw(hPtEta,    cPtEta, iPad, kBlue, "p_{T}", "#eta", "SetLogz");
    Draw(hPtSubEta, cPtEta, iPad, kBlue, "p_{T} - #rho A", "#eta", "SetLogz");
    Draw(hPtEtaWithACut,    cPtEta, iPad, kBlue, "p_{T}", "#eta", "SetLogz");
    Draw(hPtSubEtaWithACut, cPtEta, iPad, kBlue, "p_{T} - #rho A", "#eta", "SetLogz");

    cPtEta->WaitPrimitive();

    // ================================== 
    // Write all canvases to output file. 
    cout << "Writing canvases to " << f_out->GetName() << endl;
    cJetBasic->Write();
    cJetInfo->Write();
    cEventInfo->Write();
    cAreaPt->Write();
    cPhiEta->Write();
    cPtEta->Write();
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


