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


class FlipBkgEst: public ::AthAnalysisAlgorithm { 
    public: 
        FlipBkgEst( const std::string& name, ISvcLocator* pSvcLocator );
        virtual ~FlipBkgEst(); 
        
        virtual StatusCode  initialize();
        virtual StatusCode  execute();
        virtual StatusCode  finalize();
        
        virtual StatusCode beginInputFile();

        virtual bool PassCosmicVeto_R_CR(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);
        virtual bool PassCosmicVeto_DeltaR(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);

        virtual void PerformFit(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv, std::string channel);
        virtual void PerformFit_flip(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv, std::string channel);

        virtual const xAOD::TruthVertex* getClosestTruthVertex(const xAOD::Vertex *dv);
        bool isMC;
    
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
        ToolHandle<DDL::IOverlapRemoval> m_or; //!
        ToolHandle<DDL::IDispVertexer> m_vertexer; //!
        ToolHandle<Trig::IMatchingTool> m_tmt; //!
        ToolHandle<DDL::ITrigMatch> m_trig; //!

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

        TH1D* m_mumu_cf_input; //!
        TH1D* m_mumu_cf_noflip; //!
        TH1D* m_mumu_cf_flip; //!

        TH1D* m_ee_cf_input; //!
        TH1D* m_ee_cf_noflip; //!
        TH1D* m_ee_cf_flip; //!

        TH1D* m_emu_cf_input; //!
        TH1D* m_emu_cf_noflip; //!
        TH1D* m_emu_cf_flip; //!

        TH1D* m_idid_cf_input; //!
        TH1D* m_idid_cf_noflip; //!
        TH1D* m_idid_cf_flip; //!

        TH1D* m_mut_cf_input; //!
        TH1D* m_mut_cf_noflip; //!
        TH1D* m_mut_cf_flip; //!

        TH1D* m_et_cf_input; //!
        TH1D* m_et_cf_noflip; //!
        TH1D* m_et_cf_flip; //!

        // vertex distribution
        TH1F* m_mumu_noflip_R; //!
        TH1F* m_mumu_noflip_z; //!
        TH1F* m_mumu_flip_R; //!
        TH1F* m_mumu_flip_z; //!

        TH1F* m_ee_noflip_R; //!
        TH1F* m_ee_noflip_z; //!
        TH1F* m_ee_flip_R; //!
        TH1F* m_ee_flip_z; //!

        TH1F* m_emu_noflip_R; //!
        TH1F* m_emu_noflip_z; //!
        TH1F* m_emu_flip_R; //!
        TH1F* m_emu_flip_z; //!

        TH1F* m_idid_noflip_R; //!
        TH1F* m_idid_noflip_z; //!
        TH1F* m_idid_flip_R; //!
        TH1F* m_idid_flip_z; //!
        TH1F* m_idid_flip_M; //!
        TH1F* m_idid_flip_l; //!
        TH1F* m_idid_flip_chi2_ndof; //!
        TH1F* m_idid_flip_deltaR; //!

        TH1F* m_mut_noflip_R; //!
        TH1F* m_mut_noflip_z; //!
        TH1F* m_mut_flip_R; //!
        TH1F* m_mut_flip_z; //!
        TH1F* m_mut_flip_M; //!
        TH1F* m_mut_flip_l; //!
        TH1F* m_mut_flip_chi2_ndof; //!
        TH1F* m_mut_flip_deltaR; //!

        TH1F* m_et_noflip_R; //!
        TH1F* m_et_noflip_z; //!
        TH1F* m_et_flip_R; //!
        TH1F* m_et_flip_z; //!
        TH1F* m_et_flip_M; //!
        TH1F* m_et_flip_l; //!
        TH1F* m_et_flip_chi2_ndof; //!
        TH1F* m_et_flip_deltaR; //!


        // counting pairs
        int n_tt_pair = 0;
        int n_trk_sel = 0;
}; 

#endif //> !DISPDILEPATHENA_FLIPBKGEST_H
