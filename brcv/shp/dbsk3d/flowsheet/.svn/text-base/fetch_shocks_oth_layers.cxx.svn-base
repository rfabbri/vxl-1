/*------------------------------------------------------------*\
 * File: fetch_shocks_oth_layers.c
 *
 * Functions:
 *  FetchShocksExtOth
 *  FetchShocksExtOthItera
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie
 *  #1: Aug. 2003: Upgraded for use with MetaBuckets and
 *    shock validation via voxels.
 *    Moved out FetchBlueShocksExtOth() to
 *    fetch_pairs_via_layers.c

\*------------------------------------------------------------*/

#include "fetch_shocks_oth_layers.h"
#include "fetch_green_shocks_v2.h"
#include "blue_shocks_utils.h"
#include "fetch_red_shocks_v2.h"
#include "fetch_green_shocks.h"
#include "fetch_red_shocks.h"

/*------------------------------------------------------------*\
 * Function: FetchShocksExtOth
 *
 * Usage: Once all Isolated Blue shocks (valid pairs) have been
 *    found for all surrounding layers of each bucket, find
 *    other remaining shocks (Curves & Vertices & New Sheets)
 *    using combined sheet/curve flow.
 *    Some Curves (green) and Vertices (red) have already
 *    been found: Check for duplicates.
 *
 *    We intersect all existing sheets (passive or active)
 *    and curves in this first iteration, and then set all
 *    these to be passive for good.
 *
\*------------------------------------------------------------*/

