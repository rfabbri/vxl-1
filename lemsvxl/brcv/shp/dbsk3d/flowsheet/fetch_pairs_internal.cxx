/*------------------------------------------------------------*\
 * File: fetch_pairs_internal.c
 *
 * Functions:
 *  FetchBlueShocksIntSub
 *  FetchBlueShocksIntAll
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie (adapted from previous
 *    full scale version).
 *  #1: July 2003: Upgraded for use with MetaBuckets and
 *    shock validation via voxels.
 *
\*------------------------------------------------------------*/

#include "fetch_pairs_internal.h"
#include "blue_shocks_utils.h"

/*------------------------------------------------------------*\
 * Function: FetchBlueShocksIntSub
 *
 * Usage: Seek initial (isolated) Sheet shocks - some are part of
 *    the MA and thus, the source of a shock sheet, others
 *    are only part of the SS - INSIDE each bucket.
 *    Only perform computations for sub-sampled generators.
 *
\*------------------------------------------------------------*/

#if FALSE
void
FetchBlueShocksIntSub(InputData *pInData, ShockSheet *pShocks,
          short *pisLookUp, Buckets *pBucksXYZ,
          Dimension *pDim)
{
  char    *pcFName = "FetchBlueShocksIntSub";
  register int  i, j, k, l, m, n, o;
  unsigned int  iNum, iNumBucks, iGeneProc;
  int    iNum3dPts, iNumShock, iMaxShock;
  int    iNumInvalid, iFlagValid, iLabel, iMax, iBallGrowsOut;
  int    iXdim, iSliceSize, iPos, iBuckLabel;
  int    iNumGeneInBuckA, iGeneA, iMinX,iMaxX, iMinY,iMaxY, iMinZ,iMaxZ;
  int    iZjump,iYjump, iYdim,iZdim, iFlagZ,iFlagY,iFlagX, iFlagIn;
  int    iGeneB, iNumGeneInBuckB, iGeneC, iNext;
  int    *piGeneInBuckA, *piGeneInBuckA2, *piGeneInBuckB;
  int    *piGeneInBuckA3;
  static float  fEpsilon = (float) D_EPSILON;
  float    fPosXa,fPosYa,fPosZa, fPosXb,fPosYb,fPosZb, fPx,fPy,fPz;
  float    fPosXs,fPosYs,fPosZs, fPABx,fPABy,fPABz, fDSq, fDSqNew;
  float   fDistSqLargest, fPosXc,fPosYc,fPosZc, fRad;
  float    fTimeSys, fTimeUser, fMemUse, fTmpSys, fTmpUser;
  InputData  *pGeneA, *pGeneB;
  Pt3dCoord  *pGeneCoordA, *pGeneCoordB, *pGeneCoordC;
  Vector3d  *pvNorm;
  ShockSheet  *pBlue;
  Bucket  *pBuckA, *pBuckB;
#if SGI_ENV
  struct rusage  ru_start, ru_first;

  /* Preliminaries */
  if(getrusage(RUSAGE_SELF, &ru_start) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
  }
#endif
  iNum3dPts = pDim->iNum3dPts;
  iNumShock = pDim->iNumShockSheet = 0; /* No shocks yet: We start here ! */
  iMaxShock = pDim->iMaxBlueShocks;
  iNumInvalid = 0;

  fDistSqLargest = 0.0;

  iLabel = 0;
  iBallGrowsOut = 0;

  iXdim = pDim->iXdim;
  iYdim = pDim->iYdim;
  iZdim = pDim->iZdim;
  iSliceSize = pDim->iSliceSize;
  iNumBucks = pBucksXYZ->iNumBucks;

  fprintf(stderr,
    "MESG(%s):\n\tSeeking Initial Blue Shocks amongst %d generators,\n",
    pcFName, iNum3dPts);
  fprintf(stderr, "\t\tprocessing %d BucketsXYZ,\n", iNumBucks);
  fprintf(stderr, "\t\tonly for pairs INTERNAL to each bucket.\n");

  /* ------------ Process BucketsXYZ one by one ----------- *\
  \*   seek "internal" pairs from their sub-sampled genes   */ 

  iGeneProc = 0;
  pBuckA = pBucksXYZ->pBucket-1;
  pBlue = pShocks;

  for(i = 0; i < iNumBucks; i++) {

    pBuckA++;
    /* iNumGeneInBuckA = pBuckA->iNumGenes; */
    iNumGeneInBuckA = pBuckA->isNumSubGenes;
    if(iNumGeneInBuckA < 1) continue;

    /* Build the list of Blue Shocks for each gene, within the Bucket */

    /* piGeneInBuckA = pBuckA->piGenes; */
    piGeneInBuckA = pBuckA->piSubGenes;
    for(j = 0; j < iNumGeneInBuckA; j++) {
      iGeneProc++;

      iGeneA  = *piGeneInBuckA++;
      pGeneA = pInData+iGeneA;
      pGeneCoordA  = &(pGeneA->Sample);
      fPosXa = pGeneCoordA->fPosX;
      fPosYa = pGeneCoordA->fPosY;
      fPosZa = pGeneCoordA->fPosZ;

      /* With respect to other generators in that bucket, *\
      \* in ascending order only to avoid doubling pairs  */
      iNext = j + 1;
      piGeneInBuckA2 = pBuckA->piSubGenes+iNext;
      for(k = iNext; k < iNumGeneInBuckA; k++) {
  iLabel--; /* Decrement label of potential shock : for ngb. buckets */

  iGeneB  = *piGeneInBuckA2++;
  pGeneB = pInData+iGeneB;
  pGeneCoordB  = &(pGeneB->Sample);
  fPosXb = pGeneCoordB->fPosX;
  fPosYb = pGeneCoordB->fPosY;
  fPosZb = pGeneCoordB->fPosZ;

  fPABx = fPosXb - fPosXa;
  fPABy = fPosYb - fPosYa;
  fPABz = fPosZb - fPosZa;

  fDSq  = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
  fDSq *= 0.25;  /* Divided by half^2 */

  fPosXs = 0.5 * (fPosXa + fPosXb); /* Potential Shock coords. */
  fPosYs = 0.5 * (fPosYa + fPosYb);
  fPosZs = 0.5 * (fPosZa + fPosZb);

  /* ----------- VALIDATE Blue Shock Candidate ----------- */

  iFlagValid = TRUE;

  /* Shock Must be inside BucketA which contains the Convex Hull */
  pBuckA->iInitSheetLabel = iLabel;
  piGeneInBuckA3 = pBuckA->piSubGenes;
  for(l = 0; l < iNumGeneInBuckA; l++) {
    iGeneC = *piGeneInBuckA3++;
    if(iGeneC == iGeneA || iGeneC == iGeneB) continue;
    pGeneCoordC  = &((pInData+iGeneC)->Sample);
    fPosXc = pGeneCoordC->fPosX;
    fPosYc = pGeneCoordC->fPosY;
    fPosZc = pGeneCoordC->fPosZ;

    fPx = fPosXc - fPosXs;
    fPy = fPosYc - fPosYs;
    fPz = fPosZc - fPosZs;

    fDSqNew  = fPx*fPx + fPy*fPy + fPz*fPz;
    fDSqNew += fEpsilon; /* Rule-out very close results */

    if(fDSq > fDSqNew) { /* Invalid shock */
      iFlagValid = FALSE;
      iNumInvalid++;
      break; /* Get out of For(l) loop */
    }
  } /* Still valid: Check w/r next gene in BucketA : l++ */
  if(!iFlagValid) continue; /* Part of SS only: goto next pair */

  /* Otherwise, if the ball is not entirely within this bucket *\
   * find the closest buckets which ensure the ball is covered *
        \* and test with respect to generators contained in these.   */
      
  /* We go through the minimum bounding box enclosing tightly  *\
   * the sphere centered at the potential shock point and of   *
  \* diameter prescribed by its pair of associated generators. */

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

  iFlagZ = iFlagY = iFlagX = FALSE;
  if(iMinX > pBuckA->Limits.iMinX && iMaxX < pBuckA->Limits.iMaxX)
    iFlagX = TRUE;
  if(iMinY > pBuckA->Limits.iMinY && iMaxY < pBuckA->Limits.iMaxY)
    iFlagY = TRUE;
  if(iMinZ > pBuckA->Limits.iMinZ && iMaxZ < pBuckA->Limits.iMaxZ)
    iFlagZ = TRUE;
  iFlagIn = iFlagZ + iFlagY + iFlagX;

  if(iFlagIn < 3) { /* Ball is not fully contained in Bucket A */
    iBallGrowsOut++;
    iZjump = iMinZ * iSliceSize;
    for(l = iMinZ; l < iMaxZ; l++) {
      /* iZjump = l * iSliceSize; */
      iYjump = iMinY * iXdim;
      for(m = iMinY; m < iMaxY; m++) {
        /* iYjump = m * iXdim; */
        iPos = iZjump + iYjump + iMinX;
        for(n = iMinX; n < iMaxX; n++) {
    /* iPos = iZjump + iYjump + n; */
    iBuckLabel = *(pisLookUp+iPos);
    pBuckB = pBucksXYZ->pBucket+iBuckLabel;
    if(pBuckB->iInitSheetLabel == iLabel) { /* Already visited */
      n = pBuckB->Limits.iMaxX; /* Jump ahead in X */
      iPos = iZjump + iYjump + n;
      n--;
      continue; } /* Visited */
    pBuckB->iInitSheetLabel = iLabel;
    /* Check viz the genes in this bucket */
    iNumGeneInBuckB = pBuckB->isNumSubGenes;
    if(iNumGeneInBuckB > 0) {
      piGeneInBuckB = pBuckB->piSubGenes;
      for(o = 0; o < iNumGeneInBuckB; o++) {
        iGeneC = *piGeneInBuckB++;
        /* if(iGeneC == iGeneA || iGeneC == iGeneB) continue; */
        pGeneCoordC  = &((pInData+iGeneC)->Sample);
        fPosXc = pGeneCoordC->fPosX;
        fPosYc = pGeneCoordC->fPosY;
        fPosZc = pGeneCoordC->fPosZ;
        fPx = fPosXc - fPosXs;
        fPy = fPosYc - fPosYs;
        fPz = fPosZc - fPosZs;
        
        fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
        fDSqNew += fEpsilon;
        if(fDSq > fDSqNew) { /* Invalid shock */
          iFlagValid = FALSE;
          iNumInvalid++;
          break; /* Get out of For(o) loop */
        }
      } /* Next gene in bucket : o++ */
    }
    if(!iFlagValid) break; /* Get out of For(n) loop */
    /* Bucket containing shock Invalidates Shock */
    iPos++;
        } /* n++ */
        if(!iFlagValid) break; /* Get out of For(m) loop */
        iYjump += iXdim;
      } /* m++ */
      if(!iFlagValid) break; /* Get out of For(l) loop */
      iZjump += iSliceSize;
    } /* l++ */
  } /* Cover of ball has been visited */
  if(!iFlagValid) continue; /* Part of SS only: Goto next pair */

  /* ----- Found a New Valid Sheet Shock ----- */

  if(fDSq > fDistSqLargest) fDistSqLargest = fDSq;

  /* -- Set Blue Shock Data : On Tmp ShockSheet List -- */

  pBlue->isFlagValid = TRUE;
  pBlue->iBuckXYZ   = i;

#if HIGH_MEM
  pBlue->MidPt.fPosX = fPosXs;
  pBlue->MidPt.fPosY = fPosYs;
  pBlue->MidPt.fPosZ = fPosZs;
  pBlue->fDist2GeneSq = fDSq;
#endif
  pBlue->iGene[0] = iGeneA;
  pBlue->iGene[1] = iGeneB;

  pBlue->isInitType = BLUE_SUB; /* Low Res. Shock */
  pBlue->isStatus   = IDLE;
  pBlue->iGreen     = IDLE;
  pBlue->iRed       = IDLE;
  pBlue->isItera    = -1; /* Sub res. */

  /* -- Set Gene Data: Backpointers to Shock & Normal data -- *\
   *  We do this immediately, so that visibility constraints  *
  \*  are available immediately to rule out other candidates. */

  /* Put Info on GeneA */

  iNum = pGeneA->isNumBlue;
  iMax = pGeneA->isMaxBlue - 2;
  if(iNum > iMax) {
    iMax = (iNum+1) * 2;
    if((pGeneA->piBlueList =
        (int *) realloc((int *) pGeneA->piBlueList,
            iMax * sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pGeneA+%d)->piBlueList[%d] fails.\n",
        pcFName, iGeneA, iMax);
      exit(-3); }
    if((pGeneA->pvNormal =
        (Vector3d *) realloc((Vector3d *) pGeneA->pvNormal,
           iMax * sizeof(Vector3d))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pGeneA+%d)->pvNormal[%d] fails.\n",
        pcFName, iGeneA, iMax);
      exit(-3); }
    pGeneA->isMaxBlue = iMax;
    /* pGeneA->isMaxNorm = iMax; */
  }
  pGeneA->piBlueList[iNum] = iNumShock; /* Backpointer to shock */
  pGeneA->isNumBlue++;

  pvNorm = pGeneA->pvNormal+iNum;
  pvNorm->fX = fPABx;
  pvNorm->fY = fPABy;
  pvNorm->fZ = fPABz;
  pvNorm->fLengthSq = fDSq * 4.0;
  /* pGeneA->iNumNorm++; */

  /* Put Info on GeneB */

  iNum = pGeneB->isNumBlue;
  iMax = pGeneB->isMaxBlue - 2;
  if(iNum > iMax) {
    iMax = (iNum+1) * 2;
    if((pGeneB->piBlueList =
        (int *) realloc((int *) pGeneB->piBlueList,
            iMax * sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pGeneB+%d)->piBlueList[%d] fails.\n",
        pcFName, iGeneB, iMax);
      exit(-3); }
    if((pGeneB->pvNormal =
        (Vector3d *) realloc((Vector3d *) pGeneB->pvNormal,
           iMax * sizeof(Vector3d))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pGeneB+%d)->pvNormal[%d] fails.\n",
        pcFName, iGeneB, iMax);
      exit(-3); }
    pGeneB->isMaxBlue = iMax;
    /* pGeneB->isMaxNorm = iMax; */
  }
  pGeneB->piBlueList[iNum] = iNumShock; /* Backpointer to shock */
  pGeneB->isNumBlue++;

  pvNorm = pGeneB->pvNormal+iNum;
  pvNorm->fX = -fPABx;
  pvNorm->fY = -fPABy;
  pvNorm->fZ = -fPABz;
  pvNorm->fLengthSq = fDSq * 4.0;
  /* pGeneB->iNumNorm++; */

  iNumShock++;
  pBlue++;

      } /* Goto next 2nd gene in BuckA to create a new pair: k++ */

    } /* Goto next 1st gene in BuckA to create new pairs: j++ */

    *(pBuckA->pucFlagViz+i) = 2; /* Don't test self-visibility */
    *(pBuckA->pucFlagPair+i) = 1; /* Pairings performed */

  } /* Goto next Bucket(A) to create new pairs: i++ */
  if(iNumShock > iMaxShock) {
    fprintf(stderr, "ERROR(%s):\n\tMax Num of shock exceeded: %d > %d.\n",
      pcFName, iNumShock, iMaxShock);
    exit(-3);
  }

  fprintf(stderr,
    "\tProcessed %d generators at low res. (viz. %d srcs at full res.)\n",
    iGeneProc, iNum3dPts);
  fprintf(stderr, "\n\t***** `Internal' pairs found *****\n");
  fprintf(stderr, "\t%d VALID Blue shocks (Max = %d).\n",
    iNumShock, iMaxShock);
  fprintf(stderr, "\tLargest distance between generators found = %f.\n",
    (float) sqrt((double) fDistSqLargest));
  fprintf(stderr, "\tInvalid shocks = %d.\n", iNumInvalid);
  fprintf(stderr, "\tBalls growing out of buckets = %d.\n", iBallGrowsOut);
