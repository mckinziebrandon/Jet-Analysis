#include "../include/EventGenerator.h"
#include "../include/MyJetFinder.h"

/* In addition to constructing the general EventGenerator(),
 * MyJetFinder creates TLorentzVectors that can interact with
 * the FastJet methods. */
MyJetFinder::MyJetFinder() {
    tNJets      = new TNtuple("tNJets", "Number of jets in event", "n");
    tJetInfo    = new TNtuple("tJetInfo", "pt of reco jets in event", "pt:eta:phi:area:ptSub");
    // Create jet definition. Should not change.
    jetDef = JetDefinition(antikt_algorithm, R);
}

/* In addition to deleting ~EventGenerator(),
 * delete all jet finder objects. */
MyJetFinder::~MyJetFinder() {
}

/* Cluster on particlesVector and create jetsVector.*/
void MyJetFinder::FindJets(vector<PseudoJet> event) {
    // Jet area definition. 
    Double_t ghostMaxRap = maxEta; // Fiducial cut for back
    GhostedAreaSpec areaSpec(ghostMaxRap);
    AreaDefinition areaDef(active_area_explicit_ghosts, GhostedAreaSpec(ghostMaxRap, 1, 0.01));

    // Cluster all particles in event according to jet area & area definitions.
    particlesVector = event;
    clusterSequence = new ClusterSequenceArea(particlesVector, jetDef, areaDef);

    // Assert jetsVector = all jets(pt > ptMin) and satisfy fiducial cut.
    Selector fidCutSelector = SelectorAbsEtaMax(1.0 - R);
    jetsVector = sorted_by_pt(clusterSequence->inclusive_jets(ptMin));
    jetsVector = fidCutSelector(jetsVector);

    // Background definition.
    JetDefinition jetDefBkg(kt_algorithm, R);
    AreaDefinition areaDefBkg(active_area_explicit_ghosts, GhostedAreaSpec(ghostMaxRap, 1, 0.01));
    Selector bkgSelector = SelectorAbsEtaMax(1.0) * (!SelectorNHardest(Remove_N_hardest));

    // Background estimation and subtraction. 
    JetMedianBackgroundEstimator bkgEstimator(bkgSelector, jetDefBkg, areaDefBkg);
    Subtractor subtractor(&bkgEstimator);
    bkgEstimator.set_particles(particlesVector);

    // Obtain rho: the median bkg pT per area. 
    rho     = bkgEstimator.rho();
    sigma   = bkgEstimator.sigma();

    tNJets->Fill((Int_t) jetsVector.size());
    for (Int_t i = 0; i < jetsVector.size(); i++) {
    	tJetInfo->Fill(jetsVector[i].pt(), 
                jetsVector[i].eta(), 
                jetsVector[i].phi(), 
                jetsVector[i].area(),
                jetsVector[i].perp() - rho * jetsVector[i].area());
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
