#ifndef I_FILTERMATCHINGTOOLS_H
#define I_FILTERMATCHINGTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/Vertex.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"

static const InterfaceID IID_IFilterMatchingTools("IFilterMatchingTools", 1, 0);


class IFilterMatchingTools: virtual public IAlgTool {

    public: 

        virtual ~IFilterMatchingTools() {}; 
        static const InterfaceID& interfaceID(){return IID_IFilterMatchingTools;};

        virtual StatusCode  initialize()=0;
        virtual StatusCode finalize()=0;
        
        // filter matching functions
        virtual bool PassFilter(std::string channel, xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2) = 0;
        virtual bool PassSingleEgamma(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2) = 0;
        virtual bool PassDiEgamma(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2) = 0;
        virtual bool PassSingleMuon(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2) = 0;
    
    private:
    
}; 

#endif // FILTERMATCHINGTOOLS_H
