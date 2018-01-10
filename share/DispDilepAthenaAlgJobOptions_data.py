#!/usr/bin/env python
# --------------------------------------------------------------
# Example jobOption.py file to feed athena.py using algorithms
# and tools/cuts of the DV_xAODAnalysis package
# --------------------------------------------------------------

# read athena pool xAOD files
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.GlobalFlags import globalflags
import glob   

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if not "InputFiles" in dir():
    InputFiles = []

    # mumu test
    #InputFiles = glob.glob('/n/atlas05/userdata/sche/20.7.8.7.SUSY15/output/mumu_noskim/DAOD_SUSY15.mumu.*.root')
    
    # emu test
    #InputFiles = glob.glob('/n/atlas05/userdata/sche/20.7.8.7.SUSY15/output/emu_noskim/DAOD_SUSY15.emu.*.root')
    
    # ee test
    #InputFiles = glob.glob('/n/atlas05/userdata/sche/20.7.8.7.SUSY15/output/ee_noskim/DAOD_SUSY15.ee.*.root')

    # ttbar background
    #InputFiles = glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/background/user.sche.mc15_13TeV.410252.Sherpa_ttbar_dilepton_MEPS_NLO.recon.DAOD_SUSY15.e5450_s2726_r8788_p2949.lepfilter.r4_EXT0/user.sche.11532947.EXT0._00000*.DAOD_SUSY15.pool.root')

    # data
    InputFiles += glob.glob('/n/atlas05/userdata/sche/data/DAOD_SUSY15_FILTERED/user.sche.data16_13TeV.physics_Main.DAOD_SUSY15.r8669_p2950.Lep_Filtered.r10_EXT0/*.root*')

if not "athenaCommonFlags.EvtMax" in dir():
    athenaCommonFlags.EvtMax = -1
    #athenaCommonFlags.EvtMax = 5000

# Set up the needed RecEx flags:
athenaCommonFlags.FilesInput.set_Value_and_Lock( InputFiles )

# skip event
#ServiceMgr.EventSelector.SkipEvents = 0

# uncomment for data
globalflags.DataSource.set_Value_and_Lock('data')

#--------------------------------------------
# DAOD_SUSY15 data
#--------------------------------------------
# all 2016 data
#svcMgr.EventSelector.InputCollections = glob.glob( "/n/atlas05/userdata/sche/data/DAOD_SUSY15/*/*.root.1" )

# trigger filtered
#svcMgr.EventSelector.InputCollections += glob.glob("/n/atlas05/userdata/sche/data/DAOD_SUSY15_FILTERED/user.sche.data16_13TeV.physics_Main.DAOD_RPVLL.r8669.trigfilter.r1_EXT0/*.root")


#--------------------------------------------
# Signal MC samples
#--------------------------------------------

# ee test
#svcMgr.EventSelector.InputCollections = glob.glob('/n/atlas05/userdata/sche/20.7.8.7.SUSY15/output/DAOD_SUSY15.DAOD_SUSY15.ee.pool.root')
# emu test
#svcMgr.EventSelector.InputCollections = glob.glob('/n/atlas05/userdata/sche/20.7.8.7.SUSY15/output/DAOD_SUSY15.DAOD_SUSY15.emu.pool.root')

