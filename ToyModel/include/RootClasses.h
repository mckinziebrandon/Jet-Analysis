#ifndef ROOTCLASSES_H
#define ROOTCLASSES_H

#include "TBranch.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TGenPhaseSpace.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2.h"
#include "TH2F.h"
#include "TLatex.h"
#include "TMath.h"
#include "TPad.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TString.h"
#include "TTree.h"

TString HistName;
TCanvas* MyC;
TPad* pad_1;
TPad* pad_2;

void SetDrawOptions(TH1F* h, TString x_label, TString  y_label);

void SetDrawOptions(TH1F* h, TString x_label, TString  y_label)
{
    h->SetStats(0);

    h->SetFillColor(kBlue+3);
    h->SetLineColor(kBlue+3);

    h->SetLineWidth(2);
    h->SetFillStyle(3365);

    h->SetTitle("");
    h->GetXaxis()->SetTitle(x_label.Data());
    h->GetXaxis()->SetTitleSize(0.05);

    h->GetYaxis()->SetTitle(y_label.Data());
    h->GetYaxis()->SetTitleOffset(1.3);
    h->GetYaxis()->SetTitleSize(0.05);

    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->CenterTitle();
}





#endif
