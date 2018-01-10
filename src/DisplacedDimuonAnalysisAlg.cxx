// DispDilepAthena includes
#include "DisplacedDimuonAnalysisAlg.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"

// xAOD
#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"

// tools
#include "PathResolver/PathResolver.h"

// for M_PI
#include "cmath"

// debug
#include <typeinfo>

DisplacedDimuonAnalysisAlg::DisplacedDimuonAnalysisAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
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
m_or("DDL::OverlapRemoval/OverlapRemoval"),
m_accMu("DDL_Muons"),
m_accEl("DDL_Electrons"),
m_trackToVertexTool("Reco::TrackToVertex"),
m_accMass("mass")
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
    declareProperty("DiLepCosmics", m_cos);
    declareProperty("OverlapRemoval", m_or);
    declareProperty ("TrackToVertexTool", m_trackToVertexTool);


}


DisplacedDimuonAnalysisAlg::~DisplacedDimuonAnalysisAlg() {}


StatusCode DisplacedDimuonAnalysisAlg::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    ServiceHandle<ITHistSvc> histSvc("THistSvc",name());

    // event cut flow
    m_event_cutflow = new TH1D( "m_event_cutflow", "Event cutflow", 4,0,4);
    CHECK( histSvc->regHist("/DV/event_cutflow", m_event_cutflow) );

    //--------------------------------------------------------
    // mumu
    //--------------------------------------------------------

    //Float_t m_dv_mumu_M_bins[] = {0,10,40,70,100,400,700,1000,2000};
    m_dv_mumu_cf = new TH1D( "m_dv_mumu_cf", "Reco dv mumu cutflow", 9,0,9);
    m_dv_mumu_M = new TH1F("dv_mumu_M","Dimuon DV mass in GeV", 1000,0,1000 );
    m_dv_mumu_R = new TH1F("dv_mumu_R","R of dimuon dv [mm]",50,0.,300.);
    m_dv_mumu_R_low = new TH1F("dv_mumu_R_low","R of dimuon dv [mm], low",50,0.,50.);
    m_dv_mumu_z = new TH1F("dv_mumu_z","z of dimuon dv [mm]",1000,-1000.,1000.);
    m_dv_mumu_l = new TH1F("dv_mumu_l","l of dimuon dv [mm]",100,0.,1000.);
    m_dv_mumu_R_M = new TH2F("dv_mumu_R_M","Dimuon DV position R vs M", 50,0,300,1000,0,1000);

    // muon kinematics distribution
    m_dv_mumu_mu_pt = new TH1F("dv_mumu_mu_pt","Signal muon pT",50,0.,1000.);
    m_dv_mumu_mu_pt_low = new TH1F("dv_mumu_mu_pt_low","Signal muon low pT",50,0.,100.);
    m_dv_mumu_mu_eta = new TH1F("dv_mumu_mu_eta","Signal muon eta",50,-3.0,3.0);
    m_dv_mumu_mu_phi = new TH1F("dv_mumu_mu_phi","Signal muon phi",50,-M_PI,M_PI);
    m_dv_mumu_mu_d0 = new TH1F("dv_mumu_mu_d0","Signal muon d0",50,-300,300);
    m_dv_mumu_mu_z0 = new TH1F("dv_mumu_mu_z0","Signal muon z0",50,-1000,1000);

    // cosmic veto
    m_dv_mumu_DeltaR = new TH1F("dv_mumu_DeltaR","Signal muon Delta R",100, 0., 5.);
    m_dv_mumu_DeltaR_low = new TH1F("dv_mumu_DeltaR_low","Signal muon Delta R low",60, 0., 1);
    m_dv_mumu_Rcos = new TH1F("dv_mumu_Rcos","Signal muon Rcos",50, 0., 5.);
    m_dv_mumu_Rcos_low = new TH1F("dv_mumu_Rcos_low","Signal muon Rcos low",1000, 0., 0.1);
    m_dv_mumu_DeltaR_Rcos = new TH2F("dv_mumu_DeltaR_Rcos","#DeltaR vs R_{CR}", 100,0,5,100,0,5);

    // only for MC
    m_dv_mumu_M_matched = new TH1F("dv_mumu_M_matched","matched dimuon DV mass in GeV",200,0.,2000.);
    m_dv_mumu_R_matched = new TH1F("dv_mumu_R_matched","R of matched dimuon dv [mm]",50,0.,300.);
    m_dv_mumu_R_M_matched = new TH2F("dv_mumu_R_M_matched","matched dimuon DV position R vs M", 50,0,300,100,0,1000);
    m_dv_mumu_chi2_ndof = new TH1F("dv_mumu_chi2_ndof", "chi^2 / ndof (mumu)", 100, 0, 10);
    m_dv_mumu_eta_matched = new TH1F("dv_mumu_eta_matched","eta of dimuon dv",40,-4.,4.);

    // registor for output
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/dv_mumu_cf", m_dv_mumu_cf) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/dv_mumu_M", m_dv_mumu_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/dv_mumu_R", m_dv_mumu_R) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/dv_mumu_R_low", m_dv_mumu_R_low) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/dv_mumu_z", m_dv_mumu_z) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/dv_mumu_l", m_dv_mumu_l) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/dv_mumu_R_M", m_dv_mumu_R_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/dv_mumu_chi2_ndof", m_dv_mumu_chi2_ndof) );

    // muon kinematics distribution
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/outgoing/dv_mumu_mu_pt", m_dv_mumu_mu_pt) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/outgoing/dv_mumu_mu_pt_low", m_dv_mumu_mu_pt_low) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/outgoing/dv_mumu_mu_eta", m_dv_mumu_mu_eta) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/outgoing/dv_mumu_mu_phi", m_dv_mumu_mu_phi) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/outgoing/dv_mumu_mu_d0", m_dv_mumu_mu_d0) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/outgoing/dv_mumu_mu_z0", m_dv_mumu_mu_z0) );

    // cosmic veto
    CHECK( histSvc->regHist("/DV/cosmicBkg/mumu/dv_mumu_DeltaR", m_dv_mumu_DeltaR) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/mumu/dv_mumu_DeltaR_low", m_dv_mumu_DeltaR_low) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/mumu/dv_mumu_Rcos", m_dv_mumu_Rcos) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/mumu/dv_mumu_Rcos_low", m_dv_mumu_Rcos_low) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/mumu/dv_mumu_DeltaR_Rcos", m_dv_mumu_DeltaR_Rcos) );

    // only for MC
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/truth-matched/reco_dv_mumu_M", m_dv_mumu_M_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/truth-matched/reco_dv_mumu_R", m_dv_mumu_R_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/truth-matched/reco_dv_mumu_R_M", m_dv_mumu_R_M_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mumu/truth-matched/truth_dv_zp_eta", m_dv_mumu_eta_matched) );

    //--------------------------------------------------------
    // ee
    //--------------------------------------------------------

    //Float_t m_dv_ee_M_bins[] = {0,10,40,70,100,400,700,1000,2000};
    m_dv_ee_cf = new TH1D( "m_dv_ee_cf", "Reco dv ee cutflow", 9,0,9);
    m_dv_ee_M = new TH1F("dv_ee_M","ee DV mass in GeV",1000, 0, 1000);
    m_dv_ee_R = new TH1F("dv_ee_R","R of ee dv [mm]",50,0.,300.);
    m_dv_ee_R_low = new TH1F("dv_ee_R_low","R of ee dv [mm], low",50,0.,50.);
    m_dv_ee_z = new TH1F("dv_ee_z","z of ee dv [mm]",1000,-1000.,1000.);
    m_dv_ee_l = new TH1F("dv_ee_l","l of ee dv [mm]",100,0.,1000.);
    m_dv_ee_R_M = new TH2F("dv_ee_R_M","ee DV position R vs M", 50,0,300,1000,0,1000);

    // muon kinematics distribution
    m_dv_ee_e_pt = new TH1F("dv_ee_e_pt","Signal electron pT",50,0.,1000.);
    m_dv_ee_e_pt_low = new TH1F("dv_ee_e_pt_low","Signal electron low pT",50,0.,100.);
    m_dv_ee_e_eta = new TH1F("dv_ee_e_eta","Signal electron eta",50,-3.0,3.0);
    m_dv_ee_e_phi = new TH1F("dv_ee_e_phi","Signal electron phi",50,-M_PI,M_PI);
    m_dv_ee_e_d0 = new TH1F("dv_ee_e_d0","Signal electron d0",50,-300,300);
    m_dv_ee_e_z0 = new TH1F("dv_ee_e_z0","Signal electron z0",50,-1000,1000);

    // cosmic veto
    m_dv_ee_DeltaR = new TH1F("dv_ee_DeltaR","Signal ee Delta R",100, 0., 5.);
    m_dv_ee_DeltaR_low = new TH1F("dv_ee_DeltaR_low","Signal ee Delta R low",100, 0., 1);
    m_dv_ee_Rcos = new TH1F("dv_ee_Rcos","Signal ee Rcos",50, 0., 5.);
    m_dv_ee_Rcos_low = new TH1F("dv_ee_Rcos_low","Signal ee Rcos low",25, 0., 0.1);

    // only for MC
    m_dv_ee_M_matched = new TH1F("dv_ee_M_matched","matched ee DV mass in GeV",200,0.,2000.);
    m_dv_ee_R_matched = new TH1F("dv_ee_R_matched","R of matched ee dv [mm]",50,0.,300.);
    m_dv_ee_R_M_matched = new TH2F("dv_ee_R_M_matched","matched ee DV position R vs M", 50,0,300,100,0,1000);
    m_dv_ee_eta_matched = new TH1F("dv_ee_eta_matched","eta of ee dv",100,-4.,4.);
    m_dv_ee_chi2_ndof = new TH1F("dv_ee_chi2_ndof", "chi^2 / ndof (ee)", 100, 0, 10);

    // registor for output
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/dv_ee_cf", m_dv_ee_cf) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/dv_ee_M", m_dv_ee_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/dv_ee_R", m_dv_ee_R) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/dv_ee_R_low", m_dv_ee_R_low) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/dv_ee_z", m_dv_ee_z) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/dv_ee_l", m_dv_ee_l) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/dv_ee_R_M", m_dv_ee_R_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/dv_ee_chi2_ndof", m_dv_ee_chi2_ndof) );

    // electron kinematics distribution
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/outgoing/dv_ee_e_pt", m_dv_ee_e_pt) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/outgoing/dv_ee_e_pt_low", m_dv_ee_e_pt_low) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/outgoing/dv_ee_e_eta", m_dv_ee_e_eta) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/outgoing/dv_ee_e_phi", m_dv_ee_e_phi) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/outgoing/dv_ee_e_d0", m_dv_ee_e_d0) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/outgoing/dv_ee_e_z0", m_dv_ee_e_z0) );

    // cosmic veto
    CHECK( histSvc->regHist("/DV/cosmicBkg/ee/dv_ee_DeltaR", m_dv_ee_DeltaR) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/ee/dv_ee_DeltaR_low", m_dv_ee_DeltaR_low) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/ee/dv_ee_Rcos", m_dv_ee_Rcos) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/ee/dv_ee_Rcos_low", m_dv_ee_Rcos_low) );

    // only for MC
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/truth-matched/reco_dv_ee_M", m_dv_ee_M_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/truth-matched/reco_dv_ee_R", m_dv_ee_R_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/truth-matched/reco_dv_ee_R_M", m_dv_ee_R_M_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_ee/truth-matched/truth_dv_zp_eta", m_dv_ee_eta_matched) );

    //--------------------------------------------------------
    // emu
    //--------------------------------------------------------

    //Float_t m_dv_emu_M_bins[] = {0,10,40,70,100,400,700,1000,2000};
    m_dv_emu_cf = new TH1D( "m_dv_emu_cf", "Reco dv emu cutflow", 9,0,9);
    m_dv_emu_M = new TH1F("dv_emu_M","emu DV mass in GeV",1000,0,1000);
    m_dv_emu_R = new TH1F("dv_emu_R","R of emu dv [mm]",50,0.,300.);
    m_dv_emu_R_low = new TH1F("dv_emu_R_low","R of emu dv [mm], low",50,0.,50.);
    m_dv_emu_z = new TH1F("dv_emu_z","z of emu dv [mm]",1000,-1000.,1000.);
    m_dv_emu_l = new TH1F("dv_emu_l","l of emu dv [mm]",100,0.,1000.);
    m_dv_emu_R_M = new TH2F("dv_emu_R_M","emu DV position R vs M", 50,0,300,1000,0,1000);

    // muon kinematics distribution
    m_dv_emu_e_pt = new TH1F("dv_emu_e_pt","Signal electron pT",50,0.,1000.);
    m_dv_emu_e_pt_low = new TH1F("dv_emu_e_pt_low","Signal electron low pT",50,0.,100.);
    m_dv_emu_e_eta = new TH1F("dv_emu_e_eta","Signal electron eta",50,-3.0,3.0);
    m_dv_emu_e_phi = new TH1F("dv_emu_e_phi","Signal electron phi",50,-M_PI,M_PI);
    m_dv_emu_e_d0 = new TH1F("dv_emu_e_d0","Signal electron d0",50,-300,300);
    m_dv_emu_e_z0 = new TH1F("dv_emu_e_z0","Signal electron z0",50,-1000,1000);

    m_dv_emu_mu_pt = new TH1F("dv_emu_mu_pt","Signal electron pT",50,0.,1000.);
    m_dv_emu_mu_pt_low = new TH1F("dv_emu_mu_pt_low","Signal electron low pT",50,0.,100.);
    m_dv_emu_mu_eta = new TH1F("dv_emu_mu_eta","Signal electron eta",50,-3.0,3.0);
    m_dv_emu_mu_phi = new TH1F("dv_emu_mu_phi","Signal electron phi",50,-M_PI,M_PI);
    m_dv_emu_mu_d0 = new TH1F("dv_emu_mu_d0","Signal electron d0",50,-300,300);
    m_dv_emu_mu_z0 = new TH1F("dv_emu_mu_z0","Signal electron z0",50,-1000,1000);

    // cosmic veto
    m_dv_emu_DeltaR = new TH1F("dv_emu_DeltaR","Signal emu Delta R",100, 0., 5.);
    m_dv_emu_DeltaR_low = new TH1F("dv_emu_DeltaR_low","Signal emu Delta R low",100, 0., 1);
    m_dv_emu_Rcos = new TH1F("dv_emu_Rcos","Signal emu Rcos",50, 0., 5.);
    m_dv_emu_Rcos_low = new TH1F("dv_emu_Rcos_low","Signal emu Rcos low",25, 0., 0.1);

    // only for MC
    m_dv_emu_M_matched = new TH1F("dv_emu_M_matched","matched emu DV mass in GeV",200,0.,2000.);
    m_dv_emu_R_matched = new TH1F("dv_emu_R_matched","R of matched emu dv [mm]",50,0.,300.);
    m_dv_emu_R_M_matched = new TH2F("dv_emu_R_M_matched","matched emu DV position R vs M", 50,0,300,100,0,1000);
    m_dv_emu_eta_matched = new TH1F("dv_emu_eta_matched","eta of emu dv",100,-4.,4.);
    m_dv_emu_chi2_ndof = new TH1F("dv_emu_chi2_ndof", "chi^2 / ndof (emu)", 100, 0, 10);

    // registor for output
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/dv_emu_cf", m_dv_emu_cf) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/dv_emu_M", m_dv_emu_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/dv_emu_R", m_dv_emu_R) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/dv_emu_R_low", m_dv_emu_R_low) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/dv_emu_z", m_dv_emu_z) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/dv_emu_l", m_dv_emu_l) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/dv_emu_R_M", m_dv_emu_R_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/dv_emu_chi2_ndof", m_dv_emu_chi2_ndof) );

    // electron kinematics distribution
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_e_pt", m_dv_emu_e_pt) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_e_pt_low", m_dv_emu_e_pt_low) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_e_eta", m_dv_emu_e_eta) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_e_phi", m_dv_emu_e_phi) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_e_d0", m_dv_emu_e_d0) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_e_z0", m_dv_emu_e_z0) );

    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_mu_pt", m_dv_emu_mu_pt) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_mu_pt_low", m_dv_emu_mu_pt_low) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_mu_eta", m_dv_emu_mu_eta) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_mu_phi", m_dv_emu_mu_phi) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_mu_d0", m_dv_emu_mu_d0) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/outgoing/dv_emu_mu_z0", m_dv_emu_mu_z0) );

    // cosmic veto
    CHECK( histSvc->regHist("/DV/cosmicBkg/emu/dv_emu_DeltaR", m_dv_emu_DeltaR) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/emu/dv_emu_DeltaR_low", m_dv_emu_DeltaR_low) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/emu/dv_emu_Rcos", m_dv_emu_Rcos) );
    CHECK( histSvc->regHist("/DV/cosmicBkg/emu/dv_emu_Rcos_low", m_dv_emu_Rcos_low) );

    // only for MC
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/truth-matched/reco_dv_emu_M", m_dv_emu_M_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/truth-matched/reco_dv_emu_R", m_dv_emu_R_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/truth-matched/reco_dv_emu_R_M", m_dv_emu_R_M_matched) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_emu/truth-matched/truth_dv_zp_eta", m_dv_emu_eta_matched) );


    //--------------------------------------------------------
    // e+track, mu+track
    //--------------------------------------------------------
    m_dv_mut_cf = new TH1D( "m_dv_mut_cf", "Reco dv mut cutflow", 8,0,8);
    m_dv_mut_M = new TH1F("dv_mut_M","DV mass in GeV", 1000, 0, 1000. );
    m_dv_mut_R = new TH1F("dv_mut_R","R [mm]", 300, 0, 300. );
    m_dv_mut_z = new TH1F("dv_mut_z","z [mm]", 1000, -1000., 1000.);
    m_dv_mut_deltaR = new TH1F("dv_mut_deltaR","deltaR", 20, 0., 2.);
    m_dv_mut_l = new TH1F("dv_mut_l","l [mm]", 20, 0., 1000.);
    m_dv_mut_chi2_ndof = new TH1F("dv_mut_chi2_ndof","chi^2 / ndof", 200,0.,50);

    m_dv_et_cf = new TH1D( "m_dv_et_cf", "Reco dv et cutflow", 8,0,8);
    m_dv_et_M = new TH1F("dv_et_M","DV mass in GeV", 1000, 0, 1000. );
    m_dv_et_R = new TH1F("dv_et_R","R [mm]", 300, 0, 300. );
    m_dv_et_z = new TH1F("dv_et_z","z [mm]", 1000, -1000., 1000.);
    m_dv_et_deltaR = new TH1F("dv_et_deltaR","deltaR", 20, 0., 2.);
    m_dv_et_l = new TH1F("dv_et_l","l [mm]", 20, 0., 1000.);
    m_dv_et_chi2_ndof = new TH1F("dv_et_chi2_ndof","chi^2 / ndof", 200,0.,50);

    CHECK( histSvc->regHist("/DV/main_analysis/dv_mut/dv_mut_cf", m_dv_mut_cf) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mut/dv_mut_M", m_dv_mut_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mut/dv_mut_R", m_dv_mut_R) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mut/dv_mut_z", m_dv_mut_z) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mut/dv_mut_deltaR", m_dv_mut_deltaR) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mut/dv_mut_l", m_dv_mut_l) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_mut/dv_mut_chi2_ndof", m_dv_mut_chi2_ndof) );

    CHECK( histSvc->regHist("/DV/main_analysis/dv_et/dv_et_cf", m_dv_et_cf) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_et/dv_et_M", m_dv_et_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_et/dv_et_R", m_dv_et_R) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_et/dv_et_z", m_dv_et_z) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_et/dv_et_deltaR", m_dv_et_deltaR) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_et/dv_et_l", m_dv_et_l) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_et/dv_et_chi2_ndof", m_dv_et_chi2_ndof) );


    //--------------------------------------------------------
    // idid vertex plots
    //--------------------------------------------------------
    m_dv_idid_cf = new TH1D( "m_dv_idid_cf", "Reco dv idid cutflow", 8,0,8);
    m_dv_idid_M = new TH1F("dv_idid_M","DV mass in GeV", 1000, 0, 1000. );
    m_dv_idid_R = new TH1F("dv_idid_R","R [mm]", 600, 0, 600. );
    m_dv_idid_z = new TH1F("dv_idid_z","z [mm]", 1000, -1000., 1000.);
    m_dv_idid_deltaR = new TH1F("dv_idid_deltaR","deltaR", 20, 0., 2.);
    m_dv_idid_l = new TH1F("dv_idid_l","l [mm]", 100, 0., 1000.);
    m_dv_idid_n_tracks = new TH1F( "dv_idid_n_tracks", "vertex vs track multiplicity", 500,0,500);
    m_dv_idid_chi2_ndof = new TH1F("dv_idid_chi2_ndof","chi^2 / ndof", 200,0.,50);

    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/dv_idid_cf", m_dv_idid_cf) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/dv_idid_M", m_dv_idid_M) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/dv_idid_R", m_dv_idid_R) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/dv_idid_z", m_dv_idid_z) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/dv_idid_deltaR", m_dv_idid_deltaR) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/dv_idid_l", m_dv_idid_l) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/dv_idid_n_tracks", m_dv_idid_n_tracks) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/dv_idid_chi2_ndof", m_dv_idid_chi2_ndof) );

    // track parameters of idid vertex

    m_track_p = new TH1F( "m_track_p", "track p", 10000,0,1000);
    m_track_pt = new TH1F( "m_track_pt", "track pt", 10000,0,1000);
    m_track_z0 = new TH1F( "m_track_z0", "track z0", 2000,-1000,1000);
    m_track_d0 = new TH1F( "m_track_d0", "track d0", 600,-300,300);
    m_track_phi = new TH1F( "m_track_phi", "track phi", 1000,-M_PI,M_PI);
    m_track_theta = new TH1F( "m_track_theta", "track theta", 200,0,M_PI);
    m_track_d0sigma = new TH1F( "m_track_d0sigma", "d0sigma", 500,0,50);
    m_track_z0sigma = new TH1F( "m_track_z0sigma", "z0sigma", 2000,-100,100);
    m_track_d0_over_d0sigma = new TH1F( "m_track_d0_d0sigma", "d0/d0sigma", 500,0,50);
    m_track_z0_over_z0sigma = new TH1F( "m_track_z0_z0sigma", "z0/z0sigma", 2000,-100,100);

    m_track_p_wrtSV = new TH1F( "m_track_p_wrtSV", "track p_wrtSV", 10000,0,1000);
    m_track_pt_wrtSV = new TH1F( "m_track_pt_wrtSV", "track pt_wrtSV", 10000,0,1000);
    m_track_z0_wrtSV = new TH1F( "m_track_z0_wrtSV", "track z0_wrtSV", 20000,-1000,1000);
    m_track_d0_wrtSV = new TH1F( "m_track_d0_wrtSV", "track d0_wrtSV", 6000,-300,300);
    m_track_phi_wrtSV = new TH1F( "m_track_phi_wrtSV", "track phi_wrtSV", 1000,-M_PI,M_PI);
    m_track_theta_wrtSV = new TH1F( "m_track_theta_wrtSV", "track theta_wrtSV", 200,0,M_PI);
    m_track_d0sigma_wrtSV = new TH1F( "m_track_d0sigma_wrtSV", "d0sigma_wrtSV", 500,0,50);
    m_track_z0sigma_wrtSV = new TH1F( "m_track_z0sigma_wrtSV", "z0sigma_wrtSV", 2000,-100,100);
    m_track_d0_over_d0sigma_wrtSV = new TH1F( "m_track_d0_d0sigma_wrtSV", "d0/d0sigma_wrtSV", 500,0,50);
    m_track_z0_over_z0sigma_wrtSV = new TH1F( "m_track_z0_z0sigma_wrtSV", "z0/z0sigma_wrtSV", 2000,-100,100);

    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_p", m_track_p) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_pt", m_track_pt) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_z0", m_track_z0) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_d0", m_track_d0) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_phi", m_track_phi) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_theta", m_track_theta) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_d0sigma", m_track_d0sigma) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_z0sigma", m_track_z0sigma) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_d0_over_d0sigma", m_track_d0_over_d0sigma) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtBeam/track_z0_over_z0sigma", m_track_z0_over_z0sigma) );

    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_p_wrtSV", m_track_p_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_pt_wrtSV", m_track_pt_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_z0_wrtSV", m_track_z0_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_d0_wrtSV", m_track_d0_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_phi_wrtSV", m_track_phi_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_theta_wrtSV", m_track_theta_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_d0sigma_wrtSV", m_track_d0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_z0sigma_wrtSV", m_track_z0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_d0_over_d0sigma_wrtSV", m_track_d0_over_d0sigma_wrtSV) );
    CHECK( histSvc->regHist("/DV/main_analysis/dv_idid/track_wrtSV/track_z0_over_z0sigma_wrtSV", m_track_z0_over_z0sigma_wrtSV) );

    // counting selected id tracks
    int n_trk_sel = 0;

    return StatusCode::SUCCESS;
}

