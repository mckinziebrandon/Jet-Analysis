#ifndef MYJETFINDER_H
#define MYJETFINDER_H
#include "RootClasses.h"
#include "fastjet/ClusterSequence.hh"
#include <string>

using namespace fastjet;
using std::vector;
using std::string;

const Int_t nEmbed = 3;
const Float_t R = 0.2;
const Float_t ptmin = 20.0;

class MyJetFinder: public EventModel {
public:
    MyJetFinder();
    ~MyJetFinder();
    void FindJets();
    void GenerateParticle();
    void FindJetsInEvent();
    void PrintJetsInEvent();
    void EmbedJetsInEvent();
    const Int_t GetNumEmbed();
    void Write(TString fileName);
	Int_t GetNumJetsInEvent();
    Double_t GetTrackPt();
    void ResetEventVariables();
private:
    // Full-model objects.
    JetDefinition 		jetDef;
    ClusterSequence* 	clusterSequence;
    vector<PseudoJet> 	particlesVector;
    vector<PseudoJet> 	jetsVector;
    // Per-event objects.
    TNtuple* 			t_nJetReco;
    TNtuple* 			t_ptJetReco;
    ClusterSequence* 	eventClusterSequence;
    vector<PseudoJet> 	eventParticlesVector;
    vector<PseudoJet> 	eventJetsVector;
};

#endif
