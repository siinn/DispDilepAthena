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

// for M_PI
#include "cmath"

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
    declareProperty("TrigMatch", m_trig);
    declareProperty("DiLepCosmics", m_cos);
    declareProperty("OverlapRemoval", m_or);
    declareProperty("Vertexer", m_vertexer);
}


FlipBkgEst::~FlipBkgEst() {}


StatusCode FlipBkgEst::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    ServiceHandle<ITHistSvc> histSvc("THistSvc",name());

    m_n_mu = new TH1D( "m_n_mu", "Number of muons in event", 100,0,100);
    m_n_elc = new TH1D( "m_n_elc", "Number of electrons in event", 100,0,100);
    m_n_id = new TH1D( "m_n_id", "Number id tracks in event", 300,0,300);

    m_n_mu_sel = new TH1D( "m_n_mu_sel", "Number of selected muons in event", 100,0,100);
    m_n_elc_sel = new TH1D( "m_n_elc_sel", "Number of selected electrons in event", 100,0,100);
    m_n_id_sel = new TH1D( "m_n_id_sel", "Number of selected id tracks in event", 300,0,300);

    m_mumu_cf_input = new TH1D( "m_mumu_cf_input", "Input mumu cutflow", 12,0,12);
    m_mumu_cf_noflip = new TH1D( "m_mumu_cf_noflip", "mumu cutflow (no flip)", 12,0,12);
    m_mumu_cf_flip = new TH1D( "m_mumu_cf_flip", "mumu cutflow (flip)", 12,0,12);

    m_ee_cf_input = new TH1D( "m_ee_cf_input", "Input ee cutflow", 12,0,12);
    m_ee_cf_noflip = new TH1D( "m_ee_cf_noflip", "ee cutflow (no flip)", 12,0,12);
    m_ee_cf_flip = new TH1D( "m_ee_cf_flip", "ee cutflow (flip)", 12,0,12);

    m_emu_cf_input = new TH1D( "m_emu_cf_input", "Input emu cutflow", 12,0,12);
    m_emu_cf_noflip = new TH1D( "m_emu_cf_noflip", "emu cutflow (no flip)", 12,0,12);
    m_emu_cf_flip = new TH1D( "m_emu_cf_flip", "emu cutflow (flip)", 12,0,12);

    m_idid_cf_input = new TH1D( "m_idid_cf_input", "Input idid cutflow", 12,0,12);
    m_idid_cf_noflip = new TH1D( "m_idid_cf_noflip", "idid cutflow (no flip)", 12,0,12);
    m_idid_cf_flip = new TH1D( "m_idid_cf_flip", "idid cutflow (flip)", 12,0,12);

    m_mut_cf_input = new TH1D( "m_mut_cf_input", "Input mut cutflow", 12,0,12);
    m_mut_cf_noflip = new TH1D( "m_mut_cf_noflip", "mut cutflow (no flip)", 12,0,12);
    m_mut_cf_flip = new TH1D( "m_mut_cf_flip", "mut cutflow (flip)", 12,0,12);

    m_et_cf_input = new TH1D( "m_et_cf_input", "Input et cutflow", 12,0,12);
    m_et_cf_noflip = new TH1D( "m_et_cf_noflip", "et cutflow (no flip)", 12,0,12);
    m_et_cf_flip = new TH1D( "m_et_cf_flip", "et cutflow (flip)", 12,0,12);

    m_mumu_noflip_R = new TH1F( "m_mumu_noflip_R", "vertex R (no flip)", 100,0,300);
    m_mumu_noflip_z = new TH1F( "m_mumu_noflip_z", "vertex z (no flip)", 100,-1000,1000);
    m_mumu_noflip_M = new TH1F( "m_mumu_noflip_M", "vertex M (no flip)", 100,0,1000);
    m_mumu_flip_R = new TH1F( "m_mumu_flip_R", "vertex R (flip)", 100,0,300);
    m_mumu_flip_z = new TH1F( "m_mumu_flip_z", "vertex z (flip)", 100,-1000,1000);
    m_mumu_flip_M = new TH1F( "m_mumu_flip_M", "vertex M (flip)", 100,0,1000);

    m_ee_noflip_R = new TH1F( "m_ee_noflip_R", "vertex R (no flip)", 100,0,300);
    m_ee_noflip_z = new TH1F( "m_ee_noflip_z", "vertex z (no flip)", 100,-1000,1000);
    m_ee_noflip_M = new TH1F( "m_ee_noflip_M", "vertex M (no flip)", 100,0,1000);
    m_ee_flip_R = new TH1F( "m_ee_flip_R", "vertex R (flip)", 100,0,300);
    m_ee_flip_z = new TH1F( "m_ee_flip_z", "vertex z (flip)", 100,-1000,1000);
    m_ee_flip_M = new TH1F( "m_ee_flip_M", "vertex M (flip)", 100,0,1000);

    m_emu_noflip_R = new TH1F( "m_emu_noflip_R", "vertex R (no flip)", 100,0,300);
    m_emu_noflip_z = new TH1F( "m_emu_noflip_z", "vertex z (no flip)", 100,-1000,1000);
    m_emu_noflip_M = new TH1F( "m_emu_noflip_M", "vertex M (no flip)", 100,0,1000);
    m_emu_flip_R = new TH1F( "m_emu_flip_R", "vertex R (flip)", 100,0,300);
    m_emu_flip_z = new TH1F( "m_emu_flip_z", "vertex z (flip)", 100,-1000,1000);
    m_emu_flip_M = new TH1F( "m_emu_flip_M", "vertex M (flip)", 100,0,1000);

    m_idid_noflip_R = new TH1F( "m_idid_noflip_R", "vertex R (no flip)", 100,0,300);
    m_idid_noflip_z = new TH1F( "m_idid_noflip_z", "vertex z (no flip)", 100,-1000,1000);
    m_idid_flip_M = new TH1F("m_idid_flip_M","DV mass in GeV", 1000, 0, 1000. );
    m_idid_flip_R = new TH1F( "m_idid_flip_R", "vertex R (flip)", 60,0,300);
    m_idid_flip_z = new TH1F( "m_idid_flip_z", "vertex z (flip)", 20,-1000,1000);
    m_idid_flip_l = new TH1F("m_idid_flip_l","vertex l (flip)", 20, 0, 1000. );
    m_idid_flip_deltaR = new TH1F( "m_idid_flip_deltaR", "vertex deltaR (flip)", 20,0.,2.);
    m_idid_flip_chi2_ndof = new TH1F("m_idid_flip_chi2_ndof","chi2 / ndof", 20, 0, 5. );

    m_mut_noflip_R = new TH1F( "m_mut_noflip_R", "vertex R (no flip)", 100,0,300);
    m_mut_noflip_z = new TH1F( "m_mut_noflip_z", "vertex z (no flip)", 100,-1000,1000);
    m_mut_flip_M = new TH1F("m_mut_flip_M","DV mass in GeV", 1000, 0, 1000. );
    m_mut_flip_R = new TH1F( "m_mut_flip_R", "vertex R (flip)", 60,0,300);
    m_mut_flip_z = new TH1F( "m_mut_flip_z", "vertex z (flip)", 20,-1000,1000);
    m_mut_flip_l = new TH1F("m_mut_flip_l","vertex l (flip)", 20, 0, 1000. );
    m_mut_flip_deltaR = new TH1F( "m_mut_flip_deltaR", "vertex deltaR (flip)", 20,0.,2.);
    m_mut_flip_chi2_ndof = new TH1F("m_mut_flip_chi2_ndof","chi2 / ndof", 20, 0, 5. );

    m_et_noflip_R = new TH1F( "m_et_noflip_R", "vertex R (no flip)", 100,0,300);
    m_et_noflip_z = new TH1F( "m_et_noflip_z", "vertex z (no flip)", 100,-1000,1000);
    m_et_flip_M = new TH1F("m_et_flip_M","DV mass in GeV", 1000, 0, 1000. );
    m_et_flip_R = new TH1F( "m_et_flip_R", "vertex R (flip)", 60,0,300);
    m_et_flip_z = new TH1F( "m_et_flip_z", "vertex z (flip)", 20,-1000,1000);
    m_et_flip_l = new TH1F("m_et_flip_l","vertex l (flip)", 20, 0, 1000. );
    m_et_flip_deltaR = new TH1F( "m_et_flip_deltaR", "vertex deltaR (flip)", 20,0.,2.);
    m_et_flip_chi2_ndof = new TH1F("m_et_flip_chi2_ndof","chi2 / ndof", 20, 0, 5. );


    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_mu", m_n_mu) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_elc", m_n_elc) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_id", m_n_id) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_mu_sel", m_n_mu_sel) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_elc_sel", m_n_elc_sel) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/n_id_sel", m_n_id_sel) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_cf_input", m_mumu_cf_input) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_cf_noflip", m_mumu_cf_noflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_cf_flip", m_mumu_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_cf_input", m_ee_cf_input) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_cf_noflip", m_ee_cf_noflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_cf_flip", m_ee_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_cf_input", m_emu_cf_input) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_cf_noflip", m_emu_cf_noflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_cf_flip", m_emu_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_cf_input", m_idid_cf_input) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_cf_noflip", m_idid_cf_noflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_cf_flip", m_idid_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_cf_input", m_mut_cf_input) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_cf_noflip", m_mut_cf_noflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_cf_flip", m_mut_cf_flip) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_cf_input", m_et_cf_input) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_cf_noflip", m_et_cf_noflip) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_cf_flip", m_et_cf_flip) );

    // vertex distribution
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_noflip_R", m_mumu_noflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_noflip_z", m_mumu_noflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_noflip_M", m_mumu_noflip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_flip_R", m_mumu_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_flip_z", m_mumu_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mumu/mumu_flip_M", m_mumu_flip_M) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_noflip_R", m_ee_noflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_noflip_z", m_ee_noflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_noflip_M", m_ee_noflip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_flip_R", m_ee_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_flip_z", m_ee_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_ee/ee_flip_M", m_ee_flip_M) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_noflip_R", m_emu_noflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_noflip_z", m_emu_noflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_noflip_M", m_emu_noflip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_flip_R", m_emu_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_flip_z", m_emu_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_emu/emu_flip_M", m_emu_flip_M) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_noflip_R", m_idid_noflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_noflip_z", m_idid_noflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_M", m_idid_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_R", m_idid_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_z", m_idid_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_l", m_idid_flip_l) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_deltaR", m_idid_flip_deltaR) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_idid/idid_flip_chi2_ndof", m_idid_flip_chi2_ndof) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_noflip_R", m_mut_noflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_noflip_z", m_mut_noflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_R", m_mut_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_z", m_mut_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_M", m_mut_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_l", m_mut_flip_l) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_deltaR", m_mut_flip_deltaR) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_mut/mut_flip_chi2_ndof", m_mut_flip_chi2_ndof) );

    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_noflip_R", m_et_noflip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_noflip_z", m_et_noflip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_R", m_et_flip_R) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_z", m_et_flip_z) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_M", m_et_flip_M) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_l", m_et_flip_l) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_deltaR", m_et_flip_deltaR) );
    CHECK( histSvc->regHist("/DV/FlipBkgEst/dv_et/et_flip_chi2_ndof", m_et_flip_chi2_ndof) );

    // flag to check MC
    bool isMC;

    int n_trk_pair = 0;

    return StatusCode::SUCCESS;
}

