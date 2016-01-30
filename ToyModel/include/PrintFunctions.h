#ifndef PRINTFUNCTIONS_H
#define PRINTFUNCTIONS_H
#include "RootClasses.h"

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------
void PrintEventStatus(const Int_t i_event, const Int_t nEvents, const Int_t percent);


// -----------------------------------------------------------------------------
// Function Implementations
// -----------------------------------------------------------------------------

// Print
void PrintEventStatus(const Int_t i_event, const Int_t nEvents, const Int_t percent)
{
    if (i_event % (nEvents / percent) == 0)
    {
        Double_t percent_complete = Double_t(i_event) / Double_t(nEvents) * 100.;
        std::cout << "\nBeginning event " << i_event;
        std::cout << ". Simulation is " <<  percent_complete << "\% complete.";
    }
    else if (i_event % (nEvents / (percent * 10)) == 0)
        std::cout << " . ";
}

#endif 
