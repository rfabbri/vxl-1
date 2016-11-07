/*------------------------------------------------------------*\
 * File: fetch_red_shocks.c
 *
 * Functions:
 *  FetchRedShocks
 *  SetValidVertices
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie (adapted from old version)
 *  #1: June 2002: Fill-in Blue slots of Green Curves.
 *  #2: July 2003: Updated for use with MetaBuckets.
 *
\*------------------------------------------------------------*/

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include "fetch_red_shocks.h"
#include "circum.h"
#include "valid_shocks.h"
#include "barycentric.h"

/*------------------------------------------------------------*\
 * Function: FetchRedShocks
 *
 * Usage: Seek all Initial Vertex shocks - some are part of
 *    the MA, others are only part of the SS. Keep only
 *    valid shocks (on MA). We intersect curves found at
 *    Full Resolution in each bucket. Used WITHIN buckets
 *    and for 1ST LAYER of surrounding ones.
 *
\*------------------------------------------------------------*/

void
FetchRedShocks(InputData *pInData, ShockScaffold *pScaffold,
         ShockVertex *pShockVertices, short *pisLookUp,
         Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
         Voxel *pVoxels, Dimension *pDimMBuck,
         Dimension *pDimAll)
{
  char    *pcFName = "FetchRedShocks";
  register int  i, j, k, l, m;
  int    iGeneA,iGeneB,iGeneC,iGeneD, iCircum;
  int    iOffShootsToInfinity, iNumInvalid, iNumBucks, iNumGreenA;
  int    iGreen1,iGreen2, iNext, iNewGeneA,iNewGeneB,iNewGeneC, iSet;
  int    iSetA,iSetB,iSetC,iSetD, iPosXs,iPosYs,iPosZs, iSliceSize;
  int    iXdim, iLabelBuckShock, iNumGeneInBuckA, iPos;
  int    iXmin,iYmin,iZmin, iXmax,iYmax,iZmax, iOutOfBox;
  int    iRedValid, iNumProxiA, iOldRed, iFlagPermute,iFlagPermuteNot;
  int    iOldA,iOldB,iOldC,iOldD, iSameRed, iTmp;
  int    *piGreen1,*piGreen2, *piGeneInBuckA, *piOldRed;
  /* static float  fEpsilon = (float) D_EPSILON; */
  float    fDX,fDY,fDZ, fDSq, fPosXs,fPosYs,fPosZs;
  float    fDistSqLargest, fDistSqLarge;
  double  dGeneA[3],dGeneB[3],dGeneC[3],dGeneD[3], dCircum[3];
  ShockCurve  *pGreen1, *pGreen2;
  ShockVertex  *pRed, *pOldRed;
  Bucket  *pBuckA;
  Pt3dCoord  *pGeneA, *pGeneB, *pGeneC, *pGeneD;
  InputData  *pDataA, *pDataB, *pDataC, *pDataD;
  ShockData  *pShockData;

  /* Preliminaries */

  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }

  fprintf(stderr, "MESG(%s):\n", pcFName);

  iXdim = pDimMBuck->iXdim;
  iXmin = pDimMBuck->Limits.iMinX;
  iYmin = pDimMBuck->Limits.iMinY;
  iZmin = pDimMBuck->Limits.iMinZ;
  iXmax = pDimMBuck->Limits.iMaxX;
  iYmax = pDimMBuck->Limits.iMaxY;
  iZmax = pDimMBuck->Limits.iMaxZ;
  iSliceSize = pDimMBuck->iSliceSize;
  iNumBucks = pBucksXYZ->iNumBucks;
  iOffShootsToInfinity = 0;
  iNumInvalid = iSameRed = 0;
  fDistSqLarge = fDistSqLargest = 0.0;

  iCircum = iOutOfBox = 0;
  pRed = pShockVertices;
  iFlagPermute = pDimMBuck->isFlagPermute;
  iFlagPermuteNot = iFlagPermute - 1;

  /* ------------ Process BucketsXYZ one by one ----------- *\
  \*      Seek quadruplets of generators      */

  pBuckA = pBucksXYZ->pBucket-1;
  for(i = 0; i < iNumBucks; i++) {

    pBuckA++;
    iNumGeneInBuckA = pBuckA->isNumGenes;
    if(iNumGeneInBuckA < 1) continue;

    /* For each GeneA : Visit its Active Green Shocks to build Quadruplets */
    piGeneInBuckA = pBuckA->piGenes;
    for(j = 0; j < iNumGeneInBuckA; j++) {
      iGeneA  = *piGeneInBuckA++;
      pDataA = pInData+iGeneA;
      pGeneA  = &(pDataA->Sample);
      dGeneA[0] = (double) pGeneA->fPosX;
      dGeneA[1] = (double) pGeneA->fPosY;
      dGeneA[2] = (double) pGeneA->fPosZ;
      /* To avoid permutations of quadruples */
      pDataA->isFlagPermute = iFlagPermute;

      /* Go through the list of associated Green shocks in *\
       * ascending order only, to avoid redundancies.      *
       * Intersect only Active sheets (i.e., found at Full *
      \* Resolution) with other active and passive ones.   */

      iNumGreenA = pDataA->isNumGreen;
      if(iNumGreenA < 2)
  continue; /* Need at least 2 Green shocks : Next GeneA: j++ */

      piGreen1 = pDataA->piGreenList;
      for(k = 0; k < iNumGreenA; k++) {

  iGreen1 = *piGreen1++;
  pGreen1 = (pScaffold->pGreen)+iGreen1;
  if(pGreen1->isStatus != ACTIVE) 
    continue; /* Need to intersect an ACTIVE curve with others */

  if(iGeneA == pGreen1->iGene[0]) { /* Must share GeneA */
    iGeneB = pGreen1->iGene[1];
    iGeneC = pGreen1->iGene[2];
  }
  else if(iGeneA == pGreen1->iGene[1]) {
    iGeneB = pGreen1->iGene[0];
    iGeneC = pGreen1->iGene[2];
  }
  else {
    iGeneB = pGreen1->iGene[0];
    iGeneC = pGreen1->iGene[1];
  }

  pDataB = pInData+iGeneB;
  if(pDataB->isFlagPermute > iFlagPermuteNot)
    continue; /* Already used */
  pDataC = pInData+iGeneC;
  if(pDataC->isFlagPermute > iFlagPermuteNot)
    continue; /* Already used */

  pGeneB = &(pDataB->Sample);
  dGeneB[0] = (double) pGeneB->fPosX;
  dGeneB[1] = (double) pGeneB->fPosY;
  dGeneB[2] = (double) pGeneB->fPosZ;
  
  pGeneC = &(pDataC->Sample);
  dGeneC[0] = (double) pGeneC->fPosX;
  dGeneC[1] = (double) pGeneC->fPosY;
  dGeneC[2] = (double) pGeneC->fPosZ;

  piGreen2 = piGreen1;
  iNext = k + 1;
  for(l = iNext; l < iNumGreenA; l++) {

    iGreen2 = *piGreen2++;
    if(iGreen2 == iGreen1) {
      fprintf(stderr, "ERROR(%s): iGreen1 (%d) == iGreen2\n",
        pcFName, iGreen1);
      exit(-3);
    }
    pGreen2 = (pScaffold->pGreen)+iGreen2;
    /* We may intersect an active shock with a passive one */
    /* if(pGreen2->iStatus != ACTIVE) continue; */

    iNewGeneA = pGreen2->iGene[0];
    iNewGeneB = pGreen2->iGene[1];
    iNewGeneC = pGreen2->iGene[2];

    iSet = iSetA = iSetB = iSetC = FALSE;
    if(iNewGeneA == iGeneA || iNewGeneA == iGeneB || iNewGeneA == iGeneC)
      iSetA = TRUE;
    if(iNewGeneB == iGeneA || iNewGeneB == iGeneB || iNewGeneB == iGeneC)
      iSetB = TRUE;
    if(iNewGeneC == iGeneA || iNewGeneC == iGeneB || iNewGeneC == iGeneC)
      iSetC = TRUE;
    iSet = iSetA + iSetB + iSetC;
    if(iSet != 2)
      continue; /* Must share 2 genes at intersection, i.e., a sheet */
    if(!iSetA) iGeneD = iNewGeneA;
    else if(!iSetB) iGeneD = iNewGeneB;
    else iGeneD = iNewGeneC;

    pDataD = pInData+iGeneD;
    if(pDataD->isFlagPermute > iFlagPermuteNot)
      continue; /* Already used */

    pGeneD  = &(pDataD->Sample);
    dGeneD[0] = (double) pGeneD->fPosX;
    dGeneD[1] = (double) pGeneD->fPosY;
    dGeneD[2] = (double) pGeneD->fPosZ;

    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

    /* CircumCenter of tetrahedron is calculated.   *\
    \* Returned coordinates are relative to VertexA */

    if(!tet_circum(dGeneA,dGeneB,dGeneC,dGeneD,dCircum)) {
      /* Offshoot to infinity */
      iOffShootsToInfinity++;
      continue; /* l++ */
    }

    /* Potential 3D Tetrahedron CircumCenter */

    fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
    fDZ = (float) dCircum[2];
    fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
    if(fDSq > fDistSqLargest) fDistSqLargest = fDSq;

    fPosXs = fDX + (float) dGeneA[0];
    fPosYs = fDY + (float) dGeneA[1];
    fPosZs = fDZ + (float) dGeneA[2];

    /* -- Validate Shocks located INSIDE the box bounding data -- */

    iPosXs = (int) floor((double) fPosXs);
    iPosYs = (int) floor((double) fPosYs);
    iPosZs = (int) floor((double) fPosZs);
    /* -- Is Shock located INSIDE the MetaBucket ? -- */
    if(iPosXs < iXmin || iPosXs >= iXmax ||
       iPosYs < iYmin || iPosYs >= iYmax ||
       iPosZs < iZmin || iPosZs >= iZmax) {
      /* Shock is OUTSIDE the box bounding data */
      /* pRed->iStatus = PASSIVE; */
      iOutOfBox++;
      continue; /* We do not keep these */
      /* Goto next Green 2nd shock viz GeneA : l++ */
    }

    iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
    iLabelBuckShock = *(pisLookUp+iPos);
    pRed->iBuckXYZ = iLabelBuckShock;

    pShockData->iGene[0] = pRed->iGene[0] = iGeneA;
    pShockData->iGene[1] = pRed->iGene[1] = iGeneB;
    pShockData->iGene[2] = pRed->iGene[2] = iGeneC;
    pShockData->iGene[3] = pRed->iGene[3] = iGeneD;
    pShockData->fDist2GeneSq = fDSq;
    pShockData->ShockCoord.fPosX = fPosXs;
    pShockData->ShockCoord.fPosY = fPosYs;
    pShockData->ShockCoord.fPosZ = fPosZs;

#if HIGH_MEM
    pRed->CircumSphere.fPosX = fPosXs;
    pRed->CircumSphere.fPosY = fPosYs;
    pRed->CircumSphere.fPosZ = fPosZs;
    pRed->fDist2GeneSq = fDSq;
#endif

#if FALSE
    if(!GetQuadCoordOfVertex(pInData, pRed)) {
      fprintf(stderr,
        "\tWARNING(%s): Quad coords failed on Red shock %d\n",
        pcFName, iCircum);
      continue;
    }
#endif

    /* -- Check for duplicate -- */

    iRedValid = TRUE;

    iNumProxiA = pDataA->isNumRed;
    /* We arbitrarily pick GeneA as the anchor point to do *\
     * comparisons via backpointers. We could optimize by  * 
    \* finding the gene having the lesser red shocks       */

    piOldRed = pDataA->piRedList;
    for(m = 0; m < iNumProxiA; m++) {
      iOldRed = *piOldRed++;
      pOldRed = (pScaffold->pRed)+iOldRed;
      iOldA = pOldRed->iGene[0];
      iOldB = pOldRed->iGene[1];
      iOldC = pOldRed->iGene[2];
      iOldD = pOldRed->iGene[3];

      iSetB = iSetC = iSetD = FALSE;
      if(iGeneB == iOldA || iGeneB == iOldB || iGeneB == iOldC ||
         iGeneB == iOldD)
        iSetB = TRUE;
      if(iGeneC == iOldA || iGeneC == iOldB || iGeneC == iOldC ||
         iGeneC == iOldD)
        iSetC = TRUE;
      if(iGeneD == iOldA || iGeneD == iOldB || iGeneD == iOldC ||
         iGeneD == iOldD)
        iSetD = TRUE;
      
      iSet = iSetB + iSetC + iSetD;
      if(iSet > 2) { /* Found the same shock */
        iRedValid = FALSE;
        iSameRed++;
        break;
      }
    } /* m++ */

    if(!iRedValid) { /* Duplicate : Track new Green shock(s) */
#if FALSE
      if(pOldRed->iGreen[2] > -1 && pOldRed->iGreen[3] > -1) {
        fprintf(stderr,
          "WARNING(%s): Red shock %d type I duplicate,\n",
          pcFName, iOldRed);
        fprintf(stderr,
          "\tbut 3rd and 4th Green shocks already set.\n");
        fprintf(stderr, "\t*Old* Green shocks: (%d,%d,%d,%d).\n",
          pOldRed->iGreen[0], pOldRed->iGreen[1],
          pOldRed->iGreen[2], pOldRed->iGreen[3]);
        fprintf(stderr, "\t*New* Green shocks: %d and %d\n",
          iGreen1, iGreen2);
      }
#endif
      if(pOldRed->iGreen[2] < 0 && pOldRed->iGreen[3] < 0) {
        iTmp = 2;
        if((pOldRed->iGreen[0] != iGreen1) &&
     (pOldRed->iGreen[1] != iGreen1))
    pOldRed->iGreen[iTmp++] = iGreen1;
        if((pOldRed->iGreen[0] != iGreen2) &&
     (pOldRed->iGreen[1] != iGreen2))
    pOldRed->iGreen[iTmp] = iGreen2;
      }
      else if(pOldRed->iGreen[2] > -1 && pOldRed->iGreen[3] < 0) {
        if((pOldRed->iGreen[0] != iGreen1) &&
     (pOldRed->iGreen[1] != iGreen1) &&
     (pOldRed->iGreen[2] != iGreen1))
    pOldRed->iGreen[3] = iGreen1;
        else if((pOldRed->iGreen[0] != iGreen2) &&
          (pOldRed->iGreen[1] != iGreen2) &&
          (pOldRed->iGreen[2] != iGreen2))
    pOldRed->iGreen[3] = iGreen2;
      }
      else if(pOldRed->iGreen[2] < 0 && pOldRed->iGreen[3] > -1) {
        if((pOldRed->iGreen[0] != iGreen1) &&
     (pOldRed->iGreen[1] != iGreen1) &&
     (pOldRed->iGreen[3] != iGreen1))
    pOldRed->iGreen[2] = iGreen1;
        else if((pOldRed->iGreen[0] != iGreen2) &&
          (pOldRed->iGreen[1] != iGreen2) &&
          (pOldRed->iGreen[3] != iGreen2))
    pOldRed->iGreen[2] = iGreen2;
      }
#if FALSE
      else {
        fprintf(stderr,
          "WARNING(%s): Red shock %d type I duplicate,\n",
          pcFName, iOldRed);
        fprintf(stderr, "\tOld Green shocks: (%d , %d , %d , %d)\n",
          pOldRed->iGreen[0], pOldRed->iGreen[1],
          pOldRed->iGreen[2], pOldRed->iGreen[3]);
        fprintf(stderr, "\tNew Green shocks: %d and %d\n",
          iGreen1, iGreen2);
      }
#endif
      continue; /* Goto next Green2 : l++ */
    }
    
    /* -------------------- VALIDATE --------------------- */

    if(!ValidRedShockViaVoxels(pShockData, pInData, pGeneCoord,
             pVoxels, pDimAll)) {
      iNumInvalid++;
      continue; /* Part of SS only: Goto next Blue ngb : j++ */
    }

    /* -------- Found a New Valid Vertex Shock --------- */

    if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;

    pRed->isFlagValid = TRUE;

    pRed->iGreen[0] = iGreen1;
    pRed->iGreen[1] = iGreen2;
    pRed->iGreen[2] = IDLE;
    pRed->iGreen[3] = IDLE;
#if FALSE
    switch(pRed->isType) {
    case RED_I:
      iRedI++;
      break;
    case RED_II: /* CAP: One Green curve exits */
      iRedII++;
      iNumG++;
      break;
    case RED_III: /* A pair of green curves bounding a sheet exit */
      iRedIII++;
      iNumB++;
      break;
    default:
      break;
    }
#endif
    pRed++;
    iCircum++;

  } /* Goto next Green 2nd shock viz GeneA : l++ */

      } /* Goto next Green 1st shock viz GeneA : k++ */

    } /* Goto next GeneA in Bucket : j++ */

  } /* Goto next Bucket : i++ */

  fprintf(stderr, "\tFound %d new Red shocks.\n", iCircum);
  fprintf(stderr,
    "\tMax distance to Generators amongst Valid shocks = %.2f \n",  
    (float) sqrt((double) fDistSqLarge));
  fprintf(stderr,
    "\tLargest ball radius computed = %.2f\n",
    (float) sqrt((double) fDistSqLargest));

  pDimMBuck->iNumShockVertex = iCircum;
  /* pDimMBuck->iNumShockVertexV = iCircum; */
  pDimMBuck->iNumShockRed2Green = iCircum;
  pDimMBuck->iNumShockRed2Blue = iCircum;
