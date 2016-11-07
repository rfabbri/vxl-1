/*------------------------------------------------------------*\
 * File: shock_utils.c
 *
 * Functions:
 *  TestShocksBlue
 *  TestShocksGreen
 *  TestShocksRed
 *  CheckInputs
 *  CheckInputGreen
 *  TestShocksRedValid
 *  TestShocksGreenValid
 *  TestShocksBlueValid
 *  LinkRedToGreen
 *  InitScaf
 *  CheckResults
 *
 * History:
 *  #0: Nov. 2001, by F.Leymarie
 *  #1: June 2002: Added InitScaf() and CheckResults()
 *
\*------------------------------------------------------------*/

#include "shock_utils.h"
#include "circum.h"
#include "barycentric.h"

/*------------------------------------------------------------*\
 * Function: TestShocksBlue
 *
 * Usage: For each Blue shock, test its uniqueness by visiting its
 *    ngbs. via backpointers on sources.
 *
\*------------------------------------------------------------*/
#if 0
void
TestShocksBlue(ShockScaffold *pScaffold, InputData *pInData, Dimension *pDim)
{
  char    *pcFName = "TestShocksBlue";
  register int  i, j;
  int    iNumBlue, iNumBlueA, iDuplicSheet;
  int    iGeneA,iGeneB, iBlueA;
  int    *piBlueA;
  ShockSheet  *pBlue, *pBlueA;
  InputData  *pGeneA;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  /* ---- Test Sheets --- */

  iNumBlue = pScaffold->iBlueActiveLast;
  pBlue = pScaffold->pBlue-1;
  iDuplicSheet = 0;
  for(i = 0; i < iNumBlue; i++) {
    pBlue++;
    if(!pBlue->isFlagValid) continue;
    
    iGeneA = pBlue->iGene[0];
    iGeneB = pBlue->iGene[1];

    pGeneA = pInData+iGeneA;
    iNumBlueA = pGeneA->isNumBlue;
    piBlueA = pGeneA->piBlueList;
    for(j = 0; j < iNumBlueA; j++) { /* Sharing GeneA */
      iBlueA = *piBlueA++;
      if(iBlueA == i) continue;
      pBlueA = (pScaffold->pBlue)+iBlueA;
      if(!pBlueA->isFlagValid) continue;
      if(iGeneB == pBlueA->iGene[0] || iGeneB == pBlueA->iGene[1]) {
  iDuplicSheet++;
  fprintf(stderr, "\tSheet %d: Type = %d, Genes = (%d,%d)\n",
    i, pBlue->isInitType, iGeneA, iGeneB);
  fprintf(stderr,
    "\t has sheet %d: Type = %d, Genes = (%d,%d) for duplicate(A)\n",
    iBlueA, pBlueA->isInitType, pBlueA->iGene[0],
    pBlueA->iGene[1]);
  pBlueA->isFlagValid = FALSE;
      }
    }
  }

  fprintf(stderr, "\tFound %d duplicates of sheets,\n", iDuplicSheet);
  fprintf(stderr, "\t out of a total of %d sheets.\n", iNumBlue);

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: TestShocksGreen
 *
 * Usage: For each Green shock, test its uniqueness by visiting
 *    its ngbs. via backpointers on sources.
 *
\*------------------------------------------------------------*/
#if 0
void
TestShocksGreen(ShockScaffold *pScaffold, InputData *pInData, Dimension *pDim)
{
  char    *pcFName = "TestShocksGreen";
  register int  i, j;
  int    iNumGreen, iNumGreenA, iDuplicCurve, iGeneA,iGeneB,iGeneC;
  int    iGreenA, iNewA,iNewB,iNewC, iSet,iSetB,iSetC;
  int    *piGreenA;
  ShockCurve  *pGreen, *pGreenA;
  InputData  *pGene;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  /* ---- Test Curves --- */

  iNumGreen = pScaffold->iGreenActiveLast;
  pGreen = pScaffold->pGreen-1;
  iDuplicCurve = 0;

  for(i = 0; i < iNumGreen; i++) {
    pGreen++;
    
    iGeneA = pGreen->iGene[0];
    iGeneB = pGreen->iGene[1];
    iGeneC = pGreen->iGene[2];

    pGene = pInData+iGeneA;
    iNumGreenA = pGene->isNumGreen;
    piGreenA = pGene->piGreenList;
    for(j = 0; j < iNumGreenA; j++) { /* Sharing GeneA */
      iGreenA = *piGreenA++;
      if(iGreenA == i) continue;
      pGreenA = (pScaffold->pGreen)+iGreenA;

      iNewA = pGreenA->iGene[0];
      iNewB = pGreenA->iGene[1];
      iNewC = pGreenA->iGene[2];
      iSetB = iSetC = FALSE;

      if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC))
  iSetB = TRUE;
      if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
  iSetC = TRUE;
      iSet = iSetB + iSetC;
      if(iSet > 1) { /* Found the same shock */
  iDuplicCurve++;
  fprintf(stderr,
    "\tCurve %d: InitType = %d, Type = %d Genes = (%d,%d,%d)\n",
    i, pGreen->isInitType, pGreen->isType,
    iGeneA,iGeneB,iGeneC);
  fprintf(stderr,
    "\t has duplicate %d: InitType = %d, Type = %d, Genes = (%d,%d,%d)\n",
    iGreenA, pGreenA->isInitType, pGreenA->isType,
    iNewA, iNewB, iNewC);
      }
    }
  }

  fprintf(stderr, "\tFound %d duplicates of curves\n", iDuplicCurve);
  fprintf(stderr, "\tOut of a total of %d curves.\n", iNumGreen);

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: TestShocksRed
 *
 * Usage: For each Red shock, test its uniqueness by visiting
 *    its ngbs. via backpointers on sources.
 *
