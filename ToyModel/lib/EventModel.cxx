#include "../include/EventModel.h"

/************************************
* Default EventModel Constructor.   *
* Initializes all instance objects. *
*************************************/
EventModel::EventModel() 
{
    // Initialize instance attribute functions.
    fPolynomial     = (TF1*)GetfPolynomial();
    fLine           = (TF1*)GetfLine();
    fTrackSpectrum  = (TF1*)GetfSpectrum();
    fdNdPhi         = (TF1*)GetfdNdPhi();

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
    delete fPolynomial;
    delete fLine;
    delete fdNdPhi;
    delete fTrackSpectrum;
    delete rand;
}

/****************************************************
* Returns random value of phi that depends on       *
* the value of v2(pt) input parameter for dN/dPhi.  *
*****************************************************/
Float_t EventModel::GetTriggerPhi(Float_t pt) 
{
    Float_t v2;
    if (pt > 5) v2 = fLine->Eval(pt);
    else v2 = fPolynomial->Eval(pt);
    fdNdPhi->SetParameter("v2", v2);
    return fdNdPhi->GetRandom();
}

/****************************************************
* Returns difference of inputs, while constraining  *
* the result to be in [0, 2 * pi].                  *
*****************************************************/
Float_t EventModel::dphi(Float_t phi1, Float_t phi2) 
{
    Float_t phimin  = 0.;
    Float_t phimax  = 2. * pi;
    Float_t dphi    = phi1 - phi2;
    if (dphi > phimax)      dphi -= 2*pi;
    else if (dphi < phimin) dphi += 2*pi;
    return dphi;
}

/********************************************************************************
* Similar to dphi(p1, p2), this returns a deltaPhi, but with a gaussian spread  *
* centered at pi from the trigger particle.                                     *
*********************************************************************************/
Float_t EventModel::GetAssocPhi(Float_t trig_phi, Float_t sigma_dphi)
{
    Float_t assoc_phi_mean  = (trig_phi >=  pi) ? trig_phi - pi : trig_phi + pi;
    Float_t phi_random      = rand->Gaus(assoc_phi_mean, sigma_dphi);

    if (phi_random < 0.) 
        return phi_random + 2 * pi;
    else if (phi_random > 2 * pi) 
        return phi_random - 2 * pi;

    return phi_random;
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


TF1* EventModel::GetfPolynomial() { reurn new TF1("fPolynomial", "pol3", 0., 5.); }

TF1* EventModel::GetfLine() 
{
    TF1* f = new TF1("fLine", "[0] + [1] * x", 5., 18.);
    f->SetParameter(0, 0.15);
    f->SetParameter(1, -0.1 / 10.);
    return f;
}

TF1* EventModel::GetfSpectrum () 
{
    TString funcString = "[0] * (TMath::Power([1], 2) * x * TMath::Exp(-[1]*x))";
    funcString += " + (x>1) * [2]";
    funcString += " * (1.17676e-01 * TMath::Sqrt(0.1396*0.1396+x*x)";
    funcString += " * TMath::Power(1. + 1./ [3] / 8.21795e-01";
    funcString += " * TMath::Sqrt(0.1396*0.1396+x*x), -1. * [3]))";
    funcString += " * (1 / ( 1 + TMath::Exp( -(x - [4]) / [5] )))";
    TF1* f = new TF1("fspectrum", funcString.Data(), .2, 200.);
    f->SetParameters(2434401791.20528, 2.98507, 10069622.25117, 5.50000, 2.80000, 0.20000);
    return f;
}

TF1* EventModel::GetfdNdPhi() 
{
    TF1* f= new TF1("fdNdPhi", "1 + 2 * [0] * TMath::Cos(2. * x)", 0., 2. * pi);
    f->SetParameter(0, 0.);
    f->SetParName(0, "v2");
    return f;
}

