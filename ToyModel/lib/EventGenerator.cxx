#include "../include/EventGenerator.h"
#include "../include/EventFunctions.h"

/************************************
* Default EventGenerator Constructor.   *
* Initializes all instance objects. *
*************************************/
EventGenerator::EventGenerator()
{    
    tTrig  = (TTree*)InitializeTrigger();
    tAssoc = (TTree*)InitializeAssoc();
    tBkg   = (TTree*)InitializeBackground();

    // Initialize all functions.
    functions = new EventFunctions();

    // Fit ALICE data and store in fPolynomial and fLine.
    TFile* file_v2 = new TFile("/home/student/jet-radius-analysis/ToyModel/rootFiles/ALICE_v2pt.root");
    TGraph* g = (TGraphAsymmErrors*) file_v2->Get("v2Plot_2;1");
    g->Fit(functions->GetfPolynomial(), "RQ");
    g->Fit(functions->GetfLine(), "RQ+");
    delete g;
    delete file_v2;

    //
    TFile*  fileCMS  = new TFile("/home/student/jet-radius-analysis/ToyModel/rootFiles/CMS_pt.root");
    TGraph* graphCMS = (TGraphAsymmErrors*) fileCMS->Get("ptDistribution;1");
    graphCMS->Fit(functions->GetfTrackSpectrum(), "RQ");
    delete graphCMS;
    delete fileCMS;

    //
    TFile * multFile = new TFile("/home/student/jet-radius-analysis/ToyModel/rootFiles/Multiplicity.root");
    TGraph* gMult = (TGraphAsymmErrors*) multFile->Get("multiplicity;1");
    gMult->Fit(functions->GetfMult(), "RQ");
    delete gMult;
    delete multFile;

    // Initialize random number generator.
    rand = new TRandom3();
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

void EventGenerator::Write(TString fileName) {
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
    functionDir->Add(functions->GetfMult(), true);

    topFile->Write();
    delete topFile;
}

Float_t EventGenerator::Generate(const string& str, Int_t n) {
    if (str == "bkg") {
        for (Int_t i = 0; i < n; i++) {
            pt  = GetTrackPt();
            eta = GetRandEta();
            phi = GetPhi(pt);
            tBkg->Fill();
        }
        return pt;
	} else if (str == "trig") {
        // Create the trigger particle. 
        pt  = GetTrackPt(trigPtThreshold);
        eta = GetRandEta();
        phi = GetPhi(pt);
        tTrig->Fill();
        Float_t res = pt;
        // Create its associated particle.
        pt = 100.0;
    	eta = -eta;
    	phi = GetAssocPhi(phi);
    	tAssoc->Fill();
        return res;
    } else {
        std::cout << "Error: EventGenerator::Generate() called with bad str." << std::endl;
        return -1;
    }
}
/****************************************************
* Returns random value of phi that depends on       *
* the value of v2(pt) input parameter for dN/dPhi.  *
*****************************************************/
Float_t EventGenerator::GetPhi(Float_t pt) {
    // Todo: Implement more legitmate method of getting v2 than mult by 2.
    functions->GetfdNdPhi()->SetParameter("v2", 3.0 * GetV2(pt));
    return functions->GetfdNdPhi()->GetRandom();
}


/********************************************************************************
* Similar to dphi(p1, p2), this returns a deltaPhi, but with a gaussian spread  *
* centered at pi from the trigger particle.                                     *
*********************************************************************************/
Float_t EventGenerator::GetAssocPhi(Float_t trigPhi) {
    // Setup. 
    Float_t mu, result;

    // Get the average phi, centered at pi w.r.t. the trigger.
    mu  = trigPhi + pi;
    result = (Float_t) rand->Gaus(mu, sigmaDeltaPhi);
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
    Float_t dphi    = phi1 - phi2;
    if (dphi < 0) return dphi + 2 * pi;
    return dphi;
}

/* Returns random eta uniform in [-1, 1]. */
Float_t EventGenerator::GetRandEta() { 
    return rand->Uniform(-1.00, 1.00); 
}

/* Returns random pt from boltzmann probability distribution. */
Double_t EventGenerator::GetTrackPt(Float_t xMin) { 
    return functions->GetfTrackSpectrum()->GetRandom(xMin, 100.0); 
}

void EventGenerator::SetCentrality(int percent) {
    percentCentrality = percent;
    multiplicity = functions->GetfMult()->Eval(percent);
}

Float_t EventGenerator::GetMultiplicity() {
    return multiplicity;
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

