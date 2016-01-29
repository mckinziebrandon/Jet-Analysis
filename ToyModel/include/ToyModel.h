#include <fstream>

#ifndef TOYMODEL_H
#define TOYMODEL_H

const Float_t   pi = TMath::Pi();

Float_t dphi(Float_t phi1, Float_t phi2, 
             Float_t phimin = -0.5 * pi,
             Float_t phimax = 1.5*pi);

// Ensure dphi == phi1 - phi2 is within [phimin, phimax]
Float_t dphi(Float_t phi1,Float_t phi2,Float_t phimin,Float_t phimax){
  Float_t dphi = phi1-phi2;
  if (dphi > phimax) dphi -= 2*pi;
  if (dphi < phimin) dphi += 2*pi;
  return dphi;
}

#endif