#if SGI_ENV
  if(getrusage(RUSAGE_SELF, &ru_first) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
  }
  fTmpSys  = (float) SECS(ru_first.ru_stime);
  fTmpUser = (float) SECS(ru_first.ru_utime);
  fTimeSys  = fTmpSys - ((float) SECS(ru_start.ru_stime));
  fTimeUser = fTmpUser - ((float) SECS(ru_start.ru_utime));
  fMemUse   = (float) (ru_first.ru_maxrss / 1000.0);
  fprintf(stderr, "RUSAGE (Low Res. 1st pass):\n");
  fprintf(stderr, "\tSys = %.2f , User = %.2f , Mem = %.2f Meg.\n",
    fTimeSys, fTimeUser, fMemUse);
#endif
  pDim->iNumShockSheetIntSub = iNumShock;
  pDim->iNumShockSheet = iNumShock;

  return;
}
#endif

/*------------------------------------------------------------*\
 * Function: FetchBlueShocksIntAll
 *
 * Usage: Seek initial (isolated) Sheet shocks - some are part of
 *    the MA and thus, the source of a shock sheet, others
 *    are only part of the SS - INSIDE each bucket.
 *    Perform computations for ALL generators.
 *    NB: Shock constraints obtained at sub-resolution are
 *        NOT used yet, at this stage, because most likely
 *        they have an impact BETWEEN buckets.
 *
\*------------------------------------------------------------*/

