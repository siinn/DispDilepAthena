#ifndef DISPDILEPATHENA_FLIPBKGEST_H
#define DISPDILEPATHENA_FLIPBKGEST_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

// STL
#include <map>
#include <string>
#include <vector>

// EDM
#include "AsgTools/ToolHandle.h"

// xAOD
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"

// Track to vertex 
#include "ITrackToVertex/ITrackToVertex.h"

// DV
#include "DDLBase/IEventCuts.h"
#include "DDLBase/IDVCuts.h"
#include "DDLBase/IDiLepDVCuts.h"
#include "DDLBase/IDiLepCosmics.h"
#include "DDLBase/IOverlapRemoval.h"
#include "DDLBase/ITrigMatch.h"
#include "DDLCombBkg/IVertexing.h"

// DVUtil
#include "DispDilepAthena/DVUtils.h"
#include "DispDilepAthena/LeptonSelectionTools.h"
#include "DispDilepAthena/CosmicTools.h"

// GRL
#include "GoodRunsLists/IGoodRunsListSelectionTool.h"

// Trigger decision tool
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"

// Athena
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"


class FlipBkgEst: public ::AthAnalysisAlgorithm { 
    public: 
        FlipBkgEst( const std::string& name, ISvcLocator* pSvcLocator );
        virtual ~FlipBkgEst(); 
        
        virtual StatusCode  initialize();
        virtual StatusCode  execute();
        virtual StatusCode  finalize();
        
        virtual StatusCode beginInputFile();

        // Cosmic veto cut
        virtual bool PassCosmicVeto_R_CR(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);
        //virtual bool PassCosmicVeto_DeltaR(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);
        //virtual std::string ApplyLeptonRequirement(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);

        // Perform vertexing and fill final histograms
        virtual void PerformFit(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv, std::string channel);
        virtual void PerformFit_flip(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv, std::string channel);

        // Find vertex type given number of e and m
        virtual std::string FindDecayChannel(int n_mu, int n_elc);

        // Fill track pairs
        virtual void FillTrackPair(std::string channel);

        virtual const xAOD::TruthVertex* getClosestTruthVertex(const xAOD::Vertex *dv);

        virtual bool RefitVertex(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2,xAOD::Vertex& vrt);
        bool isMC;
    
    private: 
        // setting tools
        ToolHandle<DDL::IEventCuts> m_evtc; //!
        ToolHandle<DDL::IDiLepDVCuts> m_dilepdvc;
        ToolHandle<DDL::IDVCuts> m_dvc;
        ToolHandle<IGoodRunsListSelectionTool> m_grlTool; //!
        ToolHandle<Trig::TrigDecisionTool> m_tdt; //!
        ToolHandle<IDVUtils> m_dvutils; //!
        ToolHandle<ILeptonSelectionTools> m_leptool; //!
        ToolHandle<ICosmicTools> m_costool; //!
        ToolHandle<DDL::IOverlapRemoval> m_or; //!
        ToolHandle<DDL::IDispVertexer> m_vertexer; //!
        ToolHandle<Trig::IMatchingTool> m_tmt; //!
        ToolHandle<DDL::ITrigMatch> m_trig; //!
        ToolHandle<DDL::IDiLepCosmics> m_cos; //!
        ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool; //!
        ToolHandle<Trk::ITrkVKalVrtFitter> m_fitsvc;

        // DV mass accessor
        SG::AuxElement::ConstAccessor<float> m_accMass;
        SG::AuxElement::Accessor<std::shared_ptr<xAOD::ElectronContainer>> m_accEl;
        SG::AuxElement::Accessor<std::shared_ptr<xAOD::MuonContainer>> m_accMu;
        SG::AuxElement::Accessor<TLorentzVector> m_acc_p4;

        // histograms
        TH1D* m_n_mu; //!
        TH1D* m_n_elc; //!
        TH1D* m_n_id; //!

        TH1D* m_n_mu_sel; //!
        TH1D* m_n_elc_sel; //!
        TH1D* m_n_id_sel; //!

        TH1D* m_mumu_cf_nonflip; //!
        TH1D* m_mumu_cf_flip; //!

        TH1D* m_ee_cf_nonflip; //!
        TH1D* m_ee_cf_flip; //!

        TH1D* m_emu_cf_nonflip; //!
        TH1D* m_emu_cf_flip; //!

        TH1D* m_idid_cf_nonflip; //!
        TH1D* m_idid_cf_flip; //!

        TH1D* m_mut_cf_nonflip; //!
        TH1D* m_mut_cf_flip; //!

        TH1D* m_et_cf_nonflip; //!
        TH1D* m_et_cf_flip; //!

        // vertex distribution
        TH1F* m_mumu_nonflip_R; //!
        TH1F* m_mumu_nonflip_z; //!
        TH1F* m_mumu_nonflip_M; //!
        TH1F* m_mumu_nonflip_Rcos; //!
        TH1F* m_mumu_flip_R; //!
        TH1F* m_mumu_flip_z; //!
        TH1F* m_mumu_flip_M; //!
        TH1F* m_mumu_flip_Rcos; //!

        TH1F* m_ee_nonflip_R; //!
        TH1F* m_ee_nonflip_z; //!
        TH1F* m_ee_nonflip_M; //!
        TH1F* m_ee_nonflip_Rcos; //!
        TH1F* m_ee_flip_R; //!
        TH1F* m_ee_flip_z; //!
        TH1F* m_ee_flip_M; //!
        TH1F* m_ee_flip_Rcos; //!

