#include "../include/EventFunctions.h"
#include "../include/EventGenerator.h"

/************************************
* Default EventGenerator Constructor.   *
* Initializes all instance objects. *
*************************************/
EventGenerator::EventGenerator() {    
    std::string PATH = "/home/student/jet-radius-analysis/";

    // ----- Initialize all instance variables. -----
    rand    = new TRandom3();
    functions = new EventFunctions();
    tTrig   = (TTree*)InitializeTrigger();
    tAssoc  = (TTree*)InitializeAssoc();
    tBkg    = (TTree*)InitializeBackground();
    // TODO: correct way to get multiplicity??
    //multiplicity = 1.6 * 2.0 * pi * functions->GetfTrackSpectrum()->Integral(0.5, 20.0);

    // ----- Store fits to all desired distributions inside the functions variable. -----
    // 1. Get efficiency 4d histogram. 
    TFile* fEff = new TFile((PATH + "TaskBrandon/correction_2010_aod162_hybrid.root").data(), "READ");
    hEff = (THnD*) fEff->Get("correction;1");
    PATH += "ToyModel/rootFiles/";

    // 2. Fit ALICE data and store in fPolynomial and fLine.
    TFile* file_v2 = new TFile((PATH + "ALICE_v2pt.root").data());
    TGraph* g = (TGraphAsymmErrors*) file_v2->Get("v2Plot_2;1");
    g->Fit(functions->GetfPolynomial(), "RQ");
    g->Fit(functions->GetfLine(), "RQ+");
    delete file_v2;

    // 3. Get pt distribution.
    TFile*  fileALICE  = new TFile((PATH + "ALICE_pt.root").data());
    TGraph* graphALICE = (TGraphAsymmErrors*) fileALICE->Get("ptDistribution;1");
    graphALICE->Fit(functions->GetfTrackSpectrum(), "RQ");
    delete fileALICE;

    // 4. Get multiplicity vs. centrality distribution. 
    TFile * multFile = new TFile((PATH + "Multiplicity.root").data());
    TGraph* gMult = (TGraphAsymmErrors*) multFile->Get("multiplicity;1");
    spline = new TSpline3("spline", gMult);
    delete multFile;

    effVars[2]  = (Int_t) hEff->GetAxis(2)->FindBin(2.5); 
    effVars[3]  = (Int_t) hEff->GetAxis(3)->FindBin(0.0);           // ok to fix zVtx to 0.0?
}

/************************************
* Default EventGenerator Destructor.    *
* Deletes instance objects.         *
*************************************/
EventGenerator::~EventGenerator() {
    delete functions;
    delete rand;
    delete tTrig;
    delete tAssoc;
    delete tBkg;
}

/**************************************************************
* Returns the tracking efficiency of current particle,        *
* determined by the current values of eta and pt.             *
* EventGenerator assumes that (Z-vtx = 0, cent = 2.5) always. *
***************************************************************/
Float_t EventGenerator::GetEfficiency() {
    effVars[0]  = hEff->GetAxis(0)->FindBin(eta);
    effVars[1]  = hEff->GetAxis(1)->FindBin(pt); 
    //return 1.0 / (Float_t) hEff->GetBinContent(effVars); // TODO: eventually switch back
    return 1.0;

}

/************************************************************
* Generate(str, n):                                         *
* Most important method of EventGenerator class.            *
* STR identifies the type of particle(s) to be generated.   *
* N identifies the number of particles to be generated.     *
* All particles generated are stored in a TTree object.     *
*************************************************************/
Float_t EventGenerator::Generate(const string& str, Int_t n) {
    Printer::print("\tEntering EventGenerator::Generate");
    recoMult = 0;
    if (str == "bkg") {
        Printer::print("\t\tBeginning background particle construction . . .");
        for (Int_t i = 0; i < n; i++) {
            eta = GetRandEta();
            pt  = GetTrackPt(ptMin);
            phi = GetPhi(pt);
            // Fill track tree probabalistically based on efficiency. 
            // Needs to be done track-by-track since Eff is function of track properties.
            Float_t eff = GetEfficiency();
            if (rand->Rndm() < eff) {
                tBkg->Fill();
                recoMult++;
            }
        }
        return pt;
	} else if (str == "trig") {
        // Create the trigger particle. 
        eta = GetRandEta();
        pt  = GetTrackPt(trigPtThreshold);
        phi = GetPhi(pt);
        tTrig->Fill();
        Float_t res = pt;
        // Create its associated particle.
        pt = ptMax;
    	eta = -eta;
    	phi = GetAssocPhi(phi);
    	tAssoc->Fill();
        return res;
    } else {
        std::cout << "Error: EventGenerator::Generate() called with bad str." << std::endl;
        return -1;
    }
}

/********************************************************************
* Wraps all particles in tBkg into PseudoJets                       *
* and returns a vector of these (PseudoJets).                       *
* Primarily used to hand off to a JetFinder object for analysis.    *
*********************************************************************/
vector<PseudoJet> EventGenerator::GetLastEvent() {
    vector<PseudoJet> res;
    // Initialize entry index to first background particle in most recent event. 
    Int_t entry = tBkg->GetEntries() - multiplicity;
    // Loop over all background particles in most recent event and store as PseudoJets in res.
    while (entry++ < tBkg->GetEntries()) {
        tBkg->GetEntry(entry);
        res.push_back(GetPseudoJet());
    }
    // Place most recent associated particle (forced to be always one) in res.
    tAssoc->GetEntry(tAssoc->GetEntries()-1);
    res.push_back(GetPseudoJet());
    // Place most recent trigger particle in res.
    tTrig->GetEntry(tTrig->GetEntries()-1);
    res.push_back(GetPseudoJet());
    return res;
}

