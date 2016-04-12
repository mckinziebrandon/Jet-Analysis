#include "../include/EventGenerator.h"
#include "../include/MyJetFinder.h"

/* In addition to constructing the general EventGenerator(),
 * MyJetFinder creates TLorentzVectors that can interact with
 * the FastJet methods. */
MyJetFinder::MyJetFinder() {
    tNJets      = new TNtuple("tNJets", "Number of jets in event", "n");
    tJetInfo    = new TNtuple("tJetInfo", "pt of reco jets in event", "pt:eta:phi");
    // Create jet definition. Should not change.
    jetDef = JetDefinition(antikt_algorithm, R);
}

/* In addition to deleting ~EventGenerator(),
 * delete all jet finder objects. */
MyJetFinder::~MyJetFinder() {
}

/* Cluster on particlesVector and create jetsVector.*/
void MyJetFinder::FindJets(vector<PseudoJet> event) {
    particlesVector = event;
    clusterSequence = new ClusterSequence(particlesVector, jetDef);
    jetsVector      = sorted_by_pt(clusterSequence->inclusive_jets(/*ptmin*/));
    tNJets->Fill((Int_t) jetsVector.size());
    for (Int_t i = 0; i < jetsVector.size(); i++) {
    	tJetInfo->Fill(jetsVector[i].pt(), jetsVector[i].eta(), jetsVector[i].phi());
    }
}

Int_t MyJetFinder::GetNumJets() {
	return jetsVector.size();
}

void MyJetFinder::Clear() {
    particlesVector.clear();
    jetsVector.clear();
}

/*
 * Code snippet copy/paste from short_example.cc in fastjet documentation.
 */
void MyJetFinder::PrintJets() {
    // Print out some jet information.
    cout << "Clustering with " << jetDef.description() << endl;

    // Print the pt, y, and phi of each jet.
    cout << "Jet Index\t pt\t rapidity\t phi" << endl;
    for (unsigned i = 0; i < jetsVector.size(); i++) {
        cout << "Jet " << i << ":\t ";
        cout << jetsVector[i].pt() 	<< "\t ";
        cout << jetsVector[i].eta() << "\t ";
        cout << jetsVector[i].phi() << endl;

        // Print the pt for each constituent.
        vector<PseudoJet> constituents = jetsVector[i].constituents();
        for (unsigned j = 0; j < constituents.size(); j++) {
            cout << "Constituent " << j << "'s pt: " << constituents[j].pt() << endl;
        } 
    }
}

/*
 * Places a jetPlots directory in the root file created by EventGenerator::Write().
 */
void MyJetFinder::Write(TString fileName) {
    TFile* jetFile = new TFile(fileName.Data(), "UPDATE");

    // _____ Store all plots from jet finder. _____
    TDirectory* jetDir = jetFile->mkdir("jetPlots");
    jetDir->cd();
    jetDir->Add(tNJets, true);
    jetDir->Add(tJetInfo, true);

    jetFile->Write();
    delete jetFile;
}
