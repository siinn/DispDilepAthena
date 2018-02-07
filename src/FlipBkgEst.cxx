// DisplacedDimuonAnalysis includes
#include "FlipBkgEst.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"

// xAOD
#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODCore/tools/SafeDeepCopy.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

// tools
#include "PathResolver/PathResolver.h"

// standard library
#include "cmath"
#include <random>

// debug
#include <typeinfo>

FlipBkgEst::FlipBkgEst( const std::string& name, ISvcLocator* pSvcLocator ) :
AthAnalysisAlgorithm( name, pSvcLocator ),
m_dilepdvc("DDL::DiLepDVCuts/DiLepDVCuts"),
m_evtc("DDL::EventCuts/DiLepEventCuts"),
m_dvc("DDL::DVCuts/DiLepBaseCuts"),
m_cos("DDL::DiLepCosmics/DiLepCosmics"),
m_dvutils("DVUtils"),
m_leptool("LeptonSelectionTools"),
m_costool("CosmicTools"),
m_grlTool("GoodRunsListSelectionTool/GRLTool"),
m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
m_tmt("Trig::MatchingTool/TrigMatchingTool"),
m_trig("DDL::TrigMatch/TrigMatch"),
m_or("DDL::OverlapRemoval/OverlapRemoval"),
m_accMu("DDL_Muons"),
m_accEl("DDL_Electrons"),
m_vertexer("DDL::DispVertexer/DispVertexer"),
m_acc_p4("DDL_p4"),
m_accMass("mass"),
m_trackToVertexTool("Reco::TrackToVertex"),
m_fitsvc("Trk::TrkVKalVrtFitter/VxFitter")
{
    // initialize tools
    declareProperty("DiLepDVCuts", m_dilepdvc);
    declareProperty("DVUtils", m_dvutils);
    declareProperty("LeptonSelectionTool", m_leptool);
    declareProperty("CosmicTool", m_costool);
    declareProperty("DiLepEventCuts", m_evtc);
    declareProperty("DiLepBaseCuts", m_dvc);
    declareProperty("GRLTool",  m_grlTool, "The private GoodRunsListSelectionTool" );
    declareProperty("TrigDecisionTool", m_tdt);
    declareProperty("TrigMatchingTool", m_tmt);
    declareProperty("TrigMatch", m_trig);
    declareProperty("DiLepCosmics", m_cos);
    declareProperty("OverlapRemoval", m_or);
    declareProperty("Vertexer", m_vertexer);
    declareProperty ("TrackToVertexTool", m_trackToVertexTool);
    declareProperty ("TrkVKalVrtFitter", m_fitsvc);
}


FlipBkgEst::~FlipBkgEst() {}