\*------------------------------------------------------------*/
#if 0
void
TestShocksRed(ShockScaffold *pScaffold, InputData *pInData, Dimension *pDim)
{
  char    *pcFName = "TestShocksRed";
  register int  i, j;
  int    iNumRed, iNumRedA, iDuplicVertex, iGeneA,iGeneB,iGeneC,iGeneD;
  int    iRedA, iNewA,iNewB,iNewC,iNewD, iSet,iSetB,iSetC,iSetD;
  int    iGreenA,iGreenB,iGreenC,iGreenD;
  int    *piRedA;
  ShockVertex  *pRed, *pRedA;
  InputData  *pGene;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  /* ---- Test Vertices --- */

  iNumRed = pScaffold->iNumRedLast;
  pRed = pScaffold->pRed-1;
  iDuplicVertex = 0;

  for(i = 0; i < iNumRed; i++) {
    pRed++;
    
    iGeneA = pRed->iGene[0];
    iGeneB = pRed->iGene[1];
    iGeneC = pRed->iGene[2];
    iGeneD = pRed->iGene[3];

    iGreenA = pRed->iGreen[0];
    iGreenB = pRed->iGreen[1];
    iGreenC = pRed->iGreen[2];
    iGreenD = pRed->iGreen[3];
#if FALSE
    if(i == 12) {
      fprintf(stderr, "\tReached Red Shock %d ...\n", i);
    }
#endif
    if(iGreenC == iGreenA || iGreenC == iGreenB) {
      fprintf(stderr,
        " Vertex %d has 3rd green shock %d == one of 1st two (%d,%d).\n",
        i, iGreenC, iGreenA, iGreenB);
      pRed->iGreen[2] = IDLE;
    }
    if(iGreenD == iGreenA || iGreenD == iGreenB ||
       (iGreenD == iGreenC && iGreenD > -1)) {
      fprintf(stderr,
        " Vertex %d has 4th green shock %d == one of 1st three (%d,%d,%d).\n",
        i, iGreenD, iGreenA, iGreenB, iGreenC);
      pRed->iGreen[3] = IDLE;
    }

    pGene = pInData+iGeneA;
    iNumRedA = pGene->isNumRed;
    piRedA = pGene->piRedList;
    for(j = 0; j < iNumRedA; j++) { /* Sharing GeneA */
      iRedA = *piRedA++;
      if(iRedA == i) continue;
      pRedA = (pScaffold->pRed)+iRedA;

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
      if(iSet > 2) { /* Found the same shock */
  iDuplicVertex++;
  fprintf(stderr,
    "\tVertex %d (%d): Genes = (%d,%d,%d,%d) has duplic.\n",
    i, pRed->isType, iGeneA,iGeneB,iGeneC,iGeneD);
  fprintf(stderr,
    "\t     %d (%d): Genes = (%d,%d,%d,%d).\n",
    iRedA, pRedA->isType, iNewA,iNewB,iNewC,iNewD);
#if HIGH_MEM
  if(pRed->isType != pRedA->isType) {
    fprintf(stderr,
      "\tQuads: (%.2f,%.2f,%.2f,%.2f) vs. (%.2f,%.2f,%.2f,%.2f)\n",
      pRed->fQuadCoord[0], pRed->fQuadCoord[1],
      pRed->fQuadCoord[2], pRed->fQuadCoord[3],
      pRedA->fQuadCoord[0], pRedA->fQuadCoord[1],
      pRedA->fQuadCoord[2], pRedA->fQuadCoord[3]);
    fprintf(stderr,
      "\tCc: (%.2f,%.2f,%.2f) d=%.2f vs. (%.2f,%.2f,%.2f) d=%.2f\n",
      pRed->CircumSphere.fPosX,pRed->CircumSphere.fPosY,
      pRed->CircumSphere.fPosZ,pRed->fDist2GeneSq,
      pRedA->CircumSphere.fPosX,pRedA->CircumSphere.fPosY,
      pRedA->CircumSphere.fPosZ,pRedA->fDist2GeneSq);
  }
#endif
      }
    }
  }

  fprintf(stderr, "\tFound %d duplicates of nodes\n", iDuplicVertex);
  fprintf(stderr, "\tOut of a total of %d nodes.\n", iNumRed);

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: CheckInputs
 *
 * Usage: Reports on Sources having more than a given number of
 *    shocks (sheets, curves or nodes). Also checks if
 *    maximum number of shocks (space allocated) has been
 *    reached.
 *
\*------------------------------------------------------------*/
#if 0
void
CheckInputs(ShockScaffold *pScaffold, InputData *pInData,
      Dimension *pDim, int iNum)
{
  char    *pcFName = "CheckInputs";
  register int  i, j;
  int    iNumPts, iBlue, iGreen, iRed, iTmp;
  int    iBlueMax, iGreenMax, iRedMax;
  int    *piBlue, *piGreen, *piRed;
  InputData  *pData;

  fprintf(stderr, "MESG(%s):...\n", pcFName);

  iBlueMax = pScaffold->iBlueActiveLast;
  iGreenMax = pScaffold->iGreenActiveLast;
  iRedMax = pScaffold->iNumRedLast;

  iNumPts = pDim->iNum3dPts;
  pData = pInData;
  for(i = 0; i < iNumPts; i++) {

    if(pData->isNumBlue > iNum) {
      iTmp = pData->isNumBlue;
      fprintf(stderr, "\tGene %d: has %d Blue shocks (max = %d).\n",
        i, iTmp, pData->isMaxBlue);
      piBlue = pData->piBlueList;
      for(j = 0; j < iTmp; j++) {
  iBlue = *piBlue++;
  if(iBlue > iBlueMax) {
    fprintf(stderr,
      "\t\tList el. %d indicates Blue shock %d out of bound %d.\n",
      j, iBlue, iBlueMax);
    exit(-1);
  }
      }
    }
    if(pData->isNumGreen > iNum) {
      iTmp = pData->isNumGreen;
      fprintf(stderr, "\tGene %d: has %d Green shocks (max = %d).\n",
        i, iTmp, pData->isMaxGreen);
      piGreen = pData->piGreenList;
      for(j = 0; j < iTmp; j++) {
  iGreen = *piGreen++;
  if(iGreen > iGreenMax) {
    fprintf(stderr,
      "\t\tList el. %d indicates Green shock %d out of bound %d\n",
      j, iGreen, iGreenMax);
    exit(-1);
  }
      }
    }
    if(pData->isNumRed > iNum) {
      iTmp = pData->isNumRed;
      fprintf(stderr, "\tGene %d: has %d Red shocks (max = %d).\n",
        i, iTmp, pData->isMaxRed);
      piRed = pData->piRedList;
      for(j = 0; j < iTmp; j++) {
  iRed = *piRed++;
  if(iRed > iRedMax) {
    fprintf(stderr,
      "\t\tList el. %d indicates Red shock %d out of bound %d\n",
      j, iRed, iRedMax);
    exit(-1);
  }
      }
    }

    pData++;
  }

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: CheckInputGreen
 *
 * Usage: Reports on Sources having more than a given number of
 *    curve shocks. Prints the list of shocks, associated
 *    sources and circumcenter info. For Debugging purpose.
 *
\*------------------------------------------------------------*/
#if 0
void
CheckInputGreen(ShockScaffold *pScaffold, InputData *pInData,
    Dimension *pDim, int iNum)
{
  char    *pcFName = "CheckInputGreen";
  register int  i, j;
  int    iNumPts, iGreen, iTmp;
  int    *piGreen;
  InputData  *pData;
  ShockCurve  *pGreen;

  fprintf(stderr, "MESG(%s):...\n", pcFName);

  /* iGreenMax = pScaffold->iGreenActiveLast; */
  iNumPts = pDim->iNum3dPts;
  pData = pInData;
  for(i = 0; i < iNumPts; i++) {

    if(pData->isNumGreen > iNum) {
      iTmp = pData->isNumGreen;
      fprintf(stderr, "\tGene %d: has %d Green shocks (max = %d).\n",
        i, iTmp, pData->isMaxGreen);
      piGreen = pData->piGreenList;
      for(j = 0; j < iTmp; j++) {
  iGreen = *piGreen++;
  pGreen = (pScaffold->pGreen)+iGreen;
  fprintf(stderr, "\t %d: Genes = (%d,%d,%d), ", j,
    pGreen->iGene[0], pGreen->iGene[1], pGreen->iGene[2]);
#if HIGH_MEM
  fprintf(stderr, "Cc = (%.2f,%.2f,%.2f), Buck = %d, R = %.2f\n",
    pGreen->CircumTri.fPosX, pGreen->CircumTri.fPosY,
    pGreen->CircumTri.fPosZ, pGreen->iBuckXYZ,
    (float) sqrt((double) pGreen->fDist2GeneSq));
#endif
      } /* j++ */
    }

    pData++;
  } /* i++ */

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: TestShocksRedValid
 *
 * Usage: For each Red shock, test its validity exhaustively
 *    by visiting all sources.
 *
\*------------------------------------------------------------*/
#if 0
#if HIGH_MEM
void
TestShocksRedValid(ShockScaffold *pScaffold, InputData *pInData,
       Dimension *pDim)
{
  char    *pcFName = "TestShocksRedValid";
  register int  i, j;
  int    iNumRed, iInvalidVertex, iGeneA,iGeneB,iGeneC,iGeneD, iNumPts;
  static float  fEpsilon = (float) D_EPSILON;
  float    fDSq, fPosXs,fPosYs,fPosZs, fPx,fPy,fPz, fDSqNew;
  ShockVertex  *pRed;
  InputData  *pData;
  Pt3dCoord  *pGene;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  /* ---- Test Vertices --- */

  iNumPts = pDim->iNum3dPts;
  iNumRed = pScaffold->iNumRedLast;
  pRed = pScaffold->pRed-1;
  iInvalidVertex = 0;

  for(i = 0; i < iNumRed; i++) {
    pRed++;
    
    iGeneA = pRed->iGene[0];
    iGeneB = pRed->iGene[1];
    iGeneC = pRed->iGene[2];
    iGeneD = pRed->iGene[3];

    fDSq  = pRed->fDist2GeneSq;

    fPosXs = pRed->CircumSphere.fPosX;
    fPosYs = pRed->CircumSphere.fPosY;
    fPosZs = pRed->CircumSphere.fPosZ;

    pData = pInData-1;
    for(j = 0; j < iNumPts; j++) {
      pData++;
      if(iGeneA == j || iGeneB == j || iGeneC == j || iGeneD == j)
  continue;

      pGene = &(pData->Sample);
      fPx = pGene->fPosX - fPosXs;
      fPy = pGene->fPosY - fPosYs;
      fPz = pGene->fPosZ - fPosZs;
      fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
      fDSqNew += fEpsilon;

      if(fDSq > fDSqNew) { /* Invalid shock */
  iInvalidVertex++;
  pRed->isFlagValid = FALSE;
  fprintf(stderr, "\tShock %d: Gene=(%d,%d,%d,%d), Buck %d\n", i,
    iGeneA,iGeneB,iGeneC,iGeneD, pRed->iBuckXYZ);
  fprintf(stderr, "\t\tRadSq = %lf > %lf = DistSq to Gene %d, Buck %d\n",
    fDSq, fDSqNew, j, pData->iBuckXYZ);
  break;
      }

    }
  }

  fprintf(stderr, "\tFound %d INVALID nodes\n", iInvalidVertex);
  fprintf(stderr, "\tOut of a total of %d nodes.\n", iNumRed);

  return;
}
#endif
#endif
/*------------------------------------------------------------*\
 * Function: TestShocksGreenValid
 *
 * Usage: For each Green shock, test its validity exhaustively
 *    by visiting all sources.
 *
\*------------------------------------------------------------*/
#if 0
#if HIGH_MEM
void
TestShocksGreenValid(ShockScaffold *pScaffold, InputData *pInData,
         Dimension *pDim)
{
  char    *pcFName = "TestShocksGreenValid";
  register int  i, j;
  int    iNumGreen, iInvalidCurve, iGeneA,iGeneB,iGeneC, iNumPts;
  static float  fEpsilon = (float) D_EPSILON;
  float    fDSq, fPosXs,fPosYs,fPosZs, fPx,fPy,fPz, fDSqNew;
  ShockCurve  *pGreen;
  InputData  *pData;
  Pt3dCoord  *pGene;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  /* ---- Test Vertices --- */

  iNumPts = pDim->iNum3dPts;
  iNumGreen = pScaffold->iGreenActiveLast;
  pGreen = pScaffold->pGreen-1;
  iInvalidCurve = 0;

  for(i = 0; i < iNumGreen; i++) {
    pGreen++;
    
    iGeneA = pGreen->iGene[0];
    iGeneB = pGreen->iGene[1];
    iGeneC = pGreen->iGene[2];

    fDSq  = pGreen->fDist2GeneSq;

    fPosXs = pGreen->CircumTri.fPosX;
    fPosYs = pGreen->CircumTri.fPosY;
    fPosZs = pGreen->CircumTri.fPosZ;

    pData = pInData-1;
    for(j = 0; j < iNumPts; j++) {
      pData++;
      if(iGeneA == j || iGeneB == j || iGeneC == j)
  continue;

      pGene = &(pData->Sample);
      fPx = pGene->fPosX - fPosXs;
      fPy = pGene->fPosY - fPosYs;
      fPz = pGene->fPosZ - fPosZs;
      fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
      fDSqNew += fEpsilon;

      if(fDSq > fDSqNew) { /* Invalid shock */
  iInvalidCurve++;
  pGreen->isFlagValid = FALSE;
  fprintf(stderr, "\tShock %d: Gene=(%d,%d,%d), Buck %d\n", i,
    iGeneA,iGeneB,iGeneC, pGreen->iBuckXYZ);
  fprintf(stderr, "\t\tRadSq = %lf > %lf = DistSq to Gene %d, Buck %d\n",
    fDSq, fDSqNew, j, pData->iBuckXYZ);
  break;
      }

    }
  }

  fprintf(stderr, "\tFound %d INVALID curves\n", iInvalidCurve);
  fprintf(stderr, "\tOut of a total of %d curves.\n", iNumGreen);

  return;
}
#endif
#endif
/*------------------------------------------------------------*\
 * Function: TestShocksBlueValid
 *
 * Usage: For each Blue shock, test its validity exhaustively
 *    by visiting all sources.
 *
\*------------------------------------------------------------*/
#if 0
#if HIGH_MEM
void
TestShocksBlueValid(ShockScaffold *pScaffold, InputData *pInData,
        Dimension *pDim)
{
  char    *pcFName = "TestShocksBlueValid";
  register int  i, j;
  int    iNumBlue, iInvalidSheet, iGeneA,iGeneB, iNumPts;
  static float  fEpsilon = (float) D_EPSILON;
  float    fDSq, fPosXs,fPosYs,fPosZs, fPx,fPy,fPz, fDSqNew;
  ShockSheet  *pBlue;
  InputData  *pGene;
  Pt3dCoord  *pGeneCoord;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);

  /* ---- Test Vertices --- */

  iNumPts = pDim->iNum3dPts;
  iNumBlue = pScaffold->iBlueActiveLast;
  pBlue = pScaffold->pBlue-1;
  iInvalidSheet = 0;

  for(i = 0; i < iNumBlue; i++) {
    pBlue++;

    if(pBlue->isInitType == BLUE_SUB) continue;
    if(!pBlue->isFlagValid) continue;
    
    iGeneA = pBlue->iGene[0];
    iGeneB = pBlue->iGene[1];

    fDSq  = pBlue->fDist2GeneSq;

    fPosXs = pBlue->MidPt.fPosX;
    fPosYs = pBlue->MidPt.fPosY;
    fPosZs = pBlue->MidPt.fPosZ;

    pGene = pInData-1;
    for(j = 0; j < iNumPts; j++) {
      pGene++;
      if(iGeneA == j || iGeneB == j)
  continue;

      pGeneCoord = &(pGene->Sample);
      fPx = pGeneCoord->fPosX - fPosXs;
      fPy = pGeneCoord->fPosY - fPosYs;
      fPz = pGeneCoord->fPosZ - fPosZs;
      fDSqNew = fPx*fPx + fPy*fPy + fPz*fPz;
      fDSqNew += fEpsilon;

      if(fDSq > fDSqNew) { /* Invalid shock */
  iInvalidSheet++;
  pBlue->isFlagValid = FALSE;
  fprintf(stderr,
    "\tShock %d: Gene=(%d,%d), Buck %d , InitType = %d , Itera = %d\n",
    i, iGeneA, iGeneB, pBlue->iBuckXYZ, pBlue->isInitType,
    pBlue->isItera);
  fprintf(stderr, "\t\tRadSq = %lf > %lf = DistSq to Gene %d, Buck %d\n",
    fDSq, fDSqNew, j, pGene->iBuckXYZ);
  break;
      }

    }
  }

  fprintf(stderr, "\tFound %d INVALID sheets\n", iInvalidSheet);
  fprintf(stderr, "\tOut of a total of %d sheets.\n", iNumBlue);

  return;
}
#endif
#endif
/*------------------------------------------------------------*\
 * Function: LinkRedToGreen
 *
 * Usage: For each Red shock, check if it has missing links.
 *    If so, finds which triplet of genes is unlinked, and
 *    then seeks missing Green shock.
 *
\*------------------------------------------------------------*/
#if 0
int
LinkRedToGreen(ShockScaffold *pScaffold, InputData *pInData,
         int *pLookUp, Dimension *pDim)
{
  char    *pcFName = "LinkRedToGreen";
  register int  i, j;
  int    iNumRed, iGeneA,iGeneB,iGeneC,iGeneD, iGreenA,iGreenB,iGreenC;
  int    iSetABC,iSetACD,iSetABD,iSetBCD, iGeneAA,iGeneAB,iGeneAC, iSet;
  int    iSetA,iSetB,iSetC,iSetD, iGeneBA,iGeneBB,iGeneBC, iGreenD;
  int    iFlag, iNumProxi, iGreen, iNewA,iNewB,iNewC, iNumEnds;
  int    iGreenAddRed, iRedAddGreen, iRedAddGreenNotFound;
  int    iGeneCA,iGeneCB,iGeneCC;
  int    iPos,iPosXs,iPosYs,iPosZs, iNewGreen;
  int    iXdim, iXmin,iYmin,iZmin, iXmax,iYmax,iZmax;
  int    iSliceSize;
  int    *piGreen;
  float    fDX,fDY,fDZ, fPosXs,fPosYs,fPosZs, fDSq;
  double  dVertexA[3],dVertexB[3],dVertexC[3],dCircum[3];
  ShockVertex  *pRed;
  ShockCurve  *pGreen, *pGreenA, *pGreenB, *pGreenC, *pNewGreen;
  InputData  *pGeneA, *pGeneB, *pGeneC, *pGeneD;
  Pt3dCoord  *pGeneAcoord, *pGeneBcoord, *pGeneCcoord, *pGeneDcoord;
  ShockData  *pShockData;

  /* Preliminaries */

  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }
  pShockData->iGene[3] = IDLE;

  fprintf(stderr, "MESG(%s): ...\n", pcFName);
  
  iXdim = pDim->iXdim;
  iXmin = iYmin = iZmin = 0;
  iXmax = pDim->iXdim - 1;
  iYmax = pDim->iYdim - 1;
  iZmax = pDim->iZdim - 1;
  iSliceSize = pDim->iSliceSize;

  iGreenAddRed = iRedAddGreen = iRedAddGreenNotFound = 0;
  iNewGreen = 0;

  iNumRed = pScaffold->iNumRedLast;
  pRed = pScaffold->pRed-1;

  for(i = 0; i < iNumRed; i++) {
    pRed++;
    
    iGreenC = pRed->iGreen[2];
    iGreenD = pRed->iGreen[3];

    if(iGreenC > -1 && iGreenD > -1) continue; /* Fully linked */

    iGreenA = pRed->iGreen[0];
    iGreenB = pRed->iGreen[1];

    iGeneA = pRed->iGene[0];
    iGeneB = pRed->iGene[1];
    iGeneC = pRed->iGene[2];
    iGeneD = pRed->iGene[3];

    /* -- Find which face(s)/triplet(s) of the Tet are not linked -- */

    iSetABC = iSetACD = iSetABD = iSetBCD = FALSE;

    pGreenA = (pScaffold->pGreen)+iGreenA;

    iGeneAA = pGreenA->iGene[0];
    iGeneAB = pGreenA->iGene[1];
    iGeneAC = pGreenA->iGene[2];

    iSetA = iSetB = iSetC = iSetD = FALSE;

    if(iGeneAA == iGeneA || iGeneAB == iGeneA || iGeneAC == iGeneA) iSetA = TRUE;
    if(iGeneAA == iGeneB || iGeneAB == iGeneB || iGeneAC == iGeneB) iSetB = TRUE;
    if(iGeneAA == iGeneC || iGeneAB == iGeneC || iGeneAC == iGeneC) iSetC = TRUE;
    if(iGeneAA == iGeneD || iGeneAB == iGeneD || iGeneAC == iGeneD) iSetD = TRUE;
    iSet = iSetA + iSetB + iSetC + iSetD;
    if(iSet != 3) {
      fprintf(stderr,
        "ERROR(%s): 3 srcs (%d,%d,%d) of Green(A) Shock %d\n",
        pcFName, iGeneAA, iGeneAB, iGeneAC, iGreenA);
      fprintf(stderr, "\tdo not match srcs (%d,%d,%d,%d) of Red shock %d\n",
        iGeneA,iGeneB,iGeneC,iGeneD, i);
      return(FALSE);
    }

    iSet -= iSetD;
    if(iSet == 3) iSetABC = TRUE;
    else {
      iSet -= iSetB;
      iSet += iSetD;
      if(iSet == 3) iSetACD = TRUE;
      else {
  iSet -= iSetC;
  iSet += iSetB;
  if(iSet == 3) iSetABD = TRUE;
  else {
    iSet -= iSetA;
    iSet += iSetC;
    if(iSet == 3) iSetBCD = TRUE;
    else {
      fprintf(stderr,
        "ERROR(%s): No triplet for Red = %d, GreenA = %d\n",
        pcFName, i, iGreenA);
      return(FALSE);
    }
  }
      }
    }

    pGreenB = (pScaffold->pGreen)+iGreenB;

    iGeneBA = pGreenB->iGene[0];
    iGeneBB = pGreenB->iGene[1];
    iGeneBC = pGreenB->iGene[2];

    iSetA = iSetB = iSetC = iSetD = FALSE;

    if(iGeneBA == iGeneA || iGeneBB == iGeneA || iGeneBC == iGeneA) iSetA = TRUE;
    if(iGeneBA == iGeneB || iGeneBB == iGeneB || iGeneBC == iGeneB) iSetB = TRUE;
    if(iGeneBA == iGeneC || iGeneBB == iGeneC || iGeneBC == iGeneC) iSetC = TRUE;
    if(iGeneBA == iGeneD || iGeneBB == iGeneD || iGeneBC == iGeneD) iSetD = TRUE;
    iSet = iSetA + iSetB + iSetC + iSetD;
    if(iSet != 3) {
      fprintf(stderr,
        "ERROR(%s): 3 srcs (%d,%d,%d) of Green(B) Shock %d\n",
        pcFName, iGeneBA, iGeneBB, iGeneBC, iGreenA);
      fprintf(stderr, "\tdo not match srcs (%d,%d,%d,%d) of Red shock %d\n",
        iGeneA,iGeneB,iGeneC,iGeneD, i);
      return(FALSE);
    }

    iSet -= iSetD;
    if(iSet == 3) iSetABC = TRUE;
    else {
      iSet -= iSetB;
      iSet += iSetD;
      if(iSet == 3) iSetACD = TRUE;
      else {
  iSet -= iSetC;
  iSet += iSetB;
  if(iSet == 3) iSetABD = TRUE;
  else {
    iSet -= iSetA;
    iSet += iSetC;
    if(iSet == 3) iSetBCD = TRUE;
    else {
      fprintf(stderr,
        "ERROR(%s): No triplet for Red = %d, GreenB = %d\n",
        pcFName, i, iGreenB);
      return(FALSE);
    }
  }
      }
    }
    iSet = iSetABC + iSetACD + iSetABD + iSetBCD;
    if(iSet != 2) {
      fprintf(stderr,
        "ERROR(%s): Can't find triplet for Red = %d, GreenB = %d\n",
        pcFName, i, iGreenB);
      return(FALSE);
    }

    if(iGreenC < 0) { /* Not linked yet: Find missing link */

      iFlag = FALSE;
      if(!iSetABC) { /* Fetch the associated Green shock */
  pGeneA = pInData+iGeneA;    /* Sharing GeneA */
  iNumProxi = pGeneA->isNumGreen;
  piGreen = pGeneA->piGreenList;
  for(j = 0; j < iNumProxi; j++) {
    iGreen = *piGreen++;
    pGreen = (pScaffold->pGreen)+iGreen;
    iNewA = pGreen->iGene[0];
    iNewB = pGreen->iGene[1];
    iNewC = pGreen->iGene[2];
    iSetB = iSetC = FALSE;
    if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC))
      iSetB = TRUE;
    if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
      iSetC = TRUE;
    iSet = iSetB + iSetC;
    if(iSet > 1) { /* Found the ABC Green shock */
      iFlag = TRUE;
      pRed->iGreen[2] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      iNumEnds = pGreen->isNumEndPts;
      if(iNumEnds > 1) {
        fprintf(stderr,
          "WARNING(%s): Green %d already has %d endpts\n",
          pcFName, iGreen, iNumEnds);
        fprintf(stderr, "\twhen linking (ABC) with Red %d\n", i);
      }
      else if(iNumEnds < 1) {
        pGreen->iRed[0] = i;
        pGreen->isNumEndPts = 1;
        iGreenAddRed++;
      }
      else {
        pGreen->iRed[1] = i;
        pGreen->isNumEndPts = 2;
        iGreenAddRed++;
      }
      iSetABC = TRUE;
      iRedAddGreen++;
      break;
    }
  } /* j++ */
  if(!iFlag) { /* Link not found yet: Need to check a 2nd source */
    pGeneB = pInData+iGeneB;    /* Sharing GeneB */
    iNumProxi = pGeneB->isNumGreen;
    piGreen = pGeneB->piGreenList;
    for(j = 0; j < iNumProxi; j++) {
      iGreen = *piGreen++;
      pGreen = (pScaffold->pGreen)+iGreen;
      iNewA = pGreen->iGene[0];
      iNewB = pGreen->iGene[1];
      iNewC = pGreen->iGene[2];
      iSetA = iSetC = FALSE;
      if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
        iSetA = TRUE;
      if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
        iSetC = TRUE;
      iSet = iSetA + iSetC;
      if(iSet > 1) { /* Found the ABC Green shock */
        iFlag = TRUE;
        pRed->iGreen[2] = iGreen;
        if(iGreen == iGreenA || iGreen == iGreenB) {
    fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
      pcFName, iGreen);
    exit(-4);
        }
        iNumEnds = pGreen->isNumEndPts;
        if(iNumEnds > 1) {
    fprintf(stderr,
      "WARNING(%s): Green %d already has %d endpts\n",
      pcFName, iGreen, iNumEnds);
    fprintf(stderr, "\twhen linking (ABC) with Red %d\n", i);
        }
        else if(iNumEnds < 1) {
    pGreen->iRed[0] = i;
    pGreen->isNumEndPts = 1;
    iGreenAddRed++;
        }
        else {
    pGreen->iRed[1] = i;
    pGreen->isNumEndPts = 2;
    iGreenAddRed++;
        }
        iSetABC = TRUE;
        iRedAddGreen++;
        break;
      }
    } /* j++ */
  } /* End of if(!iFlag) */
      } /* End of if(!iSetABC) */

      if(!iSetABD && !iFlag) {
  pGeneA = pInData+iGeneA;    /* Sharing GeneA */
  iNumProxi = pGeneA->isNumGreen;
  piGreen = pGeneA->piGreenList;
  for(j = 0; j < iNumProxi; j++) {
    iGreen = *piGreen++;
    pGreen = (pScaffold->pGreen)+iGreen;
    iNewA = pGreen->iGene[0];
    iNewB = pGreen->iGene[1];
    iNewC = pGreen->iGene[2];
    iSetB = iSetD = FALSE;
    if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC))
      iSetB = TRUE;
    if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
      iSetD = TRUE;
    iSet = iSetB + iSetD;
    if(iSet > 1) { /* Found the ABD Green shock */
      iFlag = TRUE;
      pRed->iGreen[2] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      iNumEnds = pGreen->isNumEndPts;
      if(iNumEnds > 1) {
        fprintf(stderr,
          "WARNING(%s): Green %d already has %d endpts\n",
          pcFName, iGreen, iNumEnds);
        fprintf(stderr, "\twhen linking (ABD) with Red %d\n", i);
      }
      else if(iNumEnds < 1) {
        pGreen->iRed[0] = i;
        pGreen->isNumEndPts = 1;
        iGreenAddRed++;
      }
      else {
        pGreen->iRed[1] = i;
        pGreen->isNumEndPts = 2;
        iGreenAddRed++;
      }
      iSetABD = TRUE;
      iRedAddGreen++;
      break;
    }
  } /* j++ */
  if(!iFlag) { /* Link not found yet: Need to check a 2nd source */
    pGeneB = pInData+iGeneB;    /* Sharing GeneB */
    iNumProxi = pGeneB->isNumGreen;
    piGreen = pGeneB->piGreenList;
    for(j = 0; j < iNumProxi; j++) {
      iGreen = *piGreen++;
      pGreen = (pScaffold->pGreen)+iGreen;
      iNewA = pGreen->iGene[0];
      iNewB = pGreen->iGene[1];
      iNewC = pGreen->iGene[2];
      iSetA = iSetD = FALSE;
      if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
        iSetA = TRUE;
      if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
        iSetD = TRUE;
      iSet = iSetA + iSetD;
      if(iSet > 1) { /* Found the ABD Green shock */
        iFlag = TRUE;
        pRed->iGreen[2] = iGreen;
        if(iGreen == iGreenA || iGreen == iGreenB) {
    fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
      pcFName, iGreen);
    exit(-4);
        }
        iNumEnds = pGreen->isNumEndPts;
        if(iNumEnds > 1) {
    fprintf(stderr,
      "WARNING(%s): Green %d already has %d endpts\n",
      pcFName, iGreen, iNumEnds);
    fprintf(stderr, "\twhen linking (ABD) with Red %d\n", i);
        }
        else if(iNumEnds < 1) {
    pGreen->iRed[0] = i;
    pGreen->isNumEndPts = 1;
    iGreenAddRed++;
        }
        else {
    pGreen->iRed[1] = i;
    pGreen->isNumEndPts = 2;
    iGreenAddRed++;
        }
        iSetABD = TRUE;
        iRedAddGreen++;
        break;
      }
    } /* j++ */
  } /* End of if(!iFlag) */
      } /* End of if(!iSetABD) */

      if(!iSetACD && !iFlag) {
  pGeneA = pInData+iGeneA;    /* Sharing GeneA */
  iNumProxi = pGeneA->isNumGreen;
  piGreen = pGeneA->piGreenList;
  for(j = 0; j < iNumProxi; j++) {
    iGreen = *piGreen++;
    pGreen = (pScaffold->pGreen)+iGreen;
    iNewA = pGreen->iGene[0];
    iNewB = pGreen->iGene[1];
    iNewC = pGreen->iGene[2];
    iSetC = iSetD = FALSE;
    if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
      iSetC = TRUE;
    if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
      iSetD = TRUE;
    iSet = iSetC + iSetD;
    if(iSet > 1) { /* Found the ABD Green shock */
      iFlag = TRUE;
      pRed->iGreen[2] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      iNumEnds = pGreen->isNumEndPts;
      if(iNumEnds > 1) {
        fprintf(stderr,
          "WARNING(%s): Green %d already has %d endpts\n",
          pcFName, iGreen, iNumEnds);
        fprintf(stderr, "\twhen linking (ACD) with Red %d\n", i);
      }
      else if(iNumEnds < 1) {
        pGreen->iRed[0] = i;
        pGreen->isNumEndPts = 1;
        iGreenAddRed++;
      }
      else {
        pGreen->iRed[1] = i;
        pGreen->isNumEndPts = 2;
        iGreenAddRed++;
      }
      iSetACD = TRUE;
      iRedAddGreen++;
      break;
    }
  } /* j++ */
  if(!iFlag) { /* Link not found yet: Need to check a 2nd source */
    pGeneC = pInData+iGeneC;    /* Sharing GeneC */
    iNumProxi = pGeneC->isNumGreen;
    piGreen = pGeneC->piGreenList;
    for(j = 0; j < iNumProxi; j++) {
      iGreen = *piGreen++;
      pGreen = (pScaffold->pGreen)+iGreen;
      iNewA = pGreen->iGene[0];
      iNewB = pGreen->iGene[1];
      iNewC = pGreen->iGene[2];
      iSetA = iSetD = FALSE;
      if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
        iSetA = TRUE;
      if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
        iSetD = TRUE;
      iSet = iSetA + iSetD;
      if(iSet > 1) { /* Found the ACD Green shock */
        iFlag = TRUE;
        pRed->iGreen[2] = iGreen;
        if(iGreen == iGreenA || iGreen == iGreenB) {
    fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
      pcFName, iGreen);
    exit(-4);
        }
        iNumEnds = pGreen->isNumEndPts;
        if(iNumEnds > 1) {
    fprintf(stderr,
      "WARNING(%s): Green %d already has %d endpts\n",
      pcFName, iGreen, iNumEnds);
    fprintf(stderr, "\twhen linking (ACD) with Red %d\n", i);
        }
        else if(iNumEnds < 1) {
    pGreen->iRed[0] = i;
    pGreen->isNumEndPts = 1;
    iGreenAddRed++;
        }
        else {
    pGreen->iRed[1] = i;
    pGreen->isNumEndPts = 2;
    iGreenAddRed++;
        }
        iSetACD = TRUE;
        iRedAddGreen++;
        break;
      }
    } /* j++ */
  } /* End of if(!iFlag) */
      } /* End of if(!iSetACD) */

      if(!iSetBCD && !iFlag) {
  pGeneB = pInData+iGeneB;    /* Sharing GeneB */
  iNumProxi = pGeneB->isNumGreen;
  piGreen = pGeneB->piGreenList;
  for(j = 0; j < iNumProxi; j++) {
    iGreen = *piGreen++;
    pGreen = (pScaffold->pGreen)+iGreen;
    iNewA = pGreen->iGene[0];
    iNewB = pGreen->iGene[1];
    iNewC = pGreen->iGene[2];
    iSetC = iSetD = FALSE;
    if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
      iSetC = TRUE;
    if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
      iSetD = TRUE;
    iSet = iSetC + iSetD;
    if(iSet > 1) { /* Found the BCD Green shock */
      iFlag = TRUE;
      pRed->iGreen[2] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      iNumEnds = pGreen->isNumEndPts;
      if(iNumEnds > 1) {
        fprintf(stderr,
          "WARNING(%s): Green %d already has %d endpts\n",
          pcFName, iGreen, iNumEnds);
        fprintf(stderr, "\twhen linking (BCD) with Red %d\n", i);
      }
      else if(iNumEnds < 1) {
        pGreen->iRed[0] = i;
        pGreen->isNumEndPts = 1;
        iGreenAddRed++;
      }
      else {
        pGreen->iRed[1] = i;
        pGreen->isNumEndPts = 2;
        iGreenAddRed++;
      }
      iSetBCD = TRUE;
      iRedAddGreen++;
      break;
    }
  } /* j++ */
  if(!iFlag) { /* Link not found yet: Need to check a 2nd source */
    pGeneC = pInData+iGeneC;    /* Sharing GeneC */
    iNumProxi = pGeneC->isNumGreen;
    piGreen = pGeneC->piGreenList;
    for(j = 0; j < iNumProxi; j++) {
      iGreen = *piGreen++;
      pGreen = (pScaffold->pGreen)+iGreen;
      iNewA = pGreen->iGene[0];
      iNewB = pGreen->iGene[1];
      iNewC = pGreen->iGene[2];
      iSetB = iSetD = FALSE;
      if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC))
        iSetB = TRUE;
      if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
        iSetD = TRUE;
      iSet = iSetB + iSetD;
      if(iSet > 1) { /* Found the BCD Green shock */
        iFlag = TRUE;
        pRed->iGreen[2] = iGreen;
        if(iGreen == iGreenA || iGreen == iGreenB) {
    fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
      pcFName, iGreen);
    exit(-4);
        }
        iNumEnds = pGreen->isNumEndPts;
        if(iNumEnds > 1) {
    fprintf(stderr,
      "WARNING(%s): Green %d already has %d endpts\n",
      pcFName, iGreen, iNumEnds);
    fprintf(stderr, "\twhen linking (BCD) with Red %d\n", i);
        }
        else if(iNumEnds < 1) {
    pGreen->iRed[0] = i;
    pGreen->isNumEndPts = 1;
    iGreenAddRed++;
        }
        else {
    pGreen->iRed[1] = i;
    pGreen->isNumEndPts = 2;
    iGreenAddRed++;
        }
        iSetBCD = TRUE;
        iRedAddGreen++;
        break;
      }
    } /* j++ */
  } /* End of if(!iFlag) */
      } /* End of if(!iSetBCD) */

      if(!iFlag) {
  iRedAddGreenNotFound++;
      }

    } /* End of if(iGreenC < 0) */
    else { /* GreenC is set : Find face/triplet */
      pGreenC = (pScaffold->pGreen)+iGreenC;

      iGeneCA = pGreenC->iGene[0];
      iGeneCB = pGreenC->iGene[1];
      iGeneCC = pGreenC->iGene[2];

      iSetA = iSetB = iSetC = iSetD = FALSE;

      if(iGeneCA == iGeneA || iGeneCB == iGeneA || iGeneCC == iGeneA) iSetA = TRUE;
      if(iGeneCA == iGeneB || iGeneCB == iGeneB || iGeneCC == iGeneB) iSetB = TRUE;
      if(iGeneCA == iGeneC || iGeneCB == iGeneC || iGeneCC == iGeneC) iSetC = TRUE;
      if(iGeneCA == iGeneD || iGeneCB == iGeneD || iGeneCC == iGeneD) iSetD = TRUE;
      iSet = iSetA + iSetB + iSetC + iSetD;
      if(iSet != 3) {
  fprintf(stderr,
    "ERROR(%s): 3 srcs (%d,%d,%d) of Green(C) Shock %d\n",
    pcFName, iGeneCA, iGeneCB, iGeneCC, iGreenA);
  fprintf(stderr, "\tdo not match srcs (%d,%d,%d,%d) of Red shock %d\n",
    iGeneA,iGeneB,iGeneC,iGeneD, i);
  return(FALSE);
      }

      iSet -= iSetD;
      if(iSet == 3) iSetABC = TRUE;
      else {
  iSet -= iSetB;
  iSet += iSetD;
  if(iSet == 3) iSetACD = TRUE;
  else {
    iSet -= iSetC;
    iSet += iSetB;
    if(iSet == 3) iSetABD = TRUE;
    else {
      iSet -= iSetA;
      iSet += iSetC;
      if(iSet == 3) iSetBCD = TRUE;
      else {
        fprintf(stderr,
          "ERROR(%s): No triplet for Red = %d, GreenC = %d\n",
          pcFName, i, iGreenC);
        return(FALSE);
      }
    }
  }
      }
      iSet = iSetABC + iSetACD + iSetABD + iSetBCD;
      if(iSet != 3) {
  fprintf(stderr,
    "ERROR(%s): Can't find triplet for Red = %d, GreenC = %d\n",
    pcFName, i, iGreenB);
  return(FALSE);
      }
    } /* End of if(iGreenC > -1) */

    /* ----- Check 4th Green slot ----- */

    iGreenC = pRed->iGreen[3];
    if(iGreenD < 0) { /* Link last Green slot */

      iFlag = FALSE;
      if(!iSetABC) { /* Fetch the associated Green shock */
  pGeneA = pInData+iGeneA;    /* Sharing GeneA */
  iNumProxi = pGeneA->isNumGreen;
  piGreen = pGeneA->piGreenList;
  for(j = 0; j < iNumProxi; j++) {
    iGreen = *piGreen++;
    pGreen = (pScaffold->pGreen)+iGreen;
    iNewA = pGreen->iGene[0];
    iNewB = pGreen->iGene[1];
    iNewC = pGreen->iGene[2];
    iSetB = iSetC = FALSE;
    if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC))
      iSetB = TRUE;
    if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
      iSetC = TRUE;
    iSet = iSetB + iSetC;
    if(iSet > 1) { /* Found the ABC Green shock */
      iFlag = TRUE;
      pRed->iGreen[3] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      iNumEnds = pGreen->isNumEndPts;
      if(iNumEnds > 1) {
        fprintf(stderr,
          "WARNING(%s): Green %d already has %d endpts\n",
          pcFName, iGreen, iNumEnds);
        fprintf(stderr, "\twhen linking (ABC) with Red %d\n", i);
      }
      else if(iNumEnds < 1) {
        pGreen->iRed[0] = i;
        pGreen->isNumEndPts = 1;
        iGreenAddRed++;
      }
      else {
        pGreen->iRed[1] = i;
        pGreen->isNumEndPts = 2;
        iGreenAddRed++;
      }
      iSetABC = TRUE;
      iRedAddGreen++;
      break;
    }
  } /* j++ */
  if(!iFlag) { /* Link not found yet: Need to check a 2nd source */
    pGeneB = pInData+iGeneB;    /* Sharing GeneB */
    iNumProxi = pGeneB->isNumGreen;
    piGreen = pGeneB->piGreenList;
    for(j = 0; j < iNumProxi; j++) {
      iGreen = *piGreen++;
      pGreen = (pScaffold->pGreen)+iGreen;
      iNewA = pGreen->iGene[0];
      iNewB = pGreen->iGene[1];
      iNewC = pGreen->iGene[2];
      iSetA = iSetC = FALSE;
      if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
        iSetA = TRUE;
      if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
        iSetC = TRUE;
      iSet = iSetA + iSetC;
      if(iSet > 1) { /* Found the ABC Green shock */
        iFlag = TRUE;
        pRed->iGreen[3] = iGreen;
        if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
    fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
      pcFName, iGreen);
    exit(-4);
        }
        iNumEnds = pGreen->isNumEndPts;
        if(iNumEnds > 1) {
    fprintf(stderr,
      "WARNING(%s): Green %d already has %d endpts\n",
      pcFName, iGreen, iNumEnds);
    fprintf(stderr, "\twhen linking (ABC) with Red %d\n", i);
        }
        else if(iNumEnds < 1) {
    pGreen->iRed[0] = i;
    pGreen->isNumEndPts = 1;
    iGreenAddRed++;
        }
        else {
    pGreen->iRed[1] = i;
    pGreen->isNumEndPts = 2;
    iGreenAddRed++;
        }
        iSetABC = TRUE;
        iRedAddGreen++;
        break;
      }
    } /* j++ */
  } /* End of if(!iFlag) */

  if(!iFlag) { /* Last link not found: Try creating New Green Shock */

    iFlag = TRUE;
    pGeneA = pInData+iGeneA;
    pGeneAcoord = &(pGeneA->Sample);
    dVertexA[0] = (double) pGeneAcoord->fPosX;
    dVertexA[1] = (double) pGeneAcoord->fPosY;
    dVertexA[2] = (double) pGeneAcoord->fPosZ;

    pGeneB = pInData+iGeneB;
    pGeneBcoord = &(pGeneB->Sample);
    dVertexB[0] = (double) pGeneBcoord->fPosX;
    dVertexB[1] = (double) pGeneBcoord->fPosY;
    dVertexB[2] = (double) pGeneBcoord->fPosZ;

    pGeneC = pInData+iGeneC;
    pGeneCcoord = &(pGeneC->Sample);
    dVertexC[0] = (double) pGeneCcoord->fPosX;
    dVertexC[1] = (double) pGeneCcoord->fPosY;
    dVertexC[2] = (double) pGeneCcoord->fPosZ;

    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;
    if(!tri_circum_3d(dVertexA,dVertexB,dVertexC,dCircum)) {
      /* Offshoot to infinity */
      fprintf(stderr,
        "ERROR(%s): New CircumCenter %d out to infinity.\n",
        pcFName, iGreen);
      iFlag = FALSE;
    }

    fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
    fDZ = (float) dCircum[2];

    fPosXs = fDX + (float) dVertexA[0];
    fPosYs = fDY + (float) dVertexA[1];
    fPosZs = fDZ + (float) dVertexA[2];

    iGreen = pScaffold->iGreenActiveLast;
    pNewGreen = (pScaffold->pGreen)+iGreen;

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
      fprintf(stderr, "ERROR(%s): New Green shock %d out of box\n",
        pcFName, iGreen);
      pNewGreen->iBuckXYZ = IDLE;
      iFlag = FALSE;
    }
    else {
      iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
      pNewGreen->iBuckXYZ = *(pLookUp+iPos);
    }
