#ifndef EVENTMODEL_H
#define EVENTMODEL_H
#include "RootClasses.h"
#include "EventFunctions.h"
#include <string>

// --- Identifiers ---
const Int_t trig    = 0;
const Int_t assoc   = 1;
const Int_t bkg     = 2;
// -------------------

// ---------- Constants ----------
const Float_t pi    = TMath::Pi();
const Int_t nBkg                = 100;
const Int_t nParticles			= 1000;
const Int_t nCanvas             = 3;
const Int_t trig_pt_threshold   = 1;
const Int_t parton_mass         = 0;
const Float_t sigma_dphi        = (pi / 4) / 2;
const Float_t triggerThreshold = 5.0;
// -------------------------------

class EventModel 
{
public:
    // Constructors / Destructors.
    EventModel();
    ~EventModel();
    // Setters.
    void GenerateParticle();
    void NewEvent();
    // Getters.
    Float_t GetTriggerPhi(Float_t pt);
    Float_t GetAssocPhi(Float_t pt, Float_t tphi, Float_t sigma);
    Float_t GetRandEta();
    Double_t GetTrackPt();
    // Miscellaneous. 
    static Float_t dphi(Float_t p, Float_t p2);
    void Write(TString fileName);
protected:
	Bool_t haveTrigger = false; // flag per-event if found trigger particle yet.
    Float_t eta, phi, pt;
    // Instance objects.
    TTree* t_trig;
    TTree* t_assoc;
    TTree* t_bkg;
    TRandom3* rand;
    // User-defined EventFunctions.
    EventFunctions* functions;
    // Private helper methods.
    Float_t GetV2(Float_t pt);
    TTree* InitializeBackground();
    TTree* InitializeJet();
    TTree* InitializeTrigger();
};

#endif
