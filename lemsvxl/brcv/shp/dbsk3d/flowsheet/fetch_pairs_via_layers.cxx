/*------------------------------------------------------------*\
 * File: fetch_pairs_via_layers.c
 *
 * Functions:
 *  FetchBlueShocksExtOth
 *  FetchBlueShocksExtAll
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie (adapted from
 *    fetch_pairs_via_layers_v6.c)
 *  #1: Aug. 2003: Upgraded for use with MetaBuckets and
 *    shock validation via voxels.
 *    Moved in FetchBlueShocksExtOth() from
 *    fetch_shocks_oth_layers.c
 *
\*------------------------------------------------------------*/

#include "fetch_pairs_via_layers.h"
#include "blue_shocks_utils.h"

/*------------------------------------------------------------*\
 * Function: FetchBlueShocksExtOth
 *
 * Usage: Seek all isolated Sheet shocks - some are part of the MA
 *    and thus, the source of a shock sheet, others are only
 *    part of the SS - this BETWEEN buckets.
 *    Layers of neighboring buckets "around" each bucket
 *    OTHER THAN THE FIRST are visited here.
 *    Perform computations for ALL Shock Sources found thus far.
 *
\*------------------------------------------------------------*/

void
FetchBlueShocksExtOth(InputData *pInData, ShockScaffold *pScaf,
          short *pisLookUp, Buckets *pBucksXYZ,
          Pt3dCoord *pGeneCoord, Voxel *pVoxels,
          Dimension *pDimMBuck, Dimension *pDimAll)
{
  char    *pcFName = "FetchBlueShocksExtOth";
  register int  i, j, k, l, m;
  unsigned int  iNum, iNumBucks, iAcquire, iAcquireTotal, iMax;
  int    iNumShock, iMaxShock, iNumInvalid, iSliceSize;
  int    iNew, iPosXs,iPosYs,iPosZs,iPos, iNumGeneViz, iNumGeneInBuckA;
  int    iGeneA,iGeneB, iXdim, iNgbLast, iNumGeneInBuckB;
  int    iNumGeneInBuckC, iNumNgbs, iCorner, iFlagVisible, iAdrNgb;
  int    iNumShockExternal, iNewBlue, iNumNotVisible, iIteraMax;
  int    iItera, iIteraNext, iLabelBuckB, iFlagVisibleBuckB, iNgbFst;
  int    iNumGeneTransmitMax, iNumIteraNoNewShocks, iNumGeneASeeingB;
  int    iNgbNew, iGeneViz, iFlagAcquire, iNumGeneVizAdded, iNumNgbNew;
  int    iNewGeneVizToAdd, iNumGeneATransmit, iNumGeneTransmitTot;
  int    iNumAddNgb, iFlagNew, iGeneAb,iGeneBb, iNumDuplic;
  int    iNumDuplicTot, iNumDuplicCheck, iFlagVisibleGene, iOldBlue;
  int    iNumShockInit, iNewBlueTot, iBlueValid, iNumProxiA,iNumProxiB;
  int    iOldA,iOldB;
  int    *piGeneInBuckA, *piGeneInBuckB, *piNgbNew, *piNewNgbTmp, *piBlue;
  int    *piGeneASeeingBTmp, *piTmp, *piGeneVizTmp;
  int    *piTmpB, *piOldGeneViz;
  static float  fEpsilon = (float) D_EPSILON;
  float    fPosXa,fPosYa,fPosZa, fPosXb,fPosYb,fPosZb, fPx,fPy,fPz;
  float    fPosXs,fPosYs,fPosZs, fPABx,fPABy,fPABz, fDSq, fDist;
  //Ming:
  float   fDistSqLargest, fTimeWaisted = 0;

  //Ming
  float    fTimeSys = 0, fTimeUser = 0, fMemUse = 0, fTmpSys = 0, fTmpUser = 0;

  float    fPxMin,fPyMin,fPzMin, fPxMax,fPyMax,fPzMax;
  InputData  *pDataA, *pDataB;
  Pt3dCoord  *pGeneA, *pGeneB;
  Vector3d  *pvNorm;
  ShockSheet  *pBlue, *pOldBlue;
  Bucket  *pBuckA, *pBuckB, *pBuckC;
  NgbBuck  *pNgbVizNew, *pNgbViz, *pNgbVizOld;
#if SGI_ENV
  struct rusage  ru_start, ru_step, ru_end;
#endif
  ShockData  *pShockData;

  /* Preliminaries */

  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }
  pShockData->iGene[2] = pShockData->iGene[3] = IDLE;

  fprintf(stderr, "MESG(%s):\n", pcFName);
#if SGI_ENV
  if(getrusage(RUSAGE_SELF, &ru_start) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
  }
  fTimeSys = fTimeUser = fMemUse = fTimeWaisted = 0.0;
  fTmpSys  = (float) SECS(ru_start.ru_stime);
  fTmpUser = (float) SECS(ru_start.ru_utime);
#endif
  iXdim = pDimMBuck->iXdim;
  iSliceSize = pDimMBuck->iSliceSize;
  iNumBucks = pBucksXYZ->iNumBucks;

  iNumShockInit = iNumShock = pScaf->iBlueActiveLast;
  /* iMaxShock = pDimMBuck->iMaxBlueShocks; */
  iMaxShock = pScaf->iNumBlueMax;
  iNumInvalid = iNumIteraNoNewShocks = 0;
  pBlue = (pScaf->pBlue)+iNumShock;

  fDistSqLargest = 0.0;
#if FALSE
  fprintf(stderr, "\tSeeking remaining Sheets amongst %d genes,\n",
    pDimMBuck->iNum3dPts);
  fprintf(stderr, "\tprocessing %d BucketsXYZ,\n", iNumBucks);
  fprintf(stderr, "\tProcessing SECOND and FOLLOWING LAYERS of ngbs.\n");
  fprintf(stderr, "\tThere are %d sheet shocks already found.\n",
    iNumShockInit);
