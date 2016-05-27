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

bool isFloat(const char* arg) {
    return typeid(atof(arg)) == typeid(double);
}
const std::string hJetLabels[5] = { "p_{T}^{jet,reco}", "#eta^{jet}", 
                                    "#varphi^{jet}",    "Area", 
                                    "p_{T}^{jet,reco} - #rho A"};

// List of jet legend items for both of these plots. 
LegendList legList  = MyJetFinder::GetJetParams();

TCanvas* GetJetBasic(TH1* hPt, TH1* hEta, TH1* hPhi);
TCanvas* GetJetInfo(TH1* hArea, TH1* hAreaCut, TH1* hPtSub, TH1* hPtSubWithACut);
TCanvas* GetEventInfo(TH1* hNJets, TH1* hRho, TH1* hSigma);
TCanvas* GetAreaPt(TH2* hAreaPt, TH2* hAreaPtSub);
TCanvas* GetPhiEta(TH2* hPhiEta, TH2* hPhiEtaWithACut);
TCanvas* GetPtEta(TH2* hPtEta, TH2* hPtSubEta, TH2* hPtEtaWithACut, TH2* hPtSubEtaWithACut);

// --------------------------------------------------------------------------------------------
void PlotFinder(Int_t nEvents = 1000, Float_t R = 0.3) {
    using std::cout;
    using std::endl;
    
    legList.push_back(std::make_pair((TObject*)0, Form("No. Events: %d", nEvents)));
    legList.push_back(std::make_pair((TObject*)0, Form("R = %.1f, anti-kT", R)));

    cout << "Plotting jet quantities from most recent Toy Model run of " 
         << nEvents << " number of events...." << endl;

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    // Grab phase space trees from ToyModel.C output. 
    const std::string fileName = Form("./rootFiles/TM_N%d_R%d.root", nEvents, (int) (R * 10));
    TFile* fTrees        = new TFile(fileName.data(), "READ");
    TDirectory* dirJets	 = (TDirectory*) fTrees->Get("jetPlots;1");
    TTree* tEventJetInfo = (TTree*) dirJets->Get("tEventJetInfo;1");
    TTree* tJetInfo      = (TTree*) dirJets->Get("tJetInfo;1");

    TFile* f_out = new TFile(Form("./rootFiles/PF_N%d_R%d.root", nEvents, (int) (R * 10)), "RECREATE");
    f_out->cd();

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


    // Create all canvases with plots.
    TCanvas* cJetBasic  = GetJetBasic(hPt, hEta, hPhi);
    TCanvas* cJetInfo   = GetJetInfo(hArea, hAreaCut, hPtSub, hPtSubWithACut);
    TCanvas* cEventInfo = GetEventInfo(hNJets, hRho, hSigma);
    TCanvas* cPhiEta    = GetPhiEta(hPhiEta, hPhiEtaWithACut);
    TCanvas* cPtEta     = GetPtEta(hPtEta, hPtSubEta, hPtEtaWithACut, hPtSubEtaWithACut);
    TCanvas* cAreaPt    = GetAreaPt(hAreaPt, hAreaPtSub);


    // Write all canvases to output file. 
    cout << "Writing canvases to " << f_out->GetName() << endl;
    cJetBasic->Write();
    cJetInfo->Write();
    cEventInfo->Write();
    cAreaPt->Write();
    cPhiEta->Write();
    cPtEta->Write();
    f_out->Write();

}

#ifndef __CINT__
int main(int argc, char* argv[]) 
{
    // Needed for incredibly obscure reason. 
    TApplication theAPP("App", &argc, argv);

    // Case 1: User only provides desired number of events. 
    if (argc == 2) {
       PlotFinder(atoi(argv[1]));
    // Case 2: User provides number of events and a radius.
    } else if (argc == 3 && isFloat(argv[2])) {
        PlotFinder(atoi(argv[1]), atof(argv[2]));
    // Case 3: User specifies no additional arguments. 
    } else {
        PlotFinder();
    }
    return 0;
}
#endif

// ___________ 1. Plot the basic jet pt, eta, phi variables. ___________ 
TCanvas* GetJetBasic(TH1* hPt, TH1* hEta, TH1* hPhi) {
    TCanvas* canvas = new TCanvas("cJetBasic", "canvas", 1000, 500);
    canvas->Divide(3, 1); Int_t iPad   = 1;
    Draw(hPt,  canvas, iPad, colors[iPad % 4], hJetLabels[0].data(), "",   "SetLogy"); 
    Draw(hEta, canvas, iPad, colors[iPad % 4], hJetLabels[1].data());
    Draw(hPhi, canvas, iPad, colors[iPad % 4], hJetLabels[2].data());
    return canvas;
}

