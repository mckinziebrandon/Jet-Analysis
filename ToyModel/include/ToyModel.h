#ifndef TOYMODEL_H
#define TOYMODEL_H

// ---------- Constants ----------
const Float_t pi    = TMath::Pi();
const Int_t nEvents             = 100000;
const Int_t nBkg                = 10;
const Int_t nCanvas             = 3;
const Int_t trig_pt_threshold   = 1;
const Int_t parton_mass         = 0;
const Float_t sigma_dphi       = (pi / 4) / 2;
// -------------------------------

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------

// My functions.
Float_t dphi(Float_t p, Float_t p2); 
Float_t GetAssocPhi(Float_t tp, Float_t sp, TRandom3* r);

// Functions from Redmer. 
Double_t GetTrackPt();

// -----------------------------------------------------------------------------
// Function Implementations
// -----------------------------------------------------------------------------
Float_t dphi(Float_t phi1,Float_t phi2)
{
    Float_t phimin  = 0.;
    Float_t phimax  = 2. * pi;
    Float_t dphi    = phi1 - phi2;
    if (dphi > phimax)      dphi -= 2*pi;
    else if (dphi < phimin) dphi += 2*pi;
    return dphi;
}

Float_t GetAssocPhi(Float_t trig_phi, Float_t sigma_dphi, TRandom3* rand)
{
    Float_t assoc_phi_mean = (trig_phi >=  pi) ? trig_phi - pi : trig_phi + pi;
    Float_t phi_random = rand->Gaus(assoc_phi_mean, sigma_dphi);
    if (phi_random < 0.) return phi_random + 2 * pi;
    else if (phi_random > 2 * pi) return phi_random - 2 * pi;
    return phi_random;
}

TF1* GetSpectrum() {
    /* Todo: Understand this function . . . */
    // combination of boltzmann spectrum and hard jet spectrum
    TString funcString = "[0] * (TMath::Power([1], 2) * x * TMath::Exp(-[1]*x))";
    funcString += " + (x>1) * [2]";
    funcString += " * (1.17676e-01 * TMath::Sqrt(0.1396*0.1396+x*x)";
    funcString += " * TMath::Power(1. + 1./ [3] / 8.21795e-01";
    funcString += " * TMath::Sqrt(0.1396*0.1396+x*x), -1. * [3]))";
    funcString += " * (1 / ( 1 + TMath::Exp( -(x - [4]) / [5] )))";
    TF1* fspectrum = new TF1("fspectrum", funcString.Data(), .2, 200.);
    fspectrum->SetParameters(2434401791.20528, 2.98507, 10069622.25117, 5.50000, 2.80000, 0.20000);
    return fspectrum;   
}

// second: just boltzmann part (background)
TF1* GetThermalSpectrum() {
    // pure boltzmann part of thermal spectrum
    TF1* boltzmann = new TF1("boltzmann", "[0]*(TMath::Power([1], 2)*x*TMath::Exp(-[1]*x))");
    boltzmann->SetParameters(2434401791.20528, 2.98507);
    return boltzmann;
}

// some extra functions which are called in the track loop (below)
// select your total pt spectrum
TF1* fTrackSpectrum = GetSpectrum();

// to generate a 'track' 
Double_t GetTrackPt() { 
    return fTrackSpectrum->GetRandom();
}

#endif
