#ifndef EVENTMODEL_H
#define EVENTMODEL_H
#include "RootClasses.h"
#include "EventFunctions.h"

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

class EventModel 
{
private:
    Float_t eta, phi, pt;
    // Event switches.
    Bool_t has_bkg;
    Bool_t has_V2;
    // Instance objects.
    TTree* t_trig;
    TTree* t_assoc;
    TTree* t_bkg;
    EventFunctions* functions;
    TRandom3* rand;
    // Private helper methods.
    Float_t GetV2(Float_t pt);
    TTree* InitializeBackground();
    TTree* InitializeJet();
    TTree* InitializeTrigger();
public:
    // Constructors / Destructors.
    EventModel();
    ~EventModel();
    // Setters.
    void GenerateTrigger();
    void GenerateJet();
    void GenerateBackground();
    // Getters.
    Float_t GetTriggerPhi(Float_t pt);
    Float_t GetAssocPhi(Float_t pt, Float_t tphi, Float_t sigma);
    Float_t GetRandEta();
    Double_t GetTrackPt();
    // Miscellaneous. 
    Float_t dphi(Float_t p, Float_t p2); 
    void Write();
};

#endif
