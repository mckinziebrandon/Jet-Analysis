#include <iostream>
#include <fstream>
#include <cassert>
#include "CUnit/CUnit.h"
#include "../include/RootClasses.h"
#include "../include/EventModel.h"
#include "../include/MyJetFinder.h"

void JetFinderTester(void) {
    using std::cout;
    using std::endl;

    /* Initializes jetFinder and creates the JetDefinition. */
    MyJetFinder* jetFinder = new MyJetFinder();

    /* Fill the eventParticlesVector with candidates. */
    jetFinder->EmbedJetsInEvent();

    /* Fill event with some background particles. */
    for (int i = 0; i < 1000; i++) {
    	jetFinder->Generate("bkg");
    }

    /* Selects candidates based on eventClusterSequence. */
    jetFinder->FindJetsInEvent();

    /* Print out comparisons of what we expected to find and what we did find.*/
    Int_t expectedNumJets 	= jetFinder->GetNumEmbed();
    Int_t actualNumJets 	= jetFinder->GetNumJetsInEvent();
    cout << "Test Results:\n"
         << "expectedNumJets = " << expectedNumJets
         << "\tactualNumJets = " <<  actualNumJets << endl;
    jetFinder->PrintJetsInEvent();
    jetFinder->Write("jetFinder.root");

}


#ifndef __CINT__
int main(int argc, char* argv[]) 
{
	/*
	CU_pSuite pSuite = NULL;

	// Initialize the CUnit test registry.
	if (CUE_SUCCESS != CU_initialize_registry()) {
		return CU_get_error();
	}

	// Add a suite to the registry.
	pSuite = CU_add_suite("Suite_1", NULL, NULL);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if (NULL == CU_add_test(pSuite, "Test 1", JetFinderTester)) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	*/

    JetFinderTester();
    return 0;
}
#endif
