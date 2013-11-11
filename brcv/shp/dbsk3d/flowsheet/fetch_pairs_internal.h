#ifndef _SHOCK3D_FETCH_BLUE_INT_H_
#define _SHOCK3D_FETCH_BLUE_INT_H_

#include "sheet_flow_mbuck_mres.h"

///void
///FetchBlueShocksIntSub(InputData *pInData, ShockSheet *pShocks,
///          short *pisLookUp, Buckets *pBucksXYZ,
///          Dimension *pDim);

void
FetchBlueShocksIntAll(InputData *pInData, ShockSheet *pShocks,
          Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
          Voxel *pVoxels, Dimension *pDimMBuck,
          Dimension *pDimAll);

#endif /* _SHOCK3D_FETCH_BLUE_INT_H_ */
