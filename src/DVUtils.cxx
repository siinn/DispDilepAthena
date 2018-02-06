// DispDilepAthena includes
#include "DispDilepAthena/DVUtils.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "cmath"
#include <algorithm>    // to find min and max
#include <string>

DVUtils::DVUtils( const std::string& type,const std::string& name, const IInterface* parent) :
AthAlgTool( type, name, parent ),
m_accTr("recoTrackLink"),
m_accMu("DDL_Muons"),
m_accEl("DDL_Electrons"),
m_dilepdvc("DDL::DiLepDVCuts/DiLepDVCuts"),
m_mc("DDL::MuonCuts/DiLepMuonCuts"),
m_trig("DDL::TrigMatch/TrigMatch"),
m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
m_leptool("LeptonSelectionTools"),
m_tmt("Trig::MatchingTool/TrigMatchingTool")
{
    declareInterface<IDVUtils>(this);
    declareProperty("MuonCut", m_mc);
    declareProperty("DiLepDVCuts", m_dilepdvc);
    declareProperty("TrigMatch", m_trig);
    declareProperty("TrigDecisionTool", m_tdt);
    declareProperty("TrigMatchingTool", m_tmt);

}


DVUtils::~DVUtils() {}


StatusCode DVUtils::initialize() {
    return StatusCode::SUCCESS;
}

StatusCode DVUtils::finalize() {
    return StatusCode::SUCCESS;
}


//-------------------------------------------------------------
// DV tools
//-------------------------------------------------------------

// find decay channel of DV
std::string DVUtils::DecayChannel(xAOD::Vertex& dv) {

    std::string decayChannel = "";

    auto dv_muc = m_accMu(dv);
    auto dv_elc = m_accEl(dv);

    if (dv_muc->size() == 2) decayChannel = "mumu";
    if (dv_elc->size() == 2) decayChannel = "ee";
    if ((dv_muc->size() == 1) and (dv_elc->size() == 1)) decayChannel = "emu";
    if ((dv_muc->size() == 1) and (dv_elc->size() == 0)) decayChannel = "mut";
    if ((dv_muc->size() == 0) and (dv_elc->size() == 1)) decayChannel = "et";
    if ((dv_muc->size() == 0) and (dv_elc->size() == 0)) decayChannel = "idid";

    return decayChannel;

}


// trig matching
bool DVUtils::TrigMatching(xAOD::Vertex& dv) {

    // find which channel we are evaluating
    std::string channel = DecayChannel(dv);

    // access leptons
    auto dv_muc = m_accMu(dv);
    auto dv_elc = m_accEl(dv);

    // set flag
    bool pass = false;

    std::vector<const xAOD::IParticle*> lep_trigCheck;

    if (channel == "mumu"){
        for(auto mu: *dv_muc){
            // clear before evaluate and add muon
            lep_trigCheck.clear();
            lep_trigCheck.push_back(mu);

            // evaluate trigger
            if(m_tmt->match(lep_trigCheck,"HLT_mu60_0eta105_msonly")) pass = true;
        }
    }
    if (channel == "ee"){
        for(auto el: *dv_elc){
            // evaluate trigger
            if(m_trig->Match(*el, "HLT_g140_loose")) pass = true;
            if(m_trig->Match(*el, "HLT_2g50_loose")) pass = true;
            if(m_trig->Match(*el, "HLT_2g60_loose_L12EM15VH")) pass = true;
        }
    }
    if (channel == "emu"){
        for(auto mu: *dv_muc){
            // clear before evaluate and add muon
            lep_trigCheck.clear();
            lep_trigCheck.push_back(mu);

            // evaluate trigger
            if(m_tmt->match(lep_trigCheck,"HLT_mu60_0eta105_msonly")) pass = true;
        }
        for(auto el: *dv_elc){
            // evaluate trigger
            if(m_trig->Match(*el, "HLT_g140_loose")) pass = true;
            if(m_trig->Match(*el, "HLT_2g50_loose")) pass = true;
            if(m_trig->Match(*el, "HLT_2g60_loose_L12EM15VH")) pass = true;
        }
    }

    return pass;
}


