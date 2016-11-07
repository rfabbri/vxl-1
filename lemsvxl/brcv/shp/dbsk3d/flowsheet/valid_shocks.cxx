/*------------------------------------------------------------*\
 * File: valid_shocks.c
 *
 * Functions:
 *  ValidGreenShockViaVoxels
 *  ValidRedShockViaVoxels
 *
 * History:
 *  #0: Aug. 2003, by F.Leymarie
 *
\*------------------------------------------------------------*/

#include "valid_shocks.h"

/*------------------------------------------------------------*\
 * Function: ValidGreenShockViaVoxels
 *
 * Usage: Validates a candidate Green shock 
 *
\*------------------------------------------------------------*/

int
ValidGreenShockViaVoxels(ShockData *pShockData, InputData *pInData,
       Pt3dCoord *pGeneCoord, Voxel *pVoxels,
       Dimension *pDimAll)
{
  /*   char    *pcFName = "ValidGreenShockViaVoxels"; */
  register int  i, j, k, l;
  int    iXdim,iYdim,iZdim, iSliceSize, iVox, iNumGenesInVox;
  int    iGeneA,iGeneAb, iGeneB,iGeneBb, iGeneC,iGeneCb;
  int    iMinX,iMinY,iMinZ, iMaxX,iMaxY,iMaxZ, iGene;
  int    *piGene;
  static float  fEpsilon = (float) D_EPSILON;
  float    fPosXs,fPosYs,fPosZs, fPx,fPy,fPz, fDSq,fDSqNew, fRad;
  float    fDistMin,fDistMax, fDistMinAbs,fDistMaxAbs;
  InputData  *pGeneA, *pGeneB, *pGeneC;
  Voxel    *pVox;
  Pt3dCoord  *pCoord;

  /* Preliminaries */

  iXdim = pDimAll->iXdim;
  iYdim = pDimAll->iYdim;
  iZdim = pDimAll->iZdim;
  iSliceSize = pDimAll->iSliceSize;

  iGeneA  = pShockData->iGene[0];
  pGeneA  = pInData+iGeneA;
  iGeneAb = pGeneA->iID;
  iGeneB  = pShockData->iGene[1];
  pGeneB  = pInData+iGeneB;
  iGeneBb = pGeneB->iID;
  iGeneC  = pShockData->iGene[2];
  pGeneC  = pInData+iGeneC;
  iGeneCb = pGeneC->iID;

  fPosXs = pShockData->ShockCoord.fPosX;
  fPosYs = pShockData->ShockCoord.fPosY;
  fPosZs = pShockData->ShockCoord.fPosZ;
  fDSq = pShockData->fDist2GeneSq;

  /* Compute the minimum bounding box limits */
  fRad = (float) sqrt((double) fDSq);
  iMinX = (int) floor((double) (fPosXs - fRad));
  if(iMinX < 1) iMinX = 1;
  iMaxX = (int) ceil((double) (fPosXs + fRad));
  if(iMaxX >= iXdim) iMaxX = iXdim - 2;
  iMinY = (int) floor((double) (fPosYs - fRad));
  if(iMinY < 1) iMinY = 1;
  iMaxY = (int) ceil((double) (fPosYs + fRad));
  if(iMaxY >= iYdim) iMaxY = iYdim - 2;
  iMinZ = (int) floor((double) (fPosZs - fRad));
  if(iMinZ < 1) iMinZ = 1;
  iMaxZ = (int) ceil((double) (fPosZs + fRad));
  if(iMaxZ >= iZdim) iMaxZ = iZdim - 2;

  /* ------------------------------------------------------ *\
   * Walk through voxels making up the minimum bounding box *
   * containing the contact sphere. We expect data to be    *
   * located near the sphere surface in general.
  \* ------------------------------------------------------ */

  iMaxX++; iMaxY++; iMaxZ++;
  for(i = iMinZ; i < iMaxZ; i++) {
    for(j = iMinY; j < iMaxY; j++) {
      for(k = iMinX; k < iMaxX; k++) {
  
  iVox = i * iSliceSize + j * iXdim + k;
  pVox = pVoxels+iVox;
  iNumGenesInVox = pVox->isNumGenes;
  if(iNumGenesInVox > 0) {

    /* ----- Make sure the voxel intersects the ball ----- *\
     * Check faces: Ball crosses a face? or is Surrounded? *
     * Compute Distances to Bucket Face's Planes & test    *
    \* Left-Right conditions         */

    fDistMin = i - fPosZs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = (i+1) - fPosZs;
    fDistMaxAbs = fabs(fDistMax);
    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket Z-faces intersect or surround Ball */

    fDistMin = j - fPosYs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = (j+1) - fPosYs;
    fDistMaxAbs = fabs(fDistMax);
    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket Y-faces intersect or surround Ball */

    fDistMin = k - fPosXs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = (k+1) - fPosXs;
    fDistMaxAbs = fabs(fDistMax);
    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket X-faces intersect or surround Ball */

    /* iIntersect++; */
    /* --- OK : Voxel covers Ball at least partially --- */
    /* Check w/r the genes in this voxel */
    piGene = pVox->piGenes;
    for(l = 0; l < iNumGenesInVox; l++) {
      iGene = *piGene++;
      if(iGene == iGeneAb || iGene == iGeneBb || iGene == iGeneCb)
        continue;

      pCoord = pGeneCoord+iGene;
      fPx = pCoord->fPosX - fPosXs;
      fPy = pCoord->fPosY - fPosYs;
      fPz = pCoord->fPosZ - fPosZs;
      fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
      fDSqNew += fEpsilon;

      if(fDSq > fDSqNew) { /* Invalid shock */
        return(FALSE); /* Part of SS only: go back */  
        /* break; Get out of For(i) loop */
      }
    } /* Next gene in voxel: l++ */

  } /* End of If(iNumGenesInVox > 0) */

      } /* Next X integer: k++ */
    } /* Next Y integer: j++ */
  } /* Next Z integer: i++ */

  /* Ball is entirely covered: Shock is valid */
  return(TRUE);

}

