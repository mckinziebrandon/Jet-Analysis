#ifndef MYJETFINDER_H
#define MYJETFINDER_H
#include "RootClasses.h"
#include "fastjet/ClusterSequence.hh"
#include <string>

using namespace fastjet;
using std::vector;
using std::string;

const Float_t R     = 0.2;
const Float_t ptmin = 20.0;

class MyJetFinder: public EventModel {
public:
    MyJetFinder();
    ~MyJetFinder();
    void        FindJets();
    void        GenerateParticle();
	Int_t       GetNumJets();
    Double_t    GetTrackPt();
    void        PrintJetsInEvent();
    void        ResetEventVariables();
    void        Write(TString fileName);
private:
    /* jetDef contains a full specification of _how_ to carry out
     * the clustering. For now, only specify anti-kt algorithm and
     * desired jet radius R. Use default recombination scheme and strategy. */
    JetDefinition 		jetDef;
    /* Underlying data structures for jet information are TNtuples. */
    TNtuple* 			t_nJetReco;
    TNtuple* 			t_ptJetReco;
    /* clusterSequence object is responsible for running the jet clustering. */
    ClusterSequence* 	clusterSequence;
    /* Store vector of particles wrapped as PseudoJets before any clustering occurs. */
    vector<PseudoJet> 	particlesVector;
    /* Store vector of PseudoJets built by FastJet during clustering procedure. */
    vector<PseudoJet> 	jetsVector;
};

#endif
