#!/bin/sh
pathena DispDilepAthenaAlgJobOptions_data.py \
--mergeOutput \
--destSE BNL-OSG2_SCRATCHDISK \
--nFilesPerJob 10 \
--forceStaged \
--inDS user.sche.data16_13TeV.physics_Main.DAOD_SUSY15.r8669_p2950.Lep_Filtered.r10_EXT0/ \
--outDS user.sche.data16_13TeV.physics_Main.DAOD_SUSY15.r8669.p2950.Lep_Filtered.TF.data.z0d0Refitting.r73
