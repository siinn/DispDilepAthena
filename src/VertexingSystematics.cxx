// DispDilepAthena includes
#include "VertexingSystematics.h"

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


VertexingSystematics::VertexingSystematics( const std::string& name, ISvcLocator* pSvcLocator ) : 
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
m_fitSvc( "Trk::TrkVKalVrtFitter")
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
    declareProperty("VrtFitter", m_fitSvc);
}


VertexingSystematics::~VertexingSystematics() {}


StatusCode VertexingSystematics::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");
    ServiceHandle<ITHistSvc> histSvc("THistSvc",name());

    //Float_t m_idid_R_bins[] = {0,25,50,75,100,150,200,250,300};

    m_idid_cf = new TH1D( "m_idid_cf", "idid cutflow", 7,0,7);
    m_idid_M = new TH1F("m_idid_M","DV mass in MeV", 50, 400, 600. ); // MeV
    m_idid_R = new TH1F( "m_idid_R", "vertex R ", 60,0,300);
    m_idid_z = new TH1F( "m_idid_z", "vertex z ", 20,-1000,1000);
    m_idid_l = new TH1F("m_idid_l","vertex l ", 20, 0, 1000. );
    m_idid_deltaR = new TH1F( "m_idid_deltaR", "vertex deltaR ", 20,0.,2.);
    m_idid_chi2_ndof = new TH1F("m_idid_chi2_ndof","chi2 / ndof", 20, 0, 5. );

    // ratio plot
    m_idid_R_ratio = new TH1F( "m_idid_R_ratio", "vertex R ratio", 12,0,300);

    // Ks candidate track pair
    m_ks_cand_M = new TH1F("m_ks_cand_M","Ks candidate mass in MeV", 100, 0, 1000. ); // MeV
    //m_ks_cand_M = new TH1F("m_ks_cand_M","Ks candidate mass in MeV", 50, 450, 550. ); // MeV
    m_ks_cand_cf = new TH1D("m_ks_cand_cf","Ks candidate cutflow", 5, 0, 5 ); // MeV

    CHECK( histSvc->regHist("/DV/vertex_syst/idid_cf", m_idid_cf) );
    CHECK( histSvc->regHist("/DV/vertex_syst/idid_M", m_idid_M) );
    CHECK( histSvc->regHist("/DV/vertex_syst/idid_R", m_idid_R) );
    CHECK( histSvc->regHist("/DV/vertex_syst/idid_z", m_idid_z) );
    CHECK( histSvc->regHist("/DV/vertex_syst/idid_l", m_idid_l) );
    CHECK( histSvc->regHist("/DV/vertex_syst/idid_deltaR", m_idid_deltaR) );
    CHECK( histSvc->regHist("/DV/vertex_syst/idid_chi2_ndof", m_idid_chi2_ndof) );
    CHECK( histSvc->regHist("/DV/vertex_syst/idid_R_ratio", m_idid_R_ratio) );

    CHECK( histSvc->regHist("/DV/vertex_syst/ks_cand_M", m_ks_cand_M) );
    CHECK( histSvc->regHist("/DV/vertex_syst/ks_cand_cf", m_ks_cand_cf) );

    // flag to check MC
    bool isMC;

    return StatusCode::SUCCESS;
}

StatusCode VertexingSystematics::finalize() {
    ATH_MSG_INFO ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
}