#endif

  /* --- Allocate Memory for Temporary structures --- */

  /* Will be used to grow ngb. in the main while loop */
  piNewNgbTmp = NULL;
  if((piNewNgbTmp = (int *)
      calloc(iNumBucks, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC on piNewNgbTmp[%d].\n",
      pcFName, iNumBucks);
    exit(-2); }
  /* Set Memory to Collect Visible Genes per BucketB per Iteration */
  iMax = pBucksXYZ->iMaxGenes;
  piGeneASeeingBTmp = NULL;
  if((piGeneASeeingBTmp = (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC on piGeneASeeingBTmp[%d].\n",
      pcFName, iMax);
    exit(-2); }
  /* Set Memory to Collect Additional Visible Genes for Ngb */
  piGeneVizTmp = NULL;
  if((piGeneVizTmp = (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC on piGeneVizTmp[%d].\n",
      pcFName, iMax);
    exit(-2); }

  /* ---------------- Grow BucketsXYZ Neighborhoods  --------------- *\
   *    Seek "external" pairs of generators.
  \*    Use visibility constraints to limit search.       */

  iNewBlueTot = iNewBlue = iNumNotVisible = iNumGeneVizAdded = 0;
  iItera = 1;
  iIteraNext = 2;
  /* Max number of layers of ngb. for a bucket */
  iIteraMax = pDimMBuck->iMaxItera - 1;

  iNumDuplicCheck = iNumDuplicTot = iNumDuplic = 0;
  iAcquireTotal = 1;

  while(iItera < iIteraMax && iAcquireTotal > 0) { /*** Main Loop ***/

    iNumGeneVizAdded = 0;
    iNumGeneTransmitTot = 0;
    iNumGeneTransmitMax = 0;
    iAcquireTotal = 0;
    iNumAddNgb = 0;

    /***** NB: Maybe maintain a list of active Buckets A *****/
    pBuckA = pBucksXYZ->pBucket-1;
    for(i = 0; i < iNumBucks; i++) { /* --- For each Bucket (A) --- */
      pBuckA++;

      iNumGeneInBuckA = pBuckA->isNumGenes;
      if(iNumGeneInBuckA < 1) {
  continue; }

      iAcquire = 0;
      iNumNgbs = pBuckA->piNgbPerItera[iItera];
      if(iNumNgbs < 1) { /* No more ngb. buckets ? */
  continue; } /* Skip : Goto next BuckA */

      iNgbFst = pBuckA->piFstNgb[iItera];
      iNgbLast = iNgbFst + iNumNgbs;

      /* ---- Go through the list of neighboring Buckets B ---- *\
      \*      obtained from previous layer of visited ngbs.     */

      for(j = iNgbFst; j < iNgbLast; j++) {

  pNgbViz = pBuckA->pNgbViz+j;
  
  iLabelBuckB = pNgbViz->isBuck;
  pBuckB = pBucksXYZ->pBucket+iLabelBuckB;
  iNumGeneInBuckB = pBuckB->isNumGenes;

  /* WARNING: If there is no gene in B, we are near    *\
   *  the border of the 3d space containing data.  *
   *  This is due to the way we build the buckets. *
   *  If we allow empty buckets in the "middle" of *
   *  that space, we will have to go through these *
   *  and hence not simply avoid these (as we may) *
   *  Empty buckets near the border tend to be     *
   *  elongated and generate many ngbs. to check   *
        \*  for the following iteration.         */

  /* if(iNumGeneInBuckB < 1) {
   *(pBuckA->pucFlagViz+iLabelBuckB) = 2;
   continue; } */

  if(iNumGeneInBuckB < 1) /* No Pairing needed */
    *(pBuckA->pucFlagPair+iLabelBuckB) = 2;

  iFlagVisibleBuckB = FALSE;
  /* Remains FALSE only if Non Visible from All GeneA */

  if(*(pBuckA->pucFlagViz+iLabelBuckB) > 1) { /* Not visible */
    /* WARNING: Means BuckB visited BuckA for visibility */
    /*  previously and found BuckA Not visible from B  */
    /*  and we assume this is a symmetrical relation,  */
    /*  which is not always true.           */
    continue; /* Go check next bucket ngb. : j++ */
  }

  /* ------- For each ngb. BuckB, Go through the ------- *\
  \*     list of GeneA transmitted thus far.          */

  iNumGeneASeeingB = 0;
  iNumGeneATransmit = pNgbViz->isNumGeneViz;
  piGeneInBuckA = pNgbViz->piGenes;

  for(k = 0; k < iNumGeneATransmit; k++) {

    iGeneA = *piGeneInBuckA++;
    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    fPosXa = pGeneA->fPosX;
    fPosYa = pGeneA->fPosY;
    fPosZa = pGeneA->fPosZ;

    /* -- 1st Check Visibility of BucketB w/r GeneA -- *\
     *    We only check if BucketB is ENTIRELY in      *
     *    the dead zone of a SINGLE Blue shock of      *
     *    GeneA. A more sophisticated solution would   *
     *    require to slice iteratively BucketB via     *
     *    each half-space and keep track of the       *
     *    the remaining visible boundaries of the       *
     *    bucket: these would be intercepts of the     *
    \*    slicing planes with the edges of bucket.     */

    iFlagVisible = TRUE;

    fPxMin = pBuckB->Limits.iMinX - fPosXa;
    fPyMin = pBuckB->Limits.iMinY - fPosYa;
    fPzMin = pBuckB->Limits.iMinZ - fPosZa;
    fPxMax = pBuckB->Limits.iMaxX - fPosXa;
    fPyMax = pBuckB->Limits.iMaxY - fPosYa;
    fPzMax = pBuckB->Limits.iMaxZ - fPosZa;

    pvNorm = pDataA->pvNormal-1;

    /* Number of Valid Blue Shocks thus far */
    iNum = pDataA->isNumBlue;

    /* For each known Blue Shock of GeneA */
    for(l = 0; l < iNum; l++) {
      pvNorm++;
      iCorner = 0; /* Label to 8 corners of Bucket */
      for(m = 0; m < 8; m++) { /* For each corner */
        switch(m) { /* Each of the 8 corners */
        case 0: /* Pk = (MinX , MinY , MinZ) */
    fPx = fPxMin;
    fPy = fPyMin;
    fPz = fPzMin;
    break;
        case 1: /* Pk = (MinX , MinY , MaxZ) */
    fPz = fPzMax;
    break;
        case 2: /* Pk = (MinX , MaxY , MaxZ) */
    fPy = fPyMax;
    break;
        case 3: /* Pk = (MinX , MaxY , MinZ) */
    fPz = fPzMin;
    break;
        case 4: /* Pk = (MaxX , MaxY , MinZ) */
    fPx = fPxMax;
    break;
        case 5: /* Pk = (MaxX , MaxY , MaxZ) */
    fPz = fPzMax;
    break;
        case 6: /* Pk = (MaxX , MinY , MaxZ) */
    fPy = fPyMin;
    break;
        case 7: /* Pk = (MaxX , MinY , MinZ) */
    fPz = fPzMin;
    break;
        }

        fDist = pvNorm->fX*fPx + pvNorm->fY*fPy + pvNorm->fZ*fPz
    - pvNorm->fLengthSq;
        if(fDist > 0.0) { /* Above plane ? */
    iCorner++;
        } /* Maybe use an Epsilon here or Robust arithm. */
        else {   /* At least 1 corner of BuckB is visible from GeneA */
    break; /* Get out of For(m) loop */
        }
      } /* Next corner : m++ */

      if(iCorner > 7) {
        iFlagVisible = FALSE;
        break; /* Get out of For(l) loop */
      }
    } /* Next shock associated to GeneA : l++ */

    if(iFlagVisible) { /* OK: BuckB is Visible from GeneA */
      iFlagVisibleBuckB = TRUE;

      /* Keep track of GeneA seeing BuckB */
      *(piGeneASeeingBTmp+iNumGeneASeeingB) = iGeneA;
      iNumGeneASeeingB++;

      /* ----- Check PAIRINGS if there are genes in B ----- *\
      \* ----- and pairing with A not done yet        ----- */

      if(iNumGeneInBuckB > 0 && *(pBuckB->pucFlagPair+i) < 2) {

        piGeneInBuckB = pBuckB->piGenes; /* Check Gene Pairings */
        for(l = 0; l < iNumGeneInBuckB; l++) {

    iGeneB = *piGeneInBuckB++;
    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    fPosXb = pGeneB->fPosX;
    fPosYb = pGeneB->fPosY;
    fPosZb = pGeneB->fPosZ;

    fPABx = fPosXb - fPosXa;
    fPABy = fPosYb - fPosYa;
    fPABz = fPosZb - fPosZa;

    /* --- 1st: Make sure GeneB sees GeneA -- */

    pvNorm = pDataA->pvNormal-1;
    /* Number of Valid Blue Shocks thus far */
    iNum = pDataA->isNumBlue;

    iFlagVisibleGene = TRUE;
    /* For each known Blue Shock of GeneA */
    for(m = 0; m < iNum; m++) {
      pvNorm++;
      fDist = pvNorm->fX*fPABx + pvNorm->fY*fPABy +
        pvNorm->fZ*fPABz - pvNorm->fLengthSq;
      /* if(fDist > 0.0) {  Above plane ? */
      if(fDist > fEpsilon) { /* Above plane ? */
        iFlagVisibleGene = FALSE;
        break; /* Get out of For(m) Loop */
      } /* Maybe use an Epsilon here or Robust arithm. */
    } /* m++ */
    if(!iFlagVisibleGene)
      continue; /* GeneA does not see GeneB : Goto next GeneB: l++ */

    /* -- Make sure this sheet does not already exist -- *\
     *    We need to compare with sheets linked to genes *
    \*    via backpointers. */

    iBlueValid = TRUE;
    iNumProxiA = pDataA->isNumBlue;
    iNumProxiB = pDataB->isNumBlue;
    if(iNumProxiA < iNumProxiB) {
      for(m = 0; m < iNumProxiA; m++) { /* sharing GeneA */
        iOldBlue = pDataA->piBlueList[m];
        pOldBlue = (pScaf->pBlue)+iOldBlue;
        iOldA = pOldBlue->iGene[0];
        iOldB = pOldBlue->iGene[1];
        if(iGeneB == iOldA || iGeneB == iOldB) {
          /* Already exist: Sharing both inputs */
          iBlueValid = FALSE;
          iNumDuplic++;
          break;
        }
      } /* m++ */
    }
    else { /* sharing B */
      for(m = 0; m < iNumProxiB; m++) {
        iOldBlue = pDataB->piBlueList[m];
        pOldBlue = (pScaf->pBlue)+iOldBlue;
        iOldA = pOldBlue->iGene[0];
        iOldB = pOldBlue->iGene[1];
        if(iGeneA == iOldA || iGeneA == iOldB) {
          /* Already exist: Sharing both inputs */
          iBlueValid = FALSE;
          iNumDuplic++;
          break;
        }
      } /* m++ */
    }
    if(!iBlueValid)
      continue; /* Already exist : Goto next GeneB: l++ */

    /* --- 2nd: GeneB sees GeneA : Construct Pair/Shock -- */

    fDSq = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
    fDSq *= 0.25;  /* Divided by half^2 = Radius^2 */

    fPosXs = 0.5 * (fPosXa + fPosXb);
    fPosYs = 0.5 * (fPosYa + fPosYb);
    fPosZs = 0.5 * (fPosZa + fPosZb);
#if HIGH_MEM
    pBlue->fDist2GeneSq = fDSq;
    /* Potential Shock coords. */
    pBlue->MidPt.fPosX = fPosXs;
    pBlue->MidPt.fPosY = fPosYs;
    pBlue->MidPt.fPosZ = fPosZs;
#endif
    /* -------------------- VALIDATE --------------------- */

    iPosXs = (int) floor((double) fPosXs);
    iPosYs = (int) floor((double) fPosYs);
    iPosZs = (int) floor((double) fPosZs);

    /* Find Bucket where shock is located */
    iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
    pBlue->iBuckXYZ = *(pisLookUp+iPos);

    pShockData->iGene[0] = pBlue->iGene[0] = iGeneA;
    pShockData->iGene[1] = pBlue->iGene[1] = iGeneB;

    pShockData->fDist2GeneSq = fDSq;
    pShockData->ShockCoord.fPosX = fPosXs;
    pShockData->ShockCoord.fPosY = fPosYs;
    pShockData->ShockCoord.fPosZ = fPosZs;

    if(!ValidBlueShockViaVoxels(pShockData, pInData, pGeneCoord,
              pVoxels, pDimAll)) {
      iNumInvalid++;
      continue; /* Part of SS only: Goto next pair : j++ */
    }

    /* ------------------ VALIDATION DONE ------------------ */

    /* If A & B paired before: Verify if duplicate */
    /* IS THIS NECESSARY ? */
    if(*(pBuckA->pucFlagPair+iLabelBuckB) > 1) {
      
      iNumDuplicCheck++;
      iFlagNew = TRUE;
      iNum = pDataA->isNumBlue;
      piBlue = pDataA->piBlueList;
      for(m = 0; m < iNum; m++) {
        iOldBlue = *piBlue++;
        pOldBlue = (pScaf->pBlue)+iOldBlue;
        iGeneAb = pOldBlue->iGene[0];
        iGeneBb = pOldBlue->iGene[1];
        if((iGeneAb == iGeneA || iGeneAb == iGeneB) &&
           (iGeneBb == iGeneA || iGeneBb == iGeneB)) {
          iFlagNew = FALSE;
          break; /* Get out of For(m) */
        }
      } /* m++ */

      if(!iFlagNew) {
        iNumDuplic++;
        continue; /* Pair already found */
      }
    }

    /* -------- Found a New Valid Sheet Shock --------- */

    if(fDSq > fDistSqLargest) fDistSqLargest = fDSq;

    /* Set Blue Shock Data */

    pBlue->isFlagValid = TRUE;
    pBlue->isInitType  = BLUE_INIT;
    pBlue->isStatus    = ACTIVE;
    pBlue->iGreen      = IDLE;
    pBlue->iRed        = IDLE;
    pBlue->isItera     = iIteraNext;

    /* Set Gene Data: Backpointers to shock & Normal data */

    iNum = pDataA->isNumBlue;
    iMax = pDataA->isMaxBlue - 2;
    if(iNum > iMax) {
      iMax = (iNum+1) * 2;
      if((pDataA->piBlueList =
          (int *) realloc((int *) pDataA->piBlueList,
              iMax * sizeof(int))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pDataA+%d)->piBlueList[%d]\n",
          pcFName, iGeneA, iNum);
        exit(-3); }
      if((pDataA->pvNormal =
          (Vector3d *) realloc((Vector3d *) pDataA->pvNormal,
             iMax * sizeof(Vector3d))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pDataA+%d)->pvNormal[%d]\n",
          pcFName, iGeneA, iNum);
        exit(-3); }
      pDataA->isMaxBlue = iMax;
      /* pDataA->iMaxNorm = iMax; */
    }
    /* Backpointer to shock */
    pDataA->piBlueList[iNum] = iNumShock;
    pDataA->isNumBlue++;

    pvNorm = pDataA->pvNormal+iNum;
    pvNorm->fX = fPABx;
    pvNorm->fY = fPABy;
    pvNorm->fZ = fPABz;
    pvNorm->fLengthSq = fDSq * 4.0;
    /* pDataA->isNumNorm++; */

    iNum = pDataB->isNumBlue;
    iMax = pDataB->isMaxBlue - 2;
    if(iNum > iMax) {
      iMax = (iNum+1) * 2;
      if((pDataB->piBlueList =
          (int *) realloc((int *) pDataB->piBlueList,
              iMax * sizeof(int))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pDataB+%d)->piBlueList[%d]\n",
          pcFName, iGeneB, iNum);
        exit(-3); }
      if((pDataB->pvNormal =
          (Vector3d *) realloc((Vector3d *) pDataB->pvNormal,
             iMax * sizeof(Vector3d))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pDataB+%d)->pvNormal[%d]\n",
          pcFName, iGeneB, iNum);
        exit(-3); }
      pDataB->isMaxBlue = iMax;
      /* pDataB->isMaxNorm = iMax; */
    }
    /* Backpointer to shock */
    pDataB->piBlueList[iNum] = iNumShock;
    pDataB->isNumBlue++;

    pvNorm = pDataB->pvNormal+iNum;
    pvNorm->fX = -fPABx;
    pvNorm->fY = -fPABy;
    pvNorm->fZ = -fPABz;
    pvNorm->fLengthSq = fDSq * 4.0;
    /* pDataB->isNumNorm++; */

    iNumShock++;
    iNewBlue++;
    pBlue++;

    if(iNumShock >= iMaxShock) {
      fprintf(stderr,
        "ERROR(%s): Overflow: Blue Shocks = %d >= Max = %d\n",
        pcFName, iNumShock, iMaxShock);
      exit(-5);
    }

        } /* Goto next 2nd gene in BuckB to create a new pair: l++ */
      }
      /* else : BuckB contains No gene or Paired with A already */
      /* break;  Get out of Loop(k) */
    } /* BuckB is visible from GeneA */
    /* else : BuckB is NOT visible from GeneA    *\
     *      We are not using this information *
    \*      later, but maybe we should/could. */

  } /* Goto next 1st gene in BuckA to create new pairs: k++ */

  /* ------------ All Viz Genes of BuckA visited ------------ *\
   *     If BuckB is Not Visible from ALL GeneA, we do NOT    *
   *     acquire new bucket neighbors linked to it.      *
   *     If it is visible, we try to acquire new neighbors.   *
   *     In both cases, we need to raise a flag for BuckB     *
  \*     to avoid pairing it latter with BuckA again.      */

  if(!iFlagVisibleBuckB) {
    iNumNotVisible++;
    /* If not paired before */
    if(*(pBuckA->pucFlagPair+iLabelBuckB) < 1) {
      /* B Not visible from A */
      *(pBuckA->pucFlagViz+iLabelBuckB) = 2;
      /* Pairing not needed */
      *(pBuckA->pucFlagPair+iLabelBuckB) = 2;

      if(iNumGeneInBuckB > 0) {
        *(pBuckB->pucFlagPair+i) = 2; /* No needs to Pair with A */
        /* WARNING: Following is not absolutely true */
        /* *(pBuckB->pucFlagViz+i) = 2; BuckA not visible from B */
      }
    }
    continue; /* Go check next bucket ngb. : j++ */
  }

  /* ------- Else: BuckB is visible from at least 1 GeneA ------- *\
  \*     Acquire New Ngbs & Transmit Visible Generators       */
  
  /* If not paired before */
  if(*(pBuckA->pucFlagPair+iLabelBuckB) < 1) {
    *(pBuckA->pucFlagViz+iLabelBuckB) = 1; /* BuckB Viz from A */
    *(pBuckA->pucFlagPair+iLabelBuckB) = 2; /* Pairings done */
    if(iNumGeneInBuckB > 0) {
      *(pBuckB->pucFlagPair+i) = 2; /* No needs to Pair with A */
    }
  }

  /* ----- Check immediate BuckB ngbs to propagate further ----- *\
   * 3 cases: (i) New ngb never visited from A before         *
   *     (ii) Not an immediate ngb of A, Been there (from A) *
   *          and not paired yet --> Check for new viz gene   *
   *    (iii) Like (ii) but paired already --> Check for new *
  \*    viz gene and re-open for pairing at next layer  */

  piNgbNew = pBuckB->piNgbs;
  iNumNgbNew = pBuckB->isNumNgbs;
  iNew = 0;
  for(k = 0; k < iNumNgbNew; k++) {

    iNgbNew = *piNgbNew++;
    if(iNgbNew == i) continue; /* Don't go back to A : k++ */

    /* --- Case (i) : New ngb --- */

    if(*(pBuckA->pucFlagNgb+iNgbNew) == 0) {
      /* Not Been there from BuckA yet */
      /* -- OK : Found a New Ngb bucket to acquire -- */
      *(piNewNgbTmp+iNew) = iNgbNew;
      iNew++;
      continue; /* k++ */
    }

    /* Position in pNgbViz , Default == 0 (void: ref. to A) */
    iAdrNgb = *(pBuckA->piPtrNgb+iNgbNew);

    /* --- Case (ii) : Same itera - Maybe add new Gene Viz --- */

    if(iAdrNgb > 0 && /* Not one of the nearest ngb. */
       *(pBuckA->pucFlagNgb+iNgbNew) > 0 &&  /* Been there */
       *(pBuckA->pucFlagPair+iNgbNew) < 1) { /* No pair yet */
      /* Then check for New Viz Genes */

      pNgbVizNew = pBuckA->pNgbViz+iAdrNgb;
      iNumGeneViz = pNgbVizNew->isNumGeneViz;

      iNewGeneVizToAdd = 0;
      if(iNumGeneViz < iNumGeneInBuckA) {
        piTmpB = piGeneVizTmp;
        piTmp = piGeneASeeingBTmp;
        for(l = 0; l < iNumGeneASeeingB; l++) {
    iGeneViz = *piTmp++;
    iFlagAcquire = TRUE;
    piOldGeneViz = pNgbVizNew->piGenes;
    for(m = 0; m < iNumGeneViz; m++) {
      if(iGeneViz == *piOldGeneViz++) {
        iFlagAcquire = FALSE;
        break; } /* Get out of For(m) loop */
    } /* m++ */
    if(iFlagAcquire) {
      *piTmpB++ = iGeneViz;
      iNewGeneVizToAdd++;
    }
        } /* l++ */
      }
      if(iNewGeneVizToAdd > 0) { /* New Viz GeneA to add */
        iMax = iNumGeneViz + iNewGeneVizToAdd;
        if((pNgbVizNew->piGenes =
      (int *) realloc((int *) pNgbVizNew->piGenes,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pNgbVizNew->piGenes[%d])\n",
      pcFName, iMax);
    exit(-3); }
        pNgbVizNew->isNumGeneViz = iMax;
        if(iMax > iNumGeneTransmitMax)
    iNumGeneTransmitMax = iMax;
        piTmpB = piGeneVizTmp;
        piTmp = pNgbVizNew->piGenes+iNumGeneViz;
        for(l = 0; l < iNewGeneVizToAdd; l++) {
    *piTmp++ = *piTmpB++;
        }
        iNumGeneTransmitTot += iNewGeneVizToAdd;
        pNgbVizNew->isNumVisit++;
        iNumGeneVizAdded++;
      }
      continue; /* k++ */
    } /* ---- End of Case (ii) ---- */

    /* --- Case (iii) : Later itera - Maybe re-open for pairing --- */

    if(iAdrNgb > 0 && /* Not one of the nearest ngb. */
       *(pBuckA->pucFlagNgb+iNgbNew) > 0 &&  /* Been there */
       *(pBuckA->pucFlagPair+iNgbNew) > 1) { /* Closed */
      /* Then check for New Viz Genes */
#if FALSE
      if((i == 905 || i == 902) &&
         (iNgbNew == 902 || iNgbNew == 905 || iNgbNew == 1011)) {
        fprintf(stderr,
          "MESG(%s): Reached Buck %d (%d) for re-opening.\n",
          pcFName, i, iNgbNew);
      }
#endif
      /* Already been there from A : Check it out */
      pNgbVizOld = pBuckA->pNgbViz+iAdrNgb;
      iNumGeneViz = pNgbVizOld->isNumGeneViz;

      iNewGeneVizToAdd = 0;
      if(iNumGeneViz < iNumGeneInBuckA) {
        piTmpB = piGeneVizTmp;
        piTmp = piGeneASeeingBTmp;
        for(l = 0; l < iNumGeneASeeingB; l++) {
    iGeneViz = *piTmp++;
    iFlagAcquire = TRUE;
    piOldGeneViz = pNgbVizOld->piGenes;
    for(m = 0; m < iNumGeneViz; m++) {
      if(iGeneViz == *piOldGeneViz++) {
        iFlagAcquire = FALSE;
        break; } /* Get out of For(m) loop */
    } /* m++ */
    if(iFlagAcquire) {
      *piTmpB++ = iGeneViz;
      iNewGeneVizToAdd++;
    }
        } /* l++ */
      }
      if(iNewGeneVizToAdd > 0) { /* New Viz GeneA to process */
        /* Add these to the list of Ngb representative on A */
        iMax = iNumGeneViz + iNewGeneVizToAdd;
        if((pNgbVizOld->piGenes =
      (int *) realloc((int *) pNgbVizOld->piGenes,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pNgbVizOld->piGenes[%d])\n",
      pcFName, iMax);
    exit(-3); }
        pNgbVizOld->isNumGeneViz = iMax;
        if(iMax > iNumGeneTransmitMax)
    iNumGeneTransmitMax = iMax;
        piTmpB = piGeneVizTmp;
        piTmp = pNgbVizOld->piGenes+iNumGeneViz;
        for(l = 0; l < iNewGeneVizToAdd; l++) {
    *piTmp++ = *piTmpB++;
        }
        iNumGeneTransmitTot += iNewGeneVizToAdd;
        pNgbVizOld->isNumVisit++;
        iNumGeneVizAdded++;

        /* -- Make an Additional copy of Ngb carrying Viz genes --*\
        \*    missed previously, to be processed at next itera   */

        iMax = pBuckA->isMaxNgbs + 1;
        if((pBuckA->pNgbViz =
      (NgbBuck *) realloc((NgbBuck *) pBuckA->pNgbViz,
              iMax * sizeof(NgbBuck))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pBuckA->pNgbViz[%d])\n",
      pcFName, iMax);
    exit(-3); }
        pBuckA->isMaxNgbs = iMax;

        iMax--;
        pNgbVizNew = pBuckA->pNgbViz+iMax;

        /* For this add. Ngb, transmit list of new viz GeneA */
      
        if((pNgbVizNew->piGenes = (int *)
      calloc(iNewGeneVizToAdd, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC pNgbVizNew->piGenes[%d]\n",
      pcFName, iNewGeneVizToAdd);
    exit(-3); }

        piTmpB = piGeneVizTmp;
        piTmp = pNgbVizNew->piGenes;
        for(l = 0; l < iNewGeneVizToAdd; l++) {
    *piTmp++ = *piTmpB++;
        }
        pNgbVizNew->isNumGeneViz = iNewGeneVizToAdd;
        pNgbVizNew->isBuck = iNgbNew;
        pNgbVizNew->isBuckFrom = iLabelBuckB;
        pNgbVizNew->isItera = iIteraNext;
        pNgbVizNew->isNumVisit = 1;
        
        iAcquire++;
        iNumAddNgb++;
        /* Re-open for pairings (at next itera) */
        /* *(pBuckA->pucFlagPair+iNgbNew) = 1; */

        pBuckC = pBucksXYZ->pBucket+iNgbNew;
        iNumGeneInBuckC = pBuckC->isNumGenes;
        if(iNumGeneInBuckC > 0)
    *(pBuckC->pucFlagPair+i) = 1;

      }
    } /* --- End of Case (iii) --- */

  } /* Check next surrounding ngb. : k++ */
  
  /* ------- If we Found New neighbors : ------- *\
  \*     Acquire these for next layer         */

  if(iNew > 0) {

    iAdrNgb = pBuckA->isMaxNgbs;
    iMax = iAdrNgb + iNew;
    if((pBuckA->pNgbViz =
        (NgbBuck *) realloc((NgbBuck *) pBuckA->pNgbViz,
          iMax * sizeof(NgbBuck))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pBuckA->pNgbViz[%d])\n",
        pcFName, iMax);
      fprintf(stderr,
        "\tA = %d, B = %d, iItera = %d, NewNgbs = %d\n",
        i, iLabelBuckB, iItera, iNew);
      exit(-3); }
    pBuckA->isMaxNgbs = iMax;
    
    pNgbVizNew = pBuckA->pNgbViz+iAdrNgb;
    for(k = 0; k < iNew; k++) {
      iNgbNew = *(piNewNgbTmp+k);    
      *(pBuckA->pucFlagNgb+iNgbNew) = 3;
      *(pBuckA->piPtrNgb+iNgbNew) = iAdrNgb;

      /* For each such new Ngb, transmit list of visible GeneA */
      
      if((pNgbVizNew->piGenes = (int *)
    calloc(iNumGeneASeeingB, sizeof(int))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): CALLOC pNgbVizNew->piGenes[%d]\n",
          pcFName, iNumGeneASeeingB);
        exit(-2); }

      piTmp = piGeneASeeingBTmp;
      for(l = 0; l < iNumGeneASeeingB; l++) {
        /* Keep track of genes */
        *(pNgbVizNew->piGenes+l) = *piTmp++;
      }
      pNgbVizNew->isNumGeneViz = iNumGeneASeeingB;

      if(iNumGeneASeeingB > iNumGeneTransmitMax)
        iNumGeneTransmitMax = iNumGeneASeeingB;
      iNumGeneTransmitTot += iNumGeneASeeingB;

      pNgbVizNew->isBuck = iNgbNew;
      pNgbVizNew->isBuckFrom = iLabelBuckB;
      pNgbVizNew->isItera = iIteraNext;
      pNgbVizNew->isNumVisit = 1;
      
      pNgbVizNew++;
      iAcquire++; iAdrNgb++;
    } /* Acquire next New Ngb : k++ */

  } /* Acquired New Ngbs. From BuckB */

      } /* Goto next Neighboring BuckB to visit : j++ */

      /* -- All ngb. BuckB at this Iteration are visited for BuckA -- */

      /* Ngb. newly acquired */
      pBuckA->piNgbPerItera[iIteraNext] = iAcquire;
      /* 1st Ngb. newly acquired */
      pBuckA->piFstNgb[iIteraNext] = iNgbLast;

      iAcquireTotal += iAcquire;
    } /* Goto next BuckA for this Initial iteration : i++ */

    fprintf(stderr, "\tIteration %d:\n", iItera);
