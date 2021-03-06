#ifndef EVENTMODEL_H
#define EVENTMODEL_H
// User-defined headers.
#include "RootClasses.h"
#include "EventFunctions.h"
#include "Printer.h"
// C++ headers.
#include <string>
#include <vector>
// Other external libraries.
#include "TSpline.h"
#include "Pythia8/Pythia.h" // new
#include "fastjet/ClusterSequence.hh"
using namespace fastjet;
using namespace Pythia8; // new

// ---------- Constants ----------
static const Float_t etaMax = 0.9;
static const Float_t ptMin = 0.5;
static const Float_t ptMax = 20.0;
static const Float_t sigmaDeltaPhi = (pi / 4) / 2;
static const Float_t trigPtThreshold = 5.0;
// -------------------------------

class EventGenerator {
    friend class MyJetFinder;
    public:
        EventGenerator();
        ~EventGenerator();
        /* Generate N number of particles of type STR, where STR can be either
           "trig" or "bkg" */
        Float_t Generate(const string& str, Int_t n=1);

        /* ---------------------------------------------------------------- *
         * Getters                                                          *
         * ---------------------------------------------------------------- */
        vector<PseudoJet> GetLastEvent();
        /* Returns phi value centered at pi from trigPhi with Gaussian spread. */ 
        Float_t GetAssocPhi(Float_t trigPhi);
        /* Returns event multiplicity defined by centrality variable. */
        Float_t GetMultiplicity(); // TODO
        /* Sample randomly from v2(pt)-modulated phi distribution. */ 
        Float_t GetPhi(Float_t pt);
        /* Returns random number b/w -1 and 1, corresponding to ALICE acceptance. */
        Float_t GetRandEta();
        Double_t GetTrackPt(Float_t xMin=ptMin);
        /* Returns current value of recoMult. Should be called after event is generated. */
        Int_t GetRecoMult();

        /* ---------------------------------------------------------------- *
         * Setters                                                          *
         * ---------------------------------------------------------------- */
        void SetCentrality(int percent);
        // ---------- Miscellaneous ----------
        /* Writes and organizes all model information into fileName.root. */
        void Write(const char* fileName);
        /* Returns difference between inputs while constraining return value 
         * to be in [0., 2. * pi]; */
        static Float_t dphi(Float_t p, Float_t p2);

    protected:
        /* ---------------------------------------------------------------- *
         * Data Containers                                                  *
         * ---------------------------------------------------------------- */
        Float_t centrality, multiplicity;
        Float_t eta, phi, pt;
        Int_t recoMult;
        Int_t effVars[4];
        TTree* tTrig;
        TTree* tAssoc;
        TTree* tBkg;
        THn* hEff;
        TSpline* spline;

        /* ---------------------------------------------------------------- *
         * Helper Methods/Objects                                           *
         * ---------------------------------------------------------------- */
        /* Random number generator */
        TRandom3* rand;
        /* The 'functions' variable has access to all sampling distributions the user
         * may wish to use/access throughout the model. */
        EventFunctions* functions;
        /* Depending on value of pt, evaluates either 
         * polynomial or linear fit to the raw pt data distribution. */
        Float_t GetV2(Float_t pt);
        /* Returns efficiency from 4-d histogram based on current track values. */
        Float_t GetEfficiency();
        /* Wraps current values of pt, eta, phi in a PseudoJet and returns it. */
        PseudoJet GetPseudoJet();

    private:
        /* Create the underlying data trees to store particle information.
         * Each function simply creates a tree with pt, eta, phi branches and returns it. */
        TTree* InitializeTrigger();
        TTree* InitializeAssoc();
        TTree* InitializeBackground();
};

#endif
