#include "../include/RootClasses.h"
// -----------------------------------------------------------------------------
// Function Implementations
// -----------------------------------------------------------------------------

// Draws histogram in its own canvas and sets personal drawing options. 
template<typename Hist>
TCanvas* DrawHistogram(Hist* h)
{
    TCanvas* c = new TCanvas("c", "canvas from DrawHistogram", 700, 500);
    c->cd();
    h->Draw();
    TString x = h->GetXaxis()->GetTitle();
    TString y = h->GetYaxis()->GetTitle();
    SetDrawOptions(h, x, y);
    return c;
}

// Customizes appearance of a recently drawn histogram.
template<typename Hist>
void SetDrawOptions(Hist* h, TString x_label="", TString  y_label="")
{
    h->SetStats(0);
    h->SetFillColor(kBlue+3);
    h->SetLineColor(kBlue+3);
    h->SetLineWidth(2);
    h->SetFillStyle(0);
    if (x_label != "" && y_label != "")
    {
        h->SetTitle("");
        h->GetXaxis()->SetTitle(x_label.Data());
        h->GetYaxis()->SetTitle(y_label.Data());
    }
    h->GetXaxis()->SetTitleSize(0.05);
    h->GetYaxis()->SetTitleOffset(1.3);
    h->GetYaxis()->SetTitleSize(0.05);
    h->GetXaxis()->CenterTitle();
    h->GetYaxis()->CenterTitle();
}

// Draws n histograms from a single tree along a single row on a canvas. 
TCanvas* DrawHists(const Int_t n, TString index, TString names[], TTree* tree)
{
    TString id;
    TTree* t_trig = (TTree*)tree;

    id = "Canvas_";
    id += index;
    TCanvas* canvas = new TCanvas("canvas", id.Data(), 50, 50, 1000, 500);
    canvas->Divide(n, 1);

    for (int i = 0; i < n; i++)
    {
        id = "hist_";
        id += names[i];
        id += "_";
        id += index;

        TString treeDraw = names[i];
        treeDraw += ">>";
        treeDraw += id;

        canvas->cd(i+1);
        t_trig->Draw(treeDraw.Data(), "", "col");

        if (i != n-1)
        {
            TH1F* h = (TH1F*)gDirectory->Get(id.Data());

            id = index;
            id += " ";
            id += names[i];
            SetDrawOptions(h, TString(id.Data()), TString("counts")); 
        }
        else
        {
            TH2F* h = (TH2F*)gDirectory->Get(id.Data());

            TString x, y;
            x = index;
            x += " ";
            x += names[0];
            y = index;
            y += " ";
            y += names[1];
            SetDrawOptions(h, x, y); 
        }
    }
    canvas->Update();
    return canvas;
}