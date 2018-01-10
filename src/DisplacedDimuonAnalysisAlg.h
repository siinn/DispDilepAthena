#ifndef DISPDILEPATHENA_DISPLACEDDIMUONANALYSISALG_H
#define DISPDILEPATHENA_DISPLACEDDIMUONANALYSISALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

// STL
#include <map>
#include <string>
#include <vector>

// EDM
#include "AsgTools/ToolHandle.h"

// DV
#include "DDLBase/IEventCuts.h"
#include "DDLBase/IDVCuts.h"
#include "DDLBase/IDiLepDVCuts.h"
#include "DDLBase/IDiLepCosmics.h"
#include "DDLBase/IOverlapRemoval.h"

// DVUtil
#include "DispDilepAthena/DVUtils.h"
#include "DispDilepAthena/LeptonSelectionTools.h"
#include "DispDilepAthena/CosmicTools.h"

// GRL
#include "GoodRunsLists/IGoodRunsListSelectionTool.h"

// Trigger decision tool
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"

// Track to vertex 
#include "ITrackToVertex/ITrackToVertex.h"

class DisplacedDimuonAnalysisAlg: public ::AthAnalysisAlgorithm { 
    public: 

        DisplacedDimuonAnalysisAlg( const std::string& name, ISvcLocator* pSvcLocator );
        virtual ~DisplacedDimuonAnalysisAlg(); 
        
        virtual StatusCode  initialize();
        virtual StatusCode  execute();
        virtual StatusCode  finalize();
        
        virtual StatusCode beginInputFile();

        virtual bool PassCosmicVeto(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel);
        virtual void plot_signal_tp(const DataVector<xAOD::Muon> dv_muc, const DataVector<xAOD::Electron> dv_elc, std::string channel);
        virtual void plot_dv(const xAOD::Vertex& dv, const xAOD::Vertex& pv, std::string channel);

        virtual bool PassCosmicVeto_R_CR(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);
        virtual bool PassCosmicVeto_DeltaR(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);
        virtual const xAOD::TruthVertex* getClosestTruthVertex(const xAOD::Vertex *dv);
    
    private:

        // setting tools
        ToolHandle<DDL::IEventCuts> m_evtc; //!
        ToolHandle<DDL::IDiLepDVCuts> m_dilepdvc;
        ToolHandle<DDL::IDVCuts> m_dvc;
        ToolHandle<DDL::IDiLepCosmics> m_cos; //!
        ToolHandle<IGoodRunsListSelectionTool> m_grlTool; //!
        ToolHandle<Trig::TrigDecisionTool> m_tdt; //!
        ToolHandle<IDVUtils> m_dvutils; //!
        ToolHandle<ILeptonSelectionTools> m_leptool; //!
        ToolHandle<ICosmicTools> m_costool; //!
        ToolHandle<DDL::IOverlapRemoval> m_or;
        ToolHandle<Trig::IMatchingTool> m_tmt; //!
        ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool; //!

        // DV mass accessor
        SG::AuxElement::ConstAccessor<float> m_accMass;
        SG::AuxElement::Accessor<std::shared_ptr<xAOD::ElectronContainer>> m_accEl;
        SG::AuxElement::Accessor<std::shared_ptr<xAOD::MuonContainer>> m_accMu;

        // output
        TH1D* m_event_cutflow; //!

        //----------------------------------
        // mumu plots
        //----------------------------------
            TH1D* m_dv_mumu_cf; //!

            // dimuon vertices
            TH1F* m_dv_mumu_M; //!
            TH1F* m_dv_mumu_R; //!
            TH1F* m_dv_mumu_R_low; //!
            TH1F* m_dv_mumu_z; //!
            TH1F* m_dv_mumu_l; //!
            TH2F* m_dv_mumu_R_M; //! ABCD
            // check vertex fit
            TH1F* m_dv_mumu_chi2_ndof; //!

            // muon kinematics
            TH1F* m_dv_mumu_mu_pt; //!
            TH1F* m_dv_mumu_mu_pt_low; //!
            TH1F* m_dv_mumu_mu_eta; //!
            TH1F* m_dv_mumu_mu_phi; //!
            TH1F* m_dv_mumu_mu_d0; //!
            TH1F* m_dv_mumu_mu_z0; //!

            // cosmic veto
            TH1F* m_dv_mumu_DeltaR; //!
            TH1F* m_dv_mumu_DeltaR_low; //!
            TH1F* m_dv_mumu_Rcos; //!
            TH1F* m_dv_mumu_Rcos_low; //!
            TH2F* m_dv_mumu_DeltaR_Rcos; //!

            // MC matching
            TH1F* m_dv_mumu_M_matched; //!
            TH1F* m_dv_mumu_R_matched; //!
            TH2F* m_dv_mumu_R_M_matched; //!
            TH1F* m_dv_mumu_eta_matched; //!

        //----------------------------------
        // ee plots
        //----------------------------------

            TH1D* m_dv_ee_cf; //!

            // dielectron vertices
            TH1F* m_dv_ee_M; //!
            TH1F* m_dv_ee_R; //!
            TH1F* m_dv_ee_R_low; //!
            TH1F* m_dv_ee_z; //!
            TH1F* m_dv_ee_l; //!
            TH2F* m_dv_ee_R_M; //! ABCD
            // check vertex fit
            TH1F* m_dv_ee_chi2_ndof; //!

