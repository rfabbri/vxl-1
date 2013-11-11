/*------------------------------------------------------------*\
 * File: fetch_shocks_int.c
 *
 * Functions:
 *  FetchShocksIntAll
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie
 *  #1: Aug. 2003: Upgraded for use with MetaBuckets and
 *    shock validation via voxels.
 *
\*------------------------------------------------------------*/

#include "fetch_shocks_int.h"
#include "fetch_green_shocks.h"
#include "fetch_red_shocks.h"

/*------------------------------------------------------------*\
 * Function: FetchShocksIntAll
 *
 * Usage: Once Isolated Blue shocks (valid pairs) have been found
 *    in each bucket, find other shocks (curves & vertices)
 *    using combined sheet and curve flows: i.e., first,
 *    intersecting sheets (pairing pairs), then intersecting
 *    curves (pairing pairs of pairs).
 *    We only perform one pass of computations, i.e.,
 *    intersecting sheets to find curves, and intersecting
 *    curves to find vertices, only once. The goal is to
 *    rapidly provide additional visibility constraints
 *    for the next stage, when seeking pairings of generators
 *    between buckets; being exhaustive is not necessary at
 *    this stage, and would probably slow things down too much,
 *    if we did recover the few missing additional visibility
 *    constraints (to be explored).
 *
\*------------------------------------------------------------*/

