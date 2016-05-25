#ifndef ROOTCLASSES_H
#define ROOTCLASSES_H
// C++ header files. 
#include <iostream>
#include <string>
#include <vector>
#include <utility> // std::pair, std::make_pair
// ROOT header files.
#include "TApplication.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TCut.h"
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
#include "TStyle.h"
#include "TTree.h"

// TypeDef: vector of (object, description) pairs for plot legends.
typedef std::vector<std::pair<TObject*, const char*>> LegendList;

// ____ Using directives. ____
using std::cout;
using std::endl;

// ________ Point class to hold coordinate (x, y). ________
class Point {
    public:
        Point(float x, float y) { X = x; Y = y; }
        float X;
        float Y;
};

// ____________________________________ CONSTANTS __________________________________________________
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

// _________________________________ Histogram Drawing _________________________________
template<typename Hist>
void Draw(Hist* h, TCanvas* c, Int_t& iPad, Color_t col, 
            const char* xLabel="", 
            const char* yLabel="", 
            const char* SetLog="");

template<typename Hist>
void SetDrawOptions(Hist* h, Color_t col=colors[1], const char* x_label="", const char* y_label="");
template<typename Hist>
void Draw(Hist* hModel, Hist* hData, TCanvas* c, Int_t& iPad);
void DrawRatio(TH1* hModDat, TH1* hModSamp, TCanvas* c);

// __________________________ Non-Histogram Drawing ______________________________
void Draw(TGraph* g, TCanvas* c, Int_t& iPad);
TLine* DrawLine(Point p1, Point p2, TPad* pad);
void DrawLegend(TH1* hModel, TH1* hData, TGraph* g, TGraph* g2, TCanvas* c, Int_t& iPad);
void DrawLegend(LegendList, TPad*);


// Draws the histogram on iPad of canvas, increments iPad by 1, and sets the draw options. 
template<typename Hist>
void Draw(Hist* h, TCanvas* c, Int_t& iPad, Color_t col, const char* xLabel, const char* yLabel, const char* SetLog) {
    if      (SetLog ==  "SetLogx") c->cd(iPad)->SetLogx(1);
    else if (SetLog ==  "SetLogy") c->cd(iPad)->SetLogy(1);
    else if (SetLog ==  "SetLogz") c->cd(iPad)->SetLogz(1);
    c->cd(iPad)->SetTicks(1, 1);
    c->cd(iPad++);
    h->Draw("same");
    SetDrawOptions(h, col, xLabel, yLabel);
}

template<typename Hist>
void SetDrawOptions(Hist* h, Color_t col=colors[1], const char* x_label="", const char* y_label="") {
    h->SetStats(0);
    // Distinctions b/w 1D and 2D hists.
    if (h->InheritsFrom(TH2::Class())) {
        h->SetDrawOption("COLZ1");
    } else {
        // Line and fill properties.
        h->SetFillColor(col);
        h->SetLineColor(col);
        h->SetLineWidth(2);
        h->SetFillStyle(0);
        // Marker Properties.
        h->SetMarkerColor(col);
        h->SetMarkerStyle(kFullStar);
    }
    // Axis properties.
    h->SetTitle("");
    if (strcmp(x_label, "")) h->GetXaxis()->SetTitle(x_label);
    if (strcmp(y_label, "")) h->GetYaxis()->SetTitle(y_label);
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleOffset(1.3);
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

#endif
