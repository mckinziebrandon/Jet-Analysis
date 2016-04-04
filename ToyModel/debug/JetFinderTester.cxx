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
