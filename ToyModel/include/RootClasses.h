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


// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------

template<typename Hist>
void SetDrawOptions(Hist* h, TString x_label="", TString  y_label="");
template<typename Hist>
TCanvas* DrawHistogram(Hist* h);
TCanvas* DrawHists(const Int_t n, TString index, TString names[], TTree* tree);

#endif
