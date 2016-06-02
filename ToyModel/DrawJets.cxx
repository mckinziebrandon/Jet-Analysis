/********************************************************************************************
* Filename:     
* Date Created: June 1, 2016                                                               *
* Author:       Brandon McKinzie                                                            *
* Description:  
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include "./include/RootClasses.h"
#include "./include/MyJetFinder.h"
#include "./include/EventGenerator.h"

// Store values of R we want to analyze.
const std::vector<Float_t> jetRadii {0.2, 0.3, 0.4, 0.5};
const int nRadii = 4;
const Color_t col[] = {kCyan-2, kBlue-4, kGreen-2, kRed-2};

// Store hist pointers globally to greatly reduce number of lines of code easily.
TH1 *hPt[nRadii], *hEta[nRadii], *hPhi[nRadii];
TH1 *hPtSub[nRadii], *hNJets[nRadii];
TH1 *hArea[nRadii], *hAreaCut[nRadii];
TH1 *hRho[nRadii];

// Function prototypes.
template<typename Obj>
void AdjustTitles(Obj* o, Float_t offset=1.3);
void PrintHeader(Int_t nEvents=1000);
void NormalizeAll(Int_t i, Float_t norm=1.0);
void SetDrawOptionsAll(Int_t i, Color_t c);

// --------------------------------------------------------------------------------------------
void DrawJets(Int_t nEvents=1000) {

    PrintHeader(nEvents);

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */


    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 800);
    canvas->Divide(2, 2);

    for (int i = 0; i < nRadii; i++) {

        const Float_t R = jetRadii[i];
        const char* fileName = Form("./rootFiles/TM_N%d_R%d.root", nEvents, (int) (R * 10));
        TFile* fTrees        = new TFile(fileName, "READ");
        TDirectory* dirJets	 = (TDirectory*) fTrees->Get("jetPlots;1");
        TTree* tJetInfo      = (TTree*) dirJets->Get("tJetInfo;1");

        canvas->cd(i + 1);
        tJetInfo->Draw("eta:area","", "surf2");
    }

    gPad->WaitPrimitive();

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TFile* outFile = new TFile("./rootFiles/DrawJets.root", "RECREATE");
    outFile->cd();
    canvas->Write();
    
}

#ifndef __CINT__
int main(int argc, char* argv[]) 
{
    // Needed for incredibly obscure reason. 
    TApplication theAPP("App", &argc, argv);
    if (argc == 2) {
       DrawJets(atoi(argv[1]));
    } else {
        DrawJets();
    }
    theAPP.Terminate();
    return 0;
}
#endif

/***********************************************************************************
                                Helper Functions
***********************************************************************************/

template<typename Obj>
void AdjustTitles(Obj* o, Float_t offset) {
    o->GetXaxis()->CenterTitle();
    o->GetYaxis()->CenterTitle();
    o->GetYaxis()->SetTitleOffset(offset);
}

void PrintHeader(Int_t nEvents) {
    cout << " ===================================================================\n"
         << "                    BEGINNING COMPARE JET R.                  \n" 
         << " NUMBER OF EVENTS: " << nEvents << "\n"
         << " =================================================================== " 
         << endl;
}

void NormalizeAll(Int_t i, Float_t norm) {
    hPt[i]->Scale(norm, "width");
    hEta[i]->Scale(norm, "width");
    hPhi[i]->Scale(norm, "width");
    hPtSub[i]->Scale(norm, "width");
    hArea[i]->Scale(norm, "width");
    hAreaCut[i]->Scale(norm, "width");
    hRho[i]->Scale(norm, "width");
}

void SetDrawOptionsAll(Int_t i, Color_t c) {
    SetDrawOptions(hPt[i], c);
    SetDrawOptions(hEta[i], c);
    SetDrawOptions(hPhi[i], c);
    SetDrawOptions(hPtSub[i], c);
    SetDrawOptions(hNJets[i], c); 
    SetDrawOptions(hArea[i], c);
    SetDrawOptions(hAreaCut[i], c);
    SetDrawOptions(hRho[i], c);
}

