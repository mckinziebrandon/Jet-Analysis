// runEMCalJetAnalysis.C
// =====================
// This macro can be used to run a jet analysis within the EMCal Jet Framework.
//
// Examples:
// -> Analyze ESDs from the pA pilot run on the AliEn grid with your task in AnaClass.cxx/.h
//     dataType = "ESD", useGrid = kTRUE, pattern = "*ESDs/pass2/*ESDs.root", addCXXs = "AnaClass.cxx", 
//     addHs = "AnaClass.h", gridDir = "/alice/data/2012/LHC12g", gridMode = "full", runNumbers = "188359 188362"
//     
// -> Analyze AODs (up to 96 files) locally given in files_aod.txt
//     dataType = "AOD", useGrid = kFALSE, numLocalFiles = 96
//
// MERGING ON ALIEN
// ++++++++++++++++
// If you run on the grid, you can monitor the jobs with alimonitor.cern.ch. When enough of them are in DONE state,
// you have to merge the output. This can be done automatically, if you just change the gridMode to "terminate" and
// give the EXACT name of the task whose output should be merged in uniqueName.
// 
//
// Authors: R. Haake, S. Aiola

#include <ctime>
#include "TGrid.h"

AliAnalysisGrid* CreateAlienHandler(const char* uniqueName, const char* gridDir, const char* gridMode, const char* runNumbers, 
                                     const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker, 
                                     Int_t workerTTL, Bool_t isMC);




TString localFiles("./listEmptyESD.list"); //list of 10000 empty esd files
const Int_t  nEvents        = 1000; //number of events to be processed
 
