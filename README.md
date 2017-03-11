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

## Quick Visualizations

### Jet Distributions in η-Area Phase Space


<img alt='JetAreaEta2D' src="http://i.imgur.com/dQ4ZpZN.png?1" width="600">


Surface plots of jet η and jet area for (top-left) R = 0.2, (top-right) R = 0.3 , (lower-left) R = 0.4, and (lower-right) R = 0. Distributions in η deviate from expectations primarily for jets with areas close to zero. This is a direct consequence of how the clustering is performed by the anti-kT algorithm. Clustering will evolve by sequentially grouping nearby high-pT tracks until their associated proto-jet has a large enough pT to be designated as a jet.  Near the end of the clustering process, however, there will inevitably be small-area proto-jets scattered in (η,φ) that either don’t meet the criteria to merge with a neighboring proto-jet or are situated such that all tracks within R have already been assigned to a jet. Therefore, it is unsurprising to observe uniformity in η for the large-area jets and less conventional behavior for small-are jets. There does appear to be a trend in the η distribution for the low-area jets as R increases that is a consequence of the boundary condition |track η| < 0.9. If our acceptance was larger, say |η| < 2.0, such boundary effects would be less noticeable. This is confirmed in the figure below. 

<img alt='JetAreaEta2D' src="http://i.imgur.com/CYKyTye.png?1" width="600">

Jet  area  and η distributions  with  an  increased  acceptance  of|η| < 2.0.  

### Reconstructed Jets: Area Distributions for Different Radii

As the value of R increases, the number of measured jets per event decreases. This is nothing more than
a consequence of geometry, since R determines the angular reach of the anti-kT algorithm in η−φ space. Therefore, since no two reconstructed jets can overlap in η−φ, we have that large R corresponds to less reconstructed jets per event. These geometrical/boundary-condition effects are evident in the figure below.

<img alt='JetArea' src="http://i.imgur.com/LvF7Kz7.png?1" width="600">

This figure shows the event-normalized distributions of reconstructed jet areas for different simulated values of R.  The  dashed vertical lines indicate where Jet Area = πR^2 for each value of R.


### The Effect of Acceptance Cuts

Below is an illustration of how the jet area cut can affect the pT distribution for R = 0.3. 

<img alt='JetCutArea' src="http://i.imgur.com/L76kc7V.png?1" width="600">

Left:  An area cut requiring A > 0.15 is applied. Blue represents the region that is kept, while green represents the region rejected. Right: background-subtracted jet pT distribution shown for jets that passed that area cut (blue), as well as all jets where no area cut was applied (green)

