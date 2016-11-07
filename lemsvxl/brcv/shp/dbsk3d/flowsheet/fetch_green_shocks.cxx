/*------------------------------------------------------------*\
 * File: fetch_green_shocks.c
 *
 * Functions:
 *  FetchGreenShocksInt
 *  FetchGreenShocksFstLayer
 *  SetValidCurvesInt
 *  SetValidCurvesFstLayer
 *  SetGreenPassive
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie (adapted from old version)
 *  #1: June 2002: Fills-in 3rd Blue slot of Green curves
 *    and create new (missing) sheets if needed.
 *  #2: July 2003: Updated for use with MetaBuckets.
 *
\*------------------------------------------------------------*/

#include "fetch_green_shocks.h"
#include "circum.h"
#include "valid_shocks.h"
#include "barycentric.h"

/*------------------------------------------------------------*\
 * Function: FetchGreenShocksInt
 *
 * Usage: Seek Initial Curve shocks - some are part of the MA
 *    and thus, the source of a shock curve, others are only
 *    part of the SS. Keep only valid shocks (on MA).
 *    Particular to the 1st pass for generators INSIDE a
 *    bucket. We intersect sheets found at Full Resolution
 *    only, in each bucket, since those found a sub-resolution
 *    tend to lead to invalid shocks.
 *    We do not need to test for duplication of shock curves
 *    here, because we can easily avoid permutations of
 *    triplets of generators.
 *
\*------------------------------------------------------------*/

void
FetchGreenShocksInt(InputData *pInData, ShockScaffold *pScaf,
        ShockCurve *pShockCurves, short *pisLookUp,
        Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
        Voxel *pVoxels, Dimension *pDimMBuck,
        Dimension *pDimAll)
{
  char    *pcFName = "FetchGreenShocksInt";
  register int  i, j, k, l;
  short    isFlagPermute, isFlagPermuteNot, iCircum, iNumInvalid;
  int    iGeneA,iGeneB,iGeneC, iNext, iOffShootsToInfinity;
  int    iNumBucks, iNumGeneInBuckA, iNumBlueA, iBlue1,iBlue2;
  int    iPos,iPosXs,iPosYs,iPosZs, iSliceSize, iLabelBuckShock;
  int    iXdim, iOutOfBox;
  int    iXmin,iYmin,iZmin, iXmax,iYmax,iZmax;
  int    *piGeneInBuckA, *piBlue1,*piBlue2;
  /* static float  fEpsilon = (float) D_EPSILON; */
  float    fDX,fDY,fDZ, fPosXs,fPosYs,fPosZs, fDSq;
  float    fDistSqLargest, fDistSqLarge;
  double  dGeneA[3],dGeneB[3],dGeneC[3],dCircum[3];
  ShockSheet  *pBlue1, *pBlue2;
  ShockCurve  *pGreen;
  Pt3dCoord  *pGeneA, *pGeneB, *pGeneC;
  Bucket  *pBuckA;
  InputData  *pDataA, *pDataB, *pDataC;
  ShockData  *pShockData;

  /* Preliminaries */

  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }
  pShockData->iGene[3] = IDLE;

  iXdim = pDimMBuck->iXdim;
  iXmin = pDimMBuck->Limits.iMinX;
  iYmin = pDimMBuck->Limits.iMinY;
  iZmin = pDimMBuck->Limits.iMinZ;
  iXmax = pDimMBuck->Limits.iMaxX;
  iYmax = pDimMBuck->Limits.iMaxY;
  iZmax = pDimMBuck->Limits.iMaxZ;
  iSliceSize = pDimMBuck->iSliceSize;
  iNumBucks = pBucksXYZ->iNumBucks;
  iOffShootsToInfinity = iNumInvalid = 0;
  fDistSqLarge = fDistSqLargest = 0.0;

  fprintf(stderr, "MESG(%s):\n", pcFName);

  pBucksXYZ->iInitCurveLabel = 0;

  iCircum = iOutOfBox = 0;
  isFlagPermute = pDimMBuck->isFlagPermute;
  isFlagPermuteNot = isFlagPermute - 1;

  /* ------------ Process BucketsXYZ one by one ----------- *\
  \*      Seek triplets of generators        */

  pGreen = pShockCurves;
  pBuckA = pBucksXYZ->pBucket-1;
  for(i = 0; i < iNumBucks; i++) {
    pBuckA++;
    iNumGeneInBuckA = pBuckA->isNumGenes;
    if(iNumGeneInBuckA < 1) continue;

    /* For each GeneA : Visit its Active Blue Shocks to build Triplets */
    piGeneInBuckA = pBuckA->piGenes;
    for(j = 0; j < iNumGeneInBuckA; j++) {
      iGeneA = *piGeneInBuckA++;
      pDataA = pInData+iGeneA;
      pGeneA = &(pDataA->Sample);
      dGeneA[0] = (double) pGeneA->fPosX;
      dGeneA[1] = (double) pGeneA->fPosY;
      dGeneA[2] = (double) pGeneA->fPosZ;
      /* To avoid permutations of triples */
      pDataA->isFlagPermute = isFlagPermute;

      /* Go through the list of associated Blue shocks in ascending *\
       * order only, to avoid redundancies. Intersect only Active   *
       * sheets (i.e., found at Full Resolution) with other active  *
      \* and passive ones.              */

      iNumBlueA = pDataA->isNumBlue;
      if(iNumBlueA < 2)
  continue; /* Need at least 2 Blue shocks : Next GeneA: j++ */

      piBlue1 = pDataA->piBlueList;
      for(k = 0; k < iNumBlueA; k++) {
  iBlue1 = *piBlue1++;
  pBlue1 = (pScaf->pBlue)+iBlue1;
  if(pBlue1->isStatus != ACTIVE)
    continue; /* Need to intersect an ACTIVE sheet with others */
#if FALSE
  if(pBlue1->isInitType == BLUE_SUB)
    continue; /* Skip Shocks found at Low Res. */
#endif
  if(!pBlue1->isFlagValid)
    continue;

  if(iGeneA == pBlue1->iGene[0]) /* Must share GeneA */
    iGeneB = pBlue1->iGene[1];
  else
    iGeneB = pBlue1->iGene[0];

  pDataB = pInData+iGeneB;
  if(pDataB->isFlagPermute > isFlagPermuteNot)
    continue; /* Already used */
  pGeneB  = &(pDataB->Sample);
  dGeneB[0] = (double) pGeneB->fPosX;
  dGeneB[1] = (double) pGeneB->fPosY;
  dGeneB[2] = (double) pGeneB->fPosZ;

  piBlue2 = piBlue1;
  iNext = k + 1;
  for(l = iNext; l < iNumBlueA; l++) {

    iBlue2 = *piBlue2++;
    pBlue2 = (pScaf->pBlue)+iBlue2;
#if FALSE
    if(pBlue2->isInitType == BLUE_SUB)
      continue; /* Skip Shocks found at Low Res. */
#endif
    if(!pBlue2->isFlagValid)
      continue;

    /* We may intersect an active shock with a passive one */
    /* if(pBlue2->iStatus != ACTIVE) continue; */
    if(iGeneA == pBlue2->iGene[0]) /* Must share GeneA */
      iGeneC = pBlue2->iGene[1];
    else
      iGeneC = pBlue2->iGene[0];
    
    pDataC = pInData+iGeneC;
    if(pDataC->isFlagPermute > isFlagPermuteNot)
      continue; /* Already used */
    pGeneC = &(pDataC->Sample);
    dGeneC[0] = (double) pGeneC->fPosX;
    dGeneC[1] = (double) pGeneC->fPosY;
    dGeneC[2] = (double) pGeneC->fPosZ;

    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

    /* CircumCenter of 3D triangle is calculated.  *\
    \* Returned coordinates are relative to GeneA  */
    if(!tri_circum_3d(dGeneA,dGeneB,dGeneC,dCircum)) {
      /* Offshoot to infinity */
      iOffShootsToInfinity++;
      continue; /* l++ */
    }

    /* Potential 3D Triangle CircumCenter */
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
      /* pGreen->iStatus = PASSIVE; */
      iOutOfBox++;
      continue; /* We do not keep these: Goto next BlueA : j++ */
    }

    iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
    iLabelBuckShock = *(pisLookUp+iPos);

    pGreen->iBuckXYZ = iLabelBuckShock;

    pShockData->iGene[0] = pGreen->iGene[0] = iGeneA; /* Shared gene */
    pShockData->iGene[1] = pGreen->iGene[1] = iGeneB;
    pShockData->iGene[2] = pGreen->iGene[2] = iGeneC;

    pShockData->fDist2GeneSq = fDSq;
    pShockData->ShockCoord.fPosX = fPosXs;
    pShockData->ShockCoord.fPosY = fPosYs;
    pShockData->ShockCoord.fPosZ = fPosZs;

#if HIGH_MEM
    pGreen->fDist2GeneSq = fDSq;
    pGreen->CircumTri.fPosX = fPosXs;
    pGreen->CircumTri.fPosY = fPosYs;
    pGreen->CircumTri.fPosZ = fPosZs;
#endif
    /* -------------------- VALIDATE --------------------- */

    if(!ValidGreenShockViaVoxels(pShockData, pInData, pGeneCoord,
               pVoxels, pDimAll)) {
      iNumInvalid++;
      continue; /* Part of SS only: Goto next Blue ngb : j++ */
    }

    /* -------- Found a New Valid Curve Shock --------- */

    if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;

    pGreen->isFlagValid = TRUE;
    pGreen->isInitType  = GREEN_FROM_BLUE;

    pGreen->iBlue[0] = iBlue1;
    pGreen->iBlue[1] = iBlue2;
    pGreen->iBlue[2] = IDLE;

    pGreen->isStatus = ACTIVE;

#if FALSE
    /* -- Find the Type of shock curve via Trilinear Coord -- */
    if(GetTriCoordOfCurve(pInData, pGreen, pShockData)) {
      switch(pGreen->isType) {
      case GREEN_I:
        iGreenTypeI++;
        /* Seek 3rd blue shock ? */
        break;
      case GREEN_II: /* One new Blue sheet exits */
        iGreenTypeII++;
        break;
      default:
        fprintf(stderr, "ERROR(%s): Wrong Green shock type = %d\n",
          pcFName, pGreen->isType);
        exit(-3);
        break;
      }
    }
    else {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, iCircum);
      exit(-3);
    }
    /* Evaluate the sign of the Cos of the angle between  *\
    \* AB and AC: use the shock mid-points instead of B,C */
    fTmp = (pBlue1->MidPt.fPosX - pGeneA->fPosX) *
      (pBlue2->MidPt.fPosX - pGeneA->fPosX);
    fTmp += (pBlue1->MidPt.fPosY - pGeneA->fPosY) *
      (pBlue2->MidPt.fPosY - pGeneA->fPosY);
    fTmp += (pBlue1->MidPt.fPosZ - pGeneA->fPosZ) *
      (pBlue2->MidPt.fPosZ - pGeneA->fPosZ);
    /* fTmp += fEpsilon; For robustness near Cos theta = 0 */
    if(fTmp < 0) { /* Obtuse triangle */
      pGreen->iType = GREEN_II;
      iNumB++; } /* 3rd Blue shock unSET: 1 new sheet */
    else 
      pGreen->iType = GREEN_I; /* Seek 3rd blue shock ? */
#endif
    pGreen++;
    iCircum++;

  } /* Goto next Blue shock C viz GeneA : l++ */

      } /* Goto next Blue shock B viz GeneA : k++ */

    } /* Goto next GeneA in Bucket : j++ */

  } /* Goto next Bucket : i++ */

  fprintf(stderr, "\tFound %d new Green shocks.\n", iCircum);
  fprintf(stderr,
    "\tMax distance to Generators amongst Valid shocks = %.2f \n",  
    (float) sqrt((double) fDistSqLarge));
  fprintf(stderr,
    "\tLargest ball radius computed = %.2f\n",
    (float) sqrt((double) fDistSqLargest));

  fprintf(stderr, "\tOffShoots: to infinity = %d ,\n", iOffShootsToInfinity);
  fprintf(stderr, "\t\tOut of Box (but finite dist) = %d .\n",
    iOutOfBox);

  fprintf(stderr, "\tInvalid shocks = %d .\n", iNumInvalid);

  pDimMBuck->iNumShockCurve  = iCircum;
  /* pDimMBuck->iNumShockCurveV = iCircum; */
  /* pDimMBuck->iNumShockGreen2Blue = iGreenTypeII; */
  pDimMBuck->iNumShockGreen2Blue = iCircum;

  return;
}

