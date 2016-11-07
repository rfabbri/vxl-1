/*------------------------------------------------------------*\
 * File: fetch_green_shocks_v2.c
 *
 * Functions:
 *  FetchGreenViaBlueFull
 *  FetchGreenViaBlue
 *  SetValidCurvesExt
 *
 * History:
 *  #0: Nov. 2001, by F.Leymarie (adapted from old version)
 *  #1: June 2002: Fills-in 3rd Blue slot of Green curves
 *    and create new (missing) sheets if needed.
 *  #2: Aug. 2003: Updated for use with MetaBuckets.
 *
\*------------------------------------------------------------*/

#include "fetch_green_shocks_v2.h"
#include "circum.h"
#include "valid_shocks.h"
#include "barycentric.h"

/*------------------------------------------------------------*\
 * Function: FetchGreenViaBlueFull
 *
 * Usage: Seek all Initial Curve shocks - some are part of the MA
 *    and thus, the source of a shock curve, others are only
 *    part of the SS. Keep only valid shocks (on MA).
 *    This is to be used once all Isolated Blue shock have
 *    been found. Rather than visiting generators to initiate
 *    curve flow (via sheet intercepts), we visit directly
 *    the sheets already on the scaffold.
 *    We intersect ALL sheets here, passive and active.
 *    We need to check for duplicates of Curves and Sheets.
 *
\*------------------------------------------------------------*/