#if HIGH_MEM
    fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
    pNewGreen->fDist2GeneSq = fDSq;
    pNewGreen->CircumTri.fPosX = fPosXs;
    pNewGreen->CircumTri.fPosY = fPosYs;
    pNewGreen->CircumTri.fPosZ = fPosZs;
#endif
    pNewGreen->iGene[0] = iGeneA;
    pNewGreen->iGene[1] = iGeneB;
    pNewGreen->iGene[2] = iGeneC;

    pNewGreen->isFlagValid = TRUE;
    pNewGreen->isInitType = GREEN_FROM_BLUE;

    pNewGreen->iBlue[0] = IDLE;
    pNewGreen->iBlue[1] = IDLE;
    pNewGreen->iBlue[2] = IDLE;

    pNewGreen->isStatus = ACTIVE;

    if(!GetTriCoordOfCurve(pInData, pNewGreen, pShockData)) {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, iGreen);
    }

    if(iFlag) {
      pScaffold->iGreenActiveLast++;
      pRed->iGreen[3] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      pNewGreen->iRed[0] = i;
      pNewGreen->iRed[1] = IDLE;
      pNewGreen->isNumEndPts = 1;
      pNewGreen->isItera = pScaffold->isLastItera;
      iGreenAddRed++;
      iNewGreen++;
    }

  } /* End of if(!iFlag) */
      } /* End of if(!iSetABC) */

      if(!iSetABD && !iFlag) {
  pGeneA = pInData+iGeneA;    /* Sharing GeneA */
  iNumProxi = pGeneA->isNumGreen;
  piGreen = pGeneA->piGreenList;
  for(j = 0; j < iNumProxi; j++) {
    iGreen = *piGreen++;
    pGreen = (pScaffold->pGreen)+iGreen;
    iNewA = pGreen->iGene[0];
    iNewB = pGreen->iGene[1];
    iNewC = pGreen->iGene[2];
    iSetB = iSetD = FALSE;
    if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC))
      iSetB = TRUE;
    if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
      iSetD = TRUE;
    iSet = iSetB + iSetD;
    if(iSet > 1) { /* Found the ABD Green shock */
      iFlag = TRUE;
      pRed->iGreen[3] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      iNumEnds = pGreen->isNumEndPts;
      if(iNumEnds > 1) {
        fprintf(stderr,
          "WARNING(%s): Green %d already has %d endpts\n",
          pcFName, iGreen, iNumEnds);
        fprintf(stderr, "\twhen linking (ABD) with Red %d\n", i);
      }
      else if(iNumEnds < 1) {
        pGreen->iRed[0] = i;
        pGreen->isNumEndPts = 1;
        iGreenAddRed++;
      }
      else {
        pGreen->iRed[1] = i;
        pGreen->isNumEndPts = 2;
        iGreenAddRed++;
      }
      iSetABD = TRUE;
      iRedAddGreen++;
      break;
    }
  } /* j++ */
  if(!iFlag) { /* Link not found yet: Need to check a 2nd source */
    pGeneB = pInData+iGeneB;    /* Sharing GeneB */
    iNumProxi = pGeneB->isNumGreen;
    piGreen = pGeneB->piGreenList;
    for(j = 0; j < iNumProxi; j++) {
      iGreen = *piGreen++;
      pGreen = (pScaffold->pGreen)+iGreen;
      iNewA = pGreen->iGene[0];
      iNewB = pGreen->iGene[1];
      iNewC = pGreen->iGene[2];
      iSetA = iSetD = FALSE;
      if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
        iSetA = TRUE;
      if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
        iSetD = TRUE;
      iSet = iSetA + iSetD;
      if(iSet > 1) { /* Found the ABD Green shock */
        iFlag = TRUE;
        pRed->iGreen[3] = iGreen;
        if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
    fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
      pcFName, iGreen);
    exit(-4);
        }
        iNumEnds = pGreen->isNumEndPts;
        if(iNumEnds > 1) {
    fprintf(stderr,
      "WARNING(%s): Green %d already has %d endpts\n",
      pcFName, iGreen, iNumEnds);
    fprintf(stderr, "\twhen linking (ABD) with Red %d\n", i);
        }
        else if(iNumEnds < 1) {
    pGreen->iRed[0] = i;
    pGreen->isNumEndPts = 1;
    iGreenAddRed++;
        }
        else {
    pGreen->iRed[1] = i;
    pGreen->isNumEndPts = 2;
    iGreenAddRed++;
        }
        iSetABD = TRUE;
        iRedAddGreen++;
        break;
      }
    } /* j++ */
  } /* End of if(!iFlag) */

  if(!iFlag) { /* Last link not found: Try creating New Green Shock */

    iFlag = TRUE;
    pGeneA = pInData+iGeneA;
    pGeneAcoord  = &(pGeneA->Sample);
    dVertexA[0] = (double) pGeneAcoord->fPosX;
    dVertexA[1] = (double) pGeneAcoord->fPosY;
    dVertexA[2] = (double) pGeneAcoord->fPosZ;

    pGeneB = pInData+iGeneB;
    pGeneBcoord  = &(pGeneB->Sample);
    dVertexB[0] = (double) pGeneBcoord->fPosX;
    dVertexB[1] = (double) pGeneBcoord->fPosY;
    dVertexB[2] = (double) pGeneBcoord->fPosZ;

    pGeneD = pInData+iGeneD;
    pGeneDcoord = &(pGeneD->Sample);
    dVertexC[0] = (double) pGeneDcoord->fPosX;
    dVertexC[1] = (double) pGeneDcoord->fPosY;
    dVertexC[2] = (double) pGeneDcoord->fPosZ;

    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;
    if(!tri_circum_3d(dVertexA,dVertexB,dVertexC,dCircum)) {
      /* Offshoot to infinity */
      fprintf(stderr, "ERROR(%s): New CircumCenter %d out to infinity.\n",
        pcFName, iGreen);
      iFlag = FALSE;
    }

    fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
    fDZ = (float) dCircum[2];

    fPosXs = fDX + (float) dVertexA[0];
    fPosYs = fDY + (float) dVertexA[1];
    fPosZs = fDZ + (float) dVertexA[2];

    iGreen = pScaffold->iGreenActiveLast;
    pNewGreen = (pScaffold->pGreen)+iGreen;

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
      fprintf(stderr, "ERROR(%s): New Green shock %d out of box\n",
        pcFName, iGreen);
      pNewGreen->iBuckXYZ = IDLE;
      iFlag = FALSE;
    }
    else {
      iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
      pNewGreen->iBuckXYZ = *(pLookUp+iPos);
    }