/*------------------------------------------------------------*\
 * Function: FetchGreenShocksFstLayer
 *
 * Usage: Seek Initial Curve shocks - some are part of the MA
 *    and thus, the source of a shock curve, others are only
 *    part of the SS. Keep only valid shocks (on MA).
 *    Used once Blue sheets have been found for the 1st layer
 *    of buckets around each individual bucket.
 *    This step is similar to FetchGreenShocksInt() except
 *    we need to check for duplicates of Curves.
 *
\*------------------------------------------------------------*/

void
FetchGreenShocksFstLayer(InputData *pInData, ShockScaffold *pScaf,
       ShockCurve *pShockCurves, short *pisLookUp,
       Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
       Voxel *pVoxels, Dimension *pDimMBuck,
       Dimension *pDimAll)
{
   char    *pcFName = "FetchGreenShocksFstLayer";
   register int  i, j, k, l, m;
   short    isFlagPermute, isFlagPermuteNot;
   int    iGeneA,iGeneB,iGeneC, iCircum, iNumInvalid, iNumBucks;
   int    iOffShootsToInfinity, iNumGeneInBuckA, iNumBlueA;
   int    iBlue1,iBlue2, iNext, iPos,iPosXs,iPosYs,iPosZs;
   int    iSliceSize, iLabelBuckShock, iXdim;
   int    iOutOfBox, iNumGreenA, iDuplicCurve, iFlagNewTri;
   int    iGreenA, iNewA,iNewB,iNewC, iSetB,iSetC,iSet;
   int    iXmin,iYmin,iZmin, iXmax,iYmax,iZmax;
   int    iDelta, iNextChunk, iChunk, iPercent;
   int    *piGeneInBuckA, *piBlue1,*piBlue2, *piGreenA;
   double  fDX,fDY,fDZ, fPosXs,fPosYs,fPosZs, fDSq, fDistSqLargest;
   double   fDistSqLarge;
   double  dGeneA[3],dGeneB[3],dGeneC[3],dCircum[3];
   ShockSheet  *pBlue1, *pBlue2;
   ShockCurve  *pGreen, *pGreenA;
   Pt3dCoord  *pGeneA, *pGeneB, *pGeneC;
   Bucket  *pBuckA;
   InputData  *pDataA, *pDataB, *pDataC;
   ShockData  *pShockData;


   pShockData = NULL;
   if((pShockData = (ShockData*)
       calloc(1, sizeof(ShockData))) == NULL) {
     fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
         pcFName);
     exit(-4); }
   pShockData->iGene[3] = IDLE;
   
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
   iOffShootsToInfinity = iNumInvalid = 0;
   fDistSqLarge = fDistSqLargest = 0.0;
   
   iCircum = iOutOfBox = iDuplicCurve = 0;
   isFlagPermute = pDimMBuck->isFlagPermute;
   isFlagPermuteNot = isFlagPermute - 1;
   
   /* ------------ Process BucketsXYZ one by one ----------- *\
   \*      Seek triplets of genes        */
   
   iDelta = (int) (iNumBucks / 4);
   iNextChunk = iDelta;
   iChunk = 0;
   
   pGreen = pShockCurves;
   pBuckA = pBucksXYZ->pBucket-1;
   for (i = 0; i < iNumBucks; i++) {
   
      if(i > iNextChunk) { /* Print a mesg. for each chunk of data */
         iChunk++;
         iPercent = iChunk * 25;
         if(iPercent < 101) {
           fprintf(stderr, "\tProcessed %d percent of buckets ...\n", iPercent);
           iNextChunk += iDelta; }
         }
   
      pBuckA++;
      iNumGeneInBuckA = pBuckA->isNumGenes;
      if(iNumGeneInBuckA < 1) 
         continue;
   
      /* For each GeneA : visit its Active Blue Shocks to build Triplets */
   
      piGeneInBuckA = pBuckA->piGenes;
      for(j = 0; j < iNumGeneInBuckA; j++) {
         iGeneA = *piGeneInBuckA++;
         pDataA = pInData+iGeneA;
         pGeneA = &(pDataA->Sample);
         dGeneA[0] = (double) pGeneA->fPosX;
         dGeneA[1] = (double) pGeneA->fPosY;
         dGeneA[2] = (double) pGeneA->fPosZ;
         /* To avoid permutations of triples */
         pDataA->isFlagPermute = isFlagPermute;
   
         /* Go through the list of associated Blue shocks in ascending *\
          * order only, to avoid redundancies. Intersect only Active   *
          * sheets (i.e., found at Full Resolution) with other active  *
         \* and passive ones.              */
   
         iNumBlueA = pDataA->isNumBlue;
         if(iNumBlueA < 2)
           continue; /* Need at least 2 Blue shocks : Next GeneA: j++ */
   
         piBlue1 = pDataA->piBlueList;
         for(k = 0; k < iNumBlueA; k++) {
   
           iBlue1 = *piBlue1++;
           pBlue1 = (pScaf->pBlue)+iBlue1;
           if(pBlue1->isStatus != ACTIVE)
             continue; /* Need to intersect an ACTIVE sheet with others */
           if(!pBlue1->isFlagValid)
             continue;
         
           if(iGeneA == pBlue1->iGene[0]) /* Must share GeneA */
             iGeneB = pBlue1->iGene[1];
           else
             iGeneB = pBlue1->iGene[0];
         
           pDataB = pInData+iGeneB;
           if(pDataB->isFlagPermute > isFlagPermuteNot)
             continue; /* Already used */
           pGeneB  = &(pDataB->Sample);
           dGeneB[0] = (double) pGeneB->fPosX;
           dGeneB[1] = (double) pGeneB->fPosY;
           dGeneB[2] = (double) pGeneB->fPosZ;
         
           piBlue2 = piBlue1;
           iNext = k + 1;
           for(l = iNext; l < iNumBlueA; l++) {
   
               iBlue2 = *piBlue2++;
               pBlue2 = (pScaf->pBlue)+iBlue2;
               if(!pBlue2->isFlagValid)
                 continue;
               
               /* We may intersect an active shock with a passive one */
               /* if(pBlue2->iStatus != ACTIVE) continue; */
               if(iGeneA == pBlue2->iGene[0]) /* Must share GeneA */
                 iGeneC = pBlue2->iGene[1];
               else
                 iGeneC = pBlue2->iGene[0];
               if(iGeneC == iGeneB) {
                 fprintf(stderr, "ERROR(%s):\n\t iGeneC (%d) == iGeneB\n",
                     pcFName, iGeneC);
                 exit(-4);
               }
               pDataC = pInData+iGeneC;
               if(pDataC->isFlagPermute > isFlagPermuteNot)
                 continue; /* Already used */
               
               /* -- Check for duplicate of existing Green shock -- */
               
               iFlagNewTri = TRUE;
               iNumGreenA = pDataA->isNumGreen; /* Sharing GeneA */
               piGreenA = pDataA->piGreenList;
               for(m = 0; m < iNumGreenA; m++) {
                 iGreenA = *piGreenA++;
                 pGreenA = (pScaf->pGreen)+iGreenA;
                 iNewA = pGreenA->iGene[0];
                 iNewB = pGreenA->iGene[1];
                 iNewC = pGreenA->iGene[2];
                 iSetB = iSetC = FALSE;
                 if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC))
                   iSetB = TRUE;
                 if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
                   iSetC = TRUE;
                 iSet = iSetB + iSetC;
                 if(iSet > 1) { /* Found an existing Green shock */
                   iDuplicCurve++;
                   /* Keep track of Blue sheet label if not already there ? */
                   /* TBD (useful if Green Type I only) */
                   iFlagNewTri = FALSE;
                   break; /* Get out of For(m) loop */
                 }
              } /* Check next Green shock linked to GeneA: m++ */
              if(!iFlagNewTri)
                 continue; /* Not a new triplet: Skip it : l++ */
   
               /* -- OK : New triplet of genes: Potential Green shock -- */
               
               pGeneC = &(pDataC->Sample);
               dGeneC[0] = (double) pGeneC->fPosX;
               dGeneC[1] = (double) pGeneC->fPosY;
               dGeneC[2] = (double) pGeneC->fPosZ;
               
               dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;
               
               /* CircumCenter of 3D triangle is calculated.  *\
               \* Returned coordinates are relative to GeneA  */
               if(!tri_circum_3d(dGeneA,dGeneB,dGeneC,dCircum)) {
                  /* Offshoot to infinity */
                  iOffShootsToInfinity++;
                  continue; /* l++ */
               }
               
               /* Potential 3D Triangle CircumCenter */
               fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
               fDZ = (float) dCircum[2];
               fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
               if(fDSq > fDistSqLargest) 
                  fDistSqLargest = fDSq;
               
               fPosXs = fDX + (float) dGeneA[0];
               fPosYs = fDY + (float) dGeneA[1];
               fPosZs = fDZ + (float) dGeneA[2];
               
               /* -- Validate Shocks located INSIDE the box bounding data -- */
               
               iPosXs = (int) floor((double) fPosXs);
               iPosYs = (int) floor((double) fPosYs);
               iPosZs = (int) floor((double) fPosZs);
               /* -- Is Shock located INSIDE the MetaBucket ? -- */
               if(iPosXs < iXmin || iPosXs > iXmax ||
                  iPosYs < iYmin || iPosYs > iYmax ||
                  iPosZs < iZmin || iPosZs > iZmax) {
                  /* Shock is OUTSIDE the box bounding data */
                  /* pGreen->iStatus = PASSIVE; */
                  iOutOfBox++;
                  continue; /* We do not keep these: Goto next BlueA : j++ */
               }
   
               iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
               iLabelBuckShock = *(pisLookUp+iPos);
               
               pGreen->iBuckXYZ = iLabelBuckShock;
               
               pShockData->iGene[0] = pGreen->iGene[0] = iGeneA; /* Shared gene */
               pShockData->iGene[1] = pGreen->iGene[1] = iGeneB;
               pShockData->iGene[2] = pGreen->iGene[2] = iGeneC;
               
               pShockData->fDist2GeneSq = fDSq;
               pShockData->ShockCoord.fPosX = fPosXs;
               pShockData->ShockCoord.fPosY = fPosYs;
               pShockData->ShockCoord.fPosZ = fPosZs;

#if HIGH_MEM
               pGreen->fDist2GeneSq = fDSq;
               pGreen->CircumTri.fPosX = fPosXs;
               pGreen->CircumTri.fPosY = fPosYs;
               pGreen->CircumTri.fPosZ = fPosZs;
#endif

               /* -------------------- VALIDATE --------------------- */
               
               if(!ValidGreenShockViaVoxels(pShockData, pInData, pGeneCoord,
                            pVoxels, pDimAll)) {
                  iNumInvalid++;
                  continue; /* Part of SS only: Goto next Blue ngb : j++ */
               }
               
               /* -------- Found a New Valid Curve Shock --------- */
               
               if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;
               
               pGreen->isFlagValid = TRUE;
               pGreen->isInitType  = GREEN_FROM_BLUE;
               
               pGreen->iBlue[0] = iBlue1;
               pGreen->iBlue[1] = iBlue2;
               pGreen->iBlue[2] = IDLE;
               
               pGreen++;
               iCircum++;

           } /* Goto next Blue shock C viz GeneA : l++ */

         } /* Goto next Blue shock B viz GeneA : k++ */

      } /* Goto next GeneA in Bucket : j++ */

   } /* Goto next Bucket : i++ */

   fprintf(stderr, "\tFound %d new Green shocks.\n", iCircum);
   fprintf(stderr, "\t\t%d Duplicates found.\n", iDuplicCurve);
   
   fprintf(stderr,
       "\tMax distance to Generators amongst Valid shocks = %.2f \n",  
       (float) sqrt((double) fDistSqLarge));
   fprintf(stderr,
       "\tLargest ball radius computed = %.2f\n",
       (float) sqrt((double) fDistSqLargest));
   
   fprintf(stderr, "\tOffShoots: to infinity = %d ,\n", iOffShootsToInfinity);
   fprintf(stderr, "\t\tOut of Box (but finite dist) = %d .\n",
       iOutOfBox);
   
   fprintf(stderr, "\tInvalid shocks = %d .\n", iNumInvalid);
   
   pDimMBuck->iNumShockCurve  = iCircum;
   pDimMBuck->iNumShockGreen2Blue = iCircum;
   
   return;
}

