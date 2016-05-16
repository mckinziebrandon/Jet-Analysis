#include <iostream>
#include <string>
using namespace std;

#include "TBranch.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TGenPhaseSpace.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2.h"
#include "TH2F.h"
#include "THn.h"
#include "THStack.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TPad.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TROOT.h"
#include "TString.h"
#include "TTree.h"

#ifndef ROOTCLASSES_H
#define ROOTCLASSES_H

void DrawLegend(TH1* hModel, TH1* hData, TGraph* g, TGraph* g2, TCanvas* c, Int_t& iPad);
template<typename Hist>
void SetDrawOptions(Hist* h, Color_t col, const char* x_label="", const char* y_label="");
template<typename Hist>
void Draw(Hist* hModel, Hist* hData, TCanvas* c, Int_t& iPad);
template<typename Hist>
void Draw(Hist* h, const char* xLabel, const char* yLabel, Color_t col, TCanvas* c, Int_t& iPad);
template<typename Graph>
void Draw(Graph* g, TCanvas* c, Int_t& iPad);
void DrawRatio(TH1* hModDat, TH1* hModSamp, TCanvas* c);

// -------------------------------------------------------------------------------------------------
const Color_t colors[] = {36, 32, 221, 49};
const Style_t styles[] = {kOpenCircle, kOpenSquare};
// pt bins from data.           1           2           3           4           5
const Double_t xBins[] = {0.3, 0.325+0.025, 0.375+0.025, 0.425+0.025, 0.475+0.025, 0.525+0.025, // 1
                            0.575+0.025, 0.625+0.025, 0.675+0.025, 0.725+0.025, 0.775+0.025, // 2
                            0.825+0.025, 0.875+0.025, 0.925+0.025, 0.975+0.025, 1.05+0.0375,  // 3
                            1.15+0.05,  1.25+0.05,  1.35+0.05,  1.45+0.05,  1.55+0.05,  // 4
                            1.65+0.05,  1.75+0.05,  1.85+0.05,  1.95+0.05,  2.1+0.075,   // 5
                            2.3+0.1,   2.5+0.1,   2.7+0.1,   2.9+0.1,   3.1+0.1,   // 6
                            3.3+0.1,   3.5+0.1,   3.7+0.1,   3.9+0.1,   4.25+0.175, // 7
                            4.75+0.25,  5.25+0.25,  5.75+0.25,  6.25+0.25,  6.75+0.25, // 8
                            7.5+0.375,   8.5+0.5,   9.5+0.5,   10.5+0.5,  11.5+0.5, // 9
                            12.5+0.5,  13.5+0.5,  14.5+0.5,  15.5+0.5,  17.0+0.75, 19.0+1};
const int numBins = 51;
// -------------------------------------------------------------------------------------------------


template<typename Hist>
void SetDrawOptions(Hist* h, Color_t col, const char* x_label="", const char* y_label="") {
    h->SetStats(0);
    // Line and fill properties.
    h->SetFillColor(col);
    h->SetLineColor(col);
    h->SetLineWidth(2);
    h->SetFillStyle(0);
    // Marker properties.
    h->SetMarkerColor(col);
    h->SetMarkerStyle(styles[0]);
    // Axis properties.
    h->SetTitle("");
    h->GetXaxis()->SetTitle(x_label);
    h->GetYaxis()->SetTitle(y_label);
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleOffset(1.2);
    h->GetYaxis()->SetTitleSize(0.04);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->CenterTitle();
}

template<typename Hist>
void Draw(Hist* hModel, Hist* hData, TCanvas* c, Int_t& iPad) {
    if (iPad == 1) { c->cd(iPad)->SetLogy(1); }
    c->cd(iPad);
    hModel->Draw("same");
    hData->Draw("same");

}


void DrawLegend(TH1* hModel, TH1* hData, TGraph* g, TGraph* g2, TCanvas* c, Int_t& iPad) {
    c->cd(iPad);
    TLegend* legPt = new TLegend(0.6, 0.8, 0.9, 0.9);
    legPt->AddEntry(hModel, "Toy Model", "l");
    legPt->AddEntry(hData, "ALICE (Data)", "l");
    legPt->AddEntry(g, "ALICE (Paper)", "p");
    legPt->AddEntry(g2, "CMS (Paper)", "p");
    legPt->SetBorderSize(0);
    legPt->Draw();
}


// Draws the histogram on iPad of canvas, increments iPad by 1, and sets the draw options. 
template<typename Hist>
void Draw(Hist* h, const char* xLabel, const char* yLabel, Color_t col, TCanvas* c, Int_t& iPad) {
    //if (iPad == 1 || iPad == 4) { c->cd(iPad)->SetLogy(1); } // so janky
    c->cd(iPad++);
    h->Draw();
    SetDrawOptions(h, col, xLabel, yLabel);
}

template<typename Graph>
void Draw(Graph* g, TCanvas* c, Int_t& iPad) {
    c->cd(iPad);
    g->Draw("AP");
    g->GetXaxis()->CenterTitle();
    g->GetYaxis()->CenterTitle();
    g->SetMarkerColor(colors[3]);
    g->SetMarkerStyle(styles[0]);
}

void DrawRatio(TH1* hModDat, TH1* hModSamp, TCanvas* c) {
    c->cd(2)->SetLogy(0);
    c->cd(2);
    //hModDat->Draw("same");
    hModSamp->Draw("same");

    //SetDrawOptions(hModDat, colors[0], (TString) "p_{T} (GeV/c)", (TString) "p_{T} Ratio");
    SetDrawOptions(hModSamp, colors[0], (TString) "p_{T} (GeV/c)", (TString) "p_{T} Ratio");
    hModDat->SetFillColor(colors[1]);    //   hModDat->SetFillStyle(3001);
    hModSamp->SetFillColor(colors[3]);  //hModSamp->SetFillStyle(3001);

    TLegend * legRatio = new TLegend(0.6, 0.8, 0.9, 0.9);
    //legRatio->AddEntry(hModDat, "Model / Eff-Corrected Data", "lf");
    legRatio->AddEntry(hModSamp, "ALICE (Data) / ALICE (Paper)", "lf");
    legRatio->SetBorderSize(0);
    legRatio->Draw();
}

#endif
