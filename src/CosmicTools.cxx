// DisplacedDimuonAnalysis includes
#include "DispDilepAthena/CosmicTools.h"
//#include "xAODEgamma/ElectronxAODHelpers.h" 
//#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
//#include "cmath"
//#include "xAODTracking/VertexContainer.h"
//#include "xAODTruth/TruthParticleContainer.h"
//#include "xAODTruth/TruthParticle.h"
//#include "xAODMuon/MuonContainer.h"
//#include "xAODMuon/MuonAuxContainer.h"
//#include "xAODTracking/TrackParticle.h"
//#include "xAODTracking/TrackParticleContainer.h"
//#include "xAODTruth/TruthVertex.h"
//#include "xAODTruth/TruthVertexContainer.h"
//#include "xAODCore/ShallowCopy.h"
//#include "xAODBase/IParticleHelpers.h"
//#include "cmath"
//#include <algorithm>    // to find min and max
//#include <string>

CosmicTools::CosmicTools( const std::string& type,const std::string& name, const IInterface* parent) :
AthAlgTool( type, name, parent )
//m_LooseLH("AsgElectronLikelihoodTool/ElectronLikelihoodTool"),
//m_mct("CP::MuonCalibrationAndSmearingTool/MuonCorrectionTool"),
//m_mst("CP::MuonSelectionTool/MuonSelectionTool"),
//m_accEl("DDL_Electrons"),
//m_accMu("DDL_Muons")
{
    declareInterface<ICosmicTools>(this);
    //declareProperty("ElectronLikelihoodTool", m_LooseLH);
    //declareProperty("MuonCorrectionTool", m_mct);
    //declareProperty("MuonSelectionTool", m_mst);

}


CosmicTools::~CosmicTools() {}


StatusCode CosmicTools::initialize() {
    return StatusCode::SUCCESS;
}

StatusCode CosmicTools::finalize() {
    return StatusCode::SUCCESS;
}


// get DeltaPhi - Pi 
float CosmicTools::getDeltaPhiMinusPi(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) {

    TLorentzVector tlv_tp0;
    TLorentzVector tlv_tp1;

    // define TLorentzVector of decay particles
    if (channel == "mumu"){
    tlv_tp0 = dv_muc.at(0)->p4();
    tlv_tp1 = dv_muc.at(1)->p4();
    }

    if (channel == "ee"){
    tlv_tp0 = dv_elc.at(0)->p4();
    tlv_tp1 = dv_elc.at(1)->p4();
    }
    if (channel == "emu"){
    tlv_tp0 = dv_elc.at(0)->p4();
    tlv_tp1 = dv_muc.at(0)->p4();
    }

    float DeltaPhiMinusPi = std::fabs(std::fabs(tlv_tp0.DeltaPhi(tlv_tp1)) - std::acos(-1.));

    return DeltaPhiMinusPi;
}

// find sum of eta of two muons from dv muon container
float CosmicTools::getSumEta(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) {

    TLorentzVector tlv_tp0;
    TLorentzVector tlv_tp1;

    // define TLorentzVector of decay particles
    if (channel == "mumu"){
    tlv_tp0 = dv_muc.at(0)->p4();
    tlv_tp1 = dv_muc.at(1)->p4();
    }

    if (channel == "ee"){
    tlv_tp0 = dv_elc.at(0)->p4();
    tlv_tp1 = dv_elc.at(1)->p4();
    }
    if (channel == "emu"){
    tlv_tp0 = dv_elc.at(0)->p4();
    tlv_tp1 = dv_muc.at(0)->p4();
    }

    float SumEta = tlv_tp0.Eta() + tlv_tp1.Eta();

    return SumEta;
}


// find delta R between two muons from dv muon container
float CosmicTools::getDeltaR(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) {

    TLorentzVector tlv_tp0;
    TLorentzVector tlv_tp1;

    // define TLorentzVector of decay particles
    if (channel == "mumu"){
    tlv_tp0 = dv_muc.at(0)->p4();
    tlv_tp1 = dv_muc.at(1)->p4();
    }

    if (channel == "ee"){
    tlv_tp0 = dv_elc.at(0)->p4();
    tlv_tp1 = dv_elc.at(1)->p4();
    }
    if (channel == "emu"){
    tlv_tp0 = dv_elc.at(0)->p4();
    tlv_tp1 = dv_muc.at(0)->p4();
    }

    float deltaR_tlv = tlv_tp0.DeltaR(tlv_tp1);

    return deltaR_tlv;
}