/*------------------------------------------------------------*\
 * Function: SetValidCurvesInt
 *
 * Usage: Sets a list of valid initial curve shocks on the Scaffold.
 *    Adds new sheets (from Green type II).
 *    Fill-in sheet slot or create missing sheet (type I & III).
 *    Takes data processed via FetchGreenShocksInt()
 *
\*------------------------------------------------------------*/

int
SetValidCurvesInt(ShockCurve *pShocks, ShockScaffold *pScaf,
      InputData *pInData, short *pisLookUp,
      Buckets *pBucksXYZ, Dimension *pDimMBuck)
{
  char    *pcFName = "SetValidCurvesInt";
  register int  i, j;
  int    iNumGreen, iGreen, iBlue, iGeneA,iGeneB,iGeneC, iMax;
  int    iNumProxiA,iNumProxiB,iNumProxiC, iTmp;
  int    iGreenActive,iGreenPassive, iBlueActive,iBluePassive;
  int    iType, iStatus, iSameBlue, iOldBlue, iCircum;
  int    iOldA,iOldB, iNumGreenI,iNumGreenII,iNumGreenIII, iSetC;
  int    iNumBlueSlotSet, iNumBlueMiss, iPosX,iPosY,iPosZ,iPos;
  int    iSliceSize, iXdim, iLabelBuckShock;
  int    *piOldBlue;
  /* static float  fEpsilon = (float) D_EPSILON; */
  float    fTmp, fAlpha,fBeta,fGamma;
  float  fPABx = 0;
  float  fPABy = 0;
  float  fPABz = 0;
  float  fDSq = 0;
  float    fPBCx,fPBCy,fPBCz, fPosX,fPosY,fPosZ, fMaxArea,fMinArea;
  ShockSheet  *pNewBlue, *pOldBlue;
  ShockCurve  *pGreen, *pNewGreen;
  InputData  *pDataA,*pDataB,*pDataC;
  Pt3dCoord  *pGeneB, *pGeneC;
  Vector3d  *pvNorm;
  ShockData  *pShockData;

  /* Preliminaries */

  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }

  fprintf(stderr, "MESG(%s):\n", pcFName);

  iSliceSize = pDimMBuck->iSliceSize;
  iXdim = pDimMBuck->iXdim;

  iNumGreen = pDimMBuck->iNumShockCurve; /* New active and passive shocks */
  iGreen = pScaf->iGreenActiveLast;
  iBlue = pScaf->iBlueActiveLast;
