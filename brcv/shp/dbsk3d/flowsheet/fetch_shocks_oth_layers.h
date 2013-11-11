#ifndef _SHOCK3D_FETCH_BLUE_EXT_OTH_H_
#define _SHOCK3D_FETCH_BLUE_EXT_OTH_H_

#include "sheet_flow_mbuck_mres.h"

void
FetchShocksExtOth(InputData *pInData, ShockScaffold *pScaf,
      short *pisLookUp, Buckets *pBucksXYZ,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimMBuck, Dimension *pDimAll);

void
FetchShocksExtOthItera(InputData *pInData, ShockScaffold *pScaf,
           short *pisLookUp, Buckets *pBucksXYZ,
           Pt3dCoord *pGeneCoord, Voxel *pVoxels,
           Dimension *pDimMBuck, Dimension *pDimAll);

#endif /* _SHOCK3D_FETCH_BLUE_EXT_OTH_H_ */
