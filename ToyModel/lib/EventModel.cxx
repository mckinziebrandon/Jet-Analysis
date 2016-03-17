#include "../include/EventModel.h"
#include "../include/EventFunctions.h"

/************************************
* Default EventModel Constructor.   *
* Initializes all instance objects. *
*************************************/
EventModel::EventModel()
{    
    t_trig  = (TTree*)InitializeTrigger();
    t_assoc = (TTree*)InitializeJet();
    t_bkg   = (TTree*)InitializeBackground();

    // Initialize all functions.
    functions = new EventFunctions();

    // Fit ALICE data and store in fPolynomial and fLine.
    TFile* file_v2 = new TFile("/home/student/jet-radius-analysis/ToyModel/rootFiles/ALICE_v2pt.root");
    TGraph* g = (TGraphAsymmErrors*) file_v2->Get("v2Plot_2;1");
    g->Fit(functions->GetfPolynomial(), "RQ");
    g->Fit(functions->GetfLine(), "RQ+");
    delete g;
    delete file_v2;

    // TODO: Fit with fTrackSpectrum. Seems fine to leave out for now.
    // Fit CMS pt distribution and store in fExp().
    /*
    TFile* fileCMS = new TFile("/home/student/jet-radius-analysis/ToyModel/rootFiles/CMS_pt.root");
    TGraph* graphCMS = (TGraphAsymmErrors*) fileCMS->Get("ptDistribution;1");
    graphCMS->Fit(functions->GetfTrackSpectrum(), "RQ");
    delete graphCMS;
    delete fileCMS;
    */

    // Initialize random number generator.
    rand = new TRandom3();
}

/************************************
* Default EventModel Destructor.    *
* Deletes instance objects.         *
*************************************/
EventModel::~EventModel() 
{
    delete functions;
    delete rand;
    delete t_trig;
    delete t_assoc;
    delete t_bkg;
}

void EventModel::Write(TString fileName)
{
    TFile* topFile = new TFile(fileName.Data(), "RECREATE");

    //_____________ Store all trees. _____________
    TDirectory* treeDir = topFile->mkdir("trees");
    treeDir->cd();
    treeDir->Add(t_trig, true);
    treeDir->Add(t_assoc, true);
    treeDir->Add(t_bkg, true);

    //_____________ Store all functions. _____________
    TDirectory* functionDir = topFile->mkdir("functions");
    functionDir->cd();
    // TODO:?
    functionDir->Add(functions->GetfPolynomial(), true);
    functionDir->Add(functions->GetfLine(), true);
    functionDir->Add(functions->GetfTrackSpectrum(), true);
    functionDir->Add(functions->GetfdNdPhi(), true);

    topFile->Write();
    delete topFile;
}

/*
 * Replacement for Generate().
 * Now it gets a random particle and figures out if it satisfies the conditions
 * for being a trigger particle. If it does, it immediately places a 100 GeV object
 * at the opposite in phi.
 */
void EventModel::GenerateParticle() {
    pt  = GetTrackPt();
    if (pt > triggerThreshold && !haveTrigger) {
    	// Create & store trigger particle.
    	haveTrigger = true;
    	eta = GetRandEta();
    	phi = GetTriggerPhi(pt);
    	t_trig->Fill();
    	// Create & store 100 GeV away-side jet.
    	pt = 100.0;
    	eta = -eta;
    	phi = GetAssocPhi(pt, phi, sigma_dphi);
    	t_assoc->Fill();
    } else {
        eta = GetRandEta();
        phi = GetTriggerPhi(pt);
        t_bkg->Fill();
	}
}
void EventModel::NewEvent() {
	haveTrigger = false;
}

/****************************************************
* Returns random value of phi that depends on       *
* the value of v2(pt) input parameter for dN/dPhi.  *
*****************************************************/
Float_t EventModel::GetTriggerPhi(Float_t pt) 
{
    // Todo.
    functions->GetfdNdPhi()->SetParameter("v2", 3.0 * GetV2(pt));
    return functions->GetfdNdPhi()->GetRandom();
}


/********************************************************************************
* Similar to dphi(p1, p2), this returns a deltaPhi, but with a gaussian spread  *
* centered at pi from the trigger particle.                                     *
*********************************************************************************/
Float_t EventModel::GetAssocPhi(Float_t pt, Float_t trig_phi, Float_t sigma_dphi)
{
    // Setup. 
    Float_t assoc_phi_mean, result;

    // Get the average phi, centered at pi w.r.t. the trigger.
    assoc_phi_mean  = trig_phi + pi;
    if (assoc_phi_mean > 2 * pi) 
        assoc_phi_mean -= 2 * pi;

    return (Float_t) rand->Gaus(assoc_phi_mean, sigma_dphi);
}

/****************************************************
* Returns difference of inputs, while constraining  *
* the result to be in [0, 2 * pi].                  *
*****************************************************/
Float_t EventModel::dphi(Float_t phi1, Float_t phi2) 
{
    Float_t dphi    = phi1 - phi2;
    if (dphi < 0) return dphi + 2 * pi;
    return dphi;
}

/* Returns random eta uniform in [-1, 1]. */
Float_t EventModel::GetRandEta() { return rand->Uniform(-1.00, 1.00); }

/* Returns random pt from boltzmann probability distribution. */
Double_t EventModel::GetTrackPt() { return functions->GetfTrackSpectrum()->GetRandom(); }


/*===============================================================================
---------------------------------------------------------------------------------
                            PRIVATE METHODS                                      
---------------------------------------------------------------------------------
================================================================================*/

Float_t EventModel::GetV2(Float_t pt)
{
    if (pt < 6)  return (Float_t)functions->GetfPolynomial()->Eval(pt);
    return (Float_t)functions->GetfLine()->Eval(pt);
}

TTree* EventModel::InitializeTrigger()
{
    TTree* tt = new TTree("tt", "Trigger attributes");
    tt->Branch("eta", &eta);
    tt->Branch("phi", &phi);
    tt->Branch("pt", &pt);
    return tt;
}

TTree* EventModel::InitializeJet()
{
    TTree* ta = new TTree("ta", "Associated attributes");
    ta->Branch("eta", &eta);
    ta->Branch("phi", &phi);
    ta->Branch("pt", &pt);
    return ta;
}

TTree* EventModel::InitializeBackground()
{
    TTree* tb = new TTree("tb", "Background attributes");
    tb->Branch("eta", &eta);
    tb->Branch("phi", &phi);
    tb->Branch("pt", &pt);
    return tb;
}

