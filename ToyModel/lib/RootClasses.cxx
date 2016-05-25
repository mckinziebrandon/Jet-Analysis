#include "../include/RootClasses.h"

TLine* DrawLine(Point p1, Point p2, TPad* pad) {
    pad->cd();
    TLine* line = new TLine(p1.X, p1.Y, p2.X, p2.Y);
    line->SetLineStyle(2);
    line->SetLineColor(12); // Dark gray. 
    line->SetLineWidth(2);
    line->Draw("same");
    return line;
}


void DrawLegend(LegendList items, TPad* pad) {
    pad->cd();
    pad->SetTicks(1, 1);
    TLegend* leg = new TLegend(0.55, 0.7, 0.95, 0.9);
    for (auto i : items) {
        if (i.first == (TObject*)0)   leg->AddEntry(i.first, i.second, "");
        else                    leg->AddEntry(i.first, i.second, "l");
    }
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    leg->SetFillStyle(1001); // Solid 
    leg->Draw("same");
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


void Draw(TGraph* g, TCanvas* c, Int_t& iPad) {
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


