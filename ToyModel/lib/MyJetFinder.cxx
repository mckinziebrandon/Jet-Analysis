#include "../include/EventModel.h"
#include "../include/MyJetFinder.h"

/*
 * In addition to constructing the general EventModel(),
 * MyJetFinder creates TLorentzVectors that can interact with
 * the FastJet methods.
 */
MyJetFinder::MyJetFinder() : EventModel() {
    t_nJetReco = new TNtuple("t_nJetReco", "Number of jets in event", "n");
    t_ptJetReco= new TNtuple("t_ptJetReco", "pt of reco jets in event", "pt");
    // Create jet definition. Should not change.
    jetDef = JetDefinition(antikt_algorithm, R);
}

/*
 * In addition to deleting ~EventModel(),
 * delete all jet finder objects.
 */
MyJetFinder::~MyJetFinder() {
}

/*
 * Depending on STR, generates a type of particle and places it in the parcticles
 * std::vector for analysis in fastjet.
 */
void MyJetFinder::GenerateParticle() {
    EventModel::GenerateParticle();
    // Check if found first trigger on above call to GenerateParticle().
    if (pt > trigPtThreshold) {
    	// ---------- Trigger ----------
    	TLorentzVector vTemp;
		vTemp.SetPtEtaPhiM(pt, eta, phi, 0.0);
		PseudoJet jetTemp(vTemp.Px(), vTemp.Py(), vTemp.Pz(), vTemp.E());
		particlesVector.push_back(jetTemp);

    	// ---------- Jet ----------
		tAssoc->GetEntry(tAssoc->GetEntries() - 1); // Get most recent associated particle. 
		if (pt != 100.0) {
			cout << "Associated particle pt != 100 in MyJetFinder::GenerateParticle()." << endl;
        }
		vTemp.SetPtEtaPhiM(pt, eta, phi, 0.0);
		jetTemp = PseudoJet(vTemp.Px(), vTemp.Py(), vTemp.Pz(), vTemp.E());
		particlesVector.push_back(jetTemp);

    } else {
    	TLorentzVector vTemp;
		vTemp.SetPtEtaPhiM(pt, eta, phi, 0.0);
		PseudoJet jetTemp(vTemp.Px(), vTemp.Py(), vTemp.Pz(), vTemp.E());
		particlesVector.push_back(jetTemp);
    }
}

/*
 * Cluster on particlesVector and create jetsVector.
 */
void MyJetFinder::FindJets() {
    clusterSequence    = new ClusterSequence(particlesVector, jetDef);
    jetsVector         = sorted_by_pt(clusterSequence->inclusive_jets(ptmin));
    t_nJetReco->Fill((Int_t) jetsVector.size());
    for (Int_t i = 0; i < jetsVector.size(); i++) {
    	t_ptJetReco->Fill(jetsVector[i].pt());
    }
}

Int_t MyJetFinder::GetNumJets() {
	return jetsVector.size();
}

void MyJetFinder::ResetEventVariables() {
    particlesVector.clear();
    jetsVector.clear();
}

/*
 * Code snippet copy/paste from short_example.cc in fastjet documentation.
 */
void MyJetFinder::PrintJetsInEvent() {
    // Print out some jet information.
    cout << "Clustering with " << jetDef.description() << endl;

    // Print the pt, y, and phi of each jet.
    cout << "Jet Index\t pt\t rapidity\t phi" << endl;
    for (unsigned i = 0; i < jetsVector.size(); i++) {
        cout << "Jet " << i << ":\t ";
        cout << jetsVector[i].pt() 	<< "\t ";
        cout << jetsVector[i].rap() << "\t ";
        cout << jetsVector[i].phi() << endl;

        // Print the pt for each constituent.
        vector<PseudoJet> constituents = jetsVector[i].constituents();
        for (unsigned j = 0; j < constituents.size(); j++) {
            cout << "Constituent " << j << "'s pt: " << constituents[j].pt() << endl;
        } 
    }
}

/*
 * Places a jetPlots directory in the root file created by EventModel::Write().
 */
void MyJetFinder::Write(TString fileName) {
    EventModel::Write(fileName);

    TFile* jetFile = new TFile(fileName.Data(), "UPDATE");

    // _____ Store all plots from jet finder. _____
    TDirectory* jetDir = jetFile->mkdir("jetPlots");
    jetDir->cd();
    jetDir->Add(t_nJetReco, true);
    jetDir->Add(t_ptJetReco, true);

    jetFile->Write();
    delete jetFile;
}