#if SGI_ENV
    if(getrusage(RUSAGE_SELF, &ru_step) == -1) {
      fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
    }
    fTimeSys  = -fTmpSys;
    fTmpSys   = (float) SECS(ru_step.ru_stime);
    fTimeSys += fTmpSys;
    fTimeUser = -fTmpUser;
    fTmpUser  = (float) SECS(ru_step.ru_utime);
    fTimeUser += fTmpUser;
    fMemUse   = (float) (ru_step.ru_maxrss / 1000.0);
    fprintf(stderr,
      "\t\tRUSAGE: Sys = %.2f , User = %.2f , Mem = %.2f Meg.\n",
      fTimeSys, fTimeUser, fMemUse);
#endif
    fprintf(stderr, "\t\tNumber of Buckets Not Visible = %d\n",
      iNumNotVisible);
    fprintf(stderr, "\t\tNumber of New Valid Shocks = %d\n",
      iNewBlue);
    if(iNewBlue < 1) {
      iNumIteraNoNewShocks++;
      fTimeWaisted += fTimeSys + fTimeUser;
    }
    iNewBlueTot += iNewBlue;
    iNewBlue = iNumNotVisible = 0;
    fprintf(stderr, "\t\tTotal number of newly acquired neigbors = %d\n",
      iAcquireTotal);
    fprintf(stderr, "\t\tTotal number of times viz genes added = %d\n",
      iNumGeneVizAdded);
    fprintf(stderr, "\t\tTotal number of existing Ngb added = %d\n",
      iNumAddNgb);
    fprintf(stderr,
      "\t\tTotal of Transmitted genes = %d, Max per Ngb. = %d\n",
      iNumGeneTransmitTot, iNumGeneTransmitMax);
    fprintf(stderr, "\t\tNumber of duplicates of valid shocks = %d\n",
      iNumDuplic);

    iNumDuplicTot += iNumDuplic;
    iNumDuplic = 0;

    iItera++;
    iIteraNext++;
      
  } /* Next iteration in While Loop */
  
  /* -- Iterations over -- */

  if(iItera == iIteraMax) {
    fprintf(stderr, "\tIteraMax (%d) reached , ", iItera);
    fprintf(stderr, "iAcquire = %d\n", iAcquireTotal);
  }
  if(iNumShock > iMaxShock) {
    fprintf(stderr,
      "ERROR(%s):\n\tMax Num of shock exceeded: %d > %d.\n",
      pcFName, iNumShock, iMaxShock);
    exit(-3);
  }