void
FetchGreenViaBlueFull(InputData *pInData, ShockScaffold *pScaf,
          ShockCurve *pShockCurves,
          short *pisLookUp, Buckets *pBucksXYZ,
          Pt3dCoord *pGeneCoord, Voxel *pVoxels,
          Dimension *pDimMBuck, Dimension *pDimAll)
{
  char    *pcFName = "FetchGreenViaBlueFull";
  register int  i, j, k;
  int    iXdim, iXmin,iYmin,iZmin, iXmax,iYmax,iZmax;
  int    iSliceSize, iOffShootsToInfinity, iNumInvalid, iPos;
  int    iFlagPermute,iFlagPermuteNot, iNumSheetV, iOutOfBox;
  int    iBlue, iCircum, iGeneA,iGeneB,iGeneC, iDuplicCurve;
  int    iNumBlueA,iNumBlueB, iBlueA,iBlueB, iPosXs,iPosYs,iPosZs;
  int    iLabelBuckShock, iNumGreenA,iNumGreenB, iGreenA,iGreenB;
  int    iFlagNewTri, iNewA,iNewB,iNewC, iSetA,iSetB,iSetC,iSet;
  int    *piBlueA,*piBlueB, *piGreenA,*piGreenB;
  float    fDistSqLarge,fDistSqLargest, fDSq;
  float    fDX,fDY,fDZ, fPosXs,fPosYs,fPosZs;
  double  dGeneA[3],dGeneB[3],dGeneC[3],dCircum[3];
  ShockSheet  *pBlue, *pBlueA, *pBlueB;
  ShockCurve  *pGreen, *pGreenA, *pGreenB;
  InputData  *pDataA, *pDataB, *pDataC;
  Pt3dCoord  *pGeneA, *pGeneB, *pGeneC;
  ShockData  *pShockData;

  /* Preliminaries */

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
  iOffShootsToInfinity = iNumInvalid = 0;
  fDistSqLarge = fDistSqLargest = 0.0;

  iNumSheetV = pScaf->iBlueActiveLast;
  /* Pointer to beginning of Active list */
  /* iBlue = pScaf->iBluePassiveLast - 1; */
  iBlue  = -1;
  pBlue  = (pScaf->pBlue)+iBlue;
  pGreen = pShockCurves;

  iCircum = iOutOfBox = iDuplicCurve = 0;
  iFlagPermute = pDimMBuck->isFlagPermute;
  iFlagPermuteNot = iFlagPermute - 1;

  /* ---- Process All Existing Blue Sheets one by one ----- *\
  \*      Seek triplets of generators        */

  for(i = 0; i < iNumSheetV; i++) { /* For each Existing Blue shock */

    pBlue++;
    iBlue++;
    /* To avoid permutations of triples : Put on the Sheets directly */
    /* pDataA->iFlag = iFlagPermute; */
    pBlue->isFlagPermute = iFlagPermute;
    if(!pBlue->isFlagValid)
      continue;

    iGeneA = pBlue->iGene[0];
    iGeneB = pBlue->iGene[1];

    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    dGeneA[0] = (double) pGeneA->fPosX;
    dGeneA[1] = (double) pGeneA->fPosY;
    dGeneA[2] = (double) pGeneA->fPosZ;
    iNumBlueA = pDataA->isNumBlue;
    piBlueA   = pDataA->piBlueList;

    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    dGeneB[0] = (double) pGeneB->fPosX;
    dGeneB[1] = (double) pGeneB->fPosY;
    dGeneB[2] = (double) pGeneB->fPosZ;
    iNumBlueB = pDataB->isNumBlue;
    piBlueB   = pDataB->piBlueList;
    
    /* ---- 1st: Intersect with other Blue shocks linked to GeneA ---- */

    for(j = 0; j < iNumBlueA; j++) {

      iBlueA = *piBlueA++;
      if(iBlueA == iBlue)
  continue; /* Same as Blue shock under consideration: Skip it */
      pBlueA = (pScaf->pBlue)+iBlueA;
      if(pBlueA->isFlagPermute > iFlagPermuteNot)
  continue; /* Already processed for intercepts: Avoid duplicate */
#if FALSE
      if(pBlueA->isInitType == BLUE_SUB)
  continue; /* Skip Shocks found at Low Res. */
#endif
      if(!pBlueA->isFlagValid)
  continue;

      if(iGeneA == pBlueA->iGene[0]) /* Must share GeneA */
  iGeneC = pBlueA->iGene[1];
      else
  iGeneC = pBlueA->iGene[0];
      if(iGeneC == iGeneB) {
  fprintf(stderr, "ERROR(%s):\n\t iGeneC (%d) == iGeneB\n",
    pcFName, iGeneC);
  exit(-4);
      }

      /* ---- Check for Duplicate of existing Green shock ---- */

      iFlagNewTri = TRUE;
      iNumGreenA = pDataA->isNumGreen; /* Sharing GeneA */
      piGreenA = pDataA->piGreenList;
      for(k = 0; k < iNumGreenA; k++) {
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
    break; /* Get out of For(k) loop */
  }
      } /* Check next Green shock linked to GeneA: k++ */

      if(!iFlagNewTri)
  continue; /* Not a new triplet: Skip it : j++ */

      /* -- OK : New triplet of genes: Potential Green shock -- */

      pDataC = pInData+iGeneC;
      pGeneC = &(pDataC->Sample);
      dGeneC[0] = (double) pGeneC->fPosX;
      dGeneC[1] = (double) pGeneC->fPosY;
      dGeneC[2] = (double) pGeneC->fPosZ;

      dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

      /* CircumCenter of 3D triangle is calculated. *\
      \* Returned coordinates are relative to GeneA */
      if(!tri_circum_3d(dGeneA,dGeneB,dGeneC,dCircum)) {
  /* Offshoot to infinity */
  iOffShootsToInfinity++;
  continue; /* Next BlueA : j++ */
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

      /* Find Bucket where shock is located */
      iPosXs = (int) floor((double) fPosXs);
      iPosYs = (int) floor((double) fPosYs);
      iPosZs = (int) floor((double) fPosZs);

      /* -- Is Shock located INSIDE the box bounding data ? -- */
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
      pGreen->CircumTri.fPosX = fPosXs;
      pGreen->CircumTri.fPosY = fPosYs;
      pGreen->CircumTri.fPosZ = fPosZs;
      pGreen->fDist2GeneSq = fDSq;
#endif

      /* -------------------- VALIDATE --------------------- */

      if(!ValidGreenShockViaVoxels(pShockData, pInData, pGeneCoord,
           pVoxels, pDimAll)) {
  iNumInvalid++;
  continue; /* Part of SS only: Goto next BlueA ngb : j++ */
      }

      /* -------- Found a New Valid Curve Shock --------- */

      if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;

      pGreen->isFlagValid = TRUE;
      pGreen->isInitType  = GREEN_FROM_BLUE;
      pGreen->iBlue[0] = iBlue;
      pGreen->iBlue[1] = iBlueA;
      pGreen->iBlue[2] = IDLE;
      pGreen->isStatus = ACTIVE;

#if FALSE
      /* -- Find the Type of shock curve via Trilinear Coordinates -- */
      if(!GetTriCoordOfCurve(pInData, pGreen)) {
  fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
    pcFName, iCircum);
  exit(-4);
      }
      switch(pGreen->isType) {
      case GREEN_I:
  /* Seek 3rd blue shock ? */
  iGreenTypeI++;
  break;
      case GREEN_II: /* One new Blue sheet exits */
  iGreenTypeII++;
  break;
      default:
  break;
      }
#endif
      iCircum++;
      pGreen++;

    } /* Next BlueA : j++ */

    /* ------- All Blue shocks of GeneA checked for intercepts ------- *\
    \* ---- 2nd: Intersect with other Blue shocks linked to GeneB ---- */

    for(j = 0; j < iNumBlueB; j++) {

      iBlueB = *piBlueB++;
      if(iBlueB == iBlue)
  continue; /* Same as Blue shock under consideration: Skip it */
      pBlueB = (pScaf->pBlue)+iBlueB;
      if(pBlueB->isFlagPermute > iFlagPermuteNot)
  continue; /* Already processed for intercept: Avoid duplicates */
#if FALSE
      if(pBlueB->isInitType == BLUE_SUB)
  continue; /* Skip Shocks found at Low Res. */
#endif
      if(!pBlueB->isFlagValid)
  continue;

      if(iGeneB == pBlueB->iGene[0]) /* Must share GeneB */
  iGeneC = pBlueB->iGene[1];
      else
  iGeneC = pBlueB->iGene[0];
      if(iGeneC == iGeneA) {
  fprintf(stderr, "ERROR(%s):\n\t iGeneC (%d) == iGeneA\n",
    pcFName, iGeneC);
  exit(-5);
      }

      /* -- Check for duplicate of existing Green shock -- */

      iFlagNewTri = TRUE;
      iNumGreenB = pDataB->isNumGreen; /* Sharing GeneB */
      piGreenB = pDataB->piGreenList;
      for(k = 0; k < iNumGreenB; k++) {
  iGreenB = *piGreenB++;
  pGreenB = (pScaf->pGreen)+iGreenB;
  iNewA = pGreenB->iGene[0];
  iNewB = pGreenB->iGene[1];
  iNewC = pGreenB->iGene[2];
  iSetA = iSetC = FALSE;
  if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
    iSetA = TRUE;
  if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
    iSetC = TRUE;
  iSet = iSetA + iSetC;
  if(iSet > 1) { /* Found an existing Green shock */
    iDuplicCurve++;
    /* Keep track of Blue sheet label if not already there ? */
    /* TBD (useful if Green Type I only) */
    iFlagNewTri = FALSE;
    break; /* Get out of For(k) loop */
  }
      } /* Check next Green shock linked to GeneB: k++ */

      if(!iFlagNewTri)
  continue; /* Not a new triplet: Skip it : j++ */

      /* -- OK : New triplet of genes: Potential Green shock -- */

      pDataC = pInData+iGeneC;
      pGeneC = &(pDataC->Sample);
      dGeneC[0] = (double) pGeneC->fPosX;
      dGeneC[1] = (double) pGeneC->fPosY;
      dGeneC[2] = (double) pGeneC->fPosZ;

      dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

      /* CircumCenter of 3D triangle is calculated. *\
      \* Returned coordinates are relative to GeneA */
      if(!tri_circum_3d(dGeneA,dGeneB,dGeneC,dCircum)) {
  /* Offshoot to infinity */
  iOffShootsToInfinity++;
  continue; /* Next BlueB : j++ */
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

      /* Find Bucket where shock is located */
      iPosXs = (int) floor((double) fPosXs);
      iPosYs = (int) floor((double) fPosYs);
      iPosZs = (int) floor((double) fPosZs);

      /* -- Is Shock located INSIDE the box bounding data ? -- */

      if(iPosXs < iXmin || iPosXs > iXmax ||
   iPosYs < iYmin || iPosYs > iYmax ||
   iPosZs < iZmin || iPosZs > iZmax) {
  /* Shock is OUTSIDE the box bounding data */
  /* pGreen->iStatus = PASSIVE; */
  iOutOfBox++;
  continue; /* We do not keep these: Goto next BlueB : j++ */
      }

      iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
      iLabelBuckShock = *(pisLookUp+iPos);

      pGreen->iBuckXYZ = iLabelBuckShock;

#if HIGH_MEM
      pGreen->CircumTri.fPosX = fPosXs;
      pGreen->CircumTri.fPosY = fPosYs;
      pGreen->CircumTri.fPosZ = fPosZs;
      pGreen->fDist2GeneSq = fDSq;
#endif

      pShockData->fDist2GeneSq = fDSq;
      pShockData->ShockCoord.fPosX = fPosXs;
      pShockData->ShockCoord.fPosY = fPosYs;
      pShockData->ShockCoord.fPosZ = fPosZs;

      pShockData->iGene[0] = pGreen->iGene[0] = iGeneA; /* Shared gene */
      pShockData->iGene[1] = pGreen->iGene[1] = iGeneB;
      pShockData->iGene[2] = pGreen->iGene[2] = iGeneC;

      /* -------------------- VALIDATE --------------------- */

      if(!ValidGreenShockViaVoxels(pShockData, pInData, pGeneCoord,
           pVoxels, pDimAll)) {
  iNumInvalid++;
  continue; /* Part of SS only: Goto next BlueB ngb : j++ */
      }

      /* Else : Valid! new green shock */

      if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;

      pGreen->isFlagValid = TRUE;
      pGreen->isInitType  = GREEN_FROM_BLUE;
      pGreen->iBlue[0] = iBlue;
      pGreen->iBlue[1] = iBlueB;
      pGreen->iBlue[2] = IDLE;
      pGreen->isStatus = ACTIVE;

#if FALSE
      /* -- Find the Type of shock curve via Trilinear Coordinates -- */
      if(GetTriCoordOfCurve(pInData, pGreen)) {
  switch(pGreen->isType) {
  case GREEN_I:
    iGreenTypeI++;
    /* Seek 3rd blue shock ? */
    break;
  case GREEN_II: /* One new Blue sheet exits */
    iGreenTypeII++;
    break;
  default:
    break;
  }
      }
      else {
  fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
    pcFName, iCircum);
      }
#endif      
      iCircum++;
      pGreen++;

    } /* Next BlueB : j++ */

  } /* Next Blue Shock : i++ */

  fprintf(stderr, "\tFound %d new Green shocks.\n", iCircum);
  fprintf(stderr, "\t\t%d Duplicates found.\n", iDuplicCurve);
  fprintf(stderr,
    "\tMax distance to Generators amongst Valid shocks = %.2f \n",  
    (float) sqrt((double) fDistSqLarge));
  fprintf(stderr,
    "\tLargest radius evaluated = %.2f\n",
    (float) sqrt((double) fDistSqLargest));
  fprintf(stderr, "\t\tTo infinity = %d ,\n", iOffShootsToInfinity);
  fprintf(stderr, "\t\tOut of Box (but < max dist) = %d .\n",
    iOutOfBox);

  fprintf(stderr, "\tInvalid shocks = %d .\n", iNumInvalid);

  pDimMBuck->iNumShockCurve  = iCircum;
  /* pDimMBuck->iNumShockCurveV = iCircum; */
  /* pDimMBuck->iNumShockGreen2Blue = iGreenTypeII; */
  pDimMBuck->iNumShockGreen2Blue = iCircum;

  return;
}

