/*------------------------------------------------------------*\
 * File: sheet_flow_io_v2.c
 *
 * Functions:
 *  LoadDataMBuck
 *  SaveTimings
 *  SaveShockData
 *  CleanUpInData
 *  CleanUpMBuck
 *  TestInData
 *  FillVoxels
 *
 * History:
 *  #0: August 2001, by F.Leymarie
 *  #1: Oct. 2001: Added CleanUpInData()
 *  #2: July 2003: Added LoadDataMBuck() and FillVoxels()
 *
\*------------------------------------------------------------*/

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include "sheet_flow_io_v2.h"
#include "circum.h"
#include "barycentric.h"

#include <vcl_cstdio.h>

/*------------------------------------------------------------*\
 * Function: LoadDataMBuck
 *
 * Usage: Input data per MetaBucket.
 *    Generators are assumed to be unique (no duplicates).
 *
\*------------------------------------------------------------*/

int
LoadDataMBuck(char *pcInFile, InputData *pInData, Buckets *pBucksXYZ,
        Dimension *pDim, int iMaxSize)
{
  char    *pcFName = "LoadDataMBuck";
  char    *pcTmp, cTmp[128] = "";
  FILE    *fp1;
  register int  i, j, k;
  int    iNumGenes, iNum, iA,iB,iC,iD,iE,iF,iG,iH,iI,iJ;
  int    iNumBucks, iNumBins, iGene, iMax, iBin, iTmp;
  int    *piBinGene, *piBuckGene;
  float    fA,fB,fC, fMaxX,fMaxY,fMaxZ, fMinX,fMinY,fMinZ;
  Pt3dCoord  *pGeneCoord;
  InputData     *pGene;
  Bucket  *pBuck;
  Bin    *pBin;
  
  if((fp1 = fopen(pcInFile, "r")) == NULL) {
    fprintf(stderr, "ERROR(%s): Can't open input UPenn file %s\n",
      pcFName, pcInFile);
    return(FALSE); }

  fprintf(stderr, "MESG(%s):\n", pcFName);
  fprintf(stderr,
    "\tMaxMemSize: backpointers to shocks on generators = %d\n",
    iMaxSize);
#if FALSE
  iMax = (int) MAXSHORT; /* 32767 == 2^15 - 1 */
  fprintf(stderr, "\tMax Short = %d (size in bytes = %d)\n", iMax,
    sizeof(short));
  iMax = (int) MAXINT; /* 32767 == 2^15 - 1 */
  fprintf(stderr, "\tMax Int = %d (size in bytes = %d)\n", iMax,
    sizeof(int));
  fMax = (float) MAXFLOAT; /* 32767 == 2^15 - 1 */
  fprintf(stderr, "\tMax Float = %f (size in bytes = %d)\n", fMax,
    sizeof(float));
#endif

  /* ---- 1st: Load in Input Generator Data ---- */

  pcTmp = &cTmp[0];
  fscanf(fp1, "Input filename: %s\n", pcTmp);
  fprintf(stderr, "\tOriginal datafile:\n\t  %s\n", pcTmp);
  fscanf(fp1, "Data for MetaBucket no. %d (out of %d).\n",
   &iA, &iB);
  pDim->iMetaBuckId = iA;
  pDim->iNumMBucks = iB;
  fprintf(stderr, "\tData for MetaBucket no. %d (out of %d).\n",
    pDim->iMetaBuckId, pDim->iNumMBucks);

  fscanf(fp1, "Min grid coords: X = %d , Y = %d , Z = %d\n",
   &iA,&iB,&iC);
  pBucksXYZ->Limits.iMinX = iA;
  pBucksXYZ->Limits.iMinY = iB;
  pBucksXYZ->Limits.iMinZ = iC;
  fprintf(stderr, "\tMin grid coords: X = %d , Y = %d , Z = %d\n",
    iA,iB,iC);
  pDim->Limits.iMinX = iA;
  pDim->Limits.iMinY = iB;
  pDim->Limits.iMinZ = iC;

  fscanf(fp1, "Max grid coords: X = %d , Y = %d , Z = %d\n",
   &iA,&iB,&iC);
  pBucksXYZ->Limits.iMaxX = iA;
  pBucksXYZ->Limits.iMaxY = iB;
  pBucksXYZ->Limits.iMaxZ = iC;
  fprintf(stderr, "\tMax grid coords: X = %d , Y = %d , Z = %d\n",
    iA,iB,iC);
  pDim->Limits.iMaxX = iA;
  pDim->Limits.iMaxY = iB;
  pDim->Limits.iMaxZ = iC;

  fscanf(fp1, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n", &iA,&iB,&iC);
  fprintf(stderr, "\t(Xdim, Ydim, Zdim) = (%d , %d , %d)\n",
    iA,iB,iC);
  pDim->iXdim = iA;  pDim->iYdim = iB;  pDim->iZdim = iC;

  iTmp = pBucksXYZ->Limits.iMaxX - pBucksXYZ->Limits.iMinX;
  if(iTmp != pDim->iXdim) {
    fprintf(stderr, "ERROR(%s): MaxX - MinX = %d != DimX = %d\n",
      pcFName, iTmp, pDim->iXdim);
    exit(-2);
  }
  iTmp = pBucksXYZ->Limits.iMaxY - pBucksXYZ->Limits.iMinY;
  if(iTmp != pDim->iYdim) {
    fprintf(stderr, "ERROR(%s): MaxY - MinY = %d != DimY = %d\n",
      pcFName, iTmp, pDim->iYdim);
    exit(-2);
  }
  iTmp = pBucksXYZ->Limits.iMaxZ - pBucksXYZ->Limits.iMinZ;
  if(iTmp != pDim->iZdim) {
    fprintf(stderr, "ERROR(%s): MaxZ - MinZ = %d != DimZ = %d\n",
      pcFName, iTmp, pDim->iZdim);
    exit(-2);
  }

  pDim->iSliceSize = iA * iB;
  pDim->iWSpaceSize = pDim->iSliceSize * iC;

  fscanf(fp1, "(Xmove, Ymove, Zmove) = (%f , %f , %f).\n", &fA,&fB,&fC);
  pDim->fMoveX = fA; pDim->fMoveY = fB; pDim->fMoveZ = fC;
  fscanf(fp1, "Offset = %f.\n", &fA);
  pDim->fOffset = fA;

  fscanf(fp1, "Number of generators: %d (out of %d)\n", &iNumGenes, &iA);
  pDim->iNum3dPts = iNumGenes;
  fprintf(stderr, "\tNumber of generators: %d (out of %d)\n", iNumGenes, iA);

  fscanf(fp1, "-----------\n");
  fscanf(fp1, "Original ID : Coords (x,y,z) : BinXYZ label\n");
  fscanf(fp1, "-----------\n");

  fMaxX = fMaxY = fMaxZ = 0.0;
  fMinX = fMinY = fMinZ = (float) pDim->iWSpaceSize;

  pGene = pInData;
  for(i = 0; i < iNumGenes; i++) {
    pGeneCoord = &(pGene->Sample);
    fscanf(fp1, "%d : (%f , %f , %f) : %d\n", &iA, &fA,&fB,&fC, &iB);
    pGeneCoord->fPosX = fA;
    pGeneCoord->fPosY = fB;
    pGeneCoord->fPosZ = fC;

    pGene->iID      = iA;
    pGene->iBinXYZ  = iB;
    pGene->iBuckXYZ = -1;

    if(fA > fMaxX) fMaxX = fA;
    else if(fA < fMinX) fMinX = fA;
    if(fB > fMaxY) fMaxY = fB;
    else if(fB < fMinY) fMinY = fB;
    if(fC > fMaxZ) fMaxZ = fC;
    else if(fC < fMinZ) fMinZ = fC;

    /* -- Set space for backpointers to shocks -- */

    pGene->isNumBlue = 0;
    pGene->isMaxBlue = iMaxSize;
    pGene->piBlueList = NULL;
    if((pGene->piBlueList = (int *) calloc(iMaxSize, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for (pGene+%d)->piBlueList[%d].\n",
        pcFName, i, iMaxSize);
      return(FALSE); }

    pGene->isNumGreen = 0;
    pGene->isMaxGreen = iMaxSize;
    pGene->piGreenList = NULL;
    if((pGene->piGreenList = (int *) calloc(iMaxSize, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for (pGene+%d)->piGreenList[%d].\n",
        pcFName, i, iMaxSize);
      return(FALSE); }

    pGene->isNumRed = 0;
    pGene->isMaxRed = iMaxSize;
    pGene->piRedList = NULL;
    if((pGene->piRedList = (int *) calloc(iMaxSize, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for (pGene+%d)->piRedList[%d].\n",
        pcFName, i, iMaxSize);
      return(FALSE); }
      
    /* -- Set space for Normal data: (Pb - Pa) -- */

    /* pGene->iNumNorm = 0;
       pGene->iMaxNorm = iNum; Same as Blue data: not needed */
    pGene->pvNormal = NULL;
    if((pGene->pvNormal = (Vector3d *)
  calloc(iMaxSize, sizeof(Vector3d))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC fails on (pGene+%d)->pvNormal[%d].\n",
        pcFName, i, iMaxSize);
      return(FALSE); }
      
    pGene++;
  }

  pDim->fMinX = fMinX;  pDim->fMaxX = fMaxX;
  pDim->fMinY = fMinY;  pDim->fMaxY = fMaxY;
  pDim->fMinZ = fMinZ;  pDim->fMaxZ = fMaxZ;

  /* -- Theoretical max number of Initial Blue (sheet) Shocks: A_1^2-2 -- */

  pDim->iMaxBlueShocks = iNumGenes * (iNumGenes - 1);
  pDim->iMaxBlueShocks >>= 1; /* Division by 2 */
  fprintf(stderr, "\tMax (theoretical) number of initial BLUE shocks = %d\n",
    pDim->iMaxBlueShocks);

  pDim->iNumShockSheet = 0;
  pDim->iMaxProxiPerBlueShock = pDim->iTotalBlueProxi = 0;
  pDim->iNumShockCurve = 0;
  pDim->iMaxGreenShocks = pDim->iMaxProxiPerGreenShock = 0;
  pDim->iTotalGreenProxi = pDim->iNumShockGreen2Blue = 0;
  pDim->iNumShockVertex = pDim->iMaxRedShocks = 0;
  pDim->iNumShockRed2Green = pDim->iNumShockRed2Blue = 0;
  pDim->iNumCurveActiveMax = 0;
  pDim->fRadMax = 0.0;
  pDim->isFlagPermute = 0;


  /* ---- 2nd: Load in BucketsXYZ data ---- */

  fscanf(fp1, "-----------\n");
  fscanf(fp1,
   "BUCKETS XYZ: Total = %d , AvgGene (Target) = %d , Tolerance = %f\n",
   &iNumBucks, &iA, &fA);
  pBucksXYZ->iNumBucks = iNumBucks;
  fprintf(stderr, "\tNumber of BucketsXYZ: %d\n", iNumBucks);
  pBucksXYZ->iAvgGenes = iA;
  pBucksXYZ->fTolerance = fA;

  fscanf(fp1, " Empty buckets = %d of which %d are virtual.\n", &iA, &iB);
  pBucksXYZ->iEmpty = iA;
  pBucksXYZ->iVirtual = iB;

  fscanf(fp1,
   "ID: (1stBin NumBins) (MinX MaxX : MinY MaxY : MinZ MaxZ) : NumGene\n");
  fscanf(fp1, "-----------\n");

  iMax = 0;
  pBuck = pBucksXYZ->pBucket;
  for(i = 0; i < iNumBucks; i++) {
    fscanf(fp1, "%d: (%d %d) (%d %d : %d %d : %d %d) :\t%d\n",
     &iA, &iB, &iC, &iD, &iE, &iF, &iG, &iH, &iI, &iJ);
    pBuck->iBinFirst = iB;
    pBuck->isNumBins = iC;
    pBuck->iBinLast = iB + iC - 1;
    pBuck->Limits.iMinX = iD;
    pBuck->Limits.iMaxX = iE;
    pBuck->Limits.iMinY = iF;
    pBuck->Limits.iMaxY = iG;
    pBuck->Limits.iMinZ = iH;
    pBuck->Limits.iMaxZ = iI;
    pBuck->isNumGenes = iJ;
    if(iJ > iMax) iMax = iJ;
    pBuck->isNumNgbs = pBuck->isNumPair = pBuck->isLastItera = 0;
    pBuck++;
  }
  pBucksXYZ->iMaxGenes = iMax;
  fprintf(stderr, "\tMax Number of Genes per Bucket = %d\n", iMax);
  pBucksXYZ->iNumGenes = iNumGenes; /* Total number of genes */
  

  /* ---- 3rd: Load in BinsXYZ data ---- */

  fscanf(fp1, "-----------\n");
  fscanf(fp1, "BINS XYZ: Total = %d\n", &iNumBins);
  fscanf(fp1, "ID: Number of Genes\n");
  fscanf(fp1, "-----------\n");
  pBucksXYZ->iNumBins = iNumBins;
  fprintf(stderr, "\tNumber of BinsXYZ (voxels): %d\n", iNumBins);

  /* Set space per bin in terms of number of genes */
  pBin = pBucksXYZ->pBin;
  for(i = 0; i < iNumBins; i++) {
    fscanf(fp1, "%d: %d\n", &iA, &iB);
    if(iB > 0) {
      pBin->piGenes = NULL;
      if((pBin->piGenes = (int *) calloc(iB, sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBin[%d]->piSrcs[%d].\n",
    pcFName, i, iB);
  exit(-2); }
    }
    pBin->isNumGenes = 0;
    pBin->isMaxGenes = iB;
    pBin++;
  }

  /* Load gene labels in bins */
  pGene = pInData;
  for(i = 0; i < iNumGenes; i++) {
    iBin = pGene->iBinXYZ;
    pBin = pBucksXYZ->pBin+iBin;
    iNum = pBin->isNumGenes;
    piBinGene = pBin->piGenes+iNum;
    *piBinGene = i;
    pBin->isNumGenes++; iNum++;
    if(iNum > pBin->isMaxGenes) {
  fprintf(stderr, "ERROR(%s): Overflow in genes for bin no. %d\n",
    pcFName, iBin);
  exit(-2); }
    pGene++;
  }

  fclose(fp1); /* No more inputs from file: close it */
  
  /* ---- 4th: Copy Gene Data from Bins to Buckets ---- */

  pBuck = pBucksXYZ->pBucket;
  for(i = 0; i < iNumBucks; i++) {
    pBuck->iInitSheetLabel = -1;
    iA = pBuck->iBinFirst;
    iB = pBuck->iBinLast + 1;
    iC = pBuck->isNumGenes;
    iD = 0;
    pBuck->piGenes = NULL;
    pBuck->pucFlagNgb = NULL;
    pBuck->pucFlagViz = NULL;
    pBuck->pucFlagPair = NULL;
    pBuck->piPtrNgb = NULL;

    if(iC > 0) {
      if((pBuck->piGenes = (int *) calloc(iC, sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBuck[%d]->piGenes[%d].\n",
    pcFName, i, iC);
  exit(-2); }

      piBuckGene = pBuck->piGenes;
      for(j = iA; j < iB; j++) {
  pBin = pBucksXYZ->pBin+j;
  iNum = pBin->isNumGenes;
  if(iNum < 1) continue;
  piBinGene = pBin->piGenes;
  for(k = 0; k < iNum; k++) {
    iGene = *piBinGene++;
    *piBuckGene++ = iGene;
    iD++;
    pGene = pInData+iGene;
    pGene->iBuckXYZ = i; /* Attach Bucket label to gene */
  }
  /* free(pBin->piGenes); */
      }
      if(iD != iC) {
  fprintf(stderr,
    "ERROR(%s): Loaded %d genes instead of %d into Buckets[%d].\n",
    pcFName, iD, iC, i);
  exit(-2); }
      /* Allocate space for Flags to Buckets */
      if((pBuck->pucFlagNgb =
    (UCHAR *) calloc(iNumBucks, sizeof(UCHAR))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBuck[%d]->pucFlagNgb[%d].\n",
    pcFName, i, iNumBucks);
  exit(-2); }
      if((pBuck->pucFlagViz =
    (UCHAR *) calloc(iNumBucks, sizeof(UCHAR))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBuck[%d]->pucFlagViz[%d].\n",
    pcFName, i, iNumBucks);
  exit(-2); }
      if((pBuck->pucFlagPair =
    (UCHAR *) calloc(iNumBucks, sizeof(UCHAR))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBuck[%d]->pucFlagPair[%d].\n",
    pcFName, i, iNumBucks);
  exit(-2); }
      if((pBuck->piPtrNgb =
    (int *) calloc(iNumBucks, sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBuck[%d]->piPtrNgb[%d].\n",
    pcFName, i, iNumBucks);
  exit(-2); }
    }

    pBuck++;
  }  
  /* free(pBucksXYZ->pBin);
     pBucksXYZ->pBin = NULL; */

  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: LoadDataMBuckValid
 *
 * Usage: Input data per MetaBucket.
 *    Generators are assumed to be unique (no duplicates).
 *
\*------------------------------------------------------------*/
#if 0
int
LoadDataMBuckValid(char *pcInFile, MetaBucket *pMBucket)
{
  char    *pcFName = "LoadDataMBuckValid";
  char    *pcTmp, cTmp[128] = "";
  FILE    *fp1;
  register int  i, j, k;
  int    iNumGenes, iNum, iA,iB,iC,iD,iE,iF,iG,iH,iI,iJ;
  int    iNumBucks, iNumBins, iGene, iBin;
  int    *piBinGene, *piBuckGene, *piBinPerGene, *piBin;
  float    fA,fB,fC;
  Pt3dCoord  *pGeneCoord;
  MBucket  *pBuck;
  Bin    *pBin;
  
  if((fp1 = fopen(pcInFile, "r")) == NULL) {
    fprintf(stderr, "ERROR(%s): Can't open input UPenn file %s\n",
      pcFName, pcInFile);
    return(FALSE); }

  /* fprintf(stderr, "MESG(%s):\n", pcFName); */

  /* ---- 1st: Load in Input Generator Data ---- */

  pcTmp = &cTmp[0];
  fscanf(fp1, "Input filename: %s\n", pcTmp);
  fprintf(stderr, "\tOriginal datafile:\n\t  %s\n", pcTmp);
  fscanf(fp1, "Data for MetaBucket no. %d (out of %d).\n",
   &iA, &iB);

  fscanf(fp1, "Min grid coords: X = %d , Y = %d , Z = %d\n",
   &iA,&iB,&iC);
  pMBucket->Limits.iMinX = iA;
  pMBucket->Limits.iMinY = iB;
  pMBucket->Limits.iMinZ = iC;

  fscanf(fp1, "Max grid coords: X = %d , Y = %d , Z = %d\n",
   &iA,&iB,&iC);
  pMBucket->Limits.iMaxX = iA;
  pMBucket->Limits.iMaxY = iB;
  pMBucket->Limits.iMaxZ = iC;

  fscanf(fp1, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n", &iA,&iB,&iC);
  fprintf(stderr, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n",
    iA,iB,iC);
  pMBucket->iXdim = iA;  pMBucket->iYdim = iB;  pMBucket->iZdim = iC;
  pMBucket->iSliceSize = iA * iB;
  pMBucket->iWSpaceSize = pMBucket->iSliceSize * iC;

  fscanf(fp1, "(Xmove, Ymove, Zmove) = (%f , %f , %f).\n", &fA,&fB,&fC);
  fscanf(fp1, "Offset = %f.\n", &fA);
  fscanf(fp1, "Number of generators: %d (out of %d)\n", &iNumGenes, &iA);

  fscanf(fp1, "-----------\n");
  fscanf(fp1, "Original ID : Coords (x,y,z) : BinXYZ label\n");
  fscanf(fp1, "-----------\n");

  iNumBins = pMBucket->iNumBins;
  if((piBinPerGene =
      (int *) calloc(iNumBins, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC fails on piBinPerGene[%d].\n",
      pcFName, iNumBins);
    exit(-2); }

  piBin = piBinPerGene;
  pGeneCoord = pMBucket->pGeneCoord-1;
  for(i = 0; i < iNumGenes; i++) {
    pGeneCoord++;
    fscanf(fp1, "%d : (%f , %f , %f) : %d\n", &iA, &fA,&fB,&fC, &iB);
    pGeneCoord->fPosX = fA;
    pGeneCoord->fPosY = fB;
    pGeneCoord->fPosZ = fC;
    *piBin++ = iB;
  }

  /* ---- 2nd: Load in BucketsXYZ data ---- */

  fscanf(fp1, "-----------\n");
  fscanf(fp1,
   "BUCKETS XYZ: Total = %d , AvgGene (Target) = %d , Tolerance = %f\n",
   &iNumBucks, &iA, &fA);
  fscanf(fp1, " Empty buckets = %d of which %d are virtual.\n", &iA, &iB);

  fscanf(fp1,
   "ID: (1stBin NumBins) (MinX MaxX : MinY MaxY : MinZ MaxZ) NumGene\n");
  fscanf(fp1, "-----------\n");

  pBuck = pMBucket->pBucket-1;
  for(i = 0; i < iNumBucks; i++) {
    pBuck++;
    fscanf(fp1, "%d: (%d %d) (%d %d : %d %d : %d %d) :\t%d\n",
     &iA, &iB, &iC, &iD, &iE, &iF, &iG, &iH, &iI, &iJ);
    pBuck->iBinFirst = iB;
    pBuck->isNumBins = iC;
    pBuck->iBinLast = iB + iC - 1;
    pBuck->Limits.iMinX = iD;
    pBuck->Limits.iMaxX = iE;
    pBuck->Limits.iMinY = iF;
    pBuck->Limits.iMaxY = iG;
    pBuck->Limits.iMinZ = iH;
    pBuck->Limits.iMaxZ = iI;
    pBuck->isNumGenes = iJ;
    pBuck->isNumNgbs = 0;
  }

  /* ---- 3rd: Load in BinsXYZ data ---- */

  fscanf(fp1, "-----------\n");
  fscanf(fp1, "BINS XYZ: Total = %d\n", &iNumBins);
  fscanf(fp1, "ID: Number of Genes\n");
  fscanf(fp1, "-----------\n");

  pBin = pMBucket->pBin-1;
  for(i = 0; i < iNumBins; i++) {
    pBin++;
    fscanf(fp1, "%d: %d\n", &iA, &iB);
    if(iB > 0) {
      pBin->piGenes = NULL;
      if((pBin->piGenes = (int *) calloc(iB, sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBin[%d]->piSrcs[%d].\n",
    pcFName, i, iB);
  exit(-2); }
    }
    pBin->isNumGenes = 0;
    pBin->isMaxGenes = iB;
    fscanf(fp1, "\n");
  }

  fclose(fp1); /* No more inputs from file: close it */
  
  piBin = piBinPerGene;
  for(i = 0; i < iNumGenes; i++) {
    iBin = *piBin++;
    pBin = pMBucket->pBin+iBin;
    iNum = pBin->isNumGenes;
    piBinGene = pBin->piGenes+iNum;
    *piBinGene = i;
    pBin->isNumGenes++; iNum++;
    if(iNum > pBin->isMaxGenes) {
  fprintf(stderr, "ERROR(%s): Overflow in genes for bin no. %d\n",
    pcFName, iBin);
  exit(-2); }
  }

  /* ---- 4th: Copy Gene Data from Bins to Buckets ---- */

  pBuck = pMBucket->pBucket-1;
  for(i = 0; i < iNumBucks; i++) {
    pBuck++;
    pBuck->iInitSheetLabel = -1;
    iA = pBuck->iBinFirst;
    iB = pBuck->iBinLast + 1;
    iC = pBuck->isNumGenes;
    iD = 0;
    pBuck->piGenes = NULL;

    if(iC > 0) {
      if((pBuck->piGenes = (int *) calloc(iC, sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBuck[%d]->piGenes[%d].\n",
    pcFName, i, iC);
  exit(-2); }

      piBuckGene = pBuck->piGenes;
      for(j = iA; j < iB; j++) {
  pBin = pMBucket->pBin+j;
  iNum = pBin->isNumGenes;
  if(iNum < 1) continue;
  piBinGene = pBin->piGenes;
  for(k = 0; k < iNum; k++) {
    iGene = *piBinGene++;
    *piBuckGene++ = iGene;
    iD++;
  }
  free(pBin->piGenes);
      }
      if(iD != iC) {
  fprintf(stderr,
    "ERROR(%s): Loaded %d genes instead of %d into Buckets[%d].\n",
    pcFName, iD, iC, i);
  exit(-2); }
    }

    pBuck++;
  }  
  free(pMBucket->pBin);
  pMBucket->pBin = NULL;

  return(TRUE);
}
#endif

/*------------------------------------------------------------*\
 * Function: SaveTimings
 *
 * Usage:  Save Timing Data
 *
\*------------------------------------------------------------*/
#if 0
void
SaveTimings(char *pcInFile, char *pcOutFile, Timings *pTime,
      ShockScaffold *pScaffold, Dimension *pDim, time_t *pTimeInit)
{
  char    *pcFName = "SaveTimings";
  FILE    *f1;
  register int  i;
  int    iNum, iTmp, iIncr, iItera, iLastItera, iMaxItera;
  float    fMoveX,fMoveY,fMoveZ, fPercent;
  time_t  time_now;

  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if(f1 == NULL) {
    fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); }
  
  /* First: Save Initial Source Data */

  fprintf(stderr, "MESG(%s): Saving Information in\n\t%s\n",
    pcFName, pcOutFile);

  fprintf(f1, "-----------\n");
  fprintf(f1, "%s", ctime(pTimeInit));

  iNum = pDim->iNum3dPts;
  fMoveX = pDim->fMoveX; fMoveY = pDim->fMoveY; fMoveZ = pDim->fMoveZ;

  fprintf(f1, "-----------\n");
  fprintf(f1, "Input filename: %s\n", pcInFile);
  fprintf(f1, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n",
    pDim->iXdim, pDim->iYdim, pDim->iZdim);
  fprintf(f1, "(Xmove, Ymove, Zmove) = (%f , %f , %f).\n",
    fMoveX, fMoveY, fMoveZ);
  fprintf(f1, "Offset = %f.\n", pDim->fOffset);
  fprintf(f1, "Number of input 3D pts (sources): %d\n", iNum);
  fprintf(f1, "-----------\n");

  iLastItera = pScaffold->isLastItera;
  iMaxItera  = pScaffold->isMaxItera;
  fprintf(f1, "Iterations to build Scaffold = %d (Max = %d).\n",
    iLastItera, iMaxItera);
  fprintf(f1,
    "Shocks: Sheets = %d , Curves = %d , Vertices = %d , Total = %d \n",
    pScaffold->iBlueActiveLast, pScaffold->iGreenActiveLast,
    pScaffold->iNumRedLast, pScaffold->iTotalShocks);
  fprintf(f1, "-----------\n");

  fprintf(f1, "Details on Sheets:\n");
  fprintf(f1, "Internal to buckets = %d , Sub-res. = %d \n",
    pScaffold->iNumBlueInt, pScaffold->iNumBlueSub);
  fprintf(f1, "Isolated = %d , Triangles = %d , Tetrahedra = %d\n",
    pScaffold->iNumBlueIsolated, pScaffold->iNumBlueFromGreen,
    pScaffold->iNumBlueFromRed);
  fprintf(f1, "Min/Max number of sheets per Generator = (%d , %d)\n",
    pScaffold->iMinBluePerGene, pScaffold->iMaxBluePerGene);
  fprintf(f1, "-----------\n");

  fprintf(f1, "Details on Curves:\n");
  fprintf(f1, "Passive = %d , Active = %d\n", pScaffold->iNumGreenPassive,
    pScaffold->iNumGreenActive);
  fprintf(f1, "Ends: 0 = %d , 1 = %d , 2 = %d\n", pScaffold->iNumGreenNoEnd,
    pScaffold->iNumGreenOneEnd, pScaffold->iNumGreenTwoEnds);
  fprintf(f1, "Triangles: Acute = %d , Obtuse = %d , Right = %d\n",
    pScaffold->iNumTriAcute, pScaffold->iNumTriObtuse,
    pScaffold->iNumTriRight);
  fprintf(f1, "Source: Sheets = %d , Tet. Caps = %d , Tet. Slivers = %d\n",
    pScaffold->iNumGreenFromSheets, pScaffold->iNumGreenFromCaps,
    pScaffold->iNumGreenFromSlivers);
  fprintf(f1, "Min/Max number of curves per Generator = (%d , %d)\n",
    pScaffold->iMinGreenPerGene, pScaffold->iMaxGreenPerGene);
  fprintf(f1, "-----------\n");

  fprintf(f1, "Details on Vertices:\n");
  fprintf(f1, "Tetrahedra: Regular = %d , Caps = %d , Slivers = %d\n",
    pScaffold->iNumTetRegulars, pScaffold->iNumTetCaps,
    pScaffold->iNumTetSlivers);
  fprintf(f1, "Min/Max number of vertices per Generator = (%d , %d)\n",
    pScaffold->iMinRedPerGene, pScaffold->iMaxRedPerGene);
  fprintf(f1, "===========\n");

  fprintf(f1, "Scaffold construction details (per step):\n");
  fprintf(f1, "===========\n");

  fprintf(f1, "Initialization: Time = %.3f secs. , Memory = %.2f Megs.\n",
    (pTime->fSysInit + pTime->fUserInit), pTime->fMemInit);

  if(pDim->iNumSubSamples > 1) {
    fprintf(f1, "Sub-sampling: Time = %.3f secs. , Memory = %.2f Megs.\n",
      (pTime->fSysSub + pTime->fUserSub), pTime->fMemSub);
    fprintf(f1, "\tNumber of Blue shocks = %d\n", pScaffold->iNumBlueSub);
  }

  fprintf(f1,
    "Inside Buckets - Sheets: Time = %.3f secs. , Memory = %.2f Megs.\n",
    (pTime->fSysBlueInt + pTime->fUserBlueInt), pTime->fMemBlueInt);
  fprintf(f1, "\tNumber of Blue shocks = %d\n", pScaffold->iNumBlueInt);
  fprintf(f1,
    "Inside Buckets - All: Time = %.3f secs. , Memory = %.2f Megs.\n",
    (pTime->fSysAllInt + pTime->fUserAllInt), pTime->fMemAllInt);
  fprintf(f1, "\tNew shocks: Blue = %d , Green = %d , Red = %d\n",
    pScaffold->iNumBlueInt, pScaffold->iNumGreenInt,
    pScaffold->iNumRedInt);

  fprintf(f1,
    "1st layer - Sheets: Time = %.3f secs. , Memory = %.2f Megs.\n",
    (pTime->fSysBlueFst + pTime->fUserBlueFst), pTime->fMemBlueFst);
  fprintf(f1, "\tNew Blue shocks = %d\n", pScaffold->iNumBlueFst);
  fprintf(f1,
    "1st layer - All: Time = %.3f secs. , Memory = %.2f Megs.\n",
    (pTime->fSysAllFst + pTime->fUserAllFst), pTime->fMemAllFst);
  fprintf(f1, "\tNew shocks: Blue = %d , Green = %d , Red = %d \n",
    pScaffold->iNumBlueFst, pScaffold->iNumGreenFst,
    pScaffold->iNumRedFst);

  fprintf(f1,
    "Other layers - Sheets: Time = %.3f secs. , Memory = %.2f Megs.\n",
    (pTime->fSysBlueOth + pTime->fUserBlueOth), pTime->fMemBlueOth);
  fprintf(f1, "\tNew Blue shocks = %d , Iterations = %d\n",
    pScaffold->iNumBlueOth, pScaffold->isBlueItera);

  fprintf(f1,
    "Other layers (Itera 0) - All: Time = %.3f secs. , Memory = %.2f Megs.\n",
    (pTime->fSysAllOth + pTime->fUserAllOth), pTime->fMemAllOth);
  fprintf(f1, "\tNew shocks: Blue = %d , Green = %d , Red = %d\n",
    pScaffold->iNumBlueOth, pScaffold->iNumGreenOth,
    pScaffold->iNumRedOth);
  fprintf(f1, "\tThus far: B = %d , G = %d , R = %d , Sum = %d\n",
    pScaffold->piNumBluePerItera[0], pScaffold->piNumGreenPerItera[0],
    pScaffold->piNumRedPerItera[0], pScaffold->piNumShocksPerItera[0]);
  fprintf(f1,
    "Other layers (Itera 1 - %d) - All: Time = %.3f secs. , Memory = %.2f Megs.\n",
    iLastItera, (pTime->fSysItera + pTime->fUserItera),
    pTime->fMemItera);
  fprintf(f1, "\tNew  : B = %d , G = %d , R = %d \n",
    pScaffold->iNumBlueItera, pScaffold->iNumGreenItera,
    pScaffold->iNumRedItera);
  fprintf(f1, "\tTotal: B = %d , G = %d , R = %d , Sum = %d\n",
    (pScaffold->iNumBlueItera + pScaffold->piNumBluePerItera[0]),
    (pScaffold->iNumGreenItera + pScaffold->piNumGreenPerItera[0]),
    (pScaffold->iNumRedItera + pScaffold->piNumRedPerItera[0]),
    (pScaffold->iBlueActiveLast + pScaffold->iGreenActiveLast +
     pScaffold->iNumRedLast));
  fprintf(f1, "===========\n");

  fprintf(f1, "Scaffold construction details (iterations 0 - %d):\n",
    iLastItera);
  fprintf(f1, "===========\n");

  fprintf(f1, "Itera: Blue - Green - Red - (Sum, Incr., %%) - Time (sec)\n");
  fprintf(f1, "-----------\n");
  
  iItera = 0;
  iTmp = pScaffold->piNumShocksPerItera[iItera];
  fprintf(f1, "%d: B = %d , G = %d , R = %d , ", iItera,
    pScaffold->piNumBluePerItera[iItera],
    pScaffold->piNumGreenPerItera[iItera],
    pScaffold->piNumRedPerItera[iItera]);
  fprintf(f1, "Sum = %d , T = %.2f\n", iTmp,
    pScaffold->pfTimePerItera[iItera]);

  for(i = 0; i < iLastItera; i++) {
    iItera++;
    fprintf(f1, "%d: B = %d , G = %d , R = %d , ", iItera,
      pScaffold->piNumBluePerItera[iItera],
      pScaffold->piNumGreenPerItera[iItera],
      pScaffold->piNumRedPerItera[iItera]);
    iNum = pScaffold->piNumShocksPerItera[iItera];
    iIncr = iNum - iTmp;
    if(iIncr < 0) {
      fprintf(stderr, "WARNING(%s): At iteration %d, there were %d shocks,\n",
        pcFName, i, iNum);
      fprintf(stderr, "\tand a change of %d from previous iteration.\n",
        iIncr);
      fprintf(f1, "\n");
      break;
    }
    if(iIncr > 0)
      fPercent = ((float) iIncr * 100.0) / (float) iNum ;
    else
      fPercent = 0.0;
    fprintf(f1, "Sum = (%d , %d , %.2f %%), T = %.2f\n", iNum, iIncr,
      fPercent, pScaffold->pfTimePerItera[iItera]);
    iTmp = iNum;
  }
  fprintf(f1, "===========\n");
  fprintf(f1, "Total: B = %d , G = %d , R = %d , Sum = %d , Time = %.3f\n",
    (pScaffold->iNumBlueItera + pScaffold->piNumBluePerItera[0]),
    (pScaffold->iNumGreenItera + pScaffold->piNumGreenPerItera[0]),
    (pScaffold->iNumRedItera + pScaffold->piNumRedPerItera[0]),
    (pScaffold->iBlueActiveLast + pScaffold->iGreenActiveLast +
     pScaffold->iNumRedLast),
    (pTime->fSysItera + pTime->fUserItera +
     pScaffold->pfTimePerItera[0]));
  fprintf(f1, "===========\n");

  time_now = time(NULL);
  fprintf(f1, "%s", ctime(&time_now));
  fprintf(f1, "-----------\n");

  fclose(f1);

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: SaveShockData
 *
 * Usage: Save Shock Data in an ASCII format.
 *
\*------------------------------------------------------------*/

void
SaveShockData(char *pcInFile, char *pcOutFile, InputData *pInData,
        ShockScaffold *pScaf, Dimension *pDimMBuck,
        Dimension *pDimAll)
{
  char    *pcFName = "SaveShockData";
  register int  i,j;
  int    iNumGenes, iNumSheets, iNumCurves, iNumVertices, iRed;
  int    iTmpBlue, iTmpGreen, iTmpRed, iType,iTypeI, iTmp, iGreen;
  int    iMinBlue,iMinGreen,iMinRed, iMaxBlue,iMaxGreen,iMaxRed;
  int    iGeneA,iGeneB,iGeneC,iGeneD, iMaxRedPerBlue, iNumRed;
  int    iMaxGreenPerRed, iMinGreenPerRed, iNumGreenPerRed, iGene;
  int    *piRed;
  float    fDist, fMin,fMax, fAvg, fLengthMin, fLengthMax;
  float    fPosXa,fPosYa,fPosZa, fPosXb,fPosYb,fPosZb, fDSq;
  float    fPABx,fPABy,fPABz, fPosXs,fPosYs,fPosZs, fArea;
  float    fDX,fDY,fDZ, fMinArea,fMaxArea, fMinL,fMaxL;
  double  dGeneA[3],dGeneB[3],dGeneC[3],dGeneD[3],dCircum[3];
  FILE    *f1;
  InputData  *pData, *pDataA, *pDataB, *pDataC, *pDataD;
  Pt3dCoord  *p3dPts, *pGeneA, *pGeneB, *pGeneC, *pGeneD;
  ShockSheet  *pBlue;
  ShockCurve  *pGreen;
  ShockVertex  *pRed;
  ShockData  *pShockData;

  /* Preliminaries */

  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }

  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if(f1 == NULL) {
    fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); }
  
  fprintf(stderr, "MESG(%s): Saving data in\n\t%s\n", pcFName, pcOutFile);

  iNumGenes = pDimMBuck->iNum3dPts;
  fprintf(f1, "GENES: Total = %d\n", iNumGenes);

  iNumSheets = pScaf->iBlueActiveLast;
  fprintf(f1, "SHEETS: Total = %d\n", iNumSheets);

  iNumCurves = pScaf->iGreenActiveLast;
  fprintf(f1, "CURVES: Total = %d\n", iNumCurves);

  iNumVertices = pScaf->iNumRedLast;
  fprintf(f1, "VERTICES: Total = %d\n", iNumVertices);

  fprintf(f1, "Input filename: %s\n", pcInFile);
  fprintf(f1, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n",
    pDimMBuck->iXdim, pDimMBuck->iYdim, pDimMBuck->iZdim);
  fprintf(f1, "Max/Min: %.2f < X < %.2f, %.2f < Y < %.2f, %.2f < Z < %.2f\n",
    pDimMBuck->fMinX,pDimMBuck->fMaxX, pDimMBuck->fMinY,pDimMBuck->fMaxY,
    pDimMBuck->fMinZ,pDimMBuck->fMaxZ);

  /* -- 1st: Save Initial Source Data -- */
  fprintf(f1, "----------- %d GENES\n", iNumGenes);

  fprintf(f1, "ID: Coords (x,y,z), Shocks: (Blue,Green,Red)\n");
  fprintf(f1, "-----------\n");

  iMinBlue = iMinGreen = iMinRed = 100;
  iMaxBlue = iMaxGreen = iMaxRed = 0;
  pData = pInData;
  for(i = 0; i < iNumGenes; i++) {
    p3dPts = &(pData->Sample);
    iTmpBlue = pData->isNumBlue;
    iTmpGreen = pData->isNumGreen;
    iTmpRed = pData->isNumRed;

    fprintf(f1, "%d: (%f , %f , %f), (%d , %d , %d)\n", i,
      p3dPts->fPosX, p3dPts->fPosY, p3dPts->fPosZ,
      iTmpBlue, iTmpGreen, iTmpRed);

    if(iMinBlue > iTmpBlue) iMinBlue = iTmpBlue;
    if(iMaxBlue < iTmpBlue) iMaxBlue = iTmpBlue;
    if(iMinGreen > iTmpGreen) iMinGreen = iTmpGreen;
    if(iMaxGreen < iTmpGreen) iMaxGreen = iTmpGreen;
    if(iMinRed > iTmpRed) iMinRed = iTmpRed;
    if(iMaxRed < iTmpRed) iMaxRed = iTmpRed;

    pData++;
  }
  fprintf(f1, "-----------\n");
  fAvg = (float) pScaf->iBlueActiveLast / (float) iNumGenes;
  fprintf(f1, "BluePerGene: Min = %d , Max = %d , Avg = %f\n",
    iMinBlue, iMaxBlue, fAvg);
  fAvg = (float) pScaf->iGreenActiveLast / (float) iNumGenes;
  fprintf(f1, "GreenPerGene: Min = %d , Max = %d , Avg = %f\n",
    iMinGreen, iMaxGreen, fAvg);
  fAvg = (float) pScaf->iNumRedLast / (float) iNumGenes;
  fprintf(f1, "RedPerGene: Min = %d , Max = %d , Avg = %f\n",
    iMinRed, iMaxRed, fAvg);
  fprintf(f1, "-----------\n");

  /* -- 2nd: Save Blue Sheet Shock Data -- */

  fprintf(f1, "----------- %d SHEETS (0 Missed)\n", iNumSheets);
  fprintf(f1, "ID: Coord (x,y,z), Genes (a,b), Dist2Gene, Type\n");
  fprintf(f1, "    Bounded, Num Vertices, Red Vertices\n");
  fprintf(f1, "-----------\n");


  fMinL = fMin = 100000.0;
  fMaxL = fMax = 0.0;
  iTypeI = iMaxRedPerBlue = 0;
  pBlue = pScaf->pBlue;
  for(i = 0; i < iNumSheets; i++) {
    iGeneA = pBlue->iGene[0];
    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    fPosXa = pGeneA->fPosX;
    fPosYa = pGeneA->fPosY;
    fPosZa = pGeneA->fPosZ;
    iGeneB = pBlue->iGene[1];
    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    fPosXb = pGeneB->fPosX;
    fPosYb = pGeneB->fPosY;
    fPosZb = pGeneB->fPosZ;
    fPABx = fPosXb - fPosXa;
    fPABy = fPosYb - fPosYa;
    fPABz = fPosZb - fPosZa;
    fDSq  = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
    fDSq *= 0.25;  /* Divided by half^2 */
    fPosXs = 0.5 * (fPosXa + fPosXb); /* Potential Shock coords. */
    fPosYs = 0.5 * (fPosYa + fPosYb);
    fPosZs = 0.5 * (fPosZa + fPosZb);
    fDist = (float) sqrt((double) fDSq);
    iType = (int) pBlue->isInitType;
    /* fprintf(f1, "%d: (%f , %f , %f), %d , (%d , %d), %f , %d , %d\n", i,
       fPosXs, fPosYs, fPosZs, pBlue->iBuckXYZ, iGeneA, iGeneB, fDist,
       pBlue->isItera, iType); */
    fprintf(f1, "%d: (%f , %f , %f), (%d , %d), %f , %d\n", i,
      fPosXs, fPosYs, fPosZs, iGeneA, iGeneB, fDist, iType);
    if(fMin > fDist) fMin = fDist;
    if(fMax < fDist) fMax = fDist;

    iNumRed = pBlue->isNumRed;
    if(iNumRed > iMaxRedPerBlue) iMaxRedPerBlue = iNumRed;
    fprintf(f1, "    %d, %d: ", (int) pBlue->isBounded, iNumRed);
    piRed = pBlue->piRedList;
    for(j = 0; j < iNumRed; j++) {
      iRed = *piRed++;
      fprintf(f1, "%d ", iRed);
    }
    fprintf(f1, "\n");

    pBlue++;
  }
  fprintf(f1, "-----------\n");
  fprintf(f1, "Radii: Min = %f , Max = %f\n", fMin, fMax);
  fprintf(f1, "Max Num of Vertices per sheet = %d\n",
    iMaxRedPerBlue);

  fprintf(f1, "-----------\n");

  /* -- 3rd: Save Green Curve Shock Data -- */

  fprintf(f1, "----------- %d CURVES\n", iNumCurves);
  fprintf(f1, "ID: Coord (x,y,z), Genes (a,b,c), InitType , Type , Valid\n");
  fprintf(f1, "    Blue (1,2,3), Red (1,2), Dist2Gene\n");
  fprintf(f1, "    Area , Perimeter , SideLength (Min , Max)\n");

  fprintf(f1, "-----------\n");

  fMinArea = fMin = 100000.0;
  fMaxArea = fMax = 0.0;
  iTypeI = 0;
  pGreen = pScaf->pGreen-1;
  for(i = 0; i < iNumCurves; i++) {
    pGreen++;
    iGeneA = pGreen->iGene[0];
    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    dGeneA[0] = (double) pGeneA->fPosX;
    dGeneA[1] = (double) pGeneA->fPosY;
    dGeneA[2] = (double) pGeneA->fPosZ;
    iGeneB = pGreen->iGene[1];
    pDataB = pInData+iGeneB;
    pGeneB  = &(pDataB->Sample);
    dGeneB[0] = (double) pGeneB->fPosX;
    dGeneB[1] = (double) pGeneB->fPosY;
    dGeneB[2] = (double) pGeneB->fPosZ;
    iGeneC = pGreen->iGene[2];
    pDataC = pInData+iGeneC;
    pGeneC = &(pDataC->Sample);
    dGeneC[0] = (double) pGeneC->fPosX;
    dGeneC[1] = (double) pGeneC->fPosY;
    dGeneC[2] = (double) pGeneC->fPosZ;
    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;
    if(!tri_circum_3d(dGeneA,dGeneB,dGeneC,dCircum)) {
      /* Offshoot to infinity */
      fprintf(stderr, "ERROR(%s): Green shock %d OffShoots to Infty.\n",
        pcFName, i);
      continue; /* i++ */
    }
    fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
    fDZ = (float) dCircum[2];
    fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
    fDist = (float) sqrt((double) fDSq);
    fPosXs = fDX + (float) dGeneA[0];
    fPosYs = fDY + (float) dGeneA[1];
    fPosZs = fDZ + (float) dGeneA[2];

    fprintf(f1, "%d: (%f , %f , %f), (%d , %d , %d), %d , %d , %d\n", i,
      fPosXs, fPosYs, fPosZs, iGeneA, iGeneB, iGeneC, 
      pGreen->isInitType, pGreen->isType, pGreen->isFlagValid);

    if(!GetTriCoordOfCurve(pInData, pGreen, pShockData)) {
      fprintf(stderr, "ERROR(%s): Tri coords failed on Green shock %d\n",
        pcFName, i);
      continue; }
    fArea = pShockData->fGeom;

    fprintf(f1, "\t(%d , %d , %d), (%d , %d), %f\n",
      pGreen->iBlue[0], pGreen->iBlue[1], pGreen->iBlue[2],
      pGreen->iRed[0], pGreen->iRed[1], fDist);
    fLengthMin = (float) sqrt((double) pGreen->fMinSideLengthSq);
    fLengthMax = (float) sqrt((double) pGreen->fMaxSideLengthSq);
    fprintf(f1, "\t%f , %f , (%f , %f)\n", pGreen->fTriArea,
      pGreen->fPerimeter, fLengthMin, fLengthMax);

    if(fMinL > fLengthMin) fMinL = fLengthMin;
    if(fMaxL < fLengthMax) fMaxL = fLengthMax;
    if(fMinArea > fArea) fMinArea = fArea;
    if(fMaxArea < fArea) fMaxArea = fArea;
    if(fMin > fDist) fMin = fDist;
    if(fMax < fDist) fMax = fDist;
  }
  fprintf(f1, "-----------\n");
  fprintf(f1, "Radii: Min = %f , Max = %f\n", fMin, fMax);
  fprintf(f1, "Triangle Side Lengths: Min = %f , Max = %f\n", fMinL, fMaxL);

  fprintf(f1, "-----------\n");


  /* -- 4th: Save Red Vertex Shock Data -- */

  fprintf(f1, "----------- %d VERTICES\n", iNumVertices);

  fprintf(f1, "ID: Coord (x,y,z), Type , Dist2Gene , Volume\n");
  fprintf(f1, "\tNum of Green: Curves\n");
  fprintf(f1, "\tNum of Genes: Genes\n");
  fprintf(f1, "-----------\n");

  iMaxGreenPerRed = iMinGreenPerRed = 4;
  fMinArea = fMin = 100000.0;
  fMaxArea = fMax = 0.0;
  iTypeI = 0;
  pRed = pScaf->pRed-1;
  for(i = 0; i < iNumVertices; i++) {
    pRed++;
    iGeneA = pRed->iGene[0];
    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    dGeneA[0] = (double) pGeneA->fPosX;
    dGeneA[1] = (double) pGeneA->fPosY;
    dGeneA[2] = (double) pGeneA->fPosZ;
    iGeneB = pRed->iGene[1];
    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    dGeneB[0] = (double) pGeneB->fPosX;
    dGeneB[1] = (double) pGeneB->fPosY;
    dGeneB[2] = (double) pGeneB->fPosZ;
    iGeneC = pRed->iGene[2];
    pDataC = pInData+iGeneC;
    pGeneC = &(pDataC->Sample);
    dGeneC[0] = (double) pGeneC->fPosX;
    dGeneC[1] = (double) pGeneC->fPosY;
    dGeneC[2] = (double) pGeneC->fPosZ;
    iGeneD = pRed->iGene[3];
    pDataD = pInData+iGeneD;
    pGeneD  = &(pDataD->Sample);
    dGeneD[0] = (double) pGeneD->fPosX;
    dGeneD[1] = (double) pGeneD->fPosY;
    dGeneD[2] = (double) pGeneD->fPosZ;
    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;
    if(!tet_circum(dGeneA,dGeneB,dGeneC,dGeneD,dCircum)) {
      /* Offshoot to infinity */
      fprintf(stderr, "ERROR(%s): Red shock %d OffShoots to Infty.\n",
        pcFName, i);
      continue; /* i++ */
    }
    fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
    fDZ = (float) dCircum[2];
    fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
    fDist = (float) sqrt((double) fDSq);
    fPosXs = fDX + (float) dGeneA[0];
    fPosYs = fDY + (float) dGeneA[1];
    fPosZs = fDZ + (float) dGeneA[2];
    if(!GetQuadCoordOfVertex(pInData, pRed, pShockData)) {
      fprintf(stderr,
        "\tWARNING(%s): Quad coords failed on Red shock %d\n",
        pcFName, i);
      continue; }
    fArea = pShockData->fGeom;
    iType = pRed->isType;
    fprintf(f1, "%d: (%f , %f , %f), %d , %f , %f\n", i,
      fPosXs, fPosYs, fPosZs, iType, fDist, fArea);

    fprintf(f1, "\t4: ");
    iNumGreenPerRed = 4;
    for(j = 0; j < 4; j++) {
      iGreen = pRed->iGreen[j];
      fprintf(f1, "%d ", iGreen);
      if(iGreen < 0)
  iNumGreenPerRed--;
    }
    fprintf(f1, "\n");
    if(iNumGreenPerRed < iMinGreenPerRed)
      iMinGreenPerRed = iNumGreenPerRed;

    fprintf(f1, "\t4: ");
    for(j = 0; j < 4; j++) {
      iGene = pRed->iGene[j];
      fprintf(f1, "%d ", iGene);
    }
    fprintf(f1, "\n");

    if(fMinArea > fArea) fMinArea = fArea;
    if(fMaxArea < fArea) fMaxArea = fArea;
    if(fMin > fDist) fMin = fDist;
    if(fMax < fDist) fMax = fDist;
  }
  fprintf(f1, "-----------\n");
  fprintf(f1, "Radii: Min = %f , Max = %f\n", fMin, fMax);
  fprintf(f1, "(Min , Max) Green Curves per Red Vertex = (%d , %d)\n",
    iMinGreenPerRed, iMaxGreenPerRed);

  fprintf(f1, "-----------\n");

  fprintf(f1, "Vertex at Infinity: 0 genes:\n");
  fprintf(f1, "-----------\n");

  fclose(f1);

  return;
}


bool save_fs_ply2 (char *pcInFile, char *pcOutFile, InputData *pInData,
        ShockScaffold *pScaf, Dimension *pDimMBuck,
        Dimension *pDimAll)
{
  char    *pcFName = "SaveShockData";
  register int  i,j;
  int    iNumGenes, iNumSheets, iNumCurves, iNumVertices, iRed;
  int    iTmpBlue, iTmpGreen, iTmpRed, iType,iTypeI, iTmp, iGreen;
  int    iMinBlue,iMinGreen,iMinRed, iMaxBlue,iMaxGreen,iMaxRed;
  int    iGeneA,iGeneB,iGeneC,iGeneD, iMaxRedPerBlue, iNumRed;
  int    iMaxGreenPerRed, iMinGreenPerRed, iNumGreenPerRed, iGene;
  int    *piRed;
  float    fDist, fMin,fMax, fAvg, fLengthMin, fLengthMax;
  float    fPosXa,fPosYa,fPosZa, fPosXb,fPosYb,fPosZb, fDSq;
  float    fPABx,fPABy,fPABz, fPosXs,fPosYs,fPosZs, fArea;
  float    fDX,fDY,fDZ, fMinArea,fMaxArea, fMinL,fMaxL;
  double  dGeneA[3],dGeneB[3],dGeneC[3],dGeneD[3],dCircum[3];
  InputData  *pData, *pDataA, *pDataB, *pDataC, *pDataD;
  Pt3dCoord  *p3dPts, *pGeneA, *pGeneB, *pGeneC, *pGeneD;
  ShockSheet  *pBlue;
  ShockCurve  *pGreen;
  ShockVertex  *pRed;
  ShockData  *pShockData;


  // #########################################################
  //: Open the *.fs file to write.
  FILE*   fp = fopen (pcOutFile, "w");
  if (fp == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open file %s.\n", pcOutFile);
    return false; 
  }
  vul_printf (vcl_cout, "\nsave_fs_ply2 (%s)\n", pcOutFile);
  
  pShockData = NULL;
  if((pShockData = (ShockData*)
      calloc(1, sizeof(ShockData))) == NULL) {
    fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
      pcFName);
    exit(-4); }

  // ################ A1) Point Generators
  //     Can put the Mesh connectivity here!
  iNumGenes = pDimMBuck->iNum3dPts;
  iNumSheets = pScaf->iBlueActiveLast;
  iNumCurves = pScaf->iGreenActiveLast;
  iNumVertices = pScaf->iNumRedLast;

  //Count the number of valid shock sheets
  int iNumValidSheets = 0;

  pBlue = pScaf->pBlue;
  for(i = 0; i < iNumSheets; i++) {
    if (pBlue->isNumRed > 2)
      iNumValidSheets++;

    pBlue++;
  }

  fprintf (fp, "%d\n", iNumVertices);
  fprintf (fp, "%d\n", iNumValidSheets);

  // ###########:B1) Save NodeElements################ SHOCK NODES  

  iMaxGreenPerRed = iMinGreenPerRed = 4;
  fMinArea = fMin = 100000.0;
  fMaxArea = fMax = 0.0;
  iTypeI = 0;
  pRed = pScaf->pRed-1;
  for(i = 0; i < iNumVertices; i++) {
    pRed++;
    iGeneA = pRed->iGene[0];
    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    dGeneA[0] = (double) pGeneA->fPosX;
    dGeneA[1] = (double) pGeneA->fPosY;
    dGeneA[2] = (double) pGeneA->fPosZ;
    iGeneB = pRed->iGene[1];
    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    dGeneB[0] = (double) pGeneB->fPosX;
    dGeneB[1] = (double) pGeneB->fPosY;
    dGeneB[2] = (double) pGeneB->fPosZ;
    iGeneC = pRed->iGene[2];
    pDataC = pInData+iGeneC;
    pGeneC = &(pDataC->Sample);
    dGeneC[0] = (double) pGeneC->fPosX;
    dGeneC[1] = (double) pGeneC->fPosY;
    dGeneC[2] = (double) pGeneC->fPosZ;
    iGeneD = pRed->iGene[3];
    pDataD = pInData+iGeneD;
    pGeneD  = &(pDataD->Sample);
    dGeneD[0] = (double) pGeneD->fPosX;
    dGeneD[1] = (double) pGeneD->fPosY;
    dGeneD[2] = (double) pGeneD->fPosZ;
    dCircum[0] = 0.0; dCircum[1] = 0.0; dCircum[2] = 0.0;
    if(!tet_circum(dGeneA,dGeneB,dGeneC,dGeneD,dCircum)) {
      /* Offshoot to infinity */
      fprintf(stderr, "ERROR(%s): Red shock %d OffShoots to Infty.\n",
        pcFName, i);
      continue; /* i++ */
    }
    fDX = (float) dCircum[0]; fDY = (float) dCircum[1];
    fDZ = (float) dCircum[2];
    fDSq = fDX*fDX + fDY*fDY + fDZ*fDZ;
    fDist = (float) sqrt((double) fDSq);
    fPosXs = fDX + (float) dGeneA[0];
    fPosYs = fDY + (float) dGeneA[1];
    fPosZs = fDZ + (float) dGeneA[2];
    if(!GetQuadCoordOfVertex(pInData, pRed, pShockData)) {
      fprintf(stderr,
        "\tWARNING(%s): Quad coords failed on Red shock %d\n",
        pcFName, i);
      continue; }
    fArea = pShockData->fGeom;
    iType = pRed->isType;
    fprintf(fp, "%f %f %f\n", fPosXs, fPosYs, fPosZs);

    ///fprintf(f1, "\t4: ");
    iNumGreenPerRed = 4;
    for(j = 0; j < 4; j++) {
      iGreen = pRed->iGreen[j];
      ///fprintf(f1, "%d ", iGreen);
      if(iGreen < 0)
        iNumGreenPerRed--;
    }
    ///fprintf(f1, "\n");
    if(iNumGreenPerRed < iMinGreenPerRed)
      iMinGreenPerRed = iNumGreenPerRed;

    ///fprintf(f1, "\t4: ");
    for(j = 0; j < 4; j++) {
      iGene = pRed->iGene[j];
      ///fprintf(f1, "%d ", iGene);
    }
    ///fprintf(f1, "\n");

    if(fMinArea > fArea) fMinArea = fArea;
    if(fMaxArea < fArea) fMaxArea = fArea;
    if(fMin > fDist) fMin = fDist;
    if(fMax < fDist) fMax = fDist;
  }
 
  //##############:B1) Save A12PatchElements
  fMinL = fMin = 100000.0;
  fMaxL = fMax = 0.0;
  iTypeI = iMaxRedPerBlue = 0;
  pBlue = pScaf->pBlue;
  for(i = 0; i < iNumSheets; i++) {
    iGeneA = pBlue->iGene[0];
    pDataA = pInData+iGeneA;
    pGeneA = &(pDataA->Sample);
    fPosXa = pGeneA->fPosX;
    fPosYa = pGeneA->fPosY;
    fPosZa = pGeneA->fPosZ;
    iGeneB = pBlue->iGene[1];
    pDataB = pInData+iGeneB;
    pGeneB = &(pDataB->Sample);
    fPosXb = pGeneB->fPosX;
    fPosYb = pGeneB->fPosY;
    fPosZb = pGeneB->fPosZ;
    fPABx = fPosXb - fPosXa;
    fPABy = fPosYb - fPosYa;
    fPABz = fPosZb - fPosZa;
    fDSq  = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
    fDSq *= 0.25;  /* Divided by half^2 */
    fPosXs = 0.5 * (fPosXa + fPosXb); /* Potential Shock coords. */
    fPosYs = 0.5 * (fPosYa + fPosYb);
    fPosZs = 0.5 * (fPosZa + fPosZb);
    fDist = (float) sqrt((double) fDSq);
    iType = (int) pBlue->isInitType;
    ///fprintf (fp, "s %d (%d-%d) V %s\n", i, iGeneA, iGeneB,
              ///pBlue->isBounded ? "F" : "I");
    if(fMin > fDist) fMin = fDist;
    if(fMax < fDist) fMax = fDist;

    iNumRed = pBlue->isNumRed;
    if(iNumRed > iMaxRedPerBlue) iMaxRedPerBlue = iNumRed;
    ///fprintf(f1, "    %d, %d: ", (int) pBlue->isBounded, iNumRed);

    //Only output the valid shock sheet.
    if (pBlue->isNumRed > 2) {
      fprintf(fp, "%d ", iNumRed);
      piRed = pBlue->piRedList;
      for(j = 0; j < iNumRed; j++) {
        iRed = *piRed++;
        fprintf(fp, "%d ", iRed);
      }
      fprintf(fp, "\n");
    }

    pBlue++;
  }

  fclose (fp);
  return true;

}

/*------------------------------------------------------------*\
 * Function: CleanUpInData
 *
 * Usage: Frees memory
 *
\*------------------------------------------------------------*/

void
CleanUpInData(InputData *pInData, Dimension *pDim)
{
  char    *pcFName = "CleanUpInData";
  register int  i;
  int    iNumGenes;
  InputData  *pGene;
  
  iNumGenes = pDim->iNum3dPts;
  fprintf(stderr, "MESG(%s): for %d generators...\n", pcFName, iNumGenes);

  pGene = pInData;
  for(i = 0; i < iNumGenes; i++) {

    free(pGene->piBlueList);
    free(pGene->pvNormal);
    free(pGene->piGreenList);
    free(pGene->piRedList);
    pGene++;
  }

  //Ming a memory problem!
  ////free(pGene);

  return;
}

/*------------------------------------------------------------*\
 * Function: CleanUpMBuck
 *
 * Usage: Frees memory
 *
\*------------------------------------------------------------*/

void
CleanUpMBuck(Buckets *pBucksXYZ, Dimension *pDim)
{
  /* char    *pcFName = "CleanUpMBuck"; */
  register int  i;
  int    iNumBins, iNumBucks;
  Bucket  *pBuck;
  Bin    *pBin;

  iNumBins = pBucksXYZ->iNumBins;
  pBin = pBucksXYZ->pBin;
  for(i = 0; i < iNumBins; i++) {
    free(pBin->piGenes);
    pBin++;
  }

  iNumBucks = pBucksXYZ->iNumBucks;
  pBuck = pBucksXYZ->pBucket;
  for(i = 0; i < iNumBucks; i++) {
    free(pBuck->piGenes);
    free(pBuck->pucFlagNgb);
    free(pBuck->pucFlagViz);
    free(pBuck->pucFlagPair);
    free(pBuck->piPtrNgb);
    pBuck++;
  }

  return;
}

/*------------------------------------------------------------*\
 * Function: FillVoxels
 *
 * Usage: Input data per Voxel.
 *    Generators are assumed to be unique (no duplicates).
 *
\*------------------------------------------------------------*/

void
FillVoxels(char *pcInFile, Pt3dCoord *pGeneCoord, Voxel *pVoxels,
     Dimension *pDim)
{
  char    *pcFName = "FillVoxels";
  char    *pcTmp, cTmp[128] = "";
  FILE    *fp1;
  register int  i;
  int    iA,iB,iC, iNumGenes, iNumVoxels, iNumBinEmpty;
  int    iDelta, iNum;
  int    *piVoxGene;
  float    fA,fB,fC;
  Voxel    *pVox;
  Pt3dCoord  *pCoord;


  if((fp1 = fopen(pcInFile, "r")) == NULL) {
    fprintf(stderr, "ERROR(%s): Can't open input UPenn file %s\n",
      pcFName, pcInFile);
    exit(-2); }

  fprintf(stderr, "MESG(%s):\n", pcFName);

  /* ---- 1st: Load in Input Generator Data ---- */

  pcTmp = &cTmp[0];
  fscanf(fp1, "Input filename: %s\n", pcTmp);
  fscanf(fp1, "Voxel Data for (all) %d generators and %d voxels (bins),\n",
   &iNumGenes, &iNumVoxels);
  fscanf(fp1, "\tof which %d are empty.\n", &iNumBinEmpty);
  fscanf(fp1, "Min grid coords: X = %f , Y = %f , Z = %f\n",
   &fA,&fB,&fC);
  fscanf(fp1, "Max grid coords: X = %f , Y = %f , Z = %f\n",
   &fA,&fB,&fC);
  fscanf(fp1, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n",
   &iA,&iB,&iC);
  fscanf(fp1, "(Xmove, Ymove, Zmove) = (%f , %f , %f).\n",
   &fA,&fB,&fC);
  fscanf(fp1, "Offset = %f.\n", &fA);
  fscanf(fp1, "-----------\n");
  fscanf(fp1, "Voxel ID : Number of Genes > 0\n");
  fscanf(fp1, "-----------\n");

  fprintf(stderr, "\t%d voxels of which %d are empty...\n",
    iNumVoxels, iNumBinEmpty);

  iDelta = iNumVoxels - iNumBinEmpty;
  for(i = 0; i < iDelta; i++) {
    fscanf(fp1, "%d : %d\n", &iA, &iB);
    if(iA < 0 || iA > iNumVoxels) {
      fprintf(stderr,
        "ERROR(%s):\n\tVoxel label = %d is out of range [0,%d].\n",
        pcFName, iA, iNumVoxels);
      exit(-3);
    }
    pVox = pVoxels+iA;
    pVox->piGenes = NULL;
    if((pVox->piGenes = (int *) calloc(iB, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s):\n\tCALLOC failed for pVox[%d]->piGenes[%d].\n",
        pcFName, i, iB);
      exit(-2); }
  }

  fscanf(fp1, "-----------\n");
  fscanf(fp1, "Number of empty voxels = %d\n", &iA);
  fscanf(fp1, "(Min , Max) number of genes per voxel = (%d , %d)\n",
   &iA, &iB);
  fscanf(fp1, "-----------\n");
  fscanf(fp1, "Original Gene ID : Coords (x,y,z) : Bin label\n");
  fscanf(fp1, "-----------\n");

  for(i = 0; i < iNumGenes; i++) {
    
    fscanf(fp1, "%d : (%f , %f , %f) : %d\n", &iA, &fA,&fB,&fC, &iB);

    pCoord = pGeneCoord+iA;
    pCoord->fPosX = fA;
    pCoord->fPosY = fB;
    pCoord->fPosZ = fC;

    pVox = pVoxels+iB;
    iNum = pVox->isNumGenes++;
    piVoxGene = pVox->piGenes+iNum;
    *piVoxGene = iA;
  }

  fprintf(stderr, "\tfilled with %d genes.\n", iNumGenes);

  fclose(fp1);

  return;
}

/* --- EoF --- */
