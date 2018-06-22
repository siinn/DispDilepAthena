// DispDilepAthena includes
#include "DispDilepAthena/LeptonSelectionTools.h"
#include "xAODEgamma/ElectronxAODHelpers.h" 
#include "xAODMuon/MuonContainer.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "cmath"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

LeptonSelectionTools::LeptonSelectionTools( const std::string& type,const std::string& name, const IInterface* parent) :
AthAlgTool( type, name, parent ),
m_elt("AsgElectronLikelihoodTool/ElectronLikelihoodTool"),
m_mct("CP::MuonCalibrationAndSmearingTool/MuonCorrectionTool"),
m_mst("CP::MuonSelectionTool/MuonSelectionTool"),
m_accEl("DDL_Electrons"),
m_accMu("DDL_Muons")
{
    declareInterface<ILeptonSelectionTools>(this);
    declareProperty("ElectronLikelihoodTool", m_elt);
    declareProperty("MuonCorrectionTool", m_mct);
    declareProperty("MuonSelectionTool", m_mst);
}


LeptonSelectionTools::~LeptonSelectionTools() {}


StatusCode LeptonSelectionTools::initialize() {


    ServiceHandle<IJobOptionsSvc> josvc("JobOptionsSvc", name());
    ATH_CHECK(josvc->addPropertyToCatalogue("ToolSvc.ElectronLikelihoodTool",
        StringProperty("ConfigFile", "ElectronPhotonSelectorTools/offline/mc15_20160512/ElectronLikelihoodLooseOfflineConfig2016_Smooth_NoD0.conf")));


    //ATH_CHECK(m_elt.retrieve());

    auto elt_cast = dynamic_cast<AsgElectronLikelihoodTool*>(&*m_elt);

    ATH_CHECK(elt_cast->setProperty("CutBL", std::vector<int>()));
    ATH_CHECK(elt_cast->setProperty("CutPi", std::vector<int>()));
    ATH_CHECK(elt_cast->setProperty("CutSi", std::vector<int>()));
    ATH_CHECK(elt_cast->setProperty("doCutConversion", true));


    return StatusCode::SUCCESS;
}

StatusCode LeptonSelectionTools::finalize() {
    return StatusCode::SUCCESS;
}

void LeptonSelectionTools::BadClusterRemoval(xAOD::Vertex& dv) {

    auto dv_elc = m_accEl(dv);
    for(auto el = dv_elc->begin(); el != dv_elc->end();) {

        if(!(*el)->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) dv_elc->erase(el);
        else ++el;
    }
}

bool LeptonSelectionTools::BadClusterRemoval(xAOD::Electron& el) {
    bool pass = true;
    if(!(el).isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) pass = false;
    return pass;
}

void LeptonSelectionTools::ElectronKinematicCut(xAOD::Vertex& dv) {

    // set pt, eta, and d0 threshold
    float el_pt_min = 10.;
    float el_eta_max = 2.47;


    auto dv_elc = m_accEl(dv);
    for(auto el = dv_elc->begin(); el != dv_elc->end();) {

        // access electron ID track
        auto el_tr = (*el)->trackParticle();

        // check if we have proper ID track
        if(el_tr != nullptr) {

            // requirement electron eta, track pt
            if(el_tr->pt() / 1000. < el_pt_min) dv_elc->erase(el);
            else if(std::fabs(el_tr->eta()) > el_eta_max) dv_elc->erase(el);
            else ++el;
        }
        else dv_elc->erase(el);
    }
}

bool LeptonSelectionTools::ElectronKinematicCut(xAOD::Electron& el) {

    // flag to mark this electron as passed or failed   
    bool pass = true;

    // set pt, eta, and d0 threshold
    float el_pt_min = 10.;
    float el_eta_max = 2.47;

    // access electron ID track
    auto el_tr = (el).trackParticle();

    // check if we have proper ID track
    if(el_tr != nullptr) {

        // requirement electron eta, track pt
        if(el_tr->pt() / 1000. < el_pt_min) pass = false;
        else if(std::fabs(el_tr->eta() > el_eta_max)) pass = false;
    }
    else pass = false;

    return pass;
}

void LeptonSelectionTools::ElectronID(xAOD::Vertex& dv) {

    auto dv_elc = m_accEl(dv);
    for(auto el = dv_elc->begin(); el != dv_elc->end();) {

        if(!(m_elt->accept(*el))) dv_elc->erase(el);
        else ++el;
    }

    return;
}

bool LeptonSelectionTools::ElectronID(xAOD::Electron& el) {

    bool pass = true;
    if(!(m_elt->accept(el))) pass = false;

    return pass;
}

void LeptonSelectionTools::MuonSelection(xAOD::Vertex& dv) {

    // set minimum pt
    float mu_pt_min = 10.;
    float mu_eta_max = 2.5;

    auto dv_muc = m_accMu(dv);
    for(auto mu = dv_muc->begin(); mu != dv_muc->end();) {

        // retrieve muon ID track 
        auto mu_tr = (*mu)->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        if(mu_tr != nullptr && (*mu)->muonType() == xAOD::Muon::Combined){

            // retrieve muon pt
            float mu_pt = mu_tr->pt() / 1000.;
            float mu_eta = std::fabs(mu_tr->eta());
            
            // apply muon correction. If fail, erase muon
            //if(!(m_mct->applyCorrection(**mu))) dv_muc->erase(mu);
            if(!(m_mst->accept(**mu))) dv_muc->erase(mu);
            else if(mu_pt < mu_pt_min) dv_muc->erase(mu);
            else if(mu_eta > mu_eta_max) dv_muc->erase(mu);
            else if(!((**mu).muonType() == xAOD::Muon::Combined)) dv_muc->erase(mu);
            else ++mu;
        }
        else {
                dv_muc->erase(mu);
        }
    }

    return;
}

bool LeptonSelectionTools::MuonSelection(xAOD::Muon& mu) {

    // set minimum pt
    float mu_pt_min = 10.;
    float mu_eta_max = 2.5;
    //float mu_d0_min = 2.0;

    bool pass = true;

    // retrieve muon ID track 
    auto mu_tr = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if(mu_tr != nullptr && mu.muonType() == xAOD::Muon::Combined){
    
        // retrieve muon pt, eta, and d0
        float mu_pt = mu_tr->pt() / 1000.;
        float mu_eta = std::fabs(mu_tr->eta());

        //if(!(m_mct->applyCorrection(mu))) pass = false;
        if(!(m_mst->accept(mu))) pass = false;
        else if(mu_pt < mu_pt_min) pass = false;
        else if(mu_eta > mu_eta_max) pass = false;
        else if(!(mu.muonType() == xAOD::Muon::Combined)) pass = false;
    }
    else pass = false;


    return pass;
}

