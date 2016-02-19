#ifndef EVENTFUNCTIONS_H
#define EVENTFUNCTIONS_H
#include "RootClasses.h"

class EventFunctions 
{
private:
    TF1* fPolynomial;
    TF1* fLine;
    TF1* fdNdPhi;
    TF1* fTrackSpectrum;
    // Private helper methods.
    TF1* InitfPolynomial();
    TF1* InitfLine();
    TF1* InitfdNdPhi();
    TF1* InitfSpectrum();
public:
    // Constructors / Destructors.
    EventFunctions();
    ~EventFunctions();
    TF1* GetfPolynomial();
    TF1* GetfLine();
    TF1* GetfdNdPhi();
    TF1* GetfSpectrum();
};
#endif

