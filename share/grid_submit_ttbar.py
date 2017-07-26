#!/bin/sh
pathena DispDilepAthena.py \
--mergeOutput \
--nFilesPerJob 3 \
--destSE BNL-OSG2_SCRATCHDISK \
--inDS user.sche.mc15_13TeV.410252.Sherpa_ttbar_dilepton_MEPS_NLO.recon.DAOD_SUSY15.e5450_s2726_r8788_p2949.lepfilter.r5_EXT0/ \
--outDS user.sche.mc15_13TeV.410252.Sherpa_ttbar_dilepton_MEPS_NLO.recon.DAOD_SUSY15.e5450_s2726_r8788.FBE.massveto0.r32
