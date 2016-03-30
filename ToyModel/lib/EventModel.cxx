#include "../include/EventModel.h"
#include "../include/EventFunctions.h"

/************************************
* Default EventModel Constructor.   *
* Initializes all instance objects. *
*************************************/
EventModel::EventModel()
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
    delete tTrig;
    delete tAssoc;
    delete tBkg;
}

void EventModel::Write(TString fileName)
{
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
    if (pt > trigPtThreshold) {
    	eta = GetRandEta();
    	phi = GetPhi(pt);
    	tTrig->Fill();
    	// Store pt eta phi before resetting to artificial jet.
    	Float_t tempPtEtaPhi[3] = {pt, eta, phi};
    	// Create & store 100 GeV away-side jet.
    	pt = 100.0;
    	eta = -eta;
    	phi = GetAssocPhi(phi);
    	tAssoc->Fill();
    	// Restore legitimate pt eta phi for functions that may want
    	// to use them later.
    	pt = tempPtEtaPhi[0];
    	eta = tempPtEtaPhi[1];
    	phi = tempPtEtaPhi[2];
    } else {
        eta = GetRandEta();
        phi = GetPhi(pt);
        tBkg->Fill();
	}
}

/****************************************************
* Returns random value of phi that depends on       *
* the value of v2(pt) input parameter for dN/dPhi.  *
*****************************************************/
Float_t EventModel::GetPhi(Float_t pt) 
{
    // Todo: Implement more legitmate method of getting v2 than just
	// multiplying by 3.0 . . . smh.
    functions->GetfdNdPhi()->SetParameter("v2", 3.0 * GetV2(pt));
    return functions->GetfdNdPhi()->GetRandom();
}


/********************************************************************************
* Similar to dphi(p1, p2), this returns a deltaPhi, but with a gaussian spread  *
* centered at pi from the trigger particle.                                     *
*********************************************************************************/
Float_t EventModel::GetAssocPhi(Float_t trigPhi)
{
    // Setup. 
    Float_t mu, result;

    // Get the average phi, centered at pi w.r.t. the trigger.
    mu  = trigPhi + pi;
    if (mu > 2 * pi) 
        mu -= 2 * pi;

    return (Float_t) rand->Gaus(mu, sigmaDeltaPhi);
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
    TTree* tt = new TTree("tTrig", "Trigger particle attributes");
    tt->Branch("eta", &eta);
    tt->Branch("phi", &phi);
    tt->Branch("pt", &pt);
    return tt;
}

TTree* EventModel::InitializeAssoc()
{
    TTree* ta = new TTree("tAssoc", "Associated particle attributes");
    ta->Branch("eta", &eta);
    ta->Branch("phi", &phi);
    ta->Branch("pt", &pt);
    return ta;
}

TTree* EventModel::InitializeBackground()
{
    TTree* tb = new TTree("tBkg", "Background particle attributes");
    tb->Branch("eta", &eta);
    tb->Branch("phi", &phi);
    tb->Branch("pt", &pt);
    return tb;
}

