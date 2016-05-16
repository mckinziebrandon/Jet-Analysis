#ifndef MYJETFINDER_H
#define MYJETFINDER_H
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Subtractor.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include <string>

using namespace fastjet;
using std::vector;
using std::string;

static const Float_t R = 0.3;
static const Int_t Remove_N_hardest = 0;

class MyJetFinder {
    public:
        MyJetFinder();
        ~MyJetFinder();
        void    FindJets(vector<PseudoJet> event);
        Int_t   GetNumJets();
        void    PrintJets();
        void    Clear();
        void    Write(TString fileName);
    private:
        Double_t rho, sigma;
        /* jetDef contains a full specification of _how_ to carry out
         * the clustering. For now, only specify anti-kt algorithm and
         * desired jet radius R. Use default recombination scheme and strategy. */
        JetDefinition 		jetDef;
        /* Underlying data structures for jet information are TNtuples. */
        TNtuple* 			tNJets;
        TNtuple* 			tJetInfo;
        /* clusterSequence object is responsible for running the jet clustering. */
        ClusterSequenceArea* 	clusterSequence;
        /* Store vector of particles wrapped as PseudoJets before any clustering occurs. */
        vector<PseudoJet> 	particlesVector;
        /* Store vector of PseudoJets built by FastJet during clustering procedure. */
        vector<PseudoJet> 	jetsVector;
};

#endif