#if FALSE
  fprintf(stderr, "\tUp to %d Green shocks to put on scaffold...\n",
    iNumGreen);
  fprintf(stderr, "\tfrom position %d (tail of Green active list)\n",
    iGreen);
  fprintf(stderr, "\tand %d for sheets (tail of Blue active list).\n",
    iBlue);
#endif

  pGreen = pShocks-1;
  pNewGreen = (pScaf->pGreen)+iGreen;
  pNewBlue = (pScaf->pBlue)+iBlue;

  iGreenActive = iGreenPassive = iBlueActive = iBluePassive = 0;
  iNumGreenI = iNumGreenII = iNumGreenIII = iCircum = 0;
  iSameBlue = iNumBlueSlotSet = iNumBlueMiss = 0;
  fMinArea = 10000000.0;
  fMaxArea = 0.0;

  fprintf(stderr, "\tFor up to %d Green shock curves...\n", iNumGreen);
  for(i = 0; i < iNumGreen; i++) {
    pGreen++;

    iGeneA = pNewGreen->iGene[0] = pGreen->iGene[0];
    iGeneB = pNewGreen->iGene[1] = pGreen->iGene[1];
    iGeneC = pNewGreen->iGene[2] = pGreen->iGene[2];

    pNewGreen->iBuckXYZ = pGreen->iBuckXYZ;

    pNewGreen->iRed[0] = IDLE; /* No end points yet */
    pNewGreen->iRed[1] = IDLE;
    pNewGreen->isNumEndPts = 0;
#if HIGH_MEM
    pNewGreen->CircumTri.fPosX = pGreen->CircumTri.fPosX;
    pNewGreen->CircumTri.fPosY = pGreen->CircumTri.fPosY;
    pNewGreen->CircumTri.fPosZ = pGreen->CircumTri.fPosZ;
    pNewGreen->fDist2GeneSq = pGreen->fDist2GeneSq;
    fAlpha = pNewGreen->fTriCoord[0] = pGreen->fTriCoord[0];
    fBeta  = pNewGreen->fTriCoord[1] = pGreen->fTriCoord[1];
    fGamma = pNewGreen->fTriCoord[2] = pGreen->fTriCoord[2];
    fTmp = pNewGreen->fTriArea = pGreen->fTriArea;
    if(fTmp > fMaxArea) fMaxArea = fTmp;
    if(fTmp < fMinArea) fMinArea = fTmp;
#endif
    pNewGreen->iBlue[0] = pGreen->iBlue[0];
    pNewGreen->iBlue[1] = pGreen->iBlue[1];
    pNewGreen->iBlue[2] = pGreen->iBlue[2];

    /* -- Find the Type of shock curve via Trilinear Coordinates -- */

    if(!GetTriCoordOfCurve(pInData, pGreen, pShockData)) {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, i);
      exit(-3); }

    iType  = pNewGreen->isType = pGreen->isType;
    fAlpha = pShockData->fBaryCoord[0];
    fBeta  = pShockData->fBaryCoord[1];
    fGamma = pShockData->fBaryCoord[2];

    pNewGreen->isInitType = GREEN_FROM_BLUE;
    pNewGreen->isFlagValid = TRUE;
    pNewGreen->isItera = pScaf->isLastItera;
    iStatus = pNewGreen->isStatus = pGreen->isStatus;
    if(iStatus != ACTIVE)
      iGreenPassive++;
    else
      iGreenActive++;

    /* -- Set backpointers to shocks for the 3 genes -- */

    pDataA = pInData+iGeneA;
    iNumProxiA = pDataA->isNumGreen;
    iMax = pDataA->isMaxGreen - 2;
    if(iNumProxiA > iMax) {
      iMax = 1 + iNumProxiA * 2;
      if((pDataA->piGreenList =
    (int *) realloc((int *) pDataA->piGreenList,
        iMax * sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc on pDataA(%d)->piGreenList[%d].\n",
    pcFName, iGeneA, iMax);
  return(FALSE); }
      pDataA->isMaxGreen = iMax; }
    pDataA->piGreenList[iNumProxiA] = iGreen; /* Backpointer to shock */
    pDataA->isNumGreen++;

    pDataB = pInData+iGeneB;
    iNumProxiB = pDataB->isNumGreen;
    iMax = pDataB->isMaxGreen - 2;
    if(iNumProxiB > iMax) {
      iMax = 1 + iNumProxiB * 2;
      if((pDataB->piGreenList =
    (int *) realloc((int *) pDataB->piGreenList,
        iMax * sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc on pDataB(%d)->piGreenList[%d].\n",
    pcFName, iGeneB, iMax);
  return(FALSE); }
      pDataB->isMaxGreen = iMax; }
    pDataB->piGreenList[iNumProxiB] = iGreen; /* Backpointer to shock */
    pDataB->isNumGreen++;

    pDataC = pInData+iGeneC;
    iNumProxiC = pDataC->isNumGreen;
    iMax = pDataC->isMaxGreen - 2;
    if(iNumProxiC > iMax) {
      iMax = 1 + iNumProxiC * 2;
      if((pDataC->piGreenList =
    (int *) realloc((int *) pDataC->piGreenList,
        iMax * sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc on pDataC(%d)->piGreenList[%d].\n",
    pcFName, iGeneC, iMax);
  return(FALSE); }
      pDataC->isMaxGreen = iMax; }
    pDataC->piGreenList[iNumProxiC] = iGreen; /* Backpointer to shock */
    pDataC->isNumGreen++;

    /* -- 3rd : If Green type II : Generate new Blue sheet -- */

    if(iType == GREEN_II) {

      iNumGreenII++;
      if(fAlpha < 0.0)
  fTmp = fAlpha + 0.1;
      else if(fBeta < 0.0)
  fTmp = fBeta + 0.1;
      else if(fGamma < 0.0) {
  fTmp = fGamma + 0.1;
#if FALSE
  fprintf(stderr,
    "MESG(%s): Curve %d is an Obtuse triangle at GeneC\n",
    pcFName, i);
  fprintf(stderr, "\tTrilinears = (%f , %f , %f).\n",
    fAlpha, fBeta, fGamma);
#endif
      }
      else {
  fprintf(stderr, "ERROR(%s): Type II (obtuse triangle)\n", pcFName);
  fprintf(stderr, "\tbut no negative tri.coord.: (%f , %f , %f).\n",
    fAlpha, fBeta, fGamma);
  return(FALSE);
      }

      if(fTmp > 0.0) { /* Right triangle */
  iType = pNewGreen->isType = GREEN_III;
  iNumGreenIII++;
      }
    }
    else { /* If(iType == GREEN_I) */
      iNumGreenI++;
    }


    /* ---- Fill-in empty Blue sheet slot or create new sheet ----- */

    /* We assume GeneA is shared by both Blue Sheets *\
    \* already set via FetchGreenShocksInt()       */

    pDataB = pInData+iGeneB;
    iNumProxiB = pDataB->isNumBlue;
    iSetC = FALSE; /* Find GeneC */
    piOldBlue = pDataB->piBlueList;
    for(j = 0; j < iNumProxiB; j++) {
      iOldBlue = *piOldBlue++;
      pOldBlue = (pScaf->pBlue)+iOldBlue;
      iOldA = pOldBlue->iGene[0];
      iOldB = pOldBlue->iGene[1];
      if((iGeneC == iOldA) || (iGeneC == iOldB)) {
  iSetC = TRUE;
  break;
      }
    } /* Next Blue Proxi of GeneB: j++ */

    if(iSetC) {
      if(iOldBlue == pNewGreen->iBlue[0] ||
   iOldBlue == pNewGreen->iBlue[1]) {
  fprintf(stderr, "ERROR(%s): New Green curve %d already has Blue\n",
    pcFName, iGreen);
  fprintf(stderr,
    "\tSheets %d and %d set, but found no. %d for 3rd slot.\n",
    pNewGreen->iBlue[0], pNewGreen->iBlue[1], iOldBlue);
  exit(-4);
      }
      pNewGreen->iBlue[2] = iOldBlue;
      iNumBlueSlotSet++;
    }
    else { /* This sheet does not exist: Create it */

      iNumBlueMiss++;

      pNewGreen->iBlue[2] = iBlue; /* Set 3rd sheet */
      
      pNewBlue->isFlagValid = TRUE;

      /* Initial shock source of sheet correspond to midpoint of BC */
      pDataB = pInData+iGeneB;
      pDataC = pInData+iGeneC;
      pGeneB = &(pDataB->Sample);
      pGeneC = &(pDataC->Sample);
      fPBCx = pGeneC->fPosX - pGeneB->fPosX;
      fPBCy = pGeneC->fPosY - pGeneB->fPosY;
      fPBCz = pGeneC->fPosZ - pGeneB->fPosZ;
      fDSq = (fPBCx*fPBCx + fPBCy*fPBCy + fPBCz*fPBCz);

      fPosX = (pGeneC->fPosX + pGeneB->fPosX) / 2.0;
      fPosY = (pGeneC->fPosY + pGeneB->fPosY) / 2.0;
      fPosZ = (pGeneC->fPosZ + pGeneB->fPosZ) / 2.0;
#if HIGH_MEM
      pNewBlue->fDist2GeneSq = fDSq / 4.0;
      pNewBlue->MidPt.fPosX = fPosX;
      pNewBlue->MidPt.fPosY = fPosY;
      pNewBlue->MidPt.fPosZ = fPosZ;
#endif
      /* Find Bucket where shock is located */
      iPosX = (int) floor((double) fPosX);
      iPosY = (int) floor((double) fPosY);
      iPosZ = (int) floor((double) fPosZ);
      iPos = iPosZ * iSliceSize + iPosY * iXdim + iPosX;
      iLabelBuckShock = *(pisLookUp+iPos);
      pNewBlue->iBuckXYZ = iLabelBuckShock;

      pNewBlue->iGene[0] = iGeneB;
      pNewBlue->iGene[1] = iGeneC;

      if(iType == GREEN_II) {
  pNewBlue->isInitType = BLUE_FROM_GREENII;
      }
      else { /* iType == GREEN_I || iType == GREEN_III */
  pNewBlue->isInitType = BLUE_MISS;
      }

      pNewBlue->iGreen   = iGreen;
      pNewBlue->iRed     = IDLE;
      pNewBlue->isItera  = pScaf->isLastItera;
      pNewBlue->isStatus = iStatus;

      if(iStatus != ACTIVE)
  iBluePassive++;
      else
  iBlueActive++;

      /* Set Gene Data: Backpointers to shock and Normal data */

      iNumProxiB = pDataB->isNumBlue;
      iMax = pDataB->isMaxBlue - 2;
      if(iNumProxiB > iMax) {
  iMax = 1 + iNumProxiB * 2;
  if((pDataB->piBlueList =
      (int *) realloc((int *) pDataB->piBlueList,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pDataB+%d)->piBlueList[%d] fails.\n",
      pcFName, iGeneB, iMax);
    return(FALSE); }
  if((pDataB->pvNormal =
      (Vector3d *) realloc((Vector3d *) pDataB->pvNormal,
         iMax * sizeof(Vector3d))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pDataB+%d)->pvNormal[%d] fails.\n",
      pcFName, iGeneB, iMax);
    return(FALSE); }
  pDataB->isMaxBlue = iMax;
  /* pDataB->isMaxNorm = iMax; */
      }
      pDataB->piBlueList[iNumProxiB] = iBlue; /* Backpointer to shock */
      pDataB->isNumBlue++;

      pvNorm = pDataB->pvNormal+iNumProxiB;
      pvNorm->fX = fPBCx;
      pvNorm->fY = fPBCy;
      pvNorm->fZ = fPBCz;
      pvNorm->fLengthSq = fDSq;
      /* pDataB->iNumNorm++; */

      iNumProxiC = pDataC->isNumBlue;
      iMax = pDataC->isMaxBlue - 2;
      if(iNumProxiC > iMax) {
  iMax = 1 + iNumProxiC * 2;
  if((pDataC->piBlueList =
      (int *) realloc((int *) pDataC->piBlueList,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pDataC+%d)->piBlueList[%d] fails.\n",
      pcFName, iGeneC, iMax);
    return(FALSE); }
  if((pDataC->pvNormal =
      (Vector3d *) realloc((Vector3d *) pDataC->pvNormal,
         iMax * sizeof(Vector3d))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pDataC+%d)->pvNormal[%d] fails.\n",
      pcFName, iGeneC, iMax);
    return(FALSE); }
  pDataC->isMaxBlue = iMax;
  /* pDataC->isMaxNorm = iMax; */
      }
      pDataC->piBlueList[iNumProxiC] = iBlue; /* Backpointer to shock */
      pDataC->isNumBlue++;

      pvNorm = pDataC->pvNormal+iNumProxiC;
      pvNorm->fX = -fPABx;
      pvNorm->fY = -fPABy;
      pvNorm->fZ = -fPABz;
      pvNorm->fLengthSq = fDSq;
      /* pDataC->isNumNorm++; */
      
      pNewBlue++;
      iBlue++; /* Next New Valid Blue shock */

    } /* End of creation of missing sheet */

    pNewGreen++;
    iCircum++;
    iGreen++; /* Next Valid Green shock */
  }

  fprintf(stderr, "\tPut %d Shock Curves on Scaffold.\n", iCircum);
  iNumGreenII -= iNumGreenIII;
  fprintf(stderr,
    "\t\tTypes: I = %d , II (obtuse) = %d , III (right) = %d .\n", 
    iNumGreenI, iNumGreenII, iNumGreenIII);
  if((iNumGreenI+iNumGreenII+iNumGreenIII) != iCircum) {
    fprintf(stderr, "\tWARNING: iNumGreen Types I+II+III != iCircum.\n");
  }
  pScaf->iNumTriAcute  = iNumGreenI;
  pScaf->iNumTriObtuse = iNumGreenII;
  pScaf->iNumTriRight  = iNumGreenIII;

  fprintf(stderr, "\t\tArea: Min = %f , Max = %f\n", fMinArea, fMaxArea);
  pScaf->fTriAreaMin = fMinArea;
  pScaf->fTriAreaMax = fMaxArea;

  iTmp = iBlue - pScaf->iBlueActiveLast;
  fprintf(stderr,
    "\tNew Shock Sheets (from type II and III curves) = %d.\n", iTmp);
  fprintf(stderr, "\t\t%d are active, %d are passive.\n", iBlueActive,
    iBluePassive);
  pScaf->iNumBlueFromGreen += iTmp;
  /* NB: We consider these all Active, because we use a single */
  /*     ordered list on the scaffold to keep track of sheets  */
  pScaf->iBlueActiveLast = iBlue;
  pScaf->iNumBlueActive += iTmp;
  pScaf->iNumBlueInt += iTmp;
  pDimMBuck->iNumShockGreen2Blue = iTmp;
  pDimMBuck->iNumShockSheet = pDimMBuck->iNumShockSheetIntAll = iBlue;
  if(iTmp != iNumBlueMiss) {
    fprintf(stderr, "WARNING: Missed Blue Sheets = %d != New sheets = %d\n",
      iNumBlueMiss, iTmp);
  }
  if(iSameBlue > 0) {
    fprintf(stderr,
      "\t\tDuplicates of sheets (due to Right Triangles) = %d.\n",
      iSameBlue);
  }
  fprintf(stderr, "\t\tNumber of 3rd Blue Slots of Curve set = %d\n",
    iNumBlueSlotSet);

  iTmp = iGreen - pScaf->iGreenActiveLast;
  fprintf(stderr, "\tNew Shock Curves (from Active Sheets) = %d.\n", iTmp);
  pScaf->iNumGreenFromSheets += iTmp;
  pScaf->iNumGreenNoEnd += iTmp;
  fprintf(stderr, "\t\t%d are active, %d are passive.\n", iGreenActive,
    iGreenPassive);
  /* NB: We consider these all Active, because we use a single */
  /*     ordered list on the scaffold to keep track of sheets  */
  pScaf->iGreenActiveLast = iGreen;
  pScaf->iNumGreenActive += iTmp;

  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: SetValidCurvesFstLayer
 *
 * Usage: Sets a list of valid curve shocks on the scaffold.
 *    Adds new sheets too (from Green type II).
 *    Fill-in sheet slot or create missing sheet (type I & III).
 *    Takes data processed via FetchGreenShocksInt()
 *
 *    NB: We don't need to check if a new curve is already on
 *    the list in this initial step (all triplets are unique).
 *    But we need to check new sheet candidates w/r to
 *    existing ones.
 *
