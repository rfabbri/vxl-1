#ifndef _SHOCK3D_FETCH_RED_V2_H_
#define _SHOCK3D_FETCH_RED_V2_H_

#include "sheet_flow_mbuck_mres.h"

int
FetchRedViaGreenFull(InputData *pInData, ShockScaffold *pScaf,
         ShockVertex *pShockVertices, short *pisLookUp,
         Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
         Voxel *pVoxels, Dimension *pDimMBuck,
         Dimension *pDimAll);
int
FetchRedViaGreen(InputData *pInData, ShockScaffold *pScaf,
     ShockVertex *pShockVertices, short *pisLookUp,
     Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
     Voxel *pVoxels, Dimension *pDimMBuck,
     Dimension *pDimAll);

/*
#define PrintMesgGreenWith3Reds(iGreen,iRedA,iRedB,iRedC,fX,fY,fZ,fDist) \
    pGreenTmp = (pScaf->pGreen)+(iGreen); \
    fprintf(stderr, \
      " ** Green curve %d (Type %d , Init %d): 2 Reds (%d , %d)\n", \
      (iGreen), pGreenTmp->isType, pGreenTmp->isInitType, \
      (iRedA), (iRedB)); \
    fprintf(stderr, "\tGreen: (%f, %f, %f), Radius = %f\n", \
      pGreenTmp->CircumTri.fPosX, \
      pGreenTmp->CircumTri.fPosY, \
      pGreenTmp->CircumTri.fPosZ, \
      (float) sqrt((double) pGreenTmp->fDist2GeneSq)); \
    pOldRed = (pScaf->pRed)+(iRedA); \
    fprintf(stderr, "\tRed1: (%f, %f, %f), Radius = %f\n", \
      pOldRed->CircumSphere.fPosX, \
      pOldRed->CircumSphere.fPosY, \
      pOldRed->CircumSphere.fPosZ, \
      (float) sqrt((double) pOldRed->fDist2GeneSq)); \
    pOldRed = (pScaf->pRed)+(iRedB); \
    fprintf(stderr, "\tRed2: (%f, %f, %f), Radius = %f\n", \
      pOldRed->CircumSphere.fPosX, \
      pOldRed->CircumSphere.fPosY, \
      pOldRed->CircumSphere.fPosZ, \
      (float) sqrt((double) pOldRed->fDist2GeneSq)); \
    fprintf(stderr, \
      "\tNew red %d: (%f, %f, %f), Radius = %f\n", \
      (iRed), (fX), (fY), (fZ), \
      (float) sqrt((double) (fDist))); \
        */

#endif /* _SHOCK3D_FETCH_RED_V2_H_ */
