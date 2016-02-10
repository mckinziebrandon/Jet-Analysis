class EventModel 
{
private:
    TF1* fPolynomial;
    TF1* fLine;
    TF1* fdNdPhi;
    TF1* fTrackSpectrum;
public:
    EventModel();
    Float_t dphi(Float_t p, Float_t p2); 
    Float_t GetTriggerPhi(Float_t pt, TF1* f);
    Float_t GetAssocPhi(Float_t tp, Float_t sp, TRandom3* r);
    inline Double_t GetTrackPt() { return fTrackSpectrum->GetRandom(); }
}


EventModel::EventModel() {
    // Extract ALICE data.
    fPolynomial    = new TF1("fPolynomial", "pol3", 0., 5.);
    fLine          = new TF1("fLine", "[0] + [1] * x", 5., 18.);
    fLine->SetParameter(0, 0.15);
    fLine->SetParameter(1, -0.1 / 10.);

    TFile* file_v2 = new TFile("./rootFiles/ALICE_v2pt.root");
    TGraph* g = (TGraphAsymmErrors*)file_v2->Get("v2Plot_2;1");
    g->Fit(fPolynomial, "RQ");
    g->Fit(fPolynomial, "RQ+");
    delete g;
    delete file_v2;

    // Set fdNdPhi.
    fdNdPhi = new TF1("fdNdPhi", "1 + 2 * [0] * TMath::Cos(2. * x)", 0., 2. * pi);
    fdNdPhi->SetParameter(0, 0.);
    fdNdPhi->SetParName(0, "v2");

    // Set fTrackSpectrum.
    TString funcString = "[0] * (TMath::Power([1], 2) * x * TMath::Exp(-[1]*x))";
    funcString += " + (x>1) * [2]";
    funcString += " * (1.17676e-01 * TMath::Sqrt(0.1396*0.1396+x*x)";
    funcString += " * TMath::Power(1. + 1./ [3] / 8.21795e-01";
    funcString += " * TMath::Sqrt(0.1396*0.1396+x*x), -1. * [3]))";
    funcString += " * (1 / ( 1 + TMath::Exp( -(x - [4]) / [5] )))";
    fTrackSpectrum = new TF1("fspectrum", funcString.Data(), .2, 200.);
    fspectrum->SetParameters(2434401791.20528, 2.98507, 10069622.25117, 5.50000, 2.80000, 0.20000);
}

Float_t EventModel::GetTriggerPhi(Float_t pt, TF1* f) {

}