StatusCode DisplacedDimuonAnalysisAlg::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    ATH_MSG_INFO("n_event_all = " << n_event_all);
    ATH_MSG_INFO("n_vrtsec_all = " << n_vrtsec_all);
    ATH_MSG_INFO("n_dvc_copy = " << n_dvc_copy);
    ATH_MSG_INFO("n_dv_all = " << n_dv_all);
    ATH_MSG_INFO("n_dv_passed_cut = " << n_dv_passed_cut);
    return StatusCode::SUCCESS;
}

StatusCode DisplacedDimuonAnalysisAlg::execute() {
    ATH_MSG_DEBUG ("Executing " << name() << "...");

    ATH_MSG_DEBUG("n_event_all = " << n_event_all
                 << ", n_vrtsec_all = " << n_vrtsec_all
                 << ", n_dvc_copy = " << n_dvc_copy
                 << ", n_dv_all = " << n_dv_all);


    // retrieve event info
    const xAOD::EventInfo* evtInfo = nullptr;
    CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

    // flag to check if data or MC
    bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

    //bool trig_passed = true;
    bool trig_passed = false;

    m_event_cutflow->Fill("AllEvents", 1);

    // GRL
    if (!isMC and !m_grlTool->passRunLB(*evtInfo)) return StatusCode::SUCCESS;
    m_event_cutflow->Fill("GRL (Data)", 1);

    // event cleaning
    if(!m_evtc->PassEventCleaning(*evtInfo)) return StatusCode::SUCCESS;
    m_event_cutflow->Fill("EvtCleaning (Data)", 1);

    if (m_tdt->isPassed("HLT_mu60_0eta105_msonly")) trig_passed = true;
    if (m_tdt->isPassed("HLT_g140_loose")) trig_passed = true;
    if (m_tdt->isPassed("HLT_2g50_loose")) trig_passed = true;
    //if (m_tdt->isPassed("HLT_2g60_loose_L12EM15VH")) trig_passed = true;

    // trigger check
    if(!trig_passed) return StatusCode::SUCCESS;
    m_event_cutflow->Fill("Trig", 1);

    // retrieve lepton container
    const xAOD::MuonContainer* muc = nullptr;
    CHECK( evtStore()->retrieve( muc, "Muons" ));

    const xAOD::ElectronContainer* elc = nullptr;
    CHECK( evtStore()->retrieve( elc, "Electrons" ));

    const xAOD::TrackParticleContainer* idc = nullptr;
    CHECK( evtStore()->retrieve( idc, "InDetTrackParticles" ));

    // retrieve secondary vertices
    const xAOD::VertexContainer* dvc = nullptr;
    CHECK( evtStore()->retrieve( dvc, "VrtSecInclusive_SecondaryVertices" ));
    n_vrtsec_all = n_vrtsec_all + dvc->size();

    // make copies of leptons
    auto muc_copy = xAOD::shallowCopyContainer(*muc);
    xAOD::setOriginalObjectLink(*muc, *muc_copy.first);

    auto elc_copy = xAOD::shallowCopyContainer(*elc);
    xAOD::setOriginalObjectLink(*elc, *elc_copy.first);

    // apply overlap removal
    m_or->FindOverlap(*elc_copy.first, *muc_copy.first);

    // retrieve primary vertices
    const xAOD::VertexContainer* pvc = nullptr;
    CHECK( evtStore()->retrieve( pvc, "PrimaryVertices" ));

    // get primary vertex
    auto pv = m_evtc->GetPV(*pvc);

    // make a copy of vertex containers
    auto dvc_copy = xAOD::shallowCopyContainer(*dvc);

    // count all event before looop
    n_event_all++;
    n_dvc_copy = n_dvc_copy + (*dvc_copy.first).size();

    //------------------------------
    // dv cut flow
    //------------------------------
    for(auto dv: *dvc_copy.first) {

        // mass cut
        float mass_min = 10.;

        // counting all dv
        n_dv_all++;

        // perform lepton matching
        m_dilepdvc->ApplyLeptonMatching(*dv, *elc_copy.first, *muc_copy.first);

        // access invariant mass
        float dv_mass = std::fabs(m_accMass(*dv)) / 1000.; // in MeV

        // collect leptons from this dv
        auto dv_muc = m_accMu(*dv);
        auto dv_elc = m_accEl(*dv);

        // access position of vertices
        auto dv_pos = dv->position();
        //auto pv_pos = m_evtc->GetPV(*pvc)->position();

        // access tracks from vertex
        auto tpLinks = dv->trackParticleLinks();

        xAOD::TrackParticle tp1 = **(tpLinks.at(0));
        xAOD::TrackParticle tp2 = **(tpLinks.at(1));

        // remove overlapping muon
        m_dilepdvc->ApplyOverlapRemoval(*dv);

        // remove bad electrons
        m_leptool->BadClusterRemoval(*dv);

        // kinematic cut
        m_leptool->ElectronKinematicCut(*dv);

        // Electron identification
        m_leptool->ElectronID(*dv);

        // muon selection tool
        m_leptool->MuonSelection(*dv);

        // select only vertex with tracks
        if(dv->trackParticleLinks().size() != 2) continue;

        // find decay channel of dv
        std::string channel = m_dvutils->DecayChannel(*dv);

        // fill 2 tkr vertex
        m_dv_mumu_cf->Fill("vertex", 1);
        m_dv_ee_cf->Fill("vertex", 1);
        m_dv_emu_cf->Fill("vertex", 1);
        m_dv_mut_cf->Fill("vertex", 1);
        m_dv_et_cf->Fill("vertex", 1);
        m_dv_idid_cf->Fill("vertex", 1);

        if (channel == "mumu") {

            // mumu pair
            m_dv_mumu_cf->Fill("#mu#mu", 1);

            // Trigger matching
            if(!m_dvutils->TrigMatching(*dv)) continue;
            m_dv_mumu_cf->Fill("Trig. Matching", 1);

            // vertex fit quality
            m_dv_mumu_chi2_ndof->Fill (dv->chiSquared() / dv->numberDoF() );
            if(!m_dvc->PassChisqCut(*dv)) continue;
            m_dv_mumu_cf->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*dv, *pvc)) continue;
            m_dv_mumu_cf->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*dv)) continue;
            m_dv_mumu_cf->Fill("#mu^{+}#mu^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*dv)) continue;
            m_dv_mumu_cf->Fill("DisabledModule", 1);

            // material veto (only e)
            //m_dv_mumu_cf->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) continue;
            m_dv_mumu_cf->Fill("LowMassVeto", 1);


            // cosmic veto
            if(!PassCosmicVeto(*dv_muc, *dv_elc, channel)) continue;
            m_dv_mumu_cf->Fill("R_{CR} > 0.04", 1);

            // end of cut flow. Now plotting
            ATH_MSG_INFO("Found signal mumu with mass = " << dv_mass << ", runNumber = "
            << evtInfo->runNumber() << ", eventNumber = "
            << evtInfo->eventNumber() << ", Lumiblock = " << evtInfo->lumiBlock() );

            // plot dv distributions
            plot_dv(*dv, *pv, channel);

            // plot muon kinematics
            plot_signal_tp(*dv_muc, *dv_elc, channel);

            // truth match
            if (isMC){
                // create truth vertex for matching
                const xAOD::TruthVertex *tru_matched = nullptr;

                tru_matched = getClosestTruthVertex(dv);
                if(tru_matched) m_dv_mumu_cf->Fill("Truth matched", 1);
            }
        } // end of mumu

        if (channel == "ee") {

            // ee pair
            m_dv_ee_cf->Fill("ee", 1);

            // Trigger matching
            if(!m_dvutils->TrigMatching(*dv)) continue;
            m_dv_ee_cf->Fill("Trig. Matching", 1);

            // vertex fit quality
            m_dv_ee_chi2_ndof->Fill (dv->chiSquared() / dv->numberDoF() );
            if(!m_dvc->PassChisqCut(*dv)) continue;
            m_dv_ee_cf->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*dv, *pvc)) continue;
            m_dv_ee_cf->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*dv)) continue;
            m_dv_ee_cf->Fill("e^{+}e^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*dv)) continue;
            m_dv_ee_cf->Fill("DisabledModule", 1);

            // material veto
            //if(!m_dvc->PassMaterialVeto(*dv)) continue;
            //m_dv_ee_cf->Fill("MaterialVeto", 1);

            // low mass veto
            if(dv_mass < mass_min) continue;
            m_dv_ee_cf->Fill("LowMassVeto", 1);


            // cosmic veto
            if(!PassCosmicVeto(*dv_muc, *dv_elc, channel)) continue;
            m_dv_ee_cf->Fill("R_{CR} > 0.04", 1);

            // plot dv distributions
            plot_dv(*dv, *pv, channel);

            // plot muon kinematics
            plot_signal_tp(*dv_muc, *dv_elc, channel);

            // truth match
            if (isMC){
                // create truth vertex for matching
                const xAOD::TruthVertex *tru_matched = nullptr;

                tru_matched = getClosestTruthVertex(dv);
                if(tru_matched) m_dv_ee_cf->Fill("Truth matched", 1);
            }

        } // end of ee

        if (channel == "emu") {

            // emu pair
            m_dv_emu_cf->Fill("e#mu", 1);

            // Trigger matching
            if(!m_dvutils->TrigMatching(*dv)) continue;
            m_dv_emu_cf->Fill("Trig. Matching", 1);

            // vertex fit quality
            m_dv_emu_chi2_ndof->Fill (dv->chiSquared() / dv->numberDoF() );
            if(!m_dvc->PassChisqCut(*dv)) continue;
            m_dv_emu_cf->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*dv, *pvc)) continue;
            m_dv_emu_cf->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*dv)) continue;
            m_dv_emu_cf->Fill("e^{+}#mu^{-}, e^{-}#mu^{+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*dv)) continue;
            m_dv_emu_cf->Fill("DisabledModule", 1);

            // material veto
            //if(!m_dvc->PassMaterialVeto(*dv)) continue;
            //m_dv_emu_cf->Fill("MaterialVeto", 1);

            // low mass veto
            if(dv_mass < mass_min) continue;
            m_dv_emu_cf->Fill("LowMassVeto", 1);


            // cosmic veto
            if(!PassCosmicVeto(*dv_muc, *dv_elc, channel)) continue;
            m_dv_emu_cf->Fill("R_{CR} > 0.04", 1);

            // plot dv distributions
            plot_dv(*dv, *pv, channel);

            // plot muon kinematics
            plot_signal_tp(*dv_muc, *dv_elc, channel);

            // truth match
            if (isMC){
                // create truth vertex for matching
                const xAOD::TruthVertex *tru_matched = nullptr;

                tru_matched = getClosestTruthVertex(dv);
                if(tru_matched) m_dv_emu_cf->Fill("Truth matched", 1);
            }

        } // end of emu

        if (channel == "mut") {

            // access tracks from vertex
            auto tpLinks = dv->trackParticleLinks();

            xAOD::TrackParticle tp1 = **(tpLinks.at(0));
            xAOD::TrackParticle tp2 = **(tpLinks.at(1));

            // mut pair
            m_dv_mut_cf->Fill("mux", 1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*dv)) continue;
            m_dv_mut_cf->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*dv, *pvc)) continue;
            m_dv_mut_cf->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*dv)) continue;
            m_dv_mut_cf->Fill("#mu^{+,-}x^{-,+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*dv)) continue;
            m_dv_mut_cf->Fill("DisabledModule", 1);

            // material veto (only e)
            //if(!m_dvc->PassMaterialVeto(*dv)) continue;
            //m_dv_mut_cf->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) continue;
            m_dv_mut_cf->Fill("LowMassVeto", 1);
            
            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tp1, tp2)) continue;
            m_dv_mut_cf->Fill("R_{CR} > 0.04", 1);

            // plot dv
            plot_dv(*dv, *pv, channel);
            m_dv_mut_chi2_ndof->Fill (dv->chiSquared() / dv->numberDoF() );

            // deltaR plot
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;

            // define TLorentzVector of decay particles
            tlv_tp0 = tp1.p4();
            tlv_tp1 = tp2.p4();

            float deltaR = tlv_tp0.DeltaR(tlv_tp1);
            m_dv_mut_deltaR->Fill(deltaR);



            // truth match
            if (isMC){
                // create truth vertex for matching
                const xAOD::TruthVertex *tru_matched = nullptr;

                tru_matched = getClosestTruthVertex(dv);
                if(tru_matched) m_dv_mut_cf->Fill("Truth matched", 1);
            }

        }

        if (channel == "et") {

            // access tracks from vertex
            auto tpLinks = dv->trackParticleLinks();

            xAOD::TrackParticle tp1 = **(tpLinks.at(0));
            xAOD::TrackParticle tp2 = **(tpLinks.at(1));

            // et pair
            m_dv_et_cf->Fill("mux", 1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*dv)) continue;
            m_dv_et_cf->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*dv, *pvc)) continue;
            m_dv_et_cf->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*dv)) continue;
            m_dv_et_cf->Fill("#mu^{+,-}x^{-,+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*dv)) continue;
            m_dv_et_cf->Fill("DisabledModule", 1);

            // material veto (only e)
            //if(!m_dvc->PassMaterialVeto(*dv)) continue;
            //m_dv_et_cf->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) continue;
            m_dv_et_cf->Fill("LowMassVeto", 1);
            
            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tp1, tp2)) continue;
            m_dv_et_cf->Fill("R_{CR} > 0.04", 1);

            // plot dv
            plot_dv(*dv, *pv, channel);
            m_dv_et_chi2_ndof->Fill (dv->chiSquared() / dv->numberDoF() );

            // deltaR plot
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;

            // define TLorentzVector of decay particles
            tlv_tp0 = tp1.p4();
            tlv_tp1 = tp2.p4();

            float deltaR = tlv_tp0.DeltaR(tlv_tp1);
            m_dv_et_deltaR->Fill(deltaR);


            // truth match
            if (isMC){
                // create truth vertex for matching
                const xAOD::TruthVertex *tru_matched = nullptr;

                tru_matched = getClosestTruthVertex(dv);
                if(tru_matched) m_dv_et_cf->Fill("Truth matched", 1);
            }

        }

        if (channel == "trktrk") {

            // access tracks from vertex
            auto tpLinks = dv->trackParticleLinks();

            xAOD::TrackParticle tp1 = **(tpLinks.at(0));
            xAOD::TrackParticle tp2 = **(tpLinks.at(1));

            // idid pair
            m_dv_idid_cf->Fill("xx", 1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*dv)) continue;
            m_dv_idid_cf->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*dv, *pvc)) continue;
            m_dv_idid_cf->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*dv)) continue;
            m_dv_idid_cf->Fill("x^{+}x^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*dv)) continue;
            m_dv_idid_cf->Fill("DisabledModule", 1);

            // material veto (only e)
            //if(!m_dvc->PassMaterialVeto(*dv)) continue;
            //m_dv_idid_cf->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) continue;
            m_dv_idid_cf->Fill("LowMassVeto", 1);
            
            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tp1, tp2)) continue;
            m_dv_idid_cf->Fill("R_{CR} > 0.04", 1);

            //==========================================
            // plot xx distributions
            //==========================================
            plot_dv(*dv, *pv, channel);
            m_dv_idid_chi2_ndof->Fill (dv->chiSquared() / dv->numberDoF() );

            // track parameters
            const Trk::Perigee* sv_perigee1 = m_trackToVertexTool->perigeeAtVertex(tp1, dv_pos);
            const Trk::Perigee* sv_perigee2 = m_trackToVertexTool->perigeeAtVertex(tp2, dv_pos);

            double tp1_d0_wrtSV        = sv_perigee1->parameters() [Trk::d0];
            double tp1_z0_wrtSV        = sv_perigee1->parameters() [Trk::z0];
            double tp1_theta_wrtSV     = sv_perigee1->parameters() [Trk::theta];
            double tp1_phi_wrtSV       = sv_perigee1->parameters() [Trk::phi];
            double tp1_qOverP_wrtSV    = sv_perigee1->parameters() [Trk::qOverP];
            double tp1_p_wrtSV         = 1.0 / fabs(tp1_qOverP_wrtSV) / 1000.;
            double tp1_pt_wrtSV        = tp1_p_wrtSV * std::sin( tp1_theta_wrtSV );
            double tp1_d0sigma_wrtSV   = (*sv_perigee1->covariance())( Trk::d0 );
            double tp1_z0sigma_wrtSV   = (*sv_perigee1->covariance())( Trk::z0 );
            double tp1_d0_over_d0sigma_wrtSV   = tp1_d0_wrtSV / tp1_d0sigma_wrtSV;
            double tp1_z0_over_z0sigma_wrtSV   = tp1_z0_wrtSV / tp1_z0sigma_wrtSV;

            double tp2_d0_wrtSV        = sv_perigee2->parameters() [Trk::d0];
            double tp2_z0_wrtSV        = sv_perigee2->parameters() [Trk::z0];
            double tp2_theta_wrtSV     = sv_perigee2->parameters() [Trk::theta];
            double tp2_phi_wrtSV       = sv_perigee2->parameters() [Trk::phi];
            double tp2_qOverP_wrtSV    = sv_perigee2->parameters() [Trk::qOverP];
            double tp2_p_wrtSV         = 1.0 / fabs(tp2_qOverP_wrtSV) / 1000.;
            double tp2_pt_wrtSV        = tp2_p_wrtSV * std::sin( tp2_theta_wrtSV );
            double tp2_d0sigma_wrtSV   = (*sv_perigee1->covariance())( Trk::d0 );
            double tp2_z0sigma_wrtSV   = (*sv_perigee1->covariance())( Trk::z0 );
            double tp2_d0_over_d0sigma_wrtSV   = tp2_d0_wrtSV / tp2_d0sigma_wrtSV;
            double tp2_z0_over_z0sigma_wrtSV   = tp2_z0_wrtSV / tp2_z0sigma_wrtSV;

            // fill track parameters wrt SV
            m_track_d0_wrtSV->Fill (tp1_d0_wrtSV);
            m_track_z0_wrtSV->Fill (tp1_z0_wrtSV);
            m_track_phi_wrtSV->Fill(tp1_phi_wrtSV);
            m_track_theta_wrtSV->Fill(tp1_theta_wrtSV);
            m_track_p_wrtSV->Fill(tp1_p_wrtSV);
            m_track_pt_wrtSV->Fill(tp1_pt_wrtSV);
            m_track_d0sigma_wrtSV->Fill(tp1_d0sigma_wrtSV);
            m_track_z0sigma_wrtSV->Fill(tp1_z0sigma_wrtSV);
            m_track_d0_over_d0sigma_wrtSV->Fill(tp1_d0_over_d0sigma_wrtSV);
            m_track_z0_over_z0sigma_wrtSV->Fill(tp1_z0_over_z0sigma_wrtSV);

            m_track_d0_wrtSV->Fill (tp2_d0_wrtSV);
            m_track_z0_wrtSV->Fill (tp2_z0_wrtSV);
            m_track_phi_wrtSV->Fill(tp2_phi_wrtSV);
            m_track_theta_wrtSV->Fill(tp2_theta_wrtSV);
            m_track_p_wrtSV->Fill(tp2_p_wrtSV);
            m_track_pt_wrtSV->Fill(tp2_pt_wrtSV);
            m_track_d0sigma_wrtSV->Fill(tp2_d0sigma_wrtSV);
            m_track_z0sigma_wrtSV->Fill(tp2_z0sigma_wrtSV);
            m_track_d0_over_d0sigma_wrtSV->Fill(tp2_d0_over_d0sigma_wrtSV);
            m_track_z0_over_z0sigma_wrtSV->Fill(tp2_z0_over_z0sigma_wrtSV);

            // fill track parameters wrt beam pipe
            double tp1_p = 1.0 / fabs(tp1.qOverP()) / 1000.;
            double tp1_pt = tp1_p * std::sin(tp1.theta());
            double tp2_p = 1.0 / fabs(tp2.qOverP()) / 1000.;
            double tp2_pt = tp2_p * std::sin(tp2.theta());
            m_track_d0->Fill(tp1.d0());
            m_track_z0->Fill(tp1.z0());
            m_track_phi->Fill(tp1.phi());
            m_track_theta->Fill(tp1.theta());
            m_track_p->Fill(tp1_p);
            m_track_pt->Fill(tp1_pt);
            m_track_d0sigma->Fill((tp1).definingParametersCovMatrixVec()[0]);
            m_track_z0sigma->Fill((tp1).definingParametersCovMatrixVec()[1]);
            m_track_d0_over_d0sigma->Fill((tp1).definingParameters()[0] / (tp1).definingParametersCovMatrixVec()[0]);
            m_track_z0_over_z0sigma->Fill((tp1).definingParameters()[1] / (tp1).definingParametersCovMatrixVec()[1]);

            m_track_d0->Fill(tp2.d0());
            m_track_z0->Fill(tp2.z0());
            m_track_phi->Fill(tp2.phi());
            m_track_theta->Fill(tp2.theta());
            m_track_p->Fill(tp2_p);
            m_track_pt->Fill(tp2_pt);
            m_track_d0sigma->Fill((tp2).definingParametersCovMatrixVec()[0]);
            m_track_z0sigma->Fill((tp2).definingParametersCovMatrixVec()[1]);
            m_track_d0_over_d0sigma->Fill((tp2).definingParameters()[0] / (tp2).definingParametersCovMatrixVec()[0]);
            m_track_z0_over_z0sigma->Fill((tp2).definingParameters()[1] / (tp2).definingParametersCovMatrixVec()[1]);

            // deltaR plot
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;

            // define TLorentzVector of decay particles
            tlv_tp0 = tp1.p4();
            tlv_tp1 = tp2.p4();

            float deltaR = tlv_tp0.DeltaR(tlv_tp1);
            m_dv_idid_deltaR->Fill(deltaR);

            n_dv_passed_cut++;

            // truth match
            if (isMC){
                // create truth vertex for matching
                const xAOD::TruthVertex *tru_matched = nullptr;

                tru_matched = getClosestTruthVertex(dv);
                if(tru_matched) m_dv_idid_cf->Fill("Truth matched", 1);
            }

        }

    } // end of dv loop

    return StatusCode::SUCCESS;
}

