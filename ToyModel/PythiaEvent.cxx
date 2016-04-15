#include <iostream>
#include "./include/RootClasses.h"
#include "Pythia8/Pythia.h"
using namespace Pythia8;

void PythiaEvent() {
    using std::cout;
    using std::endl;

    // Generator. Process selection. LHC initialization. Histogram.
    Pythia pythia;
    pythia.readString("Beams:eCM = 2760.");
    pythia.readString("HardQCD:gg2gg = on");
    pythia.readString("Main:numberOfEvents = 10");
    pythia.readString("PhaseSpace:pTHatMin = 10."); 
    pythia.init();

    TH1* hNTrig = new TH1F("hNTrig", "Number of high-pt Pythia objects per event", 20, 0.0, 20.0);

    // Begin event loop. Generate event. Skip if error. List first one.
    for (int iEvent = 0; iEvent < 10; ++iEvent) {
        if (!pythia.next()) continue;

        int nTrig = 0;
        for (int i = 0; i < pythia.event.size(); ++i) {
            if (pythia.event[i].pT() > 10.0 && pythia.event[i].isCharged()) {
                cout << "Found pt = " << pythia.event[i].pT() << endl;
                nTrig++;
            }
        }
        cout << "NUMBER OF TRIGGER PARTICLES = " << nTrig << endl;
    }
    pythia.stat();

    fileName = "./rootFiles/PythiaEvent.root";
    TFile* outFile = new TFile(fileName.Data, "UPDATE");
    outFile->cd();
    hNTrig->Write();
    outFile->Close();

}


/* This main function is needed to compile via "make." */
#ifndef __CINT__
int main(int argc, char* argv[]) {
    PythiaEvent();
    return 0;
}
#endif
