#ifndef DISPDILEPATHENA_VERTEXINGSYSTEMATICS_H
#define DISPDILEPATHENA_VERTEXINGSYSTEMATICS_H 1

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

// track fitter
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

class VertexingSystematics: public ::AthAnalysisAlgorithm { 
    public: 
        VertexingSystematics( const std::string& name, ISvcLocator* pSvcLocator );
        virtual ~VertexingSystematics(); 
     
        virtual StatusCode  initialize();
        virtual StatusCode  execute();
        virtual StatusCode  finalize();

        virtual bool PassCosmicVeto_R_CR(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);
        virtual bool PassCosmicVeto_DeltaR(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2);
        virtual bool PassKsCandidate(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv);

        virtual void PerformFit(xAOD::TrackParticle& tr1, xAOD::TrackParticle& tr2, const Amg::Vector3D& pv, std::string channel);

        // flag to check MC
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
        ToolHandle<Trk::ITrkVKalVrtFitter> m_fitSvc; //!

        // DV mass accessor
        SG::AuxElement::ConstAccessor<float> m_accMass;
        SG::AuxElement::Accessor<std::shared_ptr<xAOD::ElectronContainer>> m_accEl;
        SG::AuxElement::Accessor<std::shared_ptr<xAOD::MuonContainer>> m_accMu;
        SG::AuxElement::Accessor<TLorentzVector> m_acc_p4;

        // histograms
        TH1D* m_idid_cf; //!
        TH1F* m_idid_R; //!
        TH1F* m_idid_z; //!
        TH1F* m_idid_M; //!
        TH1F* m_idid_l; //!
        TH1F* m_idid_chi2_ndof; //!
        TH1F* m_idid_deltaR; //!

        // ratio plot. divided by number of Ks after run by root macro
        TH1F* m_idid_R_ratio; //!

        // Ks candidate
        TH1F* m_ks_cand_M; //!
        TH1D* m_ks_cand_cf; //!


}; 

#endif //> !DISPDILEPATHENA_VERTEXINGSYSTEMATICS_H