void
FetchShocksIntAll(InputData *pInData, ShockScaffold *pScaf,
      short *pisLookUp, Buckets *pBucksXYZ,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimMBuck, Dimension *pDimAll)
{
  char    *pcFName = "FetchShocksIntAll";
  int    iNum, iTmp, iMax, iNumNewBlue, iNumNewGreen;
  ShockCurve  *pShockCurves;
  ShockVertex  *pShockVertices;

  pScaf->iNumBlueIsolated = pScaf->iNumBlueActive;

  /* -- Compute Circumcenters --> Green Shock candidates -- *\
  \*  Initially, 10 times has many curves as active sheets  */
  iNum = 1 + pScaf->iNumBlueActive * 10;
  pDimMBuck->iMaxGreenShocks = iNum;
  pDimMBuck->iNumShockCurve = 0;
  pShockCurves = NULL;
  if((pShockCurves =
      (ShockCurve *) calloc(iNum, sizeof(ShockCurve))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC on pShockCurves[%d].\n",
      pcFName, iNum);
    exit(-2); }

  /* Get triplets of genes & compute Green nodes (Type I & II) *\
  \*    i.e., valid circumcenters of 3D triangles      */

  pBucksXYZ->iInitCurveLabel = 0;
  pDimMBuck->isFlagPermute = 1;

  FetchGreenShocksInt(pInData, pScaf, pShockCurves,
          pisLookUp, pBucksXYZ, pGeneCoord, pVoxels,
          pDimMBuck, pDimAll);
#if FALSE
  fprintf(stderr, "MESG(%s):\n\tIntersected up to %d sheets.\n",
    pcFName, pScaf->iNumBlueActive);
#endif
  /* Intersected Active Sheets --> Passive */
  /* SetBluePassive(pScaf); */
  /* pScaf->iBluePassiveLast += pScaf->iNumBlueActive;
     pScaf->iNumBlueActive = 0; */
  /* NB: We leave Sheets active at this early stage */

  /* ----- Set scaffold: New Green and Blue shocks ------ *\
   * For Green shocks, allocate more space than valids    *
  \* for up-coming curves from later iterations.    */

  iNum = 1 + pScaf->iNumBlueActive * 6;
  if(iNum < pDimMBuck->iNumShockCurve)
    iNum = pDimMBuck->iNumShockCurve * 2;
  pScaf->pGreen = NULL;
  if((pScaf->pGreen =
      (ShockCurve *) calloc(iNum, sizeof(ShockCurve))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC on pScaf->pGreen[%d].\n",
      pcFName, iNum);
    exit(-2); }
  pScaf->iNumGreenMax = iNum;
  pScaf->iGreenActiveLast = pScaf->iGreenActiveFst = 0;
  pScaf->iNumGreenActive = pScaf->iNumGreenPassive = 0;
  pScaf->iNumGreenNew = 0;

  /* Make sure there is enough space for new sheets */
  iTmp = pScaf->iBlueActiveLast + pScaf->iNumBlueActive;
  iNum = iTmp + pDimMBuck->iNumShockGreen2Blue;
  iMax = pScaf->iNumBlueMax;
  if(iNum >= iMax) {
    iMax = 1 + iNum * 2;
    if((pScaf->pBlue =
  (ShockSheet *) realloc((ShockSheet *) pScaf->pBlue,
             iMax * sizeof(ShockSheet))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): REALLOC on pScaf->pBlue[%d].\n",
        pcFName, iMax);
      exit(-2); }
    pScaf->iNumBlueMax = iMax;
  }

  /* -- Set Scaffold with 1st set of Green curves & New sheets if any -- */

  if(!SetValidCurvesInt(pShockCurves, pScaf, pInData,
      pisLookUp, pBucksXYZ, pDimMBuck)) {
    fprintf(stderr, "ERROR(%s): Can't set curves on scaffold.\n",
      pcFName);
    exit(-2); }
  if(pShockCurves != NULL) {
    free(pShockCurves); pShockCurves = NULL; }

  /* TestShocksBlue(pScaf, pInData, pDim);
     TestShocksGreen(pScaf, pInData, pDim);
     TestShocksBlueValid(pScaf, pInData, pDim);
     TestShocksGreenValid(pScaf, pInData, pDim); */

  /* ---- Compute Tetra-Circumcenters: Shock vertices ---- *\
  \*  Initially, 10 times as many vertices as valid curves */

  pBucksXYZ->iInitVertexLabel = 0;

  if(pScaf->iNumGreenActive > 1) {
    iNum = 1 + pScaf->iNumBlueActive * 4;
    pDimMBuck->iMaxRedShocks = iNum;
    pDimMBuck->iNumShockVertex = 0;
    pShockVertices = NULL;
    if((pShockVertices =
  (ShockVertex *) calloc(iNum, sizeof(ShockVertex))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC failed for pShockVertices[%d].\n",
        pcFName, iNum);
      exit(-3); }

    /* From quadruplets of genes, compute Tet circumcenters */
    /* and Validate CircumSpheres */

    pDimMBuck->isFlagPermute++;
    FetchRedShocks(pInData, pScaf, pShockVertices,
       pisLookUp, pBucksXYZ, pGeneCoord, pVoxels,
       pDimMBuck, pDimAll);


    /* Intersected Active Curves --> Passive */
    /* SetGreenPassive(pScaf);
       pScaf->iGreenPassiveLast += pScaf->iNumGreenActive;
       pScaf->iNumGreenActive = 0; */
    /* NB: We keep all curves active, because we do not do an  *\
    \* exhaustive search (intersections) at this initial stage */

    /* ---- Set Scaffold: New Red, Green and Blue shocks ---- *\
     * For Red shocks, allocate more space than valids for    *
    \* up-coming nodes from later iterations.          */

    iNum = 1 + pScaf->iNumBlueActive * 3;
    if(iNum < pDimMBuck->iNumShockVertex)
      iNum = pDimMBuck->iNumShockVertex * 2;
    pScaf->iNumRedMax  = iNum;
    pScaf->iNumRedNew  = 0;
    pScaf->iNumRedLast = 0;
    /* This is the first time we put Red shocks on Scaffold */
    pScaf->pRed = NULL;
    if((pScaf->pRed =
  (ShockVertex *) calloc(iNum, sizeof(ShockVertex))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC failed for pScaf->pRed[%d].\n",
        pcFName, iNum);
      exit(-2); }

    /* Make sure there is enough space for New Sheets & Curves */

    iNumNewBlue = pDimMBuck->iNumShockRed2Blue;
    iMax = pScaf->iNumBlueMax - 2;
    iTmp = iNumNewBlue + pScaf->iBlueActiveLast;
    iTmp *= 2.0;
    if(iTmp > iMax) {
      iMax = 1 + iTmp * 2;
      if((pScaf->pBlue =
    (ShockSheet *) realloc((ShockSheet *) pScaf->pBlue,
         iMax * sizeof(ShockSheet))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): REALLOC on pScaf->pBlue[%d].\n",
    pcFName, iMax);
  exit(-2); }
      pScaf->iNumBlueMax = iMax;
    }

    iNumNewGreen = pDimMBuck->iNumShockRed2Green;
    iMax = pScaf->iNumGreenMax;
    iTmp = iNumNewGreen + pScaf->iGreenActiveLast;
    if(iTmp >= iMax) {
      iMax = 1 + iTmp * 2;
      if((pScaf->pGreen =
    (ShockCurve *) realloc((ShockCurve *) pScaf->pGreen,
         iMax * sizeof(ShockCurve))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): REALLOC on pScaf->pGreen[%d].\n",
    pcFName, iMax);
  exit(-2); }
      pScaf->iNumGreenMax = iMax;
    }

    /* -- Set Scaffold with new Valid Red vertices -- */


    if(!SetValidVertices(pShockVertices, pScaf, pInData,
       pisLookUp, pBucksXYZ, pGeneCoord, pVoxels,
       pDimMBuck, pDimAll)) {
      fprintf(stderr, "ERROR(%s): Cannot set vertices on scaffold.\n",
        pcFName);
      exit(-2); }
    if(pShockVertices != NULL) {
      free(pShockVertices); pShockVertices = NULL; }

    fprintf(stderr, "MESG(%s): Done\n\tActive: %d Sheets and %d Curves.\n",
      pcFName, pScaf->iNumBlueActive, pScaf->iNumGreenActive);
    fprintf(stderr, "\tPassive: %d Sheets and %d Curves.\n",
      pScaf->iNumBluePassive, pScaf->iNumGreenPassive);
    fprintf(stderr, "\tReds: New = %d , Tail = %d\n",
      pScaf->iNumRedNew, pScaf->iNumRedLast);
  }
  else { /* No Red shocks, but allocate some space for later */

    iNum = pScaf->iBlueActiveLast;
    pScaf->iNumRedMax  = iNum;
    pScaf->iNumRedNew  = 0;
    pScaf->iNumRedLast = 0;
    pScaf->pRed = NULL;
    if((pScaf->pRed =
  (ShockVertex *) calloc(iNum, sizeof(ShockVertex))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC failed for pScaf->pRed[%d].\n",
        pcFName, iNum);
      exit(-2); }

    fprintf(stderr,"\tNo Red shocks, but allocated space for later.\n");
  }

  pScaf->iNumBlueInt  = pScaf->iBlueActiveLast;
  pScaf->iNumGreenInt = pScaf->iGreenActiveLast;
  pScaf->iNumRedInt   = pScaf->iNumRedLast;

  return;
}

/* --- EoF --- */
