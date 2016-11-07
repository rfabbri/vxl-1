#ifndef _SHOCK3D_FETCH_BLUE_EXT_FST_H_
#define _SHOCK3D_FETCH_BLUE_EXT_FST_H_

#include "sheet_flow_mbuck_mres.h"

void
FetchBlueShocksExtFst(InputData *pInData, ShockScaffold *pScaffold,
          short *pisLookUp, Buckets *pBucksXYZ,
          Pt3dCoord *pGeneCoord, Voxel *pVoxels,
          Dimension *pDimMBuck, Dimension *pDimAll);

void
FetchShocksExtFst(InputData *pInData, ShockScaffold *pScaf,
      short *pisLookUp, Buckets *pBucksXYZ,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimMBuck, Dimension *pDimAll);

#endif /* _SHOCK3D_FETCH_BLUE_EXT_FST_H_ */
