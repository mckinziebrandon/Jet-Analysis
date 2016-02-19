#include "../include/EventModel.h"

ClassImp(EventModel)

void EventModel::Streamer(TBuffer &R__b)
{
   // Stream an object of class EventModel.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> has_bkg;
      R__b >> has_V2;
      R__b >> fPolynomial;
      R__b >> fLine;
      R__b >> fdNdPhi;
      R__b >> fTrackSpectrum;
      R__b >> rand;
      R__b.CheckByteCount(R__s, R__c, EventModel::IsA());
   } else {
      R__c = R__b.WriteVersion(EventModel::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << has_bkg;
      R__b << has_V2;
      R__b << fPolynomial;
      R__b << fLine;
      R__b << fdNdPhi;
      R__b << fTrackSpectrum;
      R__b << rand;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

/************************************
* Default EventModel Constructor.   *
* Initializes all instance objects. *
*************************************/
EventModel::EventModel(): TObject()
{
    TNamed("hello","how are you");
    // Initialize event switches.
    has_bkg = true;
    has_V2  = true;

    // Initialize instance attribute functions.
    fPolynomial     = (TF1*)GetfPolynomial();
    fLine           = (TF1*)GetfLine();
    fTrackSpectrum  = (TF1*)GetfSpectrum();
    fdNdPhi         = (TF1*)GetfdNdPhi();

    // Fit ALICE data and store in fPolynomial and fLine.
    TFile* file_v2 = new TFile("./rootFiles/ALICE_v2pt.root");
    TGraph* g = (TGraphAsymmErrors*)file_v2->Get("v2Plot_2;1");
    g->Fit(fPolynomial, "RQ");
    g->Fit(fPolynomial, "RQ+");
    delete g;
    delete file_v2;

    // Initialize random number generator.
    rand = new TRandom3();
}

/************************************
* Default EventModel Destructor.    *
* Deletes instance objects.         *
*************************************/
EventModel::~EventModel() 
{
    delete fPolynomial;
    delete fLine;
    delete fdNdPhi;
    delete fTrackSpectrum;
    delete rand;
}

// uno
void EventModel::SetTrigger(Float_t &pt, Float_t &eta, Float_t &phi)
{
    pt  = GetTrackPt();
    eta = GetRandEta();
    phi = GetTriggerPhi(pt); 
}

// dos
void EventModel::SetJet(Float_t &pt, Float_t &eta, Float_t &phi)
{
    Float_t trigger_phi = phi;
    eta = -eta;
    pt  = GetTrackPt();
    phi = GetAssocPhi(pt, trigger_phi, sigma_dphi);
}

// tres
void EventModel::SetBackground(Float_t &pt, Float_t &eta, Float_t &phi)
{
    pt  = GetTrackPt();
    phi = GetTriggerPhi(pt);
    eta = GetRandEta();
}

/****************************************************
* Returns random value of phi that depends on       *
* the value of v2(pt) input parameter for dN/dPhi.  *
*****************************************************/
Float_t EventModel::GetTriggerPhi(Float_t pt) 
{
    fdNdPhi->SetParameter("v2", GetV2(pt));
    return fdNdPhi->GetRandom();
}


/********************************************************************************
* Similar to dphi(p1, p2), this returns a deltaPhi, but with a gaussian spread  *
* centered at pi from the trigger particle.                                     *
*********************************************************************************/
Float_t EventModel::GetAssocPhi(Float_t pt, Float_t trig_phi, Float_t sigma_dphi)
{
    // Setup. 
    Float_t assoc_phi_mean, result;

    // Get the average phi, centered at pi w.r.t. the trigger.
    assoc_phi_mean  = trig_phi + pi;
    if (assoc_phi_mean > 2 * pi) 
        assoc_phi_mean -= 2 * pi;

    // Create superposition of gaussian and modulation in phi.
    TF1* f_v2_gaus = new TF1("f_v2_gaus", "gaus(0)+1+2*[3]*TMath::Cos(2.*x)", 0, 2*pi);
    f_v2_gaus->SetParameters(1, assoc_phi_mean, sigma_dphi, GetV2(pt));

    // Store random value of modulated function and clean up. 
    result = f_v2_gaus->GetRandom();
    delete f_v2_gaus;
    return result;
}

/****************************************************
* Returns difference of inputs, while constraining  *
* the result to be in [0, 2 * pi].                  *
*****************************************************/
Float_t EventModel::dphi(Float_t phi1, Float_t phi2) 
{
    Float_t dphi    = phi1 - phi2;
    if (dphi < 0) return dphi + 2 * pi;
    return dphi;
}

/* Returns random eta uniform in [-1, 1]. */
Float_t EventModel::GetRandEta() { return rand->Uniform(-1.00, 1.00); }

/* Returns random pt from boltzmann probability distribution. */
Double_t EventModel::GetTrackPt() { return fTrackSpectrum->GetRandom(); }


/*===============================================================================
---------------------------------------------------------------------------------
                            PRIVATE METHODS                                      
---------------------------------------------------------------------------------
================================================================================*/


TF1* EventModel::GetfPolynomial() { return new TF1("fPolynomial", "pol3", 0., 6.); }

TF1* EventModel::GetfLine() 
{
    TF1* f = new TF1("fLine", "[0] + [1] * x", 6., 18.);
    f->SetParameter(0, 0.15);
    f->SetParameter(1, -0.1 / 10.);
    return f;
}

TF1* EventModel::GetfSpectrum () 
{
    TString funcString = "[0] * (TMath::Power([1], 2) * x * TMath::Exp(-[1]*x))";
    funcString += " + (x>1) * [2]";
    funcString += " * (1.17676e-01 * TMath::Sqrt(0.1396*0.1396+x*x)";
    funcString += " * TMath::Power(1. + 1./ [3] / 8.21795e-01";
    funcString += " * TMath::Sqrt(0.1396*0.1396+x*x), -1. * [3]))";
    funcString += " * (1 / ( 1 + TMath::Exp( -(x - [4]) / [5] )))";
    TF1* f = new TF1("fspectrum", funcString.Data(), .2, 200.);
    f->SetParameters(2434401791.20528, 2.98507, 10069622.25117, 5.50000, 2.80000, 0.20000);
    return f;
}

TF1* EventModel::GetfdNdPhi() 
{
    TF1* f= new TF1("fdNdPhi", "1 + 2 * [0] * TMath::Cos(2. * x)", 0., 2. * pi);
    f->SetParameter(0, 0.);
    f->SetParName(0, "v2");
    return f;
}

Float_t EventModel::GetV2(Float_t pt)
{
    if (!has_V2) return 0.0;
    if (pt < 6)  return 3. * (Float_t)fPolynomial->Eval(pt);
    return 3. * (Float_t)fLine->Eval(pt);
}