\*------------------------------------------------------------*/

int
SetValidCurvesFstLayer(ShockCurve *pShocks, ShockScaffold *pScaf,
           InputData *pInData, short *pisLookUp,
           Buckets *pBucksXYZ, Dimension *pDimMBuck)
{
  char    *pcFName = "SetValidCurvesFstLayer";
  register int  i, j;
  int    iNumGreen, iGreen, iBlue, iGeneA,iGeneB,iGeneC;
  int    iNumProxiA,iNumProxiB,iNumProxiC, iTmp, iSetC;
  int    iType, iOldBlue, iCircum, iMax,  iOldA,iOldB;
  int    iNumGreenI,iNumGreenII,iNumGreenIII, iNumBlueMiss;
  int    iNumBlueSlotSet, iPosX,iPosY,iPosZ,iPos;
  int    iLabelBuckShock, iSliceSize, iXdim;
  int    *piOldBlue;
  /* static float  fEpsilon = (float) D_EPSILON; */
  float    fTmp, fAlpha,fBeta,fGamma;

  //Ming: init fPABx, fPABy, fPABz to 0
  float    fPABx = 0,fPABy = 0,fPABz = 0, fDSq;


  float    fPBCx,fPBCy,fPBCz, fPosX,fPosY,fPosZ;
  ShockSheet  *pNewBlue, *pOldBlue;
  ShockCurve  *pGreen, *pNewGreen;
  InputData  *pDataA,*pDataB,*pDataC;
  Pt3dCoord  *pGeneB, *pGeneC;
  Vector3d  *pvNorm;
  ShockData  *pShockData;

  /* Preliminaries */
  
  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }

  fprintf(stderr, "MESG(%s):\n", pcFName);

  iSliceSize = pDimMBuck->iSliceSize;
  iXdim = pDimMBuck->iXdim;

  iNumGreen = pDimMBuck->iNumShockCurve; /* New active and passive shocks */
  iGreen = pScaf->iGreenActiveLast;
  iBlue = pScaf->iBlueActiveLast;