StatusCode FlipBkgEst::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    ServiceHandle<ITHistSvc> histSvc("THistSvc",name());

    m_n_mu = new TH1D( "m_n_mu", "Number of muons in event", 1000,0,1000);
    m_n_elc = new TH1D( "m_n_elc", "Number of electrons in event", 1000,0,1000);
    m_n_id = new TH1D( "m_n_id", "Number id tracks in event", 1000,0,1000);

    m_n_mu_sel = new TH1D( "m_n_mu_sel", "Number of selected muons in event", 100,0,100);
    m_n_elc_sel = new TH1D( "m_n_elc_sel", "Number of selected electrons in event", 100,0,100);
    m_n_id_sel = new TH1D( "m_n_id_sel", "Number of selected id tracks in event", 500,0,500);

    m_mumu_cf_nonflip = new TH1D( "m_mumu_cf_nonflip", "mumu cutflow (no flip)", 11,0,11);
    m_mumu_cf_flip = new TH1D( "m_mumu_cf_flip", "mumu cutflow (flip)", 11,0,11);

    m_ee_cf_nonflip = new TH1D( "m_ee_cf_nonflip", "ee cutflow (no flip)", 11,0,11);
    m_ee_cf_flip = new TH1D( "m_ee_cf_flip", "ee cutflow (flip)", 11,0,11);

    m_emu_cf_nonflip = new TH1D( "m_emu_cf_nonflip", "emu cutflow (no flip)", 11,0,11);
    m_emu_cf_flip = new TH1D( "m_emu_cf_flip", "emu cutflow (flip)", 11,0,11);

    m_idid_cf_nonflip = new TH1D( "m_idid_cf_nonflip", "idid cutflow (no flip)", 10,0,10);
    m_idid_cf_flip = new TH1D( "m_idid_cf_flip", "idid cutflow (flip)", 10,0,10);

    m_mut_cf_nonflip = new TH1D( "m_mut_cf_nonflip", "mut cutflow (no flip)", 10,0,10);
    m_mut_cf_flip = new TH1D( "m_mut_cf_flip", "mut cutflow (flip)", 10,0,10);

    m_et_cf_nonflip = new TH1D( "m_et_cf_nonflip", "et cutflow (no flip)", 10,0,10);
    m_et_cf_flip = new TH1D( "m_et_cf_flip", "et cutflow (flip)", 10,0,10);

    m_mumu_nonflip_R = new TH1F( "m_mumu_nonflip_R", "vertex R (no flip)", 200,0,600);
    m_mumu_nonflip_z = new TH1F( "m_mumu_nonflip_z", "vertex z (no flip)", 1000,-1000,1000);
    m_mumu_nonflip_M = new TH1F( "m_mumu_nonflip_M", "vertex M (no flip)", 1000,0,1000);
    m_mumu_nonflip_Rcos = new TH1F( "m_mumu_nonflip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);
    m_mumu_flip_R = new TH1F( "m_mumu_flip_R", "vertex R (flip)", 200,0,600);
    m_mumu_flip_z = new TH1F( "m_mumu_flip_z", "vertex z (flip)", 1000,-1000,1000);
    m_mumu_flip_M = new TH1F( "m_mumu_flip_M", "vertex M (flip)", 1000,0,1000);
    m_mumu_flip_Rcos = new TH1F( "m_mumu_flip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);

    m_ee_nonflip_R = new TH1F( "m_ee_nonflip_R", "vertex R (no flip)", 200,0,600);
    m_ee_nonflip_z = new TH1F( "m_ee_nonflip_z", "vertex z (no flip)", 1000,-1000,1000);
    m_ee_nonflip_M = new TH1F( "m_ee_nonflip_M", "vertex M (no flip)", 1000,0,1000);
    m_ee_nonflip_Rcos = new TH1F( "m_ee_nonflip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);
    m_ee_flip_R = new TH1F( "m_ee_flip_R", "vertex R (flip)", 200,0,600);
    m_ee_flip_z = new TH1F( "m_ee_flip_z", "vertex z (flip)", 1000,-1000,1000);
    m_ee_flip_M = new TH1F( "m_ee_flip_M", "vertex M (flip)", 1000,0,1000);
    m_ee_flip_Rcos = new TH1F( "m_ee_flip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);

    m_emu_nonflip_R = new TH1F( "m_emu_nonflip_R", "vertex R (no flip)", 200,0,600);
    m_emu_nonflip_z = new TH1F( "m_emu_nonflip_z", "vertex z (no flip)", 1000,-1000,1000);
    m_emu_nonflip_M = new TH1F( "m_emu_nonflip_M", "vertex M (no flip)", 1000,0,1000);
    m_emu_nonflip_Rcos = new TH1F( "m_emu_nonflip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);
    m_emu_flip_R = new TH1F( "m_emu_flip_R", "vertex R (flip)", 200,0,600);
    m_emu_flip_z = new TH1F( "m_emu_flip_z", "vertex z (flip)", 1000,-1000,1000);
    m_emu_flip_M = new TH1F( "m_emu_flip_M", "vertex M (flip)", 1000,0,1000);
    m_emu_flip_Rcos = new TH1F( "m_emu_flip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);

    m_idid_nonflip_M = new TH1F( "m_idid_nonflip_M", "mass in GeV", 1000, 0, 1000);
    m_idid_nonflip_R = new TH1F( "m_idid_nonflip_R", "vertex R (no flip)", 600,0,600);
    m_idid_nonflip_z = new TH1F( "m_idid_nonflip_z", "vertex z (no flip)", 1000,-1000,1000);
    m_idid_nonflip_l = new TH1F( "m_idid_nonflip_l", "vertex l (no flip)", 100, 0, 1000.);
    m_idid_nonflip_n_tracks = new TH1F( "m_idid_nonflip_n_tracks", "vertex vs track multiplicity (nonflip)", 500,0,500);

    m_idid_flip_M = new TH1F("m_idid_flip_M","mass in GeV", 1000, 0, 1000. );
    m_idid_flip_R = new TH1F( "m_idid_flip_R", "vertex R (flip)", 600,0,600);
    m_idid_flip_z = new TH1F( "m_idid_flip_z", "vertex z (flip)", 1000,-1000,1000);
    m_idid_flip_l = new TH1F("m_idid_flip_l","vertex l (flip)", 100, 0, 1000.);
    m_idid_flip_n_tracks = new TH1F( "m_idid_flip_n_tracks", "vertex vs track multiplicity (flip)", 500,0,500);
    m_idid_flip_deltaR = new TH1F( "m_idid_flip_deltaR", "vertex deltaR (flip)", 20,0.,2.);
    m_idid_flip_chi2_ndof = new TH1F("m_idid_flip_chi2_ndof","chi2 / ndof", 200,0.,50 );

    m_mut_nonflip_R = new TH1F( "m_mut_nonflip_R", "vertex R (no flip)", 600,0,600);
    m_mut_nonflip_z = new TH1F( "m_mut_nonflip_z", "vertex z (no flip)", 1000,-1000,1000);
    m_mut_nonflip_Rcos = new TH1F( "m_mut_nonflip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);
    m_mut_flip_M = new TH1F("m_mut_flip_M","DV mass in GeV", 1000, 0, 1000. );
    m_mut_flip_R = new TH1F( "m_mut_flip_R", "vertex R (flip)", 600,0,600);
    m_mut_flip_z = new TH1F( "m_mut_flip_z", "vertex z (flip)", 1000,-1000,1000);
    m_mut_flip_l = new TH1F("m_mut_flip_l","vertex l (flip)", 100, 0, 2000.);
    m_mut_flip_Rcos = new TH1F( "m_mut_flip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);
    m_mut_flip_deltaR = new TH1F( "m_mut_flip_deltaR", "vertex deltaR (flip)", 20,0.,2.);
    m_mut_flip_chi2_ndof = new TH1F("m_mut_flip_chi2_ndof","chi2 / ndof", 200,0.,50 );

    m_et_nonflip_R = new TH1F( "m_et_nonflip_R", "vertex R (no flip)", 600,0,600);
    m_et_nonflip_z = new TH1F( "m_et_nonflip_z", "vertex z (no flip)", 1000,-1000,1000);
    m_et_nonflip_Rcos = new TH1F( "m_et_nonflip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);
    m_et_flip_M = new TH1F("m_et_flip_M","DV mass in GeV", 1000, 0, 1000. );
    m_et_flip_R = new TH1F( "m_et_flip_R", "vertex R (flip)", 600,0,600);
    m_et_flip_z = new TH1F( "m_et_flip_z", "vertex z (flip)", 1000,-1000,1000);
    m_et_flip_l = new TH1F("m_et_flip_l","vertex l (flip)", 100, 0, 2000.);
    m_et_flip_Rcos = new TH1F( "m_et_flip_Rcos", "vertex Rcos (no flip)", 5000, 0, 5);
    m_et_flip_deltaR = new TH1F( "m_et_flip_deltaR", "vertex deltaR (flip)", 20,0.,2.);
    m_et_flip_chi2_ndof = new TH1F("m_et_flip_chi2_ndof","chi2 / ndof", 200,0.,50 );

    // Track parameters
    m_track_nonflip_p = new TH1F( "m_track_nonflip_p", "track p", 10000,0,1000);
    m_track_nonflip_pt = new TH1F( "m_track_nonflip_pt", "track pt", 10000,0,1000);
    m_track_nonflip_z0 = new TH1F( "m_track_nonflip_z0", "track z0", 2000,-1000,1000);
    m_track_nonflip_d0 = new TH1F( "m_track_nonflip_d0", "track d0", 600,-300,300);
    m_track_nonflip_phi = new TH1F( "m_track_nonflip_phi", "track phi", 1000,-M_PI,M_PI);
    m_track_nonflip_theta = new TH1F( "m_track_nonflip_theta", "track theta", 200,0,M_PI);
    m_track_nonflip_d0sigma = new TH1F( "m_track_nonflip_d0sigma", "d0sigma (no flip)", 500,0,50);
    m_track_nonflip_z0sigma = new TH1F( "m_track_nonflip_z0sigma", "z0sigma (no flip)", 2000,-100,100);
    m_track_nonflip_d0_over_d0sigma = new TH1F( "m_track_nonflip_d0_d0sigma", "d0/d0sigma (no flip)", 500,0,50);
    m_track_nonflip_z0_over_z0sigma = new TH1F( "m_track_nonflip_z0_z0sigma", "z0/z0sigma (no flip)", 2000,-100,100);

    m_track_nonflip_p_wrtSV = new TH1F( "m_track_nonflip_p_wrtSV", "track p_wrtSV", 10000,0,1000);
    m_track_nonflip_pt_wrtSV = new TH1F( "m_track_nonflip_pt_wrtSV", "track pt_wrtSV", 10000,0,1000);
    m_track_nonflip_z0_wrtSV = new TH1F( "m_track_nonflip_z0_wrtSV", "track z0_wrtSV", 20000,-1000,1000);
    m_track_nonflip_d0_wrtSV = new TH1F( "m_track_nonflip_d0_wrtSV", "track d0_wrtSV", 6000,-300,300);
    m_track_nonflip_phi_wrtSV = new TH1F( "m_track_nonflip_phi_wrtSV", "track phi_wrtSV", 1000,-M_PI,M_PI);
    m_track_nonflip_theta_wrtSV = new TH1F( "m_track_nonflip_theta_wrtSV", "track theta_wrtSV", 200,0,M_PI);
    m_track_nonflip_d0sigma_wrtSV = new TH1F( "m_track_nonflip_d0sigma_wrtSV", "d0sigma_wrtSV (no flip)", 500,0,50);
    m_track_nonflip_z0sigma_wrtSV = new TH1F( "m_track_nonflip_z0sigma_wrtSV", "z0sigma_wrtSV (no flip)", 2000,-100,100);
    m_track_nonflip_d0_over_d0sigma_wrtSV = new TH1F( "m_track_nonflip_d0_d0sigma_wrtSV", "d0/d0sigma_wrtSV (no flip)", 500,0,50);
    m_track_nonflip_z0_over_z0sigma_wrtSV = new TH1F( "m_track_nonflip_z0_z0sigma_wrtSV", "z0/z0sigma_wrtSV (no flip)", 2000,-100,100);

    m_track_flip_p = new TH1F( "m_track_flip_p", "track p", 10000,0,1000);
    m_track_flip_pt = new TH1F( "m_track_flip_pt", "track pt", 10000,0,1000);
    m_track_flip_z0 = new TH1F( "m_track_flip_z0", "track z0", 2000,-1000,1000);
    m_track_flip_d0 = new TH1F( "m_track_flip_d0", "track d0", 600,-300,300);
    m_track_flip_phi = new TH1F( "m_track_flip_phi", "track phi", 1000,-M_PI,M_PI);
    m_track_flip_theta = new TH1F( "m_track_flip_theta", "track theta", 200,0,M_PI);
    m_track_flip_d0sigma = new TH1F( "m_track_flip_d0sigma", "d0sigma (flip)", 500,0,50);
    m_track_flip_z0sigma = new TH1F( "m_track_flip_z0sigma", "z0sigma (flip)", 2000,-100,100);
    m_track_flip_d0_over_d0sigma = new TH1F( "m_track_flip_d0_d0sigma", "d0/d0sigma (flip)", 500,0,50);
    m_track_flip_z0_over_z0sigma = new TH1F( "m_track_flip_z0_z0sigma", "z0/z0sigma (flip)", 2000,-100,100);

    m_track_flip_p_wrtSV = new TH1F( "m_track_flip_p_wrtSV", "track p_wrtSV", 10000,0,1000);
    m_track_flip_pt_wrtSV = new TH1F( "m_track_flip_pt_wrtSV", "track pt_wrtSV", 10000,0,1000);
    m_track_flip_z0_wrtSV = new TH1F( "m_track_flip_z0_wrtSV", "track z0_wrtSV", 20000,-1000,1000);
    m_track_flip_d0_wrtSV = new TH1F( "m_track_flip_d0_wrtSV", "track d0_wrtSV", 6000,-300,300);
    m_track_flip_phi_wrtSV = new TH1F( "m_track_flip_phi_wrtSV", "track phi_wrtSV", 1000,-M_PI,M_PI);
    m_track_flip_theta_wrtSV = new TH1F( "m_track_flip_theta_wrtSV", "track theta_wrtSV", 200,0,M_PI);
    m_track_flip_d0sigma_wrtSV = new TH1F( "m_track_flip_d0sigma_wrtSV", "d0sigma_wrtSV (flip)", 500,0,50);
    m_track_flip_z0sigma_wrtSV = new TH1F( "m_track_flip_z0sigma_wrtSV", "z0sigma_wrtSV (flip)", 2000,-100,100);
    m_track_flip_d0_over_d0sigma_wrtSV = new TH1F( "m_track_flip_d0_d0sigma_wrtSV", "d0/d0sigma_wrtSV (flip)", 500,0,50);
    m_track_flip_z0_over_z0sigma_wrtSV = new TH1F( "m_track_flip_z0_z0sigma_wrtSV", "z0/z0sigma_wrtSV (flip)", 2000,-100,100);


    // register histograms
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_mu", m_n_mu) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_elc", m_n_elc) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_id", m_n_id) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_mu_sel", m_n_mu_sel) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_elc_sel", m_n_elc_sel) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_id_sel", m_n_id_sel) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_cf_nonflip", m_mumu_cf_nonflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_cf_flip", m_mumu_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_cf_nonflip", m_ee_cf_nonflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_cf_flip", m_ee_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_cf_nonflip", m_emu_cf_nonflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_cf_flip", m_emu_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_cf_nonflip", m_idid_cf_nonflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_cf_flip", m_idid_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_cf_nonflip", m_mut_cf_nonflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_cf_flip", m_mut_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_cf_nonflip", m_et_cf_nonflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_cf_flip", m_et_cf_flip) );

    // vertex distribution
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_nonflip_R", m_mumu_nonflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_nonflip_z", m_mumu_nonflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_nonflip_M", m_mumu_nonflip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_nonflip_Rcos", m_mumu_nonflip_Rcos) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_flip_R", m_mumu_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_flip_z", m_mumu_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_flip_M", m_mumu_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_flip_Rcos", m_mumu_flip_Rcos) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_nonflip_R", m_ee_nonflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_nonflip_z", m_ee_nonflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_nonflip_M", m_ee_nonflip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_nonflip_Rcos", m_ee_nonflip_Rcos) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_flip_R", m_ee_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_flip_z", m_ee_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_flip_M", m_ee_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_flip_Rcos", m_ee_flip_Rcos) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_nonflip_R", m_emu_nonflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_nonflip_z", m_emu_nonflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_nonflip_M", m_emu_nonflip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_nonflip_Rcos", m_emu_nonflip_Rcos) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_flip_R", m_emu_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_flip_z", m_emu_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_flip_M", m_emu_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_flip_Rcos", m_emu_flip_Rcos) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_nonflip_M", m_idid_nonflip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_nonflip_R", m_idid_nonflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_nonflip_z", m_idid_nonflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_nonflip_l", m_idid_nonflip_l) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_nonflip_n_tracks", m_idid_nonflip_n_tracks) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_M", m_idid_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_R", m_idid_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_z", m_idid_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_l", m_idid_flip_l) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_n_tracks", m_idid_flip_n_tracks) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_deltaR", m_idid_flip_deltaR) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_chi2_ndof", m_idid_flip_chi2_ndof) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_nonflip_R", m_mut_nonflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_nonflip_z", m_mut_nonflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_nonflip_Rcos", m_mut_nonflip_Rcos) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_R", m_mut_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_z", m_mut_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_M", m_mut_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_l", m_mut_flip_l) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_Rcos", m_mut_flip_Rcos) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_deltaR", m_mut_flip_deltaR) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_chi2_ndof", m_mut_flip_chi2_ndof) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_nonflip_R", m_et_nonflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_nonflip_z", m_et_nonflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_nonflip_Rcos", m_et_nonflip_Rcos) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_R", m_et_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_z", m_et_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_M", m_et_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_l", m_et_flip_l) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_Rcos", m_et_flip_Rcos) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_deltaR", m_et_flip_deltaR) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_chi2_ndof", m_et_flip_chi2_ndof) );

    // track parameters
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_p", m_track_nonflip_p) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_pt", m_track_nonflip_pt) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_z0", m_track_nonflip_z0) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_d0", m_track_nonflip_d0) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_phi", m_track_nonflip_phi) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_theta", m_track_nonflip_theta) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_d0sigma", m_track_nonflip_d0sigma) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_z0sigma", m_track_nonflip_z0sigma) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_d0_over_d0sigma", m_track_nonflip_d0_over_d0sigma) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_nonflip_z0_over_z0sigma", m_track_nonflip_z0_over_z0sigma) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_p_wrtSV", m_track_nonflip_p_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_pt_wrtSV", m_track_nonflip_pt_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_z0_wrtSV", m_track_nonflip_z0_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_d0_wrtSV", m_track_nonflip_d0_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_phi_wrtSV", m_track_nonflip_phi_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_theta_wrtSV", m_track_nonflip_theta_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_d0sigma_wrtSV", m_track_nonflip_d0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_z0sigma_wrtSV", m_track_nonflip_z0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_d0_over_d0sigma_wrtSV", m_track_nonflip_d0_over_d0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_nonflip_z0_over_z0sigma_wrtSV", m_track_nonflip_z0_over_z0sigma_wrtSV) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_p", m_track_flip_p) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_pt", m_track_flip_pt) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_z0", m_track_flip_z0) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_d0", m_track_flip_d0) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_phi", m_track_flip_phi) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_theta", m_track_flip_theta) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_d0sigma", m_track_flip_d0sigma) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_z0sigma", m_track_flip_z0sigma) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_d0_over_d0sigma", m_track_flip_d0_over_d0sigma) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtBeam/track_flip_z0_over_z0sigma", m_track_flip_z0_over_z0sigma) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_p_wrtSV", m_track_flip_p_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_pt_wrtSV", m_track_flip_pt_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_z0_wrtSV", m_track_flip_z0_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_d0_wrtSV", m_track_flip_d0_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_phi_wrtSV", m_track_flip_phi_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_theta_wrtSV", m_track_flip_theta_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_d0sigma_wrtSV", m_track_flip_d0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_z0sigma_wrtSV", m_track_flip_z0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_d0_over_d0sigma_wrtSV", m_track_flip_d0_over_d0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/track_wrtSV/track_flip_z0_over_z0sigma_wrtSV", m_track_flip_z0_over_z0sigma_wrtSV) );


    // flag to check MC
    bool isMC;

    // retrieve primary vertices
    const xAOD::VertexContainer* pvc = nullptr;

    // count leptons with nullptr
    int n_mu_null = 0;
    int n_mu_valid = 0;
    int n_el_null = 0;
    int n_el_valid = 0;
    int n_trk_noperigee = 0;

    // counting vertex failed on refitting
    int n_refit_flip_failed = 0;
    int n_refit_flip_succeeded = 0;
    int n_refit_nonflip_failed = 0;
    int n_refit_nonflip_succeeded = 0;

    return StatusCode::SUCCESS;
}