// calculate R of dv
float DVUtils::getR(const xAOD::Vertex& dv, const xAOD::Vertex& pv) {
    // This function calculate perpendicular distance from hard-scattering
    // primary vertex to dv

    // access position of vertices
    auto dv_pos = dv.position();
    auto pv_pos = pv.position();

    // distance in 3d vector
    auto dist = pv_pos - dv_pos;

    // return distance in transverse plane
    return dist.perp();

}

// calculate z of dv
float DVUtils::getz(const xAOD::Vertex& dv, const xAOD::Vertex& pv) {
    // This function calculate z distance from hard-scattering
    // primary vertex to dv

    // access position of vertices
    auto dv_pos = dv.position();
    auto pv_pos = pv.position();

    // distance in 3d vector
    auto dist = pv_pos - dv_pos;

    // return distance in transverse plane
    return dist.z();

}

// calculate r of dv
float DVUtils::getr(const xAOD::Vertex& dv, const xAOD::Vertex& pv) {
    // This function calculate r distance from hard-scattering
    // primary vertex to dv

    // access position of vertices
    auto dv_pos = dv.position();
    auto pv_pos = pv.position();

    // distance in 3d vector
    auto dist = pv_pos - dv_pos;
    float dv_r = sqrt( dist.perp()*dist.perp() + dist.z()*dist.z() );

    // return distance in transverse plane
    return dv_r;

}


// match dv to signal truth
const xAOD::TruthVertex* DVUtils::IsSignalDV(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) {

    // retrieve truth vertex container
    const xAOD::TruthVertexContainer* tru_vc = nullptr;
    evtStore()->retrieve( tru_vc, "TruthVertices");

    // debug
    //if (channel == "emu") {
    //    ATH_MSG_INFO("(emu) this emu dv has " << dv_muc.size() << " muons, " << dv_elc.size() << " electrons");
    //}

    // loop over the truth vertex container
    for (auto tru_v: *tru_vc){

        // select signal dv
        if ( !(isSignalVertex(tru_v)) ) continue;

        // access signal muons
        const xAOD::TruthParticle* tp0 = FindFinalState(tru_v->outgoingParticle(0));
        const xAOD::TruthParticle* tp1 = FindFinalState(tru_v->outgoingParticle(1));

        // set flag
        bool tp0_match = false;
        bool tp1_match = false;

        for(auto mu: dv_muc){
            const xAOD::TruthParticle* matched_truth_muon = 0;
            // access link to matched truth muon
            if(mu->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
                ElementLink<xAOD::TruthParticleContainer> link = mu->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
                // check if the link is valid
                if(link.isValid()) {
                    matched_truth_muon = *link;
                    // return true if at least one truthlink of reco muon matches with given truth
                    if ((matched_truth_muon->barcode() == tp0->barcode()) and (std::abs(tp0->pdgId()) == 13)) tp0_match = true;
                    if ((matched_truth_muon->barcode() == tp1->barcode()) and (std::abs(tp1->pdgId()) == 13)) tp1_match = true;
                } 
            } 
        } 

        for(auto el: dv_elc){
            const xAOD::TruthParticle* matched_truth_electron = xAOD::TruthHelpers::getTruthParticle(*el);

            // access link to matched truth electron
            if (matched_truth_electron != nullptr) {
                // return true if at least one truthlink of reco muon matches with given truth
                if ((matched_truth_electron->barcode() == tp0->barcode()) and (std::abs(tp0->pdgId()) == 11)) tp0_match = true;
                if ((matched_truth_electron->barcode() == tp1->barcode()) and (std::abs(tp1->pdgId()) == 11)) tp1_match = true;
            } 
        } 

        // if we have two muons matched, this truth dv is matched to given dv
        if (tp0_match and tp1_match) return tru_v;

    } // end of tru_vc
    return nullptr; // return null if there is no match (reco dv is not matched to truth)
}