#if FALSE
  fprintf(stderr, "\tUp to %d Green shocks to put on scaffold...\n",
    iNumGreen);
  fprintf(stderr, "\t\tTails: %d (Green), %d (Blue)\n", iGreen, iBlue);
#endif
  pGreen = pShocks-1;
  pNewGreen = (pScaf->pGreen)+iGreen;
  pNewBlue = (pScaf->pBlue)+iBlue;

  iNumGreenI = iNumGreenII = iNumGreenIII = iCircum = 0;
  iNumBlueSlotSet = iNumBlueMiss = 0;
  fprintf(stderr, "\tFor up to %d Green shock curves...\n", iNumGreen);
  for(i = 0; i < iNumGreen; i++) {
    pGreen++;

    iGeneA = pNewGreen->iGene[0] = pGreen->iGene[0];
    iGeneB = pNewGreen->iGene[1] = pGreen->iGene[1];
    iGeneC = pNewGreen->iGene[2] = pGreen->iGene[2];

    pNewGreen->iBuckXYZ = pGreen->iBuckXYZ;

    pNewGreen->iRed[0] = IDLE; /* No end points yet */
    pNewGreen->iRed[1] = IDLE;
    pNewGreen->isNumEndPts = 0;
#if HIGH_MEM
    pNewGreen->CircumTri.fPosX = pGreen->CircumTri.fPosX;
    pNewGreen->CircumTri.fPosY = pGreen->CircumTri.fPosY;
    pNewGreen->CircumTri.fPosZ = pGreen->CircumTri.fPosZ;
    pNewGreen->fDist2GeneSq = pGreen->fDist2GeneSq;
    fAlpha = pNewGreen->fTriCoord[0] = pGreen->fTriCoord[0];
    fBeta  = pNewGreen->fTriCoord[1] = pGreen->fTriCoord[1];
    fGamma = pNewGreen->fTriCoord[2] = pGreen->fTriCoord[2];
    pNewGreen->fTriArea = pGreen->fTriArea;
#endif
    pNewGreen->iBlue[0] = pGreen->iBlue[0];
    pNewGreen->iBlue[1] = pGreen->iBlue[1];
    pNewGreen->iBlue[2] = pGreen->iBlue[2];

    /* -- Find the Type of shock curve via Trilinear Coordinates -- */

    if(!GetTriCoordOfCurve(pInData, pGreen, pShockData)) {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, i);
      exit(-3); }

    iType  = pNewGreen->isType = pGreen->isType;
    fAlpha = pShockData->fBaryCoord[0];
    fBeta  = pShockData->fBaryCoord[1];
    fGamma = pShockData->fBaryCoord[2];

    pNewGreen->isInitType = GREEN_FROM_BLUE;
    pNewGreen->isFlagValid = TRUE;
    pNewGreen->isItera = pScaf->isLastItera;
    pNewGreen->isStatus = ACTIVE;

    /* -- Set backpointers to shocks for the 3 genes -- */

    pDataA = pInData+iGeneA;
    iNumProxiA = pDataA->isNumGreen;
    iMax = pDataA->isMaxGreen - 2;
    if(iNumProxiA > iMax) {
      iMax = 1 + iNumProxiA * 2;
      if((pDataA->piGreenList =
    (int *) realloc((int *) pDataA->piGreenList,
        iMax * sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc on pDataA(%d)->piGreenList[%d].\n",
    pcFName, iGeneA, iMax);
  return(FALSE); }
      pDataA->isMaxGreen = iMax; }
    pDataA->piGreenList[iNumProxiA] = iGreen; /* Backpointer to shock */
    pDataA->isNumGreen++;

    pDataB = pInData+iGeneB;
    iNumProxiB = pDataB->isNumGreen;
    iMax = pDataB->isMaxGreen - 2;
    if(iNumProxiB > iMax) {
      iMax = 1 + iNumProxiB * 2;
      if((pDataB->piGreenList =
    (int *) realloc((int *) pDataB->piGreenList,
        iMax * sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc on pDataB(%d)->piGreenList[%d].\n",
    pcFName, iGeneB, iMax);
  return(FALSE); }
      pDataB->isMaxGreen = iMax; }
    pDataB->piGreenList[iNumProxiB] = iGreen; /* Backpointer to shock */
    pDataB->isNumGreen++;

    pDataC = pInData+iGeneC;
    iNumProxiC = pDataC->isNumGreen;
    iMax = pDataC->isMaxGreen - 2;
    if(iNumProxiC > iMax) {
      iMax = 1 + iNumProxiC * 2;
      if((pDataC->piGreenList =
    (int *) realloc((int *) pDataC->piGreenList,
        iMax * sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): ReAlloc on pDataC(%d)->piGreenList[%d].\n",
    pcFName, iGeneC, iMax);
  return(FALSE); }
      pDataC->isMaxGreen = iMax; }
    pDataC->piGreenList[iNumProxiC] = iGreen; /* Backpointer to shock */
    pDataC->isNumGreen++;

    /* -- 3rd : If Green type II : Generate new Blue sheet -- */

    if(iType == GREEN_II) {
      iNumGreenII++;
      if(fAlpha < 0.0)
  fTmp = fAlpha + 0.1;
      else if(fBeta < 0.0)
  fTmp = fBeta + 0.1;
      else if(fGamma < 0.0) {
  fTmp = fGamma + 0.1;
#if FALSE
  fprintf(stderr,
    "MESG(%s): Curve %d is an Obtuse triangle at GeneC\n",
    pcFName, i);
  fprintf(stderr, "\tTrilinears = (%f , %f , %f).\n",
    fAlpha, fBeta, fGamma);
#endif
      }
      else {
  fprintf(stderr, "ERROR(%s): Type II (obtuse triangle)\n", pcFName);
  fprintf(stderr, "\tbut no negative tri.coord.: (%f , %f , %f).\n",
    fAlpha, fBeta, fGamma);
  return(FALSE);
      }
      if(fTmp > 0.0) { /* Right triangle */
  iType = pNewGreen->isType = GREEN_III;
  iNumGreenIII++;
      }
    }
    else { /* If(iType == GREEN_I) */
      iNumGreenI++;
    }

    /* ---- Fill-in empty Blue sheet slot or create new sheet ----- */

    /* We assume GeneA is shared by both Blue Sheets *\
    \* already set via FetchGreenShocksFstLayer()    */

    pDataB = pInData+iGeneB;
    iNumProxiB = pDataB->isNumBlue;
    iSetC = FALSE; /* Find GeneC */
    piOldBlue = pDataB->piBlueList;
    for(j = 0; j < iNumProxiB; j++) {
      iOldBlue = *piOldBlue++;
      pOldBlue = (pScaf->pBlue)+iOldBlue;
      iOldA = pOldBlue->iGene[0];
      iOldB = pOldBlue->iGene[1];
      if((iGeneC == iOldA) || (iGeneC == iOldB)) {
  iSetC = TRUE;
  break;
      }
    } /* Next Blue Proxi of GeneB: j++ */

    if(iSetC) {
      if(iOldBlue == pNewGreen->iBlue[0] ||
   iOldBlue == pNewGreen->iBlue[1]) {
  fprintf(stderr, "ERROR(%s): New Green curve %d already has Blue\n",
    pcFName, iGreen);
  fprintf(stderr,
    "\tSheets %d and %d set, but found no. %d for 3rd slot.\n",
    pNewGreen->iBlue[0], pNewGreen->iBlue[1], iOldBlue);
  exit(-4);
      }
      pNewGreen->iBlue[2] = iOldBlue;
      iNumBlueSlotSet++;
    }
    else { /* This sheet does not exist: Create it */

      iNumBlueMiss++;

      pNewGreen->iBlue[2] = iBlue; /* Set 3rd sheet */

      pNewBlue->isFlagValid = TRUE;

      /* Initial shock source of sheet correspond to midpoint of BC */
      pDataB = pInData+iGeneB;
      pDataC = pInData+iGeneC;
      pGeneB = &(pDataB->Sample);
      pGeneC = &(pDataC->Sample);
      fPBCx = pGeneC->fPosX - pGeneB->fPosX;
      fPBCy = pGeneC->fPosY - pGeneB->fPosY;
      fPBCz = pGeneC->fPosZ - pGeneB->fPosZ;
      fDSq = (fPBCx*fPBCx + fPBCy*fPBCy + fPBCz*fPBCz);

      fPosX = (pGeneC->fPosX + pGeneB->fPosX) / 2.0;
      fPosY = (pGeneC->fPosY + pGeneB->fPosY) / 2.0;
      fPosZ = (pGeneC->fPosZ + pGeneB->fPosZ) / 2.0;
#if HIGH_MEM
      pNewBlue->fDist2GeneSq = fDSq / 4.0;
      pNewBlue->MidPt.fPosX = fPosX;
      pNewBlue->MidPt.fPosY = fPosY;
      pNewBlue->MidPt.fPosZ = fPosZ;
#endif
      /* Find Bucket where shock is located */
      iPosX = (int) floor((double) fPosX);
      iPosY = (int) floor((double) fPosY);
      iPosZ = (int) floor((double) fPosZ);
      iPos = iPosZ * iSliceSize + iPosY * iXdim + iPosX;
      iLabelBuckShock = *(pisLookUp+iPos);
      pNewBlue->iBuckXYZ = iLabelBuckShock;

      pNewBlue->iGene[0] = iGeneB;
      pNewBlue->iGene[1] = iGeneC;

      if(iType == GREEN_II) {
  pNewBlue->isInitType = BLUE_FROM_GREENII;
      }
      else { /* iType == GREEN_I || iType == GREEN_III */
  pNewBlue->isInitType = BLUE_MISS;
      }

      pNewBlue->iGreen   = iGreen;
      pNewBlue->iRed     = IDLE;
      pNewBlue->isItera  = pScaf->isLastItera;
      pNewBlue->isStatus = ACTIVE;

      /* Set Gene Data: Backpointers to shock and Normal data */

      iNumProxiB = pDataB->isNumBlue;
      iMax = pDataB->isMaxBlue - 2;
      if(iNumProxiB > iMax) {
  iMax = 1 + iNumProxiB * 2;
  if((pDataB->piBlueList =
      (int *) realloc((int *) pDataB->piBlueList,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pDataB+%d)->piBlueList[%d] fails.\n",
      pcFName, iGeneB, iMax);
    return(FALSE); }
  if((pDataB->pvNormal =
      (Vector3d *) realloc((Vector3d *) pDataB->pvNormal,
         iMax * sizeof(Vector3d))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pDataB+%d)->pvNormal[%d] fails.\n",
      pcFName, iGeneB, iMax);
    return(FALSE); }
  pDataB->isMaxBlue = iMax;
  /* pDataB->isMaxNorm = iMax; */
      }
      pDataB->piBlueList[iNumProxiB] = iBlue; /* Backpointer to shock */
      pDataB->isNumBlue++;

      pvNorm = pDataB->pvNormal+iNumProxiB;
      pvNorm->fX = fPBCx;
      pvNorm->fY = fPBCy;
      pvNorm->fZ = fPBCz;
      pvNorm->fLengthSq = fDSq;
      /* pDataB->isNumNorm++; */

      iNumProxiC = pDataC->isNumBlue;
      iMax = pDataC->isMaxBlue - 2;
      if(iNumProxiC > iMax) {
  iMax = 1 + iNumProxiC * 2;
  if((pDataC->piBlueList =
      (int *) realloc((int *) pDataC->piBlueList,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pDataC+%d)->piBlueList[%d] fails.\n",
      pcFName, iGeneC, iMax);
    return(FALSE); }
  if((pDataC->pvNormal =
      (Vector3d *) realloc((Vector3d *) pDataC->pvNormal,
         iMax * sizeof(Vector3d))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc(pDataC+%d)->pvNormal[%d] fails.\n",
      pcFName, iGeneC, iMax);
    return(FALSE); }
  pDataC->isMaxBlue = iMax;
  /* pDataC->isMaxNorm = iMax; */
      }
      pDataC->piBlueList[iNumProxiC] = iBlue; /* Backpointer to shock */
      pDataC->isNumBlue++;

      pvNorm = pDataC->pvNormal+iNumProxiC;
      pvNorm->fX = -fPABx;
      pvNorm->fY = -fPABy;
      pvNorm->fZ = -fPABz;
      pvNorm->fLengthSq = fDSq;
      /* pDataC->isNumNorm++; */

      pNewBlue++;
      iBlue++; /* Next New Valid Blue shock */

    } /* End of creation of missing sheet */

    pNewGreen++;
    iCircum++;
    iGreen++; /* Next Valid Green shock */
  }

  fprintf(stderr, "\tPut %d Shock Curves on Scaffold.\n", iCircum);
  iNumGreenII -= iNumGreenIII;
  fprintf(stderr,
    "\t\tTypes: I = %d , II (obtuse) = %d , III (right) = %d .\n", 
    iNumGreenI, iNumGreenII, iNumGreenIII);
  if((iNumGreenI+iNumGreenII+iNumGreenIII) != iCircum) {
    fprintf(stderr, "\tWARNING: iNumGreen Types I+II+III != iCircum.\n");
  }

  iTmp = iBlue - pScaf->iBlueActiveLast;
  fprintf(stderr, "\tNew Shock sheets = %d.\n", iTmp);
  /* NB: We consider these all Active, because we use a single */
  /*     ordered list on the scaffold to keep track of sheets  */
  pScaf->iBlueActiveLast = iBlue;
  pScaf->iNumBlueActive += iTmp;
  pDimMBuck->iNumShockGreen2Blue = iTmp;

  fprintf(stderr, "\t\tNumber of 3rd Blue Slots of Curve set = %d\n",
    iNumBlueSlotSet);
  fprintf(stderr, "\t\tNumber of Missing Blue Sheets created = %d\n",
    iNumBlueMiss);

  iTmp = iGreen - pScaf->iGreenActiveLast;
  /* NB: We consider these all Active, because we use a single *\
  \*     ordered list on the scaffold to keep track of curves  */
  pScaf->iGreenActiveLast = iGreen;
  pScaf->iNumGreenActive += iTmp;

  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: SetGreenPassive
 *
 * Usage: Sets active Green Shocks which have been used to find
 *    new vertices to be PASSIVE. 
 *    This is useful to ensure curves are not overused in
 *    intersections.
 *