#if HIGH_MEM
    fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
    pNewGreen->fDist2GeneSq = fDSq;
    pNewGreen->CircumTri.fPosX = fPosXs;
    pNewGreen->CircumTri.fPosY = fPosYs;
    pNewGreen->CircumTri.fPosZ = fPosZs;
#endif
    pNewGreen->iGene[0] = iGeneA;
    pNewGreen->iGene[1] = iGeneB;
    pNewGreen->iGene[2] = iGeneD;

    pNewGreen->isFlagValid = TRUE;
    pNewGreen->isInitType = GREEN_FROM_BLUE;

    pNewGreen->iBlue[0] = IDLE;
    pNewGreen->iBlue[1] = IDLE;
    pNewGreen->iBlue[2] = IDLE;

    pNewGreen->isStatus = ACTIVE;

    if(!GetTriCoordOfCurve(pInData, pNewGreen, pShockData)) {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, iGreen);
    }

    if(iFlag) {
      pScaffold->iGreenActiveLast++;
      pRed->iGreen[3] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      pNewGreen->iRed[0] = i;
      pNewGreen->iRed[1] = IDLE;
      pNewGreen->isNumEndPts = 1;
      pNewGreen->isItera = pScaffold->isLastItera;
      iGreenAddRed++;
      iNewGreen++;
    }

  } /* End of if(!iFlag) */
      } /* End of if(!iSetABD) */

      if(!iSetACD && !iFlag) {
  pGeneA = pInData+iGeneA;    /* Sharing GeneA */
  iNumProxi = pGeneA->isNumGreen;
  piGreen = pGeneA->piGreenList;
  for(j = 0; j < iNumProxi; j++) {
    iGreen = *piGreen++;
    pGreen = (pScaffold->pGreen)+iGreen;
    iNewA = pGreen->iGene[0];
    iNewB = pGreen->iGene[1];
    iNewC = pGreen->iGene[2];
    iSetC = iSetD = FALSE;
    if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
      iSetC = TRUE;
    if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
      iSetD = TRUE;
    iSet = iSetC + iSetD;
    if(iSet > 1) { /* Found the ABD Green shock */
      iFlag = TRUE;
      pRed->iGreen[3] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      iNumEnds = pGreen->isNumEndPts;
      if(iNumEnds > 1) {
        fprintf(stderr,
          "WARNING(%s): Green %d already has %d endpts\n",
          pcFName, iGreen, iNumEnds);
        fprintf(stderr, "\twhen linking (ACD) with Red %d\n", i);
      }
      else if(iNumEnds < 1) {
        pGreen->iRed[0] = i;
        pGreen->isNumEndPts = 1;
        iGreenAddRed++;
      }
      else {
        pGreen->iRed[1] = i;
        pGreen->isNumEndPts = 2;
        iGreenAddRed++;
      }
      iSetACD = TRUE;
      iRedAddGreen++;
      break;
    }
  } /* j++ */
  if(!iFlag) { /* Link not found yet: Need to check a 2nd source */
    pGeneC = pInData+iGeneC;    /* Sharing GeneC */
    iNumProxi = pGeneC->isNumGreen;
    piGreen = pGeneC->piGreenList;
    for(j = 0; j < iNumProxi; j++) {
      iGreen = *piGreen++;
      pGreen = (pScaffold->pGreen)+iGreen;
      iNewA = pGreen->iGene[0];
      iNewB = pGreen->iGene[1];
      iNewC = pGreen->iGene[2];
      iSetA = iSetD = FALSE;
      if((iGeneA == iNewA) || (iGeneA == iNewB) || (iGeneA == iNewC))
        iSetA = TRUE;
      if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
        iSetD = TRUE;
      iSet = iSetA + iSetD;
      if(iSet > 1) { /* Found the ABD Green shock */
        iFlag = TRUE;
        pRed->iGreen[3] = iGreen;
        if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
    fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
      pcFName, iGreen);
    exit(-4);
        }
        iNumEnds = pGreen->isNumEndPts;
        if(iNumEnds > 1) {
    fprintf(stderr,
      "WARNING(%s): Green %d already has %d endpts\n",
      pcFName, iGreen, iNumEnds);
    fprintf(stderr, "\twhen linking (ACD) with Red %d\n", i);
        }
        else if(iNumEnds < 1) {
    pGreen->iRed[0] = i;
    pGreen->isNumEndPts = 1;
    iGreenAddRed++;
        }
        else {
    pGreen->iRed[1] = i;
    pGreen->isNumEndPts = 2;
    iGreenAddRed++;
        }
        iSetACD = TRUE;
        iRedAddGreen++;
        break;
      }
    } /* j++ */
  } /* End of if(!iFlag) */

  if(!iFlag) { /* Last link not found: Try creating New Green Shock */

    iFlag = TRUE;
    pGeneA = pInData+iGeneA;
    pGeneAcoord = &(pGeneA->Sample);
    dVertexA[0] = (double) pGeneAcoord->fPosX;
    dVertexA[1] = (double) pGeneAcoord->fPosY;
    dVertexA[2] = (double) pGeneAcoord->fPosZ;

    pGeneB = pInData+iGeneC;
    pGeneBcoord = &(pGeneB->Sample);
    dVertexB[0] = (double) pGeneBcoord->fPosX;
    dVertexB[1] = (double) pGeneBcoord->fPosY;
    dVertexB[2] = (double) pGeneBcoord->fPosZ;

    pGeneD = pInData+iGeneD;
    pGeneDcoord = &(pGeneD->Sample);
    dVertexC[0] = (double) pGeneDcoord->fPosX;
    dVertexC[1] = (double) pGeneDcoord->fPosY;
    dVertexC[2] = (double) pGeneDcoord->fPosZ;

    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;
    if(!tri_circum_3d(dVertexA,dVertexB,dVertexC,dCircum)) {
      /* Offshoot to infinity */
      fprintf(stderr, "ERROR(%s): New CircumCenter %d out to infinity.\n",
        pcFName, iGreen);
      iFlag = FALSE;
    }

    fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
    fDZ = (float) dCircum[2];

    fPosXs = fDX + (float) dVertexA[0];
    fPosYs = fDY + (float) dVertexA[1];
    fPosZs = fDZ + (float) dVertexA[2];

    iGreen = pScaffold->iGreenActiveLast;
    pNewGreen = (pScaffold->pGreen)+iGreen;

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
      fprintf(stderr, "ERROR(%s): New Green shock %d out of box\n",
        pcFName, iGreen);
      pNewGreen->iBuckXYZ = IDLE;
      iFlag = FALSE;
    }
    else {
      iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
      pNewGreen->iBuckXYZ = *(pLookUp+iPos);
    }