const xAOD::TruthVertex* DVUtils::IsSignalDV_loose(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel, xAOD::Vertex& dv) {

    // retrieve truth vertex container
    const xAOD::TruthVertexContainer* tru_vc = nullptr;
    evtStore()->retrieve( tru_vc, "TruthVertices");

    // loop over the truth vertex container
    for (auto tru_v: *tru_vc){

        // select signal dv
        if ( !(isSignalVertex(tru_v)) ) continue;

        // access signal muons
        const xAOD::TruthParticle* tp0 = FindFinalState(tru_v->outgoingParticle(0));
        const xAOD::TruthParticle* tp1 = FindFinalState(tru_v->outgoingParticle(1));

        // set flag
        bool tp0_match = false;
        bool tp1_match = false;
        bool pos_match = false;

        // set maximum deviation in position
        float R_err_max = 0.2;
        float z_err_max = 0.4;

        float R_err = tru_v->perp() - dv.position().perp();
        float z_err = tru_v->z() - dv.position().z();

        // flag if dv position matches with truth vertex position
        if ((R_err < R_err_max) and (z_err < z_err_max)) pos_match = true;

        for(auto mu: dv_muc){
            const xAOD::TruthParticle* matched_truth_muon = 0;
            // access link to matched truth muon
            if(mu->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
                ElementLink<xAOD::TruthParticleContainer> link = mu->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
                // check if the link is valid
                if(link.isValid()) {
                    matched_truth_muon = *link;
                    // return true if at least one truthlink of reco muon matches with given truth
                    if ((matched_truth_muon->barcode() == tp0->barcode()) and (std::abs(tp0->pdgId()) == 13)) tp0_match = true;
                    if ((matched_truth_muon->barcode() == tp1->barcode()) and (std::abs(tp1->pdgId()) == 13)) tp1_match = true;
                } 
            } 
        } 

        for(auto el: dv_elc){
            const xAOD::TruthParticle* matched_truth_electron = xAOD::TruthHelpers::getTruthParticle(*el);

            // access link to matched truth electron
            if (matched_truth_electron != nullptr) {
                // return true if at least one truthlink of reco muon matches with given truth
                if ((matched_truth_electron->barcode() == tp0->barcode()) and (std::abs(tp0->pdgId()) == 11)) tp0_match = true;
                if ((matched_truth_electron->barcode() == tp1->barcode()) and (std::abs(tp1->pdgId()) == 11)) tp1_match = true;
            } 
        } 

        // if we have two muons matched, this truth dv is matched to given dv
        if ((tp0_match or tp1_match) and (pos_match)) return tru_v;

    } // end of tru_vc
    return nullptr; // return null if there is no match (reco dv is not matched to truth)
}


float DVUtils::TrackMass (const xAOD::TrackParticle& tp1, const xAOD::TrackParticle& tp2){

    // define TLorentzVector of decay particles
    TLorentzVector tlv_tp0;
    TLorentzVector tlv_tp1;
    TLorentzVector tlv_total;

    tlv_tp0 = tp1.p4();
    tlv_tp1 = tp2.p4();
    tlv_total = tlv_tp0 + tlv_tp1;

    return tlv_total.M();
}


float DVUtils::getDeltaR (const xAOD::TrackParticle& tp1, const xAOD::TrackParticle& tp2){

    // define TLorentzVector of decay particles
    TLorentzVector tlv_tp0;
    TLorentzVector tlv_tp1;

    tlv_tp0 = tp1.p4();
    tlv_tp1 = tp2.p4();

    float deltaR_tlv = tlv_tp0.DeltaR(tlv_tp1);

    return deltaR_tlv;
}

// trigger matching. check if one muon of DV is matched to trigger
bool DVUtils::TriggerMatching(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc) {

    bool pass = false;
    
    // check if any muon matches to trigger
    for(auto mu: dv_muc){
        ATH_MSG_DEBUG("Trigger matched = " << m_tmt->match(*mu,"HLT_mu60_0eta105_msonly"));
        if (m_tmt->match(*mu,"HLT_mu60_0eta105_msonly")) pass = true;
    }

    // check if any electron matches to trigger
    ATH_MSG_INFO("Before trigger matching electrons, pass = " << pass);
    for(auto el: dv_elc){
        ATH_MSG_INFO("checking trigger.. HLT_g140_loose = " << m_tmt->match(*el,"HLT_g140_loose"));
        ATH_MSG_INFO("checking trigger.. HLT_2g50_loose = " << m_tmt->match(*el,"HLT_2g50_loose"));
        if ((m_tmt->match(*el,"HLT_g140_loose")) or (m_tmt->match(*el,"HLT_2g50_loose")) or (m_tmt->match(*el,"HLT_2g60_loose_L12EM15VH"))) pass = true;
    }
    ATH_MSG_INFO("After trigger matching electrons, pass = " << pass);
    return pass;
}