StatusCode DisplacedDimuonAnalysisAlg::beginInputFile() { 

  return StatusCode::SUCCESS;
}

bool DisplacedDimuonAnalysisAlg::PassCosmicVeto(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) {
    ATH_MSG_DEBUG ("Plotting cosmic veto" << name() << "...");

    bool PassCosmicVeto = true;

    float Rcos_min = 0.04;
    float deltaR_min = 0.5;
    float deltaPhiMinusPi = m_costool->getDeltaPhiMinusPi(dv_muc, dv_elc, channel);
    float sumEta = m_costool->getSumEta(dv_muc, dv_elc, channel);

    float Rcos = std::sqrt(sumEta * sumEta + deltaPhiMinusPi * deltaPhiMinusPi);
    float deltaR = m_costool->getDeltaR(dv_muc, dv_elc, channel);

    if (channel == "mumu"){
        // plot Rcos after applying deltaR < deltaR_min
        if (deltaR > deltaR_min){
            m_dv_mumu_Rcos->Fill(Rcos);
            m_dv_mumu_Rcos_low->Fill(Rcos);
        }

        // plot delta R after applying Rcos < Rcos_min
        if (Rcos > Rcos_min){
            m_dv_mumu_DeltaR->Fill(deltaR);
            m_dv_mumu_DeltaR_low->Fill(deltaR);
        }
        // plot delta R vs R_cr
        m_dv_mumu_DeltaR_Rcos->Fill(deltaR,Rcos);
    }

    if (channel == "ee"){
        // plot Rcos after applying deltaR < deltaR_min
        if (deltaR > deltaR_min){
            m_dv_ee_Rcos->Fill(Rcos);
            m_dv_ee_Rcos_low->Fill(Rcos);
        }

        // plot delta R after applying Rcos < Rcos_min
        if (Rcos > Rcos_min){
            m_dv_ee_DeltaR->Fill(deltaR);
            m_dv_ee_DeltaR_low->Fill(deltaR);
        }
    }

    if (channel == "emu"){
        // plot Rcos after applying deltaR < deltaR_min
        if (deltaR > deltaR_min){
            m_dv_emu_Rcos->Fill(Rcos);
            m_dv_emu_Rcos_low->Fill(Rcos);
        }

        // plot delta R after applying Rcos < Rcos_min
        if (Rcos > Rcos_min){
            m_dv_emu_DeltaR->Fill(deltaR);
            m_dv_emu_DeltaR_low->Fill(deltaR);
        }
    }

    if (Rcos < Rcos_min) PassCosmicVeto = false;

    return PassCosmicVeto;
}


