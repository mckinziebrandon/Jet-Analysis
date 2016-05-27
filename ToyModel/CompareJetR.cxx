/********************************************************************************************
* Filename:     CompareJetR.cxx                                                              *
* Date Created: April 1, 2016                                                               *
* Author:       Brandon McKinzie                                                            *
* Description:  Given raw output of ToyModel, turn into pretty plots.                       *
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include "./include/RootClasses.h"
#include "./include/MyJetFinder.h"

// --------------------------------------------------------------------------------------------
void CompareJetR(Int_t nEvents = 1000) {
    std::vector<Float_t> jetRadii {0.2, 0.3, 0.4, 0.5};

    const Int_t nRadii = jetRadii.size();
    TFile* fHists[nRadii];
    Color_t col[] = {kCyan-2, kBlue-5, kMagenta-2, kRed-2};
    
    cout << " ===================================================================\n"
         << "                    BEGINNING COMPARE JET R.                  \n" 
         << " NUMBER OF EVENTS: " << nEvents << "\n"
         << " =================================================================== " 
         << endl;

    TFile* fOut = new TFile(Form("./rootFiles/CompareJetR_%d.root", nEvents), "RECREATE");
    fOut->cd();

    /* ------------------------------------------------------------ *
     * Store input file data into trees.                            *
     * ------------------------------------------------------------ */

    TH1 *hPt[nRadii], *hEta[nRadii], *hPhi[nRadii], *hPtSub[nRadii];
    THStack* pTStack = new THStack("pTStack", ";p_{T};Counts");
    THStack* pTSubStack = new THStack("pTSubStack", ";p_{T} - #rho A;Counts");
    THStack* etaStack = new THStack("etaStack", ";#eta;Counts");
    THStack* phiStack = new THStack("phiStack", ";#varphi;Counts");
    for(Int_t i = 0; i < nRadii; i++) {
        // Grab phase space trees from ToyModel.C output. 
        fHists[i] = new TFile(Form("./rootFiles/PF_N%d_R%d.root", nEvents, (int) (jetRadii[i] * 10)));
        cout << "Opened file: " << fHists[i]->GetName() << endl;

        hPt[i]  = (TH1F*) fHists[i]->Get("hPt;1");  hPt[i]->SetLineColor(col[i]);
        hEta[i] = (TH1F*) fHists[i]->Get("hEta;1"); hEta[i]->SetLineColor(col[i]);
        hPhi[i] = (TH1F*) fHists[i]->Get("hPhi;1"); hPhi[i]->SetLineColor(col[i]);
        hPtSub[i] = (TH1F*) fHists[i]->Get("hPtSubWithACut;1");  hPtSub[i]->SetLineColor(col[i]);

        pTStack->Add(hPt[i]);
        pTSubStack->Add(hPtSub[i]);
        etaStack->Add(hEta[i]);
        phiStack->Add(hPhi[i]);
    }

    fOut->Write();

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    TCanvas* cPtSub = new TCanvas("cPtSub", "canvas", 600, 600);
    cPtSub->cd()->SetLogy(1);
    pTSubStack->Draw("nostack");
    pTSubStack->GetXaxis()->CenterTitle();
    pTSubStack->GetYaxis()->CenterTitle();
    TLegend* legPtSub = new TLegend(0.7, 0.7, 0.9, 0.9);
    legPtSub->SetHeader(Form("p_{T} - #rho A; %d events.", nEvents));
    for(Int_t i = 0; i < nRadii; i++) {
        legPtSub->AddEntry(hPtSub[i], Form("R = %.1f", jetRadii[i]), "l");
        legPtSub->SetTextSize(0.04);
    }
    legPtSub->Draw();
    Double_t y2 = hPtSub[1]->GetBinContent(hPtSub[1]->FindBin(0.));
    DrawLine(Point(0., 0.), Point(0., y2), (TPad*) cPtSub->cd());

    cPtSub->WaitPrimitive();

    
    TCanvas* cBasic = new TCanvas("cBasic", "canvas", 800, 500);
    cBasic->Divide(3, 1); Int_t iPad = 1;

    cBasic->cd(iPad++)->SetLogy(1); 
    pTStack->Draw("nostack");
    pTStack->GetXaxis()->CenterTitle();
    pTStack->GetYaxis()->CenterTitle();
    TLegend* legPt = new TLegend(0.7, 0.7, 0.9, 0.9);
    legPt->SetHeader(Form("Jet p_{T}; %d events.", nEvents));
    for(Int_t i = 0; i < nRadii; i++) {
        legPt->AddEntry(hPt[i], Form("R = %.1f", jetRadii[i]), "l");
        legPt->SetTextSize(0.04);
    }
    legPt->Draw();

    cBasic->cd(iPad++); 
    etaStack->Draw("nostack");
    etaStack->GetXaxis()->CenterTitle();
    etaStack->GetYaxis()->CenterTitle();
    TLegend* legEta = new TLegend(0.7, 0.7, 0.9, 0.9);
    legEta->SetHeader(Form("Jet #eta; %d events.", nEvents));
    for(Int_t i = 0; i < nRadii; i++) {
        legEta->AddEntry(hEta[i], Form("R = %.1f", jetRadii[i]), "l");
        legEta->SetTextSize(0.04);
    }
    legEta->Draw();

    cBasic->cd(iPad++); phiStack->Draw("nostack");
    phiStack->GetXaxis()->CenterTitle();
    phiStack->GetYaxis()->CenterTitle();
    TLegend* legPhi = new TLegend(0.7, 0.7, 0.9, 0.9);
    legPhi->SetHeader(Form("Jet #varphi; %d events.", nEvents));
    for(Int_t i = 0; i < nRadii; i++) {
        legPhi->AddEntry(hPhi[i], Form("R = %.1f", jetRadii[i]), "l");
        legPhi->SetTextSize(0.04);
    }
    legPhi->Draw();

    //cBasic->WaitPrimitive();
    fOut->cd();
    cBasic->Write();
    cPtSub->Write();

}

#ifndef __CINT__
int main(int argc, char* argv[]) 
{
    // Needed for incredibly obscure reason. 
    TApplication theAPP("App", &argc, argv);
    if (argc == 2) {
       CompareJetR(atoi(argv[1]));
    } else {
        CompareJetR();
    }
    return 0;
}
#endif

