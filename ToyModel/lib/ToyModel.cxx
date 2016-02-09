#include "../include/ToyModel.h"

// 
TF1* funcV2() 
{
    TFile* file_v2 = new TFile("./rootFiles/ALICE_v2pt.root");
    TGraph* g = (TGraphAsymmErrors*)file_v2->Get("v2Plot_3;1");
    g->Fit("pol3", "Q+0");
    return (TF1*)g->GetFunction("pol3");
}

// 
Float_t GetTriggerPhi(Float_t pt) 
{
    Float_t v2 = GetV2(pt);
    fdNdPhi->SetParameter("v2", v2);
    return fdNdPhi->GetRandom();
}

//
TF1* dNdPhi()
{
    TF1* fdNdPhi = new TF1("fdNdPhi", "1 + 2 * [0] * TMath::Cos(2. * x)", 0., 2. * pi);
    fdNdPhi->SetParameter(0, 0.);
    fdNdPhi->SetParName(0, "v2");
    return fdNdPhi;
}

//
TF1* GetSpectrum() 
{
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
// to generate a 'track' 

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


