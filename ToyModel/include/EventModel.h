#ifndef EVENTMODEL_H
#define EVENTMODEL_H
#include "RootClasses.h"
#include "EventFunctions.h"
#include <string>

const int trig = 0;
const int assoc = 1;
const int bkg = 2;


// ---------- Constants ----------
const Float_t pi    = TMath::Pi();
const Int_t nBkg                = 100;
const Int_t nParticles			= 1000;
const Int_t parton_mass         = 0;
const Float_t sigmaDeltaPhi     = (pi / 4) / 2;
const Float_t trigPtThreshold = 5.0;
// -------------------------------

class EventModel 
{
public:
    // Constructors / Destructors.
    EventModel();
    ~EventModel();
    /* Samples from pt distribution to determine whether or not trigger particle. 
     * If sampled pt > trigPtThreshold, then designated as a trigger and also
     * artificically embed a high-pt "associated" object opposite to it in eta. 
     * Otherwise, designate as background and fill tBkg with pt, eta, phi. */
    void GenerateParticle();
    /* Generate N number of particles of type STR, where STR can be either
       "trig" or "bkg" */
    Float_t Generate(const string& str, Int_t n);
    /* Sample randomly from v2(pt)-modulated phi distribution. */ 
    Float_t GetPhi(Float_t pt);
    /* Returns phi value centered at pi from trigPhi with Gaussian spread. */ 
    Float_t GetAssocPhi(Float_t trigPhi);
    /* Returns random number b/w -1 and 1, corresponding to ALICE acceptance. */
    Float_t GetRandEta();
    /* Returns random sample from fits to raw data pt distribution. */
    Double_t GetTrackPt(Float_t xMin=0.2);
    /* Writes and organizes all model information into fileName.root. */
    void Write(TString fileName);
    /* Returns difference between inputs while constraining return value 
     * to be in [0., 2. * pi]; */
    static Float_t dphi(Float_t p, Float_t p2);
protected:
    /* Random number generator */
    TRandom3* rand;
    /* The 'functions' variable has access to all sampling distributions the user
     * may wish to use/access throughout the model. */
    EventFunctions* functions;
    /* Depending on value of pt, evaluates either 
     * polynomial or linear fit to the raw pt data distribution. */
    Float_t GetV2(Float_t pt);
    /* Create the underlying data trees to store particle information.
     * Each function simply creates a tree with pt, eta, phi branches and returns it. */
    Float_t eta, phi, pt;
    TTree* tTrig;
    TTree* InitializeTrigger();
    TTree* tAssoc;
    TTree* InitializeAssoc();
    TTree* tBkg;
    TTree* InitializeBackground();
};

#endif
