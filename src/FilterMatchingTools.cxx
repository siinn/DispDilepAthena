// DispDilepAthena includes
#include "DispDilepAthena/FilterMatchingTools.h"
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

FilterMatchingTools::FilterMatchingTools( const std::string& type,const std::string& name, const IInterface* parent) :
AthAlgTool( type, name, parent )
{
    declareInterface<IFilterMatchingTools>(this);
}


FilterMatchingTools::~FilterMatchingTools() {}


StatusCode FilterMatchingTools::initialize() {
    return StatusCode::SUCCESS;
}

StatusCode FilterMatchingTools::finalize() {
    return StatusCode::SUCCESS;
}

bool FilterMatchingTools::PassFilter(std::string channel, xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2) {

    bool pass = false;

    // check if given vertex type pass one of RPVLL filters
    if (channel == "mut") {
        if (PassSingleMuon(tp1, tp2)) pass = true;
    }

    if (channel == "et") {
        if (PassSingleEgamma(tp1, tp2)) pass = true;
        else if (PassDiEgamma(tp1, tp2)) pass = true;
    }

    if (channel == "idid") {
        if (PassSingleEgamma(tp1, tp2)) pass = true;
        else if (PassDiEgamma(tp1, tp2)) pass = true;
        else if (PassSingleMuon(tp1, tp2)) pass = true;
    }

    return pass;
}

bool FilterMatchingTools::PassSingleEgamma(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2) {

    bool pass = false;

    // set selection
    float pt_min = 150.;    // GeV
    float eta_max = 2.5;
    
    // only 1 track needs to pass pt and eta cut
    if ((tp1.pt() / 1000 > pt_min) && (std::abs(tp1.eta()) < eta_max)) pass = true;
    if ((tp2.pt() / 1000 > pt_min) && (std::abs(tp2.eta()) < eta_max)) pass = true;

    return pass;
}

bool FilterMatchingTools::PassDiEgamma(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2) {

    bool pass = true;

    // set selection
    float pt_min = 55.;    // GeV
    float eta_max = 2.5;
    
    // both tracks need to pass pt and eta cut
    if ((tp1.pt() / 1000 < pt_min) || (std::abs(tp1.eta()) > eta_max)) pass = false;
    if ((tp2.pt() / 1000 < pt_min) || (std::abs(tp2.eta()) > eta_max)) pass = false;

    return pass;
}

bool FilterMatchingTools::PassSingleMuon(xAOD::TrackParticle& tp1, xAOD::TrackParticle& tp2) {

    bool pass = false;

    // set selection
    float pt_min = 62.;    // GeV
    float eta_max = 1.07;
    
    // only 1 track needs to pass pt and eta cut
    if ((tp1.pt() / 1000 > pt_min) && (std::abs(tp1.eta()) < eta_max)) pass = true;
    if ((tp2.pt() / 1000 > pt_min) && (std::abs(tp2.eta()) < eta_max)) pass = true;

    return pass;
}

