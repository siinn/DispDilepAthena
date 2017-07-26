// DispDilepAthena includes
#include "DispDilepAthena/LeptonSelectionTools.h"
#include "xAODEgamma/ElectronxAODHelpers.h" 
#include "xAODMuon/MuonContainer.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "cmath"

LeptonSelectionTools::LeptonSelectionTools( const std::string& type,const std::string& name, const IInterface* parent) :
AthAlgTool( type, name, parent ),
m_LooseLH("AsgElectronLikelihoodTool/ElectronLikelihoodTool"),
m_mct("CP::MuonCalibrationAndSmearingTool/MuonCorrectionTool"),
m_mst("CP::MuonSelectionTool/MuonSelectionTool"),
m_accEl("DDL_Electrons"),
m_accMu("DDL_Muons")
{
    declareInterface<ILeptonSelectionTools>(this);
    declareProperty("ElectronLikelihoodTool", m_LooseLH);
    declareProperty("MuonCorrectionTool", m_mct);
    declareProperty("MuonSelectionTool", m_mst);

}


LeptonSelectionTools::~LeptonSelectionTools() {}


StatusCode LeptonSelectionTools::initialize() {
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

    auto dv_elc = m_accEl(dv);
    for(auto el = dv_elc->begin(); el != dv_elc->end();) {

        if( ((*el)->pt() < 7000) or (fabs((*el)->caloCluster()->etaBE(2)) > 2.47) ) dv_elc->erase(el);
        else ++el;
    }
}

bool LeptonSelectionTools::ElectronKinematicCut(xAOD::Electron& el) {
    bool pass = true;
    if( ((el).pt() < 7000) or (fabs((el).caloCluster()->etaBE(2)) > 2.47) ) pass = false;
    return pass;
}

void LeptonSelectionTools::ElectronID(xAOD::Vertex& dv) {

    auto dv_elc = m_accEl(dv);
    for(auto el = dv_elc->begin(); el != dv_elc->end();) {

        if(!(m_LooseLH->accept(*el))) dv_elc->erase(el);
        else ++el;
    }
}

bool LeptonSelectionTools::ElectronID(xAOD::Electron& el) {
    bool pass = true;
    if(!(m_LooseLH->accept(el))) pass = false;
    return pass;
}

void LeptonSelectionTools::MuonSelection(xAOD::Vertex& dv) {

    auto dv_muc = m_accMu(dv);
    for(auto mu = dv_muc->begin(); mu != dv_muc->end();) {
        
        // apply muon correction. If fail, erase muon
        if(!(m_mct->applyCorrection(**mu))) dv_muc->erase(mu);
        else if(!(m_mst->accept(**mu))) dv_muc->erase(mu);
        else ++mu;
    }
}

bool LeptonSelectionTools::MuonSelection(xAOD::Muon& mu) {
    bool pass = true;
    if(!(m_mct->applyCorrection(mu))) pass = false;
    if(!(m_mst->accept(mu))) pass = false;
    return pass;
}

