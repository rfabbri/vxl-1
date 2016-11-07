/*------------------------------------------------------------*\
 * File: blue_shocks_utils.c
 *
 * Functions:
 *  SetSheetsSub
 *  SetValidSheets
 *  SetBluePassive
 *  ValidBlueShock
 *  ValidBlueShockViaVoxels
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie (adapted from fetch_blue_shocks.c)
 *  #1: Dec. 2001: Added SetSheetsSub()
 *  #2: July 2003: Added ValidBlueShockViaVoxels()
 *
\*------------------------------------------------------------*/

#include "blue_shocks_utils.h"

/*------------------------------------------------------------*\
 * Function: SetValidSheets
 *
 * Usage: Sets validated initial sheet shocks on the scaffold.
 *    This is used to transfer the sheets found inside each
 *    bucket at full resolution. The list of Blue shock
 *    sheets to transfer *pShocks* also has at its head the
 *    list of shocks computed at low resolution, which needs
 *    to be skiped.
 *
\*------------------------------------------------------------*/

int
SetValidSheets(ShockSheet *pShocks, ShockScaffold *pScaffold,
         InputData *pInData, Dimension *pDim)
{
  char    *pcFName = "SetValidSheets";
  register int  i;
  int    iNumBlue, iGeneA,iGeneB, iBlue, iTmp;
  int    iNumProxiA,iNumProxiB, iMax;
  float    fPABx,fPABy,fPABz, fDSq;
  ShockSheet  *pBlue, *pNewBlue;
  InputData  *pGeneA, *pGeneB;
  Pt3dCoord  *pGeneCoordA, *pGeneCoordB;
  Vector3d  *pvNorm;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  /* iNumBlue = pDim->iNumShockSheetV; * Full Res. Internal only */
  /* iNumBlueTot = pDim->iNumShockSheet; * Sub. Res. All + Full Res. Int. */

  iNumBlue = pDim->iNumShockSheet;
  iBlue = pScaffold->iBlueActiveLast; /* Start on Scaffold list */
  pNewBlue = (pScaffold->pBlue)+iBlue-1;

#if FALSE
  /* Start on sheet list: Skip shocks set at sub.res. */
  iBlueOld = iNumBlueTot - iNumBlue - 1;
  pBlue = pShocks+iBlueOld;
  fprintf(stderr, "\tTransfering %d Blue shocks to Scaffold,\n", iNumBlue);
  fprintf(stderr, "\tfrom %d to %d on list (subsampled + full res. int.).\n",
    (iBlueOld+1), iNumBlueTot);
#endif
  pBlue = pShocks-1;
  for(i = 0; i < iNumBlue; i++) {
    pBlue++;
    pNewBlue++;

    pNewBlue->isFlagValid = TRUE;
    pNewBlue->iBuckXYZ    = pBlue->iBuckXYZ;
#if HIGH_MEM
    pNewBlue->MidPt.fPosX = pBlue->MidPt.fPosX;
    pNewBlue->MidPt.fPosY = pBlue->MidPt.fPosY;
    pNewBlue->MidPt.fPosZ = pBlue->MidPt.fPosZ;
    pNewBlue->fDist2GeneSq = pBlue->fDist2GeneSq;
#endif
    /* pNewBlue->isShockLabel = pBlue->iShockLabel; Old label in Full list */

    iGeneA = pNewBlue->iGene[0] = pBlue->iGene[0];
    iGeneB = pNewBlue->iGene[1] = pBlue->iGene[1];

    pNewBlue->isInitType = BLUE_INIT;
    pNewBlue->isStatus   = pBlue->isStatus;
    pNewBlue->iGreen     = pBlue->iGreen;
    pNewBlue->iRed       = pBlue->iRed;
    pNewBlue->isItera    = pScaffold->isLastItera;

    /* Set Gene Data: Backpointers to shock and Normal data */

    pGeneA = pInData+iGeneA;
    iNumProxiA = pGeneA->isNumBlue;
    iMax = pGeneA->isMaxBlue - 2;
    if(iNumProxiA > iMax) {
      iMax = 1 + iNumProxiA * 2;
      if((pGeneA->piBlueList =
    (int *) realloc((int *) pGeneA->piBlueList,
        iMax * sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc(pGeneA+%d)->piBlueList[%d] fails.\n",
    pcFName, iGeneA, iMax);
  return(FALSE); }
      if((pGeneA->pvNormal =
    (Vector3d *) realloc((Vector3d *) pGeneA->pvNormal,
             iMax * sizeof(Vector3d))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc(pGeneA+%d)->pvNormal[%d] fails.\n",
    pcFName, iGeneA, iMax);
  return(FALSE); }
      pGeneA->isMaxBlue = iMax;
      /* pGeneA->isMaxNorm = iMax; */
    }
    pGeneA->piBlueList[iNumProxiA] = iBlue; /* Backpointer to shock */
    pGeneA->isNumBlue++;

    pGeneB = pInData+iGeneB;

    pGeneCoordA = &(pGeneA->Sample);
    pGeneCoordB = &(pGeneB->Sample);
    fPABx = pGeneCoordB->fPosX - pGeneCoordA->fPosX;
    fPABy = pGeneCoordB->fPosY - pGeneCoordA->fPosY;
    fPABz = pGeneCoordB->fPosZ - pGeneCoordA->fPosZ;
    fDSq  = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);

    pvNorm = pGeneA->pvNormal+iNumProxiA;
    pvNorm->fX = fPABx;
    pvNorm->fY = fPABy;
    pvNorm->fZ = fPABz;
    pvNorm->fLengthSq = fDSq;
    /* pGeneA->isNumNorm++; */

    iNumProxiB = pGeneB->isNumBlue;
    iMax = pGeneB->isMaxBlue - 2;
    if(iNumProxiB > iMax) {
      iMax = 1 + iNumProxiB * 2;
      if((pGeneB->piBlueList =
    (int *) realloc((int *) pGeneB->piBlueList,
        iMax * sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc(pGeneB+%d)->piBlueList[%d] fails.\n",
    pcFName, iGeneB, iMax);
  return(FALSE); }
      if((pGeneB->pvNormal =
    (Vector3d *) realloc((Vector3d *) pGeneB->pvNormal,
             iMax * sizeof(Vector3d))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc(pGeneB+%d)->pvNormal[%d] fails.\n",
    pcFName, iGeneB, iMax);
  return(FALSE); }
      pGeneB->isMaxBlue = iMax;
      /* pGeneB->isMaxNorm = iMax; */
    }
    pGeneB->piBlueList[iNumProxiB] = iBlue; /* Backpointer to shock */
    pGeneB->isNumBlue++;

    pvNorm = pGeneB->pvNormal+iNumProxiB;
    pvNorm->fX = -fPABx;
    pvNorm->fY = -fPABy;
    pvNorm->fZ = -fPABz;
    pvNorm->fLengthSq = fDSq;
    /* pGeneB->isNumNorm++; */

    iBlue++;
  } /* Load next Valid Blue shock : i++ */

  fprintf(stderr, "\tSet Scaffold with %d initial valid Blue shocks.\n",
    iNumBlue);

  iTmp = iBlue - pScaffold->iBlueActiveLast;
  pScaffold->iBlueActiveLast = iBlue;
  pScaffold->iNumBlueActive += iTmp;

  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: SetBluePassive
 *
 * Usage: Sets active Blue Shocks which have been used to find
 *    new curves to be PASSIVE. 
 *    This is necessary to ensure sheets are not
 *    overused in intersections.
 *
\*------------------------------------------------------------*/

void
SetBluePassive(ShockScaffold *pScaffold)
{
  char    *pcFName = "SetBluePassive";
  register int  i;
  int    iBlue, iBlueEnd, iNumBlue, iNumActive, iPassive;
  int    iStatus, iActive;
  ShockSheet  *pBlue;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  
  iBlue = pScaffold->iBluePassiveLast; /* Front of Active sheets on Scaffold */
  pBlue = (pScaffold->pBlue)+iBlue-1;
  iBlueEnd = pScaffold->iBlueActiveLast; /* Last Blue sheets thus far */
  iNumBlue = iBlueEnd - iBlue;
  iNumActive = pScaffold->iNumBlueActive;
  if(iNumBlue != iNumActive) {
    fprintf(stderr, "ERROR: Expects to set %d Blue shocks on scaffold\n",
      iNumBlue);
    fprintf(stderr, "\tto be PASSIVE, but there are %d ACTIVE ones.\n",
      iNumActive);
    exit(-4);
  }

  fprintf(stderr, "\tSetting %d Blue shocks on scaffold to be PASSIVE.\n",
    iNumBlue);
  fprintf(stderr, "\tStarting from position %d on Scaffold->pBlue\n",
    iBlue);

  iActive = iPassive = 0;
  for(i = 0; i < iNumBlue; i++) {
    pBlue++;

    iStatus = pBlue->isStatus;
    if(iStatus != ACTIVE)
      iPassive++;
    else {
      iActive++;
      pBlue->isStatus = PASSIVE;
    }
  }

  if(iActive != iNumActive) {
    fprintf(stderr, "ERROR: Set %d Active GreenBlue shocks to be passive,\n",
      iActive);
    fprintf(stderr, "\tbut expected to set %d ...\n", iNumActive);
    exit(-3);
  }
  if(iPassive > 0) {
    fprintf(stderr, "\t%d Blue sheets are now PASSIVE status.\n", iNumBlue);
    fprintf(stderr, "\t%d were already passive however.\n", iPassive);
  }

  pScaffold->iNumBluePassive += (iNumBlue - iPassive);
  pScaffold->iBluePassiveLast = iBlueEnd;
  pScaffold->iNumBlueActive   = 0;

  return;
}

/*------------------------------------------------------------*\
 * Function: ValidBlueShock
 *
 * Usage: Validates a candidate Blue shock, the midpoint of a
 *    pair of generators. Validation ensures that the sphere
 *    with diameter specified by this pair is empty of other
 *    generators. 
 *
\*------------------------------------------------------------*/
#if FALSE
int
ValidBlueShock(ShockSheet *pBlue, InputData *pInData,
         short *pisLookUp, Buckets *pBucksXYZ,
         int *piNgbInBall, Dimension *pDim)
{
  char    *pcFName = "ValidBlueShock";
  register int  i, j, k;
  int    iLabel, iGeneA,iGeneB,iGeneS,iGeneN, iLabelBuckShock;
  int    iNumGeneInBuckShock, iMinX,iMinY,iMinZ, iMaxX,iMaxY,iMaxZ;
  int    iFlagX,iFlagY,iFlagZ, iFlagIn, iFlagCover;
  int    iIntersect, iLabelBuckNgb, iFirstLabel, iLastLabel;
  int    iNumNgbOfS, iNumNgbInBall, iNumNgbOfNgb, iNumGeneInBuckNgb;
  int    iLabelBuckNgbOfNgb, iNumGeneInBuckNgbOfNgb;
  int    iXdim,iYdim,iZdim;
  int    *piGeneInBuckShock, *piNgbOfS, *piGeneInBuckNgb, *piNgbOfNgb;
  int    *piGeneInBuckNgbOfNgb;
  static float  fEpsilon = (float) D_EPSILON;
  float    fPosXs,fPosYs,fPosZs, fPx,fPy,fPz, fDSq,fDSqNew, fRad;
  float    fDistMin,fDistMax, fDistMinAbs,fDistMaxAbs;
  float    fPosXn,fPosYn,fPosZn;
  Bucket  *pBuckShock, *pBuckNgb, *pBuckNgbOfNgb;
  Pt3dCoord  *pGeneCoordS, *pGeneCoordN;

  /* Preliminaries */

  iXdim = pDim->iXdim;
  iYdim = pDim->iYdim;
  iZdim = pDim->iZdim;

  iLabel = pBucksXYZ->iInitSheetLabel;
  iLabel++;
  pBucksXYZ->iInitSheetLabel = iLabel;

  iGeneA = pBlue->iGene[0];
  iGeneB = pBlue->iGene[1];

  fPosXs = pBlue->MidPt.fPosX;
  fPosYs = pBlue->MidPt.fPosY;
  fPosZs = pBlue->MidPt.fPosZ;
  fDSq = pBlue->fDist2GeneSq;

  iLabelBuckShock = pBlue->iBuckXYZ;
  pBuckShock = pBucksXYZ->pBucket+iLabelBuckShock;

  /* ------------------------------------------------------ *\
   *  1st: Validate w/r to bucket where shock is located
  \* ------------------------------------------------------ */

  iNumGeneInBuckShock = pBuckShock->isNumGenes;
  if(iNumGeneInBuckShock > 0) {
    piGeneInBuckShock = pBuckShock->piGenes;
    for(i = 0; i < iNumGeneInBuckShock; i++) {
      iGeneS = *piGeneInBuckShock++;
      if(iGeneS == iGeneA || iGeneS == iGeneB)
  continue;
      pGeneCoordS = &((pInData+iGeneS)->Sample);
      fPx = pGeneCoordS->fPosX - fPosXs;
      fPy = pGeneCoordS->fPosY - fPosYs;
      fPz = pGeneCoordS->fPosZ - fPosZs;
      fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
      fDSqNew += fEpsilon;
    
      if(fDSq > fDSqNew) { /* Invalid shock */
  /* iFlagValid = FALSE; */
  return(FALSE); /* Part of MS only: go back */  
  /* break; Get out of For(i) loop */
      }
    } /* Still valid: Check w/r next source in BuckS : i++ */
  }
      
  pBuckShock->iInitSheetLabel = iLabel; /* BuckShock: tested */

  /* Otherwise, if the ball is not entirely within this bucket *\
   * find the closest buckets which ensure the ball is covered *
  \* and test with respect to sources contained in these.      */
      
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

  /* Ball entirely in BuckShock ? */
  iFlagZ = iFlagY = iFlagX = FALSE;
  if(iMinX > pBuckShock->Limits.iMinX &&
     iMaxX < pBuckShock->Limits.iMaxX)
    iFlagX = TRUE;
  if(iMinY > pBuckShock->Limits.iMinY &&
     iMaxY < pBuckShock->Limits.iMaxY)
    iFlagY = TRUE;
  if(iMinZ > pBuckShock->Limits.iMinZ &&
     iMaxZ < pBuckShock->Limits.iMaxZ)
    iFlagZ = TRUE;
  iFlagIn = iFlagZ + iFlagY + iFlagX;
      
  if(iFlagIn > 2)
    return(TRUE); /* Ball Entirely in Bucket: Valid! go back */

  /* -- Ball is NOT fully contained in Bucket -- */
  /* -- Grow from BuckShock to 1st layer of surrounding ngbs -- */

  piNgbOfS = pBuckShock->piNgbs;
  iNumNgbOfS = pBuckShock->isNumNgbs;
  iNumNgbInBall = 0;
  iIntersect = 0;

  for(i = 0; i < iNumNgbOfS; i++) {
    iLabelBuckNgb = *piNgbOfS++;
    pBuckNgb = pBucksXYZ->pBucket+iLabelBuckNgb;
    if(pBuckNgb->iInitSheetLabel == iLabel)
      continue; /* Already visited : i++ */
    pBuckNgb->iInitSheetLabel = iLabel;
          
    /* ---- Make sure this bucket intersects the ball ---- *\
    \* Check faces: Ball crosses a face? or is Surrounded? */

    /* Compute Distances to Bucket Face's  *\
    \* Planes & test Left-Right conditions */

    fDistMin = pBuckNgb->Limits.iMinZ - fPosZs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = pBuckNgb->Limits.iMaxZ - fPosZs;
    fDistMaxAbs = fabs(fDistMax);
    
    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket Z-faces intersect or surround Ball */

    fDistMin = pBuckNgb->Limits.iMinY - fPosYs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = pBuckNgb->Limits.iMaxY - fPosYs;
    fDistMaxAbs = fabs(fDistMax);

    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket Y-faces intersect or surround Ball */

    fDistMin = pBuckNgb->Limits.iMinX - fPosXs;
    fDistMinAbs = fabs(fDistMin);
    fDistMax = pBuckNgb->Limits.iMaxX - fPosXs;
    fDistMaxAbs = fabs(fDistMax);

    if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
      if(fDistMin < 0.0 && fDistMax < 0.0) continue; /* Left */
      if(fDistMin > 0.0 && fDistMax > 0.0) continue; /* Right */
    } /* Bucket outside Ball : skip it : m++ */
    /* else : Bucket X-faces intersect or surround Ball */

    iIntersect++;

    /* --- OK : Bucket covers Ball at least partially --- */

    /* Check w/r the sources in this bucket */
    iNumGeneInBuckNgb = pBuckNgb->isNumGenes;
    if(iNumGeneInBuckNgb > 0) {
      piGeneInBuckNgb = pBuckNgb->piGenes;
      for(j = 0; j < iNumGeneInBuckNgb; j++) {
  iGeneN = *piGeneInBuckNgb++;
  if(iGeneN == iGeneA || iGeneN == iGeneB)
    continue;
  pGeneCoordN = &((pInData+iGeneN)->Sample);
  fPx = pGeneCoordN->fPosX - fPosXs;
  fPy = pGeneCoordN->fPosY - fPosYs;
  fPz = pGeneCoordN->fPosZ - fPosZs;
  fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
  fDSqNew += fEpsilon;
  if(fDSq > fDSqNew) { /* Invalid shock */
    /* iFlagValid = FALSE; */
    return(FALSE);
    /* break;  Get out of For(j) loop */
  }
      } /* Next source in BucketNgb : j++ */
    }

    /* Else : Bucket does not invalidate shock */
    /* Keep track of BuckNgb to acquire surrounding neighbors */
    piNgbInBall[iNumNgbInBall++] = iLabelBuckNgb;
  } /* Check next BucketNgb : i++ */
  
  /* if(!iFlagValid)
     return(FALSE); Part of SS only: go back */

  /* -- Still Valid : Go seek new ngbs for cover of Ball -- *\
  \* Grow from BuckShock's 1st layer to surrounding layers  */

  if(iIntersect > 0) {
    /* At least 1 ngb covers partially the ball */
    iFlagCover = FALSE;
    iFirstLabel = 0;
    iLastLabel = iNumNgbInBall;
  }
  else
    return(TRUE); /* Ball Entirely covered: Valid! go back */

  while(!iFlagCover) { /* While the ball is NOT fully covered */
      
    for(i = iFirstLabel; i < iLastLabel; i++) {

      iLabelBuckNgb = *(piNgbInBall+i);
      pBuckNgb = pBucksXYZ->pBucket+iLabelBuckNgb;
      piNgbOfNgb = pBuckNgb->piNgbs;
      iNumNgbOfNgb = pBuckNgb->isNumNgbs;

      for(j = 0; j < iNumNgbOfNgb; j++) {

  iLabelBuckNgbOfNgb = *piNgbOfNgb++;
  pBuckNgbOfNgb = pBucksXYZ->pBucket+iLabelBuckNgbOfNgb;
  if(pBuckNgbOfNgb->iInitSheetLabel == iLabel)
    continue; /* Already visited : j++ */
  pBuckNgbOfNgb->iInitSheetLabel = iLabel;
          
  /* -- Make sure this bucket intersects the ball -- *\
  \* Ball crosses a Bucket's face? or is Surrounded? */
        
  /* Compute Distances to Bucket Face's Planes *\
  \* and test Left-Right conditions.       */

  fDistMin = pBuckNgbOfNgb->Limits.iMinZ - fPosZs;
  fDistMinAbs = fabs(fDistMin);
  fDistMax = pBuckNgbOfNgb->Limits.iMaxZ - fPosZs;
  fDistMaxAbs = fabs(fDistMax);
      
  if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
    /* Bucket outside Ball : skip it : j++ */
    if(fDistMin < 0.0 && fDistMax < 0.0)
      continue; /* Left */
    if(fDistMin > 0.0 && fDistMax > 0.0)
      continue; /* Right */
  }
  /* else : Bucket Z-faces intersect or surround Ball */

  fDistMin = pBuckNgbOfNgb->Limits.iMinY - fPosYs;
  fDistMinAbs = fabs(fDistMin);
  fDistMax = pBuckNgbOfNgb->Limits.iMaxY - fPosYs;
  fDistMaxAbs = fabs(fDistMax);

  if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
    /* Bucket outside Ball : skip it : j++ */
    if(fDistMin < 0.0 && fDistMax < 0.0)
      continue; /* Left */
    if(fDistMin > 0.0 && fDistMax > 0.0)
      continue; /* Right */
  }
  /* else : Bucket Y-faces intersect or surround Ball */

  fDistMin = pBuckNgbOfNgb->Limits.iMinX - fPosXs;
  fDistMinAbs = fabs(fDistMin);
  fDistMax = pBuckNgbOfNgb->Limits.iMaxX - fPosXs;
  fDistMaxAbs = fabs(fDistMax);

  if(fDistMinAbs > fRad && fDistMaxAbs > fRad) {
    /* Bucket outside Ball : skip it : j++ */
    if(fDistMin < 0.0 && fDistMax < 0.0)
      continue; /* Left */
    if(fDistMin > 0.0 && fDistMax > 0.0)
      continue; /* Right */
  }
  /* else : Bucket X-faces intersect or surround Ball */
  
  /* --- OK : Bucket covers Ball at least partially --- */

  /* Check viz the sources in this bucket */
  iNumGeneInBuckNgbOfNgb = pBuckNgbOfNgb->isNumGenes;
  if(iNumGeneInBuckNgbOfNgb > 0) {
    piGeneInBuckNgbOfNgb = pBuckNgbOfNgb->piGenes;
    for(k = 0; k < iNumGeneInBuckNgbOfNgb; k++) {
      iGeneN = *piGeneInBuckNgbOfNgb++;
      if(iGeneN == iGeneA || iGeneN == iGeneB)
        continue;
      pGeneCoordN = &((pInData+iGeneN)->Sample);
      fPosXn = pGeneCoordN->fPosX;
      fPosYn = pGeneCoordN->fPosY;
      fPosZn = pGeneCoordN->fPosZ;
      fPx = fPosXn - fPosXs;
      fPy = fPosYn - fPosYs;
      fPz = fPosZn - fPosZs;
        
      fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
      fDSqNew += fEpsilon;
      if(fDSq > fDSqNew) { /* Invalid shock */
        /* iFlagValid = FALSE; */
        return(FALSE);
        /* break; Get out of For(k) loop */
      }
    } /* Next source in BuckNgbOfNgb : k++ */
  }
  /* Else : Bucket does not invalidate shock */
  /* Keep track of BuckNgbOfNgb to Acquire new neighbors */
  piNgbInBall[iNumNgbInBall++] = iLabelBuckNgbOfNgb;
          
      } /* Check next BuckNgbOfNgb : j++ */

    } /* Go check Next BuckNgb : i++ */

    /* -- All BuckNgb visited for this Layer -- */

    if(iNumNgbInBall > iLastLabel) { /* New Ngb. to try out */
      iFirstLabel = iLastLabel;
      iLastLabel = iNumNgbInBall;
    }
    else /* No more Ngbs : Ball is covered */
      iFlagCover = TRUE;
  } /* Goto top of While loop */
    
  /* ------------------ VALIDATION DONE ------------------ */

  return(TRUE);
}
#endif