void DisplacedDimuonAnalysisAlg::plot_signal_tp(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) {

    if (channel == "mumu"){
        // plot muon distribution
        for(auto mu: dv_muc){
            m_dv_mumu_mu_pt->Fill(mu->pt() / 1000.); // GeV
            m_dv_mumu_mu_pt_low->Fill(mu->pt() / 1000.); // GeV
            m_dv_mumu_mu_eta->Fill(mu->eta());
            m_dv_mumu_mu_phi->Fill(mu->phi());
            m_dv_mumu_mu_d0->Fill(mu->primaryTrackParticle()->d0());
            m_dv_mumu_mu_z0->Fill(mu->primaryTrackParticle()->z0());
        }
    }

    if (channel == "ee"){
        // plot muon distribution
        for(auto el: dv_elc){
            m_dv_ee_e_pt->Fill(el->pt() / 1000.); // GeV
            m_dv_ee_e_pt_low->Fill(el->pt() / 1000.); // GeV
            m_dv_ee_e_eta->Fill(el->eta());
            m_dv_ee_e_phi->Fill(el->phi());
            m_dv_ee_e_d0->Fill(el->trackParticle()->d0());
            m_dv_ee_e_z0->Fill(el->trackParticle()->z0());
        }
    }

    if (channel == "emu"){
        // plot muon distribution
        for(auto mu: dv_muc){
            m_dv_emu_mu_pt->Fill(mu->pt() / 1000.); // GeV
            m_dv_emu_mu_pt_low->Fill(mu->pt() / 1000.); // GeV
            m_dv_emu_mu_eta->Fill(mu->eta());
            m_dv_emu_mu_phi->Fill(mu->phi());
            m_dv_emu_mu_d0->Fill(mu->primaryTrackParticle()->d0());
            m_dv_emu_mu_z0->Fill(mu->primaryTrackParticle()->z0());
        }
        for(auto el: dv_elc){
            m_dv_emu_e_pt->Fill(el->pt() / 1000.); // GeV
            m_dv_emu_e_pt_low->Fill(el->pt() / 1000.); // GeV
            m_dv_emu_e_eta->Fill(el->eta());
            m_dv_emu_e_phi->Fill(el->phi());
            m_dv_emu_e_d0->Fill(el->trackParticle()->d0());
            m_dv_emu_e_z0->Fill(el->trackParticle()->z0());
        }
    }

    return;
}