StatusCode FlipBkgEst::finalize() {
    ATH_MSG_INFO("n_trk_sel = " << n_trk_sel);
    ATH_MSG_INFO("n_tt_pair = " << n_tt_pair);
    return StatusCode::SUCCESS;
}

StatusCode FlipBkgEst::execute() {  
    ATH_MSG_DEBUG ("Executing " << name() << "...");

    // retrieve event info
    const xAOD::EventInfo* evtInfo = nullptr;
    CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );

    // flag to check if data or MC
    isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

    // flag to check trigger
    bool trig_passed = false;

    // GRL
    if (!isMC and !m_grlTool->passRunLB(*evtInfo)) return StatusCode::SUCCESS;

    // event cleaning
    if(!m_evtc->PassEventCleaning(*evtInfo)) return StatusCode::SUCCESS;

    // trigger check
    if (m_tdt->isPassed("HLT_mu60_0eta105_msonly")) trig_passed = true;
    if (m_tdt->isPassed("HLT_g140_loose")) trig_passed = true;
    if (m_tdt->isPassed("HLT_2g50_loose")) trig_passed = true;
    if (m_tdt->isPassed("HLT_2g60_loose_L12EM15VH")) trig_passed = true;

    if(!trig_passed) return StatusCode::SUCCESS;

    // retrieve lepton and track container
    const xAOD::MuonContainer* muc = nullptr;
    CHECK( evtStore()->retrieve( muc, "Muons" ));

    const xAOD::ElectronContainer* elc = nullptr;
    CHECK( evtStore()->retrieve( elc, "Electrons" ));

    const xAOD::TrackParticleContainer* idc = nullptr;
    CHECK( evtStore()->retrieve( idc, "InDetTrackParticles" ));
    //CHECK( evtStore()->retrieve( idc, "VrtSecInclusive_SelectedTrackParticles" ));

    // make copies of leptons
    auto muc_copy = xAOD::shallowCopyContainer(*muc);
    xAOD::setOriginalObjectLink(*muc, *muc_copy.first);

    auto elc_copy = xAOD::shallowCopyContainer(*elc);
    xAOD::setOriginalObjectLink(*elc, *elc_copy.first);

    auto idc_copy = xAOD::shallowCopyContainer(*idc);
    xAOD::setOriginalObjectLink(*idc, *idc_copy.first);

    // apply overlap removal
    m_or->FindOverlap(*elc_copy.first, *muc_copy.first);

    // retrieve primary vertices
    const xAOD::VertexContainer* pvc = nullptr;
    CHECK( evtStore()->retrieve( pvc, "PrimaryVertices" ));

    // get primary vertex
    auto pv = m_evtc->GetPV(*pvc);
    auto pv_pos = m_evtc->GetPV(*pvc)->position();

    // retrieve secondary vertices
    const xAOD::VertexContainer* dvc = nullptr;
    CHECK( evtStore()->retrieve( dvc, "VrtSecInclusive_SecondaryVertices" ));

    // make a copy of vertex containers
    auto dvc_copy = xAOD::shallowCopyContainer(*dvc);

    // perform lepton matching
    for(auto dv: *dvc_copy.first) {
        m_dilepdvc->ApplyLeptonMatching(*dv, *elc_copy.first, *muc_copy.first);
    }

    // create container for good tracks
    auto el_sel     = new xAOD::TrackParticleContainer();
    auto el_sel_aux = new xAOD::TrackParticleAuxContainer();
    el_sel->setStore(el_sel_aux);

    auto mu_sel     = new xAOD::TrackParticleContainer();
    auto mu_sel_aux = new xAOD::TrackParticleAuxContainer();
    mu_sel->setStore(mu_sel_aux);

    auto id_sel     = new xAOD::TrackParticleContainer();
    auto id_sel_aux = new xAOD::TrackParticleAuxContainer();
    id_sel->setStore(id_sel_aux);

    m_n_mu->Fill(muc->size());
    m_n_elc->Fill(elc->size());
    m_n_id->Fill(idc->size());


    //-----------------------------------
    // track selection and deep copy
    //-----------------------------------

    for(auto mu: *muc_copy.first) {

        // overlap removal
        if(m_or->IsOverlap(*mu)) continue;

        // apply muon selection
        //if(!mu->muonType() == xAOD::Muon::Combined) continue;
        if(!m_leptool->MuonSelection(*mu)) continue;

        // access muon ID track
        auto mu_tr = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        if(mu_tr == nullptr) continue;
        if(!m_vertexer->GoodTrack(*mu_tr)) continue;

        // copy ID track
        xAOD::TrackParticle* tr_ptr = new xAOD::TrackParticle();
        mu_sel->push_back(tr_ptr);
        xAOD::safeDeepCopy(*mu_tr, *tr_ptr);

        // decorate ID track with muon p4
        m_acc_p4(*tr_ptr) = mu->p4();

        // add triggr matching flag to copied particle
        // vector to check muon
        std::vector<const xAOD::IParticle*> lep_trigCheck;

        // clear before evaluate and add muon
        lep_trigCheck.clear();
        lep_trigCheck.push_back(mu);

        // trig matching
        if (m_tmt->match(lep_trigCheck,"HLT_mu60_0eta105_msonly")) {
            tr_ptr->auxdecor<int>("trig_matched") = 1;
        }
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
        if(el_tr == nullptr) continue;
        if(!m_vertexer->GoodTrack(*el_tr)) continue;

        // copy ID track
        xAOD::TrackParticle* tr_ptr = new xAOD::TrackParticle();
        el_sel->push_back(tr_ptr);
        xAOD::safeDeepCopy(*el_tr, *tr_ptr);

        // decorate ID track with muon p4
        m_acc_p4(*tr_ptr) = el->p4();

        // add triggr matching flag to copied particle
        if((m_trig->Match(*el, "HLT_g140_loose"))
            or (m_trig->Match(*el, "HLT_2g50_loose"))
            or (m_trig->Match(*el, "HLT_2g60_loose_L12EM15VH"))){
            tr_ptr->auxdecor<int>("trig_matched") = 1;
        }
    }

    for(auto id_tr: *idc_copy.first) {

        // remove lepton tracks
        if(m_or->IsLeptonTrack(*id_tr)) continue;

        // track quality requirement
        if(!m_vertexer->GoodTrack(*id_tr)) continue;

        //// copy ID track
        xAOD::TrackParticle* tr_ptr = new xAOD::TrackParticle();
        id_sel->push_back(tr_ptr);
        xAOD::safeDeepCopy(*id_tr, *tr_ptr);

        // count selected track
        n_trk_sel++;

        // decorate ID track with muon p4
        m_acc_p4(*tr_ptr) = id_tr->p4();

        // no trigger matching for InDetTrackParticles

    }
    // Fill selected leptons
    m_n_mu_sel->Fill(mu_sel->size());
    m_n_elc_sel->Fill(el_sel->size());
    m_n_id_sel->Fill(id_sel->size());


    //-----------------------------------
    // loop over combination of tracks
    // and perform vertexing
    //-----------------------------------
    // create containers for displaced vertices
    auto dv     = new xAOD::VertexContainer();
    auto dv_aux = new xAOD::VertexAuxContainer();
    dv->setStore(dv_aux);

    m_vertexer->SetVtxContainer(*dv);

    // e + e
    if(el_sel->size() > 1) {
        // perform vertexing
        for(auto el1_itr = el_sel->begin(); el1_itr != el_sel->end(); el1_itr++)
        {
            for(auto el2_itr = el1_itr+1; el2_itr != el_sel->end(); el2_itr++)
            {
                //// create accessor for trig match flag
                //static SG::AuxElement::ConstAccessor<int> acc_lep_trig_match("trig_matched");
                //bool match1 = acc_lep_trig_match.isAvailable(**el1_itr);
                //bool match2 = acc_lep_trig_match.isAvailable(**el2_itr);

                //// skip this pair if both lepton don't match to any trigger
                //if (!(match1 or match2)) {
                //    ATH_MSG_DEBUG("This pair failed trigger matching");
                //    continue;
                //}
           
                // count number of pairs
                m_ee_cf_noflip->Fill("ee pair",1);
                m_ee_cf_flip->Fill("ee pair",1);

                auto dp = (**el1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit(**el1_itr, **el2_itr, pv_pos, "ee");

                // parameters to invert
                float d0_inv = -dp[0];
                float z0_inv = -dp[1];
                float phi0_inv = -dp[2];
                float theta_inv = M_PI - dp[3];

                // set defining parameters to flip track
                (**el1_itr).setDefiningParameters(d0_inv, z0_inv, phi0_inv, theta_inv, dp[4]);
                auto dp_inverted = (**el1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit_flip(**el1_itr, **el2_itr, pv_pos, "ee");

                // flip the track back
                (**el1_itr).setDefiningParameters(dp[0], dp[1], dp[2], dp[3], dp[4]);
                auto dp_back = (**el1_itr).definingParameters();
            }
        }
    }

    // e + mu
    if(el_sel->size() + mu_sel->size() > 1) {
        // perform vertexing
        for(auto el1_itr = el_sel->begin(); el1_itr != el_sel->end(); el1_itr++)
        {
            for(auto mu_itr = mu_sel->begin(); mu_itr != mu_sel->end(); mu_itr++)
            {
                //// create accessor for trig match flag
                //static SG::AuxElement::ConstAccessor<int> acc_lep_trig_match("trig_matched");
                //bool match1 = acc_lep_trig_match.isAvailable(**el1_itr);
                //bool match2 = acc_lep_trig_match.isAvailable(**mu_itr);

                // skip this pair if both lepton don't match to any trigger
                //if (!(match1 or match2)) {
                //    ATH_MSG_DEBUG("This pair failed trigger matching");
                //    continue;
                //}

                // count number of pairs
                m_emu_cf_noflip->Fill("e#mu pair",1);
                m_emu_cf_flip->Fill("e#mu pair",1);

                auto dp = (**el1_itr).definingParameters();

                // vertex fit of two original tracks
                //ATH_MSG_INFO("Perform fit with two tracks = " << **el1_itr << ", " << **el2_itr);
                PerformFit(**el1_itr, **mu_itr, pv_pos, "emu");

                // parameters to invert
                float d0_inv = -dp[0];
                float z0_inv = -dp[1];
                float phi0_inv = -dp[2];
                float theta_inv = M_PI - dp[3];

                // set defining parameters to flip track
                (**el1_itr).setDefiningParameters(d0_inv, z0_inv, phi0_inv, theta_inv, dp[4]);
                auto dp_inverted = (**el1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit_flip(**el1_itr, **mu_itr, pv_pos, "emu");

                // flip the track back
                (**el1_itr).setDefiningParameters(dp[0], dp[1], dp[2], dp[3], dp[4]);
                auto dp_back = (**el1_itr).definingParameters();
            }
        }
    }

    // mu + mu
    if(mu_sel->size() > 1) {
        // perform vertexing
        for(auto mu1_itr = mu_sel->begin(); mu1_itr != mu_sel->end(); mu1_itr++)
        {
            for(auto mu2_itr = mu1_itr+1; mu2_itr != mu_sel->end(); mu2_itr++)
            {
                //// create accessor for trig match flag
                //static SG::AuxElement::ConstAccessor<int> acc_lep_trig_match("trig_matched");
                //bool match1 = acc_lep_trig_match.isAvailable(**mu1_itr);
                //bool match2 = acc_lep_trig_match.isAvailable(**mu2_itr);

                // skip this pair if both lepton don't match to any trigger
                //if (!(match1 or match2)) {
                //    ATH_MSG_DEBUG("This pair failed trigger matching");
                //    continue;
                //}

                // count number of pairs
                m_mumu_cf_noflip->Fill("#mu#mu pair",1);
                m_mumu_cf_flip->Fill("#mu#mu pair",1);
                
                auto dp = (**mu1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit(**mu1_itr, **mu2_itr, pv_pos, "mumu");

                // parameters to invert
                float d0_inv = -dp[0];
                float z0_inv = -dp[1];
                float phi0_inv = -dp[2];
                float theta_inv = M_PI - dp[3];

                // set defining parameters to flip track
                (**mu1_itr).setDefiningParameters(d0_inv, z0_inv, phi0_inv, theta_inv, dp[4]);
                auto dp_inverted = (**mu1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit_flip(**mu1_itr, **mu2_itr, pv_pos, "mumu");

                // flip the track back
                (**mu1_itr).setDefiningParameters(dp[0], dp[1], dp[2], dp[3], dp[4]);
                auto dp_back = (**mu1_itr).definingParameters();

            }
        }
    }

    // trk + trk
    if(id_sel->size() > 1) {
        // perform vertexing
        for(auto id1_itr = id_sel->begin(); id1_itr != id_sel->end(); id1_itr++)
        {
            for(auto id2_itr = id1_itr+1; id2_itr != id_sel->end(); id2_itr++)
            {

                // count number of pairs
                m_idid_cf_noflip->Fill("n-n pair",1);
                m_idid_cf_flip->Fill("n-n pair",1);

                // counting pairs
                n_tt_pair++;

                // no trigger matching for id tracks pair 
                auto dp = (**id1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit(**id1_itr, **id2_itr, pv_pos, "idid");

                // parameters to invert
                float d0_inv = -dp[0];
                float z0_inv = -dp[1];
                float phi0_inv = -dp[2];
                float theta_inv = M_PI - dp[3];

                // set defining parameters to flip track
                (**id1_itr).setDefiningParameters(d0_inv, z0_inv, phi0_inv, theta_inv, dp[4]);
                auto dp_inverted = (**id1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit_flip(**id1_itr, **id2_itr, pv_pos, "idid");

                // flip the track back
                (**id1_itr).setDefiningParameters(dp[0], dp[1], dp[2], dp[3], dp[4]);
                auto dp_back = (**id1_itr).definingParameters();
            }
        }
    }

    // mu + trk
    if(id_sel->size() + mu_sel->size() > 1) {
        // perform vertexing
        for(auto id1_itr = id_sel->begin(); id1_itr != id_sel->end(); id1_itr++)
        {
            for(auto mu_itr = mu_sel->begin(); mu_itr != mu_sel->end(); mu_itr++)
            {
                // create accessor for trig match flag
                //static SG::AuxElement::ConstAccessor<int> acc_lep_trig_match("trig_matched");
                //bool match1 = acc_lep_trig_match.isAvailable(**id1_itr);
                //bool match2 = acc_lep_trig_match.isAvailable(**mu_itr);

                // skip this pair if both lepton don't match to any trigger
                //if (!(match1 or match2)) continue;
                //if (!(match1 or match2)) {
                //    ATH_MSG_DEBUG("This pair failed trigger matching");
                //    continue;
                //}
                // counting pairs
                //m_mut_cf_flip->Fill("pair",1);
                //m_mut_cf_noflip->Fill("pair",1);

                // count number of pairs
                m_mut_cf_noflip->Fill("#mu-n pair",1);
                m_mut_cf_flip->Fill("#mu-n pair",1);

                auto dp = (**id1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit(**id1_itr, **mu_itr, pv_pos, "mut");

                // parameters to invert
                float d0_inv = -dp[0];
                float z0_inv = -dp[1];
                float phi0_inv = -dp[2];
                float theta_inv = M_PI - dp[3];

                // set defining parameters to flip track
                (**id1_itr).setDefiningParameters(d0_inv, z0_inv, phi0_inv, theta_inv, dp[4]);
                auto dp_inverted = (**id1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit_flip(**id1_itr, **mu_itr, pv_pos, "mut");

                // flip the track back
                (**id1_itr).setDefiningParameters(dp[0], dp[1], dp[2], dp[3], dp[4]);
                auto dp_back = (**id1_itr).definingParameters();
            }
        }
    }

    // el + trk
    if(id_sel->size() + el_sel->size() > 1) {
        // perform vertexing
        for(auto id1_itr = id_sel->begin(); id1_itr != id_sel->end(); id1_itr++)
        {
            for(auto el_itr = el_sel->begin(); el_itr != el_sel->end(); el_itr++)
            {
                // create accessor for trig match flag
                //static SG::AuxElement::ConstAccessor<int> acc_lep_trig_match("trig_matched");
                //bool match1 = acc_lep_trig_match.isAvailable(**id1_itr);
                //bool match2 = acc_lep_trig_match.isAvailable(**el_itr);

                // skip this pair if both lepton don't match to any trigger
                //if (!(match1 or match2)) continue;
                //if (!(match1 or match2)) {
                //    ATH_MSG_DEBUG("This pair failed trigger matching");
                //    continue;
                //}
                // counting pairs
                //m_et_cf_flip->Fill("pair",1);
                //m_et_cf_noflip->Fill("pair",1);

                // count number of pairs
                m_et_cf_noflip->Fill("e-n pair",1);
                m_et_cf_flip->Fill("e-n pair",1);

                auto dp = (**id1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit(**id1_itr, **el_itr, pv_pos, "et");

                // parameters to invert
                float d0_inv = -dp[0];
                float z0_inv = -dp[1];
                float phi0_inv = -dp[2];
                float theta_inv = M_PI - dp[3];

                // set defining parameters to flip track
                (**id1_itr).setDefiningParameters(d0_inv, z0_inv, phi0_inv, theta_inv, dp[4]);
                auto dp_inverted = (**id1_itr).definingParameters();

                // vertex fit of two original tracks
                PerformFit_flip(**id1_itr, **el_itr, pv_pos, "et");

                // flip the track back
                (**id1_itr).setDefiningParameters(dp[0], dp[1], dp[2], dp[3], dp[4]);
                auto dp_back = (**id1_itr).definingParameters();
            }
        }
    }

    delete dv;
    delete dv_aux;
    

    delete elc_copy.first;
    delete elc_copy.second;
    delete muc_copy.first;
    delete muc_copy.second;
    delete el_sel;
    delete el_sel_aux;
    delete mu_sel;
    delete mu_sel_aux;

    return StatusCode::SUCCESS;
}

StatusCode FlipBkgEst::beginInputFile() { 
    return StatusCode::SUCCESS;
}


bool FlipBkgEst::PassCosmicVeto_R_CR(xAOD::TrackParticle& tr0, xAOD::TrackParticle& tr1){

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

    if (Rcos < Rcos_min) PassCosmicVeto = false;

    return PassCosmicVeto;
}

bool FlipBkgEst::PassCosmicVeto_DeltaR(xAOD::TrackParticle& tr0, xAOD::TrackParticle& tr1){

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

void FlipBkgEst::PerformFit(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv,std::string channel)
{
    // create truth vertex for matching
    const xAOD::TruthVertex *tru_matched = nullptr;

    // perform vertex fit
    ATH_MSG_DEBUG("Performing vertex fit, channel = " << channel);
    auto fit = m_vertexer->FitVertex(tr1, tr2, pv);
    if(fit) {

        // retrieve primary vertices
        const xAOD::VertexContainer* pvc = nullptr;
        evtStore()->retrieve( pvc, "PrimaryVertices" );

        float vtx_x = fit->position().x();
        float vtx_y = fit->position().y();
        float vtx_z = fit->position().z();

        float vtx_perp = std::sqrt(vtx_x*vtx_x + vtx_y*vtx_y);
        float dv_mass = std::fabs(m_accMass(*fit)) / 1000.; // in MeV

        // mass cut
        float mass_min = 3.;

        ATH_MSG_DEBUG("PerfomrFit: Found vertex with perp = " << vtx_perp);
        ATH_MSG_DEBUG("Found vertex with channel " << channel);

        if(channel == "mumu") {
        
            m_mumu_cf_noflip->Fill("#mu#mu",1);

            // place holder to match with main analysis
            m_mumu_cf_noflip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_mumu_cf_noflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_mumu_cf_noflip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_mumu_cf_noflip->Fill("#mu^{+}#mu^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_mumu_cf_noflip->Fill("DisabledModule", 1);

            // material veto
            m_mumu_cf_noflip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) return;
            m_mumu_cf_noflip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_mumu_cf_noflip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_mumu_cf_noflip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_mumu_noflip_R->Fill(vtx_perp);
            m_mumu_noflip_z->Fill(vtx_z);
            m_mumu_noflip_M->Fill(dv_mass);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_mumu_cf_noflip->Fill("Truth matched", 1);
            }
    
        }
        if(channel == "emu") {
            m_emu_cf_noflip->Fill("e#mu",1);

            // place holder to match with main analysis
            m_emu_cf_noflip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_emu_cf_noflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_emu_cf_noflip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_emu_cf_noflip->Fill("e^{+/-}#mu^{-/+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_emu_cf_noflip->Fill("DisabledModule", 1);

            // material veto (only e)
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_emu_cf_noflip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) return;
            m_emu_cf_noflip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_emu_cf_noflip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_emu_cf_noflip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_emu_noflip_R->Fill(vtx_perp);
            m_emu_noflip_z->Fill(vtx_z);
            m_emu_noflip_M->Fill(dv_mass);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_emu_cf_noflip->Fill("Truth matched", 1);
            }
        }
        if(channel == "ee") {
            m_ee_cf_noflip->Fill("ee",1);

            // place holder to match with main analysis
            m_ee_cf_noflip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_ee_cf_noflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_ee_cf_noflip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_ee_cf_noflip->Fill("e^{+}e^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_ee_cf_noflip->Fill("DisabledModule", 1);

            // material veto (only e)
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_ee_cf_noflip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) return;
            m_ee_cf_noflip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_ee_cf_noflip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_ee_cf_noflip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_ee_noflip_R->Fill(vtx_perp);
            m_ee_noflip_z->Fill(vtx_z);
            m_ee_noflip_M->Fill(dv_mass);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_ee_cf_noflip->Fill("Truth matched", 1);
            }
        }
        if(channel == "idid") {
        
            m_idid_cf_noflip->Fill("Trk-Trk",1);

            // place holder to match with main analysis
            m_idid_cf_noflip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_idid_cf_noflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_idid_cf_noflip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_idid_cf_noflip->Fill("Trk^{+}Trk^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_idid_cf_noflip->Fill("DisabledModule", 1);

            // material veto
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_idid_cf_noflip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) return;
            m_idid_cf_noflip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_idid_cf_noflip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_idid_cf_noflip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_idid_noflip_R->Fill(vtx_perp);
            m_idid_noflip_z->Fill(vtx_z);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_idid_cf_noflip->Fill("Truth matched", 1);
            }
    
        }

        if(channel == "mut") {
        
            m_mut_cf_noflip->Fill("#mu-Trk",1);

            // place holder to match with main analysis
            m_mut_cf_noflip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_mut_cf_noflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_mut_cf_noflip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_mut_cf_noflip->Fill("#mu^{+,-}Trk^{-,+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_mut_cf_noflip->Fill("DisabledModule", 1);

            // material veto
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_mut_cf_noflip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) return;
            m_mut_cf_noflip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_mut_cf_noflip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_mut_cf_noflip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_mut_noflip_R->Fill(vtx_perp);
            m_mut_noflip_z->Fill(vtx_z);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_mut_cf_noflip->Fill("Truth matched", 1);
            }
    
        }

        if(channel == "et") {
        
            m_et_cf_noflip->Fill("e-Trk",1);

            // place holder to match with main analysis
            m_et_cf_noflip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_et_cf_noflip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_et_cf_noflip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_et_cf_noflip->Fill("e^{+,-}Trk^{-,+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_et_cf_noflip->Fill("DisabledModule", 1);

            // material veto
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_et_cf_noflip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) return;
            m_et_cf_noflip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_et_cf_noflip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_et_cf_noflip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_et_noflip_R->Fill(vtx_perp);
            m_et_noflip_z->Fill(vtx_z);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_et_cf_noflip->Fill("Truth matched", 1);
            }
    
        }
    }
}

void FlipBkgEst::PerformFit_flip(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv,std::string channel)
{
    // create truth vertex for matching
    const xAOD::TruthVertex *tru_matched = nullptr;

    // perform vertex fit
    ATH_MSG_DEBUG("Performing vertex fit, channel = " << channel);
    auto fit = m_vertexer->FitVertex(tr1, tr2, pv);
    if(fit) {

        // retrieve primary vertices
        const xAOD::VertexContainer* pvc = nullptr;
        evtStore()->retrieve( pvc, "PrimaryVertices" );

        float vtx_x = fit->position().x();
        float vtx_y = fit->position().y();
        float vtx_z = fit->position().z();

        float vtx_perp = std::sqrt(vtx_x*vtx_x + vtx_y*vtx_y);
        float vtx_l = std::sqrt(vtx_x*vtx_x + vtx_y*vtx_y + vtx_z*vtx_z);
        float dv_mass = std::fabs(m_accMass(*fit)) / 1000.; // in MeV

        // mass cut
        float mass_min = 3.;

        ATH_MSG_DEBUG("PerfomrFit: Found vertex with perp = " << vtx_perp);


        ATH_MSG_DEBUG("Found vertex with channel " << channel);
        if(channel == "mumu") {
        
            m_mumu_cf_flip->Fill("#mu#mu",1);

            // place holder to match with main analysis
            m_mumu_cf_flip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_mumu_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_mumu_cf_flip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_mumu_cf_flip->Fill("#mu^{+}#mu^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_mumu_cf_flip->Fill("DisabledModule", 1);

            // material veto
            m_mumu_cf_flip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            //if(!m_dvc->PassLowMassVeto(*fit)) return;
            if(dv_mass < mass_min) return;
            m_mumu_cf_flip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_mumu_cf_flip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_mumu_cf_flip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_mumu_flip_R->Fill(vtx_perp);
            m_mumu_flip_z->Fill(vtx_z);
            m_mumu_flip_M->Fill(dv_mass);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_mumu_cf_flip->Fill("Truth matched", 1);
            }
    
        }
        if(channel == "emu") {
            m_emu_cf_flip->Fill("e#mu",1);

            // place holder to match with main analysis
            m_emu_cf_flip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_emu_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_emu_cf_flip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_emu_cf_flip->Fill("e^{+/-}#mu^{-/+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_emu_cf_flip->Fill("DisabledModule", 1);

            // material veto (only e)
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_emu_cf_flip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            if(dv_mass < mass_min) return;
            m_emu_cf_flip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_emu_cf_flip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_emu_cf_flip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_emu_flip_R->Fill(vtx_perp);
            m_emu_flip_z->Fill(vtx_z);
            m_emu_flip_M->Fill(dv_mass);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_emu_cf_flip->Fill("Truth matched", 1);
            }
        }
        if(channel == "ee") {
            m_ee_cf_flip->Fill("ee",1);

            // place holder to match with main analysis
            m_ee_cf_flip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_ee_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_ee_cf_flip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_ee_cf_flip->Fill("e^{+}e^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_ee_cf_flip->Fill("DisabledModule", 1);

            // material veto (only e)
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_ee_cf_flip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            //if(!m_dvc->PassLowMassVeto(*fit)) return;
            //m_ee_cf_flip->Fill("LowMassVeto", 1);
            if(dv_mass < mass_min) return;
            m_ee_cf_flip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_ee_cf_flip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_ee_cf_flip->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_ee_flip_R->Fill(vtx_perp);
            m_ee_flip_z->Fill(vtx_z);
            m_ee_flip_M->Fill(dv_mass);

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_ee_cf_flip->Fill("Truth matched", 1);
            }
        }
        if(channel == "idid") {
        
            m_idid_cf_flip->Fill("Trk-Trk",1);

            // place holder to match with main analysis
            m_idid_cf_flip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_idid_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_idid_cf_flip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_idid_cf_flip->Fill("Trk^{+}Trk^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_idid_cf_flip->Fill("DisabledModule", 1);

            // material veto
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_idid_cf_flip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            //if(!m_dvc->PassLowMassVeto(*fit)) return;
            //m_idid_cf_flip->Fill("LowMassVeto", 1);
            if(dv_mass < mass_min) return;
            m_idid_cf_flip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_idid_cf_flip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_idid_cf_flip->Fill("#DeltaR > 0.5", 1);

            //==========================================
            // vertex distribution fill
            m_idid_flip_R->Fill(vtx_perp);
            m_idid_flip_z->Fill(vtx_z);
            m_idid_flip_l->Fill(vtx_l);
            m_idid_flip_chi2_ndof->Fill((*fit).chiSquared() / (*fit).numberDoF());

            // mass plot
            //float dv_mass = std::fabs(m_accMass(*fit)) / 1000.; // in MeV
            m_idid_flip_M->Fill(dv_mass);

            // deltaR plot
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;

            // define TLorentzVector of decay particles
            tlv_tp0 = tr1.p4();
            tlv_tp1 = tr2.p4();

            float deltaR = tlv_tp0.DeltaR(tlv_tp1);
            m_idid_flip_deltaR->Fill(deltaR);
            //==========================================

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_idid_cf_flip->Fill("Truth matched", 1);
            }
    
        }

        if(channel == "mut") {
        
            m_mut_cf_flip->Fill("#mu-Trk",1);

            // place holder to match with main analysis
            m_mut_cf_flip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_mut_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_mut_cf_flip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_mut_cf_flip->Fill("#mu^{+,-}Trk^{-,+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_mut_cf_flip->Fill("DisabledModule", 1);

            // material veto
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_mut_cf_flip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            //if(!m_dvc->PassLowMassVeto(*fit)) return;
            //m_mut_cf_flip->Fill("LowMassVeto", 1);
            if(dv_mass < mass_min) return;
            m_mut_cf_flip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_mut_cf_flip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_mut_cf_flip->Fill("#DeltaR > 0.5", 1);

            //==========================================
            // vertex distribution fill
            //==========================================
            m_mut_flip_R->Fill(vtx_perp);
            m_mut_flip_z->Fill(vtx_z);
            m_mut_flip_l->Fill(vtx_l);
            m_mut_flip_chi2_ndof->Fill((*fit).chiSquared() / (*fit).numberDoF());

            // mass plot
            m_mut_flip_M->Fill(dv_mass);

            // deltaR plot
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;

            // define TLorentzVector of decay particles
            tlv_tp0 = tr1.p4();
            tlv_tp1 = tr2.p4();

            float deltaR = tlv_tp0.DeltaR(tlv_tp1);
            m_mut_flip_deltaR->Fill(deltaR);
            //==========================================

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_mut_cf_flip->Fill("Truth matched", 1);
            }
    
        }

        if(channel == "et") {
        
            m_et_cf_flip->Fill("e-Trk",1);

            // place holder to match with main analysis
            m_et_cf_flip->Fill("place holder",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_et_cf_flip->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_et_cf_flip->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_et_cf_flip->Fill("e^{+,-}Trk^{-,+}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_et_cf_flip->Fill("DisabledModule", 1);

            // material veto
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_et_cf_flip->Fill("MaterialVeto (Only e)", 1);

            // low mass veto
            //if(!m_dvc->PassLowMassVeto(*fit)) return;
            //m_et_cf_flip->Fill("LowMassVeto", 1);
            if(dv_mass < mass_min) return;
            m_et_cf_flip->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_et_cf_flip->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            m_et_cf_flip->Fill("#DeltaR > 0.5", 1);

            //==========================================
            // vertex distribution fill
            //==========================================
            m_et_flip_R->Fill(vtx_perp);
            m_et_flip_z->Fill(vtx_z);
            m_et_flip_l->Fill(vtx_l);
            m_et_flip_chi2_ndof->Fill((*fit).chiSquared() / (*fit).numberDoF());

            // mass plot
            //float dv_mass = std::fabs(m_accMass(*fit)) / 1000.; // in MeV
            m_et_flip_M->Fill(dv_mass);

            // deltaR plot
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;

            // define TLorentzVector of decay particles
            tlv_tp0 = tr1.p4();
            tlv_tp1 = tr2.p4();

            float deltaR = tlv_tp0.DeltaR(tlv_tp1);
            m_et_flip_deltaR->Fill(deltaR);
            //==========================================

            // truth match
            if (isMC){
                tru_matched = getClosestTruthVertex(fit);
                if(tru_matched) m_et_cf_flip->Fill("Truth matched", 1);
            }
    
        }
    }
}
