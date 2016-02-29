#ifndef ALIANALYSISTASKSE_H

#include <Riostream.h>
#include <TROOT.h>
#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TKey.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH1I.h>
#include <TArrayF.h>
#include <TArrayD.h>
#include <THnSparse.h>
#include <TCanvas.h>
#include <TList.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TMath.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include "AliAnalysisTask.h"
#include "AliCentrality.h"
#include "AliStack.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliAODEvent.h"
#include "AliAODHandler.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisHelperJetTasks.h"
#include "AliParticleContainer.h"
#include "AliInputEventHandler.h"
#endif

#include <time.h>
#include <TRandom3.h>
#include "AliGenEventHeader.h"
#include "AliGenPythiaEventHeader.h"
#include "AliGenHijingEventHeader.h"
#include "AliAODMCHeader.h"
#include "AliMCEvent.h"
#include "AliLog.h"
#include <AliEmcalJet.h>
#include <AliPicoTrack.h>
#include "AliVEventHandler.h"
#include "AliVParticle.h"
#include "AliAODMCParticle.h"
#include "AliAnalysisUtils.h"
#include "AliRhoParameter.h"
#include "TVector3.h"
#include "AliVVertex.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "AliJetContainer.h"
#include "AliAnalysisTaskEmcal.h"
#include "AliAnalysisTaskEmcalJet.h"
#include "AliAnalysisTaskHJetSpectraM.h"
#include "AliHeader.h" 
#include "AliRunLoader.h"  
using namespace std;

// ANALYSIS OF HIGH PT HADRON TRIGGER ASSOCIATED SPECTRUM OF RECOIL JETS IN P+PB
// Author Filip Krizek   (23.Feb. 2016)

ClassImp(AliAnalysisTaskHJetSpectraM)
//________________________________________________________________________________________

AliAnalysisTaskHJetSpectraM::AliAnalysisTaskHJetSpectraM(): 
AliAnalysisTaskEmcalJet("AliAnalysisTaskHJetSpectraM", kTRUE), 
 fSignalJetRadius(0.4),
fSignalJetEtaWindow(0.9 - fSignalJetRadius), fTrackEtaWindow(0.9), fMinTrackPt(0.150), fMinJetArea(0.0), 
 fhTrackPt(0x0), fhJetPt(0x0), fhJetConstituentPt(0x0),
 fhVertexZMC(0x0), fhVertexZAcceptMC(0x0)
{
   //default constructor
   
}

//________________________________________________________________________
AliAnalysisTaskHJetSpectraM::AliAnalysisTaskHJetSpectraM(const char *name) : 
AliAnalysisTaskEmcalJet(name,kTRUE),  
fSignalJetRadius(0.4), 
fSignalJetEtaWindow(0.9 - fSignalJetRadius), fTrackEtaWindow(0.9), fMinTrackPt(0.150), fMinJetArea(0.0),    fhTrackPt(0x0), fhJetPt(0x0), fhJetConstituentPt(0x0),
  fhVertexZMC(0x0), fhVertexZAcceptMC(0x0)
{
//Constructor
 
   DefineOutput(1, TList::Class());
}
  
//________________________________________________________________________
Double_t AliAnalysisTaskHJetSpectraM::GetSimPrimaryVertex(){
   //get information about generator level primary vertex

   AliGenEventHeader* mcHeader = NULL; 

   AliRunLoader *rl = AliRunLoader::Instance();
   if(rl)  mcHeader = dynamic_cast<AliGenPythiaEventHeader*>(rl->GetHeader()->GenEventHeader());

   if(mcHeader){
      
      TArrayF pyVtx(3); //x,y,z coordiantes of the primary vertex
      mcHeader->PrimaryVertex(pyVtx); //this is how to read them
      return (Double_t) (pyVtx[2]); //return z vertex position of the primary vertex
   }
   AliWarning(Form("In task %s: Pythia Vertex failed!", GetName()));
   return 9999.0;
}

