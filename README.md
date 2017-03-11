# Event Generation and Jet Finding

[[Link to paper summary]](http://mckinziebrandon.me/assets/pdf/papers/LBNL_ToyModelResearch.pdf)

Recently (March 5th) moved from private BitBucket repository to Github. A research project of mine while working with Relativistic Nuclear Collisions (RNC) group at Lawrence Berkeley National Laboratory.

The two main classes, EventGenerator and MyJetFinder, can be used together simulate collision events and analyze properties of jets produced in the events. The goal of the project was to input measured parameters from ALICE event data, and compare the analysis done by MyJetFinder with analyses on the experimental data. Such comparisons are useful for testing our understanding of cause-effect relationships in high energy collisions and for ascertaining the predictive power of simulation models.

Overall, simplicity and ease of use was a primary goal in developing the codebase. The model obeys the object-oriented paradigm and strives to use classes and methods with intuitive names and behavior. For example, the following code is all that is needed to generate a single event and run the jet finder on the produced particles. Managing complexity and writing modular code is especially crucial when designing a model that may require new components to be built in some unknown order in the future.

```c++
    EventGenerator* eventGenerator = new EventGenerator();
    MyJetFinder* jetFinder = new MyJetFinder(R);

    /* ------------------------------------------------ *
     * Data Generation/Simulation.                   	*
     * ------------------------------------------------ */

    // Simulate nEvents with randomly generated tracks. 
    for (Int_t i_event = 0; i_event < nEvents; i_event++) {
    	// Print progress updates.
        Printer::print("Percent Complete: ", i_event,  nEvents);

        // Defined event centrality (and thus multiplicity);
        eventGenerator->SetCentrality(2.5); 
        Printer::print("\tNumber of particles generated: ", eventGenerator->GetMultiplicity());

        // Generate specified number/types of particles.
        eventGenerator->Generate("bkg", (Int_t) eventGenerator->GetMultiplicity()); 
        Printer::print("\tNumber of reconstructed particles: ", eventGenerator->GetRecoMult());

        // Use ClusterSequence to get list of jets in this event.
        jetFinder->FindJets(eventGenerator->GetLastEvent());
        Printer::print("\tNumber of jets found: ", jetFinder->GetNumJets());

        jetFinder->Clear();
    }

```
