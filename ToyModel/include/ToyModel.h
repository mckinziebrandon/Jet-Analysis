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

// Prototypes that return a numerical value.
Float_t dphi(Float_t p, Float_t p2); 
Float_t GetTriggerPhi(Float_t pt, TF1* f);
Float_t GetAssocPhi(Float_t tp, Float_t sp, TRandom3* r);

// Prototypes that return some function.
TF1* dNdPhi();
TF1* GetSpectrum();

extern TF1* fdNdPhi;
extern TF1* fTrackSpectrum;

// Small inline functions. 
inline Double_t GetTrackPt() { return fTrackSpectrum->GetRandom(); }

#endif
