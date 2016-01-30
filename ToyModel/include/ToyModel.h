#ifndef TOYMODEL_H
#define TOYMODEL_H

const Float_t pi = TMath::Pi();

Float_t dphi(Float_t p, Float_t p2); 
Float_t GetAssocPhi(Float_t tp, Float_t sp, TRandom3* r);

Float_t dphi(Float_t phi1,Float_t phi2)
{
    Float_t phimin = -0.5 * pi;
    Float_t phimax = 1.5 * pi;
    Float_t dphi = phi1-phi2;
    if (dphi > phimax) dphi -= 2*pi;
    if (dphi < phimin) dphi += 2*pi;
    return dphi;
}

Float_t GetAssocPhi(Float_t trig_phi, Float_t sigma_dphi, TRandom3* rand)
{
    Float_t assoc_phi_mean = (trig_phi >=  pi) ? trig_phi - pi : trig_phi + pi;
    return rand->Gaus(assoc_phi_mean, sigma_dphi);
}


#endif
