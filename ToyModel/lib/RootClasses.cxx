#include "../include/RootClasses.h"

template<typename Hist>
void SetDrawOptions(Hist* h, Color_t col, const char* x_label="", const char* y_label="") {
    h->SetStats(0);
    // Line and fill properties.
    h->SetFillColor(col);
    h->SetLineColor(col);
    h->SetLineWidth(2);
    h->SetFillStyle(0);
    // Axis properties.
    h->SetTitle("");
    if (strcmp(x_label, "") != 0) h->GetXaxis()->SetTitle(x_label);
    if (strcmp(y_label, "") != 0) h->GetYaxis()->SetTitle(y_label);
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleOffset(1.2);
    h->GetYaxis()->SetTitleSize(0.04);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->CenterTitle();
}

template<typename Hist>
void Draw(Hist* hModel, Hist* hData, TCanvas* c, Int_t& iPad) {
    if (iPad == 1) { c->cd(iPad)->SetLogy(1); }
    c->cd(iPad);
    hModel->Draw("same");
    hData->Draw("same");

}

void DrawLegend(TH1* hModel, TH1* hData, TGraph* g, TGraph* g2, TCanvas* c, Int_t& iPad) {
    c->cd(iPad);
    TLegend* legPt = new TLegend(0.6, 0.8, 0.9, 0.9);
    legPt->AddEntry(hModel, "Toy Model", "l");
    legPt->AddEntry(hData, "ALICE (Data)", "l");
    legPt->AddEntry(g, "ALICE (Paper)", "p");
    legPt->AddEntry(g2, "CMS (Paper)", "p");
    legPt->SetBorderSize(0);
    legPt->Draw();
}


// Draws the histogram on iPad of canvas, increments iPad by 1, and sets the draw options. 
//template<typename Hist>
void Draw(TH1* h, const char* xLabel, const char* yLabel, Color_t col, TCanvas* c, Int_t& iPad, bool setLog) {
    if (setLog && (iPad == 1 || iPad == 4)) { c->cd(iPad)->SetLogy(1); } // so janky
    c->cd(iPad++);
    h->Draw();
    SetDrawOptions(h, col, xLabel, yLabel);
}

template<typename Graph>
void Draw(Graph* g, TCanvas* c, Int_t& iPad) {
    c->cd(iPad);
    g->Draw("AP");
    g->GetXaxis()->CenterTitle();
    g->GetYaxis()->CenterTitle();
    g->SetMarkerColor(colors[3]);
    g->SetMarkerStyle(styles[0]);
}

void DrawRatio(TH1* hModDat, TH1* hModSamp, TCanvas* c) {
    c->cd(2)->SetLogy(0);
    c->cd(2);
    //hModDat->Draw("same");
    hModSamp->Draw("same");

    //SetDrawOptions(hModDat, colors[0], (TString) "p_{T} (GeV/c)", (TString) "p_{T} Ratio");
    SetDrawOptions(hModSamp, colors[0], (TString) "p_{T} (GeV/c)", (TString) "p_{T} Ratio");
    hModDat->SetFillColor(colors[1]);    //   hModDat->SetFillStyle(3001);
    hModSamp->SetFillColor(colors[3]);  //hModSamp->SetFillStyle(3001);

    TLegend * legRatio = new TLegend(0.6, 0.8, 0.9, 0.9);
    //legRatio->AddEntry(hModDat, "Model / Eff-Corrected Data", "lf");
    legRatio->AddEntry(hModSamp, "ALICE (Data) / ALICE (Paper)", "lf");
    legRatio->SetBorderSize(0);
    legRatio->Draw();
}