// check if two muons are both combined muons
bool DVUtils::IsCombinedMuon(const DataVector<xAOD::Muon> dv_muc) {

    bool pass = true;

    for(auto mu: dv_muc){
        //if( (mu->muonType() == xAOD::Muon::Combined) or (mu->muonType() == xAOD::Muon::MuonStandAlone)) pass = true;
        if(!(mu->muonType() == xAOD::Muon::Combined)) pass = false;
    }
    return pass;
}

//-------------------------------------------------------------
// Truth DV tools
//-------------------------------------------------------------

// check if this truth DV is reconstructed
bool DVUtils::IsReconstructed(const xAOD::TruthVertex* tru_v) {

    // retrieve secondary vertices
    const xAOD::VertexContainer* dvc = nullptr;
    CHECK( evtStore()->retrieve( dvc, "VrtSecInclusive_SecondaryVertices" ));

    // make a copy of vertex containers
    auto dvc_copy = xAOD::shallowCopyContainer(*dvc);

    // retrieve lepton container
    const xAOD::MuonContainer* muc = nullptr;
    CHECK( evtStore()->retrieve( muc, "Muons" ));

    const xAOD::ElectronContainer* elc = nullptr;
    CHECK( evtStore()->retrieve( elc, "Electrons" ));

    // make copies of muon
    auto muc_copy = xAOD::shallowCopyContainer(*muc);
    xAOD::setOriginalObjectLink(*muc, *muc_copy.first);

    auto elc_copy = xAOD::shallowCopyContainer(*elc);
    xAOD::setOriginalObjectLink(*elc, *elc_copy.first);

    // perform lepton matching
    for(auto dv: *dvc_copy.first) {
        //ApplyMuonMatching(*dv, *muc_copy.first);
        m_dilepdvc->ApplyLeptonMatching(*dv, *elc_copy.first, *muc_copy.first);
    }

    // access signal muons
    const xAOD::TruthParticle* truth_child0 = FindFinalState(tru_v->outgoingParticle(0));
    const xAOD::TruthParticle* truth_child1 = FindFinalState(tru_v->outgoingParticle(1));

    // loop over dv's
    for (auto dv: *dvc_copy.first) {

        // set flag
        bool muon_match0 = false;
        bool muon_match1 = false;

        // collect muons from this dv
        auto dv_muc = m_dilepdvc->GetMu(*dv);

        // require dv to have 2 muons
        if (dv_muc->size() != 2) continue;

        // loop over reconstructed dv's to find match
        for(auto mu: *dv_muc){
            const xAOD::TruthParticle* matched_truth_muon=0;
            // access link to matched truth muon
            if(mu->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
                ElementLink<xAOD::TruthParticleContainer> link = mu->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
                // check if the link is valid
                if(link.isValid()) {
                    matched_truth_muon = *link;
                    // return true if at least one truthlink of reco muon matches with given truth
                    if (matched_truth_muon->barcode() == truth_child0->barcode()) muon_match0 = true;
                    if (matched_truth_muon->barcode() == truth_child1->barcode()) muon_match1 = true;
                } // end of link.isValid
            } // end of if mu->isAvailable
        } // end of muon container

        // if we have two muons matched, this truth dv is reconstructed
        if (muon_match0 and muon_match1) return true;

    } // end of loop over dvc
    return false;
} // end of IsReconstructed




// check if muons of signal DV are reconstructed
bool DVUtils::MuonsReconstructed(const xAOD::TruthVertex* tru_v) {

    // access signal muons
    const xAOD::TruthParticle* truth_child0 = FindFinalState(tru_v->outgoingParticle(0));
    const xAOD::TruthParticle* truth_child1 = FindFinalState(tru_v->outgoingParticle(1));

    if (IsReconstructedAsMuon(truth_child0) and IsReconstructedAsMuon(truth_child0)) return true;
    return false;
}


// check if both muons of truth dv passes eta cut
bool DVUtils::PassAcceptance(const xAOD::TruthVertex* tru_v) {

    bool pass = true;

    for (unsigned int i = 0; i < tru_v->nOutgoingParticles(); i++){
        const xAOD::TruthParticle* truth_child = FindFinalState(tru_v->outgoingParticle(i));
        if (truth_child->eta() > 2.4) pass = false;
    }

    return pass;
}

