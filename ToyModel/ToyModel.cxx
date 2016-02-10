/********************************************************************************************
* Filename:     ToyModel.C                                                                  *
* Date Created: January 25, 2016                                                            *
* Author:       Brandon McKinzie                                                            *
* Description:  Simple model event generator for multiple parton scattering.                *
*               Serves as introduction to toy models for me (Brandon) and                   *
*               two-particle correlation study.                                             *
********************************************************************************************/
#include <iostream>
#include <fstream>
#include "./include/RootClasses.h"
#include "./include/ToyModel.h"


// --------------------------------------------------------------------------------------------
void ToyModel()
{
    /* ------------------------------------------------------------ *
     * Object Declarations.                                         *
     * ------------------------------------------------------------ */

    Float_t eta, phi, pt;

    TF1* fPolynomial    = new TF1("fPolynomial", "pol3", 0., 5.);
    TF1* fLine          = new TF1("fLine", "[0] + [1] * x", 5., 18.);
    fLine->SetParameter(0, 0.15);
    fLine->SetParameter(1, -0.1 / 10.);

    TFile* file_v2  = new TFile("./rootFiles/ALICE_v2pt.root");
    TGraph* g       = (TGraphAsymmErrors*)file_v2->Get("v2Plot_2;1");


    g->Fit(fPolynomial, "RQ");
    g->Fit(fPolynomial, "RQ+");

    TFile* f_out = new TFile("./rootFiles/ToyModel.root", "RECREATE");

    // Create file for object output tests.
    std::ofstream f_debug("./debug/debug_ToyModel.txt");

    // Phase space: trigger.
    TTree* t_trig = new TTree("t_trig", "Trigger attributes");
    t_trig->Branch("eta", &eta);
    t_trig->Branch("phi", &phi);
    t_trig->Branch("pt", &pt);

    // Phase space: associated.
    TTree* t_assoc = new TTree("t_assoc", "Associated attributes");
    t_assoc->Branch("eta", &eta);
    t_assoc->Branch("phi", &phi);
    t_assoc->Branch("pt", &pt);

    // Phase space: background.
    TTree* t_bkg = new TTree("t_bkg", "Background attributes");
    t_bkg->Branch("eta", &eta);
    t_bkg->Branch("phi", &phi);
    t_bkg->Branch("pt", &pt);

    // Will randomly generate uniform eta and phi of simulated jets. 
    TRandom3* rand = new TRandom3();


    /* ---------------------------------------------------- *
     * Data Generation/Simulation.                          *       
     * ---------------------------------------------------- */

    // Simulate nEvents with randomly generated tracks. 
    for (Int_t i_event = 0; i_event < nEvents; i_event++)
    {
        if (i_event % (nEvents / 10) == 0)
            std::cout << (Float_t)(i_event * 100)/nEvents << " Percent Complete." << std::endl;

        // Generate trigger eta, pt, and v2(pt).
        pt  = GetTrackPt();
        if (pt > 5) phi = GetTriggerPhi(pt, fLine);
        else        phi = GetTriggerPhi(pt, fPolynomial);
        eta = rand->Uniform(-1.00, 1.00);
        t_trig->Fill();

        // Generate associated eta, phi pt.
        eta = -eta;
        phi = GetAssocPhi(phi, sigma_dphi, rand);
        pt  = GetTrackPt();
        t_assoc->Fill();


        // Generate background eta, phi pt.
        for (Int_t i_bkg = 0; i_bkg < nBkg; i_bkg++)
        {
            // Generate trigger eta and phi.
            pt  = GetTrackPt();
        //    phi = GetTriggerPhi(pt);
            phi = 6.;
            eta = rand->Uniform(-1.00, 1.00);
            t_bkg->Fill();
        }
    }

    delete rand;

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    f_out->cd();
    t_trig->Write("", TObject::kOverwrite);
    t_assoc->Write("", TObject::kOverwrite);
    t_bkg->Write("", TObject::kOverwrite);

    /*
    // Create list of generic histogram identifiers for plotting.
    TString histNames[] = {"phi", "eta", "pt", "eta:phi"};

    // Draw [3] [trigger] histograms from data in [t_trig].
    TCanvas* c_trigger = (TCanvas*)DrawHists(4, TString("trigger"), histNames, t_trig);
    c_trigger->Write();
    c_trigger->Close();

    // Draw [3] [associated] histograms from data in [t_assoc].
    TCanvas* c_associated = (TCanvas*)DrawHists(4, TString("associated"), histNames, t_assoc);
    c_associated->Write();
    c_associated->Close();

    // Draw [3] [background] histograms from data in [t_bkg].
    TCanvas* c_bkg = (TCanvas*)DrawHists(4, TString("background"), histNames, t_bkg);
    c_bkg->Write();
    c_bkg->Close();
    */

    delete t_trig;
    delete t_assoc;
    delete t_bkg;
    delete fPolynomial;
    delete fLine;
    delete g;
    delete file_v2;
    delete f_out;

}

#ifndef __CINT__
int main() 
{
    ToyModel();
    return 0;
}
#endif
