#include "StJetSimAnalysis.h"

//----------------------------------------------------------------------------------------
static char     NoP[50];
static TRandom  ran;
static TRandom  ran_gen;
static TRandom3 r3;
static TString  HistName;
static Double_t         N_Events_total;
static Double_t         area_cut;
static const Double_t   particle_mass       = 0.139;
static const Double_t   min_pt_threshold    = 0.0;
static const Double_t   max_pt_threshold    = 0.5;
static const Double_t   jet_delta_eta_cut   = 100.0;
static const Double_t   jet_delta_phi_cut   = 45.0*(2.0*Pi/360.0);
static const Int_t      Remove_N_hardest    = 0;
//----------------------------------------------------------------------------------------

static const char HA_EVENT_TREE[]   = "HA_Events";
static TTree            *Tree_EP_Det_Sim;
static EP_Det_Sim_Event *HA_event;
static EP_Det_Sim_Event *HA_event_ptr;
static EP_Det_Sim_Track *HA_track;

static char* NAME_HA_EVENT_TREE   = "Tree_EP_Det_Sim_tree";
static char* NAME_HA_EVENT_BRANCH = "Tree_EP_Det_Sim_branch";

static TNtuple *NT_ReCoil_Jet;
static Float_t ReCoil_Jet_NTDataArray[14];


#include "StJetSimAnalysis_Func.h"


