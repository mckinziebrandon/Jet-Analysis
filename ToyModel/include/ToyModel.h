
#ifndef TOYMODEL_H
#define TOYMODEL_H
#include <fstream>
#include "../include/ToyModel.h"

const Double_t   pi = TMath::Pi();

Double_t dphi(Double_t phi1, Double_t phi2, 
             Double_t phimin = -0.5 * pi,
             Double_t phimax = 1.5*pi);

Double_t GetAssocPhi(Double_t tp, Double_t sp, TRandom3* r);

#endif