void DisplacedDimuonAnalysisAlg::plot_dv(const xAOD::Vertex& dv, const xAOD::Vertex& pv, std::string channel) {

    // access invariant mass
    float dv_mass = std::fabs(m_accMass(dv)) / 1000.; // in MeV
    ATH_MSG_DEBUG("At fill, m = " << dv_mass);

    // fill dimuon vertex
    float dv_R = m_dvutils->getR( dv, pv );                 // R in [mm]
    float dv_z = m_dvutils->getz( dv, pv );                 // z in [mm]
    float dv_l = m_dvutils->getr( dv, pv );                 // r in [mm]

    if (channel == "mumu"){
        m_dv_mumu_M->Fill(dv_mass);                             // dimuon mass
        m_dv_mumu_R->Fill(dv_R);                                
        m_dv_mumu_R_low->Fill(dv_R);                                
        m_dv_mumu_z->Fill(dv_z);                                
        m_dv_mumu_l->Fill(dv_l);                                
        m_dv_mumu_R_M->Fill(dv_R, dv_mass);
    }

    if (channel == "ee"){
        m_dv_ee_M->Fill(dv_mass);                             // dimuon mass
        m_dv_ee_R->Fill(dv_R);                                
        m_dv_ee_R_low->Fill(dv_R);                                
        m_dv_ee_z->Fill(dv_z);                                
        m_dv_ee_l->Fill(dv_l);                                
        m_dv_ee_R_M->Fill(dv_R, dv_mass);
    }

    if (channel == "emu"){
        m_dv_emu_M->Fill(dv_mass);                             // dimuon mass
        m_dv_emu_R->Fill(dv_R);                                
        m_dv_emu_R_low->Fill(dv_R);                                
        m_dv_emu_z->Fill(dv_z);                                
        m_dv_emu_l->Fill(dv_l);                                
        m_dv_emu_R_M->Fill(dv_R, dv_mass);
    }

    if (channel == "trktrk"){
        m_dv_idid_M->Fill(dv_mass);                             // dimuon mass
        m_dv_idid_R->Fill(dv_R);                                
        m_dv_idid_z->Fill(dv_z);                                
        m_dv_idid_l->Fill(dv_l);                                
    }

    if (channel == "mut"){
        m_dv_mut_M->Fill(dv_mass);                             // dimuon mass
        m_dv_mut_R->Fill(dv_R);                                
        m_dv_mut_z->Fill(dv_z);                                
        m_dv_mut_l->Fill(dv_l);                                
    }

    if (channel == "et"){
        m_dv_et_M->Fill(dv_mass);                             // dimuon mass
        m_dv_et_R->Fill(dv_R);                                
        m_dv_et_z->Fill(dv_z);                                
        m_dv_et_l->Fill(dv_l);                                
    }

    return;
}


