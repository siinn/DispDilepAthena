#!/bin/sh
pathena FlibBkgEst_jobOption_data.py \
--mergeOutput \
--destSE BNL-OSG2_USERDISK \
--nFilesPerJob 10 \
--site ANALY_TOKYO \
--inDS user.sche:user.sche.data16_13TeV.physics_Main.DAOD_SUSY15.r8669_p2950.Lep_Filtered.r10_EXT0.139633323 \
--outDS user.sche.data16_13TeV.physics_Main.DAOD_SUSY15.r8669.p2950.Lep_Filtered.FBE.test.r12