/*------------------------------------------------------------*\
 * Function: FetchGreenViaBlue
 *
 * Usage: Seek all Initial Curve shocks - some are part of the MA
 *    and thus, the source of a shock curve, others are only
 *    part of the SS. Keep only valid shocks (on MA).
 *    This is to be used once all Isolated Blue shock have
 *    been found. Rather than visiting genes to initiate
 *    sheet flow, we visit directly the sheets already on
 *    the scaffold.
 *    We intersect ACTIVE sheets here, with passive and
 *    other active ones.
 *
\*------------------------------------------------------------*/

int
FetchGreenViaBlue(InputData *pInData, ShockScaffold *pScaf,
      ShockCurve *pShockCurves,
      short *pisLookUp, Buckets *pBucksXYZ,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimMBuck, Dimension *pDimAll)
{
  char    *pcFName = "FetchGreenViaBlue";
  register int  i, j, k;
  int    iXdim, iXmin,iYmin,iZmin, iXmax,iYmax,iZmax;
  int    iSliceSize, iOffShootsToInfinity, iNumInvalid, iPos;
  int    iFlagPermute,iFlagPermuteNot, iNumSheetV, iBlue, iCircum;
  int    iOutOfBox, iBlueA,iBlueB;
  int    iGeneA,iGeneB,iGeneC, iNumBlueA,iNumBlueB;
  int    iPosXs,iPosYs,iPosZs, iLabelBuckShock, iDuplicCurve;
  int    iFlagNewTri, iNumGreenA,iNumGreenB, iGreenA,iGreenB;
  int    iSetA,iSetB,iSetC,iSet, iNewA,iNewB,iNewC, iMaxGreenTmp;
  int    *piBlueA,*piBlueB, *piGreenA,*piGreenB;
  float    fDistSqLarge,fDistSqLargest, fDSq;
  float    fDX,fDY,fDZ, fPosXs,fPosYs,fPosZs;
  double  dGeneA[3],dGeneB[3],dGeneC[3],dCircum[3];
  ShockSheet  *pBlue, *pBlueA, *pBlueB;
  ShockCurve  *pGreen, *pGreenA, *pGreenB;
  InputData  *pDataA, *pDataB, *pDataC;
  Pt3dCoord  *pGeneA, *pGeneB, *pGeneC;
  ShockData  *pShockData;

  /* Preliminaries */

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
  iOffShootsToInfinity = iNumInvalid = 0;
  fDistSqLarge = fDistSqLargest = 0.0;

  iNumSheetV = pScaf->iNumBlueActive;
  /* Pointer to beginning of Active list */
  iBlue  = pScaf->iBluePassiveLast - 1;
  pBlue  = (pScaf->pBlue)+iBlue;
  pGreen = pShockCurves;

  iCircum = iOutOfBox = iDuplicCurve = 0;
  iMaxGreenTmp = pDimMBuck->iMaxGreenShocks;
  iFlagPermute = pDimMBuck->isFlagPermute;
  iFlagPermuteNot = iFlagPermute - 1;

  /* ------- Process Active Blue Sheets one by one -------- *\
  \*      Seek triplets of generators        */
  
  for(i = 0; i < iNumSheetV; i++) { /* For each Active Blue shock */

    pBlue++;
    iBlue++;
    /* To avoid permutations of triples : Put on the Sheets directly */
    /* pDataA->isFlagPermute = isFlagPermute; */
    pBlue->isFlagPermute = iFlagPermute;
    if(!pBlue->isFlagValid)
      continue;
    if(pBlue->isStatus != ACTIVE)
      continue;

    iGeneA = pBlue->iGene[0];
    iGeneB = pBlue->iGene[1];

    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    dGeneA[0] = (double) pGeneA->fPosX;
    dGeneA[1] = (double) pGeneA->fPosY;
    dGeneA[2] = (double) pGeneA->fPosZ;
    iNumBlueA = pDataA->isNumBlue;
    piBlueA   = pDataA->piBlueList;

    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    dGeneB[0] = (double) pGeneB->fPosX;
    dGeneB[1] = (double) pGeneB->fPosY;
    dGeneB[2] = (double) pGeneB->fPosZ;
    iNumBlueB = pDataB->isNumBlue;
    piBlueB   = pDataB->piBlueList;
    
    /* ---- 1st: Intersect with other Blue shocks linked to GeneA ---- */

    for(j = 0; j < iNumBlueA; j++) {

      iBlueA = *piBlueA++;
      if(iBlueA == iBlue)
  continue; /* Same as Blue shock under consideration: Skip it */
      pBlueA = (pScaf->pBlue)+iBlueA;
      if(pBlueA->isFlagPermute > iFlagPermuteNot)
  continue; /* Already processed for intercepts: Avoid duplicate */
      if(!pBlueA->isFlagValid)
  continue;

      if(iGeneA == pBlueA->iGene[0]) /* Must share GeneA */
  iGeneC = pBlueA->iGene[1];
      else
  iGeneC = pBlueA->iGene[0];
      if(iGeneC == iGeneB) {
  fprintf(stderr, "ERROR(%s):\n\t iGeneC (%d) == iGeneB for Blue ",
    pcFName, iGeneC);
  fprintf(stderr, "shocks %d and %d\n", iBlue, iBlueA);
  exit(-4);
      }

      /* -- Check for duplicate of existing Green shock -- */

      iFlagNewTri = TRUE;
      iNumGreenA = pDataA->isNumGreen; /* Sharing GeneA */
      piGreenA = pDataA->piGreenList;
      for(k = 0; k < iNumGreenA; k++) {
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
    break; /* Get out of For(k) loop */
  }
      } /* Check next Green shock linked to GeneA: k++ */

      if(!iFlagNewTri)
  continue; /* Not a new triplet: Skip it : j++ */

      /* -- OK : New triplet of genes: Potential Green shock -- */

      pDataC = pInData+iGeneC;
      pGeneC = &(pDataC->Sample);
      dGeneC[0] = (double) pGeneC->fPosX;
      dGeneC[1] = (double) pGeneC->fPosY;
      dGeneC[2] = (double) pGeneC->fPosZ;

      dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

      /* CircumCenter of 3D triangle is calculated. *\
      \* Returned coordinates are relative to GeneA */
      if(!tri_circum_3d(dGeneA,dGeneB,dGeneC,dCircum)) {
  /* Offshoot to infinity */
  iOffShootsToInfinity++;
  continue; /* Next BlueA : j++ */
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

      /* Find Bucket where shock is located */
      iPosXs = (int) floor((double) fPosXs);
      iPosYs = (int) floor((double) fPosYs);
      iPosZs = (int) floor((double) fPosZs);

      /* -- Is Shock located INSIDE the box bounding data -- */

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
#if HIGH_MEM
      pGreen->CircumTri.fPosX = fPosXs;
      pGreen->CircumTri.fPosY = fPosYs;
      pGreen->CircumTri.fPosZ = fPosZs;
      pGreen->fDist2GeneSq = fDSq;
#endif

      pShockData->iGene[0] = pGreen->iGene[0] = iGeneA; /* Shared gene */
      pShockData->iGene[1] = pGreen->iGene[1] = iGeneB;
      pShockData->iGene[2] = pGreen->iGene[2] = iGeneC;

      pShockData->fDist2GeneSq = fDSq;
      pShockData->ShockCoord.fPosX = fPosXs;
      pShockData->ShockCoord.fPosY = fPosYs;
      pShockData->ShockCoord.fPosZ = fPosZs;

      /* -------------------- VALIDATE --------------------- */

      if(!ValidGreenShockViaVoxels(pShockData, pInData, pGeneCoord,
           pVoxels, pDimAll)) {
  iNumInvalid++;
  continue; /* Part of SS only: Goto next Blue ngb : j++ */
      }
      /* Else : Valid! new green shock */

      if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;

      pGreen->isFlagValid = TRUE;
      pGreen->isInitType  = GREEN_FROM_BLUE;
      pGreen->iBlue[0] = iBlue;
      pGreen->iBlue[1] = iBlueA;
      pGreen->iBlue[2] = IDLE;
      pGreen->isStatus = ACTIVE;

#if FALSE
      /* -- Find the Type of shock curve via Trilinear Coordinates -- */
      if(!GetTriCoordOfCurve(pInData, pGreen)) {
  fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
    pcFName, iCircum);
  exit(-4);
      }
      switch(pGreen->isType) {
      case GREEN_I:
  /* Seek 3rd blue shock ? */
  iGreenTypeI++;
  break;
      case GREEN_II: /* One new Blue sheet exits */
  iGreenTypeII++;
  break;
      default:
  break;
      }
#endif
      iCircum++;
      if(iCircum > iMaxGreenTmp) {
  fprintf(stderr, "WARNING(%s): Overflow(A) in CircumCenters (%d).\n",
    pcFName, iCircum);
  return(FALSE);
      }
      pGreen++;

    } /* Next BlueA : j++ */

    /* ------- All Blue shocks of GeneA checked for intercepts ------- *\
    \* ---- 2nd: Intersect with other Blue shocks linked to GeneB ---- */

    for(j = 0; j < iNumBlueB; j++) {

      iBlueB = *piBlueB++;
      if(iBlueB == iBlue)
  continue; /* Same as Blue shock under consideration: Skip it */
      pBlueB = (pScaf->pBlue)+iBlueB;
      if(pBlueB->isFlagPermute > iFlagPermuteNot)
  continue; /* Already processed for intercept: Avoid duplicates */
      if(!pBlueB->isFlagValid)
  continue;

      if(iGeneB == pBlueB->iGene[0]) /* Must share GeneB */
  iGeneC = pBlueB->iGene[1];
      else
  iGeneC = pBlueB->iGene[0];
      if(iGeneC == iGeneA) {
  fprintf(stderr, "ERROR(%s):\n\t iGeneC (%d) == iGeneA for Blue ",
    pcFName, iGeneC);
  fprintf(stderr, "shocks %d and %d\n", iBlue, iBlueB);
  exit(-5);
      }
      /* -- Check for duplicate of existing Green shock -- */

      iFlagNewTri = TRUE;
      iNumGreenB = pDataB->isNumGreen; /* Sharing GeneB */
      piGreenB = pDataB->piGreenList;
      for(k = 0; k < iNumGreenB; k++) {
  iGreenB = *piGreenB++;
  pGreenB = (pScaf->pGreen)+iGreenB;
  iNewA = pGreenB->iGene[0];
  iNewB = pGreenB->iGene[1];
  iNewC = pGreenB->iGene[2];
  iSetA = iSetC = FALSE;
  if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
    iSetA = TRUE;
  if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
    iSetC = TRUE;
  iSet = iSetA + iSetC;
  if(iSet > 1) { /* Found an existing Green shock */
    iDuplicCurve++;
    /* Keep track of Blue sheet label if not already there ? */
    /* TBD (useful if Green Type I only) */
    iFlagNewTri = FALSE;
    break; /* Get out of For(k) loop */
  }
      } /* Check next Green shock linked to GeneB: k++ */

      if(!iFlagNewTri)
  continue; /* Not a new triplet: Skip it : j++ */

      /* -- OK : New triplet of genes: Potential Green shock -- */

      pDataC = pInData+iGeneC;
      pGeneC = &(pDataC->Sample);
      dGeneC[0] = (double) pGeneC->fPosX;
      dGeneC[1] = (double) pGeneC->fPosY;
      dGeneC[2] = (double) pGeneC->fPosZ;

      dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

      /* CircumCenter of 3D triangle is calculated. *\
      \* Returned coordinates are relative to GeneA */
      if(!tri_circum_3d(dGeneA,dGeneB,dGeneC,dCircum)) {
  /* Offshoot to infinity */
  iOffShootsToInfinity++;
  continue; /* Next BlueA : j++ */
      }

      /* Potential 3D Triangle Circum Center */
      fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
      fDZ = (float) dCircum[2];
      fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
      if(fDSq > fDistSqLargest) fDistSqLargest = fDSq;

      fPosXs = fDX + (float) dGeneA[0];
      fPosYs = fDY + (float) dGeneA[1];
      fPosZs = fDZ + (float) dGeneA[2];

      /* -- Validate Shocks located INSIDE the box bounding data -- */

      /* Find Bucket where shock is located */
      iPosXs = (int) floor((double) fPosXs);
      iPosYs = (int) floor((double) fPosYs);
      iPosZs = (int) floor((double) fPosZs);

      /* -- Is Shock located INSIDE the box bounding data -- */

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
#if HIGH_MEM
      pGreen->CircumTri.fPosX = fPosXs;
      pGreen->CircumTri.fPosY = fPosYs;
      pGreen->CircumTri.fPosZ = fPosZs;
      pGreen->fDist2GeneSq = fDSq;
#endif

      pShockData->iGene[0] = pGreen->iGene[0] = iGeneA; /* Shared gene */
      pShockData->iGene[1] = pGreen->iGene[1] = iGeneB;
      pShockData->iGene[2] = pGreen->iGene[2] = iGeneC;

      pShockData->fDist2GeneSq = fDSq;
      pShockData->ShockCoord.fPosX = fPosXs;
      pShockData->ShockCoord.fPosY = fPosYs;
      pShockData->ShockCoord.fPosZ = fPosZs;

      /* -------------------- VALIDATE --------------------- */

      if(!ValidGreenShockViaVoxels(pShockData, pInData, pGeneCoord,
           pVoxels, pDimAll)) {
  iNumInvalid++;
  continue; /* Part of SS only: Goto next Blue ngb : j++ */
      }

      /* Else : Valid! new green shock */

      if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;

      pGreen->isFlagValid = TRUE;
      pGreen->isInitType  = GREEN_FROM_BLUE;
      pGreen->iBlue[0] = iBlue;
      pGreen->iBlue[1] = iBlueB;
      pGreen->iBlue[2] = IDLE;
      pGreen->isStatus = ACTIVE;

#if FALSE
      /* -- Find the Type of shock curve via Trilinear Coordinates -- */
      if(GetTriCoordOfCurve(pInData, pGreen)) {
  switch(pGreen->isType) {
  case GREEN_I:
    /* Seek 3rd blue shock ? */
    iGreenTypeI++;
    break;
  case GREEN_II: /* One new Blue sheet exits */
    iGreenTypeII++;
    break;
  default:
    break;
  }
      }
      else {
  fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
    pcFName, iCircum);
      }
#endif

      iCircum++;
      if(iCircum > iMaxGreenTmp) {
  fprintf(stderr, "WARNING(%s): Overflow(B) in CircumCenters (%d).\n",
    pcFName, iCircum);
  return(FALSE);
      }
      pGreen++;

    } /* Next BlueB : j++ */

  } /* Next Active Blue Shock : i++ */

  fprintf(stderr, "\tFound %d new Green shocks.\n", iCircum);
  fprintf(stderr,
    "\tMax distance to Generators amongst Valid shocks = %.2f \n",  
    (float) sqrt((double) fDistSqLarge));
  fprintf(stderr,
    "\tLargest radius evaluated = %.2f\n",
    (float) sqrt((double) fDistSqLargest));
  fprintf(stderr, "\t\tTo infinity = %d ,\n", iOffShootsToInfinity);
  fprintf(stderr, "\t\tOut of Box (but < max dist) = %d .\n",
    iOutOfBox);

  fprintf(stderr, "\tInvalid shocks = %d .\n", iNumInvalid);

  pDimMBuck->iNumShockCurve  = iCircum;
  /* pDimMBuck->iNumShockCurveV = iCircum; */
  /* pDimMBuck->iNumShockGreen2Blue = iGreenTypeII; */
  pDimMBuck->iNumShockGreen2Blue = iCircum;

  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: SetValidCurvesExt
 *
 * Usage: Sets a list of valid initial curve shocks on the
 *    scaffold. Adds new sheets too (from Green type II).
 *    Fill-in sheet slot or create missing sheet (type I & III).
 *    Takes data processed via FetchGreenShocksInt()
 *
 *    NB: We need to check if a potential new curve is already
 *    on the list in this step.
 *