void
FetchBlueShocksIntAll(InputData *pInData, ShockSheet *pShocks,
          Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
          Voxel *pVoxels, Dimension *pDimMBuck,
          Dimension *pDimAll)
{
  char    *pcFName = "FetchBlueShocksIntAll";
  register int  i, j, k;
  unsigned int  iNumBucks, iGeneProc, iNewBlue, iTmp;
  int    iNum3dPts, iNumShock, iMaxShock, iNumInvalid;
  int    iNumShockInit, iNumGeneInBuckA;
  int    iGeneB,iGeneA, iNext;
  int    *piGeneInBuckA, *piGeneInBuckA2;
  /* static float  fEpsilon = (float) D_EPSILON; */
  float    fPosXa,fPosYa,fPosZa, fPosXb,fPosYb,fPosZb, fDSq;
  float    fPABx,fPABy,fPABz, fDistSqLargest;
  float    fTimeSys, fTimeUser, fMemUse, fTmpSys, fTmpUser;
  InputData  *pGeneA, *pGeneB;
  Pt3dCoord  *pGeneCoordA, *pGeneCoordB;
  ShockSheet  *pBlue;
  Bucket  *pBuckA;
  ShockData  theShockData, *pShockData;
#if SGI_ENV
  struct rusage  ru_start, ru_first;

  /* Preliminaries */

  if(getrusage(RUSAGE_SELF, &ru_start) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
  }
#endif

  pShockData = &theShockData;
  pShockData->iGene[2] = pShockData->iGene[3] = IDLE;

  fprintf(stderr, "MESG(%s):...\n", pcFName);
  iNum3dPts = pDimMBuck->iNum3dPts;
  /* Start on shock list after sub.res. list */
  iNumShockInit = iNumShock = pDimMBuck->iNumShockSheet;
  iMaxShock = pDimMBuck->iMaxBlueShocks;
  iNewBlue = iNumInvalid = 0;
  pBlue = pShocks+iNumShock;

  fDistSqLargest = 0.0;

  pBucksXYZ->iInitSheetLabel = 0; /* Increment for each Potential Sheet */

  iNumBucks = pBucksXYZ->iNumBucks;

  fprintf(stderr,
    "\tSeeking Initial Blue Shocks amongst %d generators,\n",
    iNum3dPts);
  fprintf(stderr, "\tprocessing %d BucketsXYZ,\n", iNumBucks);
  fprintf(stderr, "\t\tonly for pairs INTERNAL to each bucket.\n");

  /* ------------ Process BucketsXYZ one by one ----------- *\
  \*    Seek "internal" pairs for their shock sources     */

  iGeneProc = 0;
  pBuckA = pBucksXYZ->pBucket-1;
  for(i = 0; i < iNumBucks; i++) {

    pBuckA++;
    iNumGeneInBuckA = pBuckA->isNumGenes;
    if(iNumGeneInBuckA < 1) continue;

    /* Build the list of Blue Shocks for each gene, within the Bucket */

    /* piGeneInBuckA = pBuckA->piGenes; */
    piGeneInBuckA = pBuckA->piGenes;
    for(j = 0; j < iNumGeneInBuckA; j++) {
      iGeneProc++;

      iGeneA  = *piGeneInBuckA++;
      pGeneA = pInData+iGeneA;
      pGeneCoordA  = &(pGeneA->Sample);
      fPosXa = pGeneCoordA->fPosX;
      fPosYa = pGeneCoordA->fPosY;
      fPosZa = pGeneCoordA->fPosZ;

      /* With respect to other genes in that bucket, in *\
      \* ascending order only to avoid doubling pairs   */
      iNext = j + 1;
      piGeneInBuckA2 = pBuckA->piGenes+iNext;
      for(k = iNext; k < iNumGeneInBuckA; k++) {

  iGeneB  = *piGeneInBuckA2++;
  pGeneB = pInData+iGeneB;
  pGeneCoordB  = &(pGeneB->Sample);
  fPosXb = pGeneCoordB->fPosX;
  fPosYb = pGeneCoordB->fPosY;
  fPosZb = pGeneCoordB->fPosZ;

  fPABx = fPosXb - fPosXa;
  fPABy = fPosYb - fPosYa;
  fPABz = fPosZb - fPosZa;

  fDSq  = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
  fDSq *= 0.25;  /* Divided by half^2 */
  pShockData->fDist2GeneSq = fDSq;
  pShockData->ShockCoord.fPosX = 0.5 * (fPosXa + fPosXb);
  pShockData->ShockCoord.fPosY = 0.5 * (fPosYa + fPosYb);
  pShockData->ShockCoord.fPosZ = 0.5 * (fPosZa + fPosZb);

#if HIGH_MEM
  pBlue->fDist2GeneSq = fDSq;
  /* Potential Shock coords. */
  pBlue->MidPt.fPosX = 0.5 * (fPosXa + fPosXb);
  pBlue->MidPt.fPosY = 0.5 * (fPosYa + fPosYb);
  pBlue->MidPt.fPosZ = 0.5 * (fPosZa + fPosZb);
#endif
  /* --------- VALIDATE Blue Shock Candidate ----------- *\
   * Shock must be inside BucketA (mid-point), but the   *
  \* contact sphere may not be entirely inside BucketA.  */

  pBlue->iBuckXYZ = i;

  pShockData->iGene[0] = pBlue->iGene[0] = iGeneA;
  pShockData->iGene[1] = pBlue->iGene[1] = iGeneB;

  /* -------------------- VALIDATE --------------------- */

  if(!ValidBlueShockViaVoxels(pShockData, pInData, pGeneCoord,
            pVoxels, pDimAll)) {
    iNumInvalid++;
    continue; /* Part of SS only: Goto next pair : j++ */
  }

  /* ----- Found a New Valid Sheet Shock ----- */

  if(fDSq > fDistSqLargest) fDistSqLargest = fDSq;

  /* Set Blue Shock Data */

  pBlue->isFlagValid = TRUE;
  pBlue->isInitType  = BLUE_INIT;
  pBlue->isStatus    = ACTIVE;
  pBlue->iGreen      = IDLE;
  pBlue->iRed        = IDLE;
  pBlue->isItera     = 0;

  iNumShock++; /* Total: Sub. Res. + Full Res. */
  iNewBlue++;  /* Full res. only */
  pBlue++;

      } /* Goto next 2nd gene in BuckA to create a new pair: k++ */

    } /* Goto next 1st gene in BuckA to create new pairs: j++ */

    *(pBuckA->pucFlagViz+i)  = 2; /* Don't test self-visibility */
    *(pBuckA->pucFlagPair+i) = 1; /* Pairings performed */

  } /* Goto next Bucket(A) to create new pairs: i++ */
  if(iNumShock > iMaxShock) {
    fprintf(stderr, "ERROR(%s):\n\tMax Num of shock exceeded: %d > %d.\n",
      pcFName, iNumShock, iMaxShock);
    exit(-3);
  }
  if(iGeneProc != iNum3dPts) {
    fprintf(stderr, "ERROR(%s):\n\tProcessed %d generators instead of %d.\n",
      pcFName, iGeneProc, iNum3dPts);
    exit(-3);
  }
  iTmp = iNumShock - iNumShockInit;
  if(iNewBlue != iTmp) {
    fprintf(stderr, "ERROR(%s):\n\tGenerated %d Blue shocks instead of %d.\n",
      pcFName, iNewBlue, iTmp);
    exit(-3);
  }

  fprintf(stderr, "\n\t***** `Internal' pairs found *****\n");
  fprintf(stderr, "\t%d VALID Blue shocks (at FULL Res.).\n", iNewBlue);
  fprintf(stderr, "\t\t%d Previously (at sub.res.), Max local alloc. = %d\n",
    iNumShockInit, iMaxShock);
  fprintf(stderr, "\tLargest distance between generators found = %f.\n",
    (float) sqrt((double) fDistSqLargest));
  fprintf(stderr, "\tInvalid shocks = %d.\n", iNumInvalid);
#if SGI_ENV
  if(getrusage(RUSAGE_SELF, &ru_first) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
  }
  fTmpSys  = (float) SECS(ru_first.ru_stime);
  fTmpUser = (float) SECS(ru_first.ru_utime);
  fTimeSys  = fTmpSys - ((float) SECS(ru_start.ru_stime));
  fTimeUser = fTmpUser - ((float) SECS(ru_start.ru_utime));
  fMemUse   = (float) (ru_first.ru_maxrss / 1000.0);
  fprintf(stderr,
    "\tRUSAGE (1st pass): Sys = %.2f , User = %.2f , Mem = %.2f Meg.\n",
    fTimeSys, fTimeUser, fMemUse);
#endif
  /* iTmp = pDimMBuck->iNumShockSheetIntAll = iNumShock - iNumShockInit; */
  /* Total: Subsampled + Full Res Int. */
  /* pDimMBuck->iNumShockSheetV = iTmp; * Full Res. Int. only */
  pDimMBuck->iNumShockSheet  = iNumShock;

  return;
}

/* ---- EoF ---- */
