
TF1* GetSpectrum() {
    // full spectrum used for ALICE SIMULATION PLOTS ALI-SIMUL-75145 ALI-SIMUL-75171
    // combination of boltzmann spectrum and hard jet spectrum
    TF1* fspectrum = new TF1("fspectrum", "[0]*(TMath::Power([1], 2)*x*TMath::Exp(-[1]*x))+(x>1)*[2]*(1.17676e-01*TMath::Sqrt(0.1396*0.1396+x*x)*TMath::Power(1.+1./[3]/8.21795e-01*TMath::Sqrt(0.1396*0.1396+x*x),-1.*[3]))*(1/(1 + TMath::Exp(-(x - [4])/[5])))", .2, 200.);
    fspectrum->SetParameters(2434401791.20528, 2.98507, 10069622.25117, 5.50000, 2.80000, 0.20000);
    return fspectrum;   
}

// second: just boltzmann part (background)
TF1* GetThermalSpectrum() {
    // pure boltzmann part of thermal spectrum
    TF1* boltzmann = new TF1("boltzmann", "[0]*(TMath::Power([1], 2)*x*TMath::Exp(-[1]*x))");
    boltzmann->SetParameters(2434401791.20528, 2.98507);
    return boltzmann;
}

// some extra functions which are called in the track loop (below)

// select your total pt spectrum
TF1* fTrackSpectrum = GetSpectrum();

// to generate a 'track' 
Double_t        GetTrackPt()       const                { return fTrackSpectrum->GetRandom();}
// find the v2 value that corresponds to the selected pt (first define a TF1 fFuncDiffV2 which is a parametrization of your differential v2
Double_t GetV2(Double_t pt) const { 
    return fFuncDiffV2->Eval(pt) ;
}
// same for v3
Double_t GetV3(Double_t pt) const { 
    return fFuncDiffV3->Eval(pt) ;
}        

void GetFlowFluctuation(Double_t& vn) const {
    // function to add gausisan flow fluctuations ('fFlowFluctuations' is the width of the gaussian distribution)
    // the inferse error function  comes fro inverting the guassian distribution
    vn += TMath::Sqrt(2*(vn*.25)*(vn*.25))*TMath::ErfInverse(2*(gRandom->Uniform(0, fFlowFluctuations))-1); 
}


// function to add pt differential v2 or v3 (don't apply both)

//_____________________________________________________________________________
void V2AfterBurner(Double_t &phi, Double_t &eta, Double_t &pt) const
{
    // function to add pt differential v2 to isotropic event
    // only works when initial phi is truly random
    phi = gRandom->Uniform(0, TMath::TwoPi());
    Double_t phi0(phi), v2(GetV2(pt)), f(0.), fp(0.), phiprev(0.);
    if(TMath::AreEqualAbs(v2, 0, 1e-5) return;
            // introduce flow fluctuations (gaussian)
            if(fFlowFluctuations > -10) GetFlowFluctuation(v2);
            for (Int_t i=0; i!=fMaxNumberOfIterations; ++i) {
            phiprev=phi; //store last value for comparison
            f =  phi-phi0+v2*TMath::Sin(2.*(phi-fPsi2));
            fp = 1.0+2.0*v2*TMath::Cos(2.*(phi-fPsi2)); //first derivative
            phi -= f/fp;
            if (TMath::AreEqualAbs(phiprev,phi,fPrecisionPhi)) break; 
            }
      )}
    //_____________________________________________________________________________
    void V3AfterBurner(Double_t &phi, Double_t &eta, Double_t &pt) const
{
    // similar to AliFlowTrackSimple::AddV3, except for the flow fluctuations
    phi = gRandom->Uniform(0, TMath::TwoPi());
    Double_t phi0(phi), v3(GetV3(pt)), f(0.), fp(0.), phiprev(0.);
    if(TMath::AreEqualAbs(v3, 0, 1e-5) && fQA) return;
    // introduce flow fluctuations (gaussian)
    if(fFlowFluctuations > -10) GetFlowFluctuation(v3);
    for (Int_t i=0; i<fMaxNumberOfIterations; i++)  {
        phiprev=phi; //store last value for comparison
        f =  phi-phi0+2./3.*v3*TMath::Sin(3.*(phi-fPsi3));
        fp = 1.0+2.0*v3*TMath::Cos(3.*(phi-fPsi3)); //first derivative
        phi -= f/fp;
        if (TMath::AreEqualAbs(phiprev,phi,fPrecisionPhi)) break;
    }
}

// the actual 'model'
// event loop of 100 events
for(Int_t iEvents = 0; iEvents < 1000; iEvents++) {
    // within this a track loop
    for (Int_t iTracks = 0; iTracks < 1000; ++iTracks) {
        // sample pt from pt spectrum
        pt = GetTrackPt();
        //uniform eta and phi distribution, random charge
        eta = gRandom->Uniform(-.9, .9);
        phi = gRandom->Uniform(0., TMath::TwoPi());
        charge = (gRandom->Uniform() < 0.5) ? -1 : 1;
        // add pt differential v2 or v3 (not both) using the 'afterburner' functions
        if(addV2)          V2AfterBurner(phi, eta, pt);
        else if(addV3)   V3AfterBurner(phi, eta, pt);
        // now you have events with flowing background and jets so
        // here you can do analysis}}