#if SGI_ENV
  if(getrusage(RUSAGE_SELF, &ru_end) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage (end) failed.\n", pcFName);
  }
  fTimeSys  = (float) (SECS(ru_end.ru_stime) - SECS(ru_start.ru_stime));
  fTimeUser = (float) (SECS(ru_end.ru_utime) - SECS(ru_start.ru_utime));
  fMemUse   = (float) (ru_end.ru_maxrss / 1000.0);
#endif
  iNumShockExternal = iNumShock - iNumShockInit;
  pDimMBuck->iNumShockSheetExtAll = iNumShockExternal;
  pDimMBuck->iNumShockSheet = iNumShock;
  /* pDimMBuck->iNumShockSheetV = iNumShock; */

  if(iNumShockExternal != iNewBlueTot) {
    fprintf(stderr, "ERROR(%s): iNumShockExternal (%d) != iNewBlue (%d).\n",
      pcFName, iNumShockExternal, iNewBlueTot);
    exit(-4);
  }
  pScaf->iNumBlueActive += iNewBlueTot;
  pScaf->iBlueActiveLast += iNewBlueTot;
  pScaf->iNumBlueIsolated += iNewBlueTot;

  fprintf(stderr,
    "\t********* `External' pairs found (2nd layer and more) ********\n");
  fprintf(stderr,
    "\t\tRUSAGE: Sys = %.2f , User = %.2f , Mem = %.2f Meg.\n",
    fTimeSys, fTimeUser, fMemUse);
  fprintf(stderr, "\tTotal of %d VALID Blue shocks (Max = %d).\n",
    iNumShock, iMaxShock);
  fprintf(stderr, "\t\t%d were from previous iterations 0 (internal) and 1,\n",
    iNumShockInit);
  fprintf(stderr, "\t\t%d are from External layers 2 to %d (max).\n",
    iNumShockExternal, iItera);
  fprintf(stderr, "\t\tLargest distance between genes found = %.2f\n",
    (float) sqrt((double) fDistSqLargest));
  /* fprintf(stderr, "\tOffshoots = %d.\n", iOffShoots); */
  fprintf(stderr, "\t\tInvalid shocks = %d\n", iNumInvalid);
  fprintf(stderr, "\t\tDuplicates = %d , Checks = %d\n",
    iNumDuplicTot, iNumDuplicCheck);

  fprintf(stderr, "\t\tMaximal iteration reached = %d\n", iItera);
  fprintf(stderr, "\t\tIterations without New Shocks = %d\n",
    iNumIteraNoNewShocks);
  fprintf(stderr, "\t\tTime spent without finding new shocks = %.2f\n",
    fTimeWaisted);

  return;
}

/*------------------------------------------------------------*\
 * Function: FetchBlueShocksExtAll
 *
 * Usage: Seek remaining (isolated) Sheet shock sources - some are 
 *    part of the MA and thus, the source of a shock sheet,
 *    others are only part of the SS - BETWEEN buckets.
 *    Layers of neighboring buckets are grown iteratively
 *    "around" each bucket.
 *    Perform computations for ALL generators.
 *
\*------------------------------------------------------------*/

void
FetchBlueShocksExtAll(InputData *pInData, ShockScaffold *pScaf,
          short *pisLookUp, Buckets *pBucksXYZ,
          Pt3dCoord *pGeneCoord, Voxel *pVoxels,
          Dimension *pDimMBuck, Dimension *pDimAll)
{
  char    *pcFName = "FetchBlueShocksExtAll";
  register int  i, j, k, l, m;
  unsigned int  iNum, iNumBucks, iAcquire, iAcquireTotal, iMax, iNgbLast;
  int    iNum3dPts, iNumShock, iMaxShock, iNumInvalid, iLabel;
  int    iXdim, iSliceSize, iPosXs,iPosYs,iPosZs,iPos, iNumGeneViz;
  int    iNumGeneInBuckA, iGeneA,iGeneB, iNumGeneInBuckB;
  int    iNumNgbs, iCorner, iFlagVisible, iAdrNgb, iNumShockExternal;
  int          iNewBlue, iNumNotVisible, iIteraMax, iItera, iIteraNext;
  int    iLabelBuckB, iFlagVisibleBuckB, iNgbFst, iNumGeneInBuckC;
  int    iNumIteraNoNewShocks, iNew, iNumGeneASeeingB, iNgbNew;
  int    iGeneViz, iFlagAcquire, iNumGeneVizAdded, iNewGeneVizToAdd;
  int    iNumGeneATransmit, iNumGeneTransmitTot, iNumNgbNew;
  int    iNumGeneTransmitMax, iNumAddNgb, iFlagNew, iBlue;
  int    iGeneAb,iGeneBb, iNumDuplic, iNumDuplicTot, iNumDuplicCheck;
  int    iFlagVisibleGene, iNumShockInit, iSameBlue, iNumReset;
  int    *piGeneInBuckA, *piGeneInBuckB;
  int    *piNgbNew, *piNewNgbTmp, *piBlue;
  int    *piGeneASeeingBTmp, *piTmp, *piGeneVizTmp;
  int    *piTmpB, *piOldGeneViz;
  static float  fEpsilon = (float) D_EPSILON;
  float    fPosXa,fPosYa,fPosZa, fPosXb,fPosYb,fPosZb, fPx,fPy,fPz;
  float    fPosXs,fPosYs,fPosZs, fPABx,fPABy,fPABz, fDSq;
  float   fDistSqLargest;
  float    fDist, fTimeWaisted;

  //Ming
  float    fTimeSys = 0, fTimeUser = 0, fMemUse = 0, fTmpSys = 0, fTmpUser = 0;

  float    fPxMin,fPyMin,fPzMin, fPxMax,fPyMax,fPzMax;
  InputData  *pDataA, *pDataB;
  Pt3dCoord  *pGeneA, *pGeneB;
  Vector3d  *pvNorm;
  ShockSheet  *pBlue, *pOldBlue;
  Bucket  *pBuckA, *pBuckB, *pBuckC;
  NgbBuck  *pNgbVizNew, *pNgbViz, *pNgbVizOld;
#if SGI_ENV
  struct rusage  ru_start, ru_step, ru_end;
#endif
  ShockData  *pShockData;

  /* Preliminaries */

  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }
  pShockData->iGene[2] = pShockData->iGene[3] = IDLE;
#if SGI_ENV
  if(getrusage(RUSAGE_SELF, &ru_start) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
  }
  fTimeSys = fTimeUser = fMemUse = fTimeWaisted = 0.0;
  fTmpSys  = (float) SECS(ru_start.ru_stime);
  fTmpUser = (float) SECS(ru_start.ru_utime);