#if HIGH_MEM
    fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
    pNewGreen->fDist2GeneSq = fDSq;
    pNewGreen->CircumTri.fPosX = fPosXs;
    pNewGreen->CircumTri.fPosY = fPosYs;
    pNewGreen->CircumTri.fPosZ = fPosZs;
#endif
    pNewGreen->iGene[0] = iGeneA;
    pNewGreen->iGene[1] = iGeneC;
    pNewGreen->iGene[2] = iGeneD;

    pNewGreen->isFlagValid = TRUE;
    pNewGreen->isInitType = GREEN_FROM_BLUE;

    pNewGreen->iBlue[0] = IDLE;
    pNewGreen->iBlue[1] = IDLE;
    pNewGreen->iBlue[2] = IDLE;

    pNewGreen->isStatus = ACTIVE;

    if(!GetTriCoordOfCurve(pInData, pNewGreen, pShockData)) {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, iGreen);
    }

    if(iFlag) {
      pScaffold->iGreenActiveLast++;
      pRed->iGreen[3] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      pNewGreen->iRed[0] = i;
      pNewGreen->iRed[1] = IDLE;
      pNewGreen->isNumEndPts = 1;
      pNewGreen->isItera = pScaffold->isLastItera;
      iGreenAddRed++;
      iNewGreen++;
    }

  } /* End of if(!iFlag) */
      } /* End of if(!iSetACD) */

      if(!iSetBCD && !iFlag) {
  pGeneB = pInData+iGeneB;    /* Sharing GeneB */
  iNumProxi = pGeneB->isNumGreen;
  piGreen = pGeneB->piGreenList;
  for(j = 0; j < iNumProxi; j++) {
    iGreen = *piGreen++;
    pGreen = (pScaffold->pGreen)+iGreen;
    iNewA = pGreen->iGene[0];
    iNewB = pGreen->iGene[1];
    iNewC = pGreen->iGene[2];
    iSetC = iSetD = FALSE;
    if((iGeneC == iNewA) || (iGeneC == iNewB) || (iGeneC == iNewC))
      iSetC = TRUE;
    if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
      iSetD = TRUE;
    iSet = iSetC + iSetD;
    if(iSet > 1) { /* Found the BCD Green shock */
      iFlag = TRUE;
      pRed->iGreen[3] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      iNumEnds = pGreen->isNumEndPts;
      if(iNumEnds > 1) {
        fprintf(stderr,
          "WARNING(%s): Green %d already has %d endpts\n",
          pcFName, iGreen, iNumEnds);
        fprintf(stderr, "\twhen linking (BCD) with Red %d\n", i);
      }
      else if(iNumEnds < 1) {
        pGreen->iRed[0] = i;
        pGreen->isNumEndPts = 1;
        iGreenAddRed++;
      }
      else {
        pGreen->iRed[1] = i;
        pGreen->isNumEndPts = 2;
        iGreenAddRed++;
      }
      iSetBCD = TRUE;
      iRedAddGreen++;
      break;
    }
  } /* j++ */
  if(!iFlag) { /* Link not found yet: Need to check a 2nd source */
    pGeneC = pInData+iGeneC;    /* Sharing GeneC */
    iNumProxi = pGeneC->isNumGreen;
    piGreen = pGeneC->piGreenList;
    for(j = 0; j < iNumProxi; j++) {
      iGreen = *piGreen++;
      pGreen = (pScaffold->pGreen)+iGreen;
      iNewA = pGreen->iGene[0];
      iNewB = pGreen->iGene[1];
      iNewC = pGreen->iGene[2];
      iSetB = iSetD = FALSE;
      if((iGeneB == iNewA) || (iGeneB == iNewB) || (iGeneB == iNewC))
        iSetB = TRUE;
      if((iGeneD == iNewA) || (iGeneD == iNewB) || (iGeneD == iNewC))
        iSetD = TRUE;
      iSet = iSetB + iSetD;
      if(iSet > 1) { /* Found the BCD Green shock */
        iFlag = TRUE;
        pRed->iGreen[3] = iGreen;
        if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
    fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
      pcFName, iGreen);
    exit(-4);
        }
        iNumEnds = pGreen->isNumEndPts;
        if(iNumEnds > 1) {
    fprintf(stderr,
      "WARNING(%s): Green %d already has %d endpts\n",
      pcFName, iGreen, iNumEnds);
    fprintf(stderr, "\twhen linking (BCD) with Red %d\n", i);
        }
        else if(iNumEnds < 1) {
    pGreen->iRed[0] = i;
    pGreen->isNumEndPts = 1;
    iGreenAddRed++;
        }
        else {
    pGreen->iRed[1] = i;
    pGreen->isNumEndPts = 2;
    iGreenAddRed++;
        }
        iSetBCD = TRUE;
        iRedAddGreen++;
        break;
      }
    } /* j++ */
  } /* End of if(!iFlag) */

  if(!iFlag) { /* Last link not found: Try creating New Green Shock */

    iFlag = TRUE;
    pGeneB = pInData+iGeneB;
    pGeneBcoord = &(pGeneB->Sample);
    dVertexA[0] = (double) pGeneBcoord->fPosX;
    dVertexA[1] = (double) pGeneBcoord->fPosY;
    dVertexA[2] = (double) pGeneBcoord->fPosZ;

    pGeneC = pInData+iGeneC;
    pGeneCcoord = &(pGeneC->Sample);
    dVertexB[0] = (double) pGeneCcoord->fPosX;
    dVertexB[1] = (double) pGeneCcoord->fPosY;
    dVertexB[2] = (double) pGeneCcoord->fPosZ;

    pGeneD = pInData+iGeneD;
    pGeneDcoord = &(pGeneD->Sample);
    dVertexC[0] = (double) pGeneDcoord->fPosX;
    dVertexC[1] = (double) pGeneDcoord->fPosY;
    dVertexC[2] = (double) pGeneDcoord->fPosZ;

    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;
    if(!tri_circum_3d(dVertexA,dVertexB,dVertexC,dCircum)) {
      /* Offshoot to infinity */
      fprintf(stderr, "ERROR(%s): New CircumCenter %d out to infinity.\n",
        pcFName, iGreen);
      iFlag = FALSE;
    }

    fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
    fDZ = (float) dCircum[2];

    fPosXs = fDX + (float) dVertexA[0];
    fPosYs = fDY + (float) dVertexA[1];
    fPosZs = fDZ + (float) dVertexA[2];

    iGreen = pScaffold->iGreenActiveLast;
    pNewGreen = (pScaffold->pGreen)+iGreen;

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
      fprintf(stderr, "ERROR(%s): New Green shock %d out of box\n",
        pcFName, iGreen);
      pNewGreen->iBuckXYZ = IDLE;
      iFlag = FALSE;
    }
    else {
      iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
      pNewGreen->iBuckXYZ = *(pLookUp+iPos);
    }