# SUSY15, official
#svcMgr.EventSelector.InputCollections = glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301911.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m250t100.recon.DAOD_RPVLL.e4125_s2698_r8788.r12_EXT0/*.root')
#svcMgr.EventSelector.InputCollections += glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301912.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m250t250.recon.DAOD_RPVLL.e4125_s2698_r8788.r12_EXT0/*.root')
#svcMgr.EventSelector.InputCollections += glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301913.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m250t500.recon.DAOD_RPVLL.e4125_s2698_r8788.r14_EXT0/*.root')
#svcMgr.EventSelector.InputCollections += glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301914.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m500t100.recon.DAOD_RPVLL.e4125_s2698_r8788.r12_EXT0/*.root')
#svcMgr.EventSelector.InputCollections += glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301915.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m500t250.recon.DAOD_RPVLL.e4125_s2698_r8788.r11_EXT0/*.root')
#svcMgr.EventSelector.InputCollections += glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301916.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m500t500.recon.DAOD_RPVLL.e4125_s2698_r8788.r12_EXT0/*.root')
#svcMgr.EventSelector.InputCollections += glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301917.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m1000t100.recon.DAOD_RPVLL.e4125_s2698_r8788.r13_EXT0/*.root')
#svcMgr.EventSelector.InputCollections += glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301918.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m1000t250.recon.DAOD_RPVLL.e4125_s2698_r8788.r14_EXT0/*.root')
#svcMgr.EventSelector.InputCollections += glob.glob( '/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/zprimemumu/user.sche.mc15_13TeV.301919.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m1000t500.recon.DAOD_RPVLL.e4125_s2698_r8788.r13_EXT0/*.root')


#--------------------------------------------
# Background samples
#--------------------------------------------

# diboson
#svcMgr.EventSelector.InputCollections = glob.glob("/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/background/user.sche.mc15_13TeV.361064.Sherpa_CT10_lllvSFMinus.recon.DAOD_SUSY15.e3836_s2608_s2183_r8788.r5_EXT0/*.root")
#svcMgr.EventSelector.InputCollections = glob.glob("/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/background/user.sche.mc15_13TeV.361066.Sherpa_CT10_lllvSFPlus.recon.DAOD_SUSY15.e3836_s2608_s2183_r8788.r1_EXT0/*.root")
#svcMgr.EventSelector.InputCollections = glob.glob("/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/background/user.sche.mc15_13TeV.361068.Sherpa_CT10_llvv.recon.DAOD_SUSY15.e3836_s2608_s2183_r8788.r1_EXT0/*.root")
#svcMgr.EventSelector.InputCollections = glob.glob("/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/background/user.sche.mc15_13TeV.361063.Sherpa_CT10_llll.recon.DAOD_SUSY15.e3836_s2608_s2183_r8788.r1_EXT0/*.root")

# ttbar
#svcMgr.EventSelector.InputCollections = glob.glob("/n/atlas05/userdata/sche/MC15/DAOD_SUSY15/background/user.sche.mc15_13TeV.410252.Sherpa_221_NNPDF30NNLO_ttbar_dilepton_MEPS_NLO.recon.DAOD_SUSY15.e5450_s2726_r8788.r1_EXT0/*.root")

# handler for the main sequence
algseq = CfgMgr.AthSequencer("AthAlgSeq")


#--------------------------------------------
# Algorithms to run
#--------------------------------------------
#from DisplacedDimuonAnalysis.DisplacedDimuonAnalysisConf import DisplacedDimuonAnalysisAlg

# Data and MC
algseq += CfgMgr.DisplacedDimuonAnalysisAlg()
algseq += CfgMgr.FlipBkgEst()

#---------------------------------------------------------------
# including the tools /cuts needed by the algorithm, and configuring them
#---------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
#svcMgr.MessageSvc.OutputLevel = DEBUG
svcMgr.MessageSvc.defaultLimit = 9999

# histogram output service
if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["DV DATAFILE='output.root' OPT='RECREATE'"]

# DVCuts tool
ToolSvc += CfgMgr.DDL__DVCuts("DiLepBaseCuts")
ToolSvc.DiLepBaseCuts.distMin  = 2. # mm
#ToolSvc.DiLepBaseCuts.LowMass  = 4000. # MeV
#ToolSvc.DiLepBaseCuts.LowMass  = 3000. # MeV
ToolSvc.DiLepBaseCuts.DisabledModuleMapFile = "DisabledModuleMap_Run2_v2.root"
ToolSvc.DiLepBaseCuts.MaterialMapFile = "materialMap3D_Run2_v2.1.1.root"

