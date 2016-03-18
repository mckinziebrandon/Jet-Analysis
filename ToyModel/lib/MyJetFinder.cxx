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

/* @deprecated
 * Hard-code some single-particle "jetsVector" into eventParticlesVector, as a way of
 * testing the jet finder abilities.
 */
void MyJetFinder::EmbedJetsInEvent() {
    Double_t pt_eta_phi[nEmbed][3] = {
    {100., 0.0, 3.1},
    {100., -0.5, 3.15},
    {100., 0.6, 3.18}};

    TLorentzVector v_temp;
    PseudoJet jet_temp;
    Float_t pt_temp, eta_temp, phi_temp;
    for(Int_t i = 0; i < nEmbed; i++) {
        pt_temp  = pt_eta_phi[i][0];
        eta_temp = pt_eta_phi[i][1];
        phi_temp = pt_eta_phi[i][2];

        v_temp.SetPtEtaPhiM(pt_temp, eta_temp, phi_temp, 0.0);
        jet_temp = PseudoJet(v_temp.Px(), v_temp.Py(), v_temp.Pz(), v_temp.E());
        eventParticlesVector.push_back(jet_temp);
    }
}

const Int_t MyJetFinder::GetNumEmbed() {
	return nEmbed;
}

/*
 * Depending on STR, generates a type of particle and places it in the parcticles
 * std::vector for analysis in fastjet.
 */
void MyJetFinder::GenerateParticle() {
	Bool_t hadTrigger = haveTrigger;
    EventModel::GenerateParticle();
    // Check if found first trigger on above call to GenerateParticle().
    if (hadTrigger != haveTrigger) {
    	// ---------- Trigger ----------
    	TLorentzVector v_temp;
		v_temp.SetPtEtaPhiM(pt, eta, phi, 0.0);
		PseudoJet jet_temp(v_temp.Px(), v_temp.Py(), v_temp.Pz(), v_temp.E());
		particlesVector.push_back(jet_temp);
		eventParticlesVector.push_back(jet_temp);
    	// ---------- Jet ----------
		t_assoc->GetEntry(t_assoc->GetEntries() -1);
		if (pt != 100.0) {
			cout << "jet pt not as expected in MyJetFinder::GenerateParticle()." << endl;
		} else {
			cout << "You good dawg." << endl;
		}
		v_temp.SetPtEtaPhiM(pt, eta, phi, 0.0);
		jet_temp = PseudoJet(v_temp.Px(), v_temp.Py(), v_temp.Pz(), v_temp.E());
		particlesVector.push_back(jet_temp);
		eventParticlesVector.push_back(jet_temp);

    } else {
    	TLorentzVector v_temp;
		v_temp.SetPtEtaPhiM(pt, eta, phi, 0.0);
		PseudoJet jet_temp(v_temp.Px(), v_temp.Py(), v_temp.Pz(), v_temp.E());
		particlesVector.push_back(jet_temp);
		eventParticlesVector.push_back(jet_temp);
    }
}

/*
 * Creates a jet definition with specified radius R, and
 * obtains list of jetsVector after creating a ClusterSequence of
 * particlesVector (which uses the jet definition).
 */
void MyJetFinder::FindJets() {
    clusterSequence = new ClusterSequence(particlesVector, jetDef);
    jetsVector    	= sorted_by_pt(clusterSequence->inclusive_jets(ptmin));
}

/*
 * Cluster on eventParticlesVector and create eventJetsVector.
 */
void MyJetFinder::FindJetsInEvent() {
    eventClusterSequence = new ClusterSequence(eventParticlesVector, jetDef);
    eventJetsVector = sorted_by_pt(eventClusterSequence->inclusive_jets(ptmin));
    t_nJetReco->Fill((Int_t) eventJetsVector.size());
    for (Int_t i = 0; i < eventJetsVector.size(); i++) {
    	t_ptJetReco->Fill(eventJetsVector[i].pt());
    }
}

Int_t MyJetFinder::GetNumJetsInEvent() {
	return eventJetsVector.size();
}

void MyJetFinder::ResetEventVariables() {
    eventParticlesVector.clear();
    eventJetsVector.clear();
}

/*
 * Code snippet copy/paste from short_example.cc in fastjet documentation.
 */
void MyJetFinder::PrintJetsInEvent() {
    // Print out some jet information.
    cout << "Clustering with " << jetDef.description() << endl;

    // Print the pt, y, and phi of each jet.
    cout << "Jet Index\t pt\t rapidity\t phi" << endl;
    for (unsigned i = 0; i < eventJetsVector.size(); i++) {
        cout << "Jet " << i << ":\t ";
        cout << eventJetsVector[i].pt() 	<< "\t ";
        cout << eventJetsVector[i].rap() << "\t ";
        cout << eventJetsVector[i].phi() << endl;

        // Print the pt for each constituent.
        vector<PseudoJet> constituents = eventJetsVector[i].constituents();
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