\*------------------------------------------------------------*/

int
SetValidCurvesExt(ShockCurve *pShocks, ShockScaffold *pScaf,
      InputData *pInData, short *pisLookUp,
      Buckets *pBucksXYZ, Dimension *pDimMBuck)
{
  char    *pcFName = "SetValidCurvesExt";
  register int  i, j;
  int    iNumGreen, iGreen, iBlue, iGeneA,iGeneB,iGeneC, iMax;
  int    iNumProxiA,iNumProxiB,iNumProxiC, iNewA,iNewB,iNewC;
  int    iGreenActive,iGreenPassive, iBlueActive,iBluePassive;
  int    iType, iStatus, iOldBlue, iBlue1,iBlue2, iSameGreen;
  int    iOldA,iOldB, iNumGreenI,iNumGreenII,iNumGreenIII;
  int    iFlagNewTri, iGreenA, iSetB,iSetC,iSet, iTmp,iTmp2;
  int    iNumBlueSlotSet, iNumBlueMiss, iPosX,iPosY,iPosZ,iPos;
  int    iSliceSize, iXdim, iLabelBuckShock;
  int    iGeneA1,iGeneB1,iGeneA2,iGeneB2,iGeneShared,iGeneNewA;
  int    iGeneNewB;
  int    *piGreenA, *piOldBlue;
  /* static float  fEpsilon = (float) D_EPSILON; */
  float    fAlpha,fBeta,fGamma, fTmp;

  //Ming
  float    fPABx = 0,fPABy = 0,fPABz = 0, fDSq;

  float    fPBCx,fPBCy,fPBCz, fPosX,fPosY,fPosZ;
  ShockSheet  *pNewBlue, *pOldBlue, *pBlue1,*pBlue2;
  ShockCurve  *pGreen, *pNewGreen, *pGreenA;
  InputData  *pDataA,*pDataB,*pDataC;
  Pt3dCoord  *pGeneB, *pGeneC;
  Vector3d  *pNorm;
  ShockData  *pShockData;

  /* Preliminaries */
  
  pShockData = NULL;
  if((pShockData = (ShockData *)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }

  fprintf(stderr, "MESG(%s):\n", pcFName);

  iSliceSize = pDimMBuck->iSliceSize;
  iXdim = pDimMBuck->iXdim;

  iNumGreen = pDimMBuck->iNumShockCurve; /* New active and passive shocks */
  iGreen = pScaf->iGreenActiveLast;
  iBlue  = pScaf->iBlueActiveLast;
#if FALSE
  fprintf(stderr, "\tUp to %d Green shocks to put on scaffold...\n",
    iNumGreen);
  fprintf(stderr, "\t\tfrom position %d (front of Green list)\n",
    iGreen);
  fprintf(stderr, "\t\tand %d for sheets (front of Blue list).\n",
    iBlue);
#endif
  pGreen = pShocks-1;
  pNewGreen = (pScaf->pGreen)+iGreen;
  pNewBlue  = (pScaf->pBlue)+iBlue;

  iGreenActive = iGreenPassive = iBlueActive = iBluePassive = 0;
  iNumGreenI = iNumGreenII = iNumGreenIII = 0;
  iSameGreen = 0;
  iNumBlueSlotSet = iNumBlueMiss = 0;

  /* ---- Process each potential Green shock ---- */

  for(i = 0; i < iNumGreen; i++) {
    pGreen++;

    iGeneA = pNewGreen->iGene[0] = pGreen->iGene[0];
    iGeneB = pNewGreen->iGene[1] = pGreen->iGene[1];
    iGeneC = pNewGreen->iGene[2] = pGreen->iGene[2];

    /* -- Check for duplicate of existing Green shock -- */

    pDataA = pInData+iGeneA;
    iNumProxiA = pDataA->isNumGreen;
    iFlagNewTri = TRUE;
    piGreenA = pDataA->piGreenList;
    for(j = 0; j < iNumProxiA; j++) {
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
  iSameGreen++;
  /* Keep track of Blue sheet label if not already there ? */
  /* TBD (useful if Green Type I only) */
  iFlagNewTri = FALSE;
  break; /* Get out of For(j) loop */
      }
    } /* Check next Green shock linked to GeneA: k++ */
    if(!iFlagNewTri)
      continue; /* Not a new triplet: Skip it : i++ */

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

    /* -- Find the Type of shock curve via Trilinear Coordinates -- */

    if(!GetTriCoordOfCurve(pInData, pGreen, pShockData)) {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, i);
      exit(-3); }

    iType  = pNewGreen->isType = pGreen->isType;
    fAlpha = pShockData->fBaryCoord[0];
    fBeta  = pShockData->fBaryCoord[1];
    fGamma = pShockData->fBaryCoord[2];

    pNewGreen->iBlue[0] = pGreen->iBlue[0];
    pNewGreen->iBlue[1] = pGreen->iBlue[1];
    pNewGreen->iBlue[2] = pGreen->iBlue[2];

    pNewGreen->isInitType = GREEN_FROM_BLUE;
    pNewGreen->isFlagValid = TRUE;
    pNewGreen->isItera = pScaf->isLastItera;
    iStatus = pNewGreen->isStatus = pGreen->isStatus;
    if(iStatus != ACTIVE)
      iGreenPassive++;
    else
      iGreenActive++;

    /* -- Set backpointers to shocks for the 3 Generators -- */

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

    if(pNewGreen->iBlue[2] > -1) {
      pNewGreen++;
      iGreen++; /* Next Valid Green shock */
      continue;
    }

    iSetC = FALSE;
    iBlue1 = pNewGreen->iBlue[0];
    pBlue1 = (pScaf->pBlue)+iBlue1;
    iBlue2 = pNewGreen->iBlue[1];
    pBlue2 = (pScaf->pBlue)+iBlue2;
    /* Find which Gene is shared */
    iGeneA1 = pBlue1->iGene[0];
    iGeneB1 = pBlue1->iGene[1];
    iGeneA2 = pBlue2->iGene[0];
    iGeneB2 = pBlue2->iGene[1];
    if(iGeneA1 == iGeneA2) {
      iGeneShared = iGeneA1;
      iGeneNewA = iGeneB1;
      iGeneNewB = iGeneB2;
    }
    else if(iGeneA1 == iGeneB2) {
      iGeneShared = iGeneA1;
      iGeneNewA = iGeneB1;
      iGeneNewB = iGeneA2;
    }
    else if(iGeneB1 == iGeneA2) {
      iGeneShared = iGeneB1;
      iGeneNewA = iGeneA1;
      iGeneNewB = iGeneB2;
    }
    else if(iGeneB1 == iGeneB2) {
      iGeneShared = iGeneB1;
      iGeneNewA = iGeneA1;
      iGeneNewB = iGeneA2;
    }
    else {
      fprintf(stderr,
        "ERROR(%s): Blue shocks %d and %d do not share a gene.\n",
        pcFName, iBlue1, iBlue2);
      exit(-5);
    }

    /* Use one of the unshared gene to search for an existing Blue shock */
    pDataA = pInData+iGeneNewA;
    iNumProxiA = pDataA->isNumBlue;
    piOldBlue = pDataA->piBlueList;
    for(j = 0; j < iNumProxiA; j++) {
      iOldBlue = *piOldBlue++;
      if(iOldBlue == iBlue1 || iOldBlue == iBlue2)
  continue;
      pOldBlue = (pScaf->pBlue)+iOldBlue;
      iOldA = pOldBlue->iGene[0];
      iOldB = pOldBlue->iGene[1];
      if((iGeneNewB == iOldA) || (iGeneNewB == iOldB)) {
  iSetC = TRUE;
  break;
      }
    } /* Next Blue Proxi: j++ */
    if(iSetC) {
      pNewGreen->iBlue[2] = iOldBlue;
      iNumBlueSlotSet++;
    }
    else { /* This sheet does not exist: Create it */
      iNumBlueMiss++;
#if FALSE
      if(iBlue == 11621) {
  fprintf(stderr, "\tReached New Blue shock %d...\n", iBlue);
      }
#endif
      pNewGreen->iBlue[2] = iBlue; /* Set 3rd sheet */
      pNewBlue->isFlagValid = TRUE;
      /* Initial shock source of sheet correspond to midpoint of BC */
      pDataB = pInData+iGeneNewA;
      pDataC = pInData+iGeneNewB;
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

      pNewBlue->iGene[0] = iGeneNewA;
      pNewBlue->iGene[1] = iGeneNewB;

      if(iType == GREEN_II) {
  pNewBlue->isInitType = BLUE_FROM_GREENII;
      }
      else { /* iType == GREEN_I || iType == GREEN_III */
  pNewBlue->isInitType = BLUE_MISS;
      }

      pNewBlue->iGreen    = iGreen;
      pNewBlue->iRed      = IDLE;
      pNewBlue->isItera   = pScaf->isLastItera;
      pNewBlue->isStatus  = iStatus;

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
  /* pDataB->iMaxNorm = iMax; */
      }
      pDataB->piBlueList[iNumProxiB] = iBlue; /* Backpointer to shock */
      pDataB->isNumBlue++;
      
      pNorm = pDataB->pvNormal+iNumProxiB;
      pNorm->fX = fPBCx;
      pNorm->fY = fPBCy;
      pNorm->fZ = fPBCz;
      pNorm->fLengthSq = fDSq;
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
  /* pDataC->iMaxNorm = iMax; */
      }
      pDataC->piBlueList[iNumProxiC] = iBlue; /* Backpointer to shock */
      pDataC->isNumBlue++;
      
      pNorm = pDataC->pvNormal+iNumProxiC;
      pNorm->fX = -fPABx;
      pNorm->fY = -fPABy;
      pNorm->fZ = -fPABz;
      pNorm->fLengthSq = fDSq;
      /* pDataC->iNumNorm++; */

      pNewBlue++;
      iBlue++; /* Next New Valid Blue shock */

    } /* End of creation of missing Blue sheet */

    pNewGreen++;
    iGreen++; /* Next Valid Green shock */

  } /* Next potential Green shock : i++ */

  iTmp = iNumGreenI+iNumGreenII;
  fprintf(stderr, "\tPut %d Shock Curves on Scaffold.\n", iTmp);
  iNumGreenII -= iNumGreenIII;
  fprintf(stderr,
    "\t\tTypes: I = %d , II (obtuse) = %d , III (right) = %d .\n", 
    iNumGreenI, iNumGreenII, iNumGreenIII);
  fprintf(stderr, "\t\tNew front position of green list = %d\n", iGreen);
  fprintf(stderr, "\t\tNumber of duplicates of curves = %d\n",
    iSameGreen);
  iTmp2 = iGreen - pScaf->iGreenActiveLast;
  if(iTmp != iTmp2) {
    fprintf(stderr,
      "WARNING: GreenI+GreenII (%d) != NumGreenPutOnScaffold (%d)\n",
      iTmp, iTmp2);
  }
  pScaf->iGreenActiveLast = iGreen;
  pScaf->iNumGreenActive += iTmp;
  pScaf->iNumGreenNew     = iTmp;
  
  iTmp = iBlue - pScaf->iBlueActiveLast;
  fprintf(stderr, "\tNew Blue sheets (from Green type II) = %d.\n", iTmp);
  /*fprintf(stderr, "\t\t%d are active, %d are passive.\n", iBlueActive,
    iBluePassive); */
  /* NB: We consider these all Active, because we use a single */
  /*     ordered list on the scaffold to keep track of sheets  */
  pScaf->iBlueActiveLast = iBlue;
  pScaf->iNumBlueActive += iTmp;
  pScaf->iNumBlueNew     = iTmp;
  
  fprintf(stderr, "\t\tNumber of 3rd Blue Slots of Curve set = %d\n",
    iNumBlueSlotSet);
  fprintf(stderr, "\t\tNumber of Missing Blue Sheets created = %d\n",
    iNumBlueMiss);


  return(TRUE);
}


/* --- EoF --- */