//________________________________________________________________________
Bool_t AliAnalysisTaskHJetSpectraM::IsMCEventInAcceptance(AliVEvent* event){
   //EVENT SELECTION PURE MC,  MAKE CUT on VERTEX POSITION 

   if(!event) return kFALSE;

   //___________________________________________________

   Double_t vtxMC = GetSimPrimaryVertex();
   fhVertexZMC->Fill(vtxMC); //Fill BEFORE vertex cut

   if(TMath::Abs(vtxMC) > 10.){  //accept events with vertex within +- 10 cm
      return kFALSE;
   }
   fhVertexZAcceptMC->Fill(vtxMC);//Fill AFTER vertex cut

   return kTRUE;
   
}

//________________________________________________________________________
Bool_t AliAnalysisTaskHJetSpectraM::IsTrackInAcceptance(AliVParticle* track){
   // Check if the track pt and eta range 
   if(!track) return kFALSE;

   if(!track->Charge()) return kFALSE; //check if the particle is charged
   if(TMath::Abs(track->Eta()) <= fTrackEtaWindow){ //accept particles |eta|<0.9
      if(track->Pt() >= fMinTrackPt){   // accept only particles with pT>150 MeV
         return kTRUE;
      }
   }
   return kFALSE; //the particile did not meet criteria
}


//________________________________________________________________________
Bool_t AliAnalysisTaskHJetSpectraM::IsSignalJetInAcceptance(AliEmcalJet *jet, Bool_t suppressGhost){   
   //select jets that are in acceptance
   //there is an option to exclude ghost jets (ie artificial jets created by fastjet from
   //ghost tracks 
   if(!jet) return kFALSE;
   if(TMath::Abs(jet->Eta()) <= fSignalJetEtaWindow){
      if(jet->Area() >= fMinJetArea){
         if(suppressGhost){ //ghost jets have typically pT<<1 , so pT cut removes them completely 
            if(jet->Pt() >= fMinTrackPt) return kTRUE;
         }else{
            return kTRUE;
         }
      }
   }  
   return kFALSE;
}



//________________________________________________________________________
Bool_t AliAnalysisTaskHJetSpectraM::FillHistograms(){ 
   // this is the main function of the object where you analysis is to be done 
   // it is executed in each event 
   //it is called from AliAnalysisTaskEmcal::UserExec(Option_t *)
   //Here you analyze the event and Fill histograms

   if(!InputEvent()){
      AliError("??? Event pointer == 0 ???");
      return kFALSE;
   }



   //_________________________________________________________________
   // CHECK EVENT VERTEX 

   if(!IsMCEventInAcceptance(InputEvent())) return kFALSE; //post data is in UserExec


   //_________________________________________________________________
   // JET+TRACK CONTAINERS
   AliJetContainer *jetContGen   = NULL; //AKT jet container from  MC particles
   AliJetContainer *jetContGenKT = NULL; //KT jet container from  MC particles

   AliEmcalJet  *jetGen = NULL;  //jet pointer MC jet

   AliParticleContainer *parContGen = NULL; //particle container of MC particles

   AliVParticle *constTrackGen = NULL; //jet constituent
   //_________________________________________________________
   //READ JET TRACK CONTAINERS
      
   jetContGen   = GetJetContainer(kContainerOne);  //AKT jets
   parContGen   = GetParticleContainer(kContainerOne); //kine particle container 

   jetContGenKT =  GetJetContainer(kContainerTwo); //KT jets
 

   //_________________________________________________________

   //This is an exaple how to loop over tracks in your event
   if(parContGen){ 
      parContGen->ResetCurrentID();
      while((constTrackGen = (AliVParticle*)  parContGen->GetNextAcceptParticle())){ 
         if(!constTrackGen) continue;
       
         if(IsTrackInAcceptance(constTrackGen)){ 
            fhTrackPt->Fill(constTrackGen->Pt());  //inclusive pT spectrum of tracks
         }
      }
   } 

   //_________________________________________________________
   //This is an exaple how to loop over jets and jet track constituents 
   if(jetContGen){
      jetContGen->ResetCurrentID();
      while((jetGen = jetContGen->GetNextAcceptJet())) {
         if(!jetGen){
            AliError(Form("%s: Could not receive gen jet", GetName()));
            continue;
         }
         if(!IsSignalJetInAcceptance(jetGen,kTRUE)) continue; //skip ghost jets
                 
         //areaJet = jetGen->Area();
         fhJetPt->Fill(jetGen->Pt());                 

         //This is an example how to loop over jet constituents
         for(Int_t iq=0; iq < jetGen->GetNumberOfTracks(); iq++) {
            constTrackGen = (AliVParticle*) (jetGen->TrackAt(iq,parContGen->GetArray())); 
            if(!constTrackGen) continue;
            fhJetConstituentPt->Fill(jetGen->Pt());
         }
      }
   }
   

   return kTRUE;
}

