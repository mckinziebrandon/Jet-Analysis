#include "../include/EventModel.h"
#include "../include/EventFunctions.h"

EventFunctions::EventFunctions()
{
    fPolynomial     = (TF1*)InitfPolynomial();
    fLine           = (TF1*)InitfLine();
    fTrackSpectrum  = (TF1*)InitfTrackSpectrum();
    fdNdPhi         = (TF1*)InitfdNdPhi();
}

EventFunctions::~EventFunctions() 
{
    delete fPolynomial;
    delete fLine;
    delete fTrackSpectrum;
    delete fdNdPhi;
}

// Getters. -----------------------------------------------------
TF1* EventFunctions::GetfPolynomial()       { return fPolynomial; }
TF1* EventFunctions::GetfLine()             { return fLine; }
TF1* EventFunctions::GetfTrackSpectrum()    { return fTrackSpectrum; }
TF1* EventFunctions::GetfdNdPhi()           { return fdNdPhi; }
//---------------------------------------------------------------

// -------------------------------------------------------------------------------------
TF1* EventFunctions::InitfPolynomial() {
	return new TF1("fPolynomial", "pol3", 0., 6.);
}

TF1* EventFunctions::InitfLine() {
    TF1* f = new TF1("fLine", "[0] + [1] * x", 6., 18.);
    f->SetParameter(0, 0.15);
    f->SetParameter(1, -0.1 / 10.);
    return f;
}

TF1* EventFunctions::InitfTrackSpectrum () 
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

TF1* EventFunctions::InitfdNdPhi() 
{
    TF1* f= new TF1("fdNdPhi", "1 + 2 * [0] * TMath::Cos(2. * x)", 0., 2. * pi);
    f->SetParameter(0, 0.);
    f->SetParName(0, "v2");
    return f;
}
// -------------------------------------------------------------------------------------
