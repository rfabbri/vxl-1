/*------------------------------------------------------------*\
 * File: fetch_red_shocks_v2.c
 *
 * Functions:
 *  FetchRedViaGreenFull
 *  FetchRedViaGreen
 *
 * History:
 *  #0: Nov. 2001, by F.Leymarie (adapted from old version)
 *  #1: June 2002: Fill-in Blue slots of Green Curves.
 *  #2: Aug. 2003: Updated for use with MetaBuckets.
 *
 * Notes:
 *  FetchRedViaGreenFull and FetchRedViaGreen should be
 *  combined into one function (with two ways of initializing).
 *
\*------------------------------------------------------------*/

#include "fetch_red_shocks_v2.h"
#include "circum.h"
#include "valid_shocks.h"

/*------------------------------------------------------------*\
 * Function: FetchRedViaGreenFull
 *
 * Usage: Seek all Initial Vertex shocks - part of the MA.
 *    This is to be used once all Isolated Blue shock have
 *    been found and intersected to generate Curves. Rather
 *    than visiting genes to initiate curve flow, we visit
 *    directly the curves already on the scaffold.
 *    We intersect ALL curves here, passive and active,
 *    but start from active ones only.
 *    We check for duplicates of Vertices, Curves & Sheets.
 *
\*------------------------------------------------------------*/