// find maximum d0 among outgoing particles
float DVUtils::GetMaxd0(const xAOD::TruthVertex* tru_v) {

    float Maxd0 = 0;

    // loop over outgoing particles at the truth vertex
    for (unsigned int i = 0; i < tru_v->nOutgoingParticles(); i++){

        const xAOD::TruthParticle* truth_child = FindFinalState(tru_v->outgoingParticle(i));
        if (truth_child->isAvailable<float>("d0")) {
            float d0 = fabs(truth_child->auxdata<float>("d0"));
            Maxd0 = std::max( d0, Maxd0);
        }
    }
    return Maxd0;
}

// find pT of lower pT muon
float DVUtils::GetMinPT(const xAOD::TruthVertex* tru_v) {

    float min_pT = 0;

    // loop over outgoing particles at the truth vertex
    for (unsigned int i = 0; i < tru_v->nOutgoingParticles(); i++){

        const xAOD::TruthParticle* truth_child = FindFinalState(tru_v->outgoingParticle(i));
        float pt = truth_child->pt();

        min_pT = std::max(pt, min_pT);
    }
    return min_pT;
}

// return final state of given truth particle
const xAOD::TruthParticle* DVUtils::FindFinalState(const xAOD::TruthParticle* tp) {

    ATH_MSG_DEBUG("--- New truth particle, barcode = " << tp->barcode() );
    const xAOD::TruthParticle *ftp = tp;

    int n_counter = 0;

    do {
        // return final state particle
        if (ftp->status() == 1) {
            ATH_MSG_DEBUG("--- Found Final state particle, status = " << ftp->status() << ", barcode = " << ftp->barcode() );
            return ftp;
        }
        ATH_MSG_DEBUG("Found intermediate truth particle, status = " << ftp->status() << ", barcode = " << ftp->barcode() );
        // follow decay chain
        if (ftp->child(0) != nullptr) {
            ftp = ftp->child(0);
            ATH_MSG_DEBUG("find next child, barcode = " << ftp->barcode() );
        }
        // bremsstrahlung
        if (ftp->nChildren() > 1) {
            ATH_MSG_DEBUG("More than 1 outgoing particle. nChildren = " << ftp->nChildren() );
            for (unsigned int i = 0; i < ftp->nChildren(); i++ ) {
                ATH_MSG_DEBUG("children " << i << ": status = " << ftp->child(i)->status() << ", pdgid = " << ftp->child(i)->pdgId() );

                // choose the one with same pdgId to avoid bremsstrahlung
                if (ftp->child(i)->absPdgId() == ftp->absPdgId()) {
                    ftp = ftp->child(i);
                }
            } 
        } // end of nChildren > 1
        n_counter++;

    //} while ( (ftp->status() != 1) and (ftp != nullptr) );
    } while ( (ftp != nullptr) and (n_counter < 10) );

    ATH_MSG_DEBUG( "ERROR : Found intermediate particle with invalid children, status = " << ftp->status() << ", nChildren = " << ftp->nChildren() << ", is it null?" << ftp );

    return ftp;

} // end of FindFinalState

//-------------------------------------------------------------
// Lepton efficiency tools
//-------------------------------------------------------------


// check if truth particle is reconstructed as muon
bool DVUtils::IsReconstructedAsMuon(const xAOD::TruthParticle* tp) {

    // retrieve muon container
    const xAOD::MuonContainer* muc = nullptr;
    CHECK( evtStore()->retrieve( muc, "Muons" ) );

    // truncate barcode
    int barcode = tp->barcode();
    if (barcode > 1e6) barcode = barcode - 1e6;

    // loop over muon container, looking for matched reco muon
    for(auto mu: *muc) {
        const xAOD::TruthParticle* matched_truth_muon=0;
        // access link to matched truth muon
        if(mu->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink")) {
            ElementLink<xAOD::TruthParticleContainer> link = mu->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
            // check if the link is valid
            if(link.isValid()) {
                matched_truth_muon = *link;
                // return true if at least one truthlink of reco muon matches with given truth
                if (matched_truth_muon->barcode() == barcode) return true;
                else ATH_MSG_DEBUG("MUON: matched-truth barcode = "
                    << matched_truth_muon->barcode()
                    << ", tp barcode = "
                    << barcode
                    );
            } // end of link.isValid
        } // end of if mu->isAvailable
    } // end of loop over muon container

    return false;

}