/*------------------------------------------------------------*\
 * Function: ValidRedShockViaVoxels
 *
 * Usage: Validates a candidate Red shock vertex
 *
\*------------------------------------------------------------*/

int
ValidRedShockViaVoxels(ShockData *pShockData, InputData *pInData,
           Pt3dCoord *pGeneCoord, Voxel *pVoxels,
           Dimension *pDimAll)
{
  /* char    *pcFName = "ValidRedShockViaVoxels"; */
  register int  i, j, k, l;
  int    iXdim,iYdim,iZdim, iSliceSize, iVox, iNumGenesInVox;
  int    iGeneA,iGeneAb, iGeneB,iGeneBb, iGeneC,iGeneCb;
  int    iGeneD,iGeneDb, iGene;
  int    iMinX,iMinY,iMinZ, iMaxX,iMaxY,iMaxZ;
  int    *piGene;
  static float  fEpsilon = (float) D_EPSILON;
  float    fPosXs,fPosYs,fPosZs, fPx,fPy,fPz, fDSq,fDSqNew, fRad;
  float    fDistMin,fDistMax, fDistMinAbs,fDistMaxAbs;
  InputData  *pGeneA, *pGeneB, *pGeneC, *pGeneD;
  Voxel    *pVox;
  Pt3dCoord  *pCoord;

  /* Preliminaries */

  iXdim = pDimAll->iXdim;
  iYdim = pDimAll->iYdim;
  iZdim = pDimAll->iZdim;
  iSliceSize = pDimAll->iSliceSize;

  iGeneA  = pShockData->iGene[0];
  pGeneA  = pInData+iGeneA;
  iGeneAb = pGeneA->iID;
  iGeneB  = pShockData->iGene[1];
  pGeneB  = pInData+iGeneB;
  iGeneBb = pGeneB->iID;
  iGeneC  = pShockData->iGene[2];
  pGeneC  = pInData+iGeneC;
  iGeneCb = pGeneC->iID;
  iGeneD  = pShockData->iGene[3];
  pGeneD  = pInData+iGeneD;
  iGeneDb = pGeneD->iID;

  fPosXs = pShockData->ShockCoord.fPosX;
  fPosYs = pShockData->ShockCoord.fPosY;
  fPosZs = pShockData->ShockCoord.fPosZ;
  fDSq = pShockData->fDist2GeneSq;

  /* Compute the minimum bounding box limits */
  fRad = (float) sqrt((double) fDSq);
  iMinX = (int) floor((double) (fPosXs - fRad));
  if(iMinX < 1) iMinX = 1;
  iMaxX = (int) ceil((double) (fPosXs + fRad));
  if(iMaxX >= iXdim) iMaxX = iXdim - 2;
  iMinY = (int) floor((double) (fPosYs - fRad));
  if(iMinY < 1) iMinY = 1;
  iMaxY = (int) ceil((double) (fPosYs + fRad));
  if(iMaxY >= iYdim) iMaxY = iYdim - 2;
  iMinZ = (int) floor((double) (fPosZs - fRad));
  if(iMinZ < 1) iMinZ = 1;
  iMaxZ = (int) ceil((double) (fPosZs + fRad));
  if(iMaxZ >= iZdim) iMaxZ = iZdim - 2;

  /* ------------------------------------------------------ *\
   * Walk through voxels making up the minimum bounding box *
   * containing the contact sphere. We expect data to be    *
   * located near the sphere surface in general.
  \* ------------------------------------------------------ */

  iMaxX++; iMaxY++; iMaxZ++;
  for(i = iMinZ; i < iMaxZ; i++) {
    for(j = iMinY; j < iMaxY; j++) {
      for(k = iMinX; k < iMaxX; k++) {
  
  iVox = i * iSliceSize + j * iXdim + k;
  pVox = pVoxels+iVox;
  iNumGenesInVox = pVox->isNumGenes;
  if(iNumGenesInVox > 0) {

    /* ----- Make sure the voxel intersects the ball ----- *\
     * Check faces: Ball crosses a face? or is Surrounded? *
     * Compute Distances to Bucket Face's Planes & test    *
    \* Left-Right conditions         */

    fDistMin = i - fPosZs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = (i+1) - fPosZs;
    fDistMaxAbs = fabs(fDistMax);
    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket Z-faces intersect or surround Ball */

    fDistMin = j - fPosYs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = (j+1) - fPosYs;
    fDistMaxAbs = fabs(fDistMax);
    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket Y-faces intersect or surround Ball */

    fDistMin = k - fPosXs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = (k+1) - fPosXs;
    fDistMaxAbs = fabs(fDistMax);
    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket X-faces intersect or surround Ball */

    /* iIntersect++; */
    /* --- OK : Voxel covers Ball at least partially --- */
    /* Check w/r the genes in this voxel */
    piGene = pVox->piGenes;
    for(l = 0; l < iNumGenesInVox; l++) {
      iGene = *piGene++;
      if(iGene == iGeneAb || iGene == iGeneBb ||
         iGene == iGeneCb || iGene == iGeneDb)
        continue;

      pCoord = pGeneCoord+iGene;
      fPx = pCoord->fPosX - fPosXs;
      fPy = pCoord->fPosY - fPosYs;
      fPz = pCoord->fPosZ - fPosZs;
      fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
      fDSqNew += fEpsilon;

      if(fDSq > fDSqNew) { /* Invalid shock */
        return(FALSE); /* Part of SS only: go back */  
        /* break; Get out of For(i) loop */
      }
    } /* Next gene in voxel: l++ */

  } /* End of If(iNumGenesInVox > 0) */

      } /* Next X integer: k++ */
    } /* Next Y integer: j++ */
  } /* Next Z integer: i++ */

  /* Ball is entirely covered: Shock is valid */
  return(TRUE);

}

/* ---- EoF ---- */
