#ifndef EVENTMODEL_H
#define EVENTMODEL_H
#include "RootClasses.h"

const Int_t trig = 0;
const Int_t assoc = 1;
const Int_t bkg = 2;

// ---------- Constants ----------
const Float_t pi    = TMath::Pi();
const Int_t nEvents             = 100000;
const Int_t nBkg                = 10;
const Int_t nCanvas             = 3;
const Int_t trig_pt_threshold   = 1;
const Int_t parton_mass         = 0;
const Float_t sigma_dphi        = (pi / 4) / 2;
// -------------------------------

class EventModel 
{
private:
    // Instance objects.
    TF1* fPolynomial;
    TF1* fLine;
    TF1* fdNdPhi;
    TF1* fTrackSpectrum;
    TRandom3* rand;
    // Private methods.
    TF1* GetfPolynomial();
    TF1* GetfLine();
    TF1* GetfdNdPhi();
    TF1* GetfSpectrum();
public:
    EventModel();
    ~EventModel();
    Float_t dphi(Float_t p, Float_t p2); 
    Float_t GetAssocPhi(Float_t tp, Float_t sp);
    Float_t GetRandEta();
    Float_t GetTriggerPhi(Float_t pt);
    Double_t GetTrackPt();
};

#endif