\*------------------------------------------------------------*/

void
SetGreenPassive(ShockScaffold *pScaf)
{
  char    *pcFName = "SetGreenPassive";
  register int  i;
  int    iGreen, iGreenEnd, iNumGreen, iNumActive, iPassive;
  int    iStatus, iActive;
  ShockCurve  *pGreen;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  iGreen = pScaf->iGreenPassiveLast; /* Start on Scaffold list */
  pGreen = (pScaf->pGreen)+iGreen-1;
  iGreenEnd = pScaf->iGreenActiveLast;
  iNumGreen = iGreenEnd - iGreen;
  iNumActive = pScaf->iNumGreenActive;
  if(iNumGreen != iNumActive) {
    fprintf(stderr, "WARNING: Expects to set %d Green shocks on scaffold\n",
      iNumGreen);
    fprintf(stderr, "\tto be PASSIVE, but there are only %d ACTIVE ones.\n",
      iNumActive);
    fprintf(stderr, "\tStarting from position %d on Scaffold->pGreen\n",
      iGreen);
  }
  else {
    fprintf(stderr, "\tSetting %d Green shocks on scaffold to be PASSIVE.\n",
      iNumGreen);
    fprintf(stderr, "\tStarting from position %d on Scaffold->pGreen\n",
      iGreen);
  }

  iActive = iPassive = 0;
  for(i = 0; i < iNumGreen; i++) {
    pGreen++;
    iStatus = pGreen->isStatus;
    if(iStatus != ACTIVE)
      iPassive++;
    else {
      iActive++;
      pGreen->isStatus = PASSIVE;
    }
  }

  if(iActive != iNumActive) {
    fprintf(stderr,
      "WARNING(%s): Set %d Active Green shocks to be passive,\n",
      pcFName, iActive);
    fprintf(stderr, "\tbut expected to set %d ...\n", iNumActive);
  }
  if(iPassive > 0) {    
    fprintf(stderr, "\tSet %d Green curves to PASSIVE status,\n",
      iNumGreen);
    fprintf(stderr, "\t but %d were already not ACTIVE.\n", iPassive);
  }

  /* pScaf->iNumGreenPassive += (iNumGreen - iPassive); */
  pScaf->iNumGreenPassive  = iGreenEnd + 1;
  pScaf->iGreenPassiveLast = iGreenEnd;
  pScaf->iNumGreenActive   = 0;

  return;
}


/***** E0F *****/