//------------------------------------------------------------------------------------------------------------------
ClassImp(StJetSimAnalysis)
    StJetSimAnalysis::StJetSimAnalysis()
{

}
//------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------
StJetSimAnalysis::~StJetSimAnalysis()
{

}
//------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void StJetSimAnalysis::Init()
{
    cout << "Init started" << endl;
    r3.SetSeed(0);

    area_cut = 0.4*TMath::Pi()*Jet_R*Jet_R;
    h_jet_pt_sub                 = new TH1D("h_jet_pt_sub","h_jet_pt_sub",1400,-40,100.0);
    h_Delta_phi                  = new TH1D("h_Delta_phi","h_Delta_phi",100,-TMath::Pi(),TMath::Pi());
    h_Delta_phi_trigger          = new TH1D("h_Delta_phi_trigger","h_Delta_phi_trigger",100,-TMath::Pi(),TMath::Pi());
    h_jet_pt_vs_Delta_phi        = new TH2D("h_jet_pt_vs_Delta_phi","h_jet_pt_vs_Delta_phi",50,-TMath::Pi(),TMath::Pi(),50,-20,30);
    h_jet_recoil_pt_vs_Delta_phi = new TH2D("h_jet_recoil_pt_vs_Delta_phi","h_jet_recoil_pt_vs_Delta_phi",50,-TMath::Pi(),TMath::Pi(),50,-20,30);
    h_jet_pt_vs_trigger_pt       = new TH2D("h_jet_pt_vs_trigger_pt","h_jet_pt_vs_trigger_pt",50,0,3.5,50,-20,30);
    h_track_pt_vs_delta_phi      = new TH2D("h_track_pt_vs_delta_phi","h_track_pt_vs_delta_phi",50,-TMath::Pi(),TMath::Pi(),50,0,3.5);
    h_trigger_pt_vs_delta_phi    = new TH2D("h_trigger_pt_vs_delta_phi","h_trigger_pt_vs_delta_phi",50,-TMath::Pi(),TMath::Pi(),50,0,3.5);

    //----------------------------------------------------------------------------------------------------
    // Simulation input
    TString Input_list = SEList;
    if (!Input_list.IsNull())   // if input file is ok
    {
        cout << "Open file list " << Input_list << endl;
        ifstream in(Input_list);  // input stream
        if(in)
        {
            cout << "file list is ok" << endl;
            input_chain  = new TChain( NAME_HA_EVENT_TREE, NAME_HA_EVENT_TREE );
            char str[255];       // char array for each file name
            Long64_t entries_save = 0;
            while(in)
            {
                in.getline(str,255);  // take the lines of the file list
                if(str[0] != 0)
                {
                    TString addfile;
                    addfile = eIndir;
                    addfile += str;
                    Long64_t file_entries;
                    input_chain ->AddFile(addfile.Data(),-1, NAME_HA_EVENT_TREE );
                    file_entries = input_chain->GetEntries();
                    cout << "File added to data chain: " << addfile.Data() << " with " << (file_entries-entries_save) << " entries" << endl;
                    entries_save = file_entries;
                }
            }
        }
    }

    // Set the input tree
    if (!input_chain->GetBranch( NAME_HA_EVENT_BRANCH ))
    {
        cerr << "ERROR: Could not find branch '"
            << NAME_HA_EVENT_BRANCH << "'in tree!" << endl;
    }

    cout << "Set branch address" << endl;
    input_chain ->SetBranchAddress( NAME_HA_EVENT_BRANCH, &HA_event );
    cout << "Branch address was set" << endl;

    N_Events_total = input_chain->GetEntriesFast();
    cout << "Number of events in file(s) = " << N_Events_total << endl;
    if( (eStartEvent+N_Events) > N_Events_total) N_Events = (N_Events_total-eStartEvent);
    //----------------------------------------------------------------------------------------------------



    //----------------------------------------------------------------------------------------------------
    TString outputfile_name = eOutdir;
    outputfile_name += ListName;
    outputfile_name += "_out.root";
    cout << "Output file: " << outputfile_name.Data() << endl;
    Outputfile        = new TFile(outputfile_name.Data(),"RECREATE");
    //----------------------------------------------------------------------------------------------------

    NT_ReCoil_Jet = new TNtuple("NT_ReCoil_Jet", 
                                "NT_ReCoil_Jet Ntuple", 
                                "EventId:JetId:rho:area:Jetphi:Jeteta:Jetpt:TrackId:eta:phi:pt:x:y:z");
    NT_ReCoil_Jet->SetAutoSave( 5000000 );
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void StJetSimAnalysis::Make()
{
    cout << "Make started" << endl;
    r3.SetSeed(0);
    gRandom->SetSeed(0);
    cout << "Seed = " << r3.GetSeed() << endl;

    ran_gen.SetSeed(0);
    ran.SetSeed(0);

    input_chain->GetEntry( 0 );

    cout << "Start of event loop" << endl;
    // ---------------------------------------- begin event loop ----------------------------------------
    for(Long64_t n_evt = eStartEvent; n_evt < (eStartEvent+N_Events); n_evt++)
    {
        if (n_evt != 0  &&  n_evt % 50 == 0)
        {
            cout << "." << flush;
        }

        if (n_evt != 0  &&  n_evt % 500 == 0)
        {
            Double_t event_percent = 100.0*(n_evt-eStartEvent)/N_Events;

            cout << " " << n_evt << " (" << event_percent << "%) " 
                << "\n" << "==> Processing data, " << flush;
        }
 
        // take the event -> information is stored in event
        if (!input_chain->GetEntry( n_evt ))
            break;  

        // obtain and store event info
        Int_t   N_Tracks   = (Int_t)HA_event->getN_Tracks();
        Int_t   N_Cent     = (Int_t)HA_event->getN_Cent();
        Double_t v2_smear  = (Double_t)HA_event->getv2_smear();
        Double_t Psi_angle = (Double_t)HA_event->getPsi_in();
        Double_t Psi_rec   = (Double_t)HA_event->getPsi_out();

        // only want 0-10% centrality
        if(N_Cent >= 2) continue; 

        // declare event jet container and trigger tracks
        std::vector<PseudoJet> particles_jet;
        std::vector< std::vector<Double_t> > vec_trigger_tracks; // eta, phi, pT, Delta phi

        // ------------------------------ begin track loop ------------------------------
        // Overview:    1. Store basic track information.
        //              2. Store tracks that satisfy trigger conditions.
        //              3. Store all tracks in particle_jet
        for(Int_t n_track = 0; n_track < N_Tracks; n_track++) 
        {
            // obtain track information
            HA_track = HA_event->getTrack( n_track ); 
            Double_t pT_track             = (Double_t)HA_track->getpT();
            Double_t eta                  = (Double_t)HA_track->geteta();
            Double_t v2_track_smear       = (Double_t)HA_track->getv2_in();
            Double_t Psi_phi_track        = (Double_t)HA_track->getPhi_Psi_in();
            Double_t phi_track            = (Double_t)HA_track->getPhi_in();
            Double_t theta_track          = 2.0*TMath::ATan(TMath::Exp(-eta));

            // find delta_phi and fill histograms
            Double_t Delta_phi_angle = Psi_angle-phi_track;
            if(Delta_phi_angle > TMath::Pi())  Delta_phi_angle -= 2.0*TMath::Pi();
            if(Delta_phi_angle < -TMath::Pi()) Delta_phi_angle += 2.0*TMath::Pi();
            h_Delta_phi ->Fill(Delta_phi_angle);
            h_track_pt_vs_delta_phi ->Fill(Delta_phi_angle,pT_track);

            // fill trigger track candidates into vec_trigger_tracks
            if(pT_track > min_pt_threshold && pT_track < max_pt_threshold && fabs(eta) < 1.0)
            {
                std::vector<Double_t> vec_in;
                vec_in.resize(4);
                vec_in[0] = eta;
                vec_in[1] = phi_track;
                vec_in[2] = pT_track;
                vec_in[3] = Delta_phi_angle;
                vec_trigger_tracks.push_back(vec_in);
            }

            // fill particles_jet with ALL tracks as if they are each pseudojets...
            TLorentzVector TLV_Particle_use;
            TLV_Particle_use.SetPtEtaPhiM(pT_track,eta,phi_track,particle_mass);
            PseudoJet Fill_PseudoJet(TLV_Particle_use.Px(),
                                    TLV_Particle_use.Py(),
                                    TLV_Particle_use.Pz(),
                                    TLV_Particle_use.E());
            particles_jet.push_back(Fill_PseudoJet);
        } 
        // ------------------------------ end track loop ------------------------------


        //--------------------------------------------------------------------
        // create embedded jet by filling particles_jet with constituents TLV 

        /*
        const Int_t N_embed = 5;
        Double_t pt_eta_phi[N_embed][3] = {{6.99, 0, 3.1}, 
                                            {7.99, -0.05, 3.15}, 
                                            {5.99, 0.05, 3.05},
                                            {8.99, -0.08, 3.18}, 
                                            {9.99, 0.08, 3.02}};

        for (Int_t i_embed = 0; i_embed < N_embed; i_embed++)
        {
            TLorentzVector TLV_Particle_use;
            TLV_Particle_use.SetPtEtaPhiM(pt_eta_phi[i_embed][0],
                                        pt_eta_phi[i_embed][1],
                                        pt_eta_phi[i_embed][2],
                                        particle_mass);
            PseudoJet Fill_PseudoJet(TLV_Particle_use.Px(),
                                        TLV_Particle_use.Py(),
                                        TLV_Particle_use.Pz(),
                                        TLV_Particle_use.E());
            particles_jet.push_back(Fill_PseudoJet);
        }
        */
        //--------------------------------------------------------------------

        //----------------------------------------------------------------------------------
        // Take one random track from the trigger track candidates

        // sort the pT values
        std::sort (vec_trigger_tracks.begin(), vec_trigger_tracks.end(), sortFunc);
        
        Int_t trigger_index_min   = 0;
        Int_t trigger_index_range = vec_trigger_tracks.size()-trigger_index_min;

        Int_t random_trigger_index   = ran_gen.Integer(trigger_index_range); 
        random_trigger_index        += trigger_index_min;

        if(random_trigger_index >= vec_trigger_tracks.size())
        {
            cout << "ERROR: random_trigger_index out of range!" << endl;
            continue;
        }
        h_trigger_pt_vs_delta_phi   ->Fill( vec_trigger_tracks[random_trigger_index][3],
                                            vec_trigger_tracks[random_trigger_index][2]);
        h_Delta_phi_trigger         ->Fill( vec_trigger_tracks[random_trigger_index][3]);
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------------------
        // Start of jet reconstruction

        // container of final set of jets
        vector<PseudoJet> jets;

        // choose a jet definition
        JetDefinition jet_def(antikt_algorithm, Jet_R);

        // jet area definition
        Double_t ghost_maxrap = 1.0; // Fiducial cut for background estimation
        GhostedAreaSpec area_spec(ghost_maxrap);
        AreaDefinition area_def(active_area_explicit_ghosts,GhostedAreaSpec(ghost_maxrap,1,0.01));

        // cluster all particles in event according to jet & area definitions
        ClusterSequenceArea clust_seq_hard(particles_jet, jet_def, area_def);

        // assert jets = all jets(pt > ptmin) and satisfy fiducial cut
        double ptmin = 0.2;
        vector<PseudoJet> jets_all      = sorted_by_pt(clust_seq_hard.inclusive_jets(ptmin));
        Selector Fiducial_cut_selector  = SelectorAbsEtaMax(1.0 - Jet_R);
        jets = Fiducial_cut_selector(jets_all);

        // background definition
        JetDefinition jet_def_bkgd(kt_algorithm, Jet_R); 
        AreaDefinition area_def_bkgd(active_area_explicit_ghosts,GhostedAreaSpec(ghost_maxrap,1,0.01)); 
        Selector selector = SelectorAbsEtaMax(1.0) * (!SelectorNHardest(Remove_N_hardest)); 

        // background estimation & subtraction
        JetMedianBackgroundEstimator bkgd_estimator(selector, jet_def_bkgd, area_def_bkgd); 
        Subtractor subtractor(&bkgd_estimator);
        bkgd_estimator.set_particles(particles_jet);

        // obtain rho: the median bkgd pT per area
        Double_t jet_rho   = bkgd_estimator.rho();
        Double_t jet_sigma = bkgd_estimator.sigma();
        //----------------------------------------------------------------------------------------------

        // ----------------------------------------------------------------------------------------------------------
        // __________________________________________ J E T   L O O P _______________________________________________

        if(vec_trigger_tracks.size() > 0)
        {
            // get pT of previously assigned random trigger
            std::vector<Double_t> vec_trigger = vec_trigger_tracks[random_trigger_index];
            Double_t trigger_pt = vec_trigger[2];
            //cout << "\ntrigger_pt = " << trigger_pt;

            // loop over all jet candidates
            for(Int_t i_jet = 0; i_jet < jets.size(); i_jet++)
            {
                // store jet info
                Float_t jet_pt      = jets[i_jet].perp();
                Float_t jet_area    = jets[i_jet].area();
                Float_t jet_pt_sub  = jets[i_jet].perp() - jet_rho*jet_area; // bkgd subtracted pT
                Float_t jet_eta     = jets[i_jet].eta();
                Float_t jet_phi     = jets[i_jet].phi(); 

                // angular difference b/w this jet and trigger
                Float_t jet_delta_eta = fabs(jet_eta + vec_trigger[0]);
                Float_t jet_delta_phi = fabs(jet_phi - vec_trigger[1]);
                if(jet_delta_phi > 2.0*Pi)  jet_delta_phi -= 2.0*Pi;

                Float_t dijet_delta_phi = jet_phi - vec_trigger[1]; // -2*Pi..2*Pi
                if(dijet_delta_phi < 0.0) dijet_delta_phi += 2.0*Pi; // 0..2*Pi
                if(dijet_delta_phi > 1.5*Pi)
                {
                    dijet_delta_phi = -0.5*Pi + (dijet_delta_phi-1.5*Pi); // -0.5*Pi..1.5*Pi
                }

                Double_t jet_EP_Delta_phi = Psi_angle - jet_phi;
                if(jet_EP_Delta_phi > Pi)  jet_EP_Delta_phi -= 2.0*Pi;
                if(jet_EP_Delta_phi < -Pi) jet_EP_Delta_phi += 2.0*Pi;

                // Recoil jet histograms
                if( jet_delta_eta < jet_delta_eta_cut &&        // delta_eta cut does nothing here.
                    fabs(Pi-jet_delta_phi) < jet_delta_phi_cut )
                {
                    if(jet_area > area_cut)
                    {
                        ReCoil_Jet_NTDataArray[0]   = (Float_t)n_evt;         // EventId
                        ReCoil_Jet_NTDataArray[1]   = (Float_t)i_jet;         // JetId
                        ReCoil_Jet_NTDataArray[2]   = (Float_t)jet_rho;       // rho
                        ReCoil_Jet_NTDataArray[3]   = (Float_t)jet_area;      // area
                        ReCoil_Jet_NTDataArray[4]   = (Float_t)jet_phi;       // Jetphi
                        ReCoil_Jet_NTDataArray[5]   = (Float_t)jet_eta;       // Jeteta
                        ReCoil_Jet_NTDataArray[6]   = (Float_t)jet_pt_sub;    // Jetpt
                        ReCoil_Jet_NTDataArray[7]   = (Float_t)0;             // TrackId
                        ReCoil_Jet_NTDataArray[8]   = (Float_t)0; // eta
                        ReCoil_Jet_NTDataArray[9]   = (Float_t)0; // phi
                        ReCoil_Jet_NTDataArray[10]  = (Float_t)0;  // pt
                        ReCoil_Jet_NTDataArray[11]  = (Float_t)0;             // x
                        ReCoil_Jet_NTDataArray[12]  = (Float_t)0;             // y
                        ReCoil_Jet_NTDataArray[13]  = (Float_t)0;             // z
                        NT_ReCoil_Jet->Fill(ReCoil_Jet_NTDataArray);

                        h_jet_pt_sub                 ->Fill(jet_pt_sub);
                        h_jet_recoil_pt_vs_Delta_phi ->Fill(jet_EP_Delta_phi,jet_pt_sub);

                    }
                }

                // ----------------------------- begin constituent loop ----------------------------------------------
                vector<PseudoJet> jet_constituents = jets[i_jet].constituents();
                for (Int_t j = 0; j < jet_constituents.size(); j++)
                {
                    // store constitutent info
                    Float_t jet_const_pt    = jet_constituents[j].perp();
                    Float_t jet_const_phi   = jet_constituents[j].phi();
                    Float_t jet_const_eta   = jet_constituents[j].eta();
                    Int_t   user_index      = jet_constituents[j].user_index(); // ?

                    // if jet_area is larger than input area, fill hists
                    if(jet_area > area_cut)
                    {
                        h_jet_pt_vs_Delta_phi   ->Fill(jet_EP_Delta_phi,jet_pt_sub);
                        h_jet_pt_vs_trigger_pt  ->Fill(trigger_pt,jet_pt_sub);
                    }

                    /*
                    // Recoil jet histograms
                    if( jet_delta_eta < jet_delta_eta_cut &&        // delta_eta cut does nothing here.
                        fabs(Pi-jet_delta_phi) < jet_delta_phi_cut )
                    {
                        if(jet_area > area_cut)
                        {
                            ReCoil_Jet_NTDataArray[0]   = (Float_t)n_evt;         // EventId
                            ReCoil_Jet_NTDataArray[1]   = (Float_t)i_jet;         // JetId
                            ReCoil_Jet_NTDataArray[2]   = (Float_t)jet_rho;       // rho
                            ReCoil_Jet_NTDataArray[3]   = (Float_t)jet_area;      // area
                            ReCoil_Jet_NTDataArray[4]   = (Float_t)jet_phi;       // Jetphi
                            ReCoil_Jet_NTDataArray[5]   = (Float_t)jet_eta;       // Jeteta
                            ReCoil_Jet_NTDataArray[6]   = (Float_t)jet_pt_sub;    // Jetpt
                            ReCoil_Jet_NTDataArray[7]   = (Float_t)j;             // TrackId
                            ReCoil_Jet_NTDataArray[8]   = (Float_t)jet_const_eta; // eta
                            ReCoil_Jet_NTDataArray[9]   = (Float_t)jet_const_phi; // phi
                            ReCoil_Jet_NTDataArray[10]  = (Float_t)jet_const_pt;  // pt
                            ReCoil_Jet_NTDataArray[11]  = (Float_t)0;             // x
                            ReCoil_Jet_NTDataArray[12]  = (Float_t)0;             // y
                            ReCoil_Jet_NTDataArray[13]  = (Float_t)0;             // z
                            NT_ReCoil_Jet->Fill(ReCoil_Jet_NTDataArray);

                            h_jet_pt_sub                 ->Fill(jet_pt_sub);
                            h_jet_recoil_pt_vs_Delta_phi ->Fill(jet_EP_Delta_phi,jet_pt_sub);

                        }
                    }
                    */
                }
                // ----------------------------- end constituent loop ----------------------------------------------
            }
            // ___________________________________ E N D   J E T   L O O P __________________________________________
        }
    } 
    // end event loop
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------
void StJetSimAnalysis::Finish()
{
    cout << "Finish started" << endl;
    Outputfile   ->cd();
    NT_ReCoil_Jet                   ->Write();
    h_jet_pt_sub                    ->Write();
    h_Delta_phi                     ->Write();
    h_Delta_phi_trigger             ->Write();
    h_jet_pt_vs_Delta_phi           ->Write();
    h_jet_pt_vs_trigger_pt          ->Write();
    h_track_pt_vs_delta_phi         ->Write();
    h_trigger_pt_vs_delta_phi       ->Write();
    h_jet_recoil_pt_vs_Delta_phi    ->Write();
    Outputfile   ->Close();
}
//------------------------------------------------------------------------------------------------------------------

