#ifndef ALIANALYSISTASKHJETSPECTRAm_H
#define ALIANALYSISTASKHJETSPECTRAm_H


class TH1I;
class TH1F;
class TH2F;
class TH2D;
class TH1D;
class TArrayD;
class THnSparse;
class TProfile;
class TList;
class TClonesArray;
class TString;
class AliEmcalJet;
class AliRhoParameter;
class AliVParticle;
class AliLog;
class AliAnalysisUtils;
class TRandom3;
class AliJetContainer;
class AliParticleContainer;
class AliClusterContainer;

#include <vector>
#include "AliAnalysisTaskEmcalJet.h"

using std::vector;
// "SIMPLE" EXAMPLE HOW TO USE PYTHIA + FAST JET  UNDER ALIROOT
// Author Filip Krizek   (23.Feb. 2016)

class AliAnalysisTaskHJetSpectraM : public AliAnalysisTaskEmcalJet {
   public:

  enum MyContainer {
     kContainerOne = 0, //analyze real data 
     kContainerTwo = 1, //analyze monte carlo
     kContainerThree,   //KT real 
     kContainerFour    //KT MC
  };


   // ######### CONTRUCTORS/DESTRUCTORS AND STD FUNCTIONS
   AliAnalysisTaskHJetSpectraM();
   AliAnalysisTaskHJetSpectraM(const char *name);
   virtual ~AliAnalysisTaskHJetSpectraM();
   void     UserCreateOutputObjects();
   void     Terminate(Option_t *);

   
  // ######### SETTERS/GETTERS
  void        SetSignalJetMinArea(Double_t minArea) { fMinJetArea = minArea;} 
  void        SetMinTrackPt(Double_t minPt){ fMinTrackPt = minPt;}

  void        SetAcceptanceWindows(Double_t trackEta, Double_t signalJetRadius){ 
                 fTrackEtaWindow  = trackEta; 
                 fSignalJetRadius = signalJetRadius; 
                 fSignalJetEtaWindow = fTrackEtaWindow-fSignalJetRadius; 
              } 

  Double_t GetSimPrimaryVertex(); //get z coordinate of the primary vertex

  //#  SOME STANDARD FUNCTIONS
  Bool_t   RetrieveEventObjects();
  Bool_t   Run();
  Bool_t   FillHistograms(); //this is the function whetre you do your analysis

 private:

  // ######### CHECK FUNCTIONS
  Bool_t      IsTrackInAcceptance(AliVParticle* track);  //check track properties
  Bool_t      IsSignalJetInAcceptance(AliEmcalJet* jet, Bool_t suppressGhost=1); //check jet protperties 
  Bool_t      IsMCEventInAcceptance(AliVEvent* event); //check if the primary vertex is withing +-10cm


  // ########## CUTS 
  Double_t            fSignalJetRadius;       // Radius for the signal jets
  Double_t            fSignalJetEtaWindow;    // +- window in eta for signal jets 
  Double_t            fTrackEtaWindow;        //gc +- window in eta for tracks  
  Double_t            fMinTrackPt;            //gc Min track pt to be accepted  
  Double_t            fMinJetArea;            // Min jet area to be accepted


  // ########## HISTOGRAMS

   TH1D               *fhTrackPt;  //! track pt  (mind the "//!", this means not to make streamer) 
   TH1D               *fhJetPt; //! jet pt 
   TH1D               *fhJetConstituentPt;  //! jet constituent track pt 

   TH1D  *fhVertexZMC;        // zvertex distribution of generated events
   TH1D  *fhVertexZAcceptMC; //zvertex distribution of accepted  events that fulfill vertex cut


  AliAnalysisTaskHJetSpectraM(const AliAnalysisTaskHJetSpectraM&);
  AliAnalysisTaskHJetSpectraM& operator=(const AliAnalysisTaskHJetSpectraM&);

  ClassDef(AliAnalysisTaskHJetSpectraM, 1); // Charged jet analysis for pA

};
#endif
