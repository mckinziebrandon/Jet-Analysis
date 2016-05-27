#include "../include/EventGenerator.h"
#include "../include/MyJetFinder.h"

/* In addition to constructing the general EventGenerator(),
 * MyJetFinder creates TLorentzVectors that can interact with
 * the FastJet methods. */
MyJetFinder::MyJetFinder() {
    tEventJetInfo      = new TNtuple("tEventJetInfo", "Event jet properties", "n:rho:sigma");
    tJetInfo    = new TNtuple("tJetInfo", "Individual jet properties.", "pt:eta:phi:area:ptSub");
    jetDef = JetDefinition(antikt_algorithm, R);
}

MyJetFinder::MyJetFinder(Float_t R) {
    this->R = R;
    tEventJetInfo      = new TNtuple("tEventJetInfo", "Event jet properties", "n:rho:sigma");
    tJetInfo    = new TNtuple("tJetInfo", "Individual jet properties.", "pt:eta:phi:area:ptSub");
    jetDef = JetDefinition(antikt_algorithm, R);
}

/* In addition to deleting ~EventGenerator(),
 * delete all jet finder objects. */
MyJetFinder::~MyJetFinder() {
}

/************************************************************************
* FindJets(): Most important method of MyJetFinder class.               *
* 1. Cluster vector of particles according to jet and area definitions. *
* 2. Apply fiducial (eta) cut on vector of jets.                        *
* 3. Define the combinatorial background (and do nothing with it?).     *
*************************************************************************/
void MyJetFinder::FindJets(std::vector<PseudoJet> event) {
    // Jet area definition. 
    Double_t ghostMaxRap = etaMax; // Fiducial cut for back
    GhostedAreaSpec areaSpec(ghostMaxRap);
    AreaDefinition areaDef(active_area_explicit_ghosts, GhostedAreaSpec(ghostMaxRap, 1, 0.005));

    // Cluster all particles in event according to jet area & area definitions.
    particlesVector = event;
    clusterSequence = new ClusterSequenceArea(particlesVector, jetDef, areaDef);

    // Assert jetsVector = all jets(pt > ptMin) and satisfy fiducial cut.
    Selector fidCutSelector = SelectorAbsEtaMax(etaMax - R);
    jetsVector = sorted_by_pt(clusterSequence->inclusive_jets(ptMin));
    jetsVector = fidCutSelector(jetsVector);

    // Background definition.
    JetDefinition jetDefBkg(kt_algorithm, R);
    AreaDefinition areaDefBkg(active_area_explicit_ghosts, GhostedAreaSpec(ghostMaxRap, 1, 0.005));
    Selector bkgSelector = SelectorAbsEtaMax(etaMax) * (!SelectorNHardest(Remove_N_hardest));

    // Background estimation and subtraction. 
    JetMedianBackgroundEstimator bkgEstimator(bkgSelector, jetDefBkg, areaDefBkg);
    Subtractor subtractor(&bkgEstimator);
    bkgEstimator.set_particles(particlesVector);

    // Obtain rho: the median bkg pT per area. 
    rho     = bkgEstimator.rho();
    sigma   = bkgEstimator.sigma();

    tEventJetInfo->Fill((Int_t) jetsVector.size(), (Double_t) rho, (Double_t) sigma);
    for (Int_t i = 0; i < jetsVector.size(); i++) {
    	tJetInfo->Fill(jetsVector[i].pt(), 
                jetsVector[i].eta(), 
                jetsVector[i].phi(), 
                jetsVector[i].area(),
                jetsVector[i].perp() - rho * jetsVector[i].area());
    }
}

// Returns list of legend entries that are important for any jet-related plot. 
LegendList MyJetFinder::GetJetParams() {
    LegendList params;
    //params.push_back(std::make_pair((TObject*)0, Form("R = %.1f, anti-kT", R)));
    params.push_back(std::make_pair((TObject*)0, Form("A > %.2f", areaMin)));
    params.push_back(std::make_pair((TObject*)0, Form("p_{T} = %.1f", ptMin)));
    return params;
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
        std::vector<PseudoJet> constituents = jetsVector[i].constituents();
        for (unsigned j = 0; j < constituents.size(); j++) {
            cout << "Constituent " << j << "'s pt: " << constituents[j].pt() << endl;
        } 
    }
}

/************************************************************************************
 * Places a jetPlots directory in the root file created by EventGenerator::Write(). *
 ************************************************************************************/
void MyJetFinder::Write(const char* fileName) {
    TFile* jetFile = new TFile(fileName, "UPDATE");

    // _____ Store all plots from jet finder. _____
    TDirectory* jetDir = jetFile->mkdir("jetPlots");
    jetDir->cd();
    jetDir->Add(tEventJetInfo, true);
    jetDir->Add(tJetInfo, true);

    jetFile->Write();
    delete jetFile;
}