//______________________________________________________________________________
void runEMCalJetAnalysis(
         Double_t e_cms = 5020.,     //cms energy of pythia event
         Double_t ptminHard = 50.,   //lower bias of the  Q^2 of the process
         Double_t ptmaxHard = 1000., //upper bias  of the  Q^2 of the process 
         Bool_t         useGrid     = kFALSE, // local or grid
         const char*    gridMode    = "local", // set the grid run mode (can be "full", "test", "offline", "submit" or "terminate")
         const char*    dataType    = "ESD", // set the analysis type, AOD, ESD or sESD
         UInt_t         numLocalFiles   = 1, // number of files analyzed locally
         const char*    uniqueName      = "EMCalFK_LEGOTrainTest",// sets base string for the name of the train on the grid
         // Here you have to specify additional code files you want to use but that are not in aliroot
         const char*    addCXXs = "AliAnalysisTaskHJetSpectraM.cxx",
         const char*    addHs   = "AliAnalysisTaskHJetSpectraM.h",

         // These two settings depend on the dataset and your quotas on the AliEN services
         Int_t  maxFilesPerWorker   = 4,
         Int_t  workerTTL           = 7200
         )
{

  //THIS PART YOU DO NOT NEED TO TOUCH 

  if(useGrid) localFiles ="alien:///alice/cern.ch/user/f/fkrizek/emptyesd/listEmptyESD.list";

  TString runPeriod = "";
  // Some pre-settings and constants
  gSystem->SetFPEMask();
  gSystem->Setenv("ETRAIN_ROOT", ".");
  gSystem->Setenv("ETRAIN_PERIOD", runPeriod);
  // change this objects to strings
  TString usedData(dataType);
  TString additionalCXXs(addCXXs);
  TString additionalHs(addHs);

  cout << dataType << " analysis chosen" << endl;
  if (useGrid)  
  {
    cout << "-- using AliEn grid.\n";
    if (usedData == "sESD") 
    {
      cout << "Skimmed ESD analysis not available on the grid!" << endl;
      return;
    }
  }
  else
    cout << "-- using local analysis.\n";
  


  // Load necessary libraries
  LoadLibs();

  // Create analysis manager
  AliAnalysisManager* mgr = new AliAnalysisManager(uniqueName);


  // Check type of input and create handler for it
 
  if(usedData == "AOD"){
     localFiles = "files_aod.txt";
     gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddAODHandler.C");
     AliAODInputHandler* aodH = AddAODHandler();
  }else if((usedData == "ESD") || (usedData == "sESD")){
     gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddESDHandler.C");
     AliESDInputHandler* esdH = AddESDHandler();
  }else{
     cout << "Data type not recognized! You have to specify ESD, AOD, or sESD!\n";
  }


   if(!useGrid)
     cout << "Using " << localFiles.Data() << " as input file list.\n";
   
   
   Bool_t isMC =  kTRUE ;
   
   // Create MC handler, if MC is demanded
   if(isMC && (usedData != "AOD")){
      AliMCEventHandler* mcH = new AliMCEventHandler();
      mcH->SetPreReadMode(AliMCEventHandler::kLmPreRead);
      mcH->SetReadTR(kTRUE);
      mgr->SetMCtruthEventHandler(mcH); 
   }
  
  // ################# Now: Add some basic tasks

   TString mcParticles =  "MCParticlesSelected";

   //______________________________________________________________________________________ 
   // THIS IS SOMETHING YOU SHOULD PAY ATTENTION:
   //  PYTHIA GENERATOR INITIALIZATION
 
   AliGenerator *generPythia = 0x0;
   // Generate Pythia events on-the-fly
   Double_t minEtaGen = -5.;
   Double_t maxEtaGen = 5.;
      
   gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/train/AddMCGenPythia.C");
   generPythia = AddMCGenPythia(e_cms,ptminHard,ptmaxHard,2); // 2 = Perugia2011
     
   //In case you would have and asymmetric system  like p+Pb you need to boost cms 
   // generPythia->SetDyBoost(0.465); // dy for boost into lab frame
   // generPythia->SetUseLorentzBoost(true); // flag if lorentz boost should be applied

   //task that creates branch with pythia particles
   gROOT->LoadMacro("$ALICE_PHYSICS/PWGJE/EMCALJetTasks/macros/AddTaskJetEmbeddingFromGen.C");
    AliJetEmbeddingFromGenTask *genTask = AddTaskJetEmbeddingFromGen(generPythia,
                                                                      mcParticles.Data(),
                                                                      "EmbFromGenTask",
                                                                      0., 1e6,  //min Pt max Pt
                                                                      minEtaGen,maxEtaGen,//default -0.9,0.9,
                                                                      0., TMath::TwoPi(), //min max phi
                                                                      kTRUE,kTRUE); //copy array,  draw QA
   genTask->SetSuffix("");
   genTask->SetMarkMC(0);
   genTask->SetGeometryName("EMCAL_COMPLETE12SMV1"); 
 

   //#########################################################################
   // THIS IS SOMETHING YOU SHOULD PAY ATTENTION:
   // HERE YOU SET UP YOUR TASK

   gROOT->LoadMacro("AliAnalysisTaskHJetSpectraM.cxx++g");
   gROOT->LoadMacro("AddTaskHJetSpectra.C");
   AliAnalysisTaskHJetSpectraM* anaTask =AddTaskHJetSpectra(
                                                         0.4,
                                                         "XYS" //container tag 
                                                      );

   anaTask->SetDebugLevel(0); //make this number nonzer if you want to have debuging messages
 
   //Second task can be defined here if needed
   //AliAnalysisTaskHJetSpectraM* anaTask2 =AddTaskHJetSpectra(
   //                                                         0.5,
   //                                                         "UVW", //container tag 
   //                                                      );
   //anaTask2->SetDebugLevel(0);
 

   //####################################################################################
   // THIS PART YOU DO NOT NEED TO TOUCH
   // Set the physics selection for all given tasks
  TObjArray *toptasks = mgr->GetTasks();

  mgr->SetUseProgressBar(1, 25);
        
  if (!mgr->InitAnalysis()) 
    return;
  mgr->PrintStatus();

  if (useGrid) 
  {  // GRID CALCULATION

    AliAnalysisGrid *plugin = CreateAlienHandler(uniqueName, gridDir, gridMode, runNumbers, pattern, additionalCXXs, additionalHs, maxFilesPerWorker, workerTTL, isMC);
    mgr->SetGridHandler(plugin);

    // start analysis
    cout << "Starting GRID Analysis...";
    mgr->SetDebugLevel(0);
    mgr->StartAnalysis("grid");
  }
  else
  {  // LOCAL CALCULATION

    TChain* chain = 0;
    if (usedData == "AOD") 
    {
      gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateAODChain.C");
      chain = CreateAODChain(localFiles.Data(), numLocalFiles);
    }
    else
    {  // ESD or skimmed ESD
      gROOT->LoadMacro("$ALICE_PHYSICS/PWG/EMCAL/macros/CreateESDChain.C");
      chain = CreateESDChain(localFiles.Data(), numLocalFiles);
    }
    
    // start analysis
    cout << "Starting LOCAL Analysis...";
    mgr->SetDebugLevel(0);
    mgr->StartAnalysis("local", chain, nEvents);
  }
}