#if FALSE
  pDimMBuck->iNumShockRed2Green = iNumG + 2 * iNumB;
  pDimMBuck->iNumShockRed2Blue  = iNumB;
  fprintf(stderr, "\tFrom Red Vertices, potential new shocks:\n");
  fprintf(stderr, "\t\t%d Blue Sheets and %d Green Curves.\n",
    pDimMBuck->iNumShockRed2Blue, pDimMBuck->iNumShockRed2Green);
#endif
  fprintf(stderr, "\tOffShoots: to infinity = %d ,\n", iOffShootsToInfinity);
  fprintf(stderr, "\t\tOut of Box (but finite dist) = %d .\n",
    iOutOfBox);

  fprintf(stderr, "\tInvalid shocks = %d .\n", iNumInvalid);
  fprintf(stderr, "\tDuplicates of red shocks = %d .\n", iSameRed);

  return;
}

/*------------------------------------------------------------*\
 * Function: SetValidVertices
 *
 * Usage: Sets a list of validated initial node shocks on the
 *    scaffold. Adds new sheets and curves too (from Red
 *    nodes of type II & III).
 *    Also seeks all links (shock curves) and create new
 *    curves if necessary.
 *    Fills-in all Blue slots of Green curves.
 *    Fills-in all Green slots of Red vertices.
 *    Used WITHIN buckets and for 1ST LAYER of surrounding
 *    ones.
 *
\*------------------------------------------------------------*/