// check if truth particle is reconstructed as Electron
bool DVUtils::IsReconstructedAsElectron(const xAOD::TruthParticle* tp) {

    // retrieve Electron container
    const xAOD::ElectronContainer* elc = nullptr;
    CHECK( evtStore()->retrieve( elc, "Electrons" ) );

    const xAOD::ElectronContainer* elc_HLT = nullptr;
    CHECK( evtStore()->retrieve( elc_HLT, "HLT_xAOD__ElectronContainer_egamma_Electrons" ) );

    int barcode = tp->barcode();
    if (barcode > 1e6) barcode = barcode - 1e6;

    ATH_MSG_DEBUG("----- matching for truth elc barcode = "
            << tp->barcode()
            << ", truncated = "
            << barcode
            << ", pt = "
            << tp->pt() / 1000. // GeV
            << ", eta = "
            << tp->eta()
            << ", phi = "
            << tp->phi()
            );

    ATH_MSG_DEBUG("HLT_xAOD__ElectronContainer_egamma_Electrons size = "
                << elc_HLT->size()
                );

    const xAOD::Electron* reco_elc = xAOD::EgammaHelpers::getRecoElectron(tp);
    //ATH_MSG_DEBUG("reco_elc = " << reco_elc);
    if (reco_elc){
        ATH_MSG_DEBUG("found match for reco electron = " 
            << reco_elc
            << ", reco_elcc pt = "
            << reco_elc->pt() / 1000.
            );
    }

    // loop over Electron container, looking for matched reco Electron
    for(auto el: *elc) {
        ATH_MSG_DEBUG("electron  = "
                << el
                << ", pt = "
                << el->pt() / 1000. // GeV
                << ", eta = "
                << el->eta()
                << ", phi = "
                << el->phi()
                );
        const xAOD::TruthParticle* matched_truth_electron = xAOD::TruthHelpers::getTruthParticle(*el);
        if (matched_truth_electron){
                ATH_MSG_DEBUG("matched truth for electron = " 
                    << matched_truth_electron
                    << ", pt = "
                    << matched_truth_electron->pt() / 1000.
                    << ", eta = "
                    << matched_truth_electron->eta()
                    << ", phi = "
                    << matched_truth_electron->phi()
                    << ", barcode = " 
                    << matched_truth_electron->barcode()
                    );
            if (matched_truth_electron->barcode() == barcode) {
                ATH_MSG_DEBUG("found match for electron = " 
                    << el
                    << ", elc pt = "
                    << el->pt() / 1000.
                    << ", eta = "
                    << el->eta()
                    << ", phi = "
                    << el->phi()
                    << ", matched-truth = "
                    << matched_truth_electron
                    << ", barcode = " 
                    << matched_truth_electron->barcode()
                    );
                return true;
            }
        }
    }

    // loop over Electron container, looking for matched reco Electron
    for(auto el: *elc_HLT) {
        ATH_MSG_DEBUG("HLT: electron  = "
                << el
                << ", pt = "
                << el->pt() / 1000. // GeV
                << ", eta = "
                << el->eta()
                << ", phi = "
                << el->phi()
                );
        const xAOD::TruthParticle* matched_truth_electron = xAOD::TruthHelpers::getTruthParticle(*el);
        if (matched_truth_electron){
                ATH_MSG_DEBUG("HLT: matched truth for electron = " 
                    << matched_truth_electron
                    << ", pt = "
                    << matched_truth_electron->pt() / 1000.
                    << ", eta = "
                    << matched_truth_electron->eta()
                    << ", phi = "
                    << matched_truth_electron->phi()
                    << ", barcode = " 
                    << matched_truth_electron->barcode()
                    );
            if (matched_truth_electron->barcode() == barcode) {
                ATH_MSG_DEBUG("HLT: found match for electron = " 
                    << el
                    << ", elc_HLT pt = "
                    << el->pt() / 1000.
                    << ", eta = "
                    << el->eta()
                    << ", phi = "
                    << el->phi()
                    << ", matched-truth = "
                    << matched_truth_electron
                    << ", barcode = " 
                    << matched_truth_electron->barcode()
                    );
                return true;
            }
        }
    }


    return false;
}

