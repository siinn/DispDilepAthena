#ifndef I_COSMICTOOLS_H
#define I_COSMICTOOLS_H

#include "GaudiKernel/IAlgTool.h"
//#include "xAODTracking/Vertex.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"

static const InterfaceID IID_ICosmicTools("ICosmicTools", 1, 0);


class ICosmicTools: virtual public IAlgTool {

    public: 

        virtual ~ICosmicTools() {}; 
        static const InterfaceID& interfaceID(){return IID_ICosmicTools;};

        virtual StatusCode  initialize()=0;
        virtual StatusCode finalize()=0;

        // get DeltaPhi - Pi
        virtual float getDeltaPhiMinusPi(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) = 0;

        // get Sum of eta
        virtual float getSumEta(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) = 0;

        // get Delta R between two decay particles
        virtual float getDeltaR(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) = 0;

        //// Remove electrons from bad cluster 
        //virtual void BadClusterRemoval(xAOD::Vertex& dv) = 0;

        //// Kenematic cut
        //virtual void ElectronKinematicCut(xAOD::Vertex& dv) = 0;

        //// Electron ID
        //virtual void ElectronID(xAOD::Vertex& dv) = 0;

        //// Muon selection
        //virtual void MuonSelection(xAOD::Vertex& dv) = 0;
    
    private:
    
}; 

#endif // CosmicTools_H