int
SetValidVertices(ShockVertex *pShocks, ShockScaffold *pScaffold,
     InputData *pInData, short *pisLookUp,
     Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
     Voxel *pVoxels, Dimension *pDimMBuck,
     Dimension *pDimAll)
{
   char    *pcFName = "SetValidVertices";
   register int  i, j;
   int    iNumRed, iRed,iBlue,iGreen, iGeneA,iGeneB,iGeneC,iGeneD;
   int    iMax, iGreenA,iGreenB,iGreenC,iGreenD, iSet, iTmp;
   int    iRedA,iRedB, iNewGeneA,iNewGeneB,iNewGeneC, iCircum;
   int    iBlueValid,iGreenValid, iOldA,iOldB,iOldC, iOldBlue;
   int    iSetA,iSetB,iSetC, iSameGreen,iSameBlue, iCase;
   int    iNumProxiA,iNumProxiB,iNumProxiC,iNumProxiD, iType;
   int    iOldGreen, iTmpA,iTmpB,iTmpC,iTmpD, iCurve, iSheet;
   int    iNumRedI,iNumRedII,iNumRedIII;
   int    iChunk, iNext, iPercent, iDelta, iPos, iXdim, iSliceSize;
   int    iNumInvalid, iNumGreenPassive, iNumBlueMiss;
   int    iNewGreen, iLabelBuckShock, iPosX,iPosY,iPosZ;
   int    iNumProxi, iNumProxiBlue, iNewGeneBlue, iFindSndGene;
   int    iNumProxi1,iNumProxi2, iMax1,iMax2;
   int    *piOldBlue, *piOldGreen;
   /*   static float  fSmall = (float) E_EPSILON; */
   float    fDSq, fPABx,fPABy,fPABz, fPosX,fPosY,fPosZ, fDistNew;
   float    fAlpha,fBeta,fGamma, fPosXBlue,fPosYBlue,fPosZBlue;
   float    fVolMin,fVolMax, fTmp;
   ShockSheet  *pNewBlue, *pOldBlue;
   ShockCurve  *pGreenA,*pGreenB,*pGreenC,*pGreenD, *pNewGreen, *pOldGreen;
   ShockVertex  *pRed, *pNewRed;
   InputData  *pDataA, *pDataB, *pDataC, *pDataD, *pData1,*pData2;
   Pt3dCoord  *pGeneA, *pGeneB;
   Vector3d  *pvNorm;
   ShockData  *pShockData;
   
   /* Preliminaries */
   
   pShockData = NULL;
   if((pShockData = (ShockData*)
       calloc(1, sizeof(ShockData))) == NULL) {
      vul_printf (vcl_cout, "ERROR(%s): CALLOC fails on pShockData[1].\n",
              pcFName);
      exit(-4); 
   }
   
   fprintf(stderr, "MESG(%s):\n", pcFName);
   
   iXdim = pDimMBuck->iXdim;
   iSliceSize = pDimMBuck->iSliceSize;
   
   iNumRed = pDimMBuck->iNumShockVertex;
   iRed    = pScaffold->iNumRedLast;
   iGreen  = pScaffold->iGreenActiveLast; /* Position of last shock */
   iBlue   = pScaffold->iBlueActiveLast;

   iSameBlue = iSameGreen = iCircum = iNumBlueMiss = 0;
   iNumRedI = iNumRedII = iNumRedIII = 0;
   iNumInvalid = iNumGreenPassive = 0;
   fVolMin = 100000.0;
   fVolMax = 0.0;
   
   pRed = pShocks-1;
   pNewRed = (pScaffold->pRed)+iRed;
   pNewGreen = (pScaffold->pGreen)+iGreen;
   pNewBlue = (pScaffold->pBlue)+iBlue;
   
   iDelta = (int) (iNumRed / 4);
   iNext = iDelta;
   iChunk = 0;
   
   /* ---- Process each potential Red shock ---- */
   
   for(i = 0; i < iNumRed; i++) {
      if(i > iNext) { /* Print a mesg. for each chunk of 25% of data */
         iChunk++;
         iPercent = iChunk * 25;
         if(iPercent < 101) {
           fprintf(stderr, "\t\tProcessed %d%% of potential Red shocks ...\n", iPercent);
           iNext += iDelta; 
         }
      }

      pRed++;

      iGeneA = pNewRed->iGene[0] = pRed->iGene[0];
      iGeneB = pNewRed->iGene[1] = pRed->iGene[1];
      iGeneC = pNewRed->iGene[2] = pRed->iGene[2];
      iGeneD = pNewRed->iGene[3] = pRed->iGene[3];

      pNewRed->iBuckXYZ = pRed->iBuckXYZ;

#if HIGH_MEM
      fPosX = pNewRed->CircumSphere.fPosX = pRed->CircumSphere.fPosX;
      fPosY = pNewRed->CircumSphere.fPosY = pRed->CircumSphere.fPosY;
      fPosZ = pNewRed->CircumSphere.fPosZ = pRed->CircumSphere.fPosZ;
      fDistNew = pNewRed->fDist2GeneSq = pRed->fDist2GeneSq;
      fAlpha = pNewRed->fQuadCoord[0] = pRed->fQuadCoord[0];
      fBeta  = pNewRed->fQuadCoord[1] = pRed->fQuadCoord[1];
      fGamma = pNewRed->fQuadCoord[2] = pRed->fQuadCoord[2];
      /* fDelta = pNewRed->fQuadCoord[3] = pRed->fQuadCoord[3]; */
      fTmp = pNewRed->fTetVolume = pRed->fTetVolume;
      if(fTmp < fVolMin) fVolMin = fTmp;
      if(fTmp > fVolMax) fVolMax = fTmp;
#endif
      iGreenA = pNewRed->iGreen[0] = pRed->iGreen[0];
      iGreenB = pNewRed->iGreen[1] = pRed->iGreen[1];
      iGreenC = pNewRed->iGreen[2] = pRed->iGreen[2];
      iGreenD = pNewRed->iGreen[3] = pRed->iGreen[3];
      
      if (!GetQuadCoordOfVertex(pInData, pRed, pShockData)) {
         vul_printf (vcl_cout, "\tWARNING(%s): Quad coords failed on Red shock %d\n",
                  pcFName, i);
         continue;
      }
      
      iType  = pNewRed->isType = pRed->isType;
      fAlpha = pShockData->fBaryCoord[0];
      fBeta  = pShockData->fBaryCoord[1];
      fGamma = pShockData->fBaryCoord[2];
      /* fDelta = pShockData->fBaryCoord[3]; */
      
      pNewRed->isFlagValid = TRUE;
      pNewRed->isItera = pScaffold->isLastItera;
      pNewRed->isStatus = ACTIVE;
      
      /* -- Set backpointers to shocks for the 4 Generators -- */
      pDataA = pInData+iGeneA;
      iNumProxiA = pDataA->isNumRed;
      iMax = pDataA->isMaxRed - 3;
      if(iNumProxiA > iMax) {
         iMax = 1 + iNumProxiA * 2;
         if ((pDataA->piRedList = (int *) realloc((int *) pDataA->piRedList,
               iMax * sizeof(int))) == NULL) {
           vul_printf (vcl_cout,
                   "ERROR(%s): ReAlloc on pDataA(%d)->piRedList[%d].\n",
                   pcFName, iGeneA, iMax);
           return(FALSE); 
         }
         pDataA->isMaxRed = iMax; 
      }
      pDataA->piRedList[iNumProxiA] = iRed; /* Backpointer to shock */
      pDataA->isNumRed++;
      pDataB = pInData+iGeneB;
      iNumProxiB = pDataB->isNumRed;
      iMax = pDataB->isMaxRed - 3;
      if(iNumProxiB > iMax) {
         iMax = 1 + iNumProxiB * 2;
         if((pDataB->piRedList = (int *) realloc((int *) pDataB->piRedList,
               iMax * sizeof(int))) == NULL) {
           vul_printf (vcl_cout,
                   "ERROR(%s): ReAlloc on pDataB(%d)->piRedList[%d].\n",
                   pcFName, iGeneB, iMax);
           return(FALSE); 
         }
         pDataB->isMaxRed = iMax; 
      }
      pDataB->piRedList[iNumProxiB] = iRed; /* Backpointer to shock */
      pDataB->isNumRed++;
      pDataC = pInData+iGeneC;
      iNumProxiC = pDataC->isNumRed;
      iMax = pDataC->isMaxRed - 3;
      if(iNumProxiC > iMax) {
         iMax = 1 + iNumProxiC * 2;
         if((pDataC->piRedList =
           (int *) realloc((int *) pDataC->piRedList,
               iMax * sizeof(int))) == NULL) {
           fprintf(stderr,
             "ERROR(%s): ReAlloc on pDataC(%d)->piRedList[%d].\n",
             pcFName, iGeneC, iMax);
           return(FALSE); 
         }
         pDataC->isMaxRed = iMax; 
      }
      pDataC->piRedList[iNumProxiC] = iRed; /* Backpointer to shock */
      pDataC->isNumRed++;
      pDataD = pInData+iGeneD;
      iNumProxiD = pDataD->isNumRed;
      iMax = pDataD->isMaxRed - 3;
      if(iNumProxiD > iMax) {
         iMax = 1 + iNumProxiD * 2;
         if((pDataD->piRedList =
              (int *) realloc((int *) pDataD->piRedList,
               iMax * sizeof(int))) == NULL) {
           fprintf(stderr,
             "ERROR(%s): ReAlloc on pDataD(%d)->piRedList[%d].\n",
             pcFName, iGeneD, iMax);
           return(FALSE); 
         }
         pDataD->isMaxRed = iMax; 
      }
      pDataD->piRedList[iNumProxiD] = iRed; /* Backpointer to shock */
      pDataD->isNumRed++;

      /* ------- Set END POINTS of Green curves used as intercept ------- *\
       * Endpoint should be farther away than initial curve source, but   *
       * we allow for near equality/overlap of 1st endpoint - consolidate *
      \* 2nd Endpoint should not coincide with 1st one.            */
      
      pGreenA = (pScaffold->pGreen)+iGreenA;
      iRedA = pGreenA->iRed[0]; iRedB = pGreenA->iRed[1];
      if(iRedA < 0) { /* 1st endpoint */
         pGreenA->iRed[0] = iRed;
         pGreenA->isNumEndPts = 1;
      }
      else if(iRedB < 0) { /* 2nd endpoint */
         if (iRed != iRedA) { /* Not same label or coordinates */
            pGreenA->iRed[1] = iRed;
            pGreenA->isNumEndPts = 2;
            pGreenA->isStatus = PASSIVE; /* Curve fully connected */
            iNumGreenPassive++;
         }
      }
      else if(iRed != iRedA && iRed != iRedB) { /* 3rd endpoint ... */
      }

      pGreenB = (pScaffold->pGreen)+iGreenB;
      iRedA = pGreenB->iRed[0]; iRedB = pGreenB->iRed[1];
      if(iRedA < 0) { /* 1st endpoint */
         pGreenB->iRed[0] = iRed;
         pGreenB->isNumEndPts = 1;
      }
      else if(iRedB < 0) { /* 2nd endpoint */
         if(iRed != iRedA) { /* Not same label or coordinates */
           pGreenB->iRed[1] = iRed;
           pGreenB->isNumEndPts = 2;
           pGreenB->isStatus = PASSIVE; /* Curve fully connected */
           iNumGreenPassive++;
         }
      }
      else if(iRed != iRedA && iRed != iRedB) {
      }

      if(iGreenC > -1) {
         pGreenC = (pScaffold->pGreen)+iGreenC;
         iRedA = pGreenC->iRed[0]; iRedB = pGreenC->iRed[1];
         if(iRedA < 0) { /* 1st endpoint */
           pGreenC->iRed[0] = iRed;
           pGreenC->isNumEndPts = 1;
         }
         else if(iRedB < 0) { /* 2nd endpoint */
           if(iRed != iRedA) { /* Not same label or coordinates */
              pGreenC->iRed[1] = iRed;
              pGreenC->isNumEndPts = 2;
              pGreenC->isStatus = PASSIVE; /* Curve fully connected */
              iNumGreenPassive++;
           }
         }
         else if(iRed != iRedA && iRed != iRedB) {
         }
      } /* End of If(iGreenD > -1) */

      if(iGreenD > -1) {
         pGreenD = (pScaffold->pGreen)+iGreenD;
         iRedA = pGreenD->iRed[0]; iRedB = pGreenD->iRed[1];
         if(iRedA < 0) { /* 1st endpoint */
           pGreenD->iRed[0] = iRed;
           pGreenD->isNumEndPts = 1;
         } 
         else if(iRedB < 0) { /* 2nd endpoint */
           pGreenD->iRed[1] = iRed;
           pGreenD->isNumEndPts = 2;
           pGreenD->isStatus = PASSIVE; /* Curve fully connected */
           iNumGreenPassive++;
         }
         else if(iRed != iRedA && iRed != iRedB) {
         }
      } /* End of if(iGreenD > -1) */

      /* -- Red type III : New (1) Blue & (2) Green shocks -- */

      if(iType == RED_III) {

         /* Pair of Incoming curves to red node */
         /* iGreenA = pRed->iGreen[0];  Pair of valid green shocks */
         /* iGreenB = pRed->iGreen[1];  forming the top of sliver */
       
         /* -- Order genes from Quad coords -- *\
         \*    Pair with positive coords first   */

         if(fAlpha > 0.0) {
           iTmpA = iGeneA;
           if(fBeta > 0.0) {
              iTmpB = iGeneB;
              iTmpC = iGeneC;
              iTmpD = iGeneD;
           }
           else if(fGamma > 0.0) {
               iTmpB = iGeneC;
               iTmpC = iGeneB;
               iTmpD = iGeneD;
           }
           else { /* fDelta > 0 */
              iTmpB = iGeneD;
              iTmpC = iGeneB;
              iTmpD = iGeneC;
           }
         }
         else if(fBeta > 0.0) {
            iTmpA = iGeneB;
            if(fGamma > 0.0) {
              iTmpB = iGeneC;
              iTmpC = iGeneA;
              iTmpD = iGeneD;
            }
            else { /* fDelta > 0 */
              iTmpB = iGeneD;
              iTmpC = iGeneA;
              iTmpD = iGeneC;
            }
         }
         else { /* fGamma & fDelta > 0 */
           iTmpA = iGeneC;
           iTmpB = iGeneD;
           iTmpC = iGeneA;
           iTmpD = iGeneB;
         }

         /* ------ 2nd: Set New BLUE Sheet ------ */
         
         /* - Make sure this sheet does not already exist - *\
          *   We need to compare only with existing sheets  *
         \*   linked to either genes, via backpointers.   */
         
         iNewGeneA = iTmpA; /* Pick genes with Positive Quad Coords */
         iNewGeneB = iTmpB;
         
         iBlueValid = TRUE;
         pDataA = pInData+iNewGeneA;
         iNumProxiA = pDataA->isNumBlue;
         pDataB = pInData+iNewGeneB;
         iNumProxiB = pDataB->isNumBlue;
         
         /* Check w/r to existing Blue sheets */
         if(iNumProxiA < iNumProxiB) { /* sharing A */
           piOldBlue = pDataA->piBlueList;
           for(j = 0; j < iNumProxiA; j++) {
              iOldBlue = *piOldBlue++;
              pOldBlue = (pScaffold->pBlue)+iOldBlue;
              iOldA = pOldBlue->iGene[0];
              iOldB = pOldBlue->iGene[1];
              if(iNewGeneB == iOldA || iNewGeneB == iOldB) {
                /* Already exist: Sharing both inputs */
                iBlueValid = FALSE;
                iSameBlue++;
                break;
              }
           } /* j++ */
         }
         else { /* sharing B */
           piOldBlue = pDataB->piBlueList;
           for(j = 0; j < iNumProxiB; j++) {
              iOldBlue = *piOldBlue++;
              pOldBlue = (pScaffold->pBlue)+iOldBlue;
              iOldA = pOldBlue->iGene[0];
              iOldB = pOldBlue->iGene[1];
              if(iNewGeneA == iOldA || iNewGeneA == iOldB) {
                /* Already exist: Sharing both inputs */
                iBlueValid = FALSE;
                iSameBlue++;
                break;
              }
           } /* j++ */
         }

         if(iBlueValid) {

           pNewBlue->iBuckXYZ = pNewRed->iBuckXYZ;
#if HIGH_MEM
            /* Initial shock source of sheet correspond to initial node shock */
            pNewBlue->MidPt.fPosX = pNewRed->CircumSphere.fPosX;
            pNewBlue->MidPt.fPosY = pNewRed->CircumSphere.fPosY;
            pNewBlue->MidPt.fPosZ = pNewRed->CircumSphere.fPosZ;
            pNewBlue->fDist2GeneSq = pNewRed->fDist2GeneSq;
#endif
           pNewBlue->iGene[0] = iNewGeneA;
           pNewBlue->iGene[1] = iNewGeneB;
         
           pNewBlue->isInitType = BLUE_FROM_REDIII;
           pNewBlue->isStatus   = ACTIVE;
           pNewBlue->iGreen     = IDLE;
           pNewBlue->iRed       = iRed;
           pNewBlue->isItera    = pScaffold->isLastItera;
         
           /* Set Generator Data: Backpointers to Shock and Normal data */
         
           /* pDataA = pInData+iNewGeneA;
              iNumProxiA = pDataA->isNumBlue; */
           iMax = pDataA->isMaxBlue - 3;
           if(iNumProxiA > iMax) {
             iMax = 1 + iNumProxiA * 2;
             if((pDataA->piBlueList =
                 (int *) realloc((int *) pDataA->piBlueList,
                     iMax * sizeof(int))) == NULL) {
               fprintf(stderr, 
                 "ERROR(%s): ReAlloc on pDataA(%d)->piBlueList[%d].\n",
                 pcFName, iNewGeneA, iMax);
               return(FALSE); }
             pDataA->isMaxBlue = iMax;
         
             if((pDataA->pvNormal =
                 (Vector3d *) realloc((Vector3d *) pDataA->pvNormal,
                    iMax * sizeof(Vector3d))) == NULL) {
               fprintf(stderr, 
                 "ERROR(%s): ReAlloc on pDataA(%d)->pvNormal[%d].\n",
                 pcFName, iNewGeneA, iMax);
               return(FALSE); }
             /* pDataA->isMaxNorm = iMax; */
           }
           pDataA->piBlueList[iNumProxiA] = iBlue; /* Backpointer to shock */
           pDataA->isNumBlue++;
           
           /* pDataB = pInData+iNewGeneB; */
         
           pGeneA = &(pDataA->Sample);
           pGeneB = &(pDataB->Sample);
           fPABx = pGeneB->fPosX - pGeneA->fPosX;
           fPABy = pGeneB->fPosY - pGeneA->fPosY;
           fPABz = pGeneB->fPosZ - pGeneA->fPosZ;
           fDSq = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
         
           pvNorm = pDataA->pvNormal+iNumProxiA;
           pvNorm->fX = fPABx;
           pvNorm->fY = fPABy;
           pvNorm->fZ = fPABz;
           pvNorm->fLengthSq = fDSq;
           /* pDataA->iNumNorm++; */
         
           iNumProxiB = pDataB->isNumBlue;
           iMax = pDataB->isMaxBlue - 3;
           if(iNumProxiB > iMax) {
               iMax = 1 + iNumProxiB * 2;
               if((pDataB->piBlueList =
                   (int *) realloc((int *) pDataB->piBlueList,
                         iMax * sizeof(int))) == NULL) {
                 fprintf(stderr,
                     "ERROR(%s): ReAlloc on pDataB(%d)->piBlueList[%d].\n",
                     pcFName, iNewGeneB, iMax);
                 return(FALSE); 
               }
               pDataB->isMaxBlue = iMax;
               
               if((pDataB->pvNormal =
                   (Vector3d *) realloc((Vector3d *) pDataB->pvNormal,
                        iMax * sizeof(Vector3d))) == NULL) {
                 fprintf(stderr,
                     "ERROR(%s): ReAlloc on pDataB(%d)->pvNormal[%d].\n",
                     pcFName, iNewGeneB, iMax);
                 return(FALSE); 
               }
               /* pDataB->isMaxNorm = iMax; */
           }
            pDataB->piBlueList[iNumProxiB] = iBlue; /* Backpointer to shock */
            pDataB->isNumBlue++;
            
            pvNorm = pDataB->pvNormal+iNumProxiB;
            pvNorm->fX = -fPABx;
            pvNorm->fY = -fPABy;
            pvNorm->fZ = -fPABz;
            pvNorm->fLengthSq = fDSq;
            /* pDataB->iNumNorm++; */
            
            iNumBlueMiss++;
            pNewBlue->isFlagValid = TRUE;
            iBlue++; /* Next New Valid Blue shock */
            pNewBlue++;
         }

         /* ------- 3rd: Set New PAIR of GREEN Curves -------- */
         for(iCurve = 0; iCurve < 2; iCurve++) {

           switch(iCurve) {
           case 0: /* --- 1st curve --- */
             iNewGeneC = iTmpC; /* 3rd gene with Negative Quad Coord */
             break;
           case 1: /* --- 2nd curve --- */
           default:
             iNewGeneC = iTmpD; /* 3rd gene has Negative Quad Coord */
             break;
           }

           iGreenValid = TRUE;
           pDataA = pInData+iNewGeneA;
           iNumProxiA = pDataA->isNumGreen;
           pDataB = pInData+iNewGeneB;
           iNumProxiB = pDataB->isNumGreen;
           pDataC = pInData+iNewGeneC;
           iNumProxiC = pDataC->isNumGreen;
         
           /* - Make sure this curve does not already exist - *\
            *   We need to compare only with existing curves  *
            *   linked to either genes, via backpointers.     *
           \*   Only need to check viz one gene.             */
         
           /* Check w/r to existing Green curves */
           if(iNumProxiA < iNumProxiB) {
             if(iNumProxiA < iNumProxiC) { /* Sharing GeneA */
               piOldGreen = pDataA->piGreenList;
               iNumProxi = iNumProxiA;
               iCase = 0;
             }
             else { /* Sharing GeneC */
               piOldGreen = pDataC->piGreenList;
               iNumProxi = iNumProxiC;
               iCase = 2;
             }
           }
           else if(iNumProxiB < iNumProxiC) { /* Sharing GeneB */
             piOldGreen = pDataB->piGreenList;
             iNumProxi = iNumProxiB;
             iCase = 1;
           }
           else { /* Sharing GeneC */
             piOldGreen = pDataC->piGreenList;
             iNumProxi = iNumProxiC;
             iCase = 2;
           }
         
           for(j = 0; j < iNumProxi; j++) {
             iOldGreen = *piOldGreen++;
             pOldGreen = (pScaffold->pGreen)+iOldGreen;
             iOldA = pOldGreen->iGene[0];
             iOldB = pOldGreen->iGene[1];
             iOldC = pOldGreen->iGene[2];
             switch(iCase) {
             case 0:
               iSetB = iSetC = FALSE;
               if(iNewGeneB == iOldA || iNewGeneB == iOldB || iNewGeneB == iOldC)
                 iSetB = TRUE;
               if(iNewGeneC == iOldA || iNewGeneC == iOldB || iNewGeneC == iOldC)
                 iSetC = TRUE;
               iSet = iSetB + iSetC;
               break;
             case 1:
               iSetA = iSetC = FALSE;
               if(iNewGeneA == iOldA || iNewGeneA == iOldB || iNewGeneA == iOldC)
                 iSetA = TRUE;
               if(iNewGeneC == iOldA || iNewGeneC == iOldB || iNewGeneC == iOldC)
                 iSetC = TRUE;
               iSet = iSetA + iSetC;
               break;
             case 2:
               iSetA = iSetB = FALSE;
               if(iNewGeneA == iOldA || iNewGeneA == iOldB || iNewGeneA == iOldC)
                 iSetA = TRUE;
               if(iNewGeneB == iOldA || iNewGeneB == iOldB || iNewGeneB == iOldC)
                 iSetB = TRUE;
               iSet = iSetA + iSetB;
               break;
             }
             if(iSet > 1) { /* Found the same shock */
               iGreenValid = FALSE;
               iSameGreen++;
               /* NB: Here we could add the existing Green label the Red node */
               break; /* Get out of For(j) loop */
             }
           } /* j++ */
         
           if(iGreenValid) {

               switch(iCurve) {
               case 0:
                 pNewRed->iGreen[2] = iGreen; /* 1st Exiting curve */
                 break;
               case 1:
               default:
                 if(pNewRed->iGreen[2] < 0 && pNewRed->iGreen[0] != iGreen
                    && pNewRed->iGreen[1] != iGreen)
                   pNewRed->iGreen[2] = iGreen; /* Only one new Exiting curve */
                 else if(pNewRed->iGreen[0] != iGreen && 
                     pNewRed->iGreen[1] != iGreen &&
                     pNewRed->iGreen[2] != iGreen)
                   pNewRed->iGreen[3] = iGreen; /* 2nd new Exiting curve */
                 else {
                   fprintf(stderr,
                       "ERROR(%s): Green shock %d already linked to red shock %d\n",
                       pcFName, iGreen, iRed);
                   fprintf(stderr, "\tOld Greens = (%d,%d,%d,%d)\n",
                       pNewRed->iGreen[0],pNewRed->iGreen[1],pNewRed->iGreen[2],
                       pNewRed->iGreen[3]);
                   exit(-3);
                 }
                 break;
               }

               pNewGreen->isFlagValid = TRUE;
               pNewGreen->iBuckXYZ = pNewRed->iBuckXYZ;
               
               pNewGreen->iRed[0] = iRed; /* This node is both an end & begin */
               pNewGreen->iRed[1] = IDLE;
               pNewGreen->isNumEndPts = 1;
#if HIGH_MEM
               pNewGreen->CircumTri.fPosX = pNewRed->CircumSphere.fPosX;
               pNewGreen->CircumTri.fPosY = pNewRed->CircumSphere.fPosY;
               pNewGreen->CircumTri.fPosZ = pNewRed->CircumSphere.fPosZ;
               pNewGreen->fDist2GeneSq = pNewRed->fDist2GeneSq;
#endif
               pNewGreen->isInitType = GREEN_FROM_REDIII;
               pNewGreen->isStatus   = ACTIVE;
               pNewGreen->isItera    = pScaffold->isLastItera;
               
               /* First 2 genes are at the bottom of the sliver   *\
                * creating the new Blue sheet & part of the curve *
               \* 3rd gene: at top of sliver     */
               pNewGreen->iGene[0] = iNewGeneA;
               pNewGreen->iGene[1] = iNewGeneB;
               pNewGreen->iGene[2] = iNewGeneC;
               
               /* -- Find the associated Blue sheets (see below) -- */
               
               pNewGreen->iBlue[0] = IDLE;
               pNewGreen->iBlue[1] = IDLE;
               pNewGreen->iBlue[2] = IDLE;
               
               /* -- Set backpointers to curve shock for the 3 genes -- */
               pDataA = pInData+iNewGeneA;
               iNumProxiA = pDataA->isNumGreen;
               iMax = pDataA->isMaxGreen - 3;
               if(iNumProxiA > iMax) {
                  iMax = 1 + iNumProxiA * 2;
                  if((pDataA->piGreenList =
                    (int *) realloc((int *) pDataA->piGreenList,
                     iMax * sizeof(int))) == NULL) {
                   fprintf(stderr,
                       "ERROR(%s): ReAlloc on pDataA(%d)->piGreenList[%d].\n",
                       pcFName, iNewGeneA, iMax);
                   return(FALSE); 
                  }
                  pDataA->isMaxGreen = iMax; 
               }
               pDataA->piGreenList[iNumProxiA] = iGreen; /* Backpointer to shock */
               pDataA->isNumGreen++;
               pDataB = pInData+iNewGeneB;
               iNumProxiB = pDataB->isNumGreen;
               iMax = pDataB->isMaxGreen - 3;
               if(iNumProxiB > iMax) {
                 iMax = 1 + iNumProxiB * 2;
                 if((pDataB->piGreenList =
                 (int *) realloc((int *) pDataB->piGreenList,
                     iMax * sizeof(int))) == NULL) {
                   fprintf(stderr,
                       "ERROR(%s): ReAlloc on pDataB(%d)->piGreenList[%d].\n",
                       pcFName, iNewGeneB, iMax);
                   return(FALSE); 
                 }
                 pDataB->isMaxGreen = iMax; 
               }
               pDataB->piGreenList[iNumProxiB] = iGreen; /* Backpointer to shock */
               pDataB->isNumGreen++;
               pDataC = pInData+iNewGeneC;
               iNumProxiC = pDataC->isNumGreen;
               iMax = pDataC->isMaxGreen - 3;
               if(iNumProxiC > iMax) {
                 iMax = 1 + iNumProxiC * 2;
                 if((pDataC->piGreenList =
                 (int *) realloc((int *) pDataC->piGreenList,
                     iMax * sizeof(int))) == NULL) {
                   fprintf(stderr,
                       "ERROR(%s): ReAlloc on pDataC(%d)->piGreenList[%d].\n",
                       pcFName, iNewGeneC, iMax);
                   return(FALSE); 
                 }
                 pDataC->isMaxGreen = iMax; 
               }
               pDataC->piGreenList[iNumProxiC] = iGreen; /* Backpointer to shock */
               pDataC->isNumGreen++;
               
               /* ---- Set 3 Blue slots of new Green Curve ---- */
               
               for(iSheet = 0; iSheet < 3; iSheet++) {
                  switch(iSheet) {
                  case 0: /* -- Find or Set 1st Pair: AB -- */
                    iNumProxiBlue = pDataA->isNumBlue; /* Sharing GeneA */
                    piOldBlue  = pDataA->piBlueList;
                    iNewGeneBlue = iNewGeneB; /* Find GeneB */
                    break;
                  case 1: /* -- Find or Set 2nd Pair: AC -- */
                    iNumProxiBlue = pDataA->isNumBlue; /* Sharing GeneA */
                    piOldBlue  = pDataA->piBlueList;
                    iNewGeneBlue = iNewGeneC; /* Find GeneC */
                    break;
                  case 2: /* -- Find or Set 3rd Pair: BC -- */
                    iNumProxiBlue = pDataB->isNumBlue; /* Sharing GeneB */
                    piOldBlue  = pDataB->piBlueList;
                    iNewGeneBlue = iNewGeneC; /* Find GeneC */
                    break;
                  }
               
                  iFindSndGene = FALSE;
                  for(j = 0; j < iNumProxiBlue; j++) {
                    iOldBlue = *piOldBlue++;
                    pOldBlue = (pScaffold->pBlue)+iOldBlue;
                    iOldA = pOldBlue->iGene[0];
                    iOldB = pOldBlue->iGene[1];
                  
                    if((iNewGeneBlue == iOldA) || (iNewGeneBlue == iOldB)) {
                    iFindSndGene = TRUE;
                    break;
                    }
                  } /* Next Blue Proxi : j++ */
                  if(iFindSndGene) {
                    switch(iSheet) {
                    case 0: /* -- Found Pair AB -- */
                    pNewGreen->iBlue[0] = iOldBlue;
                    break;
                    case 1: /* -- Found Pair AC -- */
                    pNewGreen->iBlue[1] = iOldBlue;
                    break;
                    case 2: /* -- Found Pair BC -- */
                    pNewGreen->iBlue[2] = iOldBlue;
                    break;
                    }
                  }
                  else { /* This sheet does not exist: Create it */
                    switch(iSheet) {
                    case 0: /* -- Set Pair AB -- */
                    pNewGreen->iBlue[0] = iBlue;
                    pGeneA = &(pDataA->Sample);
                    pGeneB = &(pDataB->Sample);
                    pNewBlue->iGene[0] = iNewGeneA;
                    pNewBlue->iGene[1] = iNewGeneB;
                    pData1 = pDataA;
                    pData2 = pDataB;
                    break;
                    case 1: /* -- Find or Set 2nd Pair: AC -- */
                    pNewGreen->iBlue[1] = iBlue;
                    pGeneA = &(pDataA->Sample);
                    pGeneB = &(pDataC->Sample);
                    pNewBlue->iGene[0] = iNewGeneA;
                    pNewBlue->iGene[1] = iNewGeneC;
                    pData1 = pDataA;
                    pData2 = pDataC;
                    break;
                    case 2: /* -- Find or Set 3rd Pair: BC -- */
                    pNewGreen->iBlue[2] = iBlue;
                    pGeneA = &(pDataB->Sample);
                    pGeneB = &(pDataC->Sample);
                    pNewBlue->iGene[0] = iNewGeneB;
                    pNewBlue->iGene[1] = iNewGeneC;
                    pData1 = pDataB;
                    pData2 = pDataC;
                    break;
                    }
                  
                    /* Initial sheet shock src corresponds to midpoint */
                  
                    fPABx = pGeneB->fPosX - pGeneA->fPosX;
                    fPABy = pGeneB->fPosY - pGeneA->fPosY;
                    fPABz = pGeneB->fPosZ - pGeneA->fPosZ;
                    fDSq = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
                  
                    fPosXBlue = (pGeneA->fPosX + pGeneB->fPosX) / 2.0;
                    fPosYBlue = (pGeneA->fPosY + pGeneB->fPosY) / 2.0;
                    fPosZBlue = (pGeneA->fPosZ + pGeneB->fPosZ) / 2.0;
#if HIGH_MEM
                    pNewBlue->fDist2GeneSq = fDSq / 4.0;
                    pNewBlue->MidPt.fPosX = fPosXBlue;
                    pNewBlue->MidPt.fPosY = fPosYBlue;
                    pNewBlue->MidPt.fPosZ = fPosZBlue;
#endif
                     /* Find Bucket where shock is located */
                     iPosX = (int) floor((double) fPosXBlue);
                     iPosY = (int) floor((double) fPosYBlue);
                     iPosZ = (int) floor((double) fPosZBlue);
                     iPos = iPosZ * iSliceSize + iPosY * iXdim + iPosX;
                     iLabelBuckShock = *(pisLookUp+iPos);
                     pNewBlue->iBuckXYZ = iLabelBuckShock;
                     
                     pNewBlue->isInitType = BLUE_MISS;
                     pNewBlue->iGreen     = iGreen;
                     pNewBlue->iRed       = IDLE;
                     pNewBlue->isItera    = pScaffold->isLastItera;
                     pNewBlue->isStatus   = ACTIVE;
                     
                     /* Set Gene Data: Backpointers to shock and Normal data */
                     
                     iNumProxi1 = pData1->isNumBlue;
                     iMax1 = pData1->isMaxBlue - 3;
                     if(iNumProxi1 > iMax1) {
                        iMax1 = 1 + iNumProxi1 * 2;
                        if((pData1->piBlueList =
                            (int *) realloc((int *) pData1->piBlueList,
                                iMax1 * sizeof(int))) == NULL) {
                          fprintf(stderr,
                            "ERROR(%s): ReAlloc pData1->piBlueList[%d].\n",
                            pcFName, iMax1);
                          return(FALSE); }
                        if((pData1->pvNormal =
                            (Vector3d *) realloc((Vector3d *) pData1->pvNormal,
                               iMax1 * sizeof(Vector3d))) == NULL) {
                          fprintf(stderr,
                            "ERROR(%s): ReAlloc on pData1->pvNormal[%d].\n",
                            pcFName, iMax1);
                          return(FALSE); }
                        pData1->isMaxBlue = iMax1;
                        /* pData1->isMaxNorm = iMax1; */
                    }
                     pData1->piBlueList[iNumProxi1] = iBlue;
                     pData1->isNumBlue++;
                     pvNorm = pData1->pvNormal+iNumProxi1;
                     pvNorm->fX = fPABx;
                     pvNorm->fY = fPABy;
                     pvNorm->fZ = fPABz;
                     pvNorm->fLengthSq = fDSq;
                     /* pData1->isNumNorm++; */
                     iNumProxi2 = pData2->isNumBlue;
                     iMax2 = pData2->isMaxBlue - 3;
                     if(iNumProxi2 > iMax2) {
                        iMax2 = 1 + iNumProxi2 * 2;
                        if((pData2->piBlueList =
                            (int *) realloc((int *) pData2->piBlueList,
                                iMax2 * sizeof(int))) == NULL) {
                          fprintf(stderr,
                            "ERROR(%s): ReAlloc pData2->piBlueList[%d].\n",
                            pcFName, iMax2);
                          return(FALSE); }
                        if((pData2->pvNormal =
                            (Vector3d *) realloc((Vector3d *) pData2->pvNormal,
                               iMax2 * sizeof(Vector3d))) == NULL) {
                          fprintf(stderr,
                            "ERROR(%s): ReAlloc pData2->pvNormal[%d].\n",
                            pcFName, iMax2);
                          return(FALSE); }
                        pData2->isMaxBlue = iMax2;
                        /* pData2->isMaxNorm = iMax2; */
                    }
                     pData2->piBlueList[iNumProxi2] = iBlue;
                     pData2->isNumBlue++;
                     pvNorm = pData2->pvNormal+iNumProxi2;
                     pvNorm->fX = -fPABx;
                     pvNorm->fY = -fPABy;
                     pvNorm->fZ = -fPABz;
                     pvNorm->fLengthSq = fDSq;
                     /* pData2->isNumNorm++; */
                     
                     iNumBlueMiss++;
                     pNewBlue->isFlagValid = TRUE;
                     iBlue++; /* Next New Valid Blue shock */
                     pNewBlue++;
                 } /* End of creation of missing sheet */

              } /* End of for(iSheet) */

               /* -- Find the Type of shock curve via Trilinear Coordinates -- */
               if(!GetTriCoordOfCurve(pInData, pNewGreen, pShockData)) {
                 fprintf(stderr,
                     "ERROR(%s): Tri coords failed on Green shock %d\n",
                     pcFName, iGreen);
               }
    
               iGreen++; /* Next New Valid Green shock */
              pNewGreen++;

           } /* End of if(iGreenValid) */

         } /* End of for(iCurve) */

         iNumRedIII++;
      } /* End of case RED_III */

      /* -- Red type II : New Green curve -- */
      else if(iType == RED_II) {

         /* -- 1st: Find which gene is on the far-side of the cap -- */
         /*    I.e. which is shared by 3 triangles of incoming curves  */
   
         /*  Incoming curves to red node */
         /* iGreenA = pRed->iGreen[0]; Triad of valid green shocks */
         /* iGreenB = pRed->iGreen[1]; forming the pyramid cap */
         /* iGreenC = pRed->iGreen[2]; */
   
         /* -- Order genes from Quad coords -- *\
         \*    Gene with negative coord last   */

         if(fAlpha < 0.0) {
           iNewGeneA = iGeneB;
           iNewGeneB = iGeneC;
           iNewGeneC = iGeneD;
           /* iNewGeneD = iGeneA; */
           /* fTmp1 = fAlpha + 0.1; */
         }
         else if(fBeta < 0.0) {
           iNewGeneA = iGeneA;
           iNewGeneB = iGeneC;
           iNewGeneC = iGeneD;
           /* iNewGeneD = iGeneB; */
           /* fTmp1 = fBeta + 0.1; */
         }
         else if(fGamma < 0.0) {
           iNewGeneA = iGeneA;
           iNewGeneB = iGeneB;
           iNewGeneC = iGeneD;
           /* iNewGeneD = iGeneC; */
           /* fTmp1 = fGamma + 0.1; */
         }
         else { /* fDelta < 0.0 */
           iNewGeneA = iGeneA;
           iNewGeneB = iGeneB;
           iNewGeneC = iGeneC;
           /* iNewGeneD = iGeneD; */
           /* fTmp1 = fDelta + 0.1; */
         }

         /* -- 2nd: Set (single) New GREEN Curve -- */
         
         iGreenValid = TRUE;
         pDataA = pInData+iNewGeneA;
         iNumProxiA = pDataA->isNumGreen;
         pDataB = pInData+iNewGeneB;
         iNumProxiB = pDataB->isNumGreen;
         pDataC = pInData+iNewGeneC;
         iNumProxiC = pDataC->isNumGreen;
         
         /* - Make sure this curve does not already exist - *\
          *   We need to compare only with existing curves  *
          *   linked to either genes, via backpointers.     *
         \*   Only need to check viz one gene.           */

         /* Check w/r to existing Green curves */
         if(iNumProxiA < iNumProxiB) {
           if(iNumProxiA < iNumProxiC) { /* Sharing GeneA */
             piOldGreen = pDataA->piGreenList;
             iNumProxi = iNumProxiA;
             iCase = 0;
           }
           else { /* Sharing GeneC */
             piOldGreen = pDataC->piGreenList;
             iNumProxi = iNumProxiC;
             iCase = 2;
           }
         }
         else if(iNumProxiB < iNumProxiC) { /* Sharing GeneB */
            piOldGreen = pDataB->piGreenList;
            iNumProxi = iNumProxiB;
            iCase = 1;
         }
         else { /* Sharing GeneC */
            piOldGreen = pDataC->piGreenList;
            iNumProxi = iNumProxiC;
            iCase = 2;
         }
      
         for(j = 0; j < iNumProxi; j++) {
           iOldGreen = *piOldGreen++;
           pOldGreen = (pScaffold->pGreen)+iOldGreen;
           iOldA = pOldGreen->iGene[0];
           iOldB = pOldGreen->iGene[1];
           iOldC = pOldGreen->iGene[2];
           switch(iCase) {
           case 0:
             iSetB = iSetC = FALSE;
             if(iNewGeneB == iOldA || iNewGeneB == iOldB || iNewGeneB == iOldC)
               iSetB = TRUE;
             if(iNewGeneC == iOldA || iNewGeneC == iOldB || iNewGeneC == iOldC)
               iSetC = TRUE;
             iSet = iSetB + iSetC;
             break;
           case 1:
             iSetA = iSetC = FALSE;
             if(iNewGeneA == iOldA || iNewGeneA == iOldB || iNewGeneA == iOldC)
               iSetA = TRUE;
             if(iNewGeneC == iOldA || iNewGeneC == iOldB || iNewGeneC == iOldC)
               iSetC = TRUE;
             iSet = iSetA + iSetC;
             break;
           case 2:
             iSetA = iSetB = FALSE;
             if(iNewGeneA == iOldA || iNewGeneA == iOldB || iNewGeneA == iOldC)
               iSetA = TRUE;
             if(iNewGeneB == iOldA || iNewGeneB == iOldB || iNewGeneB == iOldC)
               iSetB = TRUE;
             iSet = iSetA + iSetB;
             break;
           }
           if(iSet > 1) { /* Found the same shock */
             iGreenValid = FALSE;
             iSameGreen++;
             /* NB: Here we could add the existing Green label the Red node */
             break; /* Get out of For(j) loop */
           }
         } /* j++ */

         if(iGreenValid) {

           if(pNewRed->iGreen[3] < 0 && pNewRed->iGreen[0] != iGreen
              && pNewRed->iGreen[1] != iGreen
              && pNewRed->iGreen[2] != iGreen)
             pNewRed->iGreen[3] = iGreen; /* Exiting curve */
           else {
             fprintf(stderr,
               "ERROR(%s): Green shock %d already linked to red shock %d\n",
               pcFName, iGreen, iRed);
             fprintf(stderr, "\tOld Greens = (%d,%d,%d,%d)\n",
               pNewRed->iGreen[0],pNewRed->iGreen[1],pNewRed->iGreen[2],
               pNewRed->iGreen[3]);
             exit(-3);
           }

           pNewGreen->isFlagValid = TRUE;
           pNewGreen->iBuckXYZ = pNewRed->iBuckXYZ;
         
           pNewGreen->iRed[0] = iRed; /* This node is both an end & begin */
           pNewGreen->iRed[1] = IDLE;
           pNewGreen->isNumEndPts = 1;
#if HIGH_MEM
            pNewGreen->CircumTri.fPosX = pNewRed->CircumSphere.fPosX;
            pNewGreen->CircumTri.fPosY = pNewRed->CircumSphere.fPosY;
            pNewGreen->CircumTri.fPosZ = pNewRed->CircumSphere.fPosZ;
            pNewGreen->fDist2GeneSq = pNewRed->fDist2GeneSq;
#endif
            pNewGreen->isInitType = GREEN_FROM_REDII;
            pNewGreen->isStatus   = ACTIVE;
            pNewGreen->isItera    = pScaffold->isLastItera;
            
            /* The 3 genes are at the bottom of the cap */
            pNewGreen->iGene[0] = iNewGeneA;
            pNewGreen->iGene[1] = iNewGeneB;
            pNewGreen->iGene[2] = iNewGeneC;
            
            /* -- Find the associated 3 Blue sheets (see below) -- */
            
            pNewGreen->iBlue[0] = IDLE;
            pNewGreen->iBlue[1] = IDLE;
            pNewGreen->iBlue[2] = IDLE;
            
            /* -- Set backpointers to curve shock for the 3 genes -- */
            pDataA = pInData+iNewGeneA;
            iNumProxiA = pDataA->isNumGreen;
            iMax = pDataA->isMaxGreen - 2;
            if(iNumProxiA > iMax) {
              iMax = 1 + iNumProxiA * 2;
              if((pDataA->piGreenList =
                  (int *) realloc((int *) pDataA->piGreenList,
                      iMax * sizeof(int))) == NULL) {
                fprintf(stderr,
                  "ERROR(%s): ReAlloc on pDataA(%d)->piGreenList[%d].\n",
                  pcFName, iNewGeneA, iMax);
                return(FALSE); }
              pDataA->isMaxGreen = iMax; }
            pDataA->piGreenList[iNumProxiA] = iGreen; /* Backpointer to shock */
            pDataA->isNumGreen++;
            pDataB = pInData+iNewGeneB;
            iNumProxiB = pDataB->isNumGreen;
            iMax = pDataB->isMaxGreen - 2;
            if(iNumProxiB > iMax) {
              iMax = 1 + iNumProxiB * 2;
              if((pDataB->piGreenList =
                  (int *) realloc((int *) pDataB->piGreenList,
                      iMax * sizeof(int))) == NULL) {
                fprintf(stderr,
                  "ERROR(%s): ReAlloc on pDataB(%d)->piGreenList[%d].\n",
                  pcFName, iNewGeneB, iMax);
                return(FALSE); }
              pDataB->isMaxGreen = iMax; }
            pDataB->piGreenList[iNumProxiB] = iGreen; /* Backpointer to shock */
            pDataB->isNumGreen++;
            pDataC = pInData+iNewGeneC;
            iNumProxiC = pDataC->isNumGreen;
            iMax = pDataC->isMaxGreen - 2;
            if(iNumProxiC > iMax) {
              iMax = 1 + iNumProxiC * 2;
              if((pDataC->piGreenList =
                  (int *) realloc((int *) pDataC->piGreenList,
                      iMax * sizeof(int))) == NULL) {
                fprintf(stderr,
                  "ERROR(%s): ReAlloc on pDataC(%d)->piGreenList[%d].\n",
                  pcFName, iNewGeneC, iMax);
                return(FALSE); }
              pDataC->isMaxGreen = iMax; }
            pDataC->piGreenList[iNumProxiC] = iGreen; /* Backpointer to shock */
            pDataC->isNumGreen++;
            
            /* ---- Set 3 Blue slots of new Green Curve ---- */
            
            for(iSheet = 0; iSheet < 3; iSheet++) {
              switch(iSheet) {
              case 0: /* -- Find or Set 1st Pair: AB -- */
                iNumProxiBlue = pDataA->isNumBlue; /* Sharing GeneA */
                piOldBlue  = pDataA->piBlueList;
                iNewGeneBlue = iNewGeneB; /* Find GeneB */
                break;
              case 1: /* -- Find or Set 2nd Pair: AC -- */
                iNumProxiBlue = pDataA->isNumBlue; /* Sharing GeneA */
                piOldBlue  = pDataA->piBlueList;
                iNewGeneBlue = iNewGeneC; /* Find GeneC */
                break;
              case 2: /* -- Find or Set 3rd Pair: BC -- */
                iNumProxiBlue = pDataB->isNumBlue; /* Sharing GeneB */
                piOldBlue  = pDataB->piBlueList;
                iNewGeneBlue = iNewGeneC; /* Find GeneC */
                break;
              }
            
              iFindSndGene = FALSE;
              for(j = 0; j < iNumProxiBlue; j++) {
                iOldBlue = *piOldBlue++;
                pOldBlue = (pScaffold->pBlue)+iOldBlue;
                iOldA = pOldBlue->iGene[0];
                iOldB = pOldBlue->iGene[1];
            
                if((iNewGeneBlue == iOldA) || (iNewGeneBlue == iOldB)) {
                  iFindSndGene = TRUE;
                  break;
                }
              } /* Next Blue Proxi : j++ */
              if(iFindSndGene) {
                switch(iSheet) {
                case 0: /* -- Found Pair AB -- */
                  pNewGreen->iBlue[0] = iOldBlue;
                  break;
                case 1: /* -- Found Pair AC -- */
                  pNewGreen->iBlue[1] = iOldBlue;
                  break;
                case 2: /* -- Found Pair BC -- */
                  pNewGreen->iBlue[2] = iOldBlue;
                  break;
                }
              }
              else { /* This sheet does not exist: Create it */
            
                switch(iSheet) {
                case 0: /* -- Set Pair AB -- */
                  pNewGreen->iBlue[0] = iBlue;
                  pGeneA = &(pDataA->Sample);
                  pGeneB = &(pDataB->Sample);
                  pNewBlue->iGene[0] = iNewGeneA;
                  pNewBlue->iGene[1] = iNewGeneB;
                  pData1 = pDataA;
                  pData2 = pDataB;
                  break;
                case 1: /* -- Find or Set 2nd Pair: AC -- */
                  pNewGreen->iBlue[1] = iBlue;
                  pGeneA = &(pDataA->Sample);
                  pGeneB = &(pDataC->Sample);
                  pNewBlue->iGene[0] = iNewGeneA;
                  pNewBlue->iGene[1] = iNewGeneC;
                  pData1 = pDataA;
                  pData2 = pDataC;
                  break;
                case 2: /* -- Find or Set 3rd Pair: BC -- */
                  pNewGreen->iBlue[2] = iBlue;
                  pGeneA = &(pDataB->Sample);
                  pGeneB = &(pDataC->Sample);
                  pNewBlue->iGene[0] = iNewGeneB;
                  pNewBlue->iGene[1] = iNewGeneC;
                  pData1 = pDataB;
                  pData2 = pDataC;
                  break;
                }
            
                /* Initial sheet shock src corresponds to midpoint */
            
                fPABx = pGeneB->fPosX - pGeneA->fPosX;
                fPABy = pGeneB->fPosY - pGeneA->fPosY;
                fPABz = pGeneB->fPosZ - pGeneA->fPosZ;
                fDSq = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
            
                fPosXBlue = (pGeneA->fPosX + pGeneB->fPosX) / 2.0;
                fPosYBlue = (pGeneA->fPosY + pGeneB->fPosY) / 2.0;
                fPosZBlue = (pGeneA->fPosZ + pGeneB->fPosZ) / 2.0;
#if HIGH_MEM
               pNewBlue->fDist2GeneSq = fDSq / 4.0;
               pNewBlue->MidPt.fPosX = fPosXBlue;
               pNewBlue->MidPt.fPosY = fPosYBlue;
               pNewBlue->MidPt.fPosZ = fPosZBlue;
#endif
               /* Find Bucket where shock is located */
               iPosX = (int) floor((double) fPosXBlue);
               iPosY = (int) floor((double) fPosYBlue);
               iPosZ = (int) floor((double) fPosZBlue);
               iPos = iPosZ * iSliceSize + iPosY * iXdim + iPosX;
               iLabelBuckShock = *(pisLookUp+iPos);
               pNewBlue->iBuckXYZ = iLabelBuckShock;
         
               pNewBlue->isInitType = BLUE_MISS;
               pNewBlue->iGreen     = iGreen;
               pNewBlue->iRed       = IDLE;
               pNewBlue->isItera    = pScaffold->isLastItera;
               pNewBlue->isStatus   = ACTIVE;
         
               /* Set Gene Data: Backpointers to shock and Normal data */
                 
               iNumProxi1 = pData1->isNumBlue;
               iMax1 = pData1->isMaxBlue - 3;
               if(iNumProxi1 > iMax1) {
                 iMax1 = 1 + iNumProxi1 * 2;
                 if((pData1->piBlueList =
               (int *) realloc((int *) pData1->piBlueList,
                   iMax1 * sizeof(int))) == NULL) {
                fprintf(stderr,
                  "ERROR(%s): ReAlloc pData1->piBlueList[%d].\n",
                  pcFName, iMax1);
                return(FALSE); }
                    if((pData1->pvNormal =
                  (Vector3d *) realloc((Vector3d *) pData1->pvNormal,
                           iMax1 * sizeof(Vector3d))) == NULL) {
                fprintf(stderr,
                  "ERROR(%s): ReAlloc on pData1->pvNormal[%d].\n",
                  pcFName, iMax1);
                return(FALSE); }
                    pData1->isMaxBlue = iMax1;
                    /* pData1->isMaxNorm = iMax1; */
               }
                pData1->piBlueList[iNumProxi1] = iBlue;
                pData1->isNumBlue++;
                pvNorm = pData1->pvNormal+iNumProxi1;
                pvNorm->fX = fPABx;
                pvNorm->fY = fPABy;
                pvNorm->fZ = fPABz;
                pvNorm->fLengthSq = fDSq;
                /* pData1->isNumNorm++; */
                iNumProxi2 = pData2->isNumBlue;
                iMax2 = pData2->isMaxBlue - 3;
                if(iNumProxi2 > iMax2) {
                  iMax2 = 1 + iNumProxi2 * 2;
                  if((pData2->piBlueList =
                 (int *) realloc((int *) pData2->piBlueList,
                     iMax2 * sizeof(int))) == NULL) {
                fprintf(stderr,
                  "ERROR(%s): ReAlloc pData2->piBlueList[%d].\n",
                  pcFName, iMax2);
                return(FALSE); }
                    if((pData2->pvNormal =
                  (Vector3d *) realloc((Vector3d *) pData2->pvNormal,
                           iMax2 * sizeof(Vector3d))) == NULL) {
                fprintf(stderr,
                  "ERROR(%s): ReAlloc pData2->pvNormal[%d].\n",
                  pcFName, iMax2);
                return(FALSE); }
                    pData2->isMaxBlue = iMax2;
                    /* pData2->isMaxNorm = iMax2; */
                  }
                  pData2->piBlueList[iNumProxi2] = iBlue;
                  pData2->isNumBlue++;
                  pvNorm = pData2->pvNormal+iNumProxi2;
                  pvNorm->fX = -fPABx;
                  pvNorm->fY = -fPABy;
                  pvNorm->fZ = -fPABz;
                  pvNorm->fLengthSq = fDSq;
                  /* pData2->isNumNorm++; */
            
                  iNumBlueMiss++;
                  pNewBlue->isFlagValid = TRUE;
                  iBlue++; /* Next New Valid Blue shock */
                  pNewBlue++;
              } /* End of creation of missing sheet */

           } /* End of for(iSheet) */

           /* -- Find the Type of shock curve via Trilinear Coordinates -- */
           if(!GetTriCoordOfCurve(pInData, pNewGreen, pShockData)) {
             fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
               pcFName, iGreen);
           }
         
           iGreen++; /* Next New Valid Green shock */
           pNewGreen++;
         }

         iNumRedII++;
      } /* End of RED_II case */

      /* -- Red type I : Final node : No new curves nor sheets created -- */
      else { /* iType == RED_I */
        iNumRedI++;
      }

      /* ----- Check if Red Shock still has missing links ----- */


      iRed++; /* Next Valid Red shock */
      iCircum++;
      pNewRed++;

   } /* i++ */

   /* -- Print Info - update Scaffold -- */
   
   fprintf(stderr, "\tPut %d Red Vertices on Scaffold.\n", iCircum);
   fprintf(stderr,
       "\t\tTypes: I = %d , II = %d (caps) , III = %d (slivers).\n", 
       iNumRedI, iNumRedII, iNumRedIII);
   if((iNumRedI+iNumRedII+iNumRedIII) != iCircum) {
     fprintf(stderr, "\tWARNING: iNumRedI+iNumRedII+iNumRedIII != iCircum\n");
   }
   pScaffold->iNumRedLast = iRed;
   pScaffold->iNumRedNew  = iCircum;
   pScaffold->iNumTetRegulars = iNumRedI;
   pScaffold->iNumTetCaps     = iNumRedII;
   pScaffold->iNumTetSlivers  = iNumRedIII;
   fprintf(stderr, "\t\tTet. Volumes: Min = %f , Max = %f\n",
       fVolMin, fVolMax);
   pScaffold->fTetVolMin = fVolMin;
   pScaffold->fTetVolMax = fVolMax;
   
   iOldGreen = pScaffold->iGreenActiveLast;
   pScaffold->iGreenActiveLast = iGreen;
   fprintf(stderr, "\tThere are now %d Green Shocks on the Scaffold,\n",
       iGreen);
   iNewGreen = iGreen - iOldGreen;
   fprintf(stderr, "\t\t%d are New ones (from Red type II & III),\n",
       iNewGreen);
   fprintf(stderr, "\t\t%d were fully connected (PASSIVE),\n",
       iNumGreenPassive);
   pScaffold->iNumGreenPassive += iNumGreenPassive;
   pScaffold->iNumGreenActive = iGreen - pScaffold->iNumGreenPassive;
   /* MING if(pScaffold->iNumGreenActive < 0) {
     fprintf(stderr,
         "ERROR(%s): Negative number (%d) of Active Green shocks...\n",
         pcFName, pScaffold->iNumGreenActive);
     exit(-5);
   }*/
   fprintf(stderr, "\t\t%d are still ACTIVE.\n", pScaffold->iNumGreenActive);
   
   fprintf(stderr, "\t\tDuplicates of curves = %d\n", iSameGreen);
   
   iTmp = iBlue - pScaffold->iBlueActiveLast;
   fprintf(stderr,
       "\tNew Blue sheets (from Red III & Green II) = %d.\n", iTmp);
   fprintf(stderr, "\t\tDuplicates of sheets = %d\n", iSameBlue);
   pScaffold->iBlueActiveLast = iBlue;
   pScaffold->iNumBlueActive += iTmp;
   if(iNumInvalid > 0) {
     fprintf(stderr, "\t\tInvalid Sheets = %d\n", iNumInvalid);
   }
   if(iTmp != iNumBlueMiss) {
     fprintf(stderr,
         "WARNING: Number of Missed Blue Sheets = %d != new sheets = %d\n",
         iNumBlueMiss, iTmp);
   }
   
   /* --- Reset pDmiMBuck --- */
   
   pDimMBuck->iNumShockSheetIntAll = pDimMBuck->iNumShockSheet =
     pScaffold->iBlueActiveLast;
   pDimMBuck->iNumShockCurve = pScaffold->iGreenActiveLast;
   pDimMBuck->iNumShockVertex = pScaffold->iNumRedLast;
   
   return(TRUE);
}

/* ----- EoF ----- */
