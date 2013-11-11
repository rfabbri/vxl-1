/*------------------------------------------------------------*\
 * File: sheet_flow_mbuck_mres.c
 *
 * Functions:
 *  main: Compute the Shock Scaffold via Sheet and Curve
 *        Flows together with validation driven by bucketing,
 *        in a multi-resolution scheme.
 *
 * History:
 *  #0: October 2001, by F.Leymarie
 *  #1: May 2002: Updated some stuff (message handling mostly).
 *  #2: April-July 2003: Uses MetaBuckets as an input datastructure.
 *  #3: Feb. 2004: Debugging
 *
\*------------------------------------------------------------*/

#include <vcl_cassert.h>

#include "sheet_flow_mbuck_mres.h"
#include "buck_utils.h"
#include "sheet_flow_io_v2.h"
#include "shock_utils.h"
#include "fetch_pairs_internal.h"
#include "blue_shocks_utils.h"
#include "fetch_shocks_int.h"
#include "fetch_shocks_fst_layer.h"
#include "fetch_pairs_via_layers.h"
#include "fetch_shocks_oth_layers.h"
#include "shock_connect.h"

///char  *pcFileNameIn = NULL;
int  iMaxItera = 10;
int  iMemDiv = 10;
int  iFlagRad = FALSE;
int  iNumSubSamples = 1;
int  iFlagSubType = 0;

/*static OPTTABLE optab [] = {
  {"i", STRING, 0, &pcFileNameIn, "Input (*.Bucks.*) FileName Prefix"},
  {"imax", INT, 0, &iMaxItera, "Maximum number of iterations (def. = 10)."},
  {"imem", INT, 0, &iMemDiv,
   "Divisor (integer) for memory consumption (def. = 10)."},
  {"isub", INT, 0, &iNumSubSamples,
   "Number of (sub-)samples for 1st pass (def. = 1; if = 0: skip 1st pass)."},
  {"isubtype", INT, 0, &iFlagSubType,
   "Type of sub-sampling: Random (def. = 0) , Qhull (1)."},
  {(char*)0, (OPT_TYPE)0, (int*)0, (void*)0, (char*)0}
};*/


/*------------------------------------------------------------*\
 * Function: main
 *
 * Usage: Loads user inputs and launches Scaffold computation.
 *    Expects input file written via sort3dpts_space_v3.c
 *    (meta_bucket3d_sgi)
 *
\*------------------------------------------------------------*/

