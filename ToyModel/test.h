#include <iostream>

#include "TCanvas.h"
#include "TEllipse.h"
#include "TFile.h"
#include "TH1.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TRandom3.h"
#include "TString.h"

#ifndef TEST_H
#define TEST_H

// -------------------------------------
const double Pi = TMath::Pi();
// -------------------------------------

// =============================================================
class Hadron
{
private:
    double eta;
    double phi;
    TH1D* h_eta = new TH1D("h_eta", "Hadron Eta Hist", 100, -1., 1.);

public:
    Hadron();
    ~Hadron();
    Hadron(double e, double p);
    void SetEta(double e);
    void FillEta();
    void SaveHists(TFile** f);
};

void Hadron::SaveHists(TFile** f)
{
    TFile * file = *f;
    file->cd();
    h_eta->Write();
}

Hadron::~Hadron(void)
{
    h_eta->Delete();
}

void Hadron::FillEta()
{
    h_eta->Fill(eta);
}
void Hadron::SetEta(double e)
{
    eta = e;
}

Hadron::Hadron(void)
{
    eta = phi = 0.0;
}

Hadron::Hadron(double e, double p)
{
    eta = e;
    phi = p;
}
// =============================================================


/*
// = = = = =  = = = = = = = = =  
class Jet
{
private:
    double delta_phi;
    double sigma_delta_phi;

public:
    Jet();
}

Jet::Jet()
{
    delta_phi = Pi;
    sigma_delta_phi = 0.0;
}

Hadron::Hadron(double e, double p)
{
    eta = e;
    phi = p;
}
*/

#endif