StatusCode VertexingSystematics::execute() {  
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

    //-----------------------------------
    // track selection and deep copy
    //-----------------------------------

    for(auto id_tr: *idc_copy.first) {

        // remove lepton tracks
        //if(m_or->IsLeptonTrack(*id_tr)) continue;

        // track quality requirement
        //if(!m_vertexer->GoodTrack(*id_tr)) continue;

        //// copy ID track
        xAOD::TrackParticle* tr_ptr = new xAOD::TrackParticle();
        id_sel->push_back(tr_ptr);
        xAOD::safeDeepCopy(*id_tr, *tr_ptr);

        // decorate ID track with muon p4
        m_acc_p4(*tr_ptr) = id_tr->p4();

        // no trigger matching for InDetTrackParticles

    }

    //-----------------------------------
    // loop over combination of tracks
    // and perform vertexing
    //-----------------------------------
    // create containers for displaced vertices
    auto dv     = new xAOD::VertexContainer();
    auto dv_aux = new xAOD::VertexAuxContainer();
    dv->setStore(dv_aux);

    m_vertexer->SetVtxContainer(*dv);

    // trk + trk
    if(id_sel->size() > 1) {
        // perform vertexing
        for(auto id1_itr = id_sel->begin(); id1_itr != id_sel->end(); id1_itr++)
        {
            for(auto id2_itr = id1_itr+1; id2_itr != id_sel->end(); id2_itr++)
            {
                // select Ks candidate
                if(!PassKsCandidate(**id1_itr, **id2_itr, pv_pos)) continue;

                // track quality requirement
                // this needs to be applied to replicate VrtSecInclusive
                if(!m_vertexer->GoodTrack(**id1_itr) or
                    !m_vertexer->GoodTrack(**id2_itr)) continue;

                // vertex fit of two original tracks
                PerformFit(**id1_itr, **id2_itr, pv_pos, "idid");
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



bool VertexingSystematics::PassCosmicVeto_R_CR(xAOD::TrackParticle& tr0, xAOD::TrackParticle& tr1){

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

bool VertexingSystematics::PassCosmicVeto_DeltaR(xAOD::TrackParticle& tr0, xAOD::TrackParticle& tr1){

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

bool VertexingSystematics::PassKsCandidate(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv){

    // master flag
    bool pass = false;

    // define cut values
    float pt_min = 400.;      // 400 MeV for samples with jet
    //float mass_min = 480.;
    //float mass_max = 520.;
    float mass_min = 200.;
    float mass_max = 800.;
    float delta_z_max = 2.5;
    float eta_max = 2.5;
    float direction_min = 0.995;
    float l_max = 10.0;
    float d0_max = 2.0;
    float z0_max = 2.0;
    //float chi2_max = 5.;      // not suitable for LRT tracks, hence, remvoing the cut

    // define TLorentzVector to calculate invariant mass of two tracks
    TLorentzVector ks_cand;
    ks_cand = tr1.p4() + tr2.p4();

    // calculate invariant mass of two tracks and distance in z axis
    float inv_mass = ks_cand.M();
    float delta_z = std::abs(tr1.z0() - tr2.z0());

    // create vector to run crude vertex estimation
    std::vector<const xAOD::TrackParticle*> ListBaseTracks = {&tr1, &tr2};
    
    // set fit svc to default 
    m_fitSvc->setDefault();

    // find initial approximate vertex
    Amg::Vector3D IniVertex;
    StatusCode sc = m_fitSvc->VKalVrtFitFast( ListBaseTracks, IniVertex ); /* Fast crude estimation */

    // if we can't form a crude vertex, return false
    if(sc.isFailure()) return false;

    // position of Ks vertex candidate and PV
    const TVector3 vertex_candidate(IniVertex.x(),IniVertex.y(),IniVertex.z());
    const TVector3 posPV(pv.x(), pv.y(), pv.z());

    // vector that represents direction from PV to vertex
    TLorentzVector KsFlightDirection(0.,0.,0.,0.);
    KsFlightDirection.SetVect(vertex_candidate - posPV);

    // rough estimate of Ks decay length
    const TVector3 decay_leng(vertex_candidate - posPV);
    float Ks_decay_leng = std::sqrt( decay_leng.x()*decay_leng.x() + decay_leng.y()*decay_leng.y() + decay_leng.z()*decay_leng.z());

    // apply candidate selection 
    while (true){
        m_ks_cand_cf->Fill("All pair", 1);

        if (!(inv_mass > mass_min) or !(inv_mass < mass_max)) break;
        m_ks_cand_cf->Fill("Mass", 1);

        if (!(tr1.pt() > pt_min) or !(tr2.pt() > pt_min)) break;
        m_ks_cand_cf->Fill("Track pt > 0.4 GeV", 1);

        if (!(std::abs(tr1.eta()) < eta_max) or !(std::abs(tr2.eta()) < eta_max)) break;
        m_ks_cand_cf->Fill("|#eta| < 2.5", 1);

        if (!(std::abs(tr1.d0()) < d0_max) or !(std::abs(tr2.d0()) < d0_max)) break;
        m_ks_cand_cf->Fill("d0 > 2 mm", 1);

        if (!(std::abs(tr1.z0()) < z0_max) or !(std::abs(tr2.z0()) < z0_max)) break;
        m_ks_cand_cf->Fill("z0 > 2 mm", 1);

        if(tr1.charge() * tr2.charge() > 0.0) break;
        m_ks_cand_cf->Fill("Opposite charge", 1);

        if(!(std::abs(std::cos(ks_cand.Angle(KsFlightDirection.Vect()))) > direction_min)) break;
        m_ks_cand_cf->Fill("cos(#theta_{V}) > 0.995", 1);

        if (!(delta_z < delta_z_max)) break;
        m_ks_cand_cf->Fill("|z1 - z2| < 2.5 mm", 1);

        if (!(Ks_decay_leng > l_max)) break;
        m_ks_cand_cf->Fill("Decay length > 10 mm", 1);

        // found Ks candidate
        pass = true;

        //fill invariant mass of this pair
        m_ks_cand_M->Fill(inv_mass);
        break;

    }

    return pass;
}

void VertexingSystematics::PerformFit(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv,std::string channel)
{
    // create truth vertex for matching
    const xAOD::TruthVertex *tru_matched = nullptr;

    // perform vertex fit
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
        float dv_mass = std::fabs(m_accMass(*fit)); // in MeV

        // mass cut
        float mass_min = 3.;

        if(channel == "idid") {
        
            m_idid_cf->Fill("Trk-Trk",1);

            // vertex fit quality
            if(!m_dvc->PassChisqCut(*fit)) return;
            m_idid_cf->Fill("#chi^{2}_{DV} / DOF < 5", 1);

            // minimum distance from pv (from 0 for MC)
            if(!m_dvc->PassDistCut(*fit, *pvc)) return;
            m_idid_cf->Fill("Disp. > 2 mm", 1);

            // charge requirements
            if(!m_dvc->PassChargeRequirement(*fit)) return;
            m_idid_cf->Fill("Trk^{+}Trk^{-}", 1);

            // disabled module
            if(!m_dvc->PassDisabledModuleVeto(*fit)) return;
            m_idid_cf->Fill("DisabledModule", 1);

            // material veto
            if(!m_dvc->PassMaterialVeto(*fit)) return;
            m_idid_cf->Fill("MaterialVeto", 1);

            // low mass veto
            //if(dv_mass < mass_min) return;
            //m_idid_cf->Fill("LowMassVeto", 1);

            // cosmic veto (R_CR)
            if(!PassCosmicVeto_R_CR(tr1, tr2)) return;
            m_idid_cf->Fill("R_{CR} > 0.04", 1);

            // cosmic veto (deltaR)
            //if(!PassCosmicVeto_DeltaR(tr1, tr2)) return;
            //m_idid_cf->Fill("#DeltaR > 0.5", 1);

            // vertex distribution fill
            m_idid_R->Fill(vtx_perp);
            m_idid_R_ratio->Fill(vtx_perp);
            m_idid_z->Fill(vtx_z);
            m_idid_l->Fill(vtx_l);
            m_idid_chi2_ndof->Fill((*fit).chiSquared() / (*fit).numberDoF());

            // mass plot
            m_idid_M->Fill(dv_mass);

            // deltaR plot
            TLorentzVector tlv_tp0;
            TLorentzVector tlv_tp1;

            // define TLorentzVector of decay particles
            tlv_tp0 = tr1.p4();
            tlv_tp1 = tr2.p4();

            float deltaR = tlv_tp0.DeltaR(tlv_tp1);
            m_idid_deltaR->Fill(deltaR);
            //==========================================

    
        }
    }
}

