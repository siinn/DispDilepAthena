#ifndef FILTERMATCHINGTOOLS_H
#define FILTERMATCHINGTOOLS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AsgTools/ToolHandle.h"
#include "IFilterMatchingTools.h"

#include "xAODTracking/TrackParticle.h" 

class FilterMatchingTools : public AthAlgTool, virtual public IFilterMatchingTools {

    public:

        FilterMatchingTools (const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~FilterMatchingTools();
        virtual StatusCode  initialize();
        virtual StatusCode  finalize();

        // filter matching functions
        virtual bool PassFilter(std::string channel, xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2);
        virtual bool PassSingleEgamma(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2);
        virtual bool PassDiEgamma(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2);
        virtual bool PassSingleMuon(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2);


    private:


};

#endif //> !FILTERMATCHINGTOOLS_H