//______________________________________________________________________________
void LoadLibs()
{
  // Load common libraries (better too many than too few)
  gSystem->Load("libTree");
  gSystem->Load("libVMC");
  gSystem->Load("libGeom");
  gSystem->Load("libGui");
  gSystem->Load("libXMLParser");
  gSystem->Load("libMinuit");
  gSystem->Load("libMinuit2");
  gSystem->Load("libProof");
  gSystem->Load("libPhysics");
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libOADB");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libCDB");
  gSystem->Load("libRAWDatabase");
  gSystem->Load("libSTEER");
  gSystem->Load("libEVGEN");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libCORRFW");
  gSystem->Load("libTOFbase");
  //gSystem->Load("libTOFrec");
  gSystem->Load("libRAWDatabase");
  gSystem->Load("libRAWDatarec");
  gSystem->Load("libTPCbase");
  gSystem->Load("libTPCrec");
  gSystem->Load("libITSbase");
  gSystem->Load("libITSrec");
  gSystem->Load("libTRDbase");
  gSystem->Load("libTender");
  gSystem->Load("libSTAT");
  gSystem->Load("libTRDrec");
  gSystem->Load("libHMPIDbase");
  gSystem->Load("libPWGPP");
  gSystem->Load("libPWGHFbase");
  gSystem->Load("libPWGDQdielectron");
  //gSystem->Load("libPWGHFhfe");
  gSystem->Load("libEMCALUtils");
  gSystem->Load("libPHOSUtils");
  gSystem->Load("libPWGCaloTrackCorrBase");
  gSystem->Load("libEMCALraw");
  gSystem->Load("libEMCALbase");
  gSystem->Load("libEMCALrec");
  gSystem->Load("libTRDbase");
  gSystem->Load("libVZERObase");
  gSystem->Load("libVZEROrec");
  gSystem->Load("libTender");
  gSystem->Load("libTenderSupplies");
  gSystem->Load("libPWGTools");
  gSystem->Load("libPWGEMCAL");
  gSystem->Load("libESDfilter");
  gSystem->Load("libPWGGAEMCALTasks");
  gSystem->Load("libPWGCFCorrelationsBase");
  gSystem->Load("libPWGCFCorrelationsDPhi");

  gSystem->Load("libpythia6_4_25"); //EMBEDDING
  gSystem->Load("libqpythia"); //EMBEDDING
  gSystem->Load("libAliPythia6"); //EMBEDDING
  gSystem->Load("libEGPythia6.so"); //KINE
  gSystem->Load("libpythia8.so"); //KINE
  gSystem->Load("libAliPythia8.so"); //KINE



  //load CGAL, Fastjet and SISCone
  gSystem->Load("libCGAL");
  gSystem->Load("libfastjet");
    //For FastJet 3.x use siscon*,fastjetplugins for 2.x use SISConePlugin
  //  gSystem->Load("libSISConePlugin");
  // gSystem->Load("libCDFConesPlugin");
  gSystem->Load("libsiscone");
  gSystem->Load("libsiscone_spherical");
  gSystem->Load("libfastjetplugins");
  gSystem->Load("libfastjettools");
  gSystem->Load("libfastjetcontribfragile");
  //
  gSystem->Load("libJETAN");
//  gSystem->Load("libFASTJETAN");
  gSystem->Load("libPWGJEEMCALJetTasks");


  gSystem->AddIncludePath("-Wno-deprecated");
  gSystem->AddIncludePath("-I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_ROOT/EMCAL -I$ALICE_PHYSICS -I$ALICE_PHYSICS/include");
  gSystem->AddIncludePath("-I$ALICE_PHYSICS/JETAN -I$ALICE_PHYSICS/PWG -I/home/filip/alicesw/fastjet/3.0.6_1.012/lib");
  gSystem->AddIncludePath("-I/home/filip/alicesw/fastjet/3.0.6_1.012/include");

}