StatusCode FlipBkgEst::finalize() {

    ATH_MSG_INFO("mu null = " << n_mu_null << ", mu valid = " << n_mu_valid << ", el null = " << n_el_null << ", el valid = " << n_el_valid);

    ATH_MSG_INFO("n_trk_noperigee = " << n_trk_noperigee);

    ATH_MSG_INFO("n_refit_nonflip_failed = " << n_refit_nonflip_failed);
    ATH_MSG_INFO("n_refit_nonflip_succeeded = " << n_refit_nonflip_succeeded);
    ATH_MSG_INFO("n_refit_flip_failed = " << n_refit_flip_failed);
    ATH_MSG_INFO("n_refit_flip_succeeded = " << n_refit_flip_succeeded);


    return StatusCode::SUCCESS;
}

StatusCode FlipBkgEst::execute() {  
    ATH_MSG_DEBUG ("Executing " << name() << "...");

    // retrieve event info
    const xAOD::EventInfo* evtInfo = nullptr;
    CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

    // flag to check if data or MC
    isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

    // retrieve lepton and track container
    const xAOD::MuonContainer* muc = nullptr;
    CHECK( evtStore()->retrieve( muc, "Muons" ));

    const xAOD::ElectronContainer* elc = nullptr;
    CHECK( evtStore()->retrieve( elc, "Electrons" ));

    //const xAOD::TrackParticleContainer* vsc = nullptr;
    //CHECK( evtStore()->retrieve( vsc, "VrtSecInclusive_SelectedTrackParticles" ));

    const xAOD::TrackParticleContainer* idc = nullptr;
    CHECK( evtStore()->retrieve( idc, "InDetTrackParticles" ));

    // retrieve primary vertices
    //const xAOD::VertexContainer* pvc = nullptr;
    CHECK( evtStore()->retrieve( pvc, "PrimaryVertices" ));

    // flag to check trigger
    bool trig_passed = false;

    // make copies of leptons
    auto muc_copy = xAOD::shallowCopyContainer(*muc);
    xAOD::setOriginalObjectLink(*muc, *muc_copy.first);

    auto elc_copy = xAOD::shallowCopyContainer(*elc);
    xAOD::setOriginalObjectLink(*elc, *elc_copy.first);

    auto idc_copy = xAOD::shallowCopyContainer(*idc);
    xAOD::setOriginalObjectLink(*idc, *idc_copy.first);

    // apply overlap removal
    m_or->FindOverlap(*elc_copy.first, *muc_copy.first);


    //---------------------------------------
    // Event cut flow
    //---------------------------------------

    // GRL
    if (!isMC and !m_grlTool->passRunLB(*evtInfo)) return StatusCode::SUCCESS;

    // event cleaning
    if(!m_evtc->PassEventCleaning(*evtInfo)) return StatusCode::SUCCESS;

    // trigger check
    if (m_tdt->isPassed("HLT_mu60_0eta105_msonly")) trig_passed = true;
    if (m_tdt->isPassed("HLT_g140_loose")) trig_passed = true;
    if (m_tdt->isPassed("HLT_2g50_loose")) trig_passed = true;
    if(!trig_passed) return StatusCode::SUCCESS;

    // cosmic veto
    if(!m_cos->PassCosmicEventVeto(*elc, *muc)) return StatusCode::SUCCESS;

    // get primary vertex
    auto pv = m_evtc->GetPV(*pvc);
    auto pv_pos = m_evtc->GetPV(*pvc)->position();

    auto id_sel     = new xAOD::TrackParticleContainer();
    auto id_sel_aux = new xAOD::TrackParticleAuxContainer();
    id_sel->setStore(id_sel_aux);

    m_n_mu->Fill(muc->size());
    m_n_elc->Fill(elc->size());
    m_n_id->Fill(idc->size());

    int n_trk_sel = 0;
    int n_mu_sel = 0;
    int n_el_sel = 0;


    //-----------------------------------
    // track selection and deep copy
    //-----------------------------------


    for(auto mu: *muc_copy.first) {

        // overlap removal
        if(m_or->IsOverlap(*mu)) continue;

        // apply muon selection
        if(!m_leptool->MuonSelection(*mu)) continue;

        // access muon ID track
        auto mu_tr = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        if(mu_tr == nullptr) {
            n_mu_null++;
            continue;
        }
        else n_mu_valid++;

        if(!m_vertexer->GoodTrack(*mu_tr)) continue;

        // mark ID track as muon
        mu_tr->auxdecor<int>("muon") = 1;
        ATH_MSG_DEBUG("DEBUG: Found muon, ptr = " << mu_tr);

        // count selected muon
        n_mu_sel++;

    }

    for(auto el: *elc_copy.first) {
        // overlap removal
        if(m_or->IsOverlap(*el)) continue;

        // remove bad electrons
        if(!m_leptool->BadClusterRemoval(*el)) continue;

        // kinematic cut
        if(!m_leptool->ElectronKinematicCut(*el)) continue;

        // Electron identification
        if(!m_leptool->ElectronID(*el)) continue;

        // access electron ID track
        auto el_tr = xAOD::EgammaHelpers::getOriginalTrackParticle(el);
        if(el_tr == nullptr) {
            n_el_null++;
            continue;
        }
        else n_el_valid++;

        if(!m_vertexer->GoodTrack(*el_tr)) continue;

        // mark ID track as electron
        el_tr->auxdecor<int>("electron") = 1;
        ATH_MSG_DEBUG("DEBUG: Found electron, ptr = " << el_tr);

        // count selected electron
        n_el_sel++;

    }

    for(auto id_tr: *idc_copy.first) {

        // track quality requirement
        if(!m_vertexer->GoodTrack(*id_tr)) continue;

        // perigee requirement
        const Trk::Perigee* perigee = &(id_tr->perigeeParameters());

        if(!perigee) {
            ATH_MSG_DEBUG("DEBUG: Found track without perigee");
            n_trk_noperigee++;
            continue; 
        }

        // copy ID track
        xAOD::TrackParticle* tr_ptr = new xAOD::TrackParticle();
        id_sel->push_back(tr_ptr);
        xAOD::safeDeepCopy(*id_tr, *tr_ptr);

        // decorate track with flipping status
        tr_ptr->auxdecor<std::string>("Flip") = "new";

        // lepton flag
        bool isLepton = false;

        // get original track
        auto orig_tr = dynamic_cast<const xAOD::TrackParticle*>(xAOD::getOriginalObject(*id_tr));
        if (orig_tr == nullptr) orig_tr = id_tr;

        ATH_MSG_DEBUG("DEBUG: current orig_tr ptr = " << orig_tr);
        ATH_MSG_DEBUG("DEBUG: current id_tr ptr = " << orig_tr);

        // check if this ID track is a lepton
        if((orig_tr)->auxdecor<int>("electron") or (orig_tr)->auxdecor<int>("muon")) {
            isLepton = true;
            ATH_MSG_DEBUG("DEBUG: found electron or muon from orig_tr");
        }

        // count non-leptonic selected track
        if (!isLepton) {
            n_trk_sel++;
        }

    }

    // Fill selected leptons
    m_n_mu_sel->Fill(n_mu_sel);
    m_n_elc_sel->Fill(n_el_sel);
    m_n_id_sel->Fill(n_trk_sel);

    // Skip the event if the number of selected tracks is more than m_SelTrkMaxCutoff
    // required in VrtSecInclusive
    if ((n_mu_sel + n_el_sel + id_sel->size()) > 300) return StatusCode::SUCCESS;

    // create containers for displaced vertices
    auto dv     = new xAOD::VertexContainer();
    auto dv_aux = new xAOD::VertexAuxContainer();

    dv->setStore(dv_aux);
    m_vertexer->SetVtxContainer(*dv);

    //--------------------------------------------------------
    // loop over combinations of tracks and perform vertexing
    //--------------------------------------------------------
    if(id_sel->size() > 1) {
        // perform vertexing
        for(auto id1_itr = id_sel->begin(); id1_itr != id_sel->end(); id1_itr++)
        {
            for(auto id2_itr = id1_itr+1; id2_itr != id_sel->end(); id2_itr++)
            {
                // skip same charge track pair
                if ((**id1_itr).charge() * (**id2_itr).charge() == 1) continue;

                // get original track
                auto orig_tr1 = dynamic_cast<const xAOD::TrackParticle*>(xAOD::getOriginalObject(**id1_itr));
                auto orig_tr2 = dynamic_cast<const xAOD::TrackParticle*>(xAOD::getOriginalObject(**id2_itr));
                if (orig_tr1 == nullptr) orig_tr1 = *id1_itr;
                if (orig_tr2 == nullptr) orig_tr2 = *id2_itr;

                // count number of leptons in this pair
                int n_mu = 0;
                int n_elc = 0;

                if((orig_tr1)->auxdecor<int>("electron")) n_elc++;
                if((orig_tr2)->auxdecor<int>("electron")) n_elc++;
                if((orig_tr1)->auxdecor<int>("muon")) n_mu++;
                if((orig_tr2)->auxdecor<int>("muon")) n_mu++;

                ATH_MSG_DEBUG("DEBUG: New pair ==========================");
                ATH_MSG_DEBUG("DEBUG: \tn_elc = " << n_elc);
                ATH_MSG_DEBUG("DEBUG: \tn_mu = " << n_mu);

                // find vertex type
                std::string channel = FindDecayChannel(n_mu, n_elc);

                // fill track pair count
                FillTrackPair(channel);

                // find track flip status. possible values: new, orig, flip 
                std::string fs1 = (*id1_itr)->auxdecor<std::string>("Flip");
                std::string fs2 = (*id2_itr)->auxdecor<std::string>("Flip");
                
                ATH_MSG_DEBUG("DEBUG: is this track 1 flipped already ? = " << (*id1_itr)->auxdecor<std::string>("Flip"));
                ATH_MSG_DEBUG("DEBUG: is this track 2 flipped already ? = " << (*id2_itr)->auxdecor<std::string>("Flip"));

                // flip track 1, leave track 2 unflipped
                if(((fs1=="new") or (fs1=="flip")) and ((fs2=="new") or (fs2=="orig"))){
                    ATH_MSG_DEBUG("DEBUG: flipping track 1. tr1 = " << (*id1_itr) << ", tr1 status = " << fs1 << ", tr2 = " << (*id2_itr) << ", tr2 status = " << fs2);

                    // set track flipping status of each track
                    (*id1_itr)->auxdecor<std::string>("Flip") = "flip";
                    (*id2_itr)->auxdecor<std::string>("Flip") = "orig";

                    // get track parameters
                    auto dp = (**id1_itr).definingParameters();

                    // vertex fit of two original tracks
                    PerformFit(**id1_itr, **id2_itr, pv_pos, channel);

                    // parameters to invert
                    float p_z = pv->z(); // primary vertex z coordinate
                    float d0_inv = -dp[0];
                    //float z0_inv = 2 * p_z - dp[1];
                    float z0_inv = -dp[1];
                    //float z0_inv = dp[1];
                    float phi0_inv = dp[2] > 0 ? -M_PI+dp[2] : M_PI+dp[2]; // just to keep phi within [-PI,+PI]
                    float theta_inv = M_PI - dp[3];

                    // set defining parameters to flip track
                    (**id1_itr).setDefiningParameters(d0_inv, z0_inv, phi0_inv, theta_inv, dp[4]);

                    // vertex fit of two original tracks
                    PerformFit_flip(**id1_itr, **id2_itr, pv_pos, channel);

                    // flip the track back
                    (**id1_itr).setDefiningParameters(dp[0], dp[1], dp[2], dp[3], dp[4]);
                }

                // flip track 2, leave track 1 unflipped
                else if((fs1=="orig" and ((fs2=="new") or (fs2=="flip"))) or (fs1=="new" and fs2=="flip")) {
                    ATH_MSG_DEBUG("DEBUG: flipping track 2. tr1 = " << (*id1_itr) << ", tr1 status = " << fs1 << ", tr2 = " << (*id2_itr) << ", tr2 status = " << fs2);

                    // set track flipping status of each track
                    (*id1_itr)->auxdecor<std::string>("Flip") = "orig";
                    (*id2_itr)->auxdecor<std::string>("Flip") = "flip";

                    // get track parameters
                    auto dp = (**id2_itr).definingParameters();

                    // vertex fit of two original tracks
                    PerformFit(**id1_itr, **id2_itr, pv_pos, channel);

                    // parameters to invert
                    float p_z = pv->z(); // primary vertex z coordinate
                    float d0_inv = -dp[0];
                    //float z0_inv = 2 * p_z - dp[1];
                    float z0_inv = -dp[1];
                    //float z0_inv = dp[1];
                    float phi0_inv = dp[2] > 0 ? -M_PI+dp[2] : M_PI+dp[2]; // just to keep phi within [-PI,+PI]
                    float theta_inv = M_PI - dp[3];

                    // set defining parameters to flip track
                    (**id2_itr).setDefiningParameters(d0_inv, z0_inv, phi0_inv, theta_inv, dp[4]);

                    // vertex fit of two original tracks
                    PerformFit_flip(**id1_itr, **id2_itr, pv_pos, channel);

                    // flip the track back
                    (**id2_itr).setDefiningParameters(dp[0], dp[1], dp[2], dp[3], dp[4]);
                }
                else {
                    ATH_MSG_DEBUG("DEBUG: non-physical pair. removed. tr1 = " << (*id1_itr) << ", tr1 status = " << fs1 << ", tr2 = " << (*id2_itr) << ", tr2 status = " << fs2);

                }
            }
        }
    }

    delete dv;
    delete dv_aux;
    
    delete elc_copy.first;
    delete elc_copy.second;
    delete muc_copy.first;
    delete muc_copy.second;
    //delete el_sel;
    //delete el_sel_aux;
    //delete mu_sel;
    //delete mu_sel_aux;

    return StatusCode::SUCCESS;
}

