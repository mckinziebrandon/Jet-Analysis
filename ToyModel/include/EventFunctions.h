#ifndef EVENTFUNCTIONS_H
#define EVENTFUNCTIONS_H
#include "RootClasses.h"

class EventFunctions 
{
public:
    /* Creates EventFunctions object and initializes (creates) all
     * of its (private) instance functions. */
    EventFunctions();
    /* Deletes of this object's pointers to its functions. */
    ~EventFunctions();
    /* Simple getter methods to all private instance functions. */
    TF1* GetfPolynomial();
    TF1* GetfLine();
    TF1* GetfdNdPhi();
    TF1* GetfTrackSpectrum();
private:
    /* Fitted function to v2(pt) in lower pt range where 
     * shape still appears polynomial [0., 6.]. */
    TF1* fPolynomial;
    TF1* InitfPolynomial();
    /* Fitted function to v2(pt) in higher pt range where
     * shape appears linear (6, inf). Implemented for 
     * performance reasons, since unnecessary to force 
     * polynomial fit out to inf when shape actually linear. */
    TF1* fLine;
    TF1* InitfLine();
    /* Gives functional particle multiplicity (at some phi) 
     * dependence on v2: 1 + 2 * v2(pt) * cos(2 * phi), meaning
     * event plane angle Psi is implicitly set to 0. */
    TF1* fdNdPhi;
    TF1* InitfdNdPhi();
    /* Modified Boltzmann distribution with power law tail 
     * that yields (relative) particle multiplicity as a function 
     * of pt. Extends to 200 GeV/c. Credit: Redmer */
    TF1* fTrackSpectrum;
    TF1* InitfTrackSpectrum();
};
#endif