/*****************************************************************
* GetPseudoJet(): Private helper method of GetLastEvent()        *
* Returns current values of pt, eta, phi wrapped in a PseudoJet. * 
*****************************************************************/
PseudoJet EventGenerator::GetPseudoJet() {
    TLorentzVector tlvTemp;
    tlvTemp.SetPtEtaPhiM(pt, eta, phi, 0.0);
    PseudoJet jetTemp(tlvTemp.Px(), tlvTemp.Py(), tlvTemp.Pz(), tlvTemp.E());
    return jetTemp;
}


/************************************************************************
* Creates a new ROOT file with name FILENAME.                           *
* Stores the tree objects in a subdirectory named "trees".              *
* Stores the fit functions in a subdirectory called "functions".        *
* Note that the friend class of EventGenerator, JetFinder, may choose   *
* to update (but not overwrite) these files with jet information.       *
*************************************************************************/
void EventGenerator::Write(TString fileName) {
    Printer::print("Writing EventGenerator to file...");
    TFile* topFile = new TFile(fileName.Data(), "RECREATE");

    //_____________ Store all trees. _____________
    TDirectory* treeDir = topFile->mkdir("trees");
    treeDir->cd();
    treeDir->Add(tTrig, true);
    treeDir->Add(tAssoc, true);
    treeDir->Add(tBkg, true);

    //_____________ Store all functions. _____________
    TDirectory* functionDir = topFile->mkdir("functions");
    functionDir->cd();
    functionDir->Add(functions->GetfPolynomial(), true);
    functionDir->Add(functions->GetfLine(), true);
    functionDir->Add(functions->GetfTrackSpectrum(), true);
    functionDir->Add(functions->GetfdNdPhi(), true);

    topFile->Write();
    delete topFile;
}

/****************************************************
* Returns random value of phi that depends on       *
* the value of v2(pt) input parameter for dN/dPhi.  *
*****************************************************/
Float_t EventGenerator::GetPhi(Float_t pt) {
    // Todo: Implement more legitmate method of getting v2 than mult by 2.
    //functions->GetfdNdPhi()->SetParameter("v2", 1.5 * GetV2(pt));// TODO: switch back on
    functions->GetfdNdPhi()->SetParameter("v2", 0.);// TODO: switch back on
    return functions->GetfdNdPhi()->GetRandom();
}


/********************************************************************************
* Similar to dphi(p1, p2), this returns a deltaPhi, but with a gaussian spread  *
* centered at pi from the trigger particle.                                     *
*********************************************************************************/
Float_t EventGenerator::GetAssocPhi(Float_t trigPhi) {
    // Get the average phi, centered at pi w.r.t. the trigger.
    const Float_t mu = trigPhi + pi;
    Float_t result = (Float_t) rand->Gaus(mu, sigmaDeltaPhi);
    if (result > 2 * pi) {
        result -= 2 * pi;
    } else if (result < 0.0) {
        result += 2 * pi;
    }
    return result;
}

/****************************************************
* Returns difference of inputs, while constraining  *
* the result to be in [0, 2 * pi].                  *
*****************************************************/
Float_t EventGenerator::dphi(Float_t phi1, Float_t phi2) 
{
    Float_t dphi = phi1 - phi2;
    if (dphi < 0) return dphi + 2 * pi;
    return dphi;
}

/* Returns random eta uniform in [-1, 1]. */
Float_t EventGenerator::GetRandEta() { 
    // Switching to -0.8 to 0.8
    // TODO: verify this doesn't screw anything else up
    return rand->Uniform(-etaMax, etaMax);
}

/* Returns random pt from boltzmann probability distribution. */
Double_t EventGenerator::GetTrackPt(Float_t xMin) { 
    return functions->GetfTrackSpectrum()->GetRandom(xMin, 20.0); 
}



void EventGenerator::SetCentrality(int percent) {
    centrality = percent;
    multiplicity = spline->Eval(percent);
}

Float_t EventGenerator::GetMultiplicity() {
    return multiplicity;
}

Int_t EventGenerator::GetRecoMult() {
    return recoMult;
}

/*===============================================================================
---------------------------------------------------------------------------------
                            PRIVATE METHODS                                      
---------------------------------------------------------------------------------
================================================================================*/

Float_t EventGenerator::GetV2(Float_t pt) {
    if (pt < 6)         return (Float_t) functions->GetfPolynomial()->Eval(pt);
    else if (pt < 18)   return (Float_t) functions->GetfLine()->Eval(pt);
    return (Float_t) functions->GetfLine()->Eval(18.);
}

TTree* EventGenerator::InitializeTrigger()
{
    TTree* tt = new TTree("tTrig", "Trigger particle attributes");
    tt->Branch("eta", &eta);
    tt->Branch("phi", &phi);
    tt->Branch("pt", &pt);
    return tt;
}

TTree* EventGenerator::InitializeAssoc()
{
    TTree* ta = new TTree("tAssoc", "Associated particle attributes");
    ta->Branch("eta", &eta);
    ta->Branch("phi", &phi);
    ta->Branch("pt", &pt);
    return ta;
}

TTree* EventGenerator::InitializeBackground()
{
    TTree* tb = new TTree("tBkg", "Background particle attributes");
    tb->Branch("eta", &eta);
    tb->Branch("phi", &phi);
    tb->Branch("pt", &pt);
    return tb;
}