StatusCode FlipBkgEst::beginInputFile() { 
    return StatusCode::SUCCESS;
}

void FlipBkgEst::PerformFit(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv,std::string channel)
{
    // create truth vertex for matching
    const xAOD::TruthVertex *tru_matched = nullptr;

    // perform vertex fit
    ATH_MSG_DEBUG("Performing vertex fit, channel = " << channel);
    auto fit = m_vertexer->FitVertex(tr1, tr2, pv);

    // cut flow
    if(fit) {

        // refit vertex
        if(RefitVertex(tr1, tr2, *fit)){
        //if(true){

            // count refit-succeeded vertices
            n_refit_nonflip_succeeded++;

            // mass and position cut
            float dv_R_max = 300;
            float dv_z_max = 300;

            // access position of vertices
            auto dv_pos = fit->position();
            auto pv_pos = m_evtc->GetPV(*pvc)->position();
   
            // distance in 3d vector
            auto dist = pv_pos - dv_pos;

            // get position of DV
            float dv_R = dist.perp();                 // R in [mm]
            float dv_z_abs = std::abs(dist.z());      // z in [mm]
   
            // position of vertex w.r.t. pv
            float dv_z = dist.z();
            float dv_l = sqrt( dist.perp()*dist.perp() + dist.z()*dist.z() );
            float dv_mass = std::fabs(m_accMass(*fit)) / 1000.; // in MeV

            // define TLorentzVector of decay particles
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;
            tlv_tp0 = tr1.p4();
            tlv_tp1 = tr2.p4();
            
            // get R_cos
            float deltaPhiMinusPi = std::fabs(std::fabs(tlv_tp0.DeltaPhi(tlv_tp1)) - std::acos(-1.));
            float sumEta = tlv_tp0.Eta() + tlv_tp1.Eta();
            float dv_Rcos = std::sqrt(sumEta * sumEta + deltaPhiMinusPi * deltaPhiMinusPi);


            // mass cut
            float mass_min = 10.;

            if ((channel == "mumu") or (channel == "ee") or (channel == "emu")) return;

            if(channel == "mumu") {

                // cut flow flag
                bool pass_vertex = true;
            
                if (pass_vertex) m_mumu_cf_nonflip->Fill("#mu#mu",1);

                // place holder to match with main analysis
                if (pass_vertex) m_mumu_cf_nonflip->Fill("place holder",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_nonflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_nonflip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_nonflip->Fill("#mu^{+}#mu^{-}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_nonflip->Fill("DisabledModule", 1);

                // material veto
                m_mumu_cf_nonflip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_nonflip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_nonflip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_nonflip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_mumu_nonflip_R->Fill(dv_R);
                    m_mumu_nonflip_z->Fill(dv_z);
                    m_mumu_nonflip_M->Fill(dv_mass);
                    m_mumu_nonflip_Rcos->Fill(dv_Rcos);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_mumu_cf_nonflip->Fill("Truth matched", 1);
                    }
                }
    
            }
            if(channel == "emu") {

                // cut flow flag
                bool pass_vertex = true;

                if (pass_vertex) m_emu_cf_nonflip->Fill("e#mu",1);

                // place holder to match with main analysis
                if (pass_vertex) m_emu_cf_nonflip->Fill("place holder",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_nonflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_nonflip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_nonflip->Fill("e^{+/-}#mu^{-/+}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_nonflip->Fill("DisabledModule", 1);

                // material veto (excl. mu)
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_nonflip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_emu_cf_nonflip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_emu_cf_nonflip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_emu_cf_nonflip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_emu_nonflip_R->Fill(dv_R);
                    m_emu_nonflip_z->Fill(dv_z);
                    m_emu_nonflip_M->Fill(dv_mass);
                    m_emu_nonflip_Rcos->Fill(dv_Rcos);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_emu_cf_nonflip->Fill("Truth matched", 1);
                    }
                }
            }
            if(channel == "ee") {

                // cut flow flag
                bool pass_vertex = true;

                if (pass_vertex) m_ee_cf_nonflip->Fill("ee",1);

                // place holder to match with main analysis
                if (pass_vertex) m_ee_cf_nonflip->Fill("place holder",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_nonflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_nonflip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_nonflip->Fill("e^{+}e^{-}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_nonflip->Fill("DisabledModule", 1);

                // material veto (excl. mu)
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_nonflip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_ee_cf_nonflip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_ee_cf_nonflip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_ee_cf_nonflip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_ee_nonflip_R->Fill(dv_R);
                    m_ee_nonflip_z->Fill(dv_z);
                    m_ee_nonflip_M->Fill(dv_mass);
                    m_ee_nonflip_Rcos->Fill(dv_Rcos);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_ee_cf_nonflip->Fill("Truth matched", 1);
                    }
                }
            }
            if(channel == "idid") {

                // cut flow flag
                bool pass_vertex = true;
            
                if (pass_vertex) m_idid_cf_nonflip->Fill("xx",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_nonflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_nonflip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_nonflip->Fill("x^{+}x^{-}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_nonflip->Fill("DisabledModule", 1);

                // material veto (excl. mu)
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_nonflip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_idid_cf_nonflip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_idid_cf_nonflip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_idid_cf_nonflip->Fill("z_{DV} > 300 mm", 1);

                // track z0 wrt beam pipe
                if(std::abs((tr1.z0()) < 10) or (std::abs(tr2.z0()) < 10)) pass_vertex = false;;
                if (pass_vertex) m_idid_cf_nonflip->Fill("Track z_{0} > 10 mm (BP)", 1);




                // vertex distribution fill
                if (pass_vertex) {
                    m_idid_nonflip_R->Fill(dv_R);
                    m_idid_nonflip_z->Fill(dv_z);
                    m_idid_nonflip_M->Fill(dv_mass);
                    m_idid_nonflip_l->Fill(dv_l);

                    // fill idid vertex vs track multiplicity
                    m_idid_nonflip_n_tracks->Fill(n_trk_sel);

                    // fill track parameters wrt SV
                    const Trk::Perigee* sv_perigee1 = m_trackToVertexTool->perigeeAtVertex(tr1, dv_pos);
                    const Trk::Perigee* sv_perigee2 = m_trackToVertexTool->perigeeAtVertex(tr2, dv_pos);

                    double tr1_d0_wrtSV        = sv_perigee1->parameters() [Trk::d0];
                    double tr1_z0_wrtSV        = sv_perigee1->parameters() [Trk::z0];
                    double tr1_theta_wrtSV     = sv_perigee1->parameters() [Trk::theta];
                    double tr1_phi_wrtSV       = sv_perigee1->parameters() [Trk::phi];
                    double tr1_qOverP_wrtSV    = sv_perigee1->parameters() [Trk::qOverP];
                    double tr1_p_wrtSV         = 1.0 / fabs(tr1_qOverP_wrtSV) / 1000.; // GeV
                    double tr1_pt_wrtSV        = tr1_p_wrtSV * std::sin( tr1_theta_wrtSV );
                    double tr1_d0sigma_wrtSV   = (*sv_perigee2->covariance())( Trk::d0 );
                    double tr1_z0sigma_wrtSV   = (*sv_perigee2->covariance())( Trk::z0 );
                    double tr1_d0_over_d0sigma_wrtSV   = tr1_d0_wrtSV / tr1_d0sigma_wrtSV;
                    double tr1_z0_over_z0sigma_wrtSV   = tr1_z0_wrtSV / tr1_z0sigma_wrtSV;

                    double tr2_d0_wrtSV        = sv_perigee2->parameters() [Trk::d0];
                    double tr2_z0_wrtSV        = sv_perigee2->parameters() [Trk::z0];
                    double tr2_theta_wrtSV     = sv_perigee2->parameters() [Trk::theta];
                    double tr2_phi_wrtSV       = sv_perigee2->parameters() [Trk::phi];
                    double tr2_qOverP_wrtSV    = sv_perigee2->parameters() [Trk::qOverP];
                    double tr2_p_wrtSV         = 1.0 / fabs(tr2_qOverP_wrtSV) / 1000.; // GeV
                    double tr2_pt_wrtSV        = tr2_p_wrtSV * std::sin( tr2_theta_wrtSV );
                    double tr2_d0sigma_wrtSV   = (*sv_perigee2->covariance())( Trk::d0 );
                    double tr2_z0sigma_wrtSV   = (*sv_perigee2->covariance())( Trk::z0 );
                    double tr2_d0_over_d0sigma_wrtSV   = tr2_d0_wrtSV / tr2_d0sigma_wrtSV;
                    double tr2_z0_over_z0sigma_wrtSV   = tr2_z0_wrtSV / tr2_z0sigma_wrtSV;

                    // fill track parameters wrt SV
                    m_track_nonflip_d0_wrtSV->Fill (tr1_d0_wrtSV);
                    m_track_nonflip_z0_wrtSV->Fill (tr1_z0_wrtSV);
                    m_track_nonflip_phi_wrtSV->Fill(tr1_phi_wrtSV);
                    m_track_nonflip_theta_wrtSV->Fill(tr1_theta_wrtSV);
                    m_track_nonflip_p_wrtSV->Fill(tr1_p_wrtSV);
                    m_track_nonflip_pt_wrtSV->Fill(tr1_pt_wrtSV);
                    m_track_nonflip_d0sigma_wrtSV->Fill(tr1_d0sigma_wrtSV);
                    m_track_nonflip_z0sigma_wrtSV->Fill(tr1_z0sigma_wrtSV);
                    m_track_nonflip_d0_over_d0sigma_wrtSV->Fill(tr1_d0_over_d0sigma_wrtSV);
                    m_track_nonflip_z0_over_z0sigma_wrtSV->Fill(tr1_z0_over_z0sigma_wrtSV);

                    m_track_nonflip_d0_wrtSV->Fill (tr2_d0_wrtSV);
                    m_track_nonflip_z0_wrtSV->Fill (tr2_z0_wrtSV);
                    m_track_nonflip_phi_wrtSV->Fill(tr2_phi_wrtSV);
                    m_track_nonflip_theta_wrtSV->Fill(tr2_theta_wrtSV);
                    m_track_nonflip_p_wrtSV->Fill(tr2_p_wrtSV);
                    m_track_nonflip_pt_wrtSV->Fill(tr2_pt_wrtSV);
                    m_track_nonflip_d0sigma_wrtSV->Fill(tr2_d0sigma_wrtSV);
                    m_track_nonflip_z0sigma_wrtSV->Fill(tr2_z0sigma_wrtSV);
                    m_track_nonflip_d0_over_d0sigma_wrtSV->Fill(tr2_d0_over_d0sigma_wrtSV);
                    m_track_nonflip_z0_over_z0sigma_wrtSV->Fill(tr2_z0_over_z0sigma_wrtSV);

                    // fill track parameters wrt beam pipe
                    double tr1_p = 1.0 / fabs(tr1.qOverP()) / 1000.; // GeV
                    double tr1_pt = tr1_p * std::sin(tr1.theta());
                    double tr2_p = 1.0 / fabs(tr2.qOverP()) / 1000.; // GeV
                    double tr2_pt = tr2_p * std::sin(tr2.theta());
                    m_track_nonflip_d0->Fill(tr1.d0());
                    m_track_nonflip_z0->Fill(tr1.z0());
                    m_track_nonflip_phi->Fill(tr1.phi());
                    m_track_nonflip_theta->Fill(tr1.theta());
                    m_track_nonflip_p->Fill(tr1_p);
                    m_track_nonflip_pt->Fill(tr1_pt);
                    m_track_nonflip_d0sigma->Fill((tr1).definingParametersCovMatrixVec()[0]);
                    m_track_nonflip_z0sigma->Fill((tr1).definingParametersCovMatrixVec()[1]);
                    m_track_nonflip_d0_over_d0sigma->Fill((tr1).definingParameters()[0] / (tr1).definingParametersCovMatrixVec()[0]);
                    m_track_nonflip_z0_over_z0sigma->Fill((tr1).definingParameters()[1] / (tr1).definingParametersCovMatrixVec()[1]);

                    m_track_nonflip_d0->Fill(tr2.d0());
                    m_track_nonflip_z0->Fill(tr2.z0());
                    m_track_nonflip_phi->Fill(tr2.phi());
                    m_track_nonflip_theta->Fill(tr2.theta());
                    m_track_nonflip_p->Fill(tr2_p);
                    m_track_nonflip_pt->Fill(tr2_pt);
                    m_track_nonflip_d0sigma->Fill((tr2).definingParametersCovMatrixVec()[0]);
                    m_track_nonflip_z0sigma->Fill((tr2).definingParametersCovMatrixVec()[1]);
                    m_track_nonflip_d0_over_d0sigma->Fill((tr2).definingParameters()[0] / (tr2).definingParametersCovMatrixVec()[0]);
                    m_track_nonflip_z0_over_z0sigma->Fill((tr2).definingParameters()[1] / (tr2).definingParametersCovMatrixVec()[1]);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_idid_cf_nonflip->Fill("Truth matched", 1);
                    }
                }
    
            }

            if(channel == "mut") {

                // cut flow flag
                bool pass_vertex = true;
            
                if (pass_vertex) m_mut_cf_nonflip->Fill("#mux",1);

                // place holder to match with main analysis
                //m_mut_cf_nonflip->Fill("place holder",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_nonflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_nonflip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_nonflip->Fill("#mu^{+,-}x^{-,+}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_nonflip->Fill("DisabledModule", 1);

                // material veto
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_nonflip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_mut_cf_nonflip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_mut_cf_nonflip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_mut_cf_nonflip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_mut_nonflip_R->Fill(dv_R);
                    m_mut_nonflip_z->Fill(dv_z);
                    m_mut_nonflip_Rcos->Fill(dv_Rcos);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_mut_cf_nonflip->Fill("Truth matched", 1);
                    }
                }
    
            }

            if(channel == "et") {

                // cut flow flag
                bool pass_vertex = true;
            
                if (pass_vertex) m_et_cf_nonflip->Fill("ex",1);

                // place holder to match with main analysis
                //m_et_cf_nonflip->Fill("place holder",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_et_cf_nonflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_et_cf_nonflip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_et_cf_nonflip->Fill("e^{+,-}x^{-,+}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_et_cf_nonflip->Fill("DisabledModule", 1);

                // material veto
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_et_cf_nonflip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_et_cf_nonflip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_et_cf_nonflip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_et_cf_nonflip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_et_nonflip_R->Fill(dv_R);
                    m_et_nonflip_z->Fill(dv_z);
                    m_et_nonflip_Rcos->Fill(dv_Rcos);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_et_cf_nonflip->Fill("Truth matched", 1);
                    }
                }
    
            }
        }
        else n_refit_nonflip_failed++;
    }

    return;
}

