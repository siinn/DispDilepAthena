#ifndef I_DVUTILS_H
#define I_DVUTILS_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/Vertex.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODMuon/MuonContainer.h"
//#include "xAODMuon/MuonAuxContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include <string>


static const InterfaceID IID_IDVUtils("IDVUtils", 1, 0);


class IDVUtils: virtual public IAlgTool {

    public: 

        virtual ~IDVUtils() {}; 
        static const InterfaceID& interfaceID(){return IID_IDVUtils;};

        virtual StatusCode  initialize()=0;
        virtual StatusCode finalize()=0;

        // get position of dv wrt pv
        virtual float getR(const xAOD::Vertex& dv, const xAOD::Vertex& pv) = 0;
        virtual float getz(const xAOD::Vertex& dv, const xAOD::Vertex& pv) = 0;
        virtual float getr(const xAOD::Vertex& dv, const xAOD::Vertex& pv) = 0;

        // get decay channel
        virtual std::string DecayChannel(xAOD::Vertex& dv) = 0;
        //virtual std::string DecayChannel(const xAOD::TrackParticle* tp1, const xAOD::TrackParticle* tp2) = 0;

        // trig matching
        virtual bool TrigMatching(xAOD::Vertex& dv) = 0;

        // reco dv is matched to signal truth dv
        virtual const xAOD::TruthVertex* IsSignalDV(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel) = 0;
        virtual const xAOD::TruthVertex* IsSignalDV_loose(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel, xAOD::Vertex& dv) = 0;

        // find invariant mass of two track
        virtual float TrackMass (const xAOD::TrackParticle& tp1, const xAOD::TrackParticle& tp2) = 0;

        // find delta R between two track
        virtual float getDeltaR (const xAOD::TrackParticle& tp1, const xAOD::TrackParticle& tp2) = 0;

        //----------------------------------------
        // truth tools
        //----------------------------------------

        // reco match using id track barcode
        virtual bool IsReconstructedAsMuon(const xAOD::TruthParticle* tp) = 0;
        virtual bool IsReconstructedAsElectron(const xAOD::TruthParticle* tp) = 0;
        // reco match using dR method
        virtual bool IsReconstructedAsIDTrack(const xAOD::TruthParticle& tp) = 0;

        // find final state truth particles
        virtual const xAOD::TruthParticle* FindFinalState(const xAOD::TruthParticle* tp) = 0;

        // muon matching tool
        //virtual void ApplyMuonMatching(xAOD::Vertex& dv, xAOD::MuonContainer& muc) = 0;

        // match truth dv to reco dv
        virtual bool IsReconstructed(const xAOD::TruthVertex* vertex_truth) = 0;

        // check if two muons if signal dv are reconstructed
        virtual bool MuonsReconstructed(const xAOD::TruthVertex* tru_v) = 0;

        // check two muons pass acceptance ( eta < 2.4)
        virtual bool PassAcceptance(const xAOD::TruthVertex* tru_v) = 0;

        // find delta between two muons from dv muon container
        virtual float getDeltaR(const DataVector<xAOD::Muon> dv_muc) = 0;
        virtual float getDeltaPhiMinusPi(const DataVector<xAOD::Muon> dv_muc) = 0;
        virtual float getSumEta(const DataVector<xAOD::Muon> dv_muc) = 0;
        virtual float getMinPT(const DataVector<xAOD::Muon> dv_muc) = 0;
        virtual float getMaxPT(const DataVector<xAOD::Muon> dv_muc) = 0;

        // find delta pT between two muons from dv muon container
        virtual float getDelta_pT(const DataVector<xAOD::Muon> dv_muc) = 0;

        // check if one muon of DV is matched to trigger
        virtual bool TriggerMatching(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc) = 0;

        // check if two muons are combined muons
        virtual bool IsCombinedMuon(const DataVector<xAOD::Muon> dv_muc) = 0;

        // get maximum d0 among outgoing particles
        virtual float GetMaxd0(const xAOD::TruthVertex* tru_v) = 0;
        virtual float GetMinPT(const xAOD::TruthVertex* tru_v) = 0;

        // check if truth track is signal muon
        virtual bool isSignal (const xAOD::TruthParticle* p) = 0;
        // check if truth vertex is Z' vertex
        virtual bool isSignalVertex (const xAOD::TruthVertex* v) = 0;
        // calculate invariant mass of truth vertex
        virtual float TruthMass (const xAOD::TruthVertex* v) = 0;
        // calculate pt of truth vertex
        virtual float TruthPt (const xAOD::TruthVertex* v) = 0;
        // calculate dr of truth vertex
        virtual float Truth_dr (const xAOD::TruthVertex* v) = 0;

        // check if ID track is LRT
        virtual bool isLargeD0Track (const xAOD::TrackParticle* tp) = 0;
        // track selection for validation
        virtual bool TrackSelection (const xAOD::TrackParticle* tp) = 0;
        virtual bool TrackSelection (const xAOD::TruthParticle* tp) = 0;

        // check if there is an associated lepton
        //virtual bool IsLepton(const xAOD::TrackParticle* tp) = 0;
        //virtual bool IsGoodMuon(const xAOD::TrackParticle* tp) = 0;
        //virtual bool IsGoodElectron(const xAOD::TrackParticle* tp) = 0;

        // RPVLL filter test
        virtual bool PassRPVLLFilter(const xAOD::ElectronContainer& elc, const xAOD::PhotonContainer& phc,const xAOD::MuonContainer& muc) = 0;
    
    private:
    
}; 

#endif // DVUTILS_H
