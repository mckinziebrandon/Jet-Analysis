#include "../include/ToyModel.h"
// Ensure dphi == phi1 - phi2 is within [phimin, phimax]
Double_t dphi(Float_t phi1,Float_t phi2,Float_t phimin,Float_t phimax)
{
  Double_t dphi = phi1-phi2;
  if (dphi > phimax) dphi -= 2*pi;
  if (dphi < phimin) dphi += 2*pi;
  return dphi;
}

Double_t GetAssocPhi(Double_t trig_phi, Double_t sigma_dphi, TRandom3* rand)
{
    Double_t assoc_phi_mean = (trig_phi >=  pi) ? trig_phi - pi : trig_phi + pi;
    return rand->Gaus(assoc_phi_mean, sigma_dphi);
}


