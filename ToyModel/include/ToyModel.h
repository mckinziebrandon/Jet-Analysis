#ifndef TOYMODEL_H
#define TOYMODEL_H
#include "RootClasses.h"

const Int_t trig = 0;
const Int_t assoc = 1;
const Int_t bkg = 2;

// ---------- Constants ----------
const Float_t pi    = TMath::Pi();
const Int_t nEvents             = 10000;
const Int_t nBkg                = 10;
const Int_t nCanvas             = 3;
const Int_t trig_pt_threshold   = 1;
const Int_t parton_mass         = 0;
const Float_t sigma_dphi        = (pi / 4) / 2;
// -------------------------------

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------

// My functions.
Float_t dphi(Float_t p, Float_t p2); 
Float_t GetTriggerPhi(Float_t pt);
Float_t GetAssocPhi(Float_t tp, Float_t sp, TRandom3* r);
TF1* funcV2();
TF1* dNdPhi();
TF1* GetSpectrum();

TF1* fV2 = funcV2();
TF1* fdNdPhi = dNdPhi();
TF1* fTrackSpectrum = GetSpectrum();

Double_t GetTrackPt() { return fTrackSpectrum->GetRandom(); }
Float_t GetV2(Float_t pt) { return fV2->Eval(pt); }
// -----------------------------------------------------------------------------
// TF1 Function Definitions
// -----------------------------------------------------------------------------
#endif