int
FetchRedViaGreenFull(InputData *pInData, ShockScaffold *pScaf,
         ShockVertex *pShockVertices, short *pisLookUp,
         Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
         Voxel *pVoxels, Dimension *pDimMBuck,
         Dimension *pDimAll)
{
  char    *pcFName = "FetchRedViaGreenFull";
  register int  i, j, k;
  short    isFlagPermute, isFlagPermuteNot;
  int    iGeneA,iGeneB,iGeneC,iGeneD, iCircum, iNumInvalid;
  int    iOffShootsToInfinity, iPos, iGreenA,iGreenB, iNumGreenA;
  int    iSliceSize, iXdim, iNumCurveV, iGreen;
  int    iNewGeneA,iNewGeneB,iNewGeneC, iSet,iSetA,iSetB,iSetC,iSetD;
  int    iPosXs,iPosYs,iPosZs, iLabelBuckShock, iOutOfBox, iMaxRed;
  int    iXmin,iYmin,iZmin, iXmax,iYmax,iZmax, iFlagNewQuad, iTmp;
  int    iNewA,iNewB,iNewC,iNewD;
  int    iDuplicVertex, iNumGreenB, iNumRedA,iNumRedB, iRedA,iRedB;
  int    *piGreenA,*piGreenB, *piRedA,*piRedB;
  float    fDX,fDY,fDZ, fDSq, fPosXs,fPosYs,fPosZs;
  float    fDistSqLargest, fDistSqLarge;
  double  dGeneA[3],dGeneB[3],dGeneC[3],dGeneD[3], dCircum[3];
  ShockCurve  *pGreen, *pGreenA, *pGreenB;
  ShockVertex  *pRed, *pRedA, *pRedB;
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

  iOffShootsToInfinity = iNumInvalid = 0;
  fDistSqLarge = fDistSqLargest = 0.0;

  pRed   = pShockVertices;
  iGreen = -1;
  pGreen = (pScaf->pGreen)+iGreen;
  iNumCurveV = pScaf->iGreenActiveLast;

  iCircum = iOutOfBox = iDuplicVertex = 0;
  iMaxRed = pDimMBuck->iMaxRedShocks;
  isFlagPermute = pDimMBuck->isFlagPermute;
  isFlagPermuteNot = isFlagPermute - 1;

  /* ------- Process All Existing Green Curves one by one ------- *\
  \*      Seek quadruplets of genes        */

  for(i = 0; i < iNumCurveV; i++) { /* For each Green shock */

    pGreen++;
    iGreen++;
    /* To avoid permutations of quads : Put on the Curves directly */
    /* pDataA->iFlag = isFlagPermute; */
    pGreen->isFlagPermute = isFlagPermute;
    if(pGreen->isStatus != ACTIVE)
      continue; /* Curve fully connected - Skip it: i++ */

    iGeneA = pGreen->iGene[0];
    iGeneB = pGreen->iGene[1];
    iGeneC = pGreen->iGene[2];

    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    dGeneA[0] = (double) pGeneA->fPosX;
    dGeneA[1] = (double) pGeneA->fPosY;
    dGeneA[2] = (double) pGeneA->fPosZ;
    iNumGreenA = pDataA->isNumGreen;
    piGreenA   = pDataA->piGreenList;

    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    dGeneB[0] = (double) pGeneB->fPosX;
    dGeneB[1] = (double) pGeneB->fPosY;
    dGeneB[2] = (double) pGeneB->fPosZ;
    iNumGreenB = pDataB->isNumGreen;
    piGreenB   = pDataB->piGreenList;

    pDataC = pInData+iGeneC;
    pGeneC = &(pDataC->Sample);
    dGeneC[0] = (double) pGeneC->fPosX;
    dGeneC[1] = (double) pGeneC->fPosY;
    dGeneC[2] = (double) pGeneC->fPosZ;

    /* ---- 1st: Intersect with other Green shocks linked to GeneA ---- */

    for(j = 0; j < iNumGreenA; j++) {

      iGreenA = *piGreenA++;
      if(iGreenA == iGreen)
  continue; /* Same as Green shock under consideration: Skip it */
      pGreenA = (pScaf->pGreen)+iGreenA;
      if(pGreenA->isFlagPermute > isFlagPermuteNot)
  continue; /* Already processed for intercepts: Avoid duplicate */

      iNewGeneA = pGreenA->iGene[0];
      iNewGeneB = pGreenA->iGene[1];
      iNewGeneC = pGreenA->iGene[2];

      iSetA = iSetB = iSetC = FALSE; /* Must Share GeneA */
      if(iNewGeneA == iGeneB || iNewGeneA == iGeneC)
  iSetA = TRUE;
      if(iNewGeneB == iGeneB || iNewGeneB == iGeneC)
  iSetB = TRUE;
      if(iNewGeneC == iGeneB || iNewGeneC == iGeneC)
  iSetC = TRUE;
      iSet = iSetA + iSetB + iSetC;
      if(iSet != 1)
  continue; /* Must share 2 genes at intersection, i.e., a sheet */

      if(!iSetA && iNewGeneA != iGeneA) iGeneD = iNewGeneA;
      else if(!iSetB && iNewGeneB != iGeneA) iGeneD = iNewGeneB;
      else if(!iSetC && iNewGeneC != iGeneA) iGeneD = iNewGeneC;
      else {
  fprintf(stderr, "ERROR(%s): Intersect Green shocks %d and %d\n",
    pcFName, iGreen, iGreenA);
  fprintf(stderr, "\tGenes: (%d,%d,%d) and (%d,%d,%d)\n",
    iGeneA,iGeneB,iGeneC, iNewGeneA,iNewGeneB,iNewGeneC);
  fprintf(stderr, "\tBut can't identify a 4th GeneD.\n");
  return(FALSE);
      }

      /* -- Check for duplicate of existing Red shocks -- *\
       *   Because we are doing a full search, we check   *
      \*   right away for duplicates, before validation.  */

      iFlagNewQuad = TRUE;
      iNumRedA = pDataA->isNumRed; /* Sharing GeneA */
      piRedA = pDataA->piRedList;
      for(k = 0; k < iNumRedA; k++) {
  iRedA = *piRedA++;
  pRedA = (pScaf->pRed)+iRedA;
  iNewA = pRedA->iGene[0];
  iNewB = pRedA->iGene[1];
  iNewC = pRedA->iGene[2];
  iNewD = pRedA->iGene[3];
  iSetB = iSetC = iSetD = FALSE;
  if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC) ||
     (iGeneB == iNewD))
    iSetB = TRUE;
  if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC) ||
     (iGeneC == iNewD))
    iSetC = TRUE;
  if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC) ||
     (iGeneD == iNewD))
    iSetD = TRUE;
  iSet = iSetB + iSetC + iSetD;
  if(iSet > 2) { /* Found an existing Red shock */
    iDuplicVertex++;
    /* Keep track of Green shock label if not already there */
    /* (useful if Red Type I or II) */
    switch(pRedA->isType) {

    case RED_I: /* REG. TET: 2 more Incoming Green Curves */

      if(pRedA->iGreen[2] < 0 && pRedA->iGreen[3] < 0) {
        iTmp = 2;
        if((pRedA->iGreen[0] != iGreen) &&
     (pRedA->iGreen[1] != iGreen))
    pRedA->iGreen[iTmp++] = iGreen;
        if((pRedA->iGreen[0] != iGreenA) &&
     (pRedA->iGreen[1] != iGreenA))
    pRedA->iGreen[iTmp] = iGreenA;
      }
      else if(pRedA->iGreen[2] > -1 && pRedA->iGreen[3] < 0) {
        if((pRedA->iGreen[0] != iGreen) &&
     (pRedA->iGreen[1] != iGreen) &&
     (pRedA->iGreen[2] != iGreen))
    pRedA->iGreen[3] = iGreen;
        else if((pRedA->iGreen[0] != iGreenA) &&
          (pRedA->iGreen[1] != iGreenA) &&
          (pRedA->iGreen[2] != iGreenA))
    pRedA->iGreen[3] = iGreenA;
      }
      else if(pRedA->iGreen[2] < 0 && pRedA->iGreen[3] > -1) {
        if((pRedA->iGreen[0] != iGreen) &&
     (pRedA->iGreen[1] != iGreen) &&
     (pRedA->iGreen[3] != iGreen))
    pRedA->iGreen[2] = iGreen;
        else if((pRedA->iGreen[0] != iGreenA) &&
          (pRedA->iGreen[1] != iGreenA) &&
          (pRedA->iGreen[3] != iGreenA))
    pRedA->iGreen[2] = iGreenA;
      }
      break;

    case RED_II: /* CAP: 1 more Incoming Green Curve */

      if(pRedA->iGreen[2] < 0) {
        if(pRedA->iGreen[0] != iGreen && pRedA->iGreen[1] != iGreen &&
     pRedA->iGreen[3] != iGreen)
    pRedA->iGreen[2] = iGreen;
        else if(pRedA->iGreen[0] != iGreenA &&
          pRedA->iGreen[1] != iGreenA &&
          pRedA->iGreen[3] != iGreenA)
    pRedA->iGreen[2] = iGreenA;
      }
      break;

    case RED_III:
    default:
      break;
    } /* End of switch(pRedA->isType) */

    iFlagNewQuad = FALSE;
    break; /* Get out of For(k) loop */
  }
      } /* Check next Red shock linked to GeneA: k++ */

      if(!iFlagNewQuad)
  continue; /* Not a new Quadruplet: Skip it : j++ */

      /* -- OK : New quadruplet of genes: Potential Red shock -- */

      pDataD = pInData+iGeneD;
      pGeneD = &(pDataD->Sample);
      dGeneD[0] = (double) pGeneD->fPosX;
      dGeneD[1] = (double) pGeneD->fPosY;
      dGeneD[2] = (double) pGeneD->fPosZ;

      dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

      /* CircumCenter of tetrahedron is calculated. *\
      \* Returned coordinates are relative to GeneA */
      if(!tet_circum(dGeneA,dGeneB,dGeneC,dGeneD,dCircum)) {
  /* Offshoot to infinity */
  iOffShootsToInfinity++;
  continue; /* Next GreenA: j++ */
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

      /* -- Is Shock located INSIDE the box bounding data ? -- */

      if(iPosXs < iXmin || iPosXs > iXmax ||
   iPosYs < iYmin || iPosYs > iYmax ||
   iPosZs < iZmin || iPosZs > iZmax) {
  /* Shock is OUTSIDE the box bounding data */
  /* pRed->isStatus = PASSIVE; */
  iOutOfBox++;
  continue; /* We do not keep these: Goto next GreenA : j++ */
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
      /* -------------------- VALIDATE --------------------- */

      if(!ValidRedShockViaVoxels(pShockData, pInData, pGeneCoord,
         pVoxels, pDimAll)) {
  iNumInvalid++;
  continue; /* Part of SS only: Goto next GreenA ngb : j++ */
      }
#if FALSE
      /* -- Find the Type of Shock Vertex via Quadrilinear Coordinates -- */
      if(!GetQuadCoordOfVertex(pInData, pRed)) {
  continue; /* Degenerate Tetrahedron: skip: j++ */
      }
      /* Else : Valid! New Red shock */
      switch(pRed->isType) {
      case RED_I:
  /* Seek 3rd & 4th Incoming Green shocks ? */
  iNumRedI++;
  break;
      case RED_II: /* CAP: One Green curve exits */
  iNumRedII++;
  break;
      case RED_III: /* A pair of green curves bounding a sheet exit */
  iNumRedIII++;
  break;
      default:
  break;
      } /* End of switch(pRed->isType) */
#endif

      /* -------- Found a New Valid Vertex Shock --------- */

      if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;

      pRed->isFlagValid = TRUE;
      pRed->iGreen[0] = iGreen;
      pRed->iGreen[1] = iGreenA;
      pRed->iGreen[2] = IDLE;
      pRed->iGreen[3] = IDLE;
      pRed->isStatus = ACTIVE;

      iCircum++;
      if(iCircum > iMaxRed) {
  fprintf(stderr,
    "ERROR(%s): OVERFLOW(A): MaxNum Red shocks allocated = %d\n",
    pcFName, iMaxRed);
  return(FALSE);
      }
      pRed++;
    } /* Next GreenA : j++ */

    /* ------- All green shocks of GeneA checked for intercepts ------- *\
    \* ---- 2nd: Intersect with other Green shocks linked to GeneB ---- */

    for(j = 0; j < iNumGreenB; j++) {

      iGreenB = *piGreenB++;
      if(iGreenB == iGreen)
  continue; /* Same as Green shock under consideration: Skip it */
      pGreenB = (pScaf->pGreen)+iGreenB;
      if(pGreenB->isFlagPermute > isFlagPermuteNot)
  continue; /* Already processed for intercepts: Avoid duplicate */

      iNewGeneA = pGreenB->iGene[0];
      iNewGeneB = pGreenB->iGene[1];
      iNewGeneC = pGreenB->iGene[2];

      /* Must Share GeneB; If share GeneA: skip; Find if it shares GeneC *\
      \*  Must share 2 genes at intersection, i.e., a sheet      */

      if(iNewGeneA == iGeneA || iNewGeneB == iGeneA || iNewGeneC == iGeneA)
  continue; /* We already checked all Green shocks of GeneA: j++ */

      if(iNewGeneA == iGeneC) {
  if(iNewGeneB != iGeneB) iGeneD = iNewGeneB;
  else iGeneD = iNewGeneC;
      }
      else if(iNewGeneB == iGeneC) {
  if(iNewGeneA != iGeneB) iGeneD = iNewGeneA;
  else iGeneD = iNewGeneC;
      }
      else if(iNewGeneC == iGeneC) {
  if(iNewGeneA != iGeneB) iGeneD = iNewGeneA;
  else iGeneD = iNewGeneB;
      }
      else
  continue; /* Does Not share GeneB and GeneC: skip: j++ */

      if(iGeneD == iGeneB) {
  fprintf(stderr, "ERROR(%s): Intersect Green shocks %d and %d\n",
    pcFName, iGreen, iGreenA);
  fprintf(stderr, "\tGenes: (%d,%d,%d) and (%d,%d,%d)\n",
    iGeneA,iGeneB,iGeneC, iNewGeneA,iNewGeneB,iNewGeneC);
  fprintf(stderr, "\tBut can't identify a 4th GeneD.\n");
  return(FALSE);
      }

      /* -- Check for duplicate of existing Red shocks -- *\
       *   Because we are doing a full search, we check   *
      \*   right away for duplicates, before validation.  */

      iFlagNewQuad = TRUE;
      iNumRedB = pDataB->isNumRed; /* Sharing GeneB */
      piRedB = pDataB->piRedList;
      for(k = 0; k < iNumRedB; k++) {
  iRedB = *piRedB++;
  pRedB = (pScaf->pRed)+iRedB;
  iNewA = pRedB->iGene[0];
  iNewB = pRedB->iGene[1];
  iNewC = pRedB->iGene[2];
  iNewD = pRedB->iGene[3];
  iSetA = iSetC = iSetD = FALSE;
  if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
    iSetA = TRUE;
  if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
    iSetC = TRUE;
  if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
    iSetD = TRUE;
  iSet = iSetA + iSetC + iSetD;
  if(iSet > 2) { /* Found an existing Red shock */
    iDuplicVertex++;
    /* Keep track of Green shock label if not already there */
    /* (useful if Red Type I or II) */

    switch(pRedB->isType) {

    case RED_I: /* REG. TET: 2 more Incoming Green Curves */

      if(pRedB->iGreen[2] < 0 && pRedB->iGreen[3] < 0) {
        iTmp = 2;
        if((pRedB->iGreen[0] != iGreen) &&
     (pRedB->iGreen[1] != iGreen))
    pRedB->iGreen[iTmp++] = iGreen;
        if((pRedB->iGreen[0] != iGreenB) &&
     (pRedB->iGreen[1] != iGreenB) &&
     (pRedB->iGreen[2] != iGreenB))
    pRedB->iGreen[iTmp] = iGreenB;
      }
      else if(pRedB->iGreen[2] > -1 && pRedB->iGreen[3] < 0) {
        if((pRedB->iGreen[0] != iGreen) &&
     (pRedB->iGreen[1] != iGreen) &&
     (pRedB->iGreen[2] != iGreen))
    pRedB->iGreen[3] = iGreen;
        else if((pRedB->iGreen[0] != iGreenB) &&
          (pRedB->iGreen[1] != iGreenB) &&
          (pRedB->iGreen[2] != iGreenB))
    pRedB->iGreen[3] = iGreenB;
      }
      else if(pRedB->iGreen[2] < 0 && pRedB->iGreen[3] > -1) {
        if((pRedB->iGreen[0] != iGreen) &&
     (pRedB->iGreen[1] != iGreen) &&
     (pRedB->iGreen[3] != iGreen))
    pRedB->iGreen[2] = iGreen;
        else if((pRedB->iGreen[0] != iGreenB) &&
          (pRedB->iGreen[1] != iGreenB) &&
          (pRedB->iGreen[3] != iGreenB))
    pRedB->iGreen[2] = iGreenB;
      }
      break;

    case RED_II: /* CAP: 1 more Incoming Green Curve */

      if(pRedB->iGreen[2] < 0) {
        if(pRedB->iGreen[0] != iGreen && pRedB->iGreen[1] != iGreen &&
     pRedB->iGreen[3] != iGreen)
    pRedB->iGreen[2] = iGreen;
        else if(pRedB->iGreen[0] != iGreenB &&
          pRedB->iGreen[1] != iGreenB &&
          pRedB->iGreen[3] != iGreenB)
    pRedB->iGreen[2] = iGreenB;
      }
      break;

    case RED_III:
    default:
      break;
    } /* End of switch(pRedB->isType) */

    iFlagNewQuad = FALSE;
    break; /* Get out of For(k) loop */
  }
      } /* Check next Red shock linked to GeneB: k++ */

      if(!iFlagNewQuad)
  continue; /* Not a new Quadruplet: Skip it : j++ */

      /* -- OK : New quadruplet of genes: Potential Red shock -- */

      pDataD = pInData+iGeneD;
      pGeneD = &(pDataD->Sample);
      dGeneD[0] = (double) pGeneD->fPosX;
      dGeneD[1] = (double) pGeneD->fPosY;
      dGeneD[2] = (double) pGeneD->fPosZ;

      dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

      /* CircumCenter of tetrahedron is calculated. *\
      \* Returned coordinates are relative to GeneA */
      if(!tet_circum(dGeneA,dGeneB,dGeneC,dGeneD,dCircum)) {
  /* Offshoot to infinity */
  iOffShootsToInfinity++;
  continue; /* Next GreenA: j++ */
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

      /* -- Is Shock located INSIDE the box bounding data ? -- */
      if(iPosXs < iXmin || iPosXs > iXmax ||
   iPosYs < iYmin || iPosYs > iYmax ||
   iPosZs < iZmin || iPosZs > iZmax) {
  /* Shock is OUTSIDE the box bounding data */
  /* pRed->isStatus = PASSIVE; */
  iOutOfBox++;
  continue; /* We do not keep these: Goto next GreenB : j++ */
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
      /* -------------------- VALIDATE --------------------- */

      if(!ValidRedShockViaVoxels(pShockData, pInData, pGeneCoord,
         pVoxels, pDimAll)) {
  iNumInvalid++;
  continue; /* Part of SS only: Goto next GreenA ngb : j++ */
      }
#if FALSE
      /* -- Find the Type of Shock Vertex via Quadrilinear Coordinates -- */
      if(!GetQuadCoordOfVertex(pInData, pRed)) {
  continue; /* Degenerate Tetrahedron: skip: j++ */
      }
      /* Else : Valid! New Red shock */
      switch(pRed->isType) {
      case RED_I:
  /* Seek 3rd & 4th Incoming Green shocks ? */
  iNumRedI++;
  break;
      case RED_II: /* CAP: One Green curve exits */
  iNumRedII++;
  break;
      case RED_III: /* A pair of green curves bounding a sheet exit */
  iNumRedIII++;
  break;
      default:
  break;
      } /* End of switch(pRed->isType) */
#endif

      if(fDSq > fDistSqLarge) fDistSqLarge = fDSq;

      pRed->isFlagValid = TRUE;
      pRed->iGreen[0] = iGreen;
      pRed->iGreen[1] = iGreenB;
      pRed->iGreen[2] = IDLE;
      pRed->iGreen[3] = IDLE;
      pRed->isStatus = ACTIVE;

      iCircum++;
      if(iCircum > iMaxRed) {
  fprintf(stderr,
    "ERROR(%s): OVERFLOW(B): MaxNum Red shocks allocated = %d\n",
    pcFName, iMaxRed);
  return(FALSE);
      }
      pRed++;
    } /* Next GreenB : j++ */
    
    /* ---- All green shocks of GeneA & GeneB checked for intercepts ---- *\
    \*   No need to Intersect with other Green shocks linked to GeneC    */

  } /* Next Green Shock : i++ */

  fprintf(stderr, "\tFound %d new Red shocks.\n", iCircum);
#if FALSE
  fprintf(stderr, "\t\tTypes: I = %d , II = %d (caps), III = %d (slivers).\n",
    iNumRedI,iNumRedII,iNumRedIII);
  fprintf(stderr, "\tDuplicates of red shocks = %d .\n", iDuplicVertex);
  if((iNumRedI+iNumRedII+iNumRedIII) != iCircum) {
    fprintf(stderr, "WARNING: TypeI+TypeII+TypeIII (%d) != iCircum\n",
      (iNumRedI+iNumRedII+iNumRedIII));
  }
#endif
  fprintf(stderr,
    "\tMax distance to Generators amongst Valid shocks = %.2f \n",  
    (float) sqrt((double) fDistSqLarge));
  fprintf(stderr,
    "\tLargest ball radius computed = %.2f\n",
    (float) sqrt((double) fDistSqLargest));

  fprintf(stderr, "\t\ttOffshoots to infinity = %d ,\n", iOffShootsToInfinity);
  fprintf(stderr, "\t\tOut of Box (but finite dist) = %d .\n",
    iOutOfBox);

  fprintf(stderr, "\tInvalid shocks = %d .\n", iNumInvalid);

  pDimMBuck->iNumShockVertex = iCircum;
  pDimMBuck->iNumShockRed2Green = iCircum;
  pDimMBuck->iNumShockRed2Blue  = iCircum;
#if FALSE
  pDimMBuck->iNumShockRed2Green = iNumRedII + (2 * iNumRedIII);
  pDimMBuck->iNumShockRed2Blue  = iNumRedIII;
  fprintf(stderr,
    "\tFrom Red Vertices (types II & III), potential new shocks:\n");
  fprintf(stderr, "\t\t%d Blue Sheets and %d Green Curves.\n",
    pDimMBuck->iNumShockRed2Blue, pDimMBuck->iNumShockRed2Green);
#endif

  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: FetchRedViaGreen
 *
 * Usage: Seek Vertex shocks - part of the MA.
 *    New active curves must have been generated. Rather
 *    than visiting Generators to initiate curve flow, we visit
 *    directly the curves already on the scaffold.
 *
 *    We intersect ACTIVE curves, with passive and active ones.
 *
\*------------------------------------------------------------*/

int
FetchRedViaGreen(InputData *pInData, ShockScaffold *pScaf,
     ShockVertex *pShockVertices, short *pisLookUp,
     Buckets *pBucksXYZ, Pt3dCoord *pGeneCoord,
     Voxel *pVoxels, Dimension *pDimMBuck,
     Dimension *pDimAll)
{
   char    *pcFName = "FetchRedViaGreen";
   //Ming
   int    iOffShoots = 0, iNumInvalid;
   int    iOffShootsToInfinity, iPos, iNumGreenA, iMaxRed;
   int    iGreenA,iGreenB, iLabelBuckShock, iRedA,iRedB;
   int    iNewGeneA,iNewGeneB,iNewGeneC, iSet,iSetA,iSetB,iSetC,iSetD;
   int    iPosXs,iPosYs,iPosZs, iSliceSize, iNext, iGreen;
   int    iNumCurveV, iPercent;
   int    isFlagPermute,isFlagPermuteNot, iNewA,iNewB,iNewC,iNewD, iTmp;
   int    iFlagNewQuad, iNumGreenB, iChunk, iDelta;
   int    iNumRedA,iNumRedB;
   int    *piGreenA,*piGreenB, *piRedA,*piRedB;
   //Ming
   float    fDX,fDY,fDZ, fDSq, fPosXs,fPosYs,fPosZs,fDistMaxSq = 0;
   float    fDistSqLargest, fDistSqLarge;
   double  dGeneA[3],dGeneB[3],dGeneC[3],dGeneD[3], dCircum[3];
   ShockCurve  *pGreen, *pGreenA, *pGreenB;
   ShockVertex  *pRed, *pRedA, *pRedB;
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
   
   int iXdim = pDimMBuck->iXdim;

   int iXmin = pDimMBuck->Limits.iMinX;
   //: The following 2 are missing in Fre's version 
   int iYmin;
   int iZmin;
   iYmin = pDimMBuck->Limits.iMinY;
   iZmin = pDimMBuck->Limits.iMinZ;

   int iXmax = pDimMBuck->Limits.iMaxX;
   int iYmax = pDimMBuck->Limits.iMaxY;
   int iZmax = pDimMBuck->Limits.iMaxZ;
   iSliceSize = pDimMBuck->iSliceSize;
   
   iOffShootsToInfinity = iNumInvalid = 0;
   fDistSqLarge = fDistSqLargest = 0.0;
   
   /* iNumCurveV = pScaf->iGreenActiveLast - iGreen; */
   iNumCurveV = pScaf->iGreenActiveLast
     - pScaf->iGreenPassiveLast;
   /* Pointer to First Active Curve */
   iGreen = pScaf->iGreenPassiveLast - 1;
   /* iGreen = pScaf->iGreenActiveFst - 1; */
   pGreen = (pScaf->pGreen)+iGreen;
   
   pRed = pShockVertices;
   /* iRed = pScaf->iNumRedLast; * Number of Red Shocks thus far */
   
   int iCircum = 0;
   int iOutOfBox = 0;
   int iDuplicVertex = 0;
   iMaxRed = pDimMBuck->iMaxRedShocks;
   isFlagPermute = pDimMBuck->isFlagPermute;
   isFlagPermuteNot = isFlagPermute - 1;
   
   /* iItera = pScaf->isLastItera; */
   /* iItera -= 20; */
   
   /* ------- Process Active Green Curves one by one ------- *\
   \*      Seek quadruplets of generators      */
   
   iDelta = (int) (iNumCurveV / 4);
   iNext = iDelta;
   iChunk = 0;

   for (int i = 0; i < iNumCurveV; i++) {

      if(i > iNext) { /* Print a mesg. for each chunk of data */
         iChunk++;
         iPercent = iChunk * 25;
         if(iPercent < 101) {
           fprintf(stderr, "\tProcessed %d percent of valid Green shocks ...\n",
                iPercent);
        iNext += iDelta; 
         }
      }

      pGreen++;
      iGreen++;
      /* To avoid permutations of quads : Put on the Curves directly */
      /* pDataA->iFlag = isFlagPermute; */
      pGreen->isFlagPermute = isFlagPermute;

      if(pGreen->isStatus != ACTIVE) {
         continue; /* Curve fully connected - Skip it: i++ */
      }

      int iGeneA = pGreen->iGene[0];
      int iGeneB = pGreen->iGene[1];
      int iGeneC = pGreen->iGene[2];

      pDataA = pInData+iGeneA;
      pGeneA = &(pDataA->Sample);
      dGeneA[0] = (double) pGeneA->fPosX;
      dGeneA[1] = (double) pGeneA->fPosY;
      dGeneA[2] = (double) pGeneA->fPosZ;
      iNumGreenA = pDataA->isNumGreen;
      piGreenA   = pDataA->piGreenList;
      
      pDataB = pInData+iGeneB;
      pGeneB = &(pDataB->Sample);
      dGeneB[0] = (double) pGeneB->fPosX;
      dGeneB[1] = (double) pGeneB->fPosY;
      dGeneB[2] = (double) pGeneB->fPosZ;
      iNumGreenB = pDataB->isNumGreen;
      piGreenB   = pDataB->piGreenList;
      
      pDataC = pInData+iGeneC;
      pGeneC = &(pDataC->Sample);
      dGeneC[0] = (double) pGeneC->fPosX;
      dGeneC[1] = (double) pGeneC->fPosY;
      dGeneC[2] = (double) pGeneC->fPosZ;

      /* ---- 1st: Intersect with other Green shocks linked to GeneA ---- */

      for (int j = 0; j < iNumGreenA; j++) {

         iGreenA = *piGreenA++;
         if(iGreenA == iGreen)
           continue; /* Same as Green shock under consideration: Skip it */
         pGreenA = (pScaf->pGreen)+iGreenA;
         if(pGreenA->isFlagPermute > isFlagPermuteNot)
           continue; /* Already processed for intercepts: Avoid duplicate */
         iNewGeneA = pGreenA->iGene[0];
         iNewGeneB = pGreenA->iGene[1];
         iNewGeneC = pGreenA->iGene[2];

         iSetA = iSetB = iSetC = FALSE; /* Must Share GeneA */
         if(iNewGeneA == iGeneB || iNewGeneA == iGeneC)
           iSetA = TRUE;
         if(iNewGeneB == iGeneB || iNewGeneB == iGeneC)
           iSetB = TRUE;
         if(iNewGeneC == iGeneB || iNewGeneC == iGeneC)
           iSetC = TRUE;
         iSet = iSetA + iSetB + iSetC;
         if(iSet != 1)
           continue; /* Must share 2 genes at intersection, i.e., a sheet */

         int iGeneD;
         if(!iSetA && iNewGeneA != iGeneA) 
            iGeneD = iNewGeneA;
         else if(!iSetB && iNewGeneB != iGeneA) 
            iGeneD = iNewGeneB;
         else if(!iSetC && iNewGeneC != iGeneA) 
            iGeneD = iNewGeneC;
         else {
           fprintf(stderr, "ERROR(%s): Intersect Green shocks %d and %d\n",
                pcFName, iGreen, iGreenA);
           fprintf(stderr, "\tGenes: (%d,%d,%d) and (%d,%d,%d)\n",
                   iGeneA,iGeneB,iGeneC, iNewGeneA,iNewGeneB,iNewGeneC);
           fprintf(stderr, "\tBut can't identify a 4th GeneD.\n");
           exit(-4);
         }

         /* -- Check for duplicate of existing Red shock -- */
   
         iFlagNewQuad = TRUE;
         iNumRedA = pDataA->isNumRed; /* Sharing GeneA */
         piRedA = pDataA->piRedList;
         for (int k = 0; k < iNumRedA; k++) {
           iRedA = *piRedA++;
           pRedA = (pScaf->pRed)+iRedA;
           iNewA = pRedA->iGene[0];
           iNewB = pRedA->iGene[1];
           iNewC = pRedA->iGene[2];
           iNewD = pRedA->iGene[3];
           iSetB = iSetC = iSetD = FALSE;
           if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC) ||
              (iGeneB == iNewD))
             iSetB = TRUE;
           if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC) ||
              (iGeneC == iNewD))
             iSetC = TRUE;
           if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC) ||
              (iGeneD == iNewD))
             iSetD = TRUE;
           iSet = iSetB + iSetC + iSetD;
           if(iSet > 2) { /* Found an existing Red shock */
               iDuplicVertex++;
               /* Keep track of Green shock label if not already there */
               /* (useful if Red Type I or II) */
               switch(pRedA->isType) {
               case RED_I: /* REG. TET: 2 more Incoming Green Curves */
                 if(pRedA->iGreen[2] < 0 && pRedA->iGreen[3] < 0) {
                    iTmp = 2;
                    if((pRedA->iGreen[0] != iGreen) &&
                      (pRedA->iGreen[1] != iGreen))
                      pRedA->iGreen[iTmp++] = iGreen;
                    if((pRedA->iGreen[0] != iGreenA) &&
                      (pRedA->iGreen[1] != iGreenA) &&
                      (pRedA->iGreen[2] != iGreenA))
                      pRedA->iGreen[iTmp] = iGreenA;
                 }
                 else if(pRedA->iGreen[2] > -1 && pRedA->iGreen[3] < 0) {
                    if((pRedA->iGreen[0] != iGreen) &&
                      (pRedA->iGreen[1] != iGreen) &&
                      (pRedA->iGreen[2] != iGreen))
                      pRedA->iGreen[3] = iGreen;
                    else if((pRedA->iGreen[0] != iGreenA) &&
                      (pRedA->iGreen[1] != iGreenA) &&
                      (pRedA->iGreen[2] != iGreenA))
                      pRedA->iGreen[3] = iGreenA;
                 }
                 else if(pRedA->iGreen[2] < 0 && pRedA->iGreen[3] > -1) {
                    if((pRedA->iGreen[0] != iGreen) &&
                      (pRedA->iGreen[1] != iGreen) &&
                      (pRedA->iGreen[3] != iGreen))
                      pRedA->iGreen[2] = iGreen;
                    else if((pRedA->iGreen[0] != iGreenA) &&
                      (pRedA->iGreen[1] != iGreenA) &&
                      (pRedA->iGreen[3] != iGreenA))
                      pRedA->iGreen[2] = iGreenA;
                 }
              break;
              case RED_II: /* CAP: 1 more Incoming Green Curve */
                 if(pRedA->iGreen[2] < 0) {
                    if(pRedA->iGreen[0] != iGreen &&
                      pRedA->iGreen[1] != iGreen &&
                      pRedA->iGreen[3] != iGreen)
                      pRedA->iGreen[2] = iGreen;
                    else if(pRedA->iGreen[0] != iGreenA &&
                      pRedA->iGreen[1] != iGreenA &&
                      pRedA->iGreen[3] != iGreenA)
                      pRedA->iGreen[2] = iGreenA;
                 }
              break;
              case RED_III:
              default:
              break;
           } /* End of switch(pRedA->isType) */

           iFlagNewQuad = FALSE;
           break; /* Get out of For(k) loop */
        }
      } /* Check next Red shock linked to GeneA: k++ */

      if(!iFlagNewQuad)
        continue; /* Not a new Quadruplet: Skip it : j++ */

      /* -- OK : New quadruplet of sources: Potential Red shock -- */

      pDataD = pInData+iGeneD;
      pGeneD = &(pDataD->Sample);
      dGeneD[0] = (double) pGeneD->fPosX;
      dGeneD[1] = (double) pGeneD->fPosY;
      dGeneD[2] = (double) pGeneD->fPosZ;

      dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

      /* CircumCenter of tetrahedron is calculated. *\
      \* Returned coordinates are relative to GeneA */

      if(!tet_circum(dGeneA,dGeneB,dGeneC,dGeneD,dCircum)) {
        /* Offshoot to infinity */
        iOffShootsToInfinity++;
        continue; /* Next GreenA: j++ */
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

      /* -- Is Shock located INSIDE the box bounding data ? -- */
      if(iPosXs < iXmin || iPosXs > iXmax ||
        iPosYs < iYmin || iPosYs > iYmax ||
        iPosZs < iZmin || iPosZs > iZmax) {
        /* Shock is OUTSIDE the box bounding data */
        /* pRed->isStatus = PASSIVE; */
        iOutOfBox++;
        continue; /* We do not keep these: Goto next GreenA : j++ */
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
      /* -------------------- VALIDATE --------------------- */

      if(!ValidRedShockViaVoxels(pShockData, pInData, pGeneCoord,
         pVoxels, pDimAll)) {
        iNumInvalid++;
        continue; /* Part of SS only: Goto next GreenA ngb : j++ */
      }
      if(fDSq > fDistSqLarge) 
         fDistSqLarge = fDSq;

      pRed->isFlagValid = TRUE;
      pRed->iGreen[0] = iGreen;
      pRed->iGreen[1] = iGreenA;
      pRed->iGreen[2] = IDLE;
      pRed->iGreen[3] = IDLE;
      pRed->isStatus = ACTIVE;

      iCircum++;
      if(iCircum > iMaxRed) {
        fprintf(stderr, "ERROR(%s): OVERFLOW(A): MaxNum Red shocks allocated = %d\n",  pcFName, iMaxRed);
        return(FALSE);
      }
      pRed++;
    } /* Next GreenA : j++ */
    
    /* ------- All green shocks of GeneA checked for intercepts ------- *\
    \* ---- 2nd: Intersect with other Green shocks linked to GeneB ---- */

    for(int j = 0; j < iNumGreenB; j++) {

      iGreenB = *piGreenB++;
      if(iGreenB == iGreen)
        continue; /* Same as Green shock under consideration: Skip it */
      pGreenB = (pScaf->pGreen)+iGreenB;
      if(pGreenB->isFlagPermute > isFlagPermuteNot)
        continue; /* Already processed for intercepts: Avoid duplicate */
      iNewGeneA = pGreenB->iGene[0];
      iNewGeneB = pGreenB->iGene[1];
      iNewGeneC = pGreenB->iGene[2];

      /* Must Share GeneB; If share GeneA: skip; Find if it shares GeneC *\
      \*  Must share 2 genes at intersection, i.e., a sheet      */

      if(iNewGeneA == iGeneA || iNewGeneB == iGeneA || iNewGeneC == iGeneA)
        continue; /* We already checked all Green shocks of GeneA: j++ */

      int iGeneD;
      if(iNewGeneA == iGeneC) {
        if(iNewGeneB != iGeneB) iGeneD = iNewGeneB;
        else iGeneD = iNewGeneC;
      }
      else if(iNewGeneB == iGeneC) {
        if(iNewGeneA != iGeneB) iGeneD = iNewGeneA;
        else iGeneD = iNewGeneC;
      }
      else if(iNewGeneC == iGeneC) {
        if(iNewGeneA != iGeneB) iGeneD = iNewGeneA;
        else iGeneD = iNewGeneB;
      }
      else
        continue; /* Does Not share GeneB and GeneC: skip: j++ */

      if(iGeneD == iGeneB) {
        fprintf(stderr, "ERROR(%s): Intersect Green shocks %d and %d\n",
          pcFName, iGreen, iGreenA);
        fprintf(stderr, "\tGenes: (%d,%d,%d) and (%d,%d,%d)\n",
          iGeneA,iGeneB,iGeneC, iNewGeneA,iNewGeneB,iNewGeneC);
        fprintf(stderr, "\tBut can't identify a 4th GeneD.\n");
        return(FALSE);
      }

      /* -- Check for duplicate of existing Red shocks -- */

      iFlagNewQuad = TRUE;
      iNumRedB = pDataB->isNumRed; /* Sharing GeneB */
      piRedB = pDataB->piRedList;
      for (int k = 0; k < iNumRedB; k++) {
        iRedB = *piRedB++;
        pRedB = (pScaf->pRed)+iRedB;
        iNewA = pRedB->iGene[0];
        iNewB = pRedB->iGene[1];
        iNewC = pRedB->iGene[2];
        iNewD = pRedB->iGene[3];
        iSetA = iSetC = iSetD = FALSE;
        if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
          iSetA = TRUE;
        if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
          iSetC = TRUE;
        if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
          iSetD = TRUE;
        iSet = iSetA + iSetC + iSetD;
        if(iSet > 2) { /* Found an existing Red shock */
          iDuplicVertex++;
          /* Keep track of Green shock label if not already there */
          /* (useful if Red Type I or II) */
      
          switch(pRedB->isType) {
      
          case RED_I: /* REG. TET: 2 more Incoming Green Curves */
      
            if(pRedB->iGreen[2] < 0 && pRedB->iGreen[3] < 0) {
              iTmp = 2;
              if((pRedB->iGreen[0] != iGreen) &&
           (pRedB->iGreen[1] != iGreen))
          pRedB->iGreen[iTmp++] = iGreen;
              if((pRedB->iGreen[0] != iGreenB) &&
           (pRedB->iGreen[1] != iGreenB) &&
           (pRedB->iGreen[2] != iGreenB))
          pRedB->iGreen[iTmp] = iGreenB;
            }
            else if(pRedB->iGreen[2] > -1 && pRedB->iGreen[3] < 0) {
              if((pRedB->iGreen[0] != iGreen) &&
           (pRedB->iGreen[1] != iGreen) &&
           (pRedB->iGreen[2] != iGreen))
          pRedB->iGreen[3] = iGreen;
              else if((pRedB->iGreen[0] != iGreenB) &&
                (pRedB->iGreen[1] != iGreenB) &&
                (pRedB->iGreen[2] != iGreenB))
          pRedB->iGreen[3] = iGreenB;
            }
            else if(pRedB->iGreen[2] < 0 && pRedB->iGreen[3] > -1) {
              if((pRedB->iGreen[0] != iGreen) &&
           (pRedB->iGreen[1] != iGreen) &&
           (pRedB->iGreen[3] != iGreen))
          pRedB->iGreen[2] = iGreen;
              else if((pRedB->iGreen[0] != iGreenB) &&
                (pRedB->iGreen[1] != iGreenB) &&
                (pRedB->iGreen[3] != iGreenB))
          pRedB->iGreen[2] = iGreenB;
            }
            break;
      
          case RED_II: /* CAP: 1 more Incoming Green Curve */
      
            if(pRedB->iGreen[2] < 0) {
              if(pRedB->iGreen[0] != iGreen &&
           pRedB->iGreen[1] != iGreen &&
           pRedB->iGreen[3] != iGreen)
          pRedB->iGreen[2] = iGreen;
              else if(pRedB->iGreen[0] != iGreenB &&
                pRedB->iGreen[1] != iGreenB &&
                pRedB->iGreen[3] != iGreenB)
          pRedB->iGreen[2] = iGreenB;
            }
            break;
      
          case RED_III:
          default:
            break;
          } /* End of switch(pRedB->isType) */
      
          iFlagNewQuad = FALSE;
          break; /* Get out of For(k) loop */
        }
      } /* Check next Red shock linked to GeneB: k++ */

      if(!iFlagNewQuad)
        continue; /* Not a new Quadruplet: Skip it : j++ */

      /* -- OK : New quadruplet of genes: Potential Red shock -- */

      pDataD = pInData+iGeneD;
      pGeneD = &(pDataD->Sample);
      dGeneD[0] = (double) pGeneD->fPosX;
      dGeneD[1] = (double) pGeneD->fPosY;
      dGeneD[2] = (double) pGeneD->fPosZ;

      dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;

      /* CircumCenter of tetrahedron is calculated. *\
      \* Returned coordinates are relative to geneA */

      if(!tet_circum(dGeneA,dGeneB,dGeneC,dGeneD,dCircum)) {
        /* Offshoot to infinity */
        iOffShootsToInfinity++;
        continue; /* Next GreenA: j++ */
      }

      /* Potential 3D Tetrahedron CircumCenter */
      fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
      fDZ = (float) dCircum[2];
      fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
      if(fDSq > fDistMaxSq) {
        if(fDSq > fDistSqLargest) 
            fDistSqLargest = fDSq;
        pRed->isFlagValid = FALSE;
        iOffShoots++;
           continue; /* Too far: Goto next sample: j++ */
      }

      fPosXs = fDX + (float) dGeneA[0];
      fPosYs = fDY + (float) dGeneA[1];
      fPosZs = fDZ + (float) dGeneA[2];

      /* -- Validate Shocks located INSIDE the box bounding data -- */

      iPosXs = (int) floor((double) fPosXs);
      iPosYs = (int) floor((double) fPosYs);
      iPosZs = (int) floor((double) fPosZs);

      /* -- Is Shock located INSIDE the box bounding data ? -- */
      if(iPosXs < iXmin || iPosXs > iXmax ||
        iPosYs < iYmin || iPosYs > iYmax ||
        iPosZs < iZmin || iPosZs > iZmax) {
        /* Shock is OUTSIDE the box bounding data */
        /* pRed->isStatus = PASSIVE; */
        iOutOfBox++;
        continue; /* We do not keep these: Goto next GreenB : j++ */
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
      /* -------------------- VALIDATE --------------------- */

      if(!ValidRedShockViaVoxels(pShockData, pInData, pGeneCoord,
         pVoxels, pDimAll)) {
        iNumInvalid++;
        continue; /* Part of SS only: Goto next GreenA ngb : j++ */
      }
      if(fDSq > fDistSqLarge) 
         fDistSqLarge = fDSq;

      pRed->isFlagValid = TRUE;
      pRed->iGreen[0] = iGreen;
      pRed->iGreen[1] = iGreenB;
      pRed->iGreen[2] = IDLE;
      pRed->iGreen[3] = IDLE;
      pRed->isStatus = ACTIVE;

      iCircum++;
      if(iCircum > iMaxRed) {
        fprintf(stderr,
          "ERROR(%s): OVERFLOW(B): MaxNum Red shocks allocated = %d\n",
          pcFName, iMaxRed);
        return(FALSE);
      }
      pRed++;
    } /* Next GreenB : j++ */
    
    /* ---- All green shocks of GeneA & GeneB checked for intercepts ---- *\
    \*   No need to Intersect with other Green shocks linked to GeneC    */

  } /* Next Green Shock : i++ */

  fprintf(stderr, "\tFound %d new Red shocks.\n", iCircum);
  fprintf(stderr,
    "\tMax distance to Generators amongst Valid shocks = %.2f \n",  
    (float) sqrt((double) fDistSqLarge));
  fprintf(stderr,
    "\tLargest ball radius computed = %.2f\n",
    (float) sqrt((double) fDistSqLargest));

  fprintf(stderr,
    "\t\tOffshoots to infinity = %d ,\n", iOffShootsToInfinity);
  fprintf(stderr, "\t\tOut of Box (but finite dist) = %d .\n",
    iOutOfBox);

  fprintf(stderr, "\tInvalid shocks = %d .\n", iNumInvalid);

  pDimMBuck->iNumShockVertex = iCircum;
  pDimMBuck->iNumShockRed2Green = iCircum;
  pDimMBuck->iNumShockRed2Blue  = iCircum;

  return(TRUE);
}

/* ----- EoF ----- */