#endif
  iXdim = pDimMBuck->iXdim;
  iSliceSize = pDimMBuck->iSliceSize;
  iNumBucks = pBucksXYZ->iNumBucks;

  iNum3dPts = pDimMBuck->iNum3dPts;
  iNumShockInit = iNumShock = pScaf->iBlueActiveLast;
  /* iMaxShock = pDimMBuck->isMaxBlueShocks; */
  iMaxShock = pScaf->iNumBlueMax;
  iNumInvalid = iNumIteraNoNewShocks = iNumReset = iSameBlue = 0;

  /* --- Allocate Memory for Temporary structures --- */

  /* Will be used to grow ngb. in the main while loop */
  piNewNgbTmp = NULL;
  if((piNewNgbTmp = (int *)
      calloc(iNumBucks, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC on piNewNgbTmp[%d].\n",
      pcFName, iNumBucks);
    exit(-2); }
  /* Set Memory to Collect Visible Genes per BucketB per Iteration */
  iMax = pBucksXYZ->iMaxGenes;
  piGeneASeeingBTmp = NULL;
  if((piGeneASeeingBTmp = (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC on piGeneASeeingBTmp[%d].\n",
      pcFName, iMax);
    exit(-2); }
  /* Set Memory to Collect Additional Visible Genes for Ngb */
  piGeneVizTmp = NULL;
  if((piGeneVizTmp = (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC on piGeneVizTmp[%d].\n",
      pcFName, iMax);
    exit(-2); }

  fDistSqLargest = 0.0;
  iLabel = 0;

  fprintf(stderr,
    "MESG(%s):\n\tSeeking Sheet Shocks amongst %d genes,\n",
    pcFName, iNum3dPts);
  fprintf(stderr, "\tprocessing %d BucketsXYZ,\n", iNumBucks);


  /* ---------------- Grow BucketsXYZ Neighborhoods  --------------- *\
   *    Seek "external" pairs of generators.
  \*    Use visibility constraints to limit search.       */

  fprintf(stderr, "\t**** Find pairs `external' to each bucket ****\n");

  iNewBlue = iNumNotVisible = iNumGeneVizAdded = iItera = 0;
  iIteraNext = 1;

  /* -------------------- 1st : IteraNext 1 ------------------------ *\
  \* ------------ Visit immediate ngbs of each Bucket A ------------ */

  iAcquireTotal = 0;
  pBuckA = pBucksXYZ->pBucket-1;
  for(i = 0; i < iNumBucks; i++) { /* -- For each Bucket (A) -- */
    pBuckA++;
    iNumGeneInBuckA = pBuckA->isNumGenes;
    if(iNumGeneInBuckA < 1) {
      continue; }

    iAcquire = 0;
    iNumNgbs = pBuckA->isNumNgbs;

    /* ------- Go through the list of Direct Ngb Buckets B ------- */

    for(j = 0; j < iNumNgbs; j++) {
      iLabelBuckB = *(pBuckA->piNgbs+j);
      pBuckB = pBucksXYZ->pBucket+iLabelBuckB;
      iNumGeneInBuckB = pBuckB->isNumGenes;

      if(iNumGeneInBuckB < 1) /* No Pairing needed */
  *(pBuckA->pucFlagPair+iLabelBuckB) = 2;

      iFlagVisibleBuckB = FALSE;
      /* Remains FALSE only if Non Visible from All GeneA */

#if FALSE
      if(*(pBuckA->pucFlagViz+iLabelBuckB) > 1) { /* Not visible */
  /* WARNING: Means BuckB visited BuckA for visibility */
  /*  previously and found BuckA Not visible from B  */
  /*  and we assume this is a symmetrical relation,  */
  /*  which is not always true.           */
  continue; /* Go check next bucket ngb. : j++ */
      }
#endif

      /* ------- For each ngb. BuckB, Go through the ------- *\
      \*   list of GeneA which all see this BuckB       */

      iNumGeneASeeingB = 0;
      piGeneInBuckA = pBuckA->piGenes;
      for(k = 0; k < iNumGeneInBuckA; k++) {

  iGeneA = *piGeneInBuckA++;
  pDataA = pInData+iGeneA;
  pGeneA  = &(pDataA->Sample);
  fPosXa = pGeneA->fPosX;
  fPosYa = pGeneA->fPosY;
  fPosZa = pGeneA->fPosZ;

  /* --------- 1st Check Visibility of BucketB w/r GeneA --------- *\
   *    We only check if BucketB is ENTIRELY in the dead zone     *
   *    of a SINGLE Blue shock of GeneA. A more sophisticated      *
   *    solution would require to slice iteratively BucketB via   *
   *    each half-space and keep track of the remaining visible   *
   *    boundaries of the  bucket: these would be intercepts of    *
  \*    the slicing planes with the edges of bucket.    */

  iFlagVisible = TRUE;

  fPxMin = pBuckB->Limits.iMinX - fPosXa;
  fPyMin = pBuckB->Limits.iMinY - fPosYa;
  fPzMin = pBuckB->Limits.iMinZ - fPosZa;
  fPxMax = pBuckB->Limits.iMaxX - fPosXa;
  fPyMax = pBuckB->Limits.iMaxY - fPosYa;
  fPzMax = pBuckB->Limits.iMaxZ - fPosZa;

  pvNorm = pDataA->pvNormal-1;
  /* Number of Valid Blue Shocks thus far */
  iNum = pDataA->isNumBlue;

  /* For each known Blue Shock of GeneA */
  for(l = 0; l < iNum; l++) {
    pvNorm++;
    iCorner = 0; /* Label to 8 corners of Bucket */
    for(m = 0; m < 8; m++) { /* For each corner */
      switch(m) { /* Each of the 8 corners */
      case 0: /* Pk = (MinX , MinY , MinZ) */
        fPx = fPxMin;
        fPy = fPyMin;
        fPz = fPzMin;
        break;
      case 1: /* Pk = (MinX , MinY , MaxZ) */
        fPz = fPzMax;
        break;
      case 2: /* Pk = (MinX , MaxY , MaxZ) */
        fPy = fPyMax;
        break;
      case 3: /* Pk = (MinX , MaxY , MinZ) */
        fPz = fPzMin;
        break;
      case 4: /* Pk = (MaxX , MaxY , MinZ) */
        fPx = fPxMax;
        break;
      case 5: /* Pk = (MaxX , MaxY , MaxZ) */
        fPz = fPzMax;
        break;
      case 6: /* Pk = (MaxX , MinY , MaxZ) */
        fPy = fPyMin;
        break;
      case 7: /* Pk = (MaxX , MinY , MinZ) */
        fPz = fPzMin;
        break;
      }

      fDist = pvNorm->fX*fPx + pvNorm->fY*fPy + pvNorm->fZ*fPz
        - pvNorm->fLengthSq;
      if(fDist > 0.0) { /* Above plane ? */
        iCorner++;
      } /* Maybe use an Epsilon here or Robust arithm. */
      else {   /* At least 1 corner of BuckB is visible from GeneA */
        break; /* Get out of For(m) loop */
      }
    } /* Next corner : m++ */

    if(iCorner > 7) {
      iFlagVisible = FALSE;
      break; /* Get out of For(l) loop */
    }
  } /* Next shock associated to GeneA : l++ */

  if(iFlagVisible) { /* OK: BuckB is Visible from GeneA */
    iFlagVisibleBuckB = TRUE;

    /* Keep track of GeneA seeing BuckB */
    *(piGeneASeeingBTmp+iNumGeneASeeingB) = iGeneA;
    iNumGeneASeeingB++;

    /* ----- Check PAIRINGS if there are genes in B ----- *\
    \* ----- and pairing with A not done yet        ----- */

    if(iNumGeneInBuckB > 0 && *(pBuckB->pucFlagPair+i) < 1) {

      piGeneInBuckB = pBuckB->piGenes;
      for(l = 0; l < iNumGeneInBuckB; l++) {

        iGeneB  = *piGeneInBuckB++;
        pDataB = pInData+iGeneB;
        pGeneB  = &(pDataB->Sample);
        fPosXb = pGeneB->fPosX;
        fPosYb = pGeneB->fPosY;
        fPosZb = pGeneB->fPosZ;

        fPABx = fPosXb - fPosXa;
        fPABy = fPosYb - fPosYa;
        fPABz = fPosZb - fPosZa;

        /* --- 1st: Make sure GeneB sees GeneA -- */

        pvNorm = pDataA->pvNormal-1;
        /* Number of Valid Blue Shocks thus far */
        iNum = pDataA->isNumBlue;

        iFlagVisibleGene = TRUE;
        /* For each known Blue Shock of GeneA */
        for(m = 0; m < iNum; m++) {
    pvNorm++;
    fDist = pvNorm->fX*fPABx + pvNorm->fY*fPABy +
      pvNorm->fZ*fPABz - pvNorm->fLengthSq;
    /* if(fDist > 0.0) {  Above plane ? */
    if(fDist > fEpsilon) { /* Above plane ? */
      iFlagVisibleGene = FALSE;
      break; /* Get out of For(m) Loop */
    } /* Maybe use an Epsilon here or Robust arithm. */
        }

        if(!iFlagVisibleGene)
    continue; /* GeneA does not see GeneB : Goto next GeneB: l++ */

        pBlue = (pScaf->pBlue)+iNumShock;

        /* --- 2nd: GeneB sees GeneA : Construct Pair/Shock -- */

        fDSq = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
        fDSq *= 0.25;  /* Divided by half^2 = Radius^2 */

        fPosXs = 0.5 * (fPosXa + fPosXb);
        fPosYs = 0.5 * (fPosYa + fPosYb);
        fPosZs = 0.5 * (fPosZa + fPosZb);
#if HIGH_MEM
        pBlue->fDist2GeneSq = fDSq;
        /* Potential Shock coords. */
        pBlue->MidPt.fPosX = fPosXs;
        pBlue->MidPt.fPosY = fPosYs;
        pBlue->MidPt.fPosZ = fPosZs;
#endif
        /* -------------------- VALIDATE --------------------- */

        iLabel++; /* Increment label of potential shock */

        iPosXs = (int) floor((double) fPosXs);
        iPosYs = (int) floor((double) fPosYs);
        iPosZs = (int) floor((double) fPosZs);

        /* Find Bucket where shock is located */
        iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
        pBlue->iBuckXYZ = *(pisLookUp+iPos);

        pShockData->iGene[0] = pBlue->iGene[0] = iGeneA;
        pShockData->iGene[1] = pBlue->iGene[1] = iGeneB;

        pShockData->fDist2GeneSq = fDSq;
        pShockData->ShockCoord.fPosX = fPosXs;
        pShockData->ShockCoord.fPosY = fPosYs;
        pShockData->ShockCoord.fPosZ = fPosZs;

        if(!ValidBlueShockViaVoxels(pShockData, pInData, pGeneCoord,
            pVoxels, pDimAll)) {
    iNumInvalid++;
    continue; /* Part of SS only: Goto next pair : j++ */
        }

        /* -------- Found a New Valid Sheet Shock --------- */

        if(fDSq > fDistSqLargest) fDistSqLargest = fDSq;

        pBlue->isFlagValid = TRUE;
        pBlue->isInitType  = BLUE_INIT;
        pBlue->isStatus    = IDLE;
        pBlue->iGreen      = IDLE;
        pBlue->iRed        = IDLE;
        pBlue->isItera     = iIteraNext;

        /* Set Gene Data: Backpointers to shock and Normal data */

        iNum = pDataA->isNumBlue;
        iMax = pDataA->isMaxBlue - 2;
        if(iNum > iMax) {
    iMax = (iNum+1) * 2;
    if((pDataA->piBlueList =
        (int *) realloc((int *) pDataA->piBlueList,
            iMax * sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pDataA+%d)->piBlueList[%d]\n",
        pcFName, iGeneA, iNum);
      exit(-3); }
    if((pDataA->pvNormal =
        (Vector3d *) realloc((Vector3d *) pDataA->pvNormal,
           iMax * sizeof(Vector3d))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pDataA+%d)->pvNormal[%d]\n",
        pcFName, iGeneA, iNum);
      exit(-3); }
    pDataA->isMaxBlue = iMax;
    /* pDataA->isMaxNorm = iMax; */
        }

        /* Backpointer to shock */
        pDataA->piBlueList[iNum] = iNumShock;

        pDataA->isNumBlue++;
        pvNorm = pDataA->pvNormal+iNum;
        pvNorm->fX = fPABx;
        pvNorm->fY = fPABy;
        pvNorm->fZ = fPABz;
        pvNorm->fLengthSq = fDSq * 4.0;
        /* pDataA->isNumNorm++; */

        iNum = pDataB->isNumBlue;
        iMax = pDataB->isMaxBlue;
        if(iNum >= iMax) {
    iMax = (iNum+1) * 2;
    if((pDataB->piBlueList =
        (int *) realloc((int *) pDataB->piBlueList,
            iMax * sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pDataB+%d)->piBlueList[%d]\n",
        pcFName, iGeneB, iNum);
      exit(-3); }
    if((pDataB->pvNormal =
        (Vector3d *) realloc((Vector3d *) pDataB->pvNormal,
           iMax * sizeof(Vector3d))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pDataB+%d)->pvNormal[%d].\n",
        pcFName, iGeneB, iNum);
      exit(-3); }
    pDataB->isMaxBlue = iMax;
    /* pDataB->isMaxNorm = iMax; */
        }

        /* Backpointer to shock */
        pDataB->piBlueList[iNum] = iNumShock;

        pDataB->isNumBlue++;
        pvNorm = pDataB->pvNormal+iNum;
        pvNorm->fX = -fPABx;
        pvNorm->fY = -fPABy;
        pvNorm->fZ = -fPABz;
        pvNorm->fLengthSq = fDSq * 4.0;
        /* pDataB->isNumNorm++; */

        iNumShock++;
        iNewBlue++;

      } /* Goto next 2nd gene in BuckB to create a new pair: l++ */
    }
    /* else : BuckB contains No gene or Has been paired to A already */
    /* break; Get out of Loop(k) */
  } /* BuckB is visible from GeneA */
  /* else : BuckB is NOT visible from GeneA    *\
         *    We are not using this information *
        \*    later, but maybe we should/could. */

      } /* Goto next GeneA: Check Viz w/r B, then test new Pairs: k++ */

      /* ------------ All Genes of BuckA visited -------------- *\
       *     If BuckB is Not Visible from ALL GeneA, we do NOT  *
       *     acquire new bucket neighbors linked to it.    *
       *     If it is visible, we try to acquire new neighbors. *
       *     In both cases, we need to raise a flag for BuckB   *
      \*     to Avoid Pairing it latter with BuckA again.  */

      if(!iFlagVisibleBuckB) {
  iNumNotVisible++;

  *(pBuckA->pucFlagViz+iLabelBuckB)  = 2; /* B Not viz from A */
  *(pBuckA->pucFlagPair+iLabelBuckB) = 2; /* Pair not needed */

  if(iNumGeneInBuckB > 0) {
    *(pBuckB->pucFlagPair+i) = 2; /* No needs for Pairing with A */
    /* WARNING: Following is not absolutely true */
    /* *(pBuckB->pucFlagViz+i) = 2; BuckA not viz from B */
  }
  continue; /* Go check next ngb. Bucket B : j++ */
      }

      /* ------- Else: BuckB is visible from at least 1 GeneA ------- *\
      \*   Acquire new Ngbs & transmit genes visible         */

      *(pBuckA->pucFlagViz+iLabelBuckB)  = 1; /* BuckB viz from A */
      *(pBuckA->pucFlagPair+iLabelBuckB) = 2; /* Pairings done */
      pBuckA->isNumPair++;
      if(iNumGeneInBuckB > 0) {
  /* No needs for Pairing with A again */
  *(pBuckB->pucFlagPair+i) = 2;
      }

      /* -- Check immediate neighbors of BuckB to propagate further -- */

      piNgbNew = pBuckB->piNgbs;
      iNumNgbNew = pBuckB->isNumNgbs;
      iNew = 0;
      for(k = 0; k < iNumNgbNew; k++) {
  iNgbNew = *piNgbNew++;
  if(iNgbNew == i) continue; /* Don't go back to A : k++ */

  /* Position in pNgbViz */
  iAdrNgb = *(pBuckA->piPtrNgb+iNgbNew);
  if(iAdrNgb > 0 && /* Not one of the nearest ngb. */
     *(pBuckA->pucFlagNgb+iNgbNew) > 0 && /* Been there */
     *(pBuckA->pucFlagPair+iNgbNew) < 1) { /* But no pair yet */
    /* Then check for New Viz Genes */

    pNgbVizNew = pBuckA->pNgbViz+iAdrNgb;
    iNumGeneViz = pNgbVizNew->isNumGeneViz;

    iNewGeneVizToAdd = 0;
    if(iNumGeneViz < iNumGeneInBuckA) {
      piTmpB = piGeneVizTmp;
      piTmp = piGeneASeeingBTmp;
      for(l = 0; l < iNumGeneASeeingB; l++) {
        iGeneViz = *piTmp++;
        iFlagAcquire = TRUE;
        piOldGeneViz = pNgbVizNew->piGenes;
        for(m = 0; m < iNumGeneViz; m++) {
    if(iGeneViz == *piOldGeneViz++) {
      iFlagAcquire = FALSE;
      break; } /* Get out of For(m) loop */
        } /* m++ */
        if(iFlagAcquire) {
    *piTmpB++ = iGeneViz;
    iNewGeneVizToAdd++;    
        }
      } /* l++ */
    }

    if(iNewGeneVizToAdd > 0) { /* New Viz GeneA to add */

      iMax = iNumGeneViz + iNewGeneVizToAdd;
      if((pNgbVizNew->piGenes =
    (int *) realloc((int *) pNgbVizNew->piGenes,
        iMax * sizeof(int))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pNgbVizNew->piGenes[%d])\n",
          pcFName, iMax);
        exit(-3); }
      pNgbVizNew->isNumGeneViz = iMax;

      piTmpB = piGeneVizTmp;
      piTmp = pNgbVizNew->piGenes+iNumGeneViz;
      for(l = 0; l < iNewGeneVizToAdd; l++) {
        *piTmp++ = *piTmpB++;
      }

      pNgbVizNew->isNumVisit++;
      iNumGeneVizAdded++;
    }

    continue; /* k++ */
  }

  if(*(pBuckA->pucFlagNgb+iNgbNew) == 0) {
    /* Not Been there from BuckA yet */
    /* -- OK : Found a New Ngb bucket to acquire -- */
    *(piNewNgbTmp+iNew) = iNgbNew;
    iNew++;
  }

      } /* Check next surrounding ngb. : k++ */

      /* --- Found New neighbors ? Acquire these for next layer --- */

      if(iNew > 0) {

  iAdrNgb = pBuckA->isMaxNgbs;
  iMax = iAdrNgb + iNew;
  if((pBuckA->pNgbViz =
      (NgbBuck *) realloc((NgbBuck *) pBuckA->pNgbViz,
        iMax * sizeof(NgbBuck))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pBuckA->pNgbViz[%d])\n",
      pcFName, iMax);
    fprintf(stderr,
      "\tfor Bucks A=%d, B=%d, iItera = %d, NewNgbs = %d\n",
      i, iLabelBuckB, iItera, iNew);
    exit(-3); }
  pBuckA->isMaxNgbs = iMax;

  pNgbVizNew = pBuckA->pNgbViz+iAdrNgb;
  for(k = 0; k < iNew; k++) {
    iNgbNew = *(piNewNgbTmp+k);    
    *(pBuckA->pucFlagNgb+iNgbNew) = 3;
    *(pBuckA->piPtrNgb+iNgbNew) = iAdrNgb;

    /* For each such new Ngb, transmit list of visible GeneA */

    if((pNgbVizNew->piGenes = (int *)
        calloc(iNumGeneASeeingB, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC on pNgbVizNew->piGenes[%d].\n",
        pcFName, iNumGeneASeeingB);
      exit(-2); }

    piTmp = piGeneASeeingBTmp;
    for(l = 0; l < iNumGeneASeeingB; l++) {
      /* Keep track of genes */
      *(pNgbVizNew->piGenes+l) = *piTmp++;
    }
    pNgbVizNew->isNumGeneViz = iNumGeneASeeingB;
    pNgbVizNew->isBuck = iNgbNew;
    pNgbVizNew->isBuckFrom = iLabelBuckB;
    pNgbVizNew->isItera = iIteraNext;
    pNgbVizNew->isNumVisit = 1;

    pNgbVizNew++;
    iAcquire++; iAdrNgb++;
  } /* Acquire next New Ngb : k++ */

      } /* Acquired New Ngbs. From BuckB */

    } /* Goto next Neighboring BuckB to visit : j++ */

    /* -- All ngb. BuckB at the Initial Iteration are visited for BuckA -- */

    pBuckA->piNgbPerItera[1] = iAcquire; /* Ngb. newly acquired */
    if(iAcquire > 0) 
      pBuckA->piFstNgb[1] = 1; /* 1st Ngb. newly acquired */
    iAcquireTotal += iAcquire;

  } /* Goto next BuckA for this Initial iteration : i++ */

  /* ---------------- Initial Iteration Done ---------------------- */

  fprintf(stderr, "\tInitial Iteration:\n");
