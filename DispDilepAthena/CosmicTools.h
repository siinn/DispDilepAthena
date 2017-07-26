#ifndef COSMICTOOLS_H
#define COSMICTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AsgTools/ToolHandle.h"
#include "ICosmicTools.h"

#include "xAODEgamma/ElectronContainer.h"
//#include "xAODEgamma/Electron.h"
#include "xAODMuon/MuonContainer.h"
//#include "xAODBase/IParticleHelpers.h"
//#include "xAODTracking/TrackParticle.h"
//#include "xAODTracking/Vertex.h"

class CosmicTools : public AthAlgTool, virtual public ICosmicTools {

    public:

        CosmicTools (const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~CosmicTools();
        virtual StatusCode  initialize();
        virtual StatusCode  finalize();

        // get DeltaPhi - Pi
        float getDeltaPhiMinusPi(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel);

        // get Sum of eta
        float getSumEta(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel);

        // get Delta R between two decay particles
        float getDeltaR(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel);

        //// Remove electrons from bad cluster 
        //void BadClusterRemoval(xAOD::Vertex& dv);

        //// Kenematic cut
        //void ElectronKinematicCut(xAOD::Vertex& dv);

        //// Electron ID
        //void ElectronID(xAOD::Vertex& dv);

        //// Muon selection
        //void MuonSelection(xAOD::Vertex& dv);

        //// accessor
        //SG::AuxElement::Accessor<std::shared_ptr<xAOD::ElectronContainer>> m_accEl;
        //SG::AuxElement::Accessor<std::shared_ptr<xAOD::MuonContainer>> m_accMu;

    private:


};

#endif //> !CosmicTools_H