void FlipBkgEst::PerformFit_flip(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv,std::string channel)
{
    // create truth vertex for matching
    const xAOD::TruthVertex *tru_matched = nullptr;

    // perform vertex fit
    ATH_MSG_DEBUG("Performing vertex fit, channel = " << channel);
    auto fit = m_vertexer->FitVertex(tr1, tr2, pv);

    if(fit) {

        // refit vertex
        if(RefitVertex(tr1, tr2, *fit)){
        //if(true){

            // count refit-succeeded vertices
            n_refit_flip_succeeded++;

            // mass and position cut
            float dv_R_max = 300;
            float dv_z_max = 300;

            // access position of vertices
            auto dv_pos = fit->position();
            auto pv_pos = m_evtc->GetPV(*pvc)->position();
   
            // distance in 3d vector
            auto dist = pv_pos - dv_pos;

            // get position of DV
            float dv_R = dist.perp();                 // R in [mm]
            float dv_z_abs = std::abs(dist.z());      // z in [mm]
   
            // position of vertex w.r.t. pv
            float dv_z = dist.z();
            float dv_l = sqrt( dist.perp()*dist.perp() + dist.z()*dist.z() );
            float dv_mass = std::fabs(m_accMass(*fit)) / 1000.; // in MeV

            // define TLorentzVector of decay particles
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;
            tlv_tp0 = tr1.p4();
            tlv_tp1 = tr2.p4();
            
            // get R_cos
            float deltaPhiMinusPi = std::fabs(std::fabs(tlv_tp0.DeltaPhi(tlv_tp1)) - std::acos(-1.));
            float sumEta = tlv_tp0.Eta() + tlv_tp1.Eta();
            float dv_Rcos = std::sqrt(sumEta * sumEta + deltaPhiMinusPi * deltaPhiMinusPi);

            // mass cut
            float mass_min = 10.;

            if(channel == "mumu") {

                // cut flow flag
                bool pass_vertex = true;
            
                if (pass_vertex) m_mumu_cf_flip->Fill("#mu#mu",1);

                // place holder to match with main analysis
                if (pass_vertex) m_mumu_cf_flip->Fill("place holder",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_flip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_flip->Fill("#mu^{+}#mu^{-}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_flip->Fill("DisabledModule", 1);

                // material veto
                m_mumu_cf_flip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_flip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_flip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_mumu_cf_flip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_mumu_flip_R->Fill(dv_R);
                    m_mumu_flip_z->Fill(dv_z);
                    m_mumu_flip_M->Fill(dv_mass);
                    m_mumu_flip_Rcos->Fill(dv_Rcos);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_mumu_cf_flip->Fill("Truth matched", 1);
                    }
                }
    
            }
            if(channel == "emu") {

                // cut flow flag
                bool pass_vertex = true;

                if (pass_vertex) m_emu_cf_flip->Fill("e#mu",1);

                // place holder to match with main analysis
                if (pass_vertex) m_emu_cf_flip->Fill("place holder",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_flip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_flip->Fill("e^{+/-}#mu^{-/+}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_flip->Fill("DisabledModule", 1);

                // material veto (excl. mu)
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_emu_cf_flip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_emu_cf_flip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_emu_cf_flip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_emu_cf_flip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_emu_flip_R->Fill(dv_R);
                    m_emu_flip_z->Fill(dv_z);
                    m_emu_flip_M->Fill(dv_mass);
                    m_emu_flip_Rcos->Fill(dv_Rcos);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_emu_cf_flip->Fill("Truth matched", 1);
                    }
                }
            }
            if(channel == "ee") {

                // cut flow flag
                bool pass_vertex = true;

                if (pass_vertex) m_ee_cf_flip->Fill("ee",1);

                // place holder to match with main analysis
                if (pass_vertex) m_ee_cf_flip->Fill("place holder",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_flip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_flip->Fill("e^{+}e^{-}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_flip->Fill("DisabledModule", 1);

                // material veto (excl. mu)
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_ee_cf_flip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_ee_cf_flip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_ee_cf_flip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_ee_cf_flip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_ee_flip_R->Fill(dv_R);
                    m_ee_flip_z->Fill(dv_z);
                    m_ee_flip_M->Fill(dv_mass);
                    m_ee_flip_Rcos->Fill(dv_Rcos);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_ee_cf_flip->Fill("Truth matched", 1);
                    }
                }
            }
            if(channel == "idid") {

                // cut flow flag
                bool pass_vertex = true;
            
                if (pass_vertex) m_idid_cf_flip->Fill("xx",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_flip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_flip->Fill("x^{+}x^{-}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_idid_cf_flip->Fill("DisabledModule", 1);

                // material veto
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;;
                if (pass_vertex) m_idid_cf_flip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_idid_cf_flip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_idid_cf_flip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_idid_cf_flip->Fill("z_{DV} > 300 mm", 1);

                // track z0 wrt beam pipe
                if(std::abs((tr1.z0()) < 10) or (std::abs(tr2.z0()) < 10)) pass_vertex = false;;
                if (pass_vertex) m_idid_cf_flip->Fill("Track z_{0} > 10 mm (BP)", 1);


                // vertex distribution fill
                if (pass_vertex) {
                    m_idid_flip_M->Fill(dv_mass);
                    m_idid_flip_R->Fill(dv_R);
                    m_idid_flip_z->Fill(dv_z);
                    m_idid_flip_l->Fill(dv_l);
                    m_idid_flip_chi2_ndof->Fill((*fit).chiSquared() / (*fit).numberDoF());

                    // fill idid vertex vs track multiplicity
                    m_idid_flip_n_tracks->Fill(n_trk_sel);

                    float deltaR = tlv_tp0.DeltaR(tlv_tp1);
                    m_idid_flip_deltaR->Fill(deltaR);

                    // fill track parameters wrt SV
                    const Trk::Perigee* sv_perigee1 = m_trackToVertexTool->perigeeAtVertex(tr1, dv_pos);
                    const Trk::Perigee* sv_perigee2 = m_trackToVertexTool->perigeeAtVertex(tr2, dv_pos);

                    double tr1_d0_wrtSV        = sv_perigee1->parameters() [Trk::d0];
                    double tr1_z0_wrtSV        = sv_perigee1->parameters() [Trk::z0];
                    double tr1_theta_wrtSV     = sv_perigee1->parameters() [Trk::theta];
                    double tr1_phi_wrtSV       = sv_perigee1->parameters() [Trk::phi];
                    double tr1_qOverP_wrtSV    = sv_perigee1->parameters() [Trk::qOverP];
                    double tr1_p_wrtSV         = 1.0 / fabs(tr1_qOverP_wrtSV) / 1000.;
                    double tr1_pt_wrtSV        = tr1_p_wrtSV * std::sin( tr1_theta_wrtSV );
                    double tr1_d0sigma_wrtSV   = (*sv_perigee2->covariance())( Trk::d0 );
                    double tr1_z0sigma_wrtSV   = (*sv_perigee2->covariance())( Trk::z0 );
                    double tr1_d0_over_d0sigma_wrtSV   = tr1_d0_wrtSV / tr1_d0sigma_wrtSV;
                    double tr1_z0_over_z0sigma_wrtSV   = tr1_z0_wrtSV / tr1_z0sigma_wrtSV;

                    double tr2_d0_wrtSV        = sv_perigee2->parameters() [Trk::d0];
                    double tr2_z0_wrtSV        = sv_perigee2->parameters() [Trk::z0];
                    double tr2_theta_wrtSV     = sv_perigee2->parameters() [Trk::theta];
                    double tr2_phi_wrtSV       = sv_perigee2->parameters() [Trk::phi];
                    double tr2_qOverP_wrtSV    = sv_perigee2->parameters() [Trk::qOverP];
                    double tr2_p_wrtSV         = 1.0 / fabs(tr2_qOverP_wrtSV) / 1000.;
                    double tr2_pt_wrtSV        = tr2_p_wrtSV * std::sin( tr2_theta_wrtSV );
                    double tr2_d0sigma_wrtSV   = (*sv_perigee2->covariance())( Trk::d0 );
                    double tr2_z0sigma_wrtSV   = (*sv_perigee2->covariance())( Trk::z0 );
                    double tr2_d0_over_d0sigma_wrtSV   = tr2_d0_wrtSV / tr2_d0sigma_wrtSV;
                    double tr2_z0_over_z0sigma_wrtSV   = tr2_z0_wrtSV / tr2_z0sigma_wrtSV;

                    // fill track parameters wrt SV
                    m_track_flip_d0_wrtSV->Fill (tr1_d0_wrtSV);
                    m_track_flip_z0_wrtSV->Fill (tr1_z0_wrtSV);
                    m_track_flip_phi_wrtSV->Fill(tr1_phi_wrtSV);
                    m_track_flip_theta_wrtSV->Fill(tr1_theta_wrtSV);
                    m_track_flip_p_wrtSV->Fill(tr1_p_wrtSV);
                    m_track_flip_pt_wrtSV->Fill(tr1_pt_wrtSV);
                    m_track_flip_d0sigma_wrtSV->Fill(tr1_d0sigma_wrtSV);
                    m_track_flip_z0sigma_wrtSV->Fill(tr1_z0sigma_wrtSV);
                    m_track_flip_d0_over_d0sigma_wrtSV->Fill(tr1_d0_over_d0sigma_wrtSV);
                    m_track_flip_z0_over_z0sigma_wrtSV->Fill(tr1_z0_over_z0sigma_wrtSV);

                    m_track_flip_d0_wrtSV->Fill (tr2_d0_wrtSV);
                    m_track_flip_z0_wrtSV->Fill (tr2_z0_wrtSV);
                    m_track_flip_phi_wrtSV->Fill(tr2_phi_wrtSV);
                    m_track_flip_theta_wrtSV->Fill(tr2_theta_wrtSV);
                    m_track_flip_p_wrtSV->Fill(tr2_p_wrtSV);
                    m_track_flip_pt_wrtSV->Fill(tr2_pt_wrtSV);
                    m_track_flip_d0sigma_wrtSV->Fill(tr2_d0sigma_wrtSV);
                    m_track_flip_z0sigma_wrtSV->Fill(tr2_z0sigma_wrtSV);
                    m_track_flip_d0_over_d0sigma_wrtSV->Fill(tr2_d0_over_d0sigma_wrtSV);
                    m_track_flip_z0_over_z0sigma_wrtSV->Fill(tr2_z0_over_z0sigma_wrtSV);

                    // fill track parameters wrt beam pipe
                    double tr1_p = 1.0 / fabs(tr1.qOverP()) / 1000.;
                    double tr1_pt = tr1_p * std::sin(tr1.theta());
                    double tr2_p = 1.0 / fabs(tr2.qOverP()) / 1000.;
                    double tr2_pt = tr2_p * std::sin(tr2.theta());

                    m_track_flip_d0->Fill(tr1.d0());
                    m_track_flip_z0->Fill(tr1.z0());
                    m_track_flip_phi->Fill(tr1.phi());
                    m_track_flip_theta->Fill(tr1.theta());
                    m_track_flip_p->Fill(tr1_p);
                    m_track_flip_pt->Fill(tr1_pt);
                    m_track_flip_d0sigma->Fill((tr1).definingParametersCovMatrixVec()[0]);
                    m_track_flip_z0sigma->Fill((tr1).definingParametersCovMatrixVec()[1]);
                    m_track_flip_d0_over_d0sigma->Fill((tr1).definingParameters()[0] / (tr1).definingParametersCovMatrixVec()[0]);
                    m_track_flip_z0_over_z0sigma->Fill((tr1).definingParameters()[1] / (tr1).definingParametersCovMatrixVec()[1]);

                    m_track_flip_d0->Fill(tr2.d0());
                    m_track_flip_z0->Fill(tr2.z0());
                    m_track_flip_phi->Fill(tr2.phi());
                    m_track_flip_theta->Fill(tr2.theta());
                    m_track_flip_p->Fill(tr2_p);
                    m_track_flip_pt->Fill(tr2_pt);
                    m_track_flip_d0sigma->Fill((tr2).definingParametersCovMatrixVec()[0]);
                    m_track_flip_z0sigma->Fill((tr2).definingParametersCovMatrixVec()[1]);
                    m_track_flip_d0_over_d0sigma->Fill((tr2).definingParameters()[0] / (tr2).definingParametersCovMatrixVec()[0]);
                    m_track_flip_z0_over_z0sigma->Fill((tr2).definingParameters()[1] / (tr2).definingParametersCovMatrixVec()[1]);


                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_idid_cf_flip->Fill("Truth matched", 1);
                    }
                }
    
            }

            if(channel == "mut") {

                // cut flow flag
                bool pass_vertex = true;
            
                if (pass_vertex) m_mut_cf_flip->Fill("#mux",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_flip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_flip->Fill("#mu^{+,-}x^{-,+}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_flip->Fill("DisabledModule", 1);

                // material veto
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_mut_cf_flip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_mut_cf_flip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_mut_cf_flip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_mut_cf_flip->Fill("z_{DV} > 300 mm", 1);

                // vertex distribution fill
                if (pass_vertex) {
                    m_mut_flip_R->Fill(dv_R);
                    m_mut_flip_z->Fill(dv_z);
                    m_mut_flip_l->Fill(dv_l);
                    m_mut_flip_chi2_ndof->Fill((*fit).chiSquared() / (*fit).numberDoF());
                    m_mut_flip_Rcos->Fill(dv_Rcos);

                    // mass plot
                    m_mut_flip_M->Fill(dv_mass);

                    float deltaR = tlv_tp0.DeltaR(tlv_tp1);
                    m_mut_flip_deltaR->Fill(deltaR);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_mut_cf_flip->Fill("Truth matched", 1);
                    }
                }
    
            }

            if(channel == "et") {

                // cut flow flag
                bool pass_vertex = true;
            
                if (pass_vertex) m_et_cf_flip->Fill("ex",1);

                // vertex fit quality
                if(!m_dvc->PassChisqCut(*fit)) pass_vertex = false;
                if (pass_vertex) m_et_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

                // minimum distance from pv (from 0 for MC)
                if(!m_dvc->PassDistCut(*fit, *pvc)) pass_vertex = false;
                if (pass_vertex) m_et_cf_flip->Fill("Disp. > 2 mm", 1);

                // charge requirements
                if(!m_dvc->PassChargeRequirement(*fit)) pass_vertex = false;
                if (pass_vertex) m_et_cf_flip->Fill("e^{+,-}x^{-,+}", 1);

                // disabled module
                if(!m_dvc->PassDisabledModuleVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_et_cf_flip->Fill("DisabledModule", 1);

                // material veto
                if(!m_dvc->PassMaterialVeto(*fit)) pass_vertex = false;
                if (pass_vertex) m_et_cf_flip->Fill("MaterialVeto (excl. mu)", 1);

                // low mass veto
                if(dv_mass < mass_min) pass_vertex = false;
                if (pass_vertex) m_et_cf_flip->Fill("LowMassVeto", 1);

                // DV R <  300 mm
                if(dv_R > dv_R_max) pass_vertex = false;
                if (pass_vertex) m_et_cf_flip->Fill("R_{DV} > 300 mm", 1);

                // DV z <  300 mm
                if(dv_z_abs > dv_z_max) pass_vertex = false;
                if (pass_vertex) m_et_cf_flip->Fill("z_{DV} > 300 mm", 1);


                // vertex distribution fill
                if (pass_vertex) {
                    m_et_flip_R->Fill(dv_R);
                    m_et_flip_z->Fill(dv_z);
                    m_et_flip_l->Fill(dv_l);
                    m_et_flip_chi2_ndof->Fill((*fit).chiSquared() / (*fit).numberDoF());
                    m_et_flip_Rcos->Fill(dv_Rcos);

                    // mass plot
                    m_et_flip_M->Fill(dv_mass);

                    float deltaR = tlv_tp0.DeltaR(tlv_tp1);
                    m_et_flip_deltaR->Fill(deltaR);

                    // truth match
                    if (isMC){
                        tru_matched = getClosestTruthVertex(fit);
                        if(tru_matched) m_et_cf_flip->Fill("Truth matched", 1);
                    }
                }
    
            }
        }
        else n_refit_flip_failed++;
    }

    return;
}


// cosmic veto cut
bool FlipBkgEst::PassCosmicVeto_R_CR(xAOD::TrackParticle& tr0, xAOD::TrackParticle& tr1){

    bool PassCosmicVeto = true;
    float Rcos_min = 0.01;

    // define TLorentzVector of decay particles
    TLorentzVector tlv_tp0;
    TLorentzVector tlv_tp1;
    tlv_tp0 = tr0.p4();
    tlv_tp1 = tr1.p4();

    float deltaPhiMinusPi = std::fabs(std::fabs(tlv_tp0.DeltaPhi(tlv_tp1)) - std::acos(-1.));
    float sumEta = tlv_tp0.Eta() + tlv_tp1.Eta();
    float Rcos = std::sqrt(sumEta * sumEta + deltaPhiMinusPi * deltaPhiMinusPi);

    if (Rcos < Rcos_min) PassCosmicVeto = false;

    return PassCosmicVeto;
}

// find closest truth vertex for truth matching
const xAOD::TruthVertex* FlipBkgEst::getClosestTruthVertex(const xAOD::Vertex *rv){

    double maxDistance = 0.7;

    double minDistance = std::numeric_limits<double>::max();
    const xAOD::TruthVertex *tvClosest{0};
    
    const xAOD::TruthVertexContainer *tvc{0};
    evtStore()->retrieve(tvc, "TruthVertices" );
    
    for (const auto tv : *tvc) {

        double distTvPv =
        sqrt(
        pow(tv->x() - rv->x(), 2) +
        pow(tv->y() - rv->y(), 2) +
        pow(tv->z() - rv->z(), 2));

        if (distTvPv < minDistance) {
            minDistance = distTvPv;
            tvClosest = tv;
        }
    }

    // return truth vertex only if distance < 1. mm
    if (minDistance < maxDistance) {
        ATH_MSG_DEBUG("truth vertex distance from reco vertex (closest) = " << minDistance );
        return tvClosest;
    }
    else return nullptr;
}


// find vertex type given number of e and m
std::string FlipBkgEst::FindDecayChannel(int n_mu, int n_elc){

    // default decay channel
    std::string channel = "idid";

    // assign channel
    if (n_mu == 2) channel = "mumu";
    else if (n_elc == 2) channel = "ee";
    else if ((n_mu == 1) and (n_elc ==1)) channel = "emu";
    else if ((n_mu == 1) and (n_elc ==0)) channel = "mut";
    else if ((n_mu == 0) and (n_elc ==1)) channel = "et";

    return channel;
}

void FlipBkgEst::FillTrackPair(std::string channel){

    // fill track pair histogram
    if (channel == "mumu") {
        // count number of pairs
        m_mumu_cf_nonflip->Fill("#mu#mu pair",1);
        m_mumu_cf_flip->Fill("#mu#mu pair",1);
    }
    else if (channel =="ee") {
        // count number of pairs
        m_ee_cf_nonflip->Fill("ee pair",1);
        m_ee_cf_flip->Fill("ee pair",1);
    }
    else if (channel =="emu") {
        // count number of pairs
        m_emu_cf_nonflip->Fill("e#mu pair",1);
        m_emu_cf_flip->Fill("e#mu pair",1);
    }
    else if (channel =="mut") {
        // count number of pairs
        m_mut_cf_nonflip->Fill("#mux pair",1);
        m_mut_cf_flip->Fill("#mux pair",1);
    }
    else if (channel == "et") {
        // count number of pairs
        m_et_cf_nonflip->Fill("ex pair",1);
        m_et_cf_flip->Fill("ex pair",1);
    }
    else {
        // count number of pairs
        m_idid_cf_nonflip->Fill("xx pair",1);
        m_idid_cf_flip->Fill("xx pair",1);
    }

}

bool FlipBkgEst::RefitVertex(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2,xAOD::Vertex& vrt){


    // vectors of seed tracks
    std::vector<const xAOD::TrackParticle*> vectorTrk = {&tr1, &tr2};

    // dummy vec
    std::vector<const xAOD::NeutralParticle*> dummyNeutralList;

    // set approximate vertex using current position
    m_fitsvc->setApproximateVertex(vrt.x(), vrt.y(), vrt.z());

    // set default
    m_fitsvc->setDefault();

    // get fast approximation
    Amg::Vector3D IniVertex;
    StatusCode sc_fast = m_fitsvc->VKalVrtFitFast(vectorTrk, IniVertex);
    if(sc_fast.isFailure()) return false;

    // vector to represent vertex position before refit
    Amg::Vector3D OrigVertex(vrt.x(), vrt.y(), vrt.z());

    // core part of refitting
    if(  (IniVertex.x()-OrigVertex.x())*(IniVertex.x()-OrigVertex.x()) +
         (IniVertex.y()-OrigVertex.y())*(IniVertex.y()-OrigVertex.y()) +
         (IniVertex.z()-OrigVertex.z())*(IniVertex.z()-OrigVertex.z())  > 100. ) {
        // set original position as approximation 
        m_fitsvc->setApproximateVertex( OrigVertex.x(), OrigVertex.y(), OrigVertex.z() );
    }
    else {
        // set new position as approximation 
        m_fitsvc->setApproximateVertex( IniVertex.x(), IniVertex.y(), IniVertex.z() );
    }

    ATH_MSG_DEBUG("==============================================");
    ATH_MSG_DEBUG("OrigVertex: x = " << vrt.x() << ", y = " << vrt.y() << ", z = " << vrt.z());
    ATH_MSG_DEBUG("IniVertex: x = " << IniVertex.x() << ", y = " << IniVertex.y() << ", z = " << IniVertex.z());

    // perform fit of displaced vertex
    StatusCode sc = m_fitsvc->VKalVrtFit(vectorTrk, dummyNeutralList, m_dv_fit, m_Momentum, m_Charge, m_ErrorMatrix, m_Chi2PerTrk, m_TrkAtVrt, m_Chi2);
     
    if(sc.isFailure()){
        ATH_MSG_DEBUG("DEBUG: >>> RefitVertex: SC in RefitVertex returned failure ");
    }

    // clear fit result
    m_dv_fit = Amg::Vector3D(0, 0, 0);
    m_Momentum = TLorentzVector();
    m_Charge = 0;
    m_ErrorMatrix.clear();
    m_Chi2PerTrk.clear();
    m_TrkAtVrt.clear();
    m_Chi2 = 0;


    return sc;
}