# EventCuts
ToolSvc += CfgMgr.DDL__EventCuts("DiLepEventCuts")
ToolSvc.DiLepEventCuts.TriggerNames = [ "HLT_mu60_0eta105_msonly",
                                        "HLT_g140_loose",
                                        "HLT_2g50_loose",
                                        #"HLT_2g60_loose_L12EM15VH"
                                        ]

# GoodRunsListSelectorTool
vecStringGRL = 'data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns_DAOD_RPVLL_r8669.xml'
ToolSvc += CfgMgr.GoodRunsListSelectionTool("GRLTool")
ToolSvc.GRLTool.GoodRunsListVec=[vecStringGRL]
ToolSvc.GRLTool.OutputLevel = INFO

# Muon selection tool
ToolSvc += CfgMgr.CP__MuonSelectionTool("MuonSelectionTool")
ToolSvc.MuonSelectionTool.MaxEta = 2.7
# 0 = tight, 1 = medium, 2 = loose, 3 = very loose
ToolSvc.MuonSelectionTool.MuQuality = 2
ToolSvc.MuonSelectionTool.OutputLevel = INFO

# Muon correction tool
ToolSvc += CfgMgr.CP__MuonCalibrationAndSmearingTool("MuonCorrectionTool")
ToolSvc.MuonCorrectionTool.Year = "Data16"
ToolSvc.MuonCorrectionTool.Release = "Recs2016_15_07"
#ToolSvc.MuonCorrectionTool.SagittaCorr = True
ToolSvc.MuonCorrectionTool.OutputLevel = INFO

# Electron Likelihood tool
confDir = "ElectronPhotonSelectorTools/offline/mc15_20160512/"
ToolSvc += CfgMgr.AsgElectronLikelihoodTool("ElectronLikelihoodTool")
ToolSvc.ElectronLikelihoodTool.ConfigFile= confDir+"ElectronLikelihoodLooseOfflineConfig2016_Smooth_NoD0.conf"
#ToolSvc.ElectronLikelihoodTool.WorkingPoint= "MediumLHElectron"
#ToolSvc.ElectronLikelihoodTool.WorkingPoint= "TightLHElectron"
ToolSvc.ElectronLikelihoodTool.OutputLevel = INFO

# Trigger decision tool
# set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.OutputLevel = INFO
#ToolSvc.TrigDecisionTool.OutputLevel = DEBUG

# set up DispVertexer
ToolSvc += CfgMgr.DDL__DispVertexer("DispVertexer",
                                    TrPtMin             = 1000.,
                                    TrChi2Max           = 50.,
                                    TrD0Min             = 2,
                                    TrD0Max             = 300.,
                                    TrZ0Min             = 0,
                                    TrZ0Max             = 1500.,
                                    TrMinSctHits        = 2,
                                    TrMaxSharedHits     = 2,
                                    TrMinPixelHitsNoTRT = 2,
                                    VxChi2Max           = 5.)

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
AtlasExtrapolator = AtlasExtrapolator()
ToolSvc += AtlasExtrapolator

# set up vertexer
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
ToolSvc += Trk__TrkVKalVrtFitter(name             = "CombBkgVxFitter",
                                 Extrapolator     = ToolSvc.AtlasExtrapolator,
                                 IterationNumber  = 30,
                                 AtlasMagFieldSvc = "AtlasFieldSvc")


from RecExConfig.RecFlags import rec
rec.AutoConfiguration = [ "everything" ]
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteESD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doESD.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doDPD.set_Value_and_Lock( False )
rec.doHist.set_Value_and_Lock( False )
rec.doPerfMon.set_Value_and_Lock( False )
rec.doForwardDet.set_Value_and_Lock( False )

# Let RecExCommon set everything up:
include( "RecExCommon/RecExCommon_topOptions.py" )

