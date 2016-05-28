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
#include "./include/EventGenerator.h"

// --------------------------------------------------------------------------------------------
void CompareJetR(Int_t nEvents = 1000) {
    std::vector<Float_t> jetRadii {0.2, 0.3, 0.4, 0.5};

    const Int_t nRadii = jetRadii.size();
    TFile* fHists[nRadii];
    Color_t col[] = {kCyan-2, kBlue-4, kGreen-2, kRed-2};
    
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

    TH1 *hPt[nRadii], *hEta[nRadii], *hPhi[nRadii], 
        *hPtSub[nRadii], *hNJets[nRadii], 
        *hArea[nRadii], *hAreaCut[nRadii];
    THStack* pTStack = new THStack("pTStack", ";p_{T};#frac{1}{N_{evt}}#frac{dN}{dp_{T}}");
    THStack* pTSubStack = new THStack("pTSubStack", ";p_{T} - #rho A;#frac{1}{N_{evt}}#frac{dN}{dp_{T}}");
    THStack* etaStack = new THStack("etaStack", ";#eta;#frac{1}{N_{evt}}#frac{dN}{d#eta}");
    THStack* phiStack = new THStack("phiStack", ";#varphi;#frac{1}{N_{evt}}#frac{dN}{d#varphi}");
    THStack* areaStack = new THStack("areaStack", ";Jet Area;#frac{1}{N_{evt}}#frac{dN}{dA}");
    THStack* nJetsStack = new THStack("nJetsStack", ";Num Jets per Event;Counts");
    for(Int_t i = 0; i < nRadii; i++) {
        Float_t R = jetRadii[i];
        const char* fileName = Form("./rootFiles/TM_N%d_R%d.root", nEvents, (int) (R * 10));
        TFile* fTrees        = new TFile(fileName, "READ");
        TDirectory* dirJets	 = (TDirectory*) fTrees->Get("jetPlots;1");
        TTree* tEventJetInfo = (TTree*) dirJets->Get("tEventJetInfo;1");
        TTree* tJetInfo      = (TTree*) dirJets->Get("tJetInfo;1");


        Double_t A = pi * TMath::Power(R, 2);
        TCut areaCut = Form("area > %f", A / 2.0);

        tEventJetInfo->Draw("n>>hNJets");       
        tJetInfo->Draw("area>>hArea");
        tJetInfo->Draw("area>>hAreaCut", areaCut);
        hNJets[i] = (TH1F*) gDirectory->Get("hNJets");
        hArea[i] = (TH1F*) gDirectory->Get("hArea");
        hAreaCut[i] = (TH1F*) gDirectory->Get("hAreaCut");

        //TCut areaCut = Form("area > %f", areaMin);
        tJetInfo->Draw(Form("ptSub>>hPtSub_R%d", (int) (R * 10)), areaCut);
        tJetInfo->Draw(Form("pt>>hPt_R%d", (int) (R * 10)), areaCut);
        tJetInfo->Draw(Form("eta>>hEta_R%d", (int) (R * 10)), areaCut);    //, "ptSub>0.0");
        tJetInfo->Draw(Form("phi>>hPhi_R%d", (int) (R * 10)), areaCut);
        hPt[i]      = (TH1F*) gDirectory->Get(Form("hPt_R%d", (int) (R * 10)));  
        hEta[i]     = (TH1F*) gDirectory->Get(Form("hEta_R%d", (int) (R * 10))); 
        hPhi[i]     = (TH1F*) gDirectory->Get(Form("hPhi_R%d", (int) (R * 10))); 
        hPtSub[i]   = (TH1F*) gDirectory->Get(Form("hPtSub_R%d", (int) (R * 10)));

        Float_t norm = 1.0 / (Float_t) nEvents;
        hPt[i]->Scale(norm, "width");
        hEta[i]->Scale(norm, "width");
        hPhi[i]->Scale(norm, "width");
        hPtSub[i]->Scale(norm, "width");
        hArea[i]->Scale(norm, "width");
   //     hAreaCut[i]->Scale(norm, "width");

        SetDrawOptions(hPt[i], col[i]);
        SetDrawOptions(hEta[i], col[i]);
        SetDrawOptions(hPhi[i], col[i]);

        SetDrawOptions(hPtSub[i], col[i]);
        SetDrawOptions(hNJets[i], col[i]); 
        SetDrawOptions(hArea[i], col[i]);
//        SetDrawOptions(hAreaCut[i], col[i]);

        if (i == 0) {
            hPt[i]->SetLineStyle(3);
            hEta[i]->SetLineStyle(3);
            hPhi[i]->SetLineStyle(3);
            hPtSub[i]->SetLineStyle(3);
            hArea[i]->SetLineStyle(3);
            hNJets[i]->SetLineStyle(3);
        }

        pTStack->Add(hPt[i]);
        etaStack->Add(hEta[i]);
        phiStack->Add(hPhi[i]);

        pTSubStack->Add(hPtSub[i]);
        nJetsStack->Add(hNJets[i]);
        areaStack->Add(hArea[i]);
    }

    fOut->Write();

    /* ------------------------------------------------------------ *
     * Drawing and Saving.                                          *
     * ------------------------------------------------------------ */

    // ----------------- PTSUB -----------------
    TCanvas* cPtSub = new TCanvas("cPtSub", "canvas", 1000, 500);
    cPtSub->cd();
    cPtSub->cd()->SetLogy(1);
    cPtSub->cd()->SetLeftMargin(0.14);
    pTSubStack->Draw("nostack");
    pTSubStack->GetXaxis()->CenterTitle();
    pTSubStack->GetYaxis()->CenterTitle();
    pTSubStack->GetYaxis()->SetTitleOffset(1.8);
    TLegend* legPtSub = new TLegend(0.7, 0.7, 0.9, 0.9);
    legPtSub->SetBorderSize(0);
    for(Int_t i = 0; i < nRadii; i++) {
        legPtSub->AddEntry(hPtSub[i], Form("R = %.1f; A > %.2f", jetRadii[i], jetRadii[i] / 2.0), "l");
        legPtSub->SetTextSize(0.04);
    }
    legPtSub->AddEntry((TObject*)0, Form("nEvents: %d", nEvents), "");
    legPtSub->AddEntry((TObject*)0, Form("|#eta^{track}| < %.1f", etaMax), "");
    legPtSub->Draw("same");
    Double_t y2 = hPtSub[1]->GetBinContent(hPtSub[1]->FindBin(0.));
    DrawLine(Point(0., 0.), Point(0., y2), (TPad*) cPtSub->cd());

    cPtSub->WaitPrimitive();

    // ----------------- NUM JETS -----------------
    TCanvas* cNumJets = new TCanvas("cNumJets", "canvas", 600, 600);
    cNumJets->cd();
    nJetsStack->Draw("nostack,C");
    nJetsStack->GetXaxis()->CenterTitle();
    nJetsStack->GetYaxis()->CenterTitle();
    nJetsStack->GetYaxis()->SetTitleOffset(1.3);
    TLegend* legNJets = new TLegend(0.7, 0.7, 0.9, 0.9);
    legNJets->SetBorderSize(0);
    for(Int_t i = 0; i < nRadii; i++) {
        legNJets->AddEntry(hNJets[i], Form("R = %.1f; A > %.2f", jetRadii[i], jetRadii[i] / 2.0), "l");
        legNJets->SetTextSize(0.035);
    }
    legNJets->AddEntry((TObject*)0, Form("nEvents: %d", nEvents), "");
    legNJets->AddEntry((TObject*)0, Form("|#eta^{track}| < %.1f", etaMax), "");
    legNJets->Draw();

    cNumJets->WaitPrimitive();

    // ----------------- AREA -----------------
    TCanvas* cArea = new TCanvas("cArea", "canvas", 600, 600);
    cArea->cd();
    cArea->cd(3)->SetLogy(1);
    cArea->cd(3)->SetLeftMargin(0.15);
    areaStack->Draw("nostack");
    areaStack->GetXaxis()->CenterTitle();
    areaStack->GetYaxis()->CenterTitle();
    areaStack->GetYaxis()->SetTitleOffset(2.);
    TLegend* legArea = new TLegend(0.6, 0.65, 0.9, 0.9);
    legArea->SetBorderSize(0);
    for(Int_t i = 0; i < nRadii; i++) {
        legArea->AddEntry(hArea[i], Form("R = %.1f; A > %.2f", jetRadii[i], jetRadii[i] / 2.0), "l");
        legArea->SetTextSize(0.035);

//        hAreaCut[i]->SetFillColor(col[i]);
//        hAreaCut[i]->SetFillStyle(3003);
//        hAreaCut[i]->Draw("same");

        // Set y2 = pi * R^2
        Double_t expectedA = pi * TMath::Power(jetRadii[i], 2);
        cout << "pi * r^2 = " << expectedA << endl;
        y2 = hArea[i]->GetBinContent(hArea[i]->FindBin(expectedA));
        DrawLine(Point(expectedA, 0.), Point(expectedA, y2), (TPad*) cArea->cd(3));

        TLatex lineLabel;
        lineLabel.SetTextAlign(12); // centered
        lineLabel.SetTextSize(0.03);
        lineLabel.DrawLatex(0.85 * expectedA, 1.2 * y2, Form("#pi(%.1f)^{2}", jetRadii[i]));

    }
    legArea->AddEntry((TObject*)0, Form("nEvents: %d", nEvents), "");
    legArea->AddEntry((TObject*)0, Form("|#eta^{track}| < %.1f", etaMax), "");
    legArea->Draw("same");

    cArea->WaitPrimitive();

    // _____________________ Basic Canvas: pT, Eta, Phi _____________________
    TCanvas* cBasic = new TCanvas("cBasic", "canvas", 800, 500);
    cBasic->Divide(3, 1); Int_t iPad = 1;

    // ----------------- PT -----------------
    cBasic->cd(iPad++)->SetLogy(1); 
    pTStack->Draw("nostack");
    pTStack->GetXaxis()->CenterTitle();
    pTStack->GetYaxis()->CenterTitle();
    TLegend* legPt = new TLegend(0.6, 0.75, 0.95, 0.95);
    legPt->SetBorderSize(0);
    for(Int_t i = 0; i < nRadii; i++) {
        legPt->AddEntry(hPt[i], Form("R = %.1f", jetRadii[i]), "l");
        legPt->SetTextSize(0.035);
    }
    legPt->AddEntry((TObject*)0, Form("nEvents: %d", nEvents), "");
    legPt->AddEntry((TObject*)0, Form("Jet A > %.2f", areaMin), "");
    legPt->AddEntry((TObject*)0, Form("|#eta^{track}| < %.1f", etaMax), "");
    legPt->Draw();

    // ----------------- ETA -----------------
    cBasic->cd(iPad++); 
    etaStack->Draw("nostack");
    etaStack->GetXaxis()->CenterTitle();
    etaStack->GetYaxis()->CenterTitle();
    TLegend* legEta = new TLegend(0.65, 0.75, 0.95, 0.95);
    legEta->SetBorderSize(0);
    for(Int_t i = 0; i < nRadii; i++) {
        legEta->AddEntry(hEta[i], Form("R = %.1f", jetRadii[i]), "l");
        legEta->SetTextSize(0.035);
    }
    legEta->AddEntry((TObject*)0, Form("Num Events: %d", nEvents), "");
    legEta->AddEntry((TObject*)0, Form("Jet Area > %.2f", areaMin), "");
    //legEta->AddEntry((TObject*)0, Form("p_{T}^{jet} - #rho A > %d", 0), "");
    legEta->Draw();

    // ----------------- PHI -----------------
    cBasic->cd(iPad++); phiStack->Draw("nostack");
    phiStack->GetXaxis()->CenterTitle();
    phiStack->GetYaxis()->CenterTitle();
    TLegend* legPhi = new TLegend(0.65, 0.75, 0.95, 0.95);
    legPhi->SetBorderSize(0);
    for(Int_t i = 0; i < nRadii; i++) {
        legPhi->AddEntry(hPhi[i], Form("R = %.1f", jetRadii[i]), "l");
        legPhi->SetTextSize(0.035);
    }
    legPhi->AddEntry((TObject*)0, Form("Num Events: %d", nEvents), "");
    legPhi->AddEntry((TObject*)0, Form("Jet Area > %.2f", areaMin), "");
    legPhi->Draw();

    cBasic->WaitPrimitive();
    fOut->cd();
    cBasic->Write();
    cPtSub->Write();
    cNumJets->Write();
    cArea->Write();

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
    theAPP.Terminate();
    return 0;
}
#endif

