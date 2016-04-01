#ifndef ROOTCLASSES_H
#define ROOTCLASSES_H
#include <iostream>
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
#include "THStack.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TPad.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TROOT.h"
#include "TString.h"
#include "TTree.h"

template<typename Hist>
void SetDrawOptions(Hist* h, Color_t col, TString x_label="", TString  y_label="") {
    h->SetStats(0);
    h->SetFillColor(col);
    h->SetLineColor(col);
    h->SetLineWidth(2);
    h->SetFillStyle(0);
    if (x_label != "" && y_label != "")
    {
        h->SetTitle("");
        h->GetXaxis()->SetTitle(x_label.Data());
        h->GetYaxis()->SetTitle(y_label.Data());
    }
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleOffset(1.2);
    h->GetYaxis()->SetTitleSize(0.05);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->CenterTitle();
}

// Draws histogram in its own canvas and sets personal drawing options. 
template<typename Hist>
TCanvas* DrawHistogram(Hist* h)
{
    TCanvas* c = new TCanvas("c", "canvas from DrawHistogram", 700, 500);
    c->cd();
    h->Draw();
    TString x = h->GetXaxis()->GetTitle();
    TString y = h->GetYaxis()->GetTitle();
    return c;
}

#endif
