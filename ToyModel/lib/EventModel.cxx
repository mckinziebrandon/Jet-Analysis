#include "../include/EventModel.h"


/************************************
* Default EventModel Constructor.   *
* Initializes all instance objects. *
*************************************/
EventModel::EventModel()
{    
    t_trig  = (TTree*)InitializeTrigger();
    t_assoc = (TTree*)InitializeJet();
    t_bkg   = (TTree*)InitializeBackground();

    // Initialize event switches.
    has_bkg = true;
    has_V2  = true;

    // Initialize all functions.
    functions = new EventFunctions();

    // Fit ALICE data and store in fPolynomial and fLine.
    TFile* file_v2 = new TFile("./rootFiles/ALICE_v2pt.root");
    TGraph* g = (TGraphAsymmErrors*)file_v2->Get("v2Plot_2;1");
    g->Fit(fPolynomial, "RQ");
    g->Fit(fPolynomial, "RQ+");
    delete g;
    delete file_v2;

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

void EventModel::Write()
{
    TFile* topFile = new TFile("./rootFiles/ToyModel.root", "RECREATE");

    //_____________ Store all trees. _____________
    TDirectory* treeDir = topFile->mkdir("trees");
    treeDir->cd();
    treeDir->Add(t_trig, true);
    treeDir->Add(t_assoc, true);
    treeDir->Add(t_bkg, true);

    //_____________ Store all functions. _____________
    TDirectory* functionDir = topFile->mkdir("functions");
    functionDir->cd();
    // Todo.
    functionDir->Add(fPolynomial, true);
    functionDir->Add(fLine, true);
    functionDir->Add(fTrackSpectrum, true);
    functionDir->Add(fdNdPhi, true);

    topFile->Write();
    delete topFile;
}

// uno
void EventModel::GenerateTrigger()
{
    // Todo.
    pt  = GetTrackPt();
    eta = GetRandEta();
    phi = GetTriggerPhi(pt); 
    t_trig->Fill();
}

// dos
void EventModel::GenerateJet()
{
    // Todo.
    Float_t trigger_phi = phi;
    eta = -eta;
    pt  = GetTrackPt();
    phi = GetAssocPhi(pt, trigger_phi, sigma_dphi);
    t_assoc->Fill();
}

// tres
void EventModel::GenerateBackground()
{
    // Todo.
    pt  = GetTrackPt();
    phi = GetTriggerPhi(pt);
    eta = GetRandEta();
    t_bkg->Fill();
}

/****************************************************
* Returns random value of phi that depends on       *
* the value of v2(pt) input parameter for dN/dPhi.  *
*****************************************************/
Float_t EventModel::GetTriggerPhi(Float_t pt) 
{
    // Todo.
    fdNdPhi->SetParameter("v2", GetV2(pt));
    return fdNdPhi->GetRandom();
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

    // Create superposition of gaussian and modulation in phi.
    TF1* f_v2_gaus = new TF1("f_v2_gaus", "gaus(0)+1+2*[3]*TMath::Cos(2.*x)", 0, 2*pi);
    f_v2_gaus->SetParameters(1, assoc_phi_mean, sigma_dphi, GetV2(pt));

    // Store random value of modulated function and clean up. 
    result = f_v2_gaus->GetRandom();
    delete f_v2_gaus;
    return result;
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
Double_t EventModel::GetTrackPt() { return fTrackSpectrum->GetRandom(); }


/*===============================================================================
---------------------------------------------------------------------------------
                            PRIVATE METHODS                                      
---------------------------------------------------------------------------------
================================================================================*/

Float_t EventModel::GetV2(Float_t pt)
{
    if (!has_V2) return 0.0;
    if (pt < 6)  return 3. * (Float_t)fPolynomial->Eval(pt);
    return 3. * (Float_t)fLine->Eval(pt);
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

