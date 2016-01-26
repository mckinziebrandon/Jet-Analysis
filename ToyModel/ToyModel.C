#include "TH2F.h"
#include "TGenPhaseSpace.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TLatex.h"
#include "TF1.h"
#include "TFile.h"
#include "TRandom3.h"

Double_t eps=0.00001;
Bool_t Simulate_2_2=1;

// ---------- Constants ----------
const Float_t   pi = TMath::Pi();
const Double_t  meanN = 3;
const Double_t  sigmaN = 0.1;
const Int_t     nevents = 1000;
const Int_t     nmpi = 3;
const Int_t     sizemixed = 1;          //roughly the size of mixed event distribution
const Double_t  beam_energy = 6500;     //in GeV
const Double_t  parton_mass = 0.3;      //in GeV
// -------------------------------

TLorentzVector *t;
TLorentzVector *a;

Float_t dphi(Float_t phi1, Float_t phi2, 
             Float_t phimin = -0.5 * pi,
             Float_t phimax = 1.5*pi);

// --------------------------------------------------------------------------------------------
void TestGenPhaseSpace()
{
    TGenPhaseSpace event_parton;
    TGenPhaseSpace event_particles;
    TRandom3* rand = new TRandom3();
    TH2F* hdphidetaS = new TH2F("hdphidetaS","hdphidetaS;dphi;deta", 40,-0.5*pi,1.5*pi,60,-3,3);
    TH2F* hdphidetaM = new TH2F("hdphidetaM","hdphidetaM;dphi;deta", 40,-0.5*pi,1.5*pi,60,-3,3);
    TH2F* hphieta    = new TH2F("hphieta","hphieta;phi;eta",    40,-pi,pi,60,-3,3);
    TH2F* hpteta     = new TH2F("hpteta","hpteta;pt;eta",    100,0.,1.,60,-3,3);
    hpteta->SetBit(TH1::kCanRebin);

    /* Parton distribution function.                                          ** 
    ** pdf(i, x, u):  i == parton index                                       **
    **                x == fraction of proton's momentum carried by parton    **
    **                u == factorization scale (approx. momentum transfer q)  **
    ** Parameters in approximation to gluon pdf below:                        **
    **    [0] == n = 4.28                                                     **
    **    [1] == -b = -1.77                                                   **
    **    [2] == a = 6.06                                                     **
    ** --> Factorization scale of mu=175GeV for low x.                        */
    TF1* pdf = new TF1("pdf","1./[0] * x^[1] * (1-x)^[2]",0,1);
    pdf->SetParameters(6.06,1.77,4.28);

    Int_t NtotM=0;                        //entries in pool
    Int_t SizeM=2*sizemixed*nmpi*meanN;   //size of mixed event pool
    TClonesArray vMixed("TLorentzVector",SizeM);

    for(Int_t iev = 0; iev < nevents; iev++)
    {
        // Print status in increments of 10 percent.
        if (ie v% (nevents / 10) == 0) 
            Printf("Event=%i   %.2lf%% done", iev, Double_t(iev) / Double_t(nevents) * 100.);

        TClonesArray vSame("TLorentzVector", nmpi * meanN);
        Int_t NtotS = vSame.GetEntriesFast();

        for(Int_t impi = 0; impi < nmpi; impi++)
        {
            // Simulate 2->2 process. 
            // Set momentum/energy of random parton's fraction of beam energy. 
            Double_t p_proj = pdf->GetRandom() * beam_energy;
            Double_t p_targ = pdf->GetRandom() * beam_energy;
            Double_t E_proj = TMath::Sqrt(p_proj * p_proj + parton_mass * parton_mass);
            Double_t E_targ = TMath::Sqrt(p_targ * p_targ + parton_mass * parton_mass);
            Double_t masses[2] ={parton_mass, parton_mass};

            // Create, then combine, the corresponding momentum four-vectors.
            TLorentzVector projectile_parton(0.0, 0.0,  p_proj, E_proj);
            TLorentzVector target_parton(0.0, 0.0,  -p_targ, E_targ);
            TLorentzVector W = projectile_parton + target_parton;

            // Define scattering process. 
            event_parton.SetDecay(W, 2, masses);

            // Generate a random final state of particles and return weight of current event.
            Double_t weight_parton = event_parton.Generate();
            for(Int_t iparton = 0; iparton < 2; iparton++){
                if(iparton!=0 && !Simulate_2_2)continue;

                // Obtain 4-vector of ith decay product.
                TLorentzVector* parton = event_parton.GetDecay(iparton);
                TLorentzVector W_parton = TLorentzVector(parton->Px(),parton->Py(),parton->Pz(),parton->E());

                // Simulate N-body decay
                Int_t nparticles= (Int_t)rand->Gaus(meanN,sigmaN);
                if(nparticles <= 1)continue;


                Double_t masses_particles[nparticles];
                for(Int_t i = 0; i < nparticles; i++){
                    // Set masses of nparticles to pion mass.
                    masses_particles[i]=0.139 ;
                }

                // Generate N-particle decay.
                if(Simulate_2_2){
                    // Create N-particle pion decay from iparton.
                    event_particles.SetDecay(W_parton, nparticles, masses_particles);
                    Double_t weight_particles = event_particles.Generate();
                }else{
                    // Create N-particle pion decay from initial collision.
                    event_particles.SetDecay(W, nparticles, masses_particles);
                    Double_t weight_particles = event_particles.Generate();
                }

                //loop particles
                for(Int_t it = 0; it < nparticles; it++){
                    TLorentzVector* t = event_particles.GetDecay(it);

                    if(TMath::IsNaN(t->Pt())) continue;

                    //fill same event array
                    new(vSame[NtotS++]) TLorentzVector(t->Px(),t->Py(),t->Pz(),t->E());

                    //fill correlations - M
                    Int_t iaBeginM=0;//start from 0

                    if(vMixed.GetEntriesFast() > SizeM)
                        // Only take the last two events.
                        iaBeginM = vMixed.GetEntriesFast() - SizeM;
                    for(Int_t ia=iaBeginM;ia<vMixed.GetEntriesFast();ia++){
                        a = (TLorentzVector*)vMixed.ConstructedAt(ia);
                        hdphidetaM->Fill(dphi(t->Phi(),a->Phi()),t->Eta()-a->Eta());
                    }//end loop mixed
                }//end loop nparticles
            }//end loop npartons
    }//end loop MPI


    NtotM=vMixed.GetEntriesFast();
    for(Int_t it=0;it<vSame.GetEntriesFast();it++){
        //loop over particles in one event
        t=(TLorentzVector*)vSame.ConstructedAt(it);
        hphieta->Fill(t->Phi(),t->Eta());
        hpteta->Fill(t->Pt(),t->Eta());
        //fill mixed events array
        new(vMixed[NtotM++]) TLorentzVector(t->Px(),t->Py(),t->Pz(),t->E());
        //fill correlations - S
        for(Int_t ia=it+1;ia<vSame.GetEntriesFast();ia++){
            a=(TLorentzVector*)vSame.ConstructedAt(ia);
            hdphidetaS->Fill(dphi(t->Phi(),a->Phi()),t->Eta()-a->Eta());
        }
    }

    //    Printf("NtotM %d, NTotS %d",vMixed.GetEntriesFast(),vSame.GetEntriesFast());
    //remove old events from mixed distr
    //    NtotM=vMixed.GetEntriesFast();
    //    if(NtotM>SizeM){
    //      vMixed.RemoveRange(0,NtotM-SizeM);
    //    }
    //    Printf("size M after %d",vMixed.GetEntriesFast());
    //    Printf("----- %d,   %d\n",vMixed.GetEntriesFast(),vMixed.GetEntriesFastFast());

    vSame.Clear();
  }//event loop
  vMixed.Clear();

  TCanvas *c_single_particle=new TCanvas("cphideta","cphideta",1200,1200);
  c_single_particle->Divide(2,2);
  c_single_particle->cd(1);
  hphieta->DrawCopy("colz");
  c_single_particle->cd(2);
  hpteta->DrawCopy("colz");
  c_single_particle->cd(3);
  pdf->Draw();
  TCanvas *c_2pc=new TCanvas("c_2pc","c_2pc",1200,1200);
  c_2pc->Divide(2,2);
  c_2pc->cd(1);
  hdphidetaS->DrawCopy("surf1");
  c_2pc->cd(2);
  hdphidetaM->DrawCopy("surf1");
  c_2pc->cd(3);
  hdphidetaS->Divide(hdphidetaM);
  hdphidetaS->DrawCopy("surf1");
  c_2pc->cd(4);
  TH1F *p1=(TH1F*)hdphidetaS->ProjectionX("p1",1,hdphidetaS->GetYaxis()->FindBin(-1.4-eps));
  TH1F *p2=(TH1F*)hdphidetaS->ProjectionX("p2",hdphidetaS->GetYaxis()->FindBin(-1.4+eps),hdphidetaS->GetYaxis()->FindBin(1.4-eps));
  TH1F *p3=(TH1F*)hdphidetaS->ProjectionX("p3",hdphidetaS->GetYaxis()->FindBin(1.4+eps),hdphidetaS->GetNbinsY());
  p1->Scale(1./p1->Integral());
  p2->Scale(1./p2->Integral());
  p3->Scale(1./p3->Integral());
  p1->SetLineColor(2);
  p2->SetLineColor(4);
  p3->SetLineColor(1);
  p1->DrawCopy("");
  p2->DrawCopy("same");
  p3->DrawCopy("same");
  TLatex* latex =new TLatex();
  latex->SetTextSize(0.04);
  latex->SetTextFont(42);
  latex->SetTextAlign(21);
  latex->SetNDC();
  latex->DrawLatex(0.35,0.8,Form("# events %d",nevents));
  latex->DrawLatex(0.35,0.75,Form("# mpi per ev %d",nmpi));
  latex->DrawLatex(0.35,0.7,Form("# part per mpi %.0f",meanN));
  latex->DrawLatex(0.35,0.65,Form("sigma # part per mpi %.0f",sigmaN));

  TFile *fout=new TFile(Form("%dmpi_%.0fpart.root",nmpi,meanN),"recreate");
  hdphidetaS->Write();
  fout->Close();
  delete fout;

}


Float_t dphi(Float_t phi1,Float_t phi2,Float_t phimin,Float_t phimax){
  Float_t dphi = phi1-phi2;
  if (dphi > phimax) dphi -= 2*pi;
  if (dphi < phimin) dphi += 2*pi;
  return dphi;
}