void
FetchShocksExtOth(InputData *pInData, ShockScaffold *pScaf,
      short *pisLookUp, Buckets *pBucksXYZ,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimMBuck, Dimension *pDimAll)
{
  char    *pcFName = "FetchShocksExtOth";
  int    iNum, iTmp, iMax, iNumNewBlue, iNumNewGreen;
  ShockCurve  *pShockCurves;
  ShockVertex  *pShockVertices;

  /* -- Compute Circumcenters --> Green Shock candidates -- *\
  \*    Initial guess: 10 times has many curves as sheets   */
  iNum = 1 + pScaf->iNumBlueActive * 10;
  pDimMBuck->iMaxGreenShocks = iNum;
  pDimMBuck->iNumShockCurve = 0;
  pShockCurves = NULL;
  if((pShockCurves =
      (ShockCurve *) calloc(iNum, sizeof(ShockCurve))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC on pShockCurves[%d].\n",
      pcFName, iNum);
    exit(-2); }

  /* ---- Get triplets of Genes and compute Green shocks --- *\
   * (Type I & II) i.e. valid circumcenters of 3D triangles  *
  \*  We intersect ALL existing sheets in this initial step  */

  pDimMBuck->isFlagPermute++;

  if(pScaf->iNumBlueActive > 0) {
    FetchGreenViaBlueFull(pInData, pScaf, pShockCurves,
        pisLookUp, pBucksXYZ, pGeneCoord,
        pVoxels, pDimMBuck, pDimAll);
    /* FetchGreenViaBlue(pInData, pScaf, pShockCurves,
       pisLookUp, pBucksXYZ, pGeneCoord,
       pVoxels, pDimMBuck, pDimAll);
    */
    if(pDimMBuck->iNumShockCurve >= pDimMBuck->iMaxGreenShocks) {
      fprintf(stderr,
        "ERROR(%s): Found %d new curves, but allocated space for %d.\n",
        pcFName, pDimMBuck->iNumShockCurve, pDimMBuck->iMaxGreenShocks);
      exit(-4);
    }
    fprintf(stderr, "MESG(%s):\n\tIntersected %d sheets.\n",
      pcFName, pScaf->iNumBlueActive);
  }
  else {
    fprintf(stderr, "MESG(%s):\n\tNo new (active) Blue sheets ...\n",
      pcFName);
  }

  /* -- Intersected Active Sheets --> Passive -- */
  SetBluePassive(pScaf);
  /* pScaf->iBluePassiveLast += pScaf->iNumBlueActive;
     pScaf->iNumBlueActive = 0; */

  /* ----- Set scaffold: New Green and Blue shocks ------ */
  /* For Green shocks, allocate more space than valids    */
  /* for up-coming curves from iterations */

  iNum = 1 + pDimMBuck->iNumShockCurve;
  iMax = pScaf->iNumGreenMax - 2;
  iTmp = iNum + pScaf->iGreenActiveLast;
  if(iTmp > iMax) {
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

  /* Make sure there is enough space for New Sheets */
  iTmp = pScaf->iBlueActiveLast;
  iNum = iTmp + pDimMBuck->iNumShockGreen2Blue;
  iMax = pScaf->iNumBlueMax - 2;
  if(iNum > iMax) {
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

  /* -- Set Scaffold with new Valid Green curves -- */

  if(!SetValidCurvesExt(pShockCurves, pScaf, pInData,
      pisLookUp, pBucksXYZ, pDimMBuck)) {
    fprintf(stderr, "ERROR(%s): Can't set curves on scaffold.\n",
      pcFName);
    exit(-2); }
  if(pShockCurves != NULL) {
    free(pShockCurves); pShockCurves = NULL; }

  fprintf(stderr, "MESG(%s):\n", pcFName);
  fprintf(stderr, "\tActive shocks: %d sheets, %d curves.\n",
    pScaf->iNumBlueActive, pScaf->iNumGreenActive);
  fprintf(stderr, "\tPassive shocks: %d sheets, %d curves.\n",
    pScaf->iNumBluePassive, pScaf->iNumGreenPassive);

  /* TestShocksBlue(pScaf, pInData, pDimMBuck);
     TestShocksGreen(pScaf, pInData, pDimMBuck); */
  /* CheckInputs(pScaf, pInData, pDimMBuck, 90); */
  /* CheckInputGreen(pScaf, pInData, pDimMBuck, 90); */

  /* ---- Compute Tetra-Circumcenters: Shock nodes ---- *\
  \*  Initial guees: 10 times as many nodes as valid curves */
  iNum = 1 + pScaf->iNumGreenActive * 10;
  pDimMBuck->iMaxRedShocks = iNum;
  pDimMBuck->iNumShockVertex = 0;
  pShockVertices = NULL;
  if((pShockVertices =
      (ShockVertex *) calloc(iNum, sizeof(ShockVertex))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC failed for pShockVertices[%d].\n",
      pcFName, iNum);
    exit(-3); }

  /* From quadruplets of sources, compute Tet circumcenters */
  /* and Validate CircumSpheres */

  pDimMBuck->isFlagPermute++;

  if(pScaf->iNumGreenActive > 0) {
    if(!FetchRedViaGreenFull(pInData, pScaf, pShockVertices,
           pisLookUp, pBucksXYZ, pGeneCoord,
           pVoxels, pDimMBuck, pDimAll)) {
      fprintf(stderr,
        "ERROR(%s): FetchRedViaGreenFull failed.\n",
        pcFName);
      exit(-4); }
  }
  else {
    fprintf(stderr, "MESG(%s):\n\tNo new (active) Green curves ...\n",
      pcFName);
  }
  fprintf(stderr, "MESG(%s):\n\tIntersected %d curves.\n",
    pcFName, pScaf->iNumGreenActive);

  /* -- Intersected Active Curves --> Passive -- */
  SetGreenPassive(pScaf);
  /* pScaf->iGreenPassiveLast += pScaf->iNumGreenActive;
     pScaf->iNumGreenActive = 0; */

  /* ---- Set Scaffold: New Red, Green and Blue shocks ---- *\
   *  For Red shocks, allocate more space than       *
  \*  valids for up-coming nodes from iterations      */

  iNum = 1 + pDimMBuck->iNumShockVertex;
  iMax = pScaf->iNumRedMax - 2;
  iTmp = iNum + pScaf->iNumRedLast;
  if(iTmp > iMax) {
    iMax = 1 + iTmp * 2;
    if((pScaf->pRed =
  (ShockVertex *) realloc((ShockVertex *) pScaf->pRed,
            iMax * sizeof(ShockVertex))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): REALLOC on pScaf->pRed[%d].\n",
        pcFName, iMax);
      exit(-2); }
    pScaf->iNumRedMax = iMax;
  }

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
  iMax = pScaf->iNumGreenMax - 2;
  iTmp = iNumNewGreen + pScaf->iGreenActiveLast;
  if(iTmp > iMax) {
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

  /* -- Set Scaffold with new Valid Red nodes -- */

  if(!SetValidVertices(pShockVertices, pScaf, pInData,
           pisLookUp, pBucksXYZ, pGeneCoord, pVoxels,
           pDimMBuck, pDimAll)) {
    fprintf(stderr, "ERROR(%s): Cannot set nodes on scaffold.\n", pcFName);
    exit(-2); }
  if(pShockVertices != NULL) {
    free(pShockVertices); pShockVertices = NULL; }

  return;
}

/*------------------------------------------------------------*\
 * Function: FetchShocksExtOthItera
 *
 * Usage: Iterative loop, where we exhaust all flows from
 *    ACTIVE Sheets and Curves.
 *
\*------------------------------------------------------------*/

void
FetchShocksExtOthItera(InputData *pInData, ShockScaffold *pScaf,
           short *pisLookUp, Buckets *pBucksXYZ,
           Pt3dCoord *pGeneCoord, Voxel *pVoxels,
           Dimension *pDimMBuck, Dimension *pDimAll)
{
  char    *pcFName = "FetchShocksExtOthItera";
  int    iNum, iTmp, iMax, iNumNewBlue, iNumNewGreen;
  int    iMaxItera, iItera, iNumTotalShocks, iNumNewFlows;
  float    fTmp, fTimeSys, fTimeUser, fMemUse;
  ShockCurve  *pShockCurves;
  ShockVertex  *pShockVertices;
#if SGI_ENV
  struct rusage ru_proc;

  /* Preliminaries */

  if(getrusage(RUSAGE_SELF, &ru_proc) == -1) {
    fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
  }
  fTimeSys  = (float) (SECS(ru_proc.ru_stime));
  fTimeUser = (float) (SECS(ru_proc.ru_utime));
  fMemUse   = (float) (ru_proc.ru_maxrss / 1000.0);
  fTmp = fTimeSys + fTimeUser;
#endif
  /* Total number of shocks (active and passive) before iterations */
  iNumTotalShocks = pScaf->piNumShocksPerItera[0] =
    pScaf->iBlueActiveLast + pScaf->iGreenActiveLast +
    pScaf->iNumRedLast;
  pScaf->piNumBluePerItera[0] = pScaf->iBlueActiveLast;
  pScaf->piNumGreenPerItera[0] = pScaf->iGreenActiveLast;
  pScaf->piNumRedPerItera[0] = pScaf->iNumRedLast;
  pScaf->piNumGreenActivePerItera[0] = pScaf->iNumGreenActive;
  pScaf->piNumGreenPassivePerItera[0] = pScaf->iNumGreenPassive;

  pScaf->iNumRedNew = pScaf->iNumGreenNew = pScaf->iNumBlueNew = 0;
  iNumNewBlue  = pScaf->iNumBlueActive;
  iNumNewGreen = pScaf->iNumGreenActive;
  iNumNewFlows = iNumNewBlue + iNumNewGreen;

  iMaxItera = pScaf->isMaxItera;
  if(iNumNewFlows > 0) {
    fprintf(stderr, "\t----------------------------\n");
    fprintf(stderr, "MESG(%s):\n\tStarting iterations (Max Itera = %d):\n",
      pcFName, iMaxItera);
    fprintf(stderr, "\tActive: Sheets = %d , Curves = %d \n",
      iNumNewBlue, iNumNewGreen);
    fprintf(stderr, "\tPassive: Sheets = %d , Curves = %d \n",
      pScaf->iNumBluePassive, pScaf->iNumGreenPassive);
    fprintf(stderr,
      "\tTotal = %d : Sheets = %d , Curves = %d , Vertices = %d\n",
      iNumTotalShocks, pScaf->iBlueActiveLast,
      pScaf->iGreenActiveLast, pScaf->iNumRedLast);
  }
  else {
    fprintf(stderr, "MESG(%s):\n\tNo iterations needed: No active shocks.\n",
      pcFName);
    return;
  }

  /***** ======== Iterative LOOP ======== ******/

  iItera = 0;
  while(iNumNewFlows > 0 && iItera < iMaxItera) {
    iItera++;
    pScaf->isLastItera = iItera;
    fprintf(stderr, "--------------- Itera %d --------------\n", iItera);
    
    if(iNumNewBlue > 0) {

      /* -- Compute Circumcenters --> Green Shock candidates -- *\
      \*    Initial guess: twice has many curves as before.     */
      /* iNum = 1 + pScaf->iGreenActiveLast * 2; */
      iNum = 10 + 10 * iNumNewBlue;
      pDimMBuck->iMaxGreenShocks = iNum;
      pDimMBuck->iNumShockCurve = 0;
      pShockCurves = NULL;
      if((pShockCurves =
    (ShockCurve *) calloc(iNum, sizeof(ShockCurve))) == NULL) {
  fprintf(stderr, "ERROR(%s): CALLOC on pShockCurves[%d].\n",
    pcFName, iNum);
  exit(-5); }

      /* ---- Get triplets of Genes and compute Green shocks ---- *\
       * (Type I & II) i.e. valid circumcenters of 3D triangles  *
      \*  We intersect Active sheets with other ones.     */

      pDimMBuck->isFlagPermute++;

      if(!FetchGreenViaBlue(pInData, pScaf, pShockCurves,
          pisLookUp, pBucksXYZ, pGeneCoord,
          pVoxels, pDimMBuck, pDimAll)) {
  fprintf(stderr,
    "ERROR(%s): Alloc. space for %d Green shock only.\n",
    pcFName, pDimMBuck->iMaxGreenShocks);
  exit(-4);
      }
      fprintf(stderr, "MESG(%s):\n\tIntersected %d sheets.\n",
        pcFName, pScaf->iNumBlueActive);

      /* -- Intersected Active Sheets --> Passive -- */
      SetBluePassive(pScaf);
      /* pScaf->iBluePassiveLast += pScaf->iNumBlueActive;
   pScaf->iNumBlueActive = 0; */

      /* ----- Set scaffold: New Green and Blue shocks ------ */

      iNum = pDimMBuck->iNumShockCurve;
      iMax = pScaf->iNumGreenMax - 2;
      iTmp = iNum + pScaf->iGreenActiveLast;
      if(iTmp > iMax) {
  iMax = 1 + iTmp + iNum;
  if((pScaf->pGreen =
      (ShockCurve *) realloc((ShockCurve *) pScaf->pGreen,
           iMax * sizeof(ShockCurve))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): REALLOC on pScaf->pGreen[%d].\n",
      pcFName, iMax);
    exit(-2); }
  pScaf->iNumGreenMax = iMax;
      }

      /* Make sure there is enough space for New Sheets */
      iTmp = pScaf->iBlueActiveLast;
      iNum = iTmp + pDimMBuck->iNumShockGreen2Blue;
      iMax = pScaf->iNumBlueMax - 2;
      if(iNum > iMax) {
  iMax = 1 + iNum + iTmp;
  if((pScaf->pBlue =
      (ShockSheet *) realloc((ShockSheet *) pScaf->pBlue,
           iMax * sizeof(ShockSheet))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): REALLOC on pScaf->pBlue[%d].\n",
      pcFName, iMax);
    exit(-2); }
  pScaf->iNumBlueMax = iMax;
      }

      /* -- Set Scaffold with new Valid Green curves -- */

      if(!SetValidCurvesExt(pShockCurves, pScaf, pInData,
          pisLookUp, pBucksXYZ, pDimMBuck)) {
  fprintf(stderr, "ERROR(%s): Can't set curves on scaffold.\n",
    pcFName);
  exit(-2); }
      if(pShockCurves != NULL) {
  free(pShockCurves); pShockCurves = NULL; }

    } /*** End of Sheet Intersection ***/

    iNumNewGreen = pScaf->iNumGreenActive;
    if(iNumNewGreen > 0) {

      /* ---- Compute Tetra-Circumcenters: Shock nodes ---- *\
      \*  Initially, 10 times as many nodes as valid curves */
      /* iNum = 1 + pScaf->iGreenActiveLast * 10; */
      iNum = 10 + 10 * iNumNewGreen;
      pDimMBuck->iMaxRedShocks = iNum;
      pDimMBuck->iNumShockVertex = 0;
      pShockVertices = NULL;
      if((pShockVertices =
    (ShockVertex *) calloc(iNum, sizeof(ShockVertex))) == NULL) {
  fprintf(stderr, "ERROR(%s): CALLOC on pShockVertices[%d].\n",
    pcFName, iNum);
  exit(-5); }

      /* From quadruplets of sources, compute Tet circumcenters */
      /* and Validate CircumSpheres */

      pDimMBuck->isFlagPermute++;

      if(!FetchRedViaGreen(pInData, pScaf, pShockVertices,
         pisLookUp, pBucksXYZ, pGeneCoord,
         pVoxels, pDimMBuck, pDimAll)) {
  fprintf(stderr,
    "ERROR(%s): Alloc. space for %d red shocks only.\n",
    pcFName, pDimMBuck->iMaxRedShocks);
  exit(-5);
      }

      /* -- Intersected Active Curves --> Passive -- */
      SetGreenPassive(pScaf);
      /* pScaf->iGreenPassiveLast += pScaf->iNumGreenActive;
   pScaf->iNumGreenActive = 0; */

      /* ---- Set Scaffold: New Red, Green and Blue shocks ---- */

      iNum = pDimMBuck->iNumShockVertex;
      if(iNum > 0) {
  iMax = pScaf->iNumRedMax - 2;
  iTmp = iNum + pScaf->iNumRedLast;
  if(iTmp > iMax) {
    iMax = iTmp + iNum + 3;
    if((pScaf->pRed =
        (ShockVertex *) realloc((ShockVertex *) pScaf->pRed,
              iMax * sizeof(ShockVertex))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): REALLOC on pScaf->pRed[%d].\n",
        pcFName, iMax);
      exit(-2); }
    pScaf->iNumRedMax = iMax;
  }
  /* Make sure there is enough space for New Sheets & Curves */
  iNumNewBlue = pDimMBuck->iNumShockRed2Blue;
  iMax = pScaf->iNumBlueMax - 2;
  iTmp = iNumNewBlue + pScaf->iBlueActiveLast;
  if(iTmp > iMax) {
    iMax = 3 + iTmp + iNumNewBlue;
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
  iMax = pScaf->iNumGreenMax - 2;
  iTmp = iNumNewGreen + pScaf->iGreenActiveLast;
  if(iTmp > iMax) {
    iMax = 3 + iTmp + iNumNewGreen;
    if((pScaf->pGreen =
        (ShockCurve *) realloc((ShockCurve *) pScaf->pGreen,
             iMax * sizeof(ShockCurve))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): REALLOC on pScaf->pGreen[%d].\n",
        pcFName, iMax);
      exit(-2); }
    pScaf->iNumGreenMax = iMax;
  }

  /* -- Set Scaffold with new Valid Red nodes -- */

  /* pDimMBuck->iNumShockVertex = pScaf->iNumRedLast; */
  /* pDimMBuck->iNumShockVertexV = pScaf->iNumRedLast; */
  /* TestInData(pInData, pDimMBuck); */

  if(!SetValidVertices(pShockVertices, pScaf, pInData, 
           pisLookUp, pBucksXYZ, pGeneCoord,
           pVoxels, pDimMBuck, pDimAll)) {
    fprintf(stderr,
      "ERROR(%s): Cannot set nodes on scaffold.\n", pcFName);
    exit(-2); }
      }
      if(pShockVertices != NULL) {
  free(pShockVertices); pShockVertices = NULL; }
      
    } /*** End of Curve Intersection ***/

    iNumNewBlue  = pScaf->iNumBlueActive;
    iNumNewGreen = pScaf->iNumGreenActive;
    iNumNewFlows = iNumNewBlue + iNumNewGreen;

    fprintf(stderr, "--------------- Itera %d --------------\n", iItera);
    fprintf(stderr,
      "\tSheets: New (Active) = %d , Passive = %d , Total = %d .\n",
      iNumNewBlue, pScaf->iNumBluePassive,
      pScaf->iBlueActiveLast);
    fprintf(stderr,
      "\tCurves: New = %d , Active = %d , Passive = %d , Total = %d .\n",
      iNumNewGreen, pScaf->iNumGreenActive,
      pScaf->iNumGreenPassive, pScaf->iGreenActiveLast);
    fprintf(stderr, "\tVertices: New = %d , Total = %d .\n",
      pScaf->iNumRedNew, pScaf->iNumRedLast);

    /* New total number of shocks (active and passive) on scaffold */
    iNumTotalShocks = pScaf->iBlueActiveLast +
      pScaf->iGreenActiveLast + pScaf->iNumRedLast;
    /* Difference from previous iteration */
    iNum = iNumTotalShocks - pScaf->piNumShocksPerItera[iItera-1];
    pScaf->piNumShocksPerItera[iItera] = iNumTotalShocks;

    fprintf(stderr, "\tTotal = %d , Increment = %d .\n",
      iNumTotalShocks, iNum);

    pScaf->piNumBluePerItera[iItera] = iNumNewBlue;
    pScaf->piNumGreenPerItera[iItera] = iNumNewGreen;
    pScaf->piNumRedPerItera[iItera] = pScaf->iNumRedNew;
    pScaf->piNumGreenActivePerItera[iItera] = pScaf->iNumGreenActive;
    pScaf->piNumGreenPassivePerItera[iItera] = pScaf->iNumGreenPassive;

    pScaf->iNumBlueNew  = 0;
    pScaf->iNumGreenNew = 0;
    pScaf->iNumRedNew   = 0;
#if SGI_ENV
    if(getrusage(RUSAGE_SELF, &ru_proc) == -1) {
      fprintf(stderr, "ERROR(%s): getrusage failed.\n", pcFName);
    }
    fTmp = (float) (SECS(ru_proc.ru_stime)) - fTimeSys;
    fTimeSys = fTmp;
    fTmp = (float) (SECS(ru_proc.ru_utime)) - fTimeUser;
    fTimeUser = fTmp;
    fMemUse   = (float) (ru_proc.ru_maxrss / 1000.0);
    fprintf(stderr,
      "RUSAGE(Itera %d): Sys = %f , User = %f , Mem = %f Meg.\n",
      iItera, fTimeSys, fTimeUser, fMemUse);
    fTmp = fTimeSys + fTimeUser;
    fprintf(stderr, "\t==>Time for Iteration %d: %f secs.\n", iItera, fTmp);
    pScaf->pfTimePerItera[iItera] = fTmp;

    fTimeSys  = (float) (SECS(ru_proc.ru_stime));
    fTimeUser = (float) (SECS(ru_proc.ru_utime));
#endif
  } /*** End of While Loop ***/


  fprintf(stderr, "--------------- END of LOOP --------------\n");
  fprintf(stderr, "MESG(%s): Found a total of:\n", pcFName);    
  fprintf(stderr, "\t%d Blue sheets, %d Green curves, %d Red nodes.\n",
    pScaf->iBluePassiveLast, pScaf->iGreenPassiveLast,
    pScaf->iNumRedLast);
  pScaf->iTotalShocks = pScaf->iBluePassiveLast +
    pScaf->iGreenPassiveLast + pScaf->iNumRedLast;

  return;
}

/* ---- EoF ---- */