        TH1F* m_emu_nonflip_R; //!
        TH1F* m_emu_nonflip_z; //!
        TH1F* m_emu_nonflip_M; //!
        TH1F* m_emu_nonflip_Rcos; //!
        TH1F* m_emu_flip_R; //!
        TH1F* m_emu_flip_z; //!
        TH1F* m_emu_flip_M; //!
        TH1F* m_emu_flip_Rcos; //!

        TH1F* m_idid_nonflip_R; //!
        TH1F* m_idid_nonflip_z; //!
        TH1F* m_idid_nonflip_M; //!
        TH1F* m_idid_nonflip_l; //!
        TH1F* m_idid_nonflip_n_tracks; //!

        TH1F* m_idid_flip_R; //!
        TH1F* m_idid_flip_z; //!
        TH1F* m_idid_flip_M; //!
        TH1F* m_idid_flip_l; //!
        TH1F* m_idid_flip_n_tracks; //!
        TH1F* m_idid_flip_chi2_ndof; //!
        TH1F* m_idid_flip_deltaR; //!

        TH1F* m_mut_nonflip_R; //!
        TH1F* m_mut_nonflip_z; //!
        TH1F* m_mut_nonflip_Rcos; //!
        TH1F* m_mut_flip_R; //!
        TH1F* m_mut_flip_z; //!
        TH1F* m_mut_flip_M; //!
        TH1F* m_mut_flip_l; //!
        TH1F* m_mut_flip_Rcos; //!
        TH1F* m_mut_flip_chi2_ndof; //!
        TH1F* m_mut_flip_deltaR; //!

        TH1F* m_et_nonflip_R; //!
        TH1F* m_et_nonflip_z; //!
        TH1F* m_et_nonflip_Rcos; //!
        TH1F* m_et_flip_R; //!
        TH1F* m_et_flip_z; //!
        TH1F* m_et_flip_M; //!
        TH1F* m_et_flip_l; //!
        TH1F* m_et_flip_Rcos; //!
        TH1F* m_et_flip_chi2_ndof; //!
        TH1F* m_et_flip_deltaR; //!

        // Track parameters
        TH1F* m_track_nonflip_p; //!
        TH1F* m_track_nonflip_pt; //!
        TH1F* m_track_nonflip_z0; //!
        TH1F* m_track_nonflip_d0; //!
        TH1F* m_track_nonflip_phi; //!
        TH1F* m_track_nonflip_theta; //!
        TH1F* m_track_nonflip_z0sigma; //!
        TH1F* m_track_nonflip_d0sigma; //!
        TH1F* m_track_nonflip_d0_over_d0sigma; //!
        TH1F* m_track_nonflip_z0_over_z0sigma; //!

        TH1F* m_track_nonflip_p_wrtSV; //!
        TH1F* m_track_nonflip_pt_wrtSV; //!
        TH1F* m_track_nonflip_z0_wrtSV; //!
        TH1F* m_track_nonflip_d0_wrtSV; //!
        TH1F* m_track_nonflip_phi_wrtSV; //!
        TH1F* m_track_nonflip_theta_wrtSV; //!
        TH1F* m_track_nonflip_z0sigma_wrtSV; //!
        TH1F* m_track_nonflip_d0sigma_wrtSV; //!
        TH1F* m_track_nonflip_d0_over_d0sigma_wrtSV; //!
        TH1F* m_track_nonflip_z0_over_z0sigma_wrtSV; //!

        TH1F* m_track_flip_p; //!
        TH1F* m_track_flip_pt; //!
        TH1F* m_track_flip_z0; //!
        TH1F* m_track_flip_d0; //!
        TH1F* m_track_flip_phi; //!
        TH1F* m_track_flip_theta; //!
        TH1F* m_track_flip_z0sigma; //!
        TH1F* m_track_flip_d0sigma; //!
        TH1F* m_track_flip_d0_over_d0sigma; //!
        TH1F* m_track_flip_z0_over_z0sigma; //!

        TH1F* m_track_flip_p_wrtSV; //!
        TH1F* m_track_flip_pt_wrtSV; //!
        TH1F* m_track_flip_z0_wrtSV; //!
        TH1F* m_track_flip_d0_wrtSV; //!
        TH1F* m_track_flip_phi_wrtSV; //!
        TH1F* m_track_flip_theta_wrtSV; //!
        TH1F* m_track_flip_d0sigma_wrtSV; //!
        TH1F* m_track_flip_z0sigma_wrtSV; //!
        TH1F* m_track_flip_d0_over_d0sigma_wrtSV; //!
        TH1F* m_track_flip_z0_over_z0sigma_wrtSV; //!


        // counting selected tracks
        int n_trk_sel = 0;
        int n_mu_sel = 0;
        int n_el_sel = 0;

        // count leptons with nullptr
        int n_mu_null = 0;
        int n_mu_valid = 0;
        int n_el_null = 0;
        int n_el_valid = 0;
        int n_trk_noperigee = 0;

        // containers
        const xAOD::VertexContainer* pvc = nullptr;

        // fit results
        Amg::Vector3D m_dv_fit;
        TLorentzVector m_Momentum;
        long int m_Charge;
        std::vector<double> m_ErrorMatrix;
        std::vector<double> m_Chi2PerTrk;
        std::vector<std::vector<double>> m_TrkAtVrt;
        double m_Chi2;
        std::vector<const Trk::Perigee*> m_perigees;

        // counting vertex failed on refitting
        int n_refit_nonflip_failed = 0;
        int n_refit_nonflip_succeeded = 0;
        int n_refit_flip_failed = 0;
        int n_refit_flip_succeeded = 0;
}; 

#endif //> !DISPDILEPATHENA_FLIPBKGEST_H

