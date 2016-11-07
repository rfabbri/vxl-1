#ifndef _SHOCK3D_FETCH_RED_H_
#define _SHOCK3D_FETCH_RED_H_

#include "sheet_flow_mbuck_mres.h"

void
FetchRedShocks(InputData *pInData, ShockScaffold *pScaffold,
         ShockVertex *pShockVertices, short *pisLookUp,
         Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
         Voxel *pVoxels, Dimension *pDimMBuck,
         Dimension *pDimAll);
int
SetValidVertices(ShockVertex *pShocks, ShockScaffold *pScaffold,
     InputData *pInData, short *pisLookUp,
     Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
     Voxel *pVoxels, Dimension *pDimMBuck,
     Dimension *pDimAll);

/*
#define PrintMesgGreenWith3Reds(iGreen,iRedA,iRedB,iRedC,fX,fY,fZ) \
  fprintf(stderr, \
    " ** Green curve %d already has 2 red nodes (%d , %d)\n", \
    (iGreen), (iRedA), (iRedB)); \
  pOldRed = (pScaffold->pRed)+(iRedA); \
  fprintf(stderr, "\twith coord: (%f, %f, %f)\n", \
    pOldRed->CircumSphere.fPosX, \
    pOldRed->CircumSphere.fPosY, \
    pOldRed->CircumSphere.fPosZ); \
  pOldRed = (pScaffold->pRed)+(iRedB); \
  fprintf(stderr, "\tand: (%f, %f, %f)\n", \
    pOldRed->CircumSphere.fPosX, \
    pOldRed->CircumSphere.fPosY, \
    pOldRed->CircumSphere.fPosZ); \
  fprintf(stderr, \
    "\twhile new red node %d has coord: (%f, %f, %f)\n", \
    (iRed), (fX), (fY), (fZ)); \*/


#endif /* _SHOCK3D_FETCH_RED_H_ */
