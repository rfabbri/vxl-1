#ifndef _SHOCK3D_FETCH_GREEN_H_
#define _SHOCK3D_FETCH_GREEN_H_

#include "sheet_flow_mbuck_mres.h"

void
FetchGreenShocksInt(InputData *pInData, ShockScaffold *pScaf,
        ShockCurve *pShockCurves, short *pisLookUp,
        Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
        Voxel *pVoxels, Dimension *pDimMBuck,
        Dimension *pDimAll);
void
FetchGreenShocksFstLayer(InputData *pInData, ShockScaffold *pScaf,
       ShockCurve *pShockCurves, short *pisLookUp,
       Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
       Voxel *pVoxels, Dimension *pDimMBuck,
       Dimension *pDimAll);
int
SetValidCurvesInt(ShockCurve *pShocks, ShockScaffold *pScaf,
      InputData *pInData, short *pisLookUp,
      Buckets *pBucksXYZ, Dimension *pDimMBuck);

int
SetValidCurvesFstLayer(ShockCurve *pShocks, ShockScaffold *pScaf,
           InputData *pInData, short *pisLookUp,
           Buckets *pBucksXYZ, Dimension *pDimMBuck);

void
SetGreenPassive(ShockScaffold *pScaf);


#endif /* _SHOCK3D_FETCH_GREEN_H_ */