bool DisplacedDimuonAnalysisAlg::PassCosmicVeto_R_CR(xAOD::TrackParticle& tr0, xAOD::TrackParticle& tr1){

    bool PassCosmicVeto = true;
    float Rcos_min = 0.04;

    // define TLorentzVector of decay particles
    TLorentzVector tlv_tp0;
    TLorentzVector tlv_tp1;
    tlv_tp0 = tr0.p4();
    tlv_tp1 = tr1.p4();

    float deltaPhiMinusPi = std::fabs(std::fabs(tlv_tp0.DeltaPhi(tlv_tp1)) - std::acos(-1.));
    float sumEta = tlv_tp0.Eta() + tlv_tp1.Eta();
    float Rcos = std::sqrt(sumEta * sumEta + deltaPhiMinusPi * deltaPhiMinusPi);

    ATH_MSG_DEBUG("Rcos = " << Rcos << ", tlv_tp0.eta = " << tlv_tp0.Eta() << ", tlv_tp1.eta = " << tlv_tp1.Eta());

    if (Rcos < Rcos_min) PassCosmicVeto = false;

    return PassCosmicVeto;
}

bool DisplacedDimuonAnalysisAlg::PassCosmicVeto_DeltaR(xAOD::TrackParticle& tr0, xAOD::TrackParticle& tr1){

    bool PassCosmicVeto = true;
    float deltaR_min = 0.5;

    TLorentzVector tlv_tp0;
    TLorentzVector tlv_tp1;

    // define TLorentzVector of decay particles
    tlv_tp0 = tr0.p4();
    tlv_tp1 = tr1.p4();

    float deltaR = tlv_tp0.DeltaR(tlv_tp1);

    if (deltaR < deltaR_min) PassCosmicVeto = false;


    return PassCosmicVeto;
}

const xAOD::TruthVertex* DisplacedDimuonAnalysisAlg::getClosestTruthVertex(const xAOD::Vertex *rv){

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