/*------------------------------------------------------------*\
 * Function: ValidBlueShockViaVoxels
 *
 * Usage: Validates a candidate Blue shock, the midpoint of a
 *    pair of generators. Validation ensures that the sphere
 *    with diameter specified by this pair is empty of other
 *    generators. 
 *
\*------------------------------------------------------------*/

int
ValidBlueShockViaVoxels(ShockData *pShockData, InputData *pInData,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimAll)
{
  /* char    *pcFName = "ValidBlueShockViaVoxels"; */
  register int  i, j, k, l;
  int    iGeneA,iGeneAb,iGeneB,iGeneBb, iXdim,iYdim,iZdim;
  int    iSliceSize, iMinX,iMinY,iMinZ, iMaxX,iMaxY,iMaxZ;
  int    iNumGenesInVox, iX,iY,iZ, iVoxShock, iGene, iVox;
  int    iFlagZ,iFlagY,iFlagX, iFlagIn;
  int    *piGene;
  static float  fEpsilon = (float) D_EPSILON;
  double    fPosXs,fPosYs,fPosZs, fPx,fPy,fPz, fDSq,fDSqNew, fRad;
  double    fDistMin,fDistMax, fDistMinAbs,fDistMaxAbs;
  Pt3dCoord  *pCoord;
  InputData  *pGeneA, *pGeneB;
  Voxel    *pVox;

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

  fPosXs = pShockData->ShockCoord.fPosX;
  fPosYs = pShockData->ShockCoord.fPosY;
  fPosZs = pShockData->ShockCoord.fPosZ;
  fDSq = pShockData->fDist2GeneSq;

  /* ------------------------------------------------------ *\
   *  1st: Check the voxel containing the shock.
   *       This is useful to rule out smaller spheres
   *       near the sampled surface.
  \* ------------------------------------------------------ */

  iX = (int) floor((double) fPosXs);
  iY = (int) floor((double) fPosYs);
  iZ = (int) floor((double) fPosZs);

  iVoxShock = iZ * iSliceSize + iY * iXdim + iX;
  pVox = pVoxels+iVoxShock;
  iNumGenesInVox = pVox->isNumGenes;
  if((iNumGenesInVox > 0) && (fDSq > 12.0)) { /* (2R_max)^2 = 12 */
    /* Ball entirely surrounds voxel, even if the shock   *\
    \* is in the corner: gene(s) inside invalidates shock */
    return(FALSE);
  }

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

  if(iNumGenesInVox > 0) {

    /* Ball surrounds entirely the voxel ? */
    iFlagZ = iFlagY = iFlagX = FALSE;
    if(iMinX < iX && iMaxX > (iX+1))
      iFlagX = TRUE;
    if(iMinY < iY && iMaxY > (iY+1))
      iFlagY = TRUE;
    if(iMinZ < iZ && iMaxZ > (iZ+1))
      iFlagZ = TRUE;
    iFlagIn = iFlagZ + iFlagY + iFlagX;
    if(iFlagIn > 2)  /* Ball surrounds Entirely the voxel */
      return(FALSE); /* Invalid shock */

    /* Else: Ball partly in voxel: check w/r genes */
    piGene = pVox->piGenes;
    for(i = 0; i < iNumGenesInVox; i++) {
      iGene = *piGene++;
      if(iGene == iGeneAb || iGene == iGeneBb)
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
    } /* Next gene in voxel: i++ */

  }
  /* else: Voxel is empty: check ngbs. */

  /* ------------------------------------------------------ *\
   *  2nd: Walk through voxels making up the minimum
   *       bounding box containing the contact sphere.
   *       We expect data to be located near the sphere
   *       surface in general.
  \* ------------------------------------------------------ */

  iMaxX++; iMaxY++; iMaxZ++;
  for(i = iMinZ; i < iMaxZ; i++) {
    for(j = iMinY; j < iMaxY; j++) {
      for(k = iMinX; k < iMaxX; k++) {
  
  iVox = i * iSliceSize + j * iXdim + k;
  if(iVox == iVoxShock)
    continue;
  pVox = pVoxels+iVox;
  iNumGenesInVox = pVox->isNumGenes;
  if(iNumGenesInVox > 0) {

    /* ----- Make sure the voxel intersects the ball ----- *\
     * Check faces: Ball crosses a face? or is Surrounded? *
     * Compute Distances to Bucket Face's
    \* Planes & test Left-Right conditions */

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
      if(iGene == iGeneAb || iGene == iGeneBb)
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

/* ----- EoF ----- */