//-------------------------------------------------------------
// help functions for signal muons
//-------------------------------------------------------------

// find delta R between two muons from dv muon container
float DVUtils::getDeltaR(const DataVector<xAOD::Muon> dv_muc ) {

    // define TLorentzVector of muons
    TLorentzVector tlv_mu0 = dv_muc.at(0)->p4();
    TLorentzVector tlv_mu1 = dv_muc.at(1)->p4();

    float deltaR_tlv = tlv_mu0.DeltaR(tlv_mu1);

    return deltaR_tlv;
}

// find delta phi - Pi between two muons from dv muon container
float DVUtils::getDeltaPhiMinusPi(const DataVector<xAOD::Muon> dv_muc ) {

    // define TLorentzVector of muons
    TLorentzVector tlv_mu0 = dv_muc.at(0)->p4();
    TLorentzVector tlv_mu1 = dv_muc.at(1)->p4();

    float DeltaPhiMinusPi = std::fabs(std::fabs(tlv_mu0.DeltaPhi(tlv_mu1)) - std::acos(-1.));

    return DeltaPhiMinusPi;
}

// find sum of eta of two muons from dv muon container
float DVUtils::getSumEta(const DataVector<xAOD::Muon> dv_muc ) {

    // define TLorentzVector of muons
    TLorentzVector tlv_mu0 = dv_muc.at(0)->p4();
    TLorentzVector tlv_mu1 = dv_muc.at(1)->p4();

    float SumEta = tlv_mu0.Eta() + tlv_mu1.Eta();

    return SumEta;
}

// find the lowest pT of two muons from dv muon container
float DVUtils::getMinPT(const DataVector<xAOD::Muon> dv_muc ) {

    // access pT of each muon
    float mu0_pt = dv_muc.at(0)->pt() / 1000.;  // GeV
    float mu1_pt = dv_muc.at(1)->pt() / 1000.;  // GeV

    float min_pT = std::min( mu0_pt, mu1_pt );

    return min_pT;

}

// find the lowest pT of two muons from dv muon container
float DVUtils::getMaxPT(const DataVector<xAOD::Muon> dv_muc ) {

    // access pT of each muon
    float mu0_pt = dv_muc.at(0)->pt() / 1000.;  // GeV
    float mu1_pt = dv_muc.at(1)->pt() / 1000.;  // GeV

    float max_pT = std::max( mu0_pt, mu1_pt );

    return max_pT;

}

// find delta pT between two muons from dv muon container
float DVUtils::getDelta_pT(const DataVector<xAOD::Muon> dv_muc ) {

    // define TLorentzVector of muons
    TLorentzVector tlv_mu0 = dv_muc.at(0)->p4();
    TLorentzVector tlv_mu1 = dv_muc.at(1)->p4();

    float deltapT = (tlv_mu0 + tlv_mu1).Perp() / 1000.;

    //ATH_MSG_INFO("delta pT = " << deltapT);

    return deltapT;

}

//-------------------------------------------------------------
// ID track tools
//-------------------------------------------------------------

// check if truth particle is reconstructed as ID track
bool DVUtils::IsReconstructedAsIDTrack(const xAOD::TruthParticle& tp) {

    // retrieve muon container
    const xAOD::TrackParticleContainer* idc = nullptr;
    CHECK( evtStore()->retrieve( idc, "InDetTrackParticles" ) );

    // loop over muon container, looking for matched reco muon
    for(auto mu: *idc) {
        // use MS track which is not affected by badly matched ID tracks
        double dr = tp.p4().DeltaR(mu->p4());
        if(dr < 0.20) {
            return true;
        }
    } // end of loop over muon container

    return false;

}


//-------------------------------------------------------------
// truth tools
//-------------------------------------------------------------

// check if truth particle is signal muon from (Z')
bool DVUtils::isSignal (const xAOD::TruthParticle* p) {

    return false;

} // end of isSignal



