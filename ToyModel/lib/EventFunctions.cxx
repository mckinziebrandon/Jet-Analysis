#include "../include/EventGenerator.h"
#include "../include/EventFunctions.h"

EventFunctions::EventFunctions()
{
    fPolynomial     = (TF1*)InitfPolynomial();
    fLine           = (TF1*)InitfLine();
    fTrackSpectrum  = (TF1*)InitfTrackSpectrum();
    fdNdPhi         = (TF1*)InitfdNdPhi();
    fMult           = (TF1*)InitfMult();
}

EventFunctions::~EventFunctions() 
{
    delete fPolynomial;
    delete fLine;
    delete fTrackSpectrum;
    delete fdNdPhi;
    delete fMult;
}

// Getters. -----------------------------------------------------
TF1* EventFunctions::GetfPolynomial()       { return fPolynomial; }
TF1* EventFunctions::GetfLine()             { return fLine; }
TF1* EventFunctions::GetfTrackSpectrum()    { return fTrackSpectrum; }
TF1* EventFunctions::GetfdNdPhi()           { return fdNdPhi; }
TF1* EventFunctions::GetfMult()             { return fMult; }
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
    TF1* f = new TF1("fspectrum", "[0] * TMath::Power(1 + x / [1], -[2])", ptMin, ptMax);
    f->SetParameters(1.0, 1.0, 5.0); // 5.0 is based on experimentally determined values. KEEP. 
    return f;
}

TF1* EventFunctions::InitfMult() {
    TF1* f = new TF1("fMult", "[0] * expo(1)", 4., 100.);
    f->SetParameters(1, 1, -1);
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
