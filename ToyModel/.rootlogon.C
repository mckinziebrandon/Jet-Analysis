{/*********************************************
Name: Brandon McKinzie
Project: .rootlogon.C

Assignment:
Setup All User Customizations
*********************************************/

// BrandonStyle Defines Default Style Configuration
TStyle* BrandonStyle = new TStyle("BrandonStyle","Brandon's Root Styles");

// Default Graph Options
BrandonStyle->SetOptStat(0);
BrandonStyle->SetOptDate(0);
BrandonStyle->SetLabelSize(0.03,"xyz"); // size of axis value font
BrandonStyle->SetTitleSize(0.035,"xyz"); // size of axis title font
BrandonStyle->SetTitleFont(22,"xyz"); // font option
BrandonStyle->SetLabelFont(22,"xyz");
BrandonStyle->SetTitleOffset(1.2,"y");

// Default Pad Options

BrandonStyle->SetPadLeftMargin(0.15);
BrandonStyle->SetPadBorderMode(0);
BrandonStyle->SetCanvasBorderMode(0);
BrandonStyle->SetFrameBorderMode(0);
BrandonStyle->SetCanvasColor(kWhite);

// Default Object Options
BrandonStyle->SetDrawOption("col");

// Set Colors
const Int_t NRGBs = 5;
const Int_t NCont = 256;
Int_t MyPalette[NCont];
Double_t stops[NRGBs] = { 0.00, 0.30, 0.61, 0.84, 1.00 };
Double_t red[NRGBs] = { 0.00, 0.00, 0.57, 0.90, 0.51 };
Double_t green[NRGBs] = { 0.00, 0.65, 0.95, 0.20, 0.00 };
Double_t blue[NRGBs] = { 0.51, 0.55, 0.15, 0.00, 0.10 };
Int_t FI = TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue,NCont);
for (Int_t i=0;i<NCont;i++) MyPalette[i] = FI+i;
BrandonStyle->SetPalette(256, MyPalette);
BrandonStyle->SetNumberContours(256);

// Finish Configuration
gROOT->SetStyle("BrandonStyle");
cout << "Styles are Set!" << endl;
return;
}