#if SGI_ENV
  if(getrusage(RUSAGE_SELF, &ru_step) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
  }
  fTimeSys  = -fTmpSys;
  fTmpSys   = (float) SECS(ru_step.ru_stime);
  fTimeSys += fTmpSys;
  fTimeUser = -fTmpUser;
  fTmpUser  = (float) SECS(ru_step.ru_utime);
  fTimeUser += fTmpUser;
  fMemUse   = (float) (ru_step.ru_maxrss / 1000.0);
  fprintf(stderr,
    "\t\tRUSAGE: Sys = %.2f , User = %.2f , Mem = %.2f Meg.\n",
    fTimeSys, fTimeUser, fMemUse);
#endif
  fprintf(stderr, "\t\tNumber of Buckets Not Visible = %d\n",
    iNumNotVisible);
  fprintf(stderr, "\t\tNumber of New Valid Shocks = %d\n", iNewBlue);
  if(iNewBlue < 1) {
    iNumIteraNoNewShocks++;
    fTimeWaisted += fTimeSys + fTimeUser;
  }
  iNewBlue = iNumNotVisible = 0;
  fprintf(stderr, "\t\tTotal number of newly acquired neigbors = %d\n",
    iAcquireTotal);
  fprintf(stderr, "\t\tTotal number of times visible genes added = %d\n",
    iNumGeneVizAdded);
  fprintf(stderr,
    "\t%d Duplicates from Sub.Res., %d of which are now InitBlue.\n",
    iSameBlue, iNumReset);

  iSameBlue = iNumReset = 0;

  /* ------------------ 2nd : Iteration loop ----------------------- *\
  \* ------------ Visit ngbs of each Bucket A by layers ------------ */

  iItera++;
  iIteraNext++;
  /* Max number of layers of ngb. for a bucket */
  iIteraMax = pDimMBuck->iMaxItera - 1;

  iNumDuplicCheck = iNumDuplicTot = iNumDuplic = 0;

  while(iItera < iIteraMax && iAcquireTotal > 0) { /*** Main Loop ***/

    iNumGeneVizAdded = 0;
    iNumGeneTransmitTot = 0;
    iNumGeneTransmitMax = 0;
    iAcquireTotal = 0;
    iNumAddNgb = 0;

    /***** NB: Maybe maintain a list of active Buckets A *****/
    pBuckA = pBucksXYZ->pBucket-1;
    for(i = 0; i < iNumBucks; i++) { /* --- For each Bucket (A) --- */
      pBuckA++;

      iNumGeneInBuckA = pBuckA->isNumGenes;
      if(iNumGeneInBuckA < 1) {
  continue; }

      iAcquire = 0;
      iNumNgbs = pBuckA->piNgbPerItera[iItera];
      if(iNumNgbs < 1) { /* No more ngb. buckets ? */
  continue; } /* Skip : Goto next BuckA */

      iNgbFst = pBuckA->piFstNgb[iItera];
      iNgbLast = iNgbFst + iNumNgbs;

      /* ---- Go through the list of neighboring Buckets B ---- *\
      \*      obtained from previous layer of visited ngbs.     */

      for(j = iNgbFst; j < iNgbLast; j++) {

  pNgbViz = pBuckA->pNgbViz+j;
  
  iLabelBuckB = pNgbViz->isBuck;
  pBuckB = pBucksXYZ->pBucket+iLabelBuckB;
  iNumGeneInBuckB = pBuckB->isNumGenes;

  /* WARNING: If there is no gene in B, we are near    *\
   *  the border of the 3d space containing data.  *
   *  This is due to the way we build the buckets. *
   *  If we allow empty buckets in the "middle" of *
   *  that space, we will have to go through these *
   *  and hence not simply avoid these (as we may) *
   *  Empty buckets near the border tend to be     *
   *  elongated and generate many ngbs. to check   *
        \*  for the following iteration.         */

  /* if(iNumGeneInBuckB < 1) {
   *(pBuckA->pucFlagViz+iLabelBuckB) = 2;
   continue; } */

  if(iNumGeneInBuckB < 1) /* No Pairing needed */
    *(pBuckA->pucFlagPair+iLabelBuckB) = 2;

  iFlagVisibleBuckB = FALSE;
  /* Remains FALSE only if Non Visible from All GeneA */

  if(*(pBuckA->pucFlagViz+iLabelBuckB) > 1) { /* Not visible */
    /* WARNING: Means BuckB visited BuckA for visibility */
    /*  previously and found BuckA Not visible from B  */
    /*  and we assume this is a symmetrical relation,  */
    /*  which is not always true.           */
    continue; /* Go check next bucket ngb. : j++ */
  }

  /* ------- For each ngb. BuckB, Go through the ------- *\
  \*     list of GeneA transmitted thus far.          */

  iNumGeneASeeingB = 0;
  iNumGeneATransmit = pNgbViz->isNumGeneViz;
  piGeneInBuckA = pNgbViz->piGenes;

  for(k = 0; k < iNumGeneATransmit; k++) {

    iGeneA = *piGeneInBuckA++;
    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    fPosXa = pGeneA->fPosX;
    fPosYa = pGeneA->fPosY;
    fPosZa = pGeneA->fPosZ;

    /* -- 1st Check Visibility of BucketB w/r GeneA -- *\
     *    We only check if BucketB is ENTIRELY in     *
     *    the dead zone of a SINGLE Blue shock of     *
     *    GeneA. A more sophisticated solution would   *
     *    require to slice iteratively BucketB via    *
     *    each half-space and keep track of the      *
     *    the remaining visible boundaries of the      *
     *    bucket: these would be intercepts of the    *
    \*    slicing planes with the edges of bucket.    */

    iFlagVisible = TRUE;

    fPxMin = pBuckB->Limits.iMinX - fPosXa;
    fPyMin = pBuckB->Limits.iMinY - fPosYa;
    fPzMin = pBuckB->Limits.iMinZ - fPosZa;
    fPxMax = pBuckB->Limits.iMaxX - fPosXa;
    fPyMax = pBuckB->Limits.iMaxY - fPosYa;
    fPzMax = pBuckB->Limits.iMaxZ - fPosZa;

    pvNorm = pDataA->pvNormal-1;

    /* Number of Valid Blue Shocks thus far */
    iNum = pDataA->isNumBlue;

    /* For each known Blue Shock of GeneA */
    for(l = 0; l < iNum; l++) {
      pvNorm++;
      iCorner = 0; /* Label to 8 corners of Bucket */
      for(m = 0; m < 8; m++) { /* For each corner */
        switch(m) { /* Each of the 8 corners */
        case 0: /* Pk = (MinX , MinY , MinZ) */
    fPx = fPxMin;
    fPy = fPyMin;
    fPz = fPzMin;
    break;
        case 1: /* Pk = (MinX , MinY , MaxZ) */
    fPz = fPzMax;
    break;
        case 2: /* Pk = (MinX , MaxY , MaxZ) */
    fPy = fPyMax;
    break;
        case 3: /* Pk = (MinX , MaxY , MinZ) */
    fPz = fPzMin;
    break;
        case 4: /* Pk = (MaxX , MaxY , MinZ) */
    fPx = fPxMax;
    break;
        case 5: /* Pk = (MaxX , MaxY , MaxZ) */
    fPz = fPzMax;
    break;
        case 6: /* Pk = (MaxX , MinY , MaxZ) */
    fPy = fPyMin;
    break;
        case 7: /* Pk = (MaxX , MinY , MinZ) */
    fPz = fPzMin;
    break;
        }

        fDist = pvNorm->fX*fPx + pvNorm->fY*fPy + pvNorm->fZ*fPz
    - pvNorm->fLengthSq;
        if(fDist > 0.0) { /* Above plane ? */
    iCorner++;
        } /* Maybe use an Epsilon here or Robust arithm. */
        else {   /* At least 1 corner of BuckB is visible from GeneA */
    break; /* Get out of For(m) loop */
        }
      } /* Next corner : m++ */

      if(iCorner > 7) {
        iFlagVisible = FALSE;
        break; /* Get out of For(l) loop */
      }
    } /* Next shock associated to GeneA : l++ */

    if(iFlagVisible) { /* OK: BuckB is Visible from GeneA */
      iFlagVisibleBuckB = TRUE;

      /* Keep track of GeneA seeing BuckB */
      *(piGeneASeeingBTmp+iNumGeneASeeingB) = iGeneA;
      iNumGeneASeeingB++;

      /* ----- Check PAIRINGS if there are genes in B ----- *\
      \* ----- and pairing with A not done yet        ----- */

      if(iNumGeneInBuckB > 0 && *(pBuckB->pucFlagPair+i) < 2) {

        piGeneInBuckB = pBuckB->piGenes; /* Check gene pairings */
        for(l = 0; l < iNumGeneInBuckB; l++) {

    iGeneB = *piGeneInBuckB++;
    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    fPosXb = pGeneB->fPosX;
    fPosYb = pGeneB->fPosY;
    fPosZb = pGeneB->fPosZ;

    fPABx = fPosXb - fPosXa;
    fPABy = fPosYb - fPosYa;
    fPABz = fPosZb - fPosZa;

    /* --- 1st: Make sure GeneB sees GeneA -- */

    pvNorm = pDataA->pvNormal-1;
    /* Number of Valid Blue Shocks thus far */
    iNum = pDataA->isNumBlue;

    iFlagVisibleGene = TRUE;
    /* For each known Blue Shock of GeneA */
    for(m = 0; m < iNum; m++) {
      pvNorm++;
      fDist = pvNorm->fX*fPABx + pvNorm->fY*fPABy +
        pvNorm->fZ*fPABz - pvNorm->fLengthSq;
      /* if(fDist > 0.0) {  Above plane ? */
      if(fDist > fEpsilon) { /* Above plane ? */
        iFlagVisibleGene = FALSE;
        break; /* Get out of For(m) Loop */
      } /* Maybe use an Epsilon here or Robust arithm. */
    }
    if(!iFlagVisibleGene)
      continue; /* GeneA does not see GeneB : Goto next GeneB: l++ */

    pBlue = (pScaf->pBlue)+iNumShock;

    /* --- 2nd: GeneB sees GeneA : Construct Pair/Shock -- */

    fDSq  = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
    fDSq *= 0.25;  /* Divided by half^2 = Radius^2 */

    fPosXs = 0.5 * (fPosXa + fPosXb);
    fPosYs = 0.5 * (fPosYa + fPosYb);
    fPosZs = 0.5 * (fPosZa + fPosZb);
#if HIGH_MEM
    pBlue->fDist2GeneSq = fDSq;
    /* Potential Shock coords. */
    pBlue->MidPt.fPosX = fPosXs;
    pBlue->MidPt.fPosY = fPosYs;
    pBlue->MidPt.fPosZ = fPosZs;
#endif
    /* -------------------- VALIDATE --------------------- */

    iLabel++; /* Increment label of potential shock */

    iPosXs = (int) floor((double) fPosXs);
    iPosYs = (int) floor((double) fPosYs);
    iPosZs = (int) floor((double) fPosZs);

    /* Find Bucket where shock is located */
    iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
    pBlue->iBuckXYZ = *(pisLookUp+iPos);

    pShockData->iGene[0] = pBlue->iGene[0] = iGeneA;
    pShockData->iGene[1] = pBlue->iGene[1] = iGeneB;

    pShockData->fDist2GeneSq = fDSq;
    pShockData->ShockCoord.fPosX = fPosXs;
    pShockData->ShockCoord.fPosY = fPosYs;
    pShockData->ShockCoord.fPosZ = fPosZs;

    if(!ValidBlueShockViaVoxels(pShockData, pInData, pGeneCoord,
              pVoxels, pDimAll)) {
      iNumInvalid++;
      continue; /* Part of SS only: Goto next pair : j++ */
    }

    /* If A & B paired before: Verify if duplicate */
    if(*(pBuckA->pucFlagPair+iLabelBuckB) > 1) {
      
      iNumDuplicCheck++;
      iFlagNew = TRUE;
      iNum = pDataA->isNumBlue;
      piBlue = pDataA->piBlueList;
      for(m = 0; m < iNum; m++) {
        iBlue = *piBlue++;
        pOldBlue = (pScaf->pBlue)+iBlue;
        iGeneAb = pOldBlue->iGene[0];
        iGeneBb = pOldBlue->iGene[1];
        if((iGeneAb == iGeneA || iGeneAb == iGeneB) &&
           (iGeneBb == iGeneA || iGeneBb == iGeneB)) {
          iFlagNew = FALSE;
          break; /* Get out of For(m) */
        }
      } /* m++ */

      if(!iFlagNew) {
        iNumDuplic++;
        continue; /* Pair already found */
      }
    }

    /* -------- Found a New Valid Sheet Shock --------- */

    if(fDSq > fDistSqLargest) fDistSqLargest = fDSq;

    /* Set Blue Shock Data */

    pBlue->isFlagValid = TRUE;
    pBlue->isInitType  = BLUE_INIT;
    pBlue->isStatus    = IDLE;
    pBlue->iGreen      = IDLE;
    pBlue->iRed        = IDLE;
    pBlue->isItera     = iIteraNext;

    /* Set Gene Data: Backpointers to shock & Normal data */

    iNum = pDataA->isNumBlue;
    iMax = pDataA->isMaxBlue;
    if(iNum >= iMax) {
      iMax = (iNum+1) * 2;
      if((pDataA->piBlueList =
          (int *) realloc((int *) pDataA->piBlueList,
              iMax * sizeof(int))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pDataA+%d)->piBlueList[%d]\n",
          pcFName, iGeneA, iNum);
        exit(-3); }
      if((pDataA->pvNormal =
          (Vector3d *) realloc((Vector3d *) pDataA->pvNormal,
             iMax * sizeof(Vector3d))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pDataA+%d)->pvNormal[%d]\n",
          pcFName, iGeneA, iNum);
        exit(-3); }
      pDataA->isMaxBlue = iMax;
      /* pDataA->isMaxNorm = iMax; */
    }

    /* Backpointer to shock */
    pDataA->piBlueList[iNum] = iNumShock;
    pDataA->isNumBlue++;
    pvNorm = pDataA->pvNormal+iNum;
    pvNorm->fX = fPABx;
    pvNorm->fY = fPABy;
    pvNorm->fZ = fPABz;
    pvNorm->fLengthSq = fDSq * 4.0;
    /* pDataA->isNumNorm++; */

    iNum = pDataB->isNumBlue;
    iMax = pDataB->isMaxBlue;
    if(iNum >= iMax) {
      iMax = (iNum+1) * 2;
      if((pDataB->piBlueList =
          (int *) realloc((int *) pDataB->piBlueList,
              iMax * sizeof(int))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pDataB+%d)->piBlueList[%d]\n",
          pcFName, iGeneB, iNum);
        exit(-3); }
      if((pDataB->pvNormal =
          (Vector3d *) realloc((Vector3d *) pDataB->pvNormal,
             iMax * sizeof(Vector3d))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pDataB+%d)->pvNormal[%d]\n",
          pcFName, iGeneB, iNum);
        exit(-3); }
      pDataB->isMaxBlue = iMax;
      /* pDataB->isMaxNorm = iMax; */
    }

    /* Backpointer to shock */
    pDataB->piBlueList[iNum] = iNumShock;
    pDataB->isNumBlue++;
    pvNorm = pDataB->pvNormal+iNum;
    pvNorm->fX = -fPABx;
    pvNorm->fY = -fPABy;
    pvNorm->fZ = -fPABz;
    pvNorm->fLengthSq = fDSq * 4.0;
    /* pDataB->isNumNorm++; */

    iNumShock++;
    iNewBlue++;

        } /* Goto next 2nd gene in BuckB to create a new pair: l++ */
      }
      /* else : BuckB contains No gene or is Paired with A already */
      /* break;  Get out of Loop(k) */
    } /* BuckB is visible from GeneA */
    /* else : BuckB is NOT visible from GeneA   *\
     *      We are not using this information *
    \*      later, but maybe we should/could. */

  } /* Goto next 1st gene in BuckA to create new pairs: k++ */

  /* ------------ All Viz Genes of BuckA visited ------------ *\
   *     If BuckB is Not Visible from ALL GeneA, we do NOT    *
   *     acquire new bucket neighbors linked to it.      *
   *     If it is visible, we try to acquire new neighbors.   *
   *     In both cases, we need to raise a flag for BuckB     *
  \*     to avoid pairing it latter with BuckA again.      */

  if(!iFlagVisibleBuckB) {
    iNumNotVisible++;
    /* If not paired before */
    if(*(pBuckA->pucFlagPair+iLabelBuckB) < 1) {
      /* B Not visible from A */
      *(pBuckA->pucFlagViz+iLabelBuckB) = 2;
      /* Pairing not needed */
      *(pBuckA->pucFlagPair+iLabelBuckB) = 2;

      if(iNumGeneInBuckB > 0) {
        *(pBuckB->pucFlagPair+i) = 2; /* No needs to Pair with A */
        /* WARNING: Following is not absolutely true */
        /* *(pBuckB->pucFlagViz+i) = 2; BuckA not visible from B */
      }
    }
    continue; /* Go check next bucket ngb. : j++ */
  }

  /* ----- Else: BuckB is visible from at least 1 GeneA ------ *\
  \*     Acquire New Ngbs & Transmit Visible Genes         */
  
  /* If not paired before */
  if(*(pBuckA->pucFlagPair+iLabelBuckB) < 1) {
    *(pBuckA->pucFlagViz+iLabelBuckB)  = 1; /* BuckB Viz from A */
    *(pBuckA->pucFlagPair+iLabelBuckB) = 2; /* Pairings done */
    if(iNumGeneInBuckB > 0) {
      *(pBuckB->pucFlagPair+i) = 2; /* No needs to Pair with A */
    }
  }

  /* ----- Check immediate BuckB ngbs to propagate further ----- *\
   * 3 cases: (i) New ngb never visited from A before         *
   *     (ii) Not an immediate ngb of A, Been there (from A) *
   *          and not paired yet --> Check for new viz gene   *
   *    (iii) Like (ii) but paired already --> Check for new *
  \*    viz gene and re-open for pairing at next layer  */

  piNgbNew = pBuckB->piNgbs;
  iNumNgbNew = pBuckB->isNumNgbs;
  iNew = 0;
  for(k = 0; k < iNumNgbNew; k++) {

    iNgbNew = *piNgbNew++;
    if(iNgbNew == i) continue; /* Don't go back to A : k++ */

    /* --- Case (i) : New ngb --- */

    if(*(pBuckA->pucFlagNgb+iNgbNew) == 0) {
      /* Not Been there from BuckA yet */
      /* -- OK : Found a New Ngb bucket to acquire -- */
      *(piNewNgbTmp+iNew) = iNgbNew;
      iNew++;
      continue; /* k++ */
    }

    /* Position in pNgbViz , Default == 0 (void: ref. to A) */
    iAdrNgb = *(pBuckA->piPtrNgb+iNgbNew);

    /* --- Case (ii) : Same itera - Maybe add new Gene Viz --- */

    if(iAdrNgb > 0 && /* Not one of the nearest ngb. */
       *(pBuckA->pucFlagNgb+iNgbNew) > 0 &&  /* Been there */
       *(pBuckA->pucFlagPair+iNgbNew) < 1) { /* No pair yet */
      /* Then check for New Viz Genes */

      pNgbVizNew = pBuckA->pNgbViz+iAdrNgb;
      iNumGeneViz = pNgbVizNew->isNumGeneViz;

      iNewGeneVizToAdd = 0;
      if(iNumGeneViz < iNumGeneInBuckA) {
        piTmpB = piGeneVizTmp;
        piTmp = piGeneASeeingBTmp;
        for(l = 0; l < iNumGeneASeeingB; l++) {
    iGeneViz = *piTmp++;
    iFlagAcquire = TRUE;
    piOldGeneViz = pNgbVizNew->piGenes;
    for(m = 0; m < iNumGeneViz; m++) {
      if(iGeneViz == *piOldGeneViz++) {
        iFlagAcquire = FALSE;
        break; } /* Get out of For(m) loop */
    } /* m++ */
    if(iFlagAcquire) {
      *piTmpB++ = iGeneViz;
      iNewGeneVizToAdd++;
    }
        } /* l++ */
      }
      if(iNewGeneVizToAdd > 0) { /* New Viz GeneA to add */
        iMax = iNumGeneViz + iNewGeneVizToAdd;
        if((pNgbVizNew->piGenes =
      (int *) realloc((int *) pNgbVizNew->piGenes,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pNgbVizNew->piGenes[%d])\n",
      pcFName, iMax);
    exit(-3); }
        pNgbVizNew->isNumGeneViz = iMax;
        if(iMax > iNumGeneTransmitMax)
    iNumGeneTransmitMax = iMax;
        piTmpB = piGeneVizTmp;
        piTmp = pNgbVizNew->piGenes+iNumGeneViz;
        for(l = 0; l < iNewGeneVizToAdd; l++) {
    *piTmp++ = *piTmpB++;
        }
        iNumGeneTransmitTot += iNewGeneVizToAdd;
        pNgbVizNew->isNumVisit++;
        iNumGeneVizAdded++;
      }
      continue; /* k++ */
    } /* ---- End of Case (ii) ---- */

    /* --- Case (iii) : Later itera - Maybe re-open for pairing --- */

    if(iAdrNgb > 0 && /* Not one of the nearest ngb. */
       *(pBuckA->pucFlagNgb+iNgbNew) > 0 &&  /* Been there */
       *(pBuckA->pucFlagPair+iNgbNew) > 1) { /* Closed */
      /* Then check for New Viz Genes */
#if FALSE
      if((i == 905 || i == 902) &&
         (iNgbNew == 902 || iNgbNew == 905 || iNgbNew == 1011)) {
        fprintf(stderr,
          "MESG(%s): Reached Buck %d (%d) for re-opening.\n",
          pcFName, i, iNgbNew);
      }
#endif
      /* Already been there from A : Check it out */
      pNgbVizOld = pBuckA->pNgbViz+iAdrNgb;
      iNumGeneViz = pNgbVizOld->isNumGeneViz;

      iNewGeneVizToAdd = 0;
      if(iNumGeneViz < iNumGeneInBuckA) {
        piTmpB = piGeneVizTmp;
        piTmp = piGeneASeeingBTmp;
        for(l = 0; l < iNumGeneASeeingB; l++) {
    iGeneViz = *piTmp++;
    iFlagAcquire = TRUE;
    piOldGeneViz = pNgbVizOld->piGenes;
    for(m = 0; m < iNumGeneViz; m++) {
      if(iGeneViz == *piOldGeneViz++) {
        iFlagAcquire = FALSE;
        break; } /* Get out of For(m) loop */
    } /* m++ */
    if(iFlagAcquire) {
      *piTmpB++ = iGeneViz;
      iNewGeneVizToAdd++;
    }
        } /* l++ */
      }
      if(iNewGeneVizToAdd > 0) { /* New Viz GeneA to process */
        /* Add these to the list of Ngb representative on A */
        iMax = iNumGeneViz + iNewGeneVizToAdd;
        if((pNgbVizOld->piGenes =
      (int *) realloc((int *) pNgbVizOld->piGenes,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pNgbVizOld->piGenes[%d])\n",
      pcFName, iMax);
    exit(-3); }
        pNgbVizOld->isNumGeneViz = iMax;
        if(iMax > iNumGeneTransmitMax)
    iNumGeneTransmitMax = iMax;
        piTmpB = piGeneVizTmp;
        piTmp = pNgbVizOld->piGenes+iNumGeneViz;
        for(l = 0; l < iNewGeneVizToAdd; l++) {
    *piTmp++ = *piTmpB++;
        }
        iNumGeneTransmitTot += iNewGeneVizToAdd;
        pNgbVizOld->isNumVisit++;
        iNumGeneVizAdded++;

        /* -- Make an Additional copy of Ngb carrying Viz genes -- *\
        \*    missed previously, to be processed at next itera     */

        iMax = pBuckA->isMaxNgbs + 1;
        if((pBuckA->pNgbViz =
      (NgbBuck *) realloc((NgbBuck *) pBuckA->pNgbViz,
              iMax * sizeof(NgbBuck))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pBuckA->pNgbViz[%d])\n",
      pcFName, iMax);
    exit(-3); }
        pBuckA->isMaxNgbs = iMax;

        iMax--;
        pNgbVizNew = pBuckA->pNgbViz+iMax;

        /* For this add. Ngb, transmit list of new viz GeneA */
      
        if((pNgbVizNew->piGenes = (int *)
      calloc(iNewGeneVizToAdd, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC pNgbVizNew->piGenes[%d]\n",
      pcFName, iNewGeneVizToAdd);
    exit(-3); }

        piTmpB = piGeneVizTmp;
        piTmp = pNgbVizNew->piGenes;
        for(l = 0; l < iNewGeneVizToAdd; l++) {
    *piTmp++ = *piTmpB++;
        }
        pNgbVizNew->isNumGeneViz = iNewGeneVizToAdd;

        pNgbVizNew->isBuck = iNgbNew;
        pNgbVizNew->isBuckFrom = iLabelBuckB;
        pNgbVizNew->isItera = iIteraNext;
        pNgbVizNew->isNumVisit = 1;
        
        iAcquire++;
        iNumAddNgb++;
        /* Re-open for pairings (at next itera) */
        /* *(pBuckA->pucFlagPair+iNgbNew) = 1; */

        pBuckC = pBucksXYZ->pBucket+iNgbNew;
        iNumGeneInBuckC = pBuckC->isNumGenes;
        if(iNumGeneInBuckC > 0)
    *(pBuckC->pucFlagPair+i) = 1;

      }
    } /* --- End of Case (iii) --- */

  } /* Check next surrounding ngb. : k++ */
  
  /* ------- If we Found New neighbors : ------- *\
  \*     Acquire these for next layer         */

  if(iNew > 0) {

    iAdrNgb = pBuckA->isMaxNgbs;
    iMax = iAdrNgb + iNew;
    if((pBuckA->pNgbViz =
        (NgbBuck *) realloc((NgbBuck *) pBuckA->pNgbViz,
          iMax * sizeof(NgbBuck))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc(pBuckA->pNgbViz[%d])\n",
        pcFName, iMax);
      fprintf(stderr,
        "\tA = %d, B = %d, iItera = %d, NewNgbs = %d\n",
        i, iLabelBuckB, iItera, iNew);
      exit(-3); }
    pBuckA->isMaxNgbs = iMax;
    
    pNgbVizNew = pBuckA->pNgbViz+iAdrNgb;
    for(k = 0; k < iNew; k++) {
      iNgbNew = *(piNewNgbTmp+k);    
      *(pBuckA->pucFlagNgb+iNgbNew) = 3;
      *(pBuckA->piPtrNgb+iNgbNew) = iAdrNgb;

      /* For each such new Ngb, transmit list of visible GeneA */
      
      if((pNgbVizNew->piGenes = (int *)
    calloc(iNumGeneASeeingB, sizeof(int))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): CALLOC pNgbVizNew->piGenes[%d]\n",
          pcFName, iNumGeneASeeingB);
        exit(-2); }

      piTmp = piGeneASeeingBTmp;
      for(l = 0; l < iNumGeneASeeingB; l++) {
        /* Keep track of genes */
        *(pNgbVizNew->piGenes+l) = *piTmp++;
      }
      pNgbVizNew->isNumGeneViz = iNumGeneASeeingB;

      if(iNumGeneASeeingB > iNumGeneTransmitMax)
        iNumGeneTransmitMax = iNumGeneASeeingB;
      iNumGeneTransmitTot += iNumGeneASeeingB;

      pNgbVizNew->isBuck = iNgbNew;
      pNgbVizNew->isBuckFrom = iLabelBuckB;
      pNgbVizNew->isItera = iIteraNext;
      pNgbVizNew->isNumVisit = 1;
      
      pNgbVizNew++;
      iAcquire++; iAdrNgb++;
    } /* Acquire next New Ngb : k++ */

  } /* Acquired New Ngbs. From BuckB */

      } /* Goto next Neighboring BuckB to visit : j++ */

      /* -- All ngb. BuckB at this Iteration are visited for BuckA -- */

      /* Ngb. newly acquired */
      pBuckA->piNgbPerItera[iIteraNext] = iAcquire;
      /* 1st Ngb. newly acquired */
      pBuckA->piFstNgb[iIteraNext] = iNgbLast;

      iAcquireTotal += iAcquire;
    } /* Goto next BuckA for this Initial iteration : i++ */

    fprintf(stderr, "\tIteration %d:\n", iItera);