int FreShockFlowSheet (const char* pcFileNameIn)
///int
///main(int argc, char **argv)
{
  char* pcInFile = (char*) pcFileNameIn;
  char    *pcFName = "main(sheet_flow_mbuck_mres)";
  char    *pcTmp, *pcOutFile, cTmp[128]="";
  char    cInFile[128]="", cInFileData[128]="", cOutFile[128]="";
  FILE    *fp1;
  short    *pisLookUp;
  register int  i;
  int    iTmp, iTmp2, iNumGenes, iNumBins,iNumBucks, iWSpaceSize;
  int    iFlag, iMax, iTmpBlue,iTmpGreen,iTmpRed, iNumGeneTot;
  int    iNumMBucks, iNumShocks;
  ///float    fTimeSys,fTimeUser,fMemUse;
  float fTmp;
  InputData  *pInData;
  Buckets  *pBucksXYZ;
  MetaBucket  *pMBuck;
  MetaBuckInfo  *pMBuckInfo;
  MetaBuckTop  *pMBuckTop;
  ShockSheet  *pShockSheets;
  ShockScaffold *pScaf;
  Voxel    *pVoxels;
  Pt3dCoord  *pGeneCoord;

  /*-----------------------------------*\
   * Read in Arguments and set values  *
  \*-----------------------------------*/

  Dimension* pDimAll = (Dimension *) calloc(1, sizeof(Dimension));
  assert (pDimAll);

  Dimension* pDimMBuck = (Dimension *) calloc(1, sizeof(Dimension));
  assert (pDimMBuck);

  ///opt_parse_args(argc, argv, optab);        
  if(pcFileNameIn == NULL){
    fprintf(stderr, "ERROR(%s): Missing input (prefix) filename\n", pcFName);
    fprintf(stderr, "\n\t use -h for more help.\n");
    exit(-1); }
  if(iMaxItera < 1) {
    fprintf(stderr, "ERROR(%s): iMaxItera = %d < 1.\n",
      pcFName, iMaxItera);
    exit(-1); }
  pDimAll->iMaxItera = iMaxItera;
  pDimMBuck->iMaxItera = iMaxItera;
  if(iMemDiv < 1) {
    fprintf(stderr, "ERROR(%s): iMemDiv = %d < 1.\n", pcFName, iMemDiv);
    exit(-1); }
  pDimAll->iMemDiv = iMemDiv;
  pDimMBuck->iMemDiv = iMemDiv;
  if(iNumSubSamples < 0) {
    fprintf(stderr, "ERROR(%s): iNumSubSamples = %d < 0.\n", pcFName,
      iNumSubSamples);
    exit(-1); }
  pDimAll->iNumSubSamples = iNumSubSamples;
  pDimMBuck->iNumSubSamples = iNumSubSamples;
  if(iFlagSubType < 0 || iFlagSubType > 1) {
    fprintf(stderr, "ERROR(%s): iFlagSubType = %d , not 0 or 1.\n",
      pcFName, iFlagSubType);
    exit(-1); }

  strcpy(cInFile, pcFileNameIn);
  strcat(cInFile, ".MetaBucks.Timings.txt");
  pcInFile = &cInFile[0];
  fp1 = NULL;
  if((fp1 = fopen(pcInFile, "r")) == NULL) {
    fprintf(stderr, "ERROR(%s): Can't open input file %s\n",
      pcFName, pcInFile);
    exit(-1); }

  /*-----------------------------------*\
   * Scan Input (ASCII) Timings file
  \*-----------------------------------*/
  
  fprintf(stderr, "MESG(%s): Scanning information datafile\n\t%s ...\n",
    pcFName, pcInFile);
  pcTmp = &cTmp[0];

  fscanf(fp1, "-----------\n");
  fscanf(fp1, "Input filename: %s\n", pcTmp);
  /* fprintf(stderr, "\tOriginal datafile: %s\n", pcTmp); */
  fscanf(fp1, "Total Number of input generators = %d\n", &iNumGeneTot);
  pDimAll->iNum3dPts = iNumGeneTot;
  fscanf(fp1, "\t(Xmove, Ymove, Zmove) = (%f , %f , %f).\n",
   &(pDimAll->fMoveX), &(pDimAll->fMoveY), &(pDimAll->fMoveZ));
  fscanf(fp1, "\tOffset = %f\n", &(pDimAll->fOffset));

  fscanf(fp1,
   "MetaBuckets XYZ: Total = %d , AvgGene = %d , Tolerance = %f\n",
   &iNumMBucks, &iTmp, &fTmp);

  pMBuckTop = NULL;
  if((pMBuckTop = (MetaBuckTop *)
      calloc(1, sizeof(MetaBuckTop))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pMBuckTop\n", pcFName);
    return(-1); }

  pMBuckTop->iNumMetaBucks = iNumMBucks;
  pDimAll->iNumMBucks = iNumMBucks;
  pMBuckTop->pMBucks = NULL;
  if((pMBuckTop->pMBucks = (MetaBucket *)
      calloc(iNumMBucks, sizeof(MetaBucket))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pMBuckTop->pMBucks[%d].\n",
      pcFName, iNumMBucks);
    return(-1); }

  fscanf(fp1, "\t(Min , Max) genes per MetaBucket = (%d , %d).\n",
   &iTmp, &iTmp2);
  pMBuckTop->iMinGenes = iTmp;
  pMBuckTop->iMaxGenes = iTmp2;
  fscanf(fp1, "\tEmpty MetaBuckets = %d of which %d are virtual.\n",
   &iTmp, &iTmp2);

  pMBuckTop->iMBuckEmpty = iTmp;
  fscanf(fp1, "\tBins XYZ: Total = %d , Empty = %d \n", &iTmp, &iTmp2);
  pMBuckTop->iNumBins = iTmp;
  fscanf(fp1, "\tCoord ranges: %d < X < %d , ", &iTmp, &iTmp2);
  pMBuckTop->Limits.iMinX = iTmp;
  pMBuckTop->Limits.iMaxX = iTmp2;
  fscanf(fp1, "%d < Y < %d , ", &iTmp, &iTmp2);
  pMBuckTop->Limits.iMinY = iTmp;
  pMBuckTop->Limits.iMaxY = iTmp2;
  fscanf(fp1, "%d < Z < %d\n", &iTmp, &iTmp2);
  pMBuckTop->Limits.iMinZ = iTmp;
  pMBuckTop->Limits.iMaxZ = iTmp2;
  fscanf(fp1, "-----------\n");

  pMBuckTop->iXdim = pDimAll->iXdim =
    pMBuckTop->Limits.iMaxX - pMBuckTop->Limits.iMinX;
  pMBuckTop->iYdim = pDimAll->iYdim =
    pMBuckTop->Limits.iMaxY - pMBuckTop->Limits.iMinY;
  pMBuckTop->iZdim = pDimAll->iZdim =
    pMBuckTop->Limits.iMaxZ - pMBuckTop->Limits.iMinZ;
  pMBuckTop->iSliceSize = pDimAll->iSliceSize =
    pDimAll->iXdim * pDimAll->iYdim;
  pMBuckTop->iWSpaceSize = pDimAll->iWSpaceSize =
    pDimAll->iSliceSize * pDimAll->iZdim;

  pMBuck = pMBuckTop->pMBucks;

  pMBuckInfo = NULL;
  if((pMBuckInfo = (MetaBuckInfo *)
      calloc(1, sizeof(MetaBuckInfo))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pMBuckInfo\n", pcFName);
    return(-1); }

  pMBuckInfo->iNumMetaBucks = iNumMBucks;
  pMBuckInfo->iNumMetaBins  = pMBuckTop->iNumBins;

  pMBuckInfo->piNumBuckets = NULL;
  if((pMBuckInfo->piNumBuckets = (int *)
      calloc(iNumMBucks, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC failed on pMBuckInfo->piNumBuckets[%d].\n",
      pcFName, iNumMBucks);
    exit(-1); }
  pMBuckInfo->piNumBuckEmpty = NULL;
  if((pMBuckInfo->piNumBuckEmpty = (int *)
      calloc(iNumMBucks, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC failed on pMBuckInfo->piNumBuckEmpty[%d].\n",
      pcFName, iNumMBucks);
    exit(-1); }
  pMBuckInfo->piNumBins = NULL;
  if((pMBuckInfo->piNumBins = (int *)
      calloc(iNumMBucks, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC failed on pMBuckInfo->piNumBins[%d].\n",
      pcFName, iNumMBucks);
    exit(-1); }
  pMBuckInfo->piNumGenes = NULL;
  if((pMBuckInfo->piNumGenes = (int *)
      calloc(iNumMBucks, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC failed on pMBuckInfo->piNumGenes[%d].\n",
      pcFName, iNumMBucks);
    exit(-1); }
  pMBuckInfo->piAvgGenes = NULL;
  if((pMBuckInfo->piAvgGenes = (int *)
      calloc(iNumMBucks, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC failed on pMBuckInfo->piAvgGenes[%d].\n",
      pcFName, iNumMBucks);
    exit(-1); }
  pMBuckInfo->piMinGenes = NULL;
  if((pMBuckInfo->piMinGenes = (int *)
      calloc(iNumMBucks, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC failed on pMBuckInfo->piMinGenes[%d].\n",
      pcFName, iNumMBucks);
    exit(-1); }
  pMBuckInfo->piMaxGenes = NULL;
  if((pMBuckInfo->piMaxGenes = (int *)
      calloc(iNumMBucks, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC failed on pMBuckInfo->piMaxGenes[%d].\n",
      pcFName, iNumMBucks);
    exit(-1); }

  fscanf(fp1, "MetaBucket no. : Bucks (Empty, Bins) : Genes (Avg, Min, Max)\n");
  fscanf(fp1, "\tRanges (Xmin < X < Xmax) : (Ymin < Y < Ymax) : (Zmin < Z < Zmax)\n");
  fscanf(fp1, "-----------\n");

  for(i = 0; i < iNumMBucks; i++) {
    fscanf(fp1, "%d : %d ", &iTmp, &iTmp2);
    *(pMBuckInfo->piNumBuckets+i) = iTmp2;
    fscanf(fp1, "(%d , %d) : ", &iTmp, &iTmp2);
    *(pMBuckInfo->piNumBuckEmpty+i) = iTmp;
    *(pMBuckInfo->piNumBins+i) = iTmp2;
    fscanf(fp1, "%d (%d , ", &iTmp, &iTmp2);
    *(pMBuckInfo->piNumGenes+i) = iTmp;
    *(pMBuckInfo->piAvgGenes+i) = iTmp2;
    fscanf(fp1, "%d , %d)\n", &iTmp, &iTmp2);
    *(pMBuckInfo->piMinGenes+i) = iTmp;
    *(pMBuckInfo->piMaxGenes+i) = iTmp2;
    fscanf(fp1, "\t%d < X < %d , ", &iTmp, &iTmp2);
    pMBuck->Limits.iMinX = iTmp;
    pMBuck->Limits.iMaxX = iTmp2;
    fscanf(fp1, "%d < Y < %d , ", &iTmp, &iTmp2);
    pMBuck->Limits.iMinY = iTmp;
    pMBuck->Limits.iMaxY = iTmp2;
    fscanf(fp1, "%d < Z < %d\n", &iTmp, &iTmp2);
    pMBuck->Limits.iMinZ = iTmp;
    pMBuck->Limits.iMaxZ = iTmp2;
    pMBuck++;
  }

  fclose(fp1);

  /* --------------------------------------------------------- *\
   *  Set Top 3D Look-up table and MetaBucket neighborhoods
  \* --------------------------------------------------------- */

  iWSpaceSize = pDimAll->iWSpaceSize;
  pMBuckTop->pisLookUp = NULL;
  if((pMBuckTop->pisLookUp = (short *)
      calloc(iWSpaceSize, sizeof(short))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC fails on pMBuckTop->pisLookUp[%d].\n",
      pcFName, iWSpaceSize);
    return(-1); }
  
  Set3dLookUpTableForMBucks(pMBuckTop);
  SetBuckNgbsForMBucks(pMBuckTop);

  /* --------------------------------------------------------- *\
   *  Set voxel look-up table to use in validation
  \* --------------------------------------------------------- */

  strcpy(cInFile, pcFileNameIn);
  strcat(cInFile, ".MetaBucks.Data.txt");
  pcInFile = &cInFile[0];

  iWSpaceSize = pDimAll->iWSpaceSize;
  pVoxels = NULL;
  if((pVoxels = (Voxel *) calloc(iWSpaceSize, sizeof(Voxel))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pVoxels[%d].\n",
      pcFName, iWSpaceSize);
    exit(-1); }

  pGeneCoord = NULL;
  if((pGeneCoord = (Pt3dCoord *)
      calloc(iNumGeneTot, sizeof(Pt3dCoord))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC failed for pGeneCoord[%d].\n",
      pcFName, iNumGeneTot);
    return(-1); }

  FillVoxels(pcInFile, pGeneCoord, pVoxels, pDimAll);


  /* --------------------------------------------------------- *\
   *  Compute scaffold within each MetaBucket
  \* --------------------------------------------------------- */

  pMBuck = pMBuckTop->pMBucks;

  for(i = 0; i < iNumMBucks; i++) {

  pMBuckTop->iMBuckID = i;
    iNumGenes = *(pMBuckInfo->piNumGenes+i);
    if(iNumGenes < 1)
      continue;
    if(iNumGenes > iNumGeneTot) {
      fprintf(stderr, "ERROR(%s):\n\tToo many genes (%d) for MBuck %d\n",
        pcFName, iNumGenes, i);
      exit(-2);
    }
    iNumBucks = *(pMBuckInfo->piNumBuckets+i);
    if(iNumBucks < 1) {
      fprintf(stderr, "ERROR(%s):\n\tNo buckets for MBuck %d\n",
        pcFName, i);
      exit(-2);
    }
    iNumBins  = *(pMBuckInfo->piNumBins+i);
    if(iNumBins < iNumBucks) {
      fprintf(stderr, "ERROR(%s):\n\tNot enough bins for MBuck %d\n",
        pcFName, i);
      exit(-2);
    }

    /* ---- Initialize memory ---- */

    pInData = NULL;
    if((pInData = (InputData *)
  calloc(iNumGenes, sizeof(InputData))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC failed for pInData[%d].\n",
        pcFName, iNumGenes);
      return(-1); }

    pBucksXYZ = NULL;
    if((pBucksXYZ = (Buckets *)
  calloc(1, sizeof(Buckets))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC fails on pBucksXYZ\n", pcFName);
      return(-1); }
    pBucksXYZ->pBucket = NULL;
    if((pBucksXYZ->pBucket = (Bucket *)
  calloc(iNumBucks, sizeof(Bucket))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC fails on pBucksXYZ->pBucket[%d].\n",
        pcFName, iNumBucks);
      return(-1); }
    pBucksXYZ->iNumBucks = iNumBucks;
    pBucksXYZ->pBin = NULL;
    if((pBucksXYZ->pBin = (Bin *)
  calloc(iNumBins, sizeof(Bin))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBucksXYZ->pBin[%d].\n",
        pcFName, iNumBins);
      return(-1); }
    pBucksXYZ->iNumBins = iNumBins;
    pBucksXYZ->iMinCoord = 0;

    /* ---- Read-in Data ---- */

    pDimMBuck->iNumMBucks = 1;
    strcpy(cInFileData, pcFileNameIn);
    sprintf(cTmp, ".MetaBuck%d.Data.txt", i);
    strcat(cInFileData, cTmp);
    pcInFile = &cInFileData[0];
    if(!LoadDataMBuck(pcInFile, pInData, pBucksXYZ, pDimMBuck, 200)) {
      fprintf(stderr, "ERROR(%s): Reading input ... exiting.\n", pcFName);
      exit(-2); }

    /* ------------------------------------------------------ *\
     *  Set 3D Look-up table and Bucket neighborhoods
    \* ------------------------------------------------------ */

    iWSpaceSize = pDimMBuck->iWSpaceSize;

    //: MING: THERE ARE MEMORY ACCESS ERROR IN FRE'S CODE, 
    //  BUT IT'S NOT DETECTED AS ERROR ON SGI!!
    iWSpaceSize *= 2;

    pisLookUp = NULL;
    if((pisLookUp = (short *) calloc(iWSpaceSize, sizeof(short))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC fails on pisLookUp[%d].\n",
        pcFName, iWSpaceSize);
      return(-1); }
  
    Set3dLookUpTable(pisLookUp, pBucksXYZ, pDimMBuck);
    SetBuckNgbs(pisLookUp, pBucksXYZ, pDimMBuck);

    /* ------------------------------------------------------ *\
     *  Initialization of shock scaffold : Itera = 0
     *  Compute 1st pass of shocks: Sheets, Curves, Vertices
    \* ------------------------------------------------------ */

    pScaf = NULL;
    if((pScaf = (ShockScaffold *)
  calloc(1, sizeof(ShockScaffold))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC fails on pScaffold[1].\n",
        pcFName);
      exit(-4); }
    pScaf->isMaxItera = iMaxItera;
    InitScaf(pScaf);

    /* -- Allocate space for initial Blue shocks -- */

    iNumShocks = pDimMBuck->iMaxBlueShocks;
    iTmp = iNumShocks /= iMemDiv;
    pShockSheets = NULL;
    if((pShockSheets = (ShockSheet *)
  calloc(iNumShocks, sizeof(ShockSheet))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC fails on pShockSheets[%d].\n",
        pcFName, iNumShocks);
      iFlag = TRUE;
      while(iFlag) {
  iNumShocks /= 200;
  if((pShockSheets = (ShockSheet *)
      calloc(iNumShocks, sizeof(ShockSheet))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC still fails on pShockSheets[%d].\n",
      pcFName, iNumShocks); }
  else iFlag = FALSE;
      }
      fprintf(stderr,
        "MESG(%s):\n\tAllocated space for %d blue shocks instead of %d.\n",
        pcFName, iNumShocks, iTmp);
    }
    else {
      fprintf(stderr, "MESG(%s):\n\tAllocated space for %d blue shocks.\n",
        pcFName, iNumShocks); }
    pDimMBuck->iMaxBlueShocks = iNumShocks;
    pDimMBuck->iNumShockSheet = 0;
    pDimMBuck->iNumShockSheetIntSub = 0;
    pDimMBuck->iNumShockSheetExtSub = 0;
  

    /* ----------------- Find & valid Init. Blue shocks --------------- *\
    \* ----------------------- For all samples ------------------------ */

    /* ----- Find Shocks INTERNAL to each bucket at FULL RES. ------ */

    /* -- 1st: Find Isolated Blue shocks -- */

    FetchBlueShocksIntAll(pInData, pShockSheets, pBucksXYZ, pGeneCoord,
        pVoxels, pDimMBuck, pDimAll);

    /* -- Transfer to Scaffold -- */

    iTmp = pDimMBuck->iNumShockSheet;
    /* Allocate more space than valids, for up-coming ones at full res. */
    iMax = 1 + 4 * iTmp;
    pScaf->pBlue = NULL;
    if((pScaf->pBlue =
  (ShockSheet *) calloc(iMax, sizeof(ShockSheet))) == NULL) {
      fprintf(stderr, "ERROR(%s): CALLOC failed for pScaf->pBlue[%d].\n",
        pcFName, iMax);
      return(-2); }
    pScaf->iNumBlueMax = iMax;

    if(!SetValidSheets(pShockSheets, pScaf, pInData, pDimMBuck)) {
      fprintf(stderr,
        "ERROR(%s): Cannot set sheets on scaffold.\n", pcFName);
      exit(-2); }

    free(pShockSheets); pShockSheets = NULL;

    pScaf->iNumBlueInt = iTmp;
    pScaf->iNumBlueIsolated = iTmp;

      /* -- Go find other shocks -- */

    iTmpBlue  = pScaf->iBlueActiveLast;
    iTmpGreen = pScaf->iGreenActiveLast;
    iTmpRed   = pScaf->iNumRedLast;

    /* Only 1 (non-iterative) step performed */
    FetchShocksIntAll(pInData, pScaf, pisLookUp, pBucksXYZ,
          pGeneCoord, pVoxels, pDimMBuck, pDimAll);

    /* pScaf->iNumBlueInt = pScaf->iBlueActiveLast; */
    pScaf->iNumGreenInt   = pScaf->iGreenActiveLast - iTmpGreen;
    pScaf->iNumRedInt     = pScaf->iNumRedLast - iTmpRed;

    pScaf->iTotalShocks = pScaf->iBlueActiveLast +
      pScaf->iGreenActiveLast + pScaf->iNumRedLast;

    /* ----- Found Shocks internal to each bucket at Full res. ------ *\
    \*     Now go find shocks that are EXTERNAL, via layers      */

    /* ---- 1st : Find shocks in FIRST LAYER around each bucket ---- */

    FetchBlueShocksExtFst(pInData, pScaf, pisLookUp, pBucksXYZ,
        pGeneCoord, pVoxels, pDimMBuck, pDimAll);

    iTmp = pDimMBuck->iNumShockSheet;
    /* Allocate more space than valids, for up-coming ones from iterations */
    iTmp *= 2;
    iMax = pScaf->iNumBlueMax - 2;
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

    /* -- Go find other shocks (curves & vertices) in 1st Layer -- */

    iTmpBlue  = pScaf->iBlueActiveLast;
    iTmpGreen = pScaf->iGreenActiveLast;
    iTmpRed   = pScaf->iNumRedLast;

    FetchShocksExtFst(pInData, pScaf, pisLookUp, pBucksXYZ,
          pGeneCoord, pVoxels, pDimMBuck, pDimAll);

    pScaf->iNumBlueFst  = pScaf->iBlueActiveLast - iTmpBlue;
    pScaf->iNumGreenFst = pScaf->iGreenActiveLast - iTmpGreen;
    pScaf->iNumRedFst   = pScaf->iNumRedLast - iTmpRed;

    pScaf->iTotalShocks = pScaf->iBlueActiveLast +
      pScaf->iGreenActiveLast + pScaf->iNumRedLast;


    /* ---- 2nd : Find shocks in other Layers around each bucket ---- */

    FetchBlueShocksExtOth(pInData, pScaf, pisLookUp, pBucksXYZ,
        pGeneCoord, pVoxels, pDimMBuck, pDimAll);

    iTmp = pScaf->iTotalShocks;
    pScaf->iTotalShocks = pScaf->iBlueActiveLast +
      pScaf->iGreenActiveLast + pScaf->iNumRedLast;
    iTmp = pScaf->iTotalShocks - iTmp;
    fprintf(stderr,
      "MESG(%s): %d new Blue shocks were added to the scaffold,\n",
      pcFName, iTmp);
    fprintf(stderr, "\tfor a total of %d shock nodes.\n",
      pScaf->iTotalShocks);

   /* -- Go find other shocks (curves and nodes) from all layers -- */

    iTmpBlue  = pScaf->iBlueActiveLast;
    iTmpGreen = pScaf->iGreenActiveLast;
    iTmpRed   = pScaf->iNumRedLast;

    FetchShocksExtOth(pInData, pScaf, pisLookUp, pBucksXYZ,
          pGeneCoord, pVoxels, pDimMBuck, pDimAll);

    pScaf->iNumBlueOth  = pScaf->iBlueActiveLast - iTmpBlue;
    pScaf->iNumGreenOth = pScaf->iGreenActiveLast - iTmpGreen;
    pScaf->iNumRedOth   = pScaf->iNumRedLast - iTmpRed;

    iTmp = pScaf->iTotalShocks;
    pScaf->iTotalShocks = pScaf->iBlueActiveLast +
      pScaf->iGreenActiveLast + pScaf->iNumRedLast;
    iTmp = pScaf->iTotalShocks - iTmp;
    fprintf(stderr,
      "MESG(%s): %d new shocks were added to the scaffold,\n",
      pcFName, iTmp);
    fprintf(stderr, "\tfor a total of %d shock nodes.\n",
      pScaf->iTotalShocks);

    iTmpBlue  = pScaf->iBlueActiveLast;
    iTmpGreen = pScaf->iGreenActiveLast;
    iTmpRed   = pScaf->iNumRedLast;

    FetchShocksExtOthItera(pInData, pScaf, pisLookUp, pBucksXYZ,
         pGeneCoord, pVoxels, pDimMBuck, pDimAll);

    pScaf->iNumBlueItera  = pScaf->iBlueActiveLast - iTmpBlue;
    pScaf->iNumGreenItera = pScaf->iGreenActiveLast - iTmpGreen;
    pScaf->iNumRedItera   = pScaf->iNumRedLast - iTmpRed;

    iTmp = pScaf->iTotalShocks;
    pScaf->iTotalShocks = pScaf->iBlueActiveLast +
      pScaf->iGreenActiveLast + pScaf->iNumRedLast;
    iTmp = pScaf->iTotalShocks - iTmp;
    fprintf(stderr,
      "MESG(%s): %d new shocks were added to the scaffold,\n",
      pcFName, iTmp);
    fprintf(stderr, "\tfor a total of %d shock nodes.\n",
      pScaf->iTotalShocks);

    CheckResults(pScaf, pInData, pDimMBuck);

    ConnectBlue2RedAndGreen(pScaf, pInData, pDimMBuck);

    /* ---- Save mini-scaffold for MetaBucket ---- *\
    \*      and clean-up data-structures     */

    strcpy(cOutFile, pcFileNameIn);
    /* sprintf(cTmp, ".MetaBuck%d.BlueFlow.ShockData.txt", i); */
    sprintf(cTmp, ".MetaBuck%d.FS", i);
    strcat(cOutFile, cTmp);
    pcOutFile = &cOutFile[0];
    SaveShockData(pcInFile, pcOutFile, pInData, pScaf, pDimMBuck,
      pDimAll);

    save_fs_ply2 (pcInFile, "shock.ply2", pInData, pScaf, pDimMBuck,
             pDimAll);

    CleanUpInData(pInData, pDimMBuck);
    CleanUpMBuck(pBucksXYZ, pDimMBuck);
    free(pInData); pInData = NULL;
    free(pBucksXYZ->pBucket); pBucksXYZ->pBucket = NULL;
    free(pBucksXYZ->pBin); pBucksXYZ->pBin = NULL;
    free(pisLookUp); pisLookUp = NULL;
    free(pScaf->pBlue); pScaf->pBlue = NULL;
    free(pScaf->pGreen); pScaf->pGreen = NULL;
    free(pScaf->pRed); pScaf->pRed = NULL;

  } /* Next MetaBucket: i++ */

  /* ---------- End Of Processing ---------- */

  fprintf(stderr, "\nMESG(%s): This is it!\n\t", pcFName);
  fprintf(stderr, "MING: task has been performed ... bye ;)\n");
  
  return(TRUE);
}

/* ================ EoF ================*/

