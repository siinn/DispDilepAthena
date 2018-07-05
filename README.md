# DispDilepAthena
Full Athena version of displaced dilepton analysis. The package contains the algorithm for random-crossing background using the track-flipping method. 

## Instruction

You need T3 clusters or machine with ATLAS software in order to run this package. Here is a quick start guide for running the algorithm on data.
~~~~
  setupATLAS
  asetup AtlasDerivation, 20.7.9.3, here
  athena DispDilepAthenaAlgJobOptions_data.py
~~~~

In the jobOption, you can change the local input file. If you need to run on grid, please see the example in 
~~~~
grid_submit_data.py
~~~~

For more information, please contact me or OSU ATLAS group.