//________________________________________________________________________
void AliAnalysisTaskHJetSpectraM::Terminate(Option_t *){
   //Treminate 
   PostData(1, fOutput);

   // Mandatory
   fOutput = dynamic_cast<TList*> (GetOutputData(1)); // '1' refers to the output slot
   if(!fOutput) {
      printf("ERROR: Output list not available\n");
      return;
   }
}

//________________________________________________________________________
AliAnalysisTaskHJetSpectraM::~AliAnalysisTaskHJetSpectraM(){
   // Destructor. Clean-up the output list, but not the histograms that are put inside
   // (the list is owner and will clean-up these histograms). Protect in PROOF case.
   if(fOutput && !AliAnalysisManager::GetAnalysisManager()->IsProofMode()) {
      delete fOutput;
   }
 
} 

//________________________________________________________________________
void AliAnalysisTaskHJetSpectraM::UserCreateOutputObjects(){
  // called once to create user defined output objects like histograms, plots etc. 
  // and to put it on the output list.
  // Note: Saving to file with e.g. OpenFile(0) is must be before creating other objects.
  //fOutput TList defined in the mother class
   AliAnalysisTaskEmcalJet::UserCreateOutputObjects();


   Bool_t oldStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   TString name;

   //-------------------------
   name = "fhTrackPt"; //TRACK PT 
   fhTrackPt = new TH1D(name.Data(),"TRACK pT", 50, 0, 50);
   fOutput->Add(fhTrackPt);
   //-------------------------
   name ="fhJetPt";  //JET PT
   fhJetPt = new TH1D(name.Data(),"JET PT", 50,0, 100);
   fOutput->Add(fhJetPt);
   //-------------------------
   name = "fhJetConstituentPt";  //JET TRACK CONSTITUENT PT
   fhJetConstituentPt = new TH1D(name.Data(),"pT of JET CONSTITUENTS", 50, 0, 50);
   fOutput->Add(fhJetConstituentPt);
   //-------------------------
   name = "fhVertexZMC";
   fhVertexZMC = new TH1D(name.Data(),"z vertex",40,-20,20);
   fOutput->Add(fhVertexZMC);
   //-------------------------
   name = "fhVertexZAcceptMC ";
   fhVertexZAcceptMC = new TH1D(name.Data(),"z vertex after cut",40,-20,20);
   fOutput->Add(fhVertexZAcceptMC);
  
   // =========== Switch on Sumw2 for all histos ===========
   for(Int_t i=0; i<fOutput->GetEntries(); i++){
      TH1 *h1 = dynamic_cast<TH1*>(fOutput->At(i));
      if(h1){
         h1->Sumw2(); //apply Poissonian error propagation
         continue;
      }
      THnSparse *hn = dynamic_cast<THnSparse*>(fOutput->At(i));
      if(hn){
         hn->Sumw2();
      }
   }
   TH1::AddDirectory(oldStatus);


   PostData(1, fOutput);
}
//________________________________________________________________________
Bool_t AliAnalysisTaskHJetSpectraM::RetrieveEventObjects() {
   //
   // retrieve event objects
   //
    if(!AliAnalysisTaskEmcalJet::RetrieveEventObjects())  return kFALSE;
 
   return kTRUE;
}
//________________________________________________________________________
Bool_t AliAnalysisTaskHJetSpectraM::Run()
{
   // Run analysis code here, if needed. It will be executed before FillHistograms().
   
   return kTRUE;
}

