#ifndef EVENTMODEL_H
#define EVENTMODEL_H
#include "RootClasses.h"
#include "TObject.h"

const Int_t trig = 0;
const Int_t assoc = 1;
const Int_t bkg = 2;

// ---------- Constants ----------
const Float_t pi    = TMath::Pi();
const Int_t nEvents             = 200;
const Int_t nBkg                = 10;
const Int_t nCanvas             = 3;
const Int_t trig_pt_threshold   = 1;
const Int_t parton_mass         = 0;
const Float_t sigma_dphi        = (pi / 4) / 2;
// -------------------------------

class EventModel: public TObject {
private:
    // Event switches.
    Bool_t has_bkg;
    Bool_t has_V2;
    // Instance objects.
    TF1* fPolynomial;
    TF1* fLine;
    TF1* fdNdPhi;
    TF1* fTrackSpectrum;
    TRandom3* rand;
    // Private helper methods.
    TF1* GetfPolynomial();
    TF1* GetfLine();
    TF1* GetfdNdPhi();
    TF1* GetfSpectrum();
    Float_t GetV2(Float_t pt);
public:
    // Constructors / Destructors.
    EventModel() {has_bkg = true; has_V2 = true; };
    EventModel(Bool_t a, Bool_t b);
    ~EventModel();
    // Setters.
    void SetTrigger(Float_t &pt, Float_t &eta, Float_t &phi);
    void SetJet(Float_t &pt, Float_t &eta, Float_t &phi);
    void SetBackground(Float_t &pt, Float_t &eta, Float_t &phi);
    // Getters.
    Float_t GetTriggerPhi(Float_t pt);
    Float_t GetAssocPhi(Float_t pt, Float_t tphi, Float_t sigma);
    Float_t GetRandEta();
    Double_t GetTrackPt();
    // Miscellaneous. 
    Float_t dphi(Float_t p, Float_t p2); 

    ClassDef(EventModel, 6);
};

#endif