#if SGI_ENV
    if(getrusage(RUSAGE_SELF, &ru_step) == -1) {
      fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
    }
    fTimeSys  = -fTmpSys;
    fTmpSys   = (float) SECS(ru_step.ru_stime);
    fTimeSys += fTmpSys;
    fTimeUser = -fTmpUser;
    fTmpUser  = (float) SECS(ru_step.ru_utime);
    fTimeUser += fTmpUser;
    fMemUse   = (float) (ru_step.ru_maxrss / 1000.0);
    fprintf(stderr,
      "\t\tRUSAGE: Sys = %.2f , User = %.2f , Mem = %.2f Meg.\n",
      fTimeSys, fTimeUser, fMemUse);
#endif
    fprintf(stderr, "\t\tNumber of Buckets Not Visible = %d\n",
      iNumNotVisible);
    fprintf(stderr, "\t\tNumber of New Valid Shocks = %d\n",
      iNewBlue);
    if(iNewBlue < 1) {
      iNumIteraNoNewShocks++;
      fTimeWaisted += fTimeSys + fTimeUser;
    }
    iNewBlue = iNumNotVisible = 0;
    fprintf(stderr, "\t\tTotal number of newly acquired neigbors = %d\n",
      iAcquireTotal);
    fprintf(stderr, "\t\tTotal number of times viz genes added = %d\n",
      iNumGeneVizAdded);
    fprintf(stderr, "\t\tTotal number of existing Ngb added = %d\n",
      iNumAddNgb);
    fprintf(stderr,
      "\t\tTotal of Transmitted genes = %d, Max per Ngb. = %d\n",
      iNumGeneTransmitTot, iNumGeneTransmitMax);

    fprintf(stderr, "\t\tNumber of duplicates of valid shocks = %d\n",
      iNumDuplic);

    iNumDuplicTot += iNumDuplic;
    iNumDuplic = 0;

    iItera++;
    iIteraNext++;
      
  } /* Next iteration in While Loop */
  
  /* -- Iterations over -- */

  if(iItera == iIteraMax) {
    fprintf(stderr, "\tIteraMax (%d) reached , ", iItera);
    fprintf(stderr, "iAcquire = %d\n", iAcquireTotal);
  }
  if(iNumShock > iMaxShock) {
    fprintf(stderr,
      "ERROR(%s):\n\tMax Num of shock exceeded: %d > %d.\n",
      pcFName, iNumShock, iMaxShock);
    exit(-3);
  }