// ___________ 2. Plot the jet area and ptSub variables. ___________ 
TCanvas* GetJetInfo(TH1* hArea, TH1* hAreaCut, TH1* hPtSub, TH1* hPtSubWithACut) {
    TCanvas* canvas = new TCanvas("cJetInfo", "canvas", 800, 500);
    canvas->Divide(2, 1); Int_t iPad = 1;
    canvas->cd(1)->SetLeftMargin(0.11);
    canvas->cd(1)->SetBottomMargin(0.12);
    canvas->cd(2)->SetBottomMargin(0.12);

    // 2.1. Draw distribution of jet areas, after cut on areaMin. 
    Double_t y2 = hArea->GetBinContent(hArea->FindBin(areaMin) + 1);
    Draw(hArea, canvas, iPad, kGreen - 5, hJetLabels[3].data());
    Draw(hAreaCut, canvas, --iPad, kBlue - 5, hJetLabels[3].data());

    legList.push_back(std::make_pair(hArea, "Below Acceptance"));
    legList.push_back(std::make_pair(hAreaCut, "Within Acceptance"));
    DrawLegend(legList, (TPad*) canvas->cd(1));
    legList.pop_back(); legList.pop_back();

    // 2.2. Draw background-subtracted pT plot.
    y2 = hPtSub->GetBinContent(hPtSub->FindBin(0.));
    Draw(hPtSub, canvas, iPad, kGreen - 5, hJetLabels[4].data(), "", "SetLogy");
    Draw(hPtSubWithACut, canvas, --iPad, kBlue - 5, hJetLabels[4].data(), "", "SetLogy");
    DrawLine(Point(0., 0.), Point(0., y2), (TPad*) canvas->cd(2));

    legList.push_back(std::make_pair(hPtSub, "Without Area Cut"));
    legList.push_back(std::make_pair(hPtSubWithACut, "With Area Cut"));
    DrawLegend(legList, (TPad*) canvas->cd(2));
    legList.pop_back(); legList.pop_back();

    return canvas;
}

// ___________ 3. Plot the per-event variables. ___________ 
TCanvas* GetEventInfo(TH1* hNJets, TH1* hRho, TH1* hSigma) {
    TCanvas* canvas = new TCanvas("cEventInfo", "canvas", 1000, 500);
    canvas->Divide(3, 1); Int_t iPad = 1;
    Draw(hNJets,    canvas, iPad, kBlue+3, "nJets (per event)");
    Draw(hRho,      canvas, iPad, kBlue+3, "#rho (per event)");
    Draw(hSigma,    canvas, iPad, kBlue+3, "#sigma (per event)");
    int p = 1; while (p <= 3) DrawLegend(legList, (TPad*) canvas->cd(p++));
    return canvas;
}

// ___________ 4. Plot 2-Dimensional jet area and ptSub variables. ___________ 
TCanvas* GetAreaPt(TH2* hAreaPt, TH2* hAreaPtSub) {
    TCanvas* canvas = new TCanvas("cAreaPt", "canvas", 900, 900);
    canvas->Divide(2, 1); Int_t iPad = 1;

    Draw(hAreaPt, canvas, iPad, kBlue, "Area", "p_{T}", "SetLogz");
    DrawLegend(legList, (TPad*) canvas->cd(iPad - 1));
    DrawLine(Point(areaMin, 0.), Point(areaMin, 50.), (TPad*) canvas->cd(iPad - 1));

    Draw(hAreaPtSub, canvas, iPad, kBlue, "Area", "p_{T} - #rho A", "SetLogz");
    DrawLine(Point(areaMin, 0.), Point(areaMin, 50.), (TPad*) canvas->cd(iPad - 1));
    DrawLegend(legList, (TPad*) canvas->cd(iPad - 1));
    return canvas;
}

// ___________ 5. Plot 2-Dimensional phi-eta. ___________ 
TCanvas* GetPhiEta(TH2* hPhiEta, TH2* hPhiEtaWithACut) {
    TCanvas* canvas = new TCanvas("cPhiEta", "canvas", 800, 600);
    canvas->Divide(2, 1); Int_t iPad = 1;

    Draw(hPhiEta, canvas, iPad, kBlue, "#varphi", "#eta");
    legList.push_back(std::make_pair((TObject*)0, "Without Area Cut"));
    DrawLegend(legList, (TPad*) canvas->cd(iPad - 1));
    legList.pop_back();

    Draw(hPhiEtaWithACut, canvas, iPad, kBlue, "#varphi", "#eta");
    legList.push_back(std::make_pair((TObject*)0, "With Area Cut"));
    DrawLegend(legList, (TPad*) canvas->cd(iPad - 1));
    legList.pop_back();
    return canvas;
}

// ___________ 6. Plot 2-Dimensional pt(Sub)-eta. ___________ 
TCanvas* GetPtEta(TH2* hPtEta, TH2* hPtSubEta, TH2* hPtEtaWithACut, TH2* hPtSubEtaWithACut) {
    TCanvas* canvas = new TCanvas("cPtEta", "canvas", 900, 900);
    canvas->Divide(2, 2); Int_t iPad = 1;

    Draw(hPtEta,    canvas, iPad, kBlue, "p_{T}", "#eta", "SetLogz");
    Draw(hPtSubEta, canvas, iPad, kBlue, "p_{T} - #rho A", "#eta", "SetLogz");
    Draw(hPtEtaWithACut,    canvas, iPad, kBlue, "p_{T}", "#eta", "SetLogz");
    Draw(hPtSubEtaWithACut, canvas, iPad, kBlue, "p_{T} - #rho A", "#eta", "SetLogz"); 
    return canvas;
}
