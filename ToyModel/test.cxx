#include <iostream>
#include "test.h"



void test()
{
    const int nTrig = 10;
    // Create file to store histograms.
    TFile * f_out = new TFile("RootFiles/test.root", "RECREATE");

    // Create random number generator.
    TRandom3* rand = new TRandom3();

    for (int i = 0; i < nTrig; i++)
    {
        // Create trigger hadron flat in eta, phi. 
        Hadron * h_trig = new Hadron();
        h_trig->SetEta( rand->Uniform(-1., 1.) );
    //    h_trig->SetPhi(rand->Uniform(0. 2 * Pi));
    }


    h_trig->FillEta();
    h_trig->SaveHists(&f_out);

    f_out->Close();




    /*
    Jet * jet = new Jet();
    double delta_phi = rand->Gaus(Pi, 0.5);
    jet->SetDeltaPhi(delta_phi);
    */
}
