#ifndef _BLUE_UTILS_H_
#define _BLUE_UTILS_H_

#include "sheet_flow_mbuck_mres.h"

///void
///SetSheetsSub(ShockSheet *pShocks, ShockScaffold *pScaffold,
       ///InputData *pInData, Dimension *pDim);
int
SetValidSheets(ShockSheet *pShocks, ShockScaffold *pScaffold,
         InputData *pInData, Dimension *pDim);
void
SetBluePassive(ShockScaffold *pScaffold);

#if FALSE
int
ValidBlueShock(ShockSheet *pBlue, InputData *pInData,
         short *pisLookUp, Buckets *pBucksXYZ,
         int *piNgbInBall, Dimension *pDim);
#endif

int
ValidBlueShockViaVoxels(ShockData *pShockData, InputData *pInData,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimAll);

#endif /* _BLUE_UTILS_H_ */