// check if truth vertex is signal vertex
bool DVUtils::isSignalVertex (const xAOD::TruthVertex* v) {

    bool signal = true;

    ATH_MSG_DEBUG("DEBUG: 0, nIncoming = " << v->nIncomingParticles() << ", nOutgoing = " << v->nOutgoingParticles());

    if (!(v->nIncomingParticles() == 1)) signal = false;
    if (!((v->nOutgoingParticles() == 2) or (v->nOutgoingParticles() == 3)))
    {
        signal = false;
    }

    // access parent
    const xAOD::TruthParticle* parent = v->incomingParticle(0);
    ATH_MSG_DEBUG("DEBUG: 1, parent->absPdgId() = " << parent->absPdgId());
    if (!((parent->absPdgId() ==32) or (parent->absPdgId() ==1000022)))
    {
        signal = false;
    }

    ATH_MSG_DEBUG("DEBUG: 2, signal = " << signal);
    return signal;

} // end of isSignal


// calculate invariant mass of truth vertex
float DVUtils::TruthMass (const xAOD::TruthVertex* v) {

    // Lorentz vector to calculate invariant mass of truth vertex
    TLorentzVector outgoing_tlv;

    for (unsigned int i = 0; i < v->nOutgoingParticles(); i++) {

        const xAOD::TruthParticle* p = v->outgoingParticle(i);

        // skip if particle is not charged (to remove neutrino from mass calculation)
        if (p->isNeutral()) continue;

        outgoing_tlv += TLorentzVector( p->px(), p->py(), p->pz(), p->e());

    } // end of outgoingn particle loop

    return outgoing_tlv.M(); // return invariant mass in MeV

} // end of TruthMass

// calculate pt of truth vertex
float DVUtils::TruthPt (const xAOD::TruthVertex* v) {

    // Lorentz vector to calculate pt of truth vertex
    TLorentzVector outgoing_tlv;

    for (unsigned int i = 0; i < v->nOutgoingParticles(); i++) {

        const xAOD::TruthParticle* p = v->outgoingParticle(i);

        // skip if particle is not charged (to remove neutrino from mass calculation)
        if (p->isNeutral()) continue;

        outgoing_tlv += TLorentzVector( p->px(), p->py(), p->pz(), p->e());
    } // end of outgoingn particle loop

    return outgoing_tlv.Pt(); // return pt in MeV
} // end of TruthPt

// calculate dr of truth vertex
float DVUtils::Truth_dr (const xAOD::TruthVertex* v) {

    const xAOD::TruthParticle* tp0 = FindFinalState(v->outgoingParticle(0));
    const xAOD::TruthParticle* tp1 = FindFinalState(v->outgoingParticle(1));

    // define TLorentzVector of muons
    TLorentzVector tlv_p0 = tp0->p4();
    TLorentzVector tlv_p1 = tp1->p4();

    float deltaR_tlv = tlv_p0.DeltaR(tlv_p1);

    return deltaR_tlv;
} // end of Truth_dr


//-------------------------------------------------------------
// validation tool
//-------------------------------------------------------------

// check if track particle is reconstructed using LRT
bool DVUtils::isLargeD0Track (const xAOD::TrackParticle* tp) {
    const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = tp->patternRecoInfo();
    if (patternReco.test(49)) { return true; }
    return false;
} // end of isLargeD0Track


// track selection for validation
bool DVUtils::TrackSelection (const xAOD::TrackParticle* tp) {
    float maxEta = 2.7;
    float minPt = 1000;
    if ( (tp->pt()>1e-7 ? (fabs(tp->eta()) < maxEta) : false) &&  \
         (tp->pt() > minPt) ) return true;
    else return false;
} // end of TrackSelection

bool DVUtils::TrackSelection (const xAOD::TruthParticle* tp) {
    float maxEta = 2.7;
    float minPt = 1000;
    if (!(tp->status() == 1)) return false;
    if (!(tp->isCharged())) return false;
    if ( (tp->pt()>1e-7 ? (fabs(tp->eta()) < maxEta) : false) &&  \
         (tp->pt() > minPt) ) return true;
    else return false;
} // end of TrackSelection





// check if event pass RPVLL filter
bool DVUtils::PassRPVLLFilter(const xAOD::ElectronContainer& elc, const xAOD::PhotonContainer& phc,const xAOD::MuonContainer& muc){

    return false;
}





