#ifndef _SHOCK3D_FETCH_GREEN_V2_H_
#define _SHOCK3D_FETCH_GREEN_V2_H_

#include "sheet_flow_mbuck_mres.h"

void
FetchGreenViaBlueFull(InputData *pInData, ShockScaffold *pScaf,
          ShockCurve *pShockCurves,
          short *pisLookUp, Buckets *pBucksXYZ,
          Pt3dCoord *pGeneCoord, Voxel *pVoxels,
          Dimension *pDimMBuck, Dimension *pDimAll);
int
FetchGreenViaBlue(InputData *pInData, ShockScaffold *pScaf,
      ShockCurve *pShockCurves,
      short *pisLookUp, Buckets *pBucksXYZ,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimMBuck, Dimension *pDimAll);

int
SetValidCurvesExt(ShockCurve *pShocks, ShockScaffold *pScaffold,
      InputData *pInData, short *pisLookUp,
      Buckets *pBucksXYZ, Dimension *pDim);


#endif /* _SHOCK3D_FETCH_GREEN_V2_H_ */
