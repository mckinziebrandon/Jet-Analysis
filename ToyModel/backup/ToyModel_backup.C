/********************************************************************************************
* Filename:     ToyModel.C                                                                  *
* Date Created: January 25, 2016                                                            *
* Author:       Leonardo Milano                                                             *
* Edited by:    Brandon McKinzie                                                            *
* Description:  Simple model event generator for multiple parton scattering.                *
*               Serves as introduction to toy models for me (Brandon) and                   *
*               two-particle correlation study.                                             *
********************************************************************************************/

#include "./include/RootClasses.h"
#include "./include/ToyModel.h"
#include <iostream>
#include <fstream>
Double_t eps=0.00001;

// ---------- Constants ----------
const Double_t  meanN = 3;              // Mean number of particles produced in N-body decay.
const Double_t  sigmaN = 0.1;           // Width of N-body decay.
const Int_t     nevents = 1000;         // Number of projectile-target collisions.
const Int_t     nParton = 3;            // Number of multi-parton interactions.
const Int_t     sizemixed = 1;          //roughly the size of mixed event distribution
const Double_t  beam_energy = 6500;     //in GeV
const Double_t  parton_mass = 0.3;      //in GeV
// -------------------------------

TLorentzVector* trig;
TLorentzVector* assoc;

// --------------------------------------------------------------------------------------------
void ToyModel()
{
    // Create file for object output tests.
    std::ofstream f_debug("./debug/debug_ToyModel.txt");

    // Create objects to store n-body event, with constant cross section.
    TGenPhaseSpace incoming_partons;
    TGenPhaseSpace final_particles;

    // Phase space: trigger.
    TH1F* h_eta_trig = new TH1F("h_eta_trig", "Trigger Eta;eta;counts", 20, -1., 1.);
    TH1F* h_phi_trig = new TH1F("h_phi_trig", "Trigger Phi;phi;counts", 40, -pi, pi);
    TH1F* h_pt_trig  = new TH1F("h_pt_trig", "Trigger p_{T};p_{T}(GeV/c);counts", 100, 0., 100.);

    // Phase space: associated.
    TH1F* h_eta_assoc = new TH1F("h_eta_assoc", "Trigger Eta;eta;counts", 20, -1., 1.);
    TH1F* h_phi_assoc = new TH1F("h_phi_assoc", "Trigger Phi;phi;counts", 40, -pi, pi);
    TH1F* h_pt_assoc  = new TH1F("h_pt_assoc", "Trigger p_{T};p_{T}(GeV/c);counts", 100, 0., 30.);

    // Phase space: background.
    TH1F* h_eta_bkg = new TH1F("h_eta_bkg", "Trigger Eta;eta;counts", 20, -1., 1.);
    TH1F* h_phi_bkg = new TH1F("h_phi_bkg", "Trigger Phi;phi;counts", 40, -pi, pi);
    TH1F* h_pt_bkg  = new TH1F("h_pt_bkg", "Trigger p_{T};p_{T}(GeV/c);counts", 100, 0., 30.);

    // Correlation histograms for same and mixed events.
    TH2F *hdphidetaS = new TH2F("hdphidetaS","hdphidetaS;dphi;deta", 40,-0.5*pi,1.5*pi,60,-3,3);
    TH2F *hdphidetaM = new TH2F("hdphidetaM","hdphidetaM;dphi;deta", 40,-0.5*pi,1.5*pi,60,-3,3);

    /***************************************************************************** 
    **   Parton distribution function.                                          ** 
    ** pdf(i, x, u):  i == parton index                                         **
    **                x == fraction of proton's momentum carried by parton      **
    **                u == factorization scale (approx. momentum transfer q)    **
    ** Parameters in approximation to gluon below:                              **
    **    [0] == n = 4.28                                                       **
    **    [1] == -b = -1.77                                                     **
    **    [2] == a = 6.06                                                       **
    ** --> Factorization scale of mu=175GeV for low x.                          **
    *****************************************************************************/ 
    TRandom3* rand = new TRandom3();
    TF1* pdf = new TF1("pdf","1./[0] * x^[1] * (1-x)^[2]",0,1);
    pdf->SetParameters(6.06,1.77,4.28);

    Int_t NtotM=0;                                      //entries in pool
    Int_t SizeM=2 * sizemixed * nParton * meanN;        //size of mixed event pool
    TClonesArray vMixed("TLorentzVector",SizeM);

    /* -------------------------------------------------
    | Loop: projectile-target collisions.               |
    ---------------------------------------------------*/
    for(Int_t i_event = 0; i_event < nevents; i_event++)
    {
        // Print status in increments of 10 percent.
        if (i_event % (nevents / 10) == 0)
        {
            Double_t percent_complete = Double_t(i_event) / Double_t(nevents) * 100.;
            Printf("Beginning event %i. Simulation is %.2lf%% complete.", i_event, percent_complete);
        }

        // Create
        TClonesArray vSame("TLorentzVector", nParton * meanN);
        
        /* "Number of total Scatterings".               **
        ** GetEntriesFast() "only OK when no gaps".     **
        ** Note: Always returns zero.                   */
        Int_t NtotS = vSame.GetEntriesFast();

        /* -------------------------------------------------
        | Loop: parton-parton scattering.                   |
        | Stage: par1 + par2 -> par3 + par4                 |
        ---------------------------------------------------*/
        for(Int_t i_parton = 0; i_parton < nParton; i_parton++)
        {
            // Set momentum(p)/energy of random parton's fraction of beam energy. 
            Double_t p_proj = pdf->GetRandom() * beam_energy;
            Double_t p_targ = pdf->GetRandom() * beam_energy;
            Double_t E_proj = TMath::Sqrt(p_proj * p_proj + parton_mass * parton_mass);
            Double_t E_targ = TMath::Sqrt(p_targ * p_targ + parton_mass * parton_mass);
            Double_t masses[2] ={parton_mass, parton_mass};

            // Create, then combine, the corresponding momentum four-vectors. (CM frame)
            TLorentzVector projectile_parton(0.0, 0.0,  p_proj, E_proj);
            TLorentzVector target_parton(0.0, 0.0,  -p_targ, E_targ);
            TLorentzVector W = projectile_parton + target_parton;

            /*********************************************************************************
            ** Define scattering process:                                                   **
            **      W: "Decay particle" determined by kinematics of colliding particles.    **
            **      2: Number of desired decay products.                                    **
            **      masses: array of decay PRODUCT masses.                                  **
            *********************************************************************************/
            incoming_partons.SetDecay(W, 2, masses);

            // Generate a random final state of particles and return weight of current event.
            // Note: "Decay products are finally boosted using betas of original particle."
            Double_t weight_parton = incoming_partons.Generate();

            /* ----------------------------------------------
            | Loop: decay products.                         |
            | Stage: par3(or 4) -> pion_1 + . . . + pion_n  |
            ------------------------------------------------*/
            for(Int_t i_dprod = 0; i_dprod < 2; i_dprod++)
            {

                // Obtain 4-vector of ith decay product.
                TLorentzVector* decay_parton = incoming_partons.GetDecay(i_dprod);

                // Simulate N-body decay
                // Note: This will return the number 2 or 3.
                // If typecasting rounded correctly, it would always be three.
                Int_t nparticles= (Int_t)rand->Gaus(meanN,sigmaN);

                // Set masses of nparticles to pion mass.
                Double_t masses_final[nparticles];
                for(Int_t i = 0; i < nparticles; i++)
                    masses_final[i]=0.139 ;

                // Generate N-particle decay.
                final_particles.SetDecay(*decay_parton, nparticles, masses_final);
                
                // Returns 1, -1 or nan.
                Double_t weight_particles = final_particles.Generate();
                
                /* ------------------------------------------
                | Loop: final-state particles.              |
                -------------------------------------------*/
                for(Int_t i_trig = 0; i_trig < nparticles; i_trig++)
                {
                    // Get final-state trigger hadron.
                    trig = (TLorentzVector*)final_particles.GetDecay(i_trig);

                    // Skip if trigger has bad Pt.
                    if(TMath::IsNaN(trig->Pt())) continue;

                    // fill same event array
                    new(vSame[NtotS++]) TLorentzVector(trig->Px(),trig->Py(),trig->Pz(),trig->E());

                    //fill correlations - M
                    Int_t iaBeginM=0;//start from 0

                    if(vMixed.GetEntriesFast() > SizeM)
                    {
                        // Only take the last two events.
                        iaBeginM = vMixed.GetEntriesFast() - SizeM;
                    }

                    /* ------------------------------------------
                    | Loop: 
                    -------------------------------------------*/
                    for(Int_t i_assoc = iaBeginM ; i_assoc < vMixed.GetEntriesFast(); i_assoc++)
                    {
                        // ConstructedAt only calls constructor once per slot. 
                        assoc = (TLorentzVector*)vMixed.ConstructedAt(i_assoc);
                    } //end loop mixed
                }//end loop nparticles
            }//end loop npartons
    }//end loop MPI


    NtotM = vMixed.GetEntriesFast();

    // Loop over full single event.
    for(Int_t i_trig = 0;i_trig < vSame.GetEntriesFast(); i_trig++)
    {
        // Fill trigger phase space histograms.
        trig = (TLorentzVector*)vSame.ConstructedAt(i_trig);
        h_eta_trig->Fill(trig->Eta());
        h_phi_trig->Fill(trig->Phi());
        h_pt_trig->Fill(trig->Pt());

        //fill correlations - S
        for(Int_t i_assoc= i_trig+1; i_assoc < vSame.GetEntriesFast(); i_assoc++)
        {
            assoc = (TLorentzVector*)vSame.ConstructedAt(i_assoc);
            hdphidetaS->Fill(dphi(trig->Phi(),assoc->Phi()),trig->Eta()-assoc->Eta());
        }
    }

    vSame.Clear();
  }//end event loop

  vMixed.Clear();

  TCanvas* c_phase_trig = new TCanvas("c_phase", "Phase Space Canvas", 700, 500);
  c_phase_trig->Divide(3, 1);

  c_phase_trig->cd(1);
  h_eta_trig->Draw();

  c_phase_trig->cd(2);
  h_phi_trig->Draw();

  c_phase_trig->cd(3);
  h_pt_trig->Draw();
      /*
  TLatex* latex =new TLatex();
  latex->SetTextSize(0.04);
  latex->SetTextFont(42);
  latex->SetTextAlign(21);
  latex->SetNDC();
  latex->DrawLatex(0.35,0.8,Form("# events %d",nevents));
  latex->DrawLatex(0.35,0.75,Form("# mpi per ev %d",nParton));
  latex->DrawLatex(0.35,0.7,Form("# part per mpi %.0f",meanN));
  latex->DrawLatex(0.35,0.65,Form("sigma # part per mpi %.0f",sigmaN));
  */

}