AliAnalysisGrid* CreateAlienHandler(const char* uniqueName, const char* gridDir, const char* gridMode, const char* runNumbers, 
                                     const char* pattern, TString additionalCode, TString additionalHeaders, Int_t maxFilesPerWorker, 
                                     Int_t workerTTL, Bool_t isMC)
{
  TDatime currentTime;
  TString tmpName(uniqueName);

  // Only add current date and time when not in terminate mode! In this case the exact name has to be supplied by the user
  if(strcmp(gridMode, "terminate"))
  {
    tmpName += "_";
    tmpName += currentTime.GetDate();
    tmpName += "_";
    tmpName += currentTime.GetTime();
  }

  TString tmpAdditionalLibs("");
  tmpAdditionalLibs = Form("libTree.so libVMC.so libGeom.so libGui.so libXMLParser.so libMinuit.so libMinuit2.so libProof.so libPhysics.so libSTEERBase.so libESD.so libAOD.so libOADB.so libANALYSIS.so libCDB.so libRAWDatabase.so libSTEER.so libANALYSISalice.so libCORRFW.so libTOFbase.so libRAWDatabase.so libRAWDatarec.so libTPCbase.so libTPCrec.so libITSbase.so libITSrec.so libTRDbase.so libTender.so libSTAT.so libTRDrec.so libHMPIDbase.so libPWGPP.so libPWGHFbase.so libPWGDQdielectron.so  libEMCALUtils.so libPHOSUtils.so libPWGCaloTrackCorrBase.so libEMCALraw.so libEMCALbase.so libEMCALrec.so libTRDbase.so libVZERObase.so libVZEROrec.so libTender.so libTenderSupplies.so libESDfilter.so libPWGTools.so libPWGEMCAL.so libPWGGAEMCALTasks.so libPWGCFCorrelationsBase.so libPWGCFCorrelationsDPhi.so libpythia6_4_25.so libqpythia.so libAliPythia6.so libCGAL.so libJETAN.so libfastjet.so libsiscone.so libsiscone_spherical.so libfastjetplugins.so libfastjettools.so libfastjetcontribfragile.so libPWGJE.so libPWGmuon.so libPWGJEEMCALJetTasks.so %s %s",additionalCode.Data(),additionalHeaders.Data());

/*libPWGHFhfe.so*/
  TString macroName("");
  TString execName("");
  TString jdlName("");
  macroName = Form("%s.C", tmpName.Data());
  execName = Form("%s.sh", tmpName.Data());
  jdlName = Form("%s.jdl", tmpName.Data());

  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  plugin->SetOverwriteMode();
  plugin->SetRunMode(gridMode);
     
  // Here you can set the (Ali)ROOT version you want to use
  plugin->SetAPIVersion("V1.1x");
  plugin->SetROOTVersion("v5-34-30");
  plugin->SetAliROOTVersion("master");

  plugin->SetGridDataDir(gridDir); // e.g. "/alice/sim/LHC10a6"
  plugin->SetDataPattern(pattern); //dir structure in run directory
  if (!isMC)
   plugin->SetRunPrefix("000");

  plugin->AddRunList(runNumbers);

  plugin->SetGridWorkingDir(Form("work/%s",tmpName.Data()));
  plugin->SetGridOutputDir("output"); // In this case will be $HOME/work/output

  plugin->SetAnalysisSource(additionalCode.Data());
  plugin->SetAdditionalLibs(tmpAdditionalLibs.Data());
  plugin->AddExternalPackage("boost::v1_53_0");
  plugin->AddExternalPackage("cgal::v4.4");
  plugin->AddExternalPackage("fastjet::v3.0.6_1.012");

  plugin->SetDefaultOutputs(kTRUE);
  //plugin->SetMergeExcludes("");
  plugin->SetAnalysisMacro(macroName.Data());
  plugin->SetSplitMaxInputFileNumber(maxFilesPerWorker);
  plugin->SetExecutable(execName.Data());
  plugin->SetTTL(workerTTL);
  plugin->SetInputFormat("xml-single");
  plugin->SetJDLName(jdlName.Data());
  plugin->SetPrice(1);      
  plugin->SetSplitMode("se");

  return plugin;
}
