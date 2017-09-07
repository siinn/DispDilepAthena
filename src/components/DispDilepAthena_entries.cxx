
#include "GaudiKernel/DeclareFactoryEntries.h"

//#include "../DispDilepAthenaAlg.h"
//DECLARE_ALGORITHM_FACTORY( DispDilepAthenaAlg )

#include "../FlipBkgEst.h"
DECLARE_ALGORITHM_FACTORY( FlipBkgEst )

#include "../DisplacedDimuonAnalysisAlg.h"
DECLARE_ALGORITHM_FACTORY( DisplacedDimuonAnalysisAlg )

#include "../DispDilepAthena/DVUtils.h"
#include "../DispDilepAthena/LeptonSelectionTools.h"
#include "../DispDilepAthena/CosmicTools.h"
DECLARE_TOOL_FACTORY( DVUtils )
DECLARE_TOOL_FACTORY( LeptonSelectionTools )
DECLARE_TOOL_FACTORY( CosmicTools )


#include "../VertexingSystematics.h"
DECLARE_ALGORITHM_FACTORY( VertexingSystematics )

DECLARE_FACTORY_ENTRIES( DispDilepAthena ) 
{
  DECLARE_ALGORITHM( VertexingSystematics );
  //DECLARE_ALGORITHM( DispDilepAthenaAlg );
  DECLARE_ALGORITHM( FlipBkgEst );
  DECLARE_ALGORITHM( DisplacedDimuonAnalysisAlg )
  DECLARE_TOOL( DVUtils )
  DECLARE_TOOL( LeptonSelectionTools )
  DECLARE_TOOL( CosmicTools )
}