#if SGI_ENV
  if(getrusage(RUSAGE_SELF, &ru_end) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage (end) failed.\n", pcFName);
  }
  fTimeSys  = (float) (SECS(ru_end.ru_stime) - SECS(ru_start.ru_stime));
  fTimeUser = (float) (SECS(ru_end.ru_utime) - SECS(ru_start.ru_utime));
  fMemUse   = (float) (ru_end.ru_maxrss / 1000.0);
#endif
  iNumShockExternal = iNumShock - iNumShockInit;
  pDimMBuck->iNumShockSheetExtAll = iNumShockExternal;
  pDimMBuck->iNumShockSheet = iNumShock;
  /* pDimMBuck->iNumShockSheetV = iNumShock; */
  pDimMBuck->iMaxBlueShocks = iMaxShock;

  fprintf(stderr, "\t********* `External' pairs found ********\n");
#if SGI_ENV
  fprintf(stderr,
    "\t\tRUSAGE: Sys = %.2f , User = %.2f , Mem = %.2f Meg.\n",
    fTimeSys, fTimeUser, fMemUse);
#endif
  fprintf(stderr,
    "\tTotal of %d VALID Blue shocks (Max alloc. on Scaff. = %d).\n",
    iNumShock, iMaxShock);
  fprintf(stderr, "\t\t%d are Internal , %d are External.\n",
    iNumShockInit, iNumShockExternal);
  fprintf(stderr, "\t\tLargest distance between genes found = %f\n",
    (float) sqrt((double) fDistSqLargest));
  /* fprintf(stderr, "\tOffshoots = %d.\n", iOffShoots); */
  fprintf(stderr, "\t\tInvalid shocks = %d\n", iNumInvalid);
  fprintf(stderr, "\t\tDuplicates = %d , Checks = %d\n",
    iNumDuplicTot, iNumDuplicCheck);
  fprintf(stderr,
    "\t\t%d Duplicates from Sub.Res., %d of which are now InitBlue.\n",
    iSameBlue, iNumReset);
  fprintf(stderr, "\t\tMaximal iteration reached = %d\n", iItera);
  fprintf(stderr, "\t\tIterations without New Shocks = %d\n",
    iNumIteraNoNewShocks);
  fprintf(stderr, "\t\tTime spent without finding new shocks = %.2f\n",
    fTimeWaisted);

  return;
}



/* ---- EoF ---- */