#if HIGH_MEM
    fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
    pNewGreen->fDist2GeneSq = fDSq;
    pNewGreen->CircumTri.fPosX = fPosXs;
    pNewGreen->CircumTri.fPosY = fPosYs;
    pNewGreen->CircumTri.fPosZ = fPosZs;
#endif
    pNewGreen->iGene[0] = iGeneB;
    pNewGreen->iGene[1] = iGeneC;
    pNewGreen->iGene[2] = iGeneD;

    pNewGreen->isFlagValid = TRUE;
    pNewGreen->isInitType = GREEN_FROM_BLUE;

    pNewGreen->iBlue[0] = IDLE;
    pNewGreen->iBlue[1] = IDLE;
    pNewGreen->iBlue[2] = IDLE;

    pNewGreen->isStatus = ACTIVE;

    if(!GetTriCoordOfCurve(pInData, pNewGreen, pShockData)) {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, iGreen);
    }

    if(iFlag) {
      pScaffold->iGreenActiveLast++;
      pRed->iGreen[3] = iGreen;
      if(iGreen == iGreenA || iGreen == iGreenB || iGreen == iGreenC) {
        fprintf(stderr, "ERROR(%s): Green %d already linked.\n",
          pcFName, iGreen);
        exit(-4);
      }
      pNewGreen->iRed[0] = i;
      pNewGreen->iRed[1] = IDLE;
      pNewGreen->isNumEndPts = 1;
      pNewGreen->isItera = pScaffold->isLastItera;
      iGreenAddRed++;
      iNewGreen++;
    }

  } /* End of if(!iFlag) */


      } /* End of if(!iSetBCD) */

      if(!iFlag) { /* Last link not found nor created */
  iRedAddGreenNotFound++;
      }

    } /* End of if(iGreenD < 0) */

  } /* Next Red Shock: i++ */

  fprintf(stderr, "\tAdded %d Green links to Red shocks\n", iGreenAddRed);
  fprintf(stderr, "\t\t%d Red ends to Green shocks\n", iRedAddGreen);
  fprintf(stderr, "\t\tand left %d Red-Green links unspecified.\n",
    iRedAddGreenNotFound);
  fprintf(stderr, "\tCreated %d New Green shocks.\n", iNewGreen);

  return(TRUE);
}
#endif