            // electron kinematics
            TH1F* m_dv_ee_e_pt; //!
            TH1F* m_dv_ee_e_pt_low; //!
            TH1F* m_dv_ee_e_eta; //!
            TH1F* m_dv_ee_e_phi; //!
            TH1F* m_dv_ee_e_d0; //!
            TH1F* m_dv_ee_e_z0; //!

            // cosmic veto
            TH1F* m_dv_ee_DeltaR; //!
            TH1F* m_dv_ee_DeltaR_low; //!
            TH1F* m_dv_ee_Rcos; //!
            TH1F* m_dv_ee_Rcos_low; //!

            // MC matching
            TH1F* m_dv_ee_M_matched; //!
            TH1F* m_dv_ee_R_matched; //!
            TH2F* m_dv_ee_R_M_matched; //!
            TH1F* m_dv_ee_eta_matched; //!

        //----------------------------------
        // emu plots
        //----------------------------------

            TH1D* m_dv_emu_cf; //!

            // dielectron vertices
            TH1F* m_dv_emu_M; //!
            TH1F* m_dv_emu_R; //!
            TH1F* m_dv_emu_R_low; //!
            TH1F* m_dv_emu_z; //!
            TH1F* m_dv_emu_l; //!
            TH2F* m_dv_emu_R_M; //! ABCD
            // check vertex fit
            TH1F* m_dv_emu_chi2_ndof; //!

            // electron kinematics
            TH1F* m_dv_emu_e_pt; //!
            TH1F* m_dv_emu_e_pt_low; //!
            TH1F* m_dv_emu_e_eta; //!
            TH1F* m_dv_emu_e_phi; //!
            TH1F* m_dv_emu_e_d0; //!
            TH1F* m_dv_emu_e_z0; //!

            TH1F* m_dv_emu_mu_pt; //!
            TH1F* m_dv_emu_mu_pt_low; //!
            TH1F* m_dv_emu_mu_eta; //!
            TH1F* m_dv_emu_mu_phi; //!
            TH1F* m_dv_emu_mu_d0; //!
            TH1F* m_dv_emu_mu_z0; //!

            // cosmic veto
            TH1F* m_dv_emu_DeltaR; //!
            TH1F* m_dv_emu_DeltaR_low; //!
            TH1F* m_dv_emu_Rcos; //!
            TH1F* m_dv_emu_Rcos_low; //!

            // MC matching
            TH1F* m_dv_emu_M_matched; //!
            TH1F* m_dv_emu_R_matched; //!
            TH2F* m_dv_emu_R_M_matched; //!
            TH1F* m_dv_emu_eta_matched; //!

        //----------------------------------
        // lepton + track plots
        //----------------------------------
            TH1D* m_dv_mut_cf; //!
            TH1F* m_dv_mut_M; //!
            TH1F* m_dv_mut_z; //!
            TH1F* m_dv_mut_R; //!
            TH1F* m_dv_mut_l; //!
            TH1F* m_dv_mut_chi2_ndof; //!
            TH1F* m_dv_mut_deltaR; //!

            TH1D* m_dv_et_cf; //!
            TH1F* m_dv_et_M; //!
            TH1F* m_dv_et_z; //!
            TH1F* m_dv_et_R; //!
            TH1F* m_dv_et_l; //!
            TH1F* m_dv_et_chi2_ndof; //!
            TH1F* m_dv_et_deltaR; //!

        //----------------------------------
        // trk-trk plots
        //----------------------------------
            TH1D* m_dv_idid_cf; //!
            TH1F* m_dv_idid_M; //!
            TH1F* m_dv_idid_z; //!
            TH1F* m_dv_idid_R; //!
            TH1F* m_dv_idid_l; //!
            TH1F* m_dv_idid_n_tracks; //!
            TH1F* m_dv_idid_chi2_ndof; //!
            TH1F* m_dv_idid_deltaR; //!

            // track parameters
            TH1F* m_track_p; //!
            TH1F* m_track_pt; //!
            TH1F* m_track_z0; //!
            TH1F* m_track_d0; //!
            TH1F* m_track_phi; //!
            TH1F* m_track_theta; //!
            TH1F* m_track_d0sigma; //!
            TH1F* m_track_z0sigma; //!
            TH1F* m_track_d0_over_d0sigma; //!
            TH1F* m_track_z0_over_z0sigma; //!

            TH1F* m_track_p_wrtSV; //!
            TH1F* m_track_pt_wrtSV; //!
            TH1F* m_track_z0_wrtSV; //!
            TH1F* m_track_d0_wrtSV; //!
            TH1F* m_track_phi_wrtSV; //!
            TH1F* m_track_theta_wrtSV; //!
            TH1F* m_track_d0sigma_wrtSV; //!
            TH1F* m_track_z0sigma_wrtSV; //!
            TH1F* m_track_d0_over_d0sigma_wrtSV; //!
            TH1F* m_track_z0_over_z0sigma_wrtSV; //!

            int n_event_all = 0;
            int n_vrtsec_all = 0;
            int n_dvc_copy = 0;
            int n_dv_all = 0;
            int n_dv_passed_cut = 0;

            // counting selected tracks
            int n_trk_sel = 0;
            int n_mu_sel = 0;
            int n_el_sel = 0;
    
}; 

#endif //> !DISPDILEPATHENA_DISPLACEDDIMUONANALYSISALG_H
