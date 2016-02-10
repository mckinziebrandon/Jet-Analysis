#include "../include/ToyModel.h"

TF1* fdNdPhi = dNdPhi();
TF1* fTrackSpectrum = GetSpectrum();
/****************************************************************
* Fits ALICE v2 data with 3rd-order polynomial and returns the  *
* fit function.                                                 *
*****************************************************************/


/********************************************
* Given a pt value, calculates the v2(pt)   *
* and returns a random value of phi from    *
* probability distribution of dNdPhi(v2)    *
* defined below in TF1* dNdPhi()            *
*********************************************/
Float_t GetTriggerPhi(Float_t pt, TF1* f) 
{
    Float_t v2 = f->Eval(pt);
    fdNdPhi->SetParameter("v2", v2);
    return fdNdPhi->GetRandom();
}

/****************************************************************************************
* Returns the function relating the differential number of particles at a given value   *
* of deltaPhi w.r.t. event plane (?).                                                   *
*****************************************************************************************/
TF1* dNdPhi()
{
    TF1* fdNdPhi = new TF1("fdNdPhi", "1 + 2 * [0] * TMath::Cos(2. * x)", 0., 2. * pi);
    fdNdPhi->SetParameter(0, 0.);
    fdNdPhi->SetParName(0, "v2");
    return fdNdPhi;
}

/********************************************************************************
* Author: Redmer from ALICE.                                                    *
* Returns combination of boltzmann spectrum and hard jet spectrum.              *
* Used in ToyModel.C to get random pt values for particles.                     *
*********************************************************************************/
TF1* GetSpectrum() 
{
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

/************************************************
* Author: Leonardo Milano, Brandon McKinzie     *
* Returns delta Phi of input particle phis that *
* is constrained to be within [0, 2* pi].       *
*************************************************/
Float_t dphi(Float_t phi1,Float_t phi2)
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
Float_t GetAssocPhi(Float_t trig_phi, Float_t sigma_dphi, TRandom3* rand)
{
    Float_t assoc_phi_mean = (trig_phi >=  pi) ? trig_phi - pi : trig_phi + pi;
    Float_t phi_random = rand->Gaus(assoc_phi_mean, sigma_dphi);
    if (phi_random < 0.) return phi_random + 2 * pi;
    else if (phi_random > 2 * pi) return phi_random - 2 * pi;
    return phi_random;
}