/*------------------------------------------------------------*\
 * Function: InitScaf
 *
 * Usage: Initialize Scaffold datastructures
 *
\*------------------------------------------------------------*/

void
InitScaf(ShockScaffold *pScaf)
{
  char  *pcFName = "InitScaf";
  int  iMax;

  pScaf->pBlue  = NULL;
  pScaf->pGreen = NULL;
  pScaf->pRed   = NULL;

  iMax = pScaf->isMaxItera;
  if(iMax < 1)
    iMax = pScaf->isMaxItera = 100;

  pScaf->piNumBluePerItera = NULL;
  if((pScaf->piNumBluePerItera = 
      (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for ", pcFName);
    fprintf(stderr, "pScaf->piNumBluePerItera[%d].\n", iMax);
    exit(-2);
  }

  pScaf->piNumGreenPerItera = NULL;
  if((pScaf->piNumGreenPerItera = 
      (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for ", pcFName);
    fprintf(stderr, "pScaffold->piNumGreenPerItera[%d].\n", iMax);
    exit(-2);
  }

  pScaf->piNumGreenActivePerItera = NULL;
  if((pScaf->piNumGreenActivePerItera = 
      (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for ", pcFName);
    fprintf(stderr, "pScaffold->piNumGreenActivePerItera[%d].\n", iMax);
    exit(-2);
  }

  pScaf->piNumGreenPassivePerItera = NULL;
  if((pScaf->piNumGreenPassivePerItera = 
      (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for ", pcFName);
    fprintf(stderr, "pScaffold->piNumGreenPassivePerItera[%d].\n", iMax);
    exit(-2);
  }

  pScaf->piNumRedPerItera = NULL;
  if((pScaf->piNumRedPerItera = 
      (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for ", pcFName);
    fprintf(stderr, "pScaffold->piNumRedPerItera[%d].\n", iMax);
    exit(-2);
  }

  pScaf->piNumShocksPerItera = NULL;
  if((pScaf->piNumShocksPerItera = 
      (int *) calloc(iMax, sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for ", pcFName);
    fprintf(stderr, "pScaffold->piNumShocksPerItera[%d].\n", iMax);
    exit(-2);
  }
  
  pScaf->pfTimePerItera = NULL;
  if((pScaf->pfTimePerItera = 
      (float *) calloc(iMax, sizeof(float))) == NULL) {
    fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for ", pcFName);
    fprintf(stderr, "pScaffold->pfTimePerItera[%d].\n", iMax);
    exit(-2);
  }

  return;
}

/*------------------------------------------------------------*\
 * Function: CheckResults
 *
 * Usage: Goes through the Scaffold data and sets a few parameters.
 *
\*------------------------------------------------------------*/

void
CheckResults(ShockScaffold *pScaf, InputData *pInData, Dimension *pDim)
{
  char    *pcFName = "CheckResults";
  register int  i,j;
  int    iNumGene, iMinBluePerGene,iMinGreenPerGene,iMinRedPerGene;
  int    iMaxBluePerGene,iMaxGreenPerGene,iMaxRedPerGene, iNum;
  int    iNumBlueIsol, iNumBlueGreen, iNumBlueRed, iNumBlue;
  int    iNumCaps, iNumSlivers, iAcute, iObtuse, iRight, iNumGreen;
  int    iNumRegs, iNumRed, iNumNoEnd, iNumOneEnd, iNumTwoEnds;
  int    iNumBlueMiss, iGreenMissBlue, iNumEmptySlot, iGreen, iBlue;
  float    fMin, fMax, fTmp;
  InputData  *pGene;
  ShockSheet  *pBlue;
  ShockCurve  *pGreen;
  ShockVertex  *pRed;

  fprintf(stderr, "MESG(%s):...\n", pcFName);

  /* Go through all generators */

  iMinBluePerGene = iMinGreenPerGene = iMinRedPerGene = 100000;
  iMaxBluePerGene = iMaxGreenPerGene = iMaxRedPerGene = 0;

  iNumGene = pDim->iNum3dPts;
  pGene = pInData-1;
  for(i = 0; i < iNumGene; i++) {
    pGene++;
    iNum = pGene->isNumBlue;
    if(iNum > iMaxBluePerGene) iMaxBluePerGene = iNum;
    if(iNum < iMinBluePerGene) iMinBluePerGene = iNum;

    iNum = pGene->isNumGreen;
    if(iNum > iMaxGreenPerGene) iMaxGreenPerGene = iNum;
    if(iNum < iMinGreenPerGene) iMinGreenPerGene = iNum;

    iNum = pGene->isNumRed;
    if(iNum > iMaxRedPerGene) iMaxRedPerGene = iNum;
    if(iNum < iMinRedPerGene) iMinRedPerGene = iNum;
  }

  pScaf->iMinBluePerGene = iMinBluePerGene;
  pScaf->iMaxBluePerGene = iMaxBluePerGene;
  pScaf->iMinGreenPerGene = iMinGreenPerGene;
  pScaf->iMaxGreenPerGene = iMaxGreenPerGene;
  pScaf->iMinRedPerGene = iMinRedPerGene;
  pScaf->iMaxRedPerGene = iMaxRedPerGene;

  fprintf(stderr, "\tMin/Max Shock Data on %d Generators:\n", iNumGene);
  fprintf(stderr, "\t\tBlue : %d - %d\n", iMinBluePerGene, iMaxBluePerGene);
  fprintf(stderr, "\t\tGreen: %d - %d\n", iMinGreenPerGene, iMaxGreenPerGene);
  fprintf(stderr, "\t\tRed  : %d - %d\n", iMinRedPerGene, iMaxRedPerGene);

  /* Go through all Blue Sheets */

  iNumBlueIsol = iNumBlueGreen = iNumBlueRed = iNumBlueMiss = 0;

  iNumBlue = pScaf->iBlueActiveLast;
  pBlue = pScaf->pBlue-1;
  for(i = 0; i < iNumBlue; i++) {
    pBlue++;
    
    switch(pBlue->isInitType) {
    case BLUE_SUB:
    case BLUE_INIT:
      iNumBlueIsol++;
      break;

    case BLUE_FROM_GREENII:
      iNumBlueGreen++;
      break;

    case BLUE_FROM_REDIII:
      iNumBlueRed++;
      break;

    case BLUE_MISS:
      iNumBlueMiss++;
      break;

    default:
      fprintf(stderr, "WARNING(%s): Wrong Init Type for Blue shock %d\n",
        pcFName, i);
    }

  } /* Next Blue sheet: i++ */

  pScaf->iNumBlueFromGreen = iNumBlueGreen;
  pScaf->iNumBlueFromRed = iNumBlueRed;
  
  fprintf(stderr, "\tInit types on %d Blue Sheets:\n", iNumBlue);
  fprintf(stderr,"\t\tIsolated = %d , from Greens = %d, from Reds = %d ,\n",
    iNumBlueIsol, iNumBlueGreen, iNumBlueRed);
  fprintf(stderr,"\t\tMissed (added) = %d\n", iNumBlueMiss);

  /* Go through all Green Curves */

  iNumBlueGreen = iNumCaps = iNumSlivers = iNumEmptySlot = 0;
  iAcute = iObtuse = iRight = 0;
  iNumNoEnd = iNumOneEnd = iNumTwoEnds = 0;
  iGreenMissBlue = 0;
#if HIGH_MEM
  fMin = 10000.0;
  fMax = 0.0;
#endif
  iNumGreen = pScaf->iGreenActiveLast;
  pGreen = pScaf->pGreen-1;
  for(i = 0; i < iNumGreen; i++) {
    pGreen++;

#if HIGH_MEM
    fTmp = pGreen->fTriArea;
    if(fTmp < fMin) fMin = fTmp;
    if(fTmp > fMax) fMax = fTmp;
#endif
    switch(pGreen->isInitType) {
    case GREEN_FROM_BLUE:
      iNumBlueGreen++;
      break;
      
    case GREEN_FROM_REDII:
      iNumCaps++;
      break;

    case GREEN_FROM_REDIII:
      iNumSlivers++;
      break;
    }

    switch(pGreen->isType) {
    case GREEN_I:
      iAcute++;
      break;

    case GREEN_II:
      iObtuse++;
      break;

    case GREEN_III:
      iRight++;
      break;
    }

    switch(pGreen->isNumEndPts) {
    case 0:
      iNumNoEnd++;
      break;
    case 1:
      iNumOneEnd++;
      break;
    case 2:
      iNumTwoEnds++;
      break;
    default:
      fprintf(stderr,
        "WARNING(%s): Wrong Number pf ends = %d for Green shock %d\n",
        pcFName, pGreen->isNumEndPts, i);
    }

    if(pGreen->iBlue[0] < 0 || pGreen->iBlue[1] < 0 || pGreen->iBlue[2] < 0) {
      iGreenMissBlue++;
    }

    for(j = 0; j < 3; j++) {
      iBlue = pGreen->iBlue[j];
      if(iBlue < 0) {
  iNumEmptySlot++;
      }
    }

  } /* Next Green Curve: i++ */

  pScaf->iNumGreenFromSheets = iNumBlueGreen;
  pScaf->iNumGreenFromCaps = iNumCaps;
  pScaf->iNumGreenFromSlivers = iNumSlivers;
  pScaf->iNumTriAcute = iAcute;
  pScaf->iNumTriObtuse = iObtuse;
  pScaf->iNumTriRight = iRight;
  pScaf->iNumGreenNoEnd = iNumNoEnd;
  pScaf->iNumGreenOneEnd = iNumOneEnd;
  pScaf->iNumGreenTwoEnds = iNumTwoEnds;

  fprintf(stderr, "\tData on %d Green Curves:\n", iNumGreen);
  fprintf(stderr, "\t\tFrom sheets = %d , Caps = %d , Slivers = %d\n",
    iNumBlueGreen, iNumCaps, iNumSlivers);
  fprintf(stderr, "\t\tAcute = %d , Obtuse = %d , Right = %d\n",
    iAcute, iObtuse, iRight);
  fprintf(stderr, "\t\tNo ends = %d , One end = %d , Two ends = %d\n",
    iNumNoEnd, iNumOneEnd, iNumTwoEnds);
  fprintf(stderr,
    "\t\tEmpty Blue slots = %d (missing sheet backpointers)\n",
    iNumEmptySlot);

#if HIGH_MEM
  pScaf->fTriAreaMin = fMin;
  pScaf->fTriAreaMax = fMax;
  fprintf(stderr, "\t\tTriangle areas: Min = %lf , Max = %.2f\n",
    fMin, fMax);
#endif
  if(iGreenMissBlue > 0) {
    fprintf(stderr, "\t\tGreen curves with empty Blue slots = %d\n",
      iGreenMissBlue);      
  }

  /* Go through all Red Vertices */

  iNumRegs = iNumCaps = iNumSlivers = 0;
#if HIGH_MEM
  fMin = 10000.0;
  fMax = 0.0;
#endif

  iNumEmptySlot = 0;
  iNumRed = pScaf->iNumRedLast;
  pRed = pScaf->pRed-1;
  for(i = 0; i < iNumRed; i++) {
    pRed++;
#if HIGH_MEM
    fTmp = fabs(pRed->fTetVolume);
    if(fTmp < fMin) fMin = fTmp;
    if(fTmp > fMax) fMax = fTmp;
#endif
    switch(pRed->isType) {
    case RED_I:
      iNumRegs++;
      break;

    case RED_II:
      iNumCaps++;
      break;

    case RED_III:
      iNumSlivers++;
      break;
    }

    for(j = 0; j < 4; j++) {
      iGreen = pRed->iGreen[j];
      if(iGreen < 0) {
  iNumEmptySlot++;
      }
    }

  } /* Next Red Vertex: i++ */

  pScaf->iNumTetRegulars = iNumRegs;
  pScaf->iNumTetCaps = iNumCaps;
  pScaf->iNumTetSlivers = iNumSlivers;

  fprintf(stderr, "\tData on %d Red vertices:\n", iNumRed);
  fprintf(stderr, "\t\t%d regular tetrahedra, %d Caps, %d Slivers.\n",
    iNumRegs, iNumCaps, iNumSlivers);
  fprintf(stderr,
    "\t\tEmpty Green slots = %d (missing curve backpointers)\n",
    iNumEmptySlot);
#if HIGH_MEM
  pScaf->fTetVolMin = fMin;
  pScaf->fTetVolMax = fMax;
  fprintf(stderr, "\t\tVolumes: Min = %lf , Max = %.2f\n",
    fMin, fMax);
#endif

  return;
}

/* ---- EoF ---- */
