#ifndef LEPTONSELECTIONTOOLS_H
#define LEPTONSELECTIONTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AsgTools/ToolHandle.h"
#include "ILeptonSelectionTools.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/Vertex.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"

class LeptonSelectionTools : public AthAlgTool, virtual public ILeptonSelectionTools {

    public:

        LeptonSelectionTools (const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~LeptonSelectionTools();
        virtual StatusCode  initialize();
        virtual StatusCode  finalize();

        // Remove electrons from bad cluster 
        void BadClusterRemoval(xAOD::Vertex& dv);
        bool BadClusterRemoval(xAOD::Electron& el);

        // Kenematic cut
        void ElectronKinematicCut(xAOD::Vertex& dv);
        bool ElectronKinematicCut(xAOD::Electron& el);

        // Electron ID
        void ElectronID(xAOD::Vertex& dv);
        bool ElectronID(xAOD::Electron& el);

        // Muon selection
        void MuonSelection(xAOD::Vertex& dv);
        bool MuonSelection(xAOD::Muon& mu);

        // accessor
        SG::AuxElement::Accessor<std::shared_ptr<xAOD::ElectronContainer>> m_accEl;
        SG::AuxElement::Accessor<std::shared_ptr<xAOD::MuonContainer>> m_accMu;

    private:

        ToolHandle<IAsgElectronLikelihoodTool> m_LooseLH;
        ToolHandle<CP::IMuonSelectionTool> m_mst;
        ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_mct;

};

#endif //> !LEPTONSELECTIONTOOLS_H
