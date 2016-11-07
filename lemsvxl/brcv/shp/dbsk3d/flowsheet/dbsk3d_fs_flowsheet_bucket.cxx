#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vgl/vgl_distance.h>

#include <dbnl/dbnl_cbrt.h>
#include <dbsk3d/flowsheet/dbsk3d_fs_flowsheet_bucket.h>

//################## File: sort3dpts_space_v3.c  ###########################

/*------------------------------------------------------------*\
 * File: sort3dpts_space_v3.c
 *
 * Functions:
 *  main: Sort points, in a 3D non-uniform grid made of
 *        parallelepipedal buckets which adapt their
 *        boundaries to the local data density.
 *        Saves results as meta-buckets in different files.
 *        Each meta-bucket is like a smaller data-set and it
 *        is then partitioned in buckets as before.
 *
 * History:
 *  #0: July 2001, by F.Leymarie
 *  #1: April 2003: removed/debugged failing cases
 *  #2: July 2003: meta-buckets option
 *
\*------------------------------------------------------------*/

/*#include "sort3dpts_space.h"
#include "sort3dpts_io.h"
#include "string.h"
#include "cbrt.h" //// cbrt is not part of ANSI C, here is my own version of it.
#include "fill_buckets.h"
#include "sort3dpts_utils.h"*/

#include <vcl_iostream.h>
///using namespace std;


float  fRadMax = 0.0;
int  iSingleMetaBuck = TRUE;
int  iBuckAvg = 20;
int  iBuckMetaSize = 10000;
float  fTolerance = 5.0;
int  iFlagConvexHull = FALSE;
int  iFlagDuplic = FALSE;
int  iFlag2D = FALSE;
int  iFlagSaveG3d = FALSE;
float  fOffset = 1.0;

/*static OPTTABLE optab [] = {
  {"i", STRING, 0, &pcFileNameIn, "Input (*.g3d) FileName Prefix"},
  {"rmax", FLOAT, 0, &fRadMax,
   "Maximum Ball Radius (Use value if > 1, otherwise use Max Bounding Box)"},
  {"one", INT, 0, &iSingleMetaBuck,
   "Process data in one single MetaBucket: TRUE = 1 (def.), FALSE = 0"},
  {"avg", INT, 0, &iBuckAvg,
   "Target average number of genes per bucket (def. = 20)"},
  {"m", INT, 0, &iBuckMetaSize,
   "Meta-Buckets0: target number of genes (def. = 10000)"},
  {"tol", FLOAT, 0, &fTolerance,
   "Tolerance (in percent) on number of sources per bucket (def. = 5.0)."},
  {"fch", INT, 0, &iFlagConvexHull,
   "Compute Convex Hull? FALSE = 0 (def.), TRUE = 1"},
  {"fd", INT, 0, &iFlagDuplic,
   "Check for duplicates? FALSE = 0 (def.), TRUE = 1"},
  {"two", INT, 0, &iFlag2D, "2D dataset? FALSE = 0 (def.), TRUE = 1"},
  {"g3d", INT, 0, &iFlagSaveG3d,
   "Save metabuckets as G3D files? FALSE = 0 (def.), TRUE = 1"},
  {"off", FLOAT, 0, &fOffset,
   "Offset: sausage around data (def. = 1.0)"},
  {(char*)0, (OPT_TYPE)0, (int*)0, (void*)0, (char*)0} 
};*/

/*------------------------------------------------------------*\
 * Function: main
 *
 * Usage: Loads user inputs and launches Bucket_Space computation.
 *    Expects simple g3d ASCII format as input file:
 *    1 DataType (3 = 3D point coord), 2 number of points,
 *    3 floating pt. coord. and RGB float values.
 *
\*------------------------------------------------------------*/

void run_flowsheet_bucketing (dbmsh3d_mesh* geneset,
                              const char* pcInFile)
{

  char  *pcFileNameIn = (char*) pcInFile;

  const char    *pcFName = "main(sort3dpts_space_v3)";
  char    *pcOutFile;
  char    cInFile[128]="", cOutFile[128]="", cTmp[128]="";
  FILE    *fp1;
  register int  i;
  int    iTmp, iX,iY,iZ, iNumBucksX, iNumBinsX, iNumGenes;
  int    iNumBinsXY, iNumBucksXY, iNumBinsXYZ, iNumBucksXYZ;
  int    iNumMetaBucket, iNumMetaBuckX, iNumMetaBuckXY;
  int    iNumMetaBuckXYZ, iNumGeneInMetaBuck, iNumBuckTarget;
  int    iXstart,iXend, iYstart,iYend, iNumBinsY;
  int    iZstart,iZend, iNumBinsZ;
  float    fAvgGene;
  double  dTmp;
  DimensionB  theDim;
  InputDataB  *pInData;
  Buckets0  *pBucketsX, theListOfBucketsX;
  Buckets0  *pBucketsXY, theListOfBucketsXY;
  Buckets0  *pBucketsXYZ, theListOfBucketsXYZ;
  Buckets0  *pMetaBuckX, theListOfMetaBuckX;
  Buckets0  *pMetaBuckXY, theListOfMetaBuckXY;
  Buckets0  *pMetaBuckXYZ, theListOfMetaBuckXYZ;
  Bucket0  *pMetaBuck;
  MetaBuckInfoB  *pMBuckInfo, theMetaBuckInfoList;
  BinB    *pBins;
  TimingsB  TimingsB, *pTime;


  /*-----------------------------------*\
   * Read in Arguments and set values  *
  \*-----------------------------------*/
  
  ///opt_parse_args (argc, argv, optab);        
  /*if (pcFileNameIn == NULL){
    vcl_fprintf(stderr, "ERROR(%s): Missing input (prefix) filename\n", pcFName);
    vcl_fprintf(stderr, "\n\t use -h for more help.\n");
    exit(-1);
  }*/
  if (iBuckAvg < 1 || iBuckAvg > 100000) {
    vcl_fprintf(stderr,
        "ERROR(%s): Incorrect average number of genes per buckets = %d .\n",
        pcFName, iBuckAvg);
    exit(-1); 
  }
  if(fTolerance <= 0.0 || fTolerance > 50.0) {
    vcl_fprintf(stderr, "ERROR(%s): Incorrect tolerance = %lf (in percent).\n",
        pcFName, fTolerance);
    exit(-1); 
  }
  if(iBuckMetaSize < 1 || iBuckMetaSize > 1000000) {
    vcl_fprintf(stderr, "ERROR(%s): Incorrect size of meta-buckets = %d genes.\n",
        pcFName, iBuckMetaSize);
    exit(-1); 
  }
  theDim.iMetaBuckSize = iBuckMetaSize;
  
  vcl_strcpy(cInFile, pcFileNameIn);
  strcat(cInFile, ".g3d");
  pcInFile = &cInFile[0];
  fp1 = NULL;
  if((fp1 = fopen(pcInFile, "r")) == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open input file %s\n",
        pcFName, pcInFile);
    exit(-1); 
  }
  if(fOffset < 0.1) {
    vcl_fprintf(stderr, "ERROR(%s): Offset invalid = %lf\n",
        pcFName, fOffset);
    exit(-2);
  }
  theDim.fOff = fOffset;
  theDim.fOffset = 0;    //Initialized it to be 0 on PC.
  
  vcl_fprintf(stderr,
      "\nMESG(%s):\n\tOffset = %lf (width of empty sausage added around).\n",
      pcFName, fOffset);
  vcl_fprintf(stderr,
      "\tTolerance on target number of points per bucket = %.2f%%\n",
      fTolerance);
  if (!iSingleMetaBuck) {
    vul_printf (vcl_cout, "\tTarget size of meta-bucket = %d generators.\n",
          iBuckMetaSize);
  }
  if (iFlag2D) { /* No need for 3rd layer */
    vul_printf (vcl_cout,
          "\tTwo-dimensional data: 3rd layer along Z dimension is fixed.\n");
  }
  vcl_fprintf(stderr, "\n");
  
  theDim.iNumBuck = 0;
  
  pTime = &TimingsB;
  
  /* --- Start Scanning the G3D formatted ASCII file */
  
  fscanf(fp1, "%d", &iTmp);  /* ID */
  fscanf(fp1, "%d", &iNumGenes);  /* Number of points */
  fclose(fp1);
  
  /* ---- Read in data ---- */
  
  pInData = NULL;
  if((pInData = (InputDataB *) calloc(iNumGenes, sizeof(InputDataB))) == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): CALLOC failed for pInData[%d].\n",
        pcFName, iNumGenes);
    exit(-2); 
  }
  if(!SetInFile(pcInFile, pInData, &theDim, iFlagDuplic)) {
    vcl_fprintf(stderr, "ERROR(%s): Reading input ... exiting.\n", pcFName);
    exit(-2);
  }
  vcl_fprintf(stderr, "\n");
  
  iX = theDim.iXdim;
  iY = theDim.iYdim;
  iZ = theDim.iZdim;
  
  if (fRadMax < 1.0) { /* Set Max Ball Radius w/r to Bounding Box */
    if(iX < iY ) {
      if (iX < iZ) 
        theDim.fRadMax = iX - 2 * theDim.fOff;
      else 
        theDim.fRadMax = iZ - 2 * theDim.fOff; 
    }
    else {
      if(iY < iZ) 
        theDim.fRadMax = iY - 2 * theDim.fOff;
      else 
        theDim.fRadMax = iZ - 2 * theDim.fOff;
    }
    theDim.fRadMax -= 2.0;
    theDim.fRadMax /= 2.0;
    fRadMax = theDim.fRadMax;
    vcl_fprintf(stderr,
        "MESG(%s):\n\tMax Ball Radius set to Min Box Radius = %.2f .\n",
        pcFName, fRadMax);
  }
  else {
    theDim.fRadMax = fRadMax;
    vcl_fprintf(stderr,
        "MESG(%s):\n\tMax Ball Radius set to input value = %.2f .\n",
        pcFName, theDim.fRadMax);
  }
  
  /* ------------------------------------------------------ *\
   *    Produce MetaBuckets if needed
  \* ------------------------------------------------------ */

  if (!iSingleMetaBuck) {
    iNumMetaBucket = 0;
    /* Check that there is enough genes */
    iTmp = iNumGenes - iBuckMetaSize;
    if (iTmp > iBuckMetaSize) {
      dTmp = ceil((double) iNumGenes / (double) iBuckMetaSize);
      iNumMetaBucket = (int) dTmp;
      if (iNumMetaBucket < 27)
        iNumMetaBucket = 27; /* At least a cubic power of 3 metabbuckets */
      vul_printf (vcl_cout, "\tWe shall produce %d Meta-buckets,\n", iNumMetaBucket);
      vul_printf (vcl_cout, "\twith about %d genes each (from %d).\n",
            iBuckMetaSize, iNumGenes);
    }
    else {
      iNumMetaBucket = 1;
      vul_printf (vcl_cout,
        "\tThere are not enough genes (%d < 2*%d) to use Meta-buckets.\n",
        iNumGenes, iBuckMetaSize);
      iSingleMetaBuck = TRUE;
    }
    
    /* Check that there is enough (integers) space along the X dimension */
    iNumBinsX = iX + 1;
    /* Cubic root: Projection along 3 axes */
    dTmp = ceil(dbnl_cbrt((double) iNumMetaBucket));
    iNumMetaBuckX = (int) dTmp;
    if (iNumMetaBuckX < 3) {
      /* Only a single Meta-Bucket0 is necessary */
      vul_printf (vcl_cout,
        "MESG(%s):\n\tOnly a single MetaBucket is useful.\n", pcFName);
      iNumMetaBuckXYZ = iNumMetaBuckX = iNumMetaBucket = 1;
      iSingleMetaBuck = TRUE;
    }
  }
  else {
    iNumMetaBuckXYZ = iNumMetaBuckX = iNumMetaBucket = 1;
    vcl_fprintf(stderr, "\tSingle Meta-Bucket0 option selected.\n");
  }

  if (!iSingleMetaBuck) {

    /* ------------------------------------------------------ *\
     *    Allocate space for 1st layer of Meta-Buckets0
    \* ------------------------------------------------------ */
    
    pMetaBuckX = &theListOfMetaBuckX;
    pMetaBuckX->pBucket = NULL;
    if ((pMetaBuckX->pBucket = (Bucket0 *) calloc(iNumMetaBuckX, sizeof(Bucket0))) == NULL) {
      vcl_fprintf(stderr,
            "ERROR(%s): CALLOC failed for pMetaBuckX->pBucket[%d].\n",
            pcFName, iNumMetaBuckX);
      exit(-1); 
    }
    pMetaBuckX->iNumBucks = iNumMetaBuckX;
    pMetaBuckX->pBin = NULL;
    if ((pMetaBuckX->pBin = (BinB *) calloc(iNumBinsX, sizeof(BinB))) == NULL) {
      vcl_fprintf(stderr,
            "ERROR(%s): CALLOC failed for pMetaBuckX->pBin[%d].\n",
            pcFName, iNumBinsX);
      exit(-1); 
    }
    pMetaBuckX->iNumBins = iNumBinsX;
    
    fAvgGene = (float) iNumGenes / (float) iNumMetaBuckX;
    
    pMetaBuckX->fTolerance = fTolerance;
    pMetaBuckX->iTotGene = iNumGenes;
    pMetaBuckX->iAvgGene = (int) ceil((double) fAvgGene);
    pMetaBuckX->iMaxGene = 0;
    pMetaBuckX->iMinGene = (int) D_LARGE;
    pMetaBuckX->iEmpty = 0;
    pMetaBuckX->iBinEmpty = 0;
    pMetaBuckX->iVirtual = 0;
    
    pMetaBuckX->Limits.iMinX = 0;
    pMetaBuckX->Limits.iMaxX = theDim.iXdim;
    pMetaBuckX->Limits.iMinY = 0;
    pMetaBuckX->Limits.iMaxY = theDim.iYdim;
    pMetaBuckX->Limits.iMinZ = 0;
    pMetaBuckX->Limits.iMaxZ = theDim.iZdim;
    
    vcl_fprintf(stderr, "\tAllocated space for %d X-MetaBuckets,\n",
          iNumMetaBuckX);
    vcl_fprintf(stderr, "\tand %d X-bins.\n", iNumBinsX);
    
    
    /* ------------------------------------------------------ *\
     *  Establish 1st set of MetaBuckets along X dimension
    \* ------------------------------------------------------ */
    
    FillFirstBucketsInX(pInData, pMetaBuckX, &theDim);
        
    if(!CheckBuck(pMetaBuckX)) {
      vcl_fprintf(stderr,
            "ERROR(%s):\n\tFailed loading X-MetaBuckets\n", pcFName);
      exit(-4);
    }
    
    /* ------------------------------------------------------ *\
     *    Produce XY MetaBuckets
    \* ------------------------------------------------------ */
    
    /* ------------------------------------------------------ *\
     *    Allocate space for 2nd layer of MetaBuckets
     *  Each MetaBucketX is sliced along the Y dimension
    \* ------------------------------------------------------ */
    
    iNumMetaBuckX = pMetaBuckX->iNumBucks;
    /* iNumBinsXY = iX * iY; */
    iNumBinsY = iY + 1;
    iNumBinsXY = iNumMetaBuckX * iNumBinsY;
    
    iTmp = (int) ceil(dbnl_cbrt((double) iNumMetaBucket));
    iNumMetaBuckXY = iTmp * iTmp;
    iTmp = iNumMetaBuckX * 3;
    if (iNumMetaBuckXY < iTmp)
      iNumMetaBuckXY = iTmp;
    
    pMetaBuckXY = &theListOfMetaBuckXY;
    pMetaBuckXY->pBucket = NULL;
    if ((pMetaBuckXY->pBucket = (Bucket0 *) calloc(iNumMetaBuckXY, sizeof(Bucket0))) == NULL) {
      vcl_fprintf(stderr,
            "ERROR(%s): CALLOC failed for pMetaBuckXY->pBucket[%d].\n",
            pcFName, iNumMetaBuckXY);
      exit(-1); 
    }
    pMetaBuckXY->iNumBucks = iNumMetaBuckXY;
    pMetaBuckXY->pBin = NULL;
    if ((pMetaBuckXY->pBin = (BinB *)  calloc(iNumBinsXY, sizeof(BinB))) == NULL) {
      vcl_fprintf(stderr,
            "ERROR(%s): CALLOC failed for pMetaBuckXY->pBin[%d].\n",
            pcFName, iNumBinsXY);
      exit(-1); 
    }
    pMetaBuckXY->iNumBins = iNumBinsXY;
    
    fAvgGene = (float) iNumGenes / (float) iNumMetaBuckXY;
    
    pMetaBuckXY->fTolerance = fTolerance;
    pMetaBuckXY->iTotGene = iNumGenes;
    pMetaBuckXY->iAvgGene = (int) ceil((double) fAvgGene);
    pMetaBuckXY->iMaxGene = 0;
    pMetaBuckXY->iMinGene = (int) D_LARGE;
    pMetaBuckXY->iEmpty = 0;
    pMetaBuckXY->iBinEmpty = 0;
    pMetaBuckXY->iVirtual = 0;
    
    pMetaBuckXY->Limits.iMinX = 0;
    pMetaBuckXY->Limits.iMaxX = theDim.iXdim;
    pMetaBuckXY->Limits.iMinY = 0;
    pMetaBuckXY->Limits.iMaxY = theDim.iYdim;
    pMetaBuckXY->Limits.iMinZ = 0;
    pMetaBuckXY->Limits.iMaxZ = theDim.iZdim;
    
    vul_printf (vcl_cout, "MESG(%s): Allocated space for %d XY MetaBuckets,\n",
          pcFName, iNumMetaBuckXY);
    vcl_fprintf(stderr, "\tand %d XY-bins.\n\n", iNumBinsXY);
    
    /* ------------------------------------------------------ *\
     *  Establish Second set of Meta Buckets0
    \* ------------------------------------------------------ */
    
    FillBucketsInXY (pInData, pMetaBuckX, pMetaBuckXY, &theDim);
        
    if (!CheckBuck(pMetaBuckXY)) {
      vcl_fprintf(stderr,
            "ERROR(%s):\n\tFailed loading XY-MetaBuckets\n", pcFName);
      exit(-4);
    }
    
    /* ------------------------------------------------------ *\
     *    Produce XYZ MetaBuckets
    \* ------------------------------------------------------ */
    
    if (iFlag2D) { /* No need for 3rd layer */
      iNumMetaBuckXYZ = pMetaBuckXY->iNumBucks;
      /**** To be developed one day ****/
    }
    else {
      /* ------------------------------------------------------ *\
       *    Allocate space for 3rd & Final layer of Buckets0
      \* ------------------------------------------------------ */
      
      iNumMetaBuckXY = pMetaBuckXY->iNumBucks;
      /* iNumBinsXYZ = iNumBinsXY * iZ; */
      iNumBinsZ = iZ + 1;
      iNumBinsXYZ = iNumMetaBuckXY * iNumBinsZ;
      
      iTmp = iNumMetaBuckXY * 3;
      if(iNumMetaBucket < iTmp)
        iNumMetaBuckXYZ = iTmp;
      else
        iNumMetaBuckXYZ = iNumMetaBucket;
      
      pMetaBuckXYZ = &theListOfMetaBuckXYZ;
      pMetaBuckXYZ->pBucket = NULL;
      if ((pMetaBuckXYZ->pBucket = (Bucket0 *) calloc(iNumMetaBuckXYZ, sizeof(Bucket0))) == NULL) {
        vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed for pMetaBuckXYZ->pBucket[%d].\n",
          pcFName, iNumMetaBuckXYZ);
        exit(-1); 
      }
      pMetaBuckXYZ->iNumBucks = iNumMetaBuckXYZ;
      pMetaBuckXYZ->pBin = NULL;
      if ((pMetaBuckXYZ->pBin = (BinB *) calloc(iNumBinsXYZ, sizeof(BinB))) == NULL) {
        vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed for pMetaBuckXYZ->pBin[%d].\n",
          pcFName, iNumBinsXYZ);
        exit(-1); 
      }
      pMetaBuckXYZ->iNumBins = iNumBinsXYZ;
      
      fAvgGene = (float) iNumGenes / (float) iNumMetaBuckXYZ;
      
      pMetaBuckXYZ->fTolerance = fTolerance;
      pMetaBuckXYZ->iTotGene = iNumGenes;
      pMetaBuckXYZ->iAvgGene = (int) ceil((double) fAvgGene);
      pMetaBuckXYZ->iMaxGene = 0;
      pMetaBuckXYZ->iMinGene = (int) D_LARGE;
      pMetaBuckXYZ->iEmpty = 0;
      pMetaBuckXYZ->iBinEmpty = 0;
      pMetaBuckXYZ->iVirtual = 0;
      
      pMetaBuckXYZ->Limits.iMinX = 0;
      pMetaBuckXYZ->Limits.iMaxX = theDim.iXdim;
      pMetaBuckXYZ->Limits.iMinY = 0;
      pMetaBuckXYZ->Limits.iMaxY = theDim.iYdim;
      pMetaBuckXYZ->Limits.iMinZ = 0;
      pMetaBuckXYZ->Limits.iMaxZ = theDim.iZdim;
      
      vcl_fprintf(stderr, "MESG(%s): Allocated space for %d XYZ Buckets0,\n",
              pcFName, iNumMetaBuckXYZ);
      vcl_fprintf(stderr, "\tand %d XYZ-bins.\n\n", iNumBinsXYZ);
      
      /* ------------------------------------------------------ *\
       *  Establish Third & Final set of Meta Buckets0
      \* ------------------------------------------------------ */
      
      FillBucketsInXYZ(pInData, pMetaBuckXY, pMetaBuckXYZ, &theDim);
      
      iNumMetaBuckXYZ = pMetaBuckXYZ->iNumBucks;
            
      if (!CheckBuck(pMetaBuckXYZ)) {
        vcl_fprintf(stderr,
          "ERROR(%s):\n\tFailed loading XYZ-MetaBuckets\n", pcFName);
        exit(-4);
      }
      
    } //end Final layer of bucket
  } //end iNumMetaBucketX >=3

  /* ###################################################### *\
   *  Process and save each MetaBucket
  \* ###################################################### */
  
  if (!iSingleMetaBuck) {
    vul_printf (vcl_cout,
          "MESG(%s):\n\tNow processing each of the %d Meta Buckets0...\n",
          pcFName, iNumMetaBuckXYZ);
  }
  else { /* Set-up the single MetaBucketXYZ */

    pMetaBuckXYZ = &theListOfMetaBuckXYZ;
    pMetaBuckXYZ->pBucket = NULL;
    if ((pMetaBuckXYZ->pBucket = (Bucket0 *)
      calloc(1, sizeof(Bucket0))) == NULL) {
      vul_printf (vcl_cout,
            "ERROR(%s): CALLOC failed for pMetaBuckXYZ->pBucket[%d].\n",
            pcFName, 1);
      exit(-1); 
    }
    pMetaBuckXYZ->iNumBucks = 1;

    iNumBinsXYZ = theDim.iWSpaceSize; /* Number of voxels */
    pMetaBuckXYZ->pBin = NULL;
    if ((pMetaBuckXYZ->pBin = (BinB *)
          calloc(iNumBinsXYZ, sizeof(BinB))) == NULL) {
      vul_printf (vcl_cout,
            "ERROR(%s): CALLOC failed for pMetaBuckXYZ->pBin[%d].\n",
            pcFName, iNumBinsXYZ);
      exit(-1); 
    }
    pMetaBuckXYZ->iNumBins = iNumBinsXYZ;

    /* fAvgGene = (float) iNumGenes; */
    pMetaBuckXYZ->fTolerance = fTolerance;
    pMetaBuckXYZ->iTotGene = iNumGenes;
    pMetaBuckXYZ->iAvgGene = iNumGenes;
    pMetaBuckXYZ->iMaxGene = 0;
    pMetaBuckXYZ->iMinGene = (int) D_LARGE;
    pMetaBuckXYZ->iEmpty = 0;
    pMetaBuckXYZ->iBinEmpty = 0;
    pMetaBuckXYZ->iVirtual = 0;

    pMetaBuckXYZ->Limits.iMinX = 0;
    pMetaBuckXYZ->Limits.iMaxX = theDim.iXdim;
    pMetaBuckXYZ->Limits.iMinY = 0;
    pMetaBuckXYZ->Limits.iMaxY = theDim.iYdim;
    pMetaBuckXYZ->Limits.iMinZ = 0;
    pMetaBuckXYZ->Limits.iMaxZ = theDim.iZdim;

    vcl_fprintf(stderr, "\tAllocated space for 1 XYZ MetaBucket,\n");
    vcl_fprintf(stderr, "\tand %d XYZ-bins (voxels).\n\n", iNumBinsXYZ);

    FillSingleMetaBucketInXYZ (pInData, pMetaBuckXYZ, &theDim);

  }

  theDim.iNumMetaBuckets = iNumMetaBuckXYZ;
  theDim.iMetaBuckId = 0;
  pMBuckInfo = &theMetaBuckInfoList;
  pMBuckInfo->iNumMetaBucks = iNumMetaBuckXYZ;
  pMBuckInfo->iNumMetaBins  = pMetaBuckXYZ->iNumBins;
  
  /* if(!iFlag2D && iNumMetaBuckXYZ > 1)  */
  if (!iFlag2D) { /* There is a 3rd layer */

    /* Allocate space to save info about each MetaBucket */
    pMBuckInfo->piNumBuckets = NULL;
    if ((pMBuckInfo->piNumBuckets = (int *)  calloc(iNumMetaBuckXYZ, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed on pMBuckInfo->piNumBuckets[%d].\n",
          pcFName, iNumMetaBuckXYZ);
      exit(-1); 
    }
    pMBuckInfo->piNumBuckEmpty = NULL;
    if ((pMBuckInfo->piNumBuckEmpty = (int *) calloc(iNumMetaBuckXYZ, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed on pMBuckInfo->piNumBuckEmpty[%d].\n",
          pcFName, iNumMetaBuckXYZ);
      exit(-1); 
    }
    pMBuckInfo->piNumBins = NULL;
    if ((pMBuckInfo->piNumBins = (int *)  calloc(iNumMetaBuckXYZ, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed on pMBuckInfo->piNumBins[%d].\n",
          pcFName, iNumMetaBuckXYZ);
      exit(-1); 
    }
    pMBuckInfo->piNumGenes = NULL;
    if ((pMBuckInfo->piNumGenes = (int *) calloc(iNumMetaBuckXYZ, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed on pMBuckInfo->piNumGenes[%d].\n",
          pcFName, iNumMetaBuckXYZ);
      exit(-1); 
    }
    pMBuckInfo->piAvgGene = NULL;
    if ((pMBuckInfo->piAvgGene = (int *)  calloc(iNumMetaBuckXYZ, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed on pMBuckInfo->piAvgGene[%d].\n",
          pcFName, iNumMetaBuckXYZ);
      exit(-1); 
    }
    pMBuckInfo->piMinGene = NULL;
    if ((pMBuckInfo->piMinGene = (int *)  calloc(iNumMetaBuckXYZ, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed on pMBuckInfo->piMinGene[%d].\n",
          pcFName, iNumMetaBuckXYZ);
      exit(-1); 
    }
    pMBuckInfo->piMaxGene = NULL;
    if ((pMBuckInfo->piMaxGene = (int *)  calloc(iNumMetaBuckXYZ, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
          "ERROR(%s): CALLOC failed on pMBuckInfo->piMaxGene[%d].\n",
          pcFName, iNumMetaBuckXYZ);
      exit(-1); 
    }
  
    /* -- OK : Process each MetaBucket -- */
  
    pBins = pMetaBuckXYZ->pBin;
    pMetaBuck = pMetaBuckXYZ->pBucket - 1;
    theDim.iNumMetaBuckets = iNumMetaBuckXYZ;
    for (i = 0; i < iNumMetaBuckXYZ; i++) { /*** MAIN LOOP ***/
  
      pMetaBuck++;
      iNumGeneInMetaBuck = pMetaBuck->iNumEl;
      if (iNumGeneInMetaBuck < 1) {
        vul_printf (vcl_cout,
              "\n\t---- MetaBucket %d has %d genes... skiping it. ----\n",
              i, iNumGeneInMetaBuck);
        continue;
      }
      vul_printf (vcl_cout,
            "\n\t---- Processing MetaBucket %d with %d genes. ----\n",
            i, iNumGeneInMetaBuck);
  
      theDim.iMetaBuckId = i;
  
      /* ------------------------------------------------------ *\
       *    Allocate space for 1st layer of Buckets0
      \* ------------------------------------------------------ */
  
      iXstart = pMetaBuck->Limits.iMinX;
      iXend   = pMetaBuck->Limits.iMaxX;
      iNumBinsX = iXend - iXstart + 1;
      iYstart = pMetaBuck->Limits.iMinY;
      iYend   = pMetaBuck->Limits.iMaxY;
      iNumBinsY = iYend - iYstart + 1;
      iZstart = pMetaBuck->Limits.iMinZ;
      iZend   = pMetaBuck->Limits.iMaxZ;
      iNumBinsZ = iZend - iZstart + 1;
  
      iNumBuckTarget = (int) ceil((double) iNumGeneInMetaBuck / (double) iBuckAvg);
  
      if (iNumBuckTarget < 27) {
        iNumBucksX = 3;
        iNumBucksXY = 9;
        iNumBucksXYZ = iNumBuckTarget = 27;
      }
      else {
        dTmp = ceil(dbnl_cbrt((double) iNumBuckTarget));
        iNumBucksX = (int) dTmp;
        iNumBucksXY = iNumBucksX * iNumBucksX;
        iNumBucksXYZ = iNumBuckTarget;
      }
  
      if (iSingleMetaBuck) {
        vul_printf (vcl_cout, "MESG(%s):\n", pcFName);
        vul_printf (vcl_cout,
              "\tTargets in Buckets0: X = %d , XY = %d , XYZ = %d\n",
              iNumBucksX, iNumBucksXY, iNumBucksXYZ);
      }

      pBucketsX = &theListOfBucketsX;
      pBucketsX->pBucket = NULL;
      if ((pBucketsX->pBucket = (Bucket0 *) calloc(iNumBucksX, sizeof(Bucket0))) == NULL) {
        vul_printf (vcl_cout,
              "ERROR(%s): CALLOC failed for pBucketsX->pBucket[%d].\n",
              pcFName, iNumBucksX);
        exit(-1); 
      }
      pBucketsX->iNumBucks = iNumBucksX;
      pBucketsX->pBin = NULL;
      if ((pBucketsX->pBin = (BinB *) calloc(iNumBinsX, sizeof(BinB))) == NULL) {
        vul_printf (vcl_cout,
              "ERROR(%s): CALLOC failed for pBucketsX->pBin[%d].\n",
              pcFName, iNumBinsX);
        exit(-1); 
      }
      pBucketsX->iNumBins = iNumBinsX;
  
      fAvgGene = (float) iNumGeneInMetaBuck / (float) iNumBucksX;
  
      pBucketsX->fTolerance = fTolerance;
      pBucketsX->iTotGene = iNumGeneInMetaBuck;
      pBucketsX->iAvgGene = (int) ceil((double) fAvgGene);
      pBucketsX->iMaxGene = 0;
      pBucketsX->iMinGene = (int) D_LARGE;
      pBucketsX->iEmpty = 0;
      pBucketsX->iBinEmpty = 0;
      pBucketsX->iVirtual = 0;
  
      pBucketsX->Limits.iMinX = iXstart;
      pBucketsX->Limits.iMaxX = iXend;
      pBucketsX->Limits.iMinY = iYstart;
      pBucketsX->Limits.iMaxY = iYend;
      pBucketsX->Limits.iMinZ = iZstart;
      pBucketsX->Limits.iMaxZ = iZend;
  
      /* ------------------------------------------------------ *\
       *  Establish First set of buckets along the X dimension  *
      \* ------------------------------------------------------ */
  
      FillBucketsInXFromMetaBucket(pInData, pBins, pMetaBuck, pBucketsX, &theDim);
  
      if (!CheckBuck(pBucketsX)) {
        vul_printf (vcl_cout,
              "ERROR(%s):\n\tFailed loading X-buckets for MetaBucket %d\n",
              pcFName, i);
        exit(-4);
      }
  
      /* ------------------------------------------------------ *\
       *    Allocate space for 2nd layer of Buckets0
       *  Each BucketX is sliced along the Y dimension
      \* ------------------------------------------------------ */
  
      iNumBucksX = pBucketsX->iNumBucks;
      iNumBinsXY = iNumBucksX * iNumBinsY;
  
      iTmp = iNumBucksX * 3;
      if (iNumBucksXY < iTmp)
        iNumBucksXY = iTmp;
      
      pBucketsXY = &theListOfBucketsXY;
      pBucketsXY->pBucket = NULL;
      if ((pBucketsXY->pBucket = (Bucket0 *) calloc(iNumBucksXY, sizeof(Bucket0))) == NULL) {
        vul_printf (vcl_cout,
              "ERROR(%s): CALLOC failed for pBucketsXY->pBucket[%d].\n",
              pcFName, iNumBucksXY);
        exit(-1); 
      }
      pBucketsXY->iNumBucks = iNumBucksXY;
  
      pBucketsXY->pBin = NULL;
      if ((pBucketsXY->pBin = (BinB *) calloc(iNumBinsXY, sizeof(BinB))) == NULL) {
      vul_printf (vcl_cout,
            "ERROR(%s): CALLOC failed for pBucketsXY->pBin[%d].\n",
            pcFName, iNumBinsXY);
      exit(-1); 
      }
      pBucketsXY->iNumBins = iNumBinsXY;
  
      fAvgGene = (float) iNumGeneInMetaBuck / (float) iNumBucksXY;
  
      pBucketsXY->fTolerance = fTolerance;
      pBucketsXY->iTotGene = iNumGeneInMetaBuck;
      pBucketsXY->iAvgGene = (int) ceil((double) fAvgGene);
      pBucketsXY->iMaxGene = 0;
      pBucketsXY->iMinGene = (int) D_LARGE;
      pBucketsXY->iEmpty = 0;
      pBucketsXY->iBinEmpty = 0;
      pBucketsXY->iVirtual = 0;
  
      pBucketsXY->Limits.iMinX = iXstart;
      pBucketsXY->Limits.iMaxX = iXend;
      pBucketsXY->Limits.iMinY = iYstart;
      pBucketsXY->Limits.iMaxY = iYend;
      pBucketsXY->Limits.iMinZ = iZstart;
      pBucketsXY->Limits.iMaxZ = iZend;
  
      /* ------------------------------------------------------ *\
       *  Establish Second set of buckets
      \* ------------------------------------------------------ */

      FillBucketsInXY(pInData, pBucketsX, pBucketsXY, &theDim);

      if (!CheckBuck(pBucketsXY)) {
        vul_printf (vcl_cout,
              "ERROR(%s):\n\tFailed loading XY-buckets for MetaBucket %d\n",
              pcFName, i);
        exit(-4);
      }

      /* ------------------------------------------------------ *\
       *    Allocate space for 3rd layer of Buckets0
       *  Each BucketXY is sliced along the Z dimension
      \* ------------------------------------------------------ */

      iNumBucksXY = pBucketsXY->iNumBucks;
      iNumBinsXYZ = iNumBucksXY * iNumBinsZ;

      iTmp = iNumBucksXY * 3;
      if (iNumBucksXYZ < iTmp)
        iNumBucksXYZ = iTmp;

      pBucketsXYZ = &theListOfBucketsXYZ;
      pBucketsXYZ->pBucket = NULL;
      if ((pBucketsXYZ->pBucket = (Bucket0 *) calloc(iNumBucksXYZ, sizeof(Bucket0))) == NULL) {
        vul_printf (vcl_cout,
              "ERROR(%s): CALLOC failed for pBucketsXYZ->pBucket[%d].\n",
        pcFName, iNumBucksXYZ);
        exit(-1); 
      }
      pBucketsXYZ->iNumBucks = iNumBucksXYZ;

      pBucketsXYZ->pBin = NULL;
      if ((pBucketsXYZ->pBin = (BinB *) calloc(iNumBinsXYZ, sizeof(BinB))) == NULL) {
        vul_printf (vcl_cout,
              "ERROR(%s): CALLOC failed for pBucketsXYZ->pBin[%d].\n",
              pcFName, iNumBinsXYZ);
        exit(-1); 
      }
      pBucketsXYZ->iNumBins = iNumBinsXYZ;

      fAvgGene = (float) iNumGeneInMetaBuck / (float) iNumBucksXYZ;

      pBucketsXYZ->fTolerance = fTolerance;
      pBucketsXYZ->iTotGene = iNumGeneInMetaBuck;
      pBucketsXYZ->iAvgGene = (int) ceil((double) fAvgGene);
      pBucketsXYZ->iMaxGene = 0;
      pBucketsXYZ->iMinGene = (int) D_LARGE;
      pBucketsXYZ->iEmpty = 0;
      pBucketsXYZ->iBinEmpty = 0;
      pBucketsXYZ->iVirtual = 0;

      pBucketsXYZ->Limits.iMinX = iXstart;
      pBucketsXYZ->Limits.iMaxX = iXend;
      pBucketsXYZ->Limits.iMinY = iYstart;
      pBucketsXYZ->Limits.iMaxY = iYend;
      pBucketsXYZ->Limits.iMinZ = iZstart;
      pBucketsXYZ->Limits.iMaxZ = iZend;

      /* ------------------------------------------------------ *\
       *  Establish Third & Final set of Buckets0
      \* ------------------------------------------------------ */

      FillBucketsInXYZ(pInData, pBucketsXY, pBucketsXYZ, &theDim);

      if (!CheckBuck(pBucketsXYZ)) {
        vul_printf (vcl_cout,
              "ERROR(%s):\n\tFailed loading XYZ-buckets for MetaBucket %d\n",
              pcFName, i);
        exit(-4);
      }

      *(pMBuckInfo->piNumBuckets+i) = pBucketsXYZ->iNumBucks;
      *(pMBuckInfo->piNumBuckEmpty+i) = pBucketsXYZ->iEmpty;
      *(pMBuckInfo->piNumBins+i) = pBucketsXYZ->iNumBins;
      *(pMBuckInfo->piNumGenes+i) = pBucketsXYZ->iTotGene;
      *(pMBuckInfo->piAvgGene+i) = pBucketsXYZ->iAvgGene;
      *(pMBuckInfo->piMinGene+i) = pBucketsXYZ->iMinGene;
      *(pMBuckInfo->piMaxGene+i) = pBucketsXYZ->iMaxGene;

      /* ------------------------------------------------------ *\
       *  Save set of XYZ-Buckets0 for MetaBucket
      \* ------------------------------------------------------ */
      
      vcl_strcpy (cOutFile, pcFileNameIn);
      pcOutFile = cOutFile;
      vcl_sprintf (cTmp, ".MetaBuck%d.Data.txt", i);
      strcat (cOutFile, cTmp);
      SaveDataMetaBuck (pcInFile, pcOutFile, pInData,
                  pBucketsX, pBucketsXY, pBucketsXYZ, &theDim);

      if (iFlagSaveG3d) {
        vcl_strcpy (cOutFile, pcFileNameIn);
        vcl_sprintf (cTmp, ".MetaBuck%d.g3d", i);
        strcat (cOutFile, cTmp);
        SaveG3dMetaBuck (pcOutFile, pInData, pBucketsXYZ, &theDim);
      }

      /* ------------------------------------------------------ *\
       *  Compute Convex Hull vertices per bucket & save  *
      \* ------------------------------------------------------ */
  
      if (iFlagConvexHull) {
        vul_printf (vcl_cout,
              "MESG(%s): Compute Convex Hull for each bucket...\n",
              pcFName);
        vcl_strcpy (cOutFile, pcFileNameIn);
        vcl_sprintf (cTmp, ".MetaBuck%d.Qhull.txt", i);
        strcat (cOutFile, cTmp);
      }

      /* ------------------------------------------------------ *\
       *  Clean-up buckets for processed meta-bucket
      \* ------------------------------------------------------ */

        CleanUpBuckBin(pBucketsX, pBucketsXY, pBucketsXYZ);
      
       free(pBucketsX->pBucket);
      free(pBucketsX->pBin);
      free(pBucketsXY->pBucket);
      free(pBucketsXY->pBin);
      free(pBucketsXYZ->pBucket);
      free(pBucketsXYZ->pBin);


    } /* Next MetaBucket: i++ */

    /* ------------------------------------------------------ *\
     *           Set inputs with voxel data 
     *        (to be used in shock validation)
    \* ------------------------------------------------------ */

    iTmp = theDim.iWSpaceSize;
    pBins = NULL;
    if ((pBins = (BinB *) calloc(iTmp, sizeof(BinB))) == NULL) {
      vul_printf (vcl_cout, "ERROR(%s): CALLOC fails on pBins[%d].\n",
            pcFName, iTmp);
      exit(-1); 
    }

    FillBinsInXYZ(pInData, pBins, &theDim);

    /* ------------------------------------------------------ *\
     *    Save global information for MetaBuckets
    \* ------------------------------------------------------ */

    vcl_strcpy(cOutFile, pcFileNameIn);
    strcat(cOutFile, ".MetaBucks.Timings.txt");
    SaveTimingsMetaBuck (pcInFile, pcOutFile, pTime, pMetaBuckXYZ,
                  pMBuckInfo, &theDim);

    vcl_strcpy(cOutFile, pcFileNameIn);
    strcat(cOutFile, ".MetaBucks.Data.txt");
    SaveAllDataMetaBuck (pcInFile, pcOutFile, pInData, pBins, &theDim);

    if (iFlagSaveG3d) {
      vcl_strcpy(cOutFile, pcFileNameIn);
      vcl_sprintf(cTmp, ".MetaBucks.g3d", i);
      strcat(cOutFile, cTmp);
      SaveG3dAll(pcOutFile, pInData, pBins, &theDim);
    }

  } /* End of 3D case with many MetaBuckets */

  /* ---------- End Of Processing ---------- */
  vcl_fprintf(stderr, "\nMESG(%s): This is it!\n\t", pcFName);
  vcl_fprintf(stderr, "task has been performed ... bye ;)\n");

}


// ######################## FULL_BUCKETS_V2.C #########################

/*------------------------------------------------------------*\
 * File: fill_buckets_v2.c
 *
 * Functions:
 *  FillFirstBucketsInX
 *  FillBucketsInXY
 *  FillBucketsInXYZ
 *  FillBucketsInXFromMetaBucket
 *  FillBinsInXYZ
 *
 * History:
 *  #0: July 2001, by F.Leymarie
 *  #1: April 2003: Remove failing cases.
 *  #2: July 2003: Added FillBucketsInXFromMetaBucket() and
 *      FillBinsInXYZ()
 *
\*------------------------------------------------------------*/

///#include "fill_buckets.h"
///#include "assert.h"

/*------------------------------------------------------------*\
 * Function: FillFirstBucketsInX
 *
 * Usage: Fill-in bins (delimited by integer coordinates along
 *    the X axis only) and construct Buckets0 by concatenation
 *    of sequences of such X-bins.
 *    Thus, each X-bin has unit size in X, and Ydim & Zdim
 *    along the other two dimensions.
 *    Add new buckets if necessary.
 *
\*------------------------------------------------------------*/

void FillFirstBucketsInX (InputDataB *pInData, Buckets0 *pBucketsX,
                  DimensionB *pDim)
{
  const char    *pcFName = "FillFirstBucketsInX";
  register int  i, j;
  int    iNumGeneTot, iNumBinsX, iNum, iX, iBin, iAvgGene;
  int    iTmp, iDelta, iLowGene, iBinStart, iMaxGene, iNumBucksX;
  int          iBinXlast, iNumGenesLeft, iMinGene, iHighGene, iNumMax;
  int    iXdim,iYdim,iZdim, iMax, iXmin,iXmax, iYmin,iYmax;
  int    iZmin,iZmax, iNumGenes, iNumBucksOld, iFlagAddBuck;
  int    *piBinGene, *piBinGeneOld;
  float    fX, fDelta;
  InputDataB  *pGene;
  BinB    *pTmpBins, *pBin, *pBinOld, *pBinX;
  Bucket0  *pBuckX;
  
  iXmin = pBucketsX->Limits.iMinX;
  iXmax = pBucketsX->Limits.iMaxX;
  iXdim = iXmax - iXmin;
  iYmin = pBucketsX->Limits.iMinY;
  iYmax = pBucketsX->Limits.iMaxY;
  iYdim = iYmax - iYmin;
  iZmin = pBucketsX->Limits.iMinZ;
  iZmax = pBucketsX->Limits.iMaxZ;
  iZdim = iZmax - iZmin;
  if (iXdim != pDim->iXdim || iYdim != pDim->iYdim || iZdim != pDim->iZdim) {
    vul_printf (vcl_cout,
          "ERROR(%s): Dimensions of set of X-buckets = (%d , %d , %d)\n",
          pcFName, iXdim, iYdim, iZdim);
    vul_printf (vcl_cout,
          "\tbut, we expected the full dataset size: (%d , %d , %d).\n",
          pDim->iXdim, pDim->iYdim, pDim->iZdim);
  }
  
  iNumBinsX = pBucketsX->iNumBins;
  iTmp = iXdim+1;
  if (iNumBinsX != iTmp) {
    vul_printf (vcl_cout, "ERROR(%s): Wrong number of BinsX: %d != %d\n",
          pcFName, iNumBinsX, iTmp);
    exit(-3);
  }
  iBinXlast = iXdim;
  
  iNumGeneTot = pBucketsX->iTotGene;
  iAvgGene    = pBucketsX->iAvgGene; /* Init target of genes per bucket */
  iNumBucksX  = pBucketsX->iNumBucks;
  
  vul_printf (vcl_cout, "MESG(%s): Initially, %d BucketsX to fill-in.\n",
        pcFName, iNumBucksX);
  vul_printf (vcl_cout, "\tTarget Number of Generators per Bucket0 = %d\n", iAvgGene);
  vul_printf (vcl_cout, "\tfrom a total of %d genes.\n", iNumGeneTot);
  vul_printf (vcl_cout, "\tNumber of BinsX = %d\n", iNumBinsX);
  
  /* ---- Fill-in X-Bins ---- */
  
  /* Temp Buffer of X-bins : we do not know a priori *\
  \*   how many generators per X-bin we may have     */
  pTmpBins = NULL;
  if ((pTmpBins = (BinB *) calloc(iNumBinsX, sizeof(BinB))) == NULL) {
    vul_printf (vcl_cout, "ERROR(%s):\n\tCALLOC failed for pTmpBins[%d].\n",
          pcFName, iNumBinsX);
    exit(-2); 
  }
  
  iNumMax = iAvgGene * 3;
  /* iNumMax = iNumGeneTot; */ /* This is expected to be a large number */
  pBin = pTmpBins;
  for (i = 0; i < iNumBinsX; i++) {
    pBin->piGenes = NULL;
    if ((pBin->piGenes = (int *) calloc(iNumMax, sizeof(int))) == NULL) {
      vul_printf (vcl_cout,
            "ERROR(%s):\n\tCALLOC failed for pBin[%d]->piGenes[%d].\n",
            pcFName, i, iNumMax);
      exit(-2); 
    }
    pBin->iNumEl = 0;
    pBin->iMaxEl = iNumMax;
    pBin++;
  }
  
  /* Put all original input generators in TmpBins */
  pGene = pInData-1;
  for (i = 0; i < iNumGeneTot; i++) {
    pGene++;
    fX = pGene->Sample.fPosX;
    iX = (int) floor((double) fX);
    iBin = iX - iXmin; /* Bin label */
    if (iBin > iBinXlast) {
      vul_printf (vcl_cout, "ERROR(%s):\n\tOverflow BinB number %d > max = %d\n",
            pcFName, iBin, iBinXlast);
      exit(-2);
    }
    pBin = pTmpBins+iBin;
  
    iNum = pBin->iNumEl;
    iMax = pBin->iMaxEl;
    piBinGene = pBin->piGenes+iNum;
    *piBinGene = i;
    pBin->iNumEl++;
    iNum++;
    if (iNum >= iMax) {
      iMax += iMax;
      if ((pBin->piGenes = (int *) realloc((int *) pBin->piGenes, iMax * sizeof(int))) == NULL) {
        vul_printf (vcl_cout,
              "ERROR(%s):\n\tREALLOC fails on pBin[%d]->piGenes[%d].\n",
              pcFName, iBin, iMax);
        exit(-2); 
      }
      pBin->iMaxEl = iMax;
      if (iMax > iNumMax) 
        iNumMax = iMax;
    }
  }
  
  /* Transfer data from Tmp Buffer */
  iTmp = 0;
  pBinOld = pTmpBins;
  pBinX = pBucketsX->pBin;
  for (i = 0; i < iNumBinsX; i++) {
    iNum = pBinOld->iNumEl;
    if (iNum > iNumMax) {
      vul_printf (vcl_cout,
            "ERROR(%s):\n\tOverflow: %d Genes (Max = %d) in X-BinB %d\n",
            pcFName, iNum, iNumMax, i);
      exit(-2);
    }
    pBinX->piGenes = NULL;
    if ((pBinX->piGenes = (int *) calloc(iNum, sizeof(int))) == NULL) {
      vul_printf (vcl_cout,
            "ERROR(%s):\n\tCALLOC failed for pBinX[%d]->piGenes[%d].\n",
            pcFName, i, iNum);
      exit(-2); 
    }
    pBinX->iMaxEl = pBinX->iNumEl = iNum;
    piBinGene = pBinX->piGenes;
    piBinGeneOld = pBinOld->piGenes;
    for (j = 0; j < iNum; j++) {
      *piBinGene++ = *piBinGeneOld++;
    }
    pBinX++;
    pBinOld++;
    iTmp += iNum;
  }
  if (iTmp != iNumGeneTot) {
    vul_printf (vcl_cout,
          "ERROR(%s):\n\tExpected to load %d genes in X-Bins, not %d\n",
          pcFName, iNumGeneTot, iTmp);
    exit(-2); 
  }
  
  /* Clean-up */
  pBinOld = pTmpBins;
  for (i = 0; i < iNumBinsX; i++) {
    free(pBinOld->piGenes);
    pBinOld++;
  }
  free(pTmpBins);
  
  /* ---- Fill-in X-Buckets0 from Sequences of X-Bins ---- */
  
  fDelta = pBucketsX->fTolerance; /* Tolerance in percent */
  iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
  iLowGene = iAvgGene;
  iAvgGene += iDelta;
  iHighGene = iAvgGene + iDelta;
  vul_printf (vcl_cout, "\tFilling X-Buckets0 from X-Bins.\n");
  vul_printf (vcl_cout,
        "\tTargets (genes per bucket): Min = %d , Avg = %d , Max = %d\n",
        iLowGene, iAvgGene, iHighGene);
  
  pBuckX = pBucketsX->pBucket;
  pBinX  = pBucketsX->pBin;
  iMaxGene = 0;
  iMinGene = iNumGeneTot;
  i = 0; /* Tracks number of BucketsX */
  j = 0; /* Tracks number of BinsX */
  iBinStart = 0; /* Start with first bin */
  iNumGenesLeft = iNumGeneTot;
  iNum = iNumBucksX - 1;
  while (i < iNum) { /* Fill-in all BucketsX except last one */
    iNumGenes = 0;
    if (iBinStart >= iNumBinsX) break; /* Overflow */
    if (iNumGenesLeft < 1) break; /* No more generators left */
  
    for (j = iBinStart; j < iNumBinsX; j++) {
      pBinX = pBucketsX->pBin+j;
      iNumGenes += pBinX->iNumEl;
      if (iNumGenes > iLowGene) { /* Target reached */
        iTmp = j - iBinStart;
        if (iNumGenes > iHighGene && iTmp > 0) { /* Overflow */
          iNumGenes -= pBinX->iNumEl; /* Go back */
          pBuckX->iNumEl = iNumGenes;
          pBuckX->iBinFirst = iBinStart;
          pBuckX->iBinLast = j - 1;
          pBuckX->iNumBins = iTmp;
          pBuckX->Limits.iMinX = iBinStart;
          pBuckX->Limits.iMaxX = j;
          iBinStart = j;
        }
        else { /* Within desired limits */
          pBuckX->iNumEl = iNumGenes;
          pBuckX->iBinFirst = iBinStart;
          pBuckX->iBinLast = j;
          pBuckX->iNumBins = iTmp + 1;
          pBuckX->Limits.iMinX = iBinStart;
          pBuckX->Limits.iMaxX = j + 1;
          iBinStart = j + 1;
        }
        pBuckX->Limits.iMinY = 0;
        pBuckX->Limits.iMaxY = iYdim;
        pBuckX->Limits.iMinZ = 0;
        pBuckX->Limits.iMaxZ = iZdim;
        
        if(iNumGenes > iMaxGene) iMaxGene = iNumGenes;
        if(iNumGenes < iMinGene) iMinGene = iNumGenes;
        /* Reset target */
        iNumGenesLeft -= iNumGenes;
        iAvgGene = (int) ceil((double) iNumGenesLeft / (double) (iNumBucksX - i));
        iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
        iLowGene = iAvgGene;
        iAvgGene += iDelta;
        iHighGene = iAvgGene + iDelta;
        if (iAvgGene < 10) {
          vul_printf (vcl_cout, "WARNING(%s):\n\tiAvgGene = %d, for BucketX %d.\n",
                pcFName, iAvgGene, i);
        }
        if (iAvgGene < 1) {
          iAvgGene = 1;
          iLowGene = 1;
          iHighGene = 10;
        }
      
        if (iNumGenesLeft < 1) {
          break;
        }
      
        /* Goto next bucket */
        pBuckX++;
        i++;
        break;
      } /* End of if(iNumGenes > iLowGene) : Target reached */
    } /* Next bin: j++ */
  } /* End of While loop */
  
  if (i < iNum) {
    vul_printf (vcl_cout,
          "WARNING(%s):\n\tOnly %d BucketsX set, instead of %d.\n",
          pcFName, i, iNum);
  }
  if (iBinStart >= iNumBinsX) { /* Overflow */
    vul_printf (vcl_cout,
          "WARNING(%s):\n\tOverflow: %d bins used, none left for last buckets.\n",
          pcFName, iBinStart);
  }
  j++;
  
  /* ---- Set Last Bucket0(s) ---- */
  
  iAvgGene = pBucketsX->iAvgGene; /* Init target num of generators per bucket */  
  iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
  iLowGene  = iAvgGene;
  iAvgGene += iDelta;
  iHighGene = iAvgGene + iDelta;
  
  /* OK: fill last Bucket0 with remaining generators */
  if (iNumGenesLeft < iHighGene) {
    pBuckX->iNumEl = iNumGenesLeft;
    pBuckX->iBinFirst = iBinStart;
    pBuckX->iBinLast = iNumBinsX - 1;
    pBuckX->iNumBins = iNumBinsX - iBinStart;
    j += pBuckX->iNumBins;
    pBuckX->Limits.iMinX = iBinStart;
    pBuckX->Limits.iMaxX = iXdim;
    pBuckX->Limits.iMinY = 0;
    pBuckX->Limits.iMaxY = iYdim;
    pBuckX->Limits.iMinZ = 0;
    pBuckX->Limits.iMaxZ = iZdim;
    i++;
    if (iNumGenesLeft > iMaxGene) 
      iMaxGene = iNumGenesLeft;
    if (iNumGenesLeft < iMinGene) 
      iMinGene = iNumGenesLeft;
  }
  else { /* Too many generators left: Need to add new buckets */
  
    /* First : Fill-in the (presently) last bucket allocated */
  
    iNumGenes = 0;
    for (j = iBinStart; j < iNumBinsX; j++) {
      pBinX = pBucketsX->pBin+j;
      iNumGenes += pBinX->iNumEl;
      if (iNumGenes > iLowGene) { /* Target reached */
        iTmp = j - iBinStart;
      if(iNumGenes > iHighGene && iTmp > 0) { /* Overflow */
        iNumGenes -= pBinX->iNumEl; /* Go back */
        pBuckX->iNumEl = iNumGenes;
        pBuckX->iBinFirst = iBinStart;
        pBuckX->iBinLast = j - 1;
        pBuckX->iNumBins = iTmp;
        pBuckX->Limits.iMinX = iBinStart;
        pBuckX->Limits.iMaxX = j;
        iBinStart = j;
      }
      else { /* Within desired limits */
        pBuckX->iNumEl = iNumGenes;
        pBuckX->iBinFirst = iBinStart;
        pBuckX->iBinLast = j;
        pBuckX->iNumBins = iTmp + 1;
        pBuckX->Limits.iMinX = iBinStart;
        pBuckX->Limits.iMaxX = j + 1;
        iBinStart = j + 1;
      }
      pBuckX->Limits.iMinY = 0;
      pBuckX->Limits.iMaxY = iYdim;
      pBuckX->Limits.iMinZ = 0;
      pBuckX->Limits.iMaxZ = iZdim;
      
      if (iNumGenes > iMaxGene) 
        iMaxGene = iNumGenes;
      if (iNumGenes < iMinGene) 
        iMinGene = iNumGenes;
      /* Reset target */
      iNumGenesLeft -= iNumGenes;
      i++;
      break; /* Get out of For loop */
    }//end for
  }
  j++;
  
  /* -- 2nd: Add, 1 by 1, new buckets function of generators left -- */
  
  iNumBucksOld = iNumBucksX;
  iFlagAddBuck = TRUE;
  while (iFlagAddBuck) {
    /* Add space for one more bucket */
     iNumBucksX++;
     if ((pBucketsX->pBucket = (Bucket0 *) realloc(
          (Bucket0 *) pBucketsX->pBucket, iNumBucksX * sizeof(Bucket0))) == NULL) {
      vul_printf (vcl_cout,
            "ERROR(%s):\n\tREALLOC failed on pBucketsX->pBucket[%d].\n",
            pcFName, iNumBucksX);
      exit(-2); 
    }
      
     iTmp = iNumBucksX - 1;
     pBuckX = pBucketsX->pBucket+iTmp; /* Pointer to lastly created Bucket0 */
     iNumGenes = 0;
     pBuckX->iNumCHgenes = 0;
  
     if (iNumGenesLeft < iLowGene) { /* Not much left: Fill this last bucket */
      pBuckX->iNumEl = iNumGenesLeft;
      pBuckX->iBinFirst = iBinStart;
      pBuckX->iBinLast = iNumBinsX - 1;
      pBuckX->iNumBins = iNumBinsX - iBinStart;
      j += pBuckX->iNumBins;
      pBuckX->Limits.iMinX = iBinStart;
      pBuckX->Limits.iMaxX = iXdim;
      pBuckX->Limits.iMinY = 0;
      pBuckX->Limits.iMaxY = iYdim;
      pBuckX->Limits.iMinZ = 0;
      pBuckX->Limits.iMaxZ = iZdim;
      iBinStart = iNumBinsX;
      if(iNumGenesLeft > iMaxGene) iMaxGene = iNumGenesLeft;
      if(iNumGenesLeft < iMinGene) iMinGene = iNumGenesLeft;
      i++;
      iNumGenesLeft = 0;
      iFlagAddBuck = FALSE;
      break; /* Get out of While loop */
    }
  
    for(j = iBinStart; j < iNumBinsX; j++) {
      pBinX = pBucketsX->pBin+j;
      iNumGenes += pBinX->iNumEl;
      if (iNumGenes > iLowGene) { /* Target reached */
        iTmp = j - iBinStart;
        if (iNumGenes > iHighGene && iTmp > 0) { /* Overflow */
          iNumGenes -= pBinX->iNumEl; /* Go back */
          pBuckX->iNumEl = iNumGenes;
          pBuckX->iBinFirst = iBinStart;
          pBuckX->iBinLast = j - 1;
          pBuckX->iNumBins = iTmp;
          pBuckX->Limits.iMinX = iBinStart;
          pBuckX->Limits.iMaxX = j;
          iBinStart = j;
        }
        else { /* Within desired limits */
          pBuckX->iNumEl = iNumGenes;
          pBuckX->iBinFirst = iBinStart;
          pBuckX->iBinLast = j;
          pBuckX->iNumBins = iTmp + 1;
          pBuckX->Limits.iMinX = iBinStart;
          pBuckX->Limits.iMaxX = j + 1;
          iBinStart = j + 1;
        }
        pBuckX->Limits.iMinY = 0;
        pBuckX->Limits.iMaxY = iYdim;
        pBuckX->Limits.iMinZ = 0;
        pBuckX->Limits.iMaxZ = iZdim;
        if(iNumGenes > iMaxGene) iMaxGene = iNumGenes;
        if(iNumGenes < iMinGene) iMinGene = iNumGenes;
        /* Reset target */
        iNumGenesLeft -= iNumGenes;
        i++;
        break; /* Get out of For loop */
      }//end if
    }//end for j
    j++;
  
    iTmp = (int) ceil((double) iLowGene * 0.3);
    if (iNumGenesLeft < iTmp) { /* Not much left: Add it to last Bucket0 */
      iNumGenes += iNumGenesLeft;
      pBuckX->iNumEl = iNumGenes;
      /* pBuckX->iBinFirst = iBinStart; */
      pBuckX->iBinLast = iNumBinsX - 1;
      pBuckX->iNumBins = iNumBinsX - pBuckX->iBinFirst;
      /* j += pBuckX->iNumBins; */
      pBuckX->Limits.iMinX = iBinStart;
      pBuckX->Limits.iMaxX = iXdim;
      pBuckX->Limits.iMinY = 0;
      pBuckX->Limits.iMaxY = iYdim;
      pBuckX->Limits.iMinZ = 0;
      pBuckX->Limits.iMaxZ = iZdim;
      if(iNumGenes > iMaxGene) 
        iMaxGene = iNumGenes;
      if(iNumGenes < iMinGene) 
        iMinGene = iNumGenes;
      iNumGenesLeft = 0;
      iFlagAddBuck = FALSE;
      break; /* Get out of While loop */
    }
  } /* End of While(iFlagAddBuck) loop */
  
  pBucketsX->iNumBucks = iNumBucksX; /* Final number of Buckets0 */
  iTmp = iNumBucksX - iNumBucksOld;
  vul_printf (vcl_cout, "\tAdded %d new Buckets0 to the initial %d.\n", iTmp,
        iNumBucksOld);
  }
  
  vcl_fprintf(stderr, "\tAll (%d) BucketsX set.\n", i);
  vcl_fprintf(stderr, "\t\tMax Number of Generators per BucketX = %d\n", iMaxGene);
  vcl_fprintf(stderr, "\t\tMin Number of Generators per BucketX = %d\n", iMinGene);
  
  pBucketsX->iMaxGene = iMaxGene;
  pBucketsX->iMinGene = iMinGene;
  
  if (j < iNumBinsX) {
    vcl_fprintf(stderr, "ERROR(%s):\n\tVisited too few BinsX: %d < %d .\n",
        pcFName, j, iNumBinsX);
    exit(-3);
  }
  else if (j > iNumBinsX) {
    vcl_fprintf(stderr, "ERROR(%s):\n\tVisited too many BinsX: %d > %d .\n",
        pcFName, j, iNumBinsX);
    exit(-3);
  }
  vcl_fprintf(stderr, "\n");
  
  return;
}

/*------------------------------------------------------------*\
 * Function: FillBucketsInXY
 *
 * Usage: Fill-in bins (delimited by integer coordinates along
 *    the X & Y axes) and construct Buckets0 in Y for each
 *    Bucket0 in X, previously filled.
 *    Each XY-bin has unit size in Y, a size in X delimited
 *    by the BucketX, and full extent in Z (i.e., Zdim).
 *
\*------------------------------------------------------------*/

void
FillBucketsInXY(InputDataB *pInData, Buckets0 *pBucketsX,
    Buckets0 *pBucketsXY, DimensionB *pDim)
{
  const char    *pcFName = "FillBucketsInXY";
  register int  i, j, k, l, m;
  int    iNumBucksY, iBinYlast, iNumMax, iNumBucksX, iNumGenes;
  int    iAvgGene, iDelta, iLowGene, iNum, iNumBinsX, iBinStart;
  int    iNumGenesInBin, iGene, iY, iBin, iNumGenesLeft;
  int    iMaxGene, iNumGenesAcc, iMinGene, iNumBucksXY, iNumBinsY;
  int    iJumpY, iBinXYloc, iHighGene, iTmp, iStep, iThresh;
  int    iXstart, iXlast, iNumBinsXY, iNumBuckVirtualTotal;
  int    iNumBuckEmpty, iNumBuckEmptyTotal, iNumBuckVirtual;
  int    iMaxGeneXY, iMinGeneXY, iNumGeneTot;
  int    iXdim,iYdim,iZdim, iXmin,iXmax, iYmin,iYmax, iZmin,iZmax;
  int    *piGene, *piBinGene, *piBinGeneOld;
  float    fDelta, fAvgGene, fY;
  Bucket0  *pBuckX, *pBuckXY;
  BinB    *pBinXY, *pTmpBins, *pBin, *pBinX, *pBinOld;

  /* iZdim = pDim->iZdim;
     iYdim = pDim->iYdim; */

  iXmin = pBucketsXY->Limits.iMinX;
  iXmax = pBucketsXY->Limits.iMaxX;
  iXdim = iXmax - iXmin;
  iYmin = pBucketsXY->Limits.iMinY;
  iYmax = pBucketsXY->Limits.iMaxY;
  iYdim = iYmax - iYmin;
  iNumBinsY = iYdim + 1;
  iZmin = pBucketsXY->Limits.iMinZ;
  iZmax = pBucketsXY->Limits.iMaxZ;
  iZdim = iZmax - iZmin;

  iNumBucksX  = pBucketsX->iNumBucks;
  iNumBucksXY = pBucketsXY->iNumBucks;
  iNumBucksY  = (int) floor((double) iNumBucksXY / (double) iNumBucksX);
  iBinYlast   = iYdim;

  iNumGeneTot = pBucketsXY->iTotGene;
  iAvgGene = pBucketsXY->iAvgGene; /* Init target of genes per bucket */
  fDelta = pBucketsXY->fTolerance; /* Tolerance in percent */
  iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
  iLowGene = iAvgGene;
  iAvgGene += iDelta;
  iHighGene = iAvgGene + iDelta;

  vcl_fprintf(stderr, "MESG(%s): %d BucketsXY to fill-in.\n", pcFName,
    iNumBucksXY);
  vcl_fprintf(stderr, "\tTarget Number of Generators per Bucket0 = %d\n", iAvgGene);
  vcl_fprintf(stderr, "\tfrom a total of %d genes.\n", iNumGeneTot);
  vcl_fprintf(stderr, "\tTarget range going from %d to %d\n", iLowGene,
    iHighGene);
  pBucketsXY->iAvgGene = iAvgGene;
  iNumBinsXY = pBucketsXY->iNumBins;
  vcl_fprintf(stderr, "\tNumber of BinsXY = %d .\n", iNumBinsXY);

  /* --- For each X Bucket0 in BucketsX list, create --- *\
  \*    iNumBucksY along the Y dimension        */ 

  pBuckX  = pBucketsX->pBucket;
  pBinXY  = pBucketsXY->pBin;
  pBuckXY = pBucketsXY->pBucket;
  l = 0; /* Tracks number of BucketsXY */
  m = 0; /* Tracks number of BinsXY */
  iNumBuckEmpty = iNumBuckEmptyTotal = 0;
  iNumBuckVirtual = iNumBuckVirtualTotal = 0;
  iMaxGeneXY = 0;      /* Tracks the overall Max/Min of */
  iMinGeneXY = iNumGeneTot; /* generators for all BucketsXY */

  for(i = 0; i < iNumBucksX; i++) {
    iNumGenes = pBuckX->iNumEl;
    fAvgGene = (float) iNumGenes / (float) iNumBucksY;
    iAvgGene = (int) ceil((double) fAvgGene);
    iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
    iDelta += iDelta;
    iLowGene = iAvgGene;
    iAvgGene += iDelta;
    iHighGene = iAvgGene + iDelta;

    iXstart = pBuckX->Limits.iMinX;
    iXlast  = pBuckX->Limits.iMaxX;
    if(iXstart >= iXlast) {
      vcl_fprintf(stderr, "ERROR(%s): MinX = %d >= MaxX = %d\n",
        pcFName, iXstart, iXlast);
      exit(-2);
    }
    
    /* ---- Fill-in Y-Bins (per X-Bucket0) ---- */

    /* Temp Buffer of Y-bins : we do not know a priori *\
    \*   how many generators per Y-bin we may have     */
    pTmpBins = NULL;
    if((pTmpBins = (BinB *) calloc(iNumBinsY, sizeof(BinB))) == NULL) {
      vcl_fprintf(stderr, "ERROR(%s): CALLOC failed for pTmpBins[%d].\n",
        pcFName, iNumBinsY);
      exit(-2); }

    /* iNumMax = iAvgGene * 10; */
      iNumMax = pBuckX->iNumEl; /* This might be a large number */
    pBin = pTmpBins;
    for(j = 0; j < iNumBinsY; j++) {
      pBin->piGenes = NULL;
      if((pBin->piGenes = (int *) calloc(iNumMax, sizeof(int))) == NULL) {
  vcl_fprintf(stderr,
    "ERROR(%s): CALLOC failed for pTmpBin[%d]->piGenes[%d].\n",
    pcFName, i, iNumMax);
  exit(-2); }
      pBin->iNumEl = 0;
      pBin->iMaxEl = iNumMax;
      pBin++;
    }
    
    /* Put generators of each X-Bucket0 in TmpBins */
    iNumBinsX = pBuckX->iNumBins;
    iBinStart = pBuckX->iBinFirst;
    pBinX = pBucketsX->pBin+iBinStart;
    for(j = 0; j < iNumBinsX; j++) { /* For each X-bin of BucketX */

      iNumGenesInBin = pBinX->iNumEl;
      piGene = pBinX->piGenes;
      for(k = 0; k < iNumGenesInBin; k++) { /* For each generator per X-bin */

  iGene = *piGene++;
  fY = (pInData+iGene)->Sample.fPosY;
  iY = (int) floor((double) fY);

  iBin = iY - iYmin; /* Y-bin label */
  if(iBin > iBinYlast) {
    vcl_fprintf(stderr, "ERROR(%s): Overflow Y-bin number %d > max = %d\n",
      pcFName, iBin, iBinYlast);
    exit(-2);
  }
  pBin = pTmpBins+iBin;

  iNum = pBin->iNumEl;
  piBinGene = pBin->piGenes+iNum;
  *piBinGene = iGene;
  pBin->iNumEl++;
      } /* Goto next generator : k++ */
      pBinX++;
    } /* Goto next X-bin : j++ */

    /* Transfer data from Tmp Buffer */
    iTmp = 0;
    pBinOld = pTmpBins;
    for(j = 0; j < iNumBinsY; j++) {
      iNum = pBinOld->iNumEl;
      if(iNum > iNumMax) {
  vcl_fprintf(stderr, "ERROR(%s): %d Genes (Max allocated = %d) ",
    pcFName, iNum, iNumMax);
  vcl_fprintf(stderr,  "in Y-bin %d from BucketX %d\n", j, i);
  exit(-3);
      }
      pBinXY->piGenes = NULL;
      if((pBinXY->piGenes = (int *) calloc(iNum, sizeof(int))) == NULL) {
  vcl_fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBinXY[%d]->piGenes[%d].\n",
    pcFName, i, iNum);
  exit(-2); }
      pBinXY->iNumEl = iNum;
      piBinGene = pBinXY->piGenes;
      piBinGeneOld = pBinOld->piGenes;
      for(k = 0; k < iNum; k++) {
  *piBinGene++ = *piBinGeneOld++;
      }
      pBinXY++;
      pBinOld++;
      iTmp += iNum;
    } /* Next Y-bin : j++ */
    if(iTmp != pBuckX->iNumEl) {
      vcl_fprintf(stderr,
        "ERROR(%s):\n\tExpected to load %d genes in Y-bins, not %d\n",
        pcFName, pBuckX->iNumEl, iTmp);
      exit(-2); }
    

    /* Clean-up */
    pBinOld = pTmpBins;
    for(j = 0; j < iNumBinsY; j++) {
      free(pBinOld->piGenes);
      pBinOld++;
    }
    free(pTmpBins);

    /* ---- Create XY-Buckets0 from a sequence of XY-Bins ---- */

    iNumGenesLeft = iNumGenes; /* Total number of generators in BucketX */
    iNum = iNumBucksY - 1;
    j = 0; /* Number of Buckets0 in Y per Bucket0 X */
    iBinStart = 0; /* Start with first XY-bin */
    iMaxGene = 0;
    iMinGene = iNumGenes;
    /* iJumpY = i * iYdim; */
    iJumpY = i * iNumBinsY;
    k = 0;
    while(j < iNum) { /* Fill-in all XY-buckets except last one */
      iNumGenesAcc = 0;
      if(k >= iNumBinsY) break; /* Problem: did not find enough generators */
      if(iBinStart >= iNumBinsY) break; /* Overflow */
      if(iNumGenesLeft < 1) break; /* No more generators left */

      for(k = iBinStart; k < iNumBinsY; k++) {
  m++; /* one more BinXY */
  iBinXYloc = k + iJumpY;
  pBin = pBucketsXY->pBin+iBinXYloc;
  iNumGenesAcc += pBin->iNumEl;
  if(iNumGenesAcc > iLowGene) { /* Target reached */
    iStep = k - iBinStart;
    iTmp = iNumGenesAcc - pBinXY->iNumEl;
    iThresh = (int) ceil((double) iLowGene * 0.5);
    /* Overflow conditions */
    if(iNumGenesAcc > iHighGene && iStep > 1 && iTmp > iThresh) {
      iNumGenesAcc -= pBin->iNumEl; /* Go back */
      pBuckXY->iNumEl = iNumGenesAcc;
      pBuckXY->iBinFirst = iBinStart + iJumpY;
      pBuckXY->iBinLast = iBinXYloc - 1;
      pBuckXY->iNumBins = iStep;
      pBuckXY->Limits.iMinY = iYmin + iBinStart;
      pBuckXY->Limits.iMaxY = iYmin + k;
      iBinStart = k;
      m--;
    }
    else { /* Within acceptable limits */
      pBuckXY->iNumEl = iNumGenesAcc;
      pBuckXY->iBinFirst = iBinStart + iJumpY;
      pBuckXY->iBinLast = iBinXYloc;
      pBuckXY->iNumBins = iStep + 1;
      pBuckXY->Limits.iMinY = iYmin + iBinStart;
      pBuckXY->Limits.iMaxY = iYmin + k + 1;
      iBinStart = k + 1;
    }
    pBuckXY->Limits.iMinX = iXstart;
    pBuckXY->Limits.iMaxX = iXlast;
    pBuckXY->Limits.iMinZ = 0;
    pBuckXY->Limits.iMaxZ = iZdim;
    if(iNumGenesAcc > iMaxGene) {
      iMaxGene = iNumGenesAcc;
      if(iMaxGeneXY < iMaxGene) iMaxGeneXY = iMaxGene;
    }
    if(iNumGenesAcc < iMinGene) {
      iMinGene = iNumGenesAcc;
      if(iMinGeneXY > iMinGene) iMinGeneXY = iMinGene;
    }
    /* Reset target */
    iNumGenesLeft -= iNumGenesAcc;
    iAvgGene = (int)
      ceil((double) iNumGenesLeft / (double) (iNumBucksY - j));
    iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
    iDelta += iDelta;
    iLowGene = iAvgGene;
    iAvgGene += iDelta;
    iHighGene = iAvgGene + iDelta;
    if(iAvgGene < 5) {
      vcl_fprintf(stderr,
        "WARNING(%s):\n\tiAvgGene = %d, for BucketXY %d.\n",
        pcFName, iAvgGene, l);
    }

    if(iAvgGene < 1) {
      iAvgGene = 1;
      iLowGene = 1;
      iHighGene = 10;
    }

    if(iNumGenesLeft < 1) {
      break;
    }

    /* Goto next bucket */
    pBuckXY++; l++;
    j++;
    break;
  } /* End of if(iNumGenesAcc > iLowGene) : Target reached */
      } /* Next bin: k++ */
    } /* End of While(j < iNum) */

    if(j < iNum) {
#if FALSE
      vcl_fprintf(stderr, "WARNING(%s): Only %d BucketsXY set, instead of %d.\n",
        pcFName, j, iNum);
#endif
      /* --- Set remaining BucketsXY --- */

      /* Set one "large" bucket with generators left */
      pBuckXY->iNumEl = iNumGenesLeft;
      pBuckXY->iBinFirst = iBinStart + iJumpY;
      pBuckXY->iBinLast  = iYdim + iJumpY;
      pBuckXY->iNumBins  = iYdim - iBinStart + 1;
      m += pBuckXY->iNumBins;
      pBuckXY->Limits.iMinY = iYmin + iBinStart;
      pBuckXY->Limits.iMaxY = iYmax;
      pBuckXY->Limits.iMinX = iXstart;
      pBuckXY->Limits.iMaxX = iXlast;
      pBuckXY->Limits.iMinZ = 0;
      pBuckXY->Limits.iMaxZ = iZdim;
      pBuckXY++; l++;
      j++;
      if(iNumGenesLeft < 1) {
  iNumBuckEmpty++;
      }
      /* Set other empty buckets with no bins : Virtual buckets */
      while(j < iNum) {
  pBuckXY->iNumEl = 0;
  pBuckXY->iBinFirst = iYdim + iJumpY;
  pBuckXY->iBinLast  = iYdim + iJumpY;
  pBuckXY->iNumBins  = 0;
  pBuckXY->Limits.iMinY = iYmax;
  pBuckXY->Limits.iMaxY = iYmax;
  pBuckXY->Limits.iMinX = iXstart;
  pBuckXY->Limits.iMaxX = iXlast;
  pBuckXY->Limits.iMinZ = 0;
  pBuckXY->Limits.iMaxZ = iZdim;
  pBuckXY++; l++;
  j++;
  iNumBuckEmpty++;
  iNumBuckVirtual++;
      }
    }
    if(iBinStart > iYdim) { /* Overflow */
      vcl_fprintf(stderr,
        "WARNING(%s): Overflow: No bins left for last bucket.\n",
        pcFName);
    }

    /* --- Set last bucket --- */

    pBuckXY->iNumEl = iNumGenesLeft;
    if(iNumGenesLeft < 1) {
      iNumBuckEmpty++;
    }
    pBuckXY->iBinFirst = iBinStart + iJumpY;
    pBuckXY->iBinLast  = iYdim + iJumpY;
    pBuckXY->iNumBins  = iYdim - iBinStart + 1;
    m += pBuckXY->iNumBins;
    pBuckXY->Limits.iMinY = iYmin + iBinStart;
    pBuckXY->Limits.iMaxY = iYmax;
    pBuckXY->Limits.iMinX = iXstart;
    pBuckXY->Limits.iMaxX = iXlast;
    pBuckXY->Limits.iMinZ = 0;
    pBuckXY->Limits.iMaxZ = iZdim;
    pBuckXY++; l++;
    j++; /* one more BucketXY */
    if(iNumGenesLeft > iMaxGene) {
      iMaxGene = iNumGenesLeft;
      if(iMaxGeneXY < iMaxGene) iMaxGeneXY = iMaxGene;
    }
    if(iNumGenesLeft < iMinGene) {
      iMinGene = iNumGenesLeft;
      if(iMinGeneXY > iMinGene) iMinGeneXY = iMinGene;
    }
  
    vcl_fprintf(stderr, "\tAll (%d) BucketsXY set, for BucketX no. %d\n", j, i);
    vcl_fprintf(stderr, "\t\tMax Number of Generators per BucketXY = %d\n",
      iMaxGene);
    vcl_fprintf(stderr, "\t\tMin Number of Generators per BucketXY = %d\n",
      iMinGene);
    vcl_fprintf(stderr, "\t\t%d Empty Buckets0, of which %d are Virtual.\n",
      iNumBuckEmpty, iNumBuckVirtual);
    iNumBuckEmptyTotal += iNumBuckEmpty;
    iNumBuckVirtualTotal += iNumBuckVirtual;
    iNumBuckEmpty = 0;
    iNumBuckVirtual = 0;

    pBuckX++; /* Go process next BucketX : i++ */
  }
  if(l < iNumBucksXY) {
    vcl_fprintf(stderr, "WARNING(%s): Set %d BucketsXY instead of %d\n",
      pcFName, l, iNumBucksXY);
    pBucketsXY->iNumBucks = l;
  }
  else if(l > iNumBucksXY) {
    vcl_fprintf(stderr,
      "ERROR(%s): Set too many BucketsXY: %d > %d (allocated).\n",
      pcFName, l, iNumBucksXY);
    exit(-3);
  }
  else {
    vcl_fprintf(stderr, "MESG(%s): Set all (allocated) %d BucketsXY.\n",
      pcFName, l);
  }
  /* m++; */
  if(m < iNumBinsXY) {
    vcl_fprintf(stderr, "WARNING(%s): Visited %d BinsXY instead of %d .\n",
      pcFName, m, iNumBinsXY);
  }
  else if(m > iNumBinsXY) {
    vcl_fprintf(stderr, "ERROR(%s): Visited too many BinsXY: %d > %d .\n",
      pcFName, m, iNumBinsXY);
   assert (0);
    exit(-4);
  }

  vcl_fprintf(stderr, "\tTotal Number of Empty Bucket0 = %d\n",
    iNumBuckEmptyTotal);
  vcl_fprintf(stderr, "\t\tof which %d are Virtual.\n",
    iNumBuckVirtualTotal);
  vcl_fprintf(stderr, "\tMax Number of Generators per BucketXY = %d\n",
    iMaxGeneXY);
  vcl_fprintf(stderr, "\tMin Number of Generators per BucketXY = %d\n\n",
    iMinGeneXY);

  pBucketsXY->iEmpty   = iNumBuckEmptyTotal;
  pBucketsXY->iVirtual = iNumBuckVirtualTotal;
  pBucketsXY->iMaxGene = iMaxGeneXY;
  pBucketsXY->iMinGene = iMinGeneXY;

  return;
}

/*------------------------------------------------------------*\
 * Function: FillBucketsInXYZ
 *
 * Usage: Fill-in bins (delimited by integer coordinates along
 *    the X, Y & Z axes) and construct Buckets0 in Z for
 *    each Bucket0 in XY, previously filled.
 *    In this final bucket construction stage, we allow for
 *    empty buckets at the begining and end of a Bucket0 in XY
 *    i.e., in the Z direction.
 *    Each XYZ-bin has unit size in Z, a size in X delimited
 *    by the BucketX, and a size in Y delimited by the BucketXY.
 *
\*------------------------------------------------------------*/

void
FillBucketsInXYZ(InputDataB *pInData, Buckets0 *pBucketsXY,
     Buckets0 *pBucketsXYZ, DimensionB *pDim)
{
  const char    *pcFName = "FillBucketsInXYZ";
  register int  i, j, k, l, m, n;
  int    iNumBucksXY, iNumBucksXYZ, iNumBucksZ, iNumGenes, iNumBinsZ;
  int    iBinZlast, iNumBuckEmpty, iNumBuckVirtual, iNumGeneTot;
  int    iAvgGene, iDelta, iLowGene, iHighGene, iNumMax, iBinStart;
  int    iNumBinsXY, iNumGenesInBin, iGene, iZ, iBin, iNum, iJumpZ;
  int    iNumGenesLeft, iMaxGene, iMinGene, iBinEmpty, iFlagEmpty;
  int    iBinXYZloc, iTmp, iFlagGeneLeft, iNumGenesAcc, iStep, iThresh;
  int    iNumBuckEmptyTotal, iNumBuckVirtualTotal, iXstart,iXlast;
  int    iYstart,iYlast, iNumBinsXYZ, iMaxGeneXYZ,iMinGeneXYZ;
  int    iXdim,iYdim,iZdim, iXmin,iXmax, iYmin,iYmax, iZmin,iZmax;
  int    iEmptyBins;
  int    *piGene, *piBinGene, *piBinGeneOld;
  float    fDelta, fAvgGene, fZ;
  Bucket0  *pBuckXY, *pBuckXYZ;
  BinB    *pBinXYZ, *pTmpBins, *pBin, *pBinXY, *pBinOld;

  /* iZdim = pDim->iZdim; */

  iXmin = pBucketsXYZ->Limits.iMinX;
  iXmax = pBucketsXYZ->Limits.iMaxX;
  iXdim = iXmax - iXmin;
  iYmin = pBucketsXYZ->Limits.iMinY;
  iYmax = pBucketsXYZ->Limits.iMaxY;
  iYdim = iYmax - iYmin;
  iZmin = pBucketsXYZ->Limits.iMinZ;
  iZmax = pBucketsXYZ->Limits.iMaxZ;
  iZdim = iZmax - iZmin;
  iNumBinsZ = iZdim + 1;

  iNumBucksXY  = pBucketsXY->iNumBucks;
  iNumBucksXYZ = pBucketsXYZ->iNumBucks;
  iNumBucksZ   = (int) floor((double) iNumBucksXYZ / (double) iNumBucksXY);
  iBinZlast    = iZdim;

  iNumGeneTot = pBucketsXYZ->iTotGene;
  iAvgGene = pBucketsXYZ->iAvgGene; /* Init target of genes per bucket */

  vcl_fprintf(stderr, "MESG(%s): %d BucketsXYZ to fill-in.\n", pcFName,
    iNumBucksXYZ);
  vcl_fprintf(stderr, "\tTarget Number of Generators per Bucket0 = %d\n",
    iAvgGene);
  vcl_fprintf(stderr, "\tfrom a total of %d genes.\n", iNumGeneTot);
  iNumBinsXYZ = pBucketsXYZ->iNumBins;
  vcl_fprintf(stderr, "\tNumber of BinsXYZ = %d .\n", iNumBinsXYZ);

  fDelta = pBucketsXYZ->fTolerance; /* Tolerance in percent */

  /* --- For each XY-Bucket0 in BucketsXY list  --- *\
  \*   create iNumBucksZ along the Z dimension      */ 

  pBuckXY  = pBucketsXY->pBucket;
  pBinXYZ  = pBucketsXYZ->pBin;
  pBuckXYZ = pBucketsXYZ->pBucket;
  l = 0; /* Tracks total number of BucketsXYZ */
  m = 0; /* Tracks total number of BinsXYZ */
  n = 0; /* Tracks number of BinsXYZ per BucketXY : <= iZdim */
  iNumBuckEmpty = iNumBuckEmptyTotal = 0;
  iNumBuckVirtual = iNumBuckVirtualTotal = 0;
  iMaxGeneXYZ = 0;       /* Tracks the overall Max/Min of */
  iMinGeneXYZ = iNumGeneTot; /* generators for all BucketsXYZ */

  for(i = 0; i < iNumBucksXY; i++) {

    iXstart = pBuckXY->Limits.iMinX;
    iXlast  = pBuckXY->Limits.iMaxX;
    iYstart = pBuckXY->Limits.iMinY;
    iYlast  = pBuckXY->Limits.iMaxY;
    if(iXstart >= iXlast) {
      vcl_fprintf(stderr, "ERROR(%s): MinX = %d >= MaxX = %d\n",
        pcFName, iXstart, iXlast);
      exit(-2);
    }
    if(iYstart >= iYlast) {
      vcl_fprintf(stderr, "ERROR(%s): MinY = %d >= MaxY = %d\n",
        pcFName, iYstart, iYlast);
      exit(-2);
    }

    iNumGenes = pBuckXY->iNumEl;
    if(iNumGenes < 1) {
      j = 0; /* Number of Buckets0 in Z per Bucket0 XY */
      pBuckXYZ->iNumEl = 0;
      pBuckXYZ->iBinFirst = iJumpZ;  /* Pointer to current XYZ-bin */
      pBuckXYZ->iBinLast = iJumpZ + iZdim;
      pBuckXYZ->iNumBins = iNumBinsZ;
      pBuckXYZ->Limits.iMinZ = iZmin;
      pBuckXYZ->Limits.iMaxZ = iZmax;
      pBuckXYZ->Limits.iMinX = iXstart;
      pBuckXYZ->Limits.iMaxX = iXlast;
      pBuckXYZ->Limits.iMinY = iYstart;
      pBuckXYZ->Limits.iMaxY = iYlast;
      iNumBuckEmpty++;
      pBuckXYZ++;
      j++;
      /* Set other empty buckets with no bins : Virtual buckets */
      while(j < iNumBucksZ) {
  pBuckXYZ->iNumEl = 0;
  pBuckXYZ->iBinFirst = iZdim + iJumpZ;
  pBuckXYZ->iBinLast  = iZdim + iJumpZ;
  pBuckXYZ->iNumBins  = 0;
  pBuckXYZ->Limits.iMinZ = iZmax;
  pBuckXYZ->Limits.iMaxZ = iZmax;
  pBuckXYZ->Limits.iMinX = iXstart;
  pBuckXYZ->Limits.iMaxX = iXlast;
  pBuckXYZ->Limits.iMinY = iYstart;
  pBuckXYZ->Limits.iMaxY = iYlast;
  pBuckXYZ++;
  j++;
  iNumBuckEmpty++;
  iNumBuckVirtual++;
      }

      vcl_fprintf(stderr,
        "\tAll (%d) BucketsXYZ are EMPTY, for BucketXY no. %d\n", j, i);

      iNumBuckEmptyTotal += iNumBuckEmpty;
      iNumBuckVirtualTotal += iNumBuckVirtual;
      iNumBuckEmpty = 0;
      iNumBuckVirtual = 0;
      l += j;
      m += iNumBinsZ;

      pBuckXY++; /* Go process next BucketXY : i++ */
      continue;
    }

    fAvgGene = (float) iNumGenes / (float) iNumBucksZ;
    iAvgGene = (int) ceil((double) fAvgGene);
    iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
    iLowGene = iAvgGene - iDelta;
    iHighGene = iAvgGene + iDelta;
    
    /* ---- Fill-in Z-Bins (per XY-Bucket0) ---- */

    /* Temp Buffer of Z-bins : we do not know a priori *\
    \*   how many generators per Z-bin we may have     */
    pTmpBins = NULL;
    if((pTmpBins = (BinB *) calloc(iNumBinsZ, sizeof(BinB))) == NULL) {
      vcl_fprintf(stderr, "ERROR(%s): CALLOC failed for pTmpBins[%d].\n",
        pcFName, iNumBinsZ);
      exit(-2); }

    iNumMax = pBuckXY->iNumEl;
    pBin = pTmpBins;
    for(j = 0; j < iNumBinsZ; j++) {
      pBin->piGenes = NULL;
      if((pBin->piGenes = (int *) calloc(iNumMax, sizeof(int))) == NULL) {
  vcl_fprintf(stderr,
    "ERROR(%s): CALLOC failed for pTmpBin[%d]->piGenes[%d].\n",
    pcFName, i, iNumMax);
  exit(-2); }
      pBin->iNumEl = 0;
      pBin->iMaxEl = iNumMax;
      pBin++;
    }
    
    /* Put generators of each XY-Bucket0 in TmpBins */
    iNumBinsXY = pBuckXY->iNumBins;
    iBinStart  = pBuckXY->iBinFirst;
    pBinXY = pBucketsXY->pBin+iBinStart;
    for(j = 0; j < iNumBinsXY; j++) { /* For each XY-bin of BucketXY */

      iNumGenesInBin = pBinXY->iNumEl;
      piGene = pBinXY->piGenes;
      for(k = 0; k < iNumGenesInBin; k++) { /* For each generator per XY-bin */

  iGene = *piGene++;
  fZ = (pInData+iGene)->Sample.fPosZ;
  iZ = (int) floor((double) fZ);

  iBin = iZ - iZmin; /* Z-Bin label */
  if(iBin > iBinZlast) {
    vcl_fprintf(stderr, "ERROR(%s): Overflow BinB number %d > max = %d\n",
      pcFName, iBin, iBinZlast);
    exit(-2);
  }
  pBin = pTmpBins+iBin;

  iNum = pBin->iNumEl;
  piBinGene = pBin->piGenes+iNum;
  *piBinGene = iGene;
  pBin->iNumEl++;
      } /* Goto next generator : k++ */
      pBinXY++;
    } /* Goto next XY-bin : j++ */

    /* Transfer data from Tmp Buffer */
    iTmp = 0;
    pBinOld = pTmpBins;
    for(j = 0; j < iNumBinsZ; j++) {
      iNum = pBinOld->iNumEl;
      if(iNum > iNumMax) {
  vcl_fprintf(stderr, "ERROR(%s): Overflow: %d Genes (Max allocated = %d)\n",
    pcFName, iNum, iNumMax);
  vcl_fprintf(stderr,  "\tin Z-BinB %d of BucketXY %d\n", j, i);
  exit(-2);
      }
      pBinXYZ->piGenes = NULL;
      if((pBinXYZ->piGenes = (int *) calloc(iNum, sizeof(int))) == NULL) {
  vcl_fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBinXYZ[%d]->piGenes[%d].\n",
    pcFName, i, iNum);
  exit(-2); }
      pBinXYZ->iNumEl = iNum;
      piBinGene = pBinXYZ->piGenes;
      piBinGeneOld = pBinOld->piGenes;
      for(k = 0; k < iNum; k++) {
  *piBinGene++ = *piBinGeneOld++;
      }
      pBinXYZ++;
      pBinOld++;
      iTmp += iNum;
    } /* Next Z-bin : j++ */
    if(iTmp != pBuckXY->iNumEl) {
      vcl_fprintf(stderr,
        "ERROR(%s):\n\tExpected to load %d genes in Z-Bins, not %d\n",
        pcFName, pBuckXY->iNumEl, iTmp);
      exit(-2); }

    /* Clean-up */
    pBinOld = pTmpBins;
    for(j = 0; j < iZdim; j++) {
      free(pBinOld->piGenes);
      pBinOld++;
    }
    free(pTmpBins);

    /* ---- Create XYZ-Buckets0 from a sequence of XYZ-Bins ---- */

    iNumGenesLeft = iNumGenes; /* Total number of generators in BucketXY */
    iNum = iNumBucksZ - 1;
    j = 0; /* Number of Buckets0 in Z per Bucket0 XY */
    iBinStart = 0; /* Start with first XYZ-bin */
    iMaxGene = 0;
    iMinGene = iNumGenes;
    iJumpZ = i * iNumBinsZ;

    /* First: Set an Initial EMPTY Bucket0 if needed */
    iBinEmpty = 0;
    iFlagEmpty = TRUE;
    k = 0; /* Number of bins along Z */
    n = 0; /* Tracks number of BinsXYZ per BucketXY : <= iZdim */
    while(iFlagEmpty && k < iNumBinsZ) {
      iBinXYZloc = k + iJumpZ;
      pBin = pBucketsXYZ->pBin+iBinXYZloc;
      iTmp = pBin->iNumEl;
      if(iTmp < 1) {
  iBinEmpty++; k++;
  m++; n++; }
      else iFlagEmpty = FALSE;
    }
    if(iBinEmpty > 1) { /* At least 2 empty bins */
      pBuckXYZ->iNumEl = 0;
      pBuckXYZ->iBinFirst = iBinStart + iJumpZ;  /* Current XYZ-bin */
      pBuckXYZ->iBinLast = iBinXYZloc - 1;
      pBuckXYZ->iNumBins = iBinEmpty; /* k - iBinStart */
      pBuckXYZ->Limits.iMinZ = iZmin;
      pBuckXYZ->Limits.iMaxZ = iZmin + k;
      pBuckXYZ->Limits.iMinX = iXstart;
      pBuckXYZ->Limits.iMaxX = iXlast;
      pBuckXYZ->Limits.iMinY = iYstart;
      pBuckXYZ->Limits.iMaxY = iYlast;
      iBinStart = k;
      pBuckXYZ++; l++; /* Goto next bucket */
      j++;
      iNumBuckEmpty++;
    }
    else if(iBinEmpty == 1) { /* Start from begining */
      m--; k = n = 0; }
    
    iFlagGeneLeft = TRUE;
    while(iFlagGeneLeft && j < iNum) { /* Fill-in BucketsZ except last one */
      iNumGenesAcc = 0;
      if(k >= iNumBinsZ) break; /* Problem: did not find enough generators */
      if(iBinStart >= iNumBinsZ) break; /* Overflow */
      if(iNumGenesLeft < 1) break; /* No more generators left */

      for(k = iBinStart; k < iNumBinsZ; k++) { /* In remaining Bins */
  m++; /* one more BinXYZ */
  n++;
  iBinXYZloc = k + iJumpZ;
  pBin = pBucketsXYZ->pBin+iBinXYZloc;
  iNumGenesAcc += pBin->iNumEl;
  if(iNumGenesAcc > iLowGene) { /* Target reached */
    iStep = k - iBinStart;
    iTmp = iNumGenesAcc - pBin->iNumEl;
    iThresh = (int) ceil((double) iAvgGene * 0.5);
    /* Overflow conditions */
    if(iNumGenesAcc > iHighGene && iStep > 1 && iTmp > iThresh) {
      iNumGenesAcc -= pBin->iNumEl; /* Go back */
      pBuckXYZ->iNumEl = iNumGenesAcc;
      pBuckXYZ->iBinFirst = iBinStart + iJumpZ;
      pBuckXYZ->iBinLast = iBinXYZloc - 1;
      pBuckXYZ->iNumBins = iStep;
      pBuckXYZ->Limits.iMinZ = iZmin + iBinStart;
      pBuckXYZ->Limits.iMaxZ = iZmin + k;
      iBinStart = k;
      m--; n--;
    }
    else { /* Within acceptable limits */
      pBuckXYZ->iNumEl = iNumGenesAcc;
      pBuckXYZ->iBinFirst = iBinStart + iJumpZ;
      pBuckXYZ->iBinLast = iBinXYZloc;
      pBuckXYZ->iNumBins = iStep + 1;
      pBuckXYZ->Limits.iMinZ = iZmin + iBinStart;
      pBuckXYZ->Limits.iMaxZ = iZmin + k + 1;
      iBinStart = k + 1;
    }
    pBuckXYZ->Limits.iMinX = iXstart;
    pBuckXYZ->Limits.iMaxX = iXlast;
    pBuckXYZ->Limits.iMinY = iYstart;
    pBuckXYZ->Limits.iMaxY = iYlast;

    if(iNumGenesAcc > iMaxGene) {
      iMaxGene = iNumGenesAcc;
      if(iMaxGeneXYZ < iMaxGene) iMaxGeneXYZ = iMaxGene;
    }
    if(iNumGenesAcc < iMinGene) {
      iMinGene = iNumGenesAcc;
      if(iMinGeneXYZ > iMinGene) iMinGeneXYZ = iMinGene;
    }
    /* Reset target */
    iNumGenesLeft -= iNumGenesAcc;
    iAvgGene = (int)
      ceil((double) iNumGenesLeft / (double) (iNumBucksZ - j));
    iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
    iLowGene = iAvgGene - iDelta;
    iHighGene = iAvgGene + iDelta;
    if(iAvgGene < 1) { /* No generators left */
      iFlagGeneLeft = FALSE;
    }
    /* Goto next bucket */
    pBuckXYZ++; l++;
    j++;
    break;
  } /* End of if(iNumGenesAcc > iLowGene) : Target reached */
      } /* Next bin: k++ */
    } /* End of While(j < iNum) */

    if(iBinStart >= iNumBinsZ) { /* Overflow */
      vcl_fprintf(stderr,
        "WARNING(%s):\n\tOverflow: No bins left for last BucketZ\n",
        pcFName);
      vcl_fprintf(stderr, "\twhen processing BucketXY no.%d (out of %d).\n",
        i, iNumBucksXY);
    }
    if(k >= iNumBinsZ) {
      vcl_fprintf(stderr, "WARNING(%s):\n\tWent through all bins\n",
        pcFName);
      vcl_fprintf(stderr, "\tbut did not find enough generators.\n");
    }

    if(iNumGenesLeft < 1) { /* No generators left : Set Empty buckets */
      /* Set one "large" empty bucket */
      pBuckXYZ->iNumEl = 0;
      pBuckXYZ->iBinFirst = iBinStart + iJumpZ;
      pBuckXYZ->iBinLast = iZdim + iJumpZ;
      pBuckXYZ->iNumBins = iZdim - iBinStart + 1;
      m += pBuckXYZ->iNumBins;
      n += pBuckXYZ->iNumBins;
      pBuckXYZ->Limits.iMinZ = iZmin + iBinStart;
      pBuckXYZ->Limits.iMaxZ = iZmax;
      pBuckXYZ->Limits.iMinX = iXstart;
      pBuckXYZ->Limits.iMaxX = iXlast;
      pBuckXYZ->Limits.iMinY = iYstart;
      pBuckXYZ->Limits.iMaxY = iYlast;
      pBuckXYZ++; l++;
      j++;
      iNumBuckEmpty++;
      /* Set other empty buckets with no bins : Virtual buckets */
      while(j < iNumBucksZ) {
  pBuckXYZ->iNumEl = 0;
  pBuckXYZ->iBinFirst = iZdim + iJumpZ;
  pBuckXYZ->iBinLast  = iZdim + iJumpZ;
  pBuckXYZ->iNumBins  = 0;
  pBuckXYZ->Limits.iMinZ = iZmax;
  pBuckXYZ->Limits.iMaxZ = iZmax;
  pBuckXYZ->Limits.iMinX = iXstart;
  pBuckXYZ->Limits.iMaxX = iXlast;
  pBuckXYZ->Limits.iMinY = iYstart;
  pBuckXYZ->Limits.iMaxY = iYlast;
  pBuckXYZ++; l++;
  j++;
  iNumBuckEmpty++;
  iNumBuckVirtual++;
      }
    }
    else { /* Set last bucket with remaining generators and bins */
      pBuckXYZ->iNumEl = iNumGenesLeft;
      pBuckXYZ->iBinFirst = iBinStart + iJumpZ;
      pBuckXYZ->iBinLast  = iZdim + iJumpZ;
      pBuckXYZ->iNumBins  = iZdim - iBinStart + 1;
      m += pBuckXYZ->iNumBins;
      n += pBuckXYZ->iNumBins;
      pBuckXYZ->Limits.iMinZ = iZmin + iBinStart;
      pBuckXYZ->Limits.iMaxZ = iZmax;
      pBuckXYZ->Limits.iMinX = iXstart;
      pBuckXYZ->Limits.iMaxX = iXlast;
      pBuckXYZ->Limits.iMinY = iYstart;
      pBuckXYZ->Limits.iMaxY = iYlast;
      pBuckXYZ++; l++;
      j++; /* one more BucketXYZ */
      if(iNumGenesLeft > iMaxGene) {
  iMaxGene = iNumGenesLeft;
  if(iMaxGeneXYZ < iMaxGene) iMaxGeneXYZ = iMaxGene;
      }
      if(iNumGenesLeft < iMinGene) {
  iMinGene = iNumGenesLeft;
  if(iMinGeneXYZ > iMinGene) iMinGeneXYZ = iMinGene;
      }
    }

    ///vcl_fprintf(stderr, "\tAll (%d) BucketsXYZ set, for BucketXY no. %d\n", j, i);
    ///vcl_fprintf(stderr, "\t\tMax Number of Generators per BucketXYZ = %d\n",
   ///   iMaxGene);
    ///vcl_fprintf(stderr, "\t\tMin Number of Generators per BucketXYZ = %d\n",
   ///   iMinGene);
    ///vcl_fprintf(stderr, "\t\t%d Empty Buckets0, of which %d are Virtual.\n",
   ///   iNumBuckEmpty, iNumBuckVirtual);

    if(n != iNumBinsZ) {
      vcl_fprintf(stderr,
        "WARNING(%s): Visited %d BinsXYZ instead of %d, for XY-Bucket0 %d\n",
        pcFName, n, iNumBinsZ, i);
    }

    iNumBuckEmptyTotal += iNumBuckEmpty;
    iNumBuckVirtualTotal += iNumBuckVirtual;
    iNumBuckEmpty = 0;
    iNumBuckVirtual = 0;

    pBuckXY++; /* Go process next BucketXY : i++ */
  }
  if(l < iNumBucksXYZ) {
    vcl_fprintf(stderr, "WARNING(%s): Set %d BucketsXYZ instead of %d\n",
      pcFName, l, iNumBucksXYZ);
    pBucketsXYZ->iNumBucks = l;
  }
  else if(l > iNumBucksXYZ) {
    vcl_fprintf(stderr,
      "ERROR(%s): Set too many BucketsXYZ: %d > %d (allocated).\n",
      pcFName, l, iNumBucksXYZ);
    exit(-3);
  }
  else {
    vcl_fprintf(stderr, "MESG(%s): Set all (allocated) %d BucketsXYZ.\n",
      pcFName, l);
  }
  /* m++; */
  if(m < iNumBinsXYZ) {
    vcl_fprintf(stderr, "WARNING(%s): Visited %d BinsXYZ instead of %d .\n",
      pcFName, m, iNumBinsXYZ);
  }
  else if(m > iNumBinsXYZ) {
    vcl_fprintf(stderr, "ERROR(%s): Visited too many BinsXYZ: %d > %d .\n",
      pcFName, m, iNumBinsXYZ);
    exit(-4);
  }

  iEmptyBins = 0;
  pBinXYZ = pBucketsXYZ->pBin;
  for(i = 0; i < iNumBinsXYZ; i++) {
    if(pBinXYZ->iNumEl < 1)
      iEmptyBins++;
    pBinXYZ++;
  }

  vcl_fprintf(stderr, "\tTotal Number of Empty Bins = %d\n", iEmptyBins);
  vcl_fprintf(stderr, "\tTotal Number of Empty XYZ-Buckets0 = %d\n",
    iNumBuckEmptyTotal);
  vcl_fprintf(stderr, "\t\tof which %d are Virtual.\n",
    iNumBuckVirtualTotal);
  vcl_fprintf(stderr,
    "\tNumber of Generators per XYZ-Bucket0: Min = %d , Max = %d\n",
    iMinGeneXYZ, iMaxGeneXYZ);

  pBucketsXYZ->iEmpty    = iNumBuckEmptyTotal;
  pBucketsXYZ->iVirtual  = iNumBuckVirtualTotal;
  pBucketsXYZ->iMaxGene  = iMaxGeneXYZ;
  pBucketsXYZ->iMinGene  = iMinGeneXYZ;
  pBucketsXYZ->iBinEmpty = iEmptyBins;

  return;
}

/*------------------------------------------------------------*\
 * Function: FillBucketsInXFromMetaBucket
 *
 * Usage: Fill-in bins (delimited by integer coordinates along
 *    the X axis only) and construct Buckets0 in X for a
 *    MetaBucket previously filled.
 *    Each X-bin has unit size in X, and full extent in Y & Z
 *    (i.e., Ydim and Zdim of the MetaBucket).
 *
\*------------------------------------------------------------*/

void
FillBucketsInXFromMetaBucket(InputDataB *pInData, BinB *pBins,
           Bucket0 *pMetaBuck, Buckets0 *pBucketsX,
           DimensionB *pDim)
{
  const char    *pcFName = "FillBucketsInXFromMetaBucket";
  register int  i, j, k;
  int    iYmin,iYmax,iZmin,iZmax, iNumGeneInMetaBuck;
  int    iBinXfirst,iBinXlast, iNumBinsX,iNumBucksX, iAvgGene;
  int    iNumMax, iGene, iX, iBin, iNum, iDelta, iLowGene;
  int    iHighGene, iMaxGene,iMinGene, iBinStart, iNumGenesLeft;
  int    iNumGenes, iTmp, iNumBucksOld, iFlagAddBuck, iBinFirst;
  int    iNumBins, iNumGeneInBin;
  int    *piGene, *piBinGene, *piBinGeneOld;
  float    fX, fDelta;
  InputDataB  *pGene;
  BinB    *pTmpBins, *pBin, *pBinOld, *pBinNew;
  Bucket0  *pBuck;

  iYmin = pMetaBuck->Limits.iMinY;
  iYmax = pMetaBuck->Limits.iMaxY;
  iZmin = pMetaBuck->Limits.iMinZ;
  iZmax = pMetaBuck->Limits.iMaxZ;

  iNumGeneInMetaBuck = pMetaBuck->iNumEl;
  iBinXfirst = pMetaBuck->Limits.iMinX;
  iNumBinsX = pBucketsX->iNumBins;
  iBinXlast = pMetaBuck->Limits.iMaxX;
  iAvgGene = pBucketsX->iAvgGene;
  iNumBucksX = pBucketsX->iNumBucks;

  vcl_fprintf(stderr, "MESG(%s):\n\tInitially, %d BucketsX to fill-in.\n",
    pcFName, iNumBucksX);
  vcl_fprintf(stderr, "\tTarget Number of Generators per Bucket0 = %d\n",
    iAvgGene);
  vcl_fprintf(stderr, "\tfrom a total of %d genes.\n", iNumGeneInMetaBuck);
  vcl_fprintf(stderr, "\tNumber of BinsX = %d\n", iNumBinsX);

  if(iBinXfirst >= iBinXlast) {
    vcl_fprintf(stderr, "ERROR(%s): MinX = %d >= MaxX = %d\n",
      pcFName, iBinXfirst, iBinXlast);
    exit(-2);
  }

  /* ---- Fill-in Bins ---- */

  /* Temp Buffer of bins : we do not assume a priori *\
  \*     how many generators per bin we may have     */
  pTmpBins = NULL;
  if((pTmpBins = (BinB *) calloc(iNumBinsX, sizeof(BinB))) == NULL) {
    vcl_fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for pTmpBins[%d].\n",
      pcFName, iNumBinsX);
    exit(-2); }

  iNumMax = iNumGeneInMetaBuck;
  pBin = pTmpBins;
  for(i = 0; i < iNumBinsX; i++) {
    pBin->piGenes = NULL;
    if((pBin->piGenes = (int *) calloc(iNumMax, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
        "ERROR(%s):\n\tCALLOC failed for pBin[%d]->piGenes[%d].\n",
        pcFName, i, iNumMax);
      exit(-2); }
    pBin->iNumEl = 0;
    pBin->iMaxEl = iNumMax;
    pBin++;
  }

  /* Put generators from Bins of MetaBucket into TmpBins */

  iBinFirst = pMetaBuck->iBinFirst;
  iNumBins  = pMetaBuck->iNumBins;
  pBin = pBins+iBinFirst;
  k = 0;
  for(i = 0; i < iNumBins; i++) {
    iNumGeneInBin = pBin->iNumEl;
    piGene = pBin->piGenes;
    for(j = 0; j < iNumGeneInBin; j++) {
      iGene = *piGene++;
      pGene = pInData+iGene;
      fX = pGene->Sample.fPosX;
      iX = (int) floor((double) fX);
      iBin = iX - iBinXfirst; /* Bin label */
      if(iBin > iBinXlast || iBin < 0) {
  vcl_fprintf(stderr,
    "ERROR(%s):\n\tOverflow in Bin number = %d (min = 0, max = %d).\n",
    pcFName, iBin, iBinXlast);
  exit(-2);
      }
      pBinNew = pTmpBins+iBin;
      iNum = pBinNew->iNumEl;
      piBinGene = pBinNew->piGenes+iNum;
      *piBinGene = iGene;
      pBinNew->iNumEl++;
      k++;
    } /* Next gene in BinB: j++ */
    pBin++;
  } /* Next BinB : i++ */
  if(k != iNumGeneInMetaBuck) {
    vcl_fprintf(stderr,
      "ERROR(%s): Loaded %d genes instead of %d in TmpBins.\n",
      pcFName, k, iNumGeneInMetaBuck);
  }

  /* Transfer data from Tmp Buffer */
  pBinOld = pTmpBins;
  pBin = pBucketsX->pBin;
  k = 0;
  for(i = 0; i < iNumBinsX; i++) {
    iNum = pBinOld->iNumEl;
    pBin->piGenes = NULL;
    if((pBin->piGenes = (int *) calloc(iNum, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
        "ERROR(%s):\n\tCALLOC failed for pBin[%d]->piGenes[%d].\n",
        pcFName, i, iNum);
      exit(-2); }
    pBin->iMaxEl = pBin->iNumEl = iNum;
    piBinGene = pBin->piGenes;
    piBinGeneOld = pBinOld->piGenes;
    for(j = 0; j < iNum; j++) {
      *piBinGene++ = *piBinGeneOld++;
    }
    pBin++;
    pBinOld++;
    k += iNum;
  }
  if(k != iNumGeneInMetaBuck) {
    vcl_fprintf(stderr,
      "ERROR(%s): Transferred %d genes instead of %d from TmpBins.\n",
      pcFName, k, iNumGeneInMetaBuck);
  }

  /* Clean-up */
  pBinOld = pTmpBins;
  for(i = 0; i < iNumBinsX; i++) {
    free(pBinOld->piGenes);
    pBinOld++;
  }
  free(pTmpBins);

  /* ---- Fill-in X-Buckets0 from Sequences of Bins ---- */

  fDelta = pBucketsX->fTolerance; /* Tolerance in percent */
  iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
  iLowGene = iAvgGene;
  iAvgGene += iDelta;
  iHighGene = iAvgGene + iDelta;
#if MY_DEBUG
  vcl_fprintf(stderr, "\tFilling BucketsX from BinsX\n");
  vcl_fprintf(stderr,
    "\tTargets (srcs per bucket): Min = %d , Avg = %d , Max = %d\n",
    iLowGene, iAvgGene, iHighGene);
#endif

  pBuck = pBucketsX->pBucket;
  pBin = pBucketsX->pBin;
  iMaxGene = 0;
  iMinGene = iNumGeneInMetaBuck;
  i = 0; /* Tracks number of BucketsX */
  j = 0; /* Tracks number of BinsX */
  iBinStart = 0; /* Start with first bin */
  iNumGenesLeft = iNumGeneInMetaBuck;
  iNum = iNumBucksX - 1;
  while(i < iNum) { /* Fill-in all BucketsX except last one */
    iNumGenes = 0;
    if(iBinStart >= iNumBinsX) break; /* Overflow */
    if(iNumGenesLeft < 1) break; /* No more generators left */

    for(j = iBinStart; j < iNumBinsX; j++) {
      pBin = pBucketsX->pBin+j;
      iNumGenes += pBin->iNumEl;
      if(iNumGenes > iLowGene) { /* Target reached */
  iTmp = j - iBinStart;
  if(iNumGenes > iHighGene && iTmp > 0) { /* Overflow */
    iNumGenes -= pBin->iNumEl; /* Go back */
    pBuck->iNumEl = iNumGenes;
    pBuck->iBinFirst = iBinStart;
    pBuck->iBinLast = j - 1;
    pBuck->iNumBins = iTmp;
    pBuck->Limits.iMinX = iBinXfirst + iBinStart;
    pBuck->Limits.iMaxX = iBinXfirst + j;
    iBinStart = j;
  }
  else { /* Within desired limits */
    pBuck->iNumEl = iNumGenes;
    pBuck->iBinFirst = iBinStart;
    pBuck->iBinLast = j;
    pBuck->iNumBins = iTmp + 1;
    pBuck->Limits.iMinX = iBinXfirst + iBinStart;
    pBuck->Limits.iMaxX = iBinXfirst + j + 1;
    iBinStart = j + 1;
  }
  pBuck->Limits.iMinY = iYmin;
  pBuck->Limits.iMaxY = iYmax;
  pBuck->Limits.iMinZ = iZmin;
  pBuck->Limits.iMaxZ = iZmax;
  
  if(iNumGenes > iMaxGene) iMaxGene = iNumGenes;
  if(iNumGenes < iMinGene) iMinGene = iNumGenes;
  /* Reset target */
  iNumGenesLeft -= iNumGenes;
  iAvgGene = (int)
    ceil((double) iNumGenesLeft / (double) (iNumBucksX - i));
  iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
  iLowGene = iAvgGene;
  iAvgGene += iDelta;
  iHighGene = iAvgGene + iDelta;
  if(iAvgGene < 10) {
    vcl_fprintf(stderr,
      "WARNING(%s):\n\tiAvgGene = %d, for BucketX %d.\n",
      pcFName, iAvgGene, i);
  }
  if(iAvgGene < 1) {
    iAvgGene = 1;
    iLowGene = 1;
    iHighGene = 10;
  }

  if(iNumGenesLeft < 1) {
    break;
  }

  /* Goto next bucket */
  pBuck++;
  i++;
  break;
      } /* End of if(iNumGenes > iLowGene) : Target reached */
    } /* Next bin: j++ */
  } /* End of While loop */
#if MY_DEBUG
  if(i < iNum) {
    vcl_fprintf(stderr,
      "WARNING(%s):\n\tOnly %d BucketsX set, instead of %d.\n",
      pcFName, i, iNum);
  }
#endif
  if(iBinStart >= iNumBinsX) { /* Overflow */
    vcl_fprintf(stderr,
      "WARNING(%s):\n\tOverflow: %d bins used: 0 left for last bucket.\n",
      pcFName, iBinStart);
  }
  j++;

  /* ---- Set Last Bucket0(s) ---- */

  iAvgGene = pBucketsX->iAvgGene; /* Init target num of genes per bucket */  
  iDelta = (int) ceil((double) ((fDelta * iAvgGene) / 100.0));
  iLowGene = iAvgGene;
  iAvgGene += iDelta;
  iHighGene = iAvgGene + iDelta;

  /* OK: fill last Bucket0 with remaining generators */
  if(iNumGenesLeft < iHighGene) {
    pBuck->iNumEl = iNumGenesLeft;
    pBuck->iBinFirst = iBinStart;
    pBuck->iBinLast = iNumBinsX - 1;
    pBuck->iNumBins = iNumBinsX - iBinStart;
    j += pBuck->iNumBins;
    pBuck->Limits.iMinX = iBinXfirst + iBinStart;
    pBuck->Limits.iMaxX = iBinXlast;
    pBuck->Limits.iMinY = iYmin;
    pBuck->Limits.iMaxY = iYmax;
    pBuck->Limits.iMinZ = iZmin;
    pBuck->Limits.iMaxZ = iZmax;
    i++;
    if(iNumGenesLeft > iMaxGene) iMaxGene = iNumGenesLeft;
    if(iNumGenesLeft < iMinGene) iMinGene = iNumGenesLeft;
  }
  else { /* Too many generators left: Need to add new buckets */

    /* First : Fill-in the (presently) last bucket allocated */

    iNumGenes = 0;
    for(j = iBinStart; j < iNumBinsX; j++) {
      pBin = pBucketsX->pBin+j;
      iNumGenes += pBin->iNumEl;
      if(iNumGenes > iLowGene) { /* Target reached */
  iTmp = j - iBinStart;
  if(iNumGenes > iHighGene && iTmp > 0) { /* Overflow */
    iNumGenes -= pBin->iNumEl; /* Go back */
    pBuck->iNumEl = iNumGenes;
    pBuck->iBinFirst = iBinStart;
    pBuck->iBinLast = j - 1;
    pBuck->iNumBins = iTmp;
    pBuck->Limits.iMinX = iBinXfirst + iBinStart;
    pBuck->Limits.iMaxX = iBinXfirst + j;
    iBinStart = j;
  }
  else { /* Within desired limits */
    pBuck->iNumEl = iNumGenes;
    pBuck->iBinFirst = iBinStart;
    pBuck->iBinLast = j;
    pBuck->iNumBins = iTmp + 1;
    pBuck->Limits.iMinX = iBinXfirst + iBinStart;
    pBuck->Limits.iMaxX = iBinXfirst + j + 1;
    iBinStart = j + 1;
  }
  pBuck->Limits.iMinY = iYmin;
  pBuck->Limits.iMaxY = iYmax;
  pBuck->Limits.iMinZ = iZmin;
  pBuck->Limits.iMaxZ = iZmax;
  
  if(iNumGenes > iMaxGene) iMaxGene = iNumGenes;
  if(iNumGenes < iMinGene) iMinGene = iNumGenes;
  /* Reset target */
  iNumGenesLeft -= iNumGenes;
  i++;
  break; /* Get out of For loop */
      }
    }
    j++;

    /* -- 2nd: Add, 1 by 1, new buckets function of generators left -- */

    iNumBucksOld = iNumBucksX;
    iFlagAddBuck = TRUE;
    while(iFlagAddBuck) {
      /* Add space for one more bucket */
      iNumBucksX++;
      if((pBucketsX->pBucket =
    (Bucket0 *) realloc((Bucket0 *) pBucketsX->pBucket,
           iNumBucksX * sizeof(Bucket0))) == NULL) {
  vcl_fprintf(stderr,
    "ERROR(%s):\n\tREALLOC failed on pBucketsX->pBucket[%d].\n",
    pcFName, iNumBucksX);
  exit(-2); }
      
      iTmp = iNumBucksX - 1;
      pBuck = pBucketsX->pBucket+iTmp; /* Pointer to lastly created Bucket0 */
      iNumGenes = 0;
      pBuck->iNumCHgenes = 0;

      if(iNumGenesLeft < iLowGene) { /* Not much left: Fill this last bucket */
  pBuck->iNumEl = iNumGenesLeft;
  pBuck->iBinFirst = iBinStart;
  pBuck->iBinLast = iNumBinsX - 1;
  pBuck->iNumBins = iNumBinsX - iBinStart;
  j += pBuck->iNumBins;
  pBuck->Limits.iMinX = iBinXfirst + iBinStart;
  pBuck->Limits.iMaxX = iBinXlast;
  pBuck->Limits.iMinY = iYmin;
  pBuck->Limits.iMaxY = iYmax;
  pBuck->Limits.iMinZ = iZmin;
  pBuck->Limits.iMaxZ = iZmax;
  iBinStart = iNumBinsX;
  if(iNumGenesLeft > iMaxGene) iMaxGene = iNumGenesLeft;
  if(iNumGenesLeft < iMinGene) iMinGene = iNumGenesLeft;
  i++;
  iNumGenesLeft = 0;
  iFlagAddBuck = FALSE;
  break; /* Get out of While loop */
      }

      for(j = iBinStart; j < iNumBinsX; j++) {
  pBin = pBucketsX->pBin+j;
  iNumGenes += pBin->iNumEl;
  if(iNumGenes > iLowGene) { /* Target reached */
    iTmp = j - iBinStart;
    if(iNumGenes > iHighGene && iTmp > 0) { /* Overflow */
      iNumGenes -= pBin->iNumEl; /* Go back */
      pBuck->iNumEl = iNumGenes;
      pBuck->iBinFirst = iBinStart;
      pBuck->iBinLast = j - 1;
      pBuck->iNumBins = iTmp;
      pBuck->Limits.iMinX = iBinXfirst + iBinStart;
      pBuck->Limits.iMaxX = iBinXfirst + j;
      iBinStart = j;
    }
    else { /* Within desired limits */
      pBuck->iNumEl = iNumGenes;
      pBuck->iBinFirst = iBinStart;
      pBuck->iBinLast = j;
      pBuck->iNumBins = iTmp + 1;
      pBuck->Limits.iMinX = iBinXfirst + iBinStart;
      pBuck->Limits.iMaxX = iBinXfirst + j + 1;
      iBinStart = j + 1;
    }
    pBuck->Limits.iMinY = iYmin;
    pBuck->Limits.iMaxY = iYmax;
    pBuck->Limits.iMinZ = iZmin;
    pBuck->Limits.iMaxZ = iZmax;
    if(iNumGenes > iMaxGene) iMaxGene = iNumGenes;
    if(iNumGenes < iMinGene) iMinGene = iNumGenes;
    /* Reset target */
    iNumGenesLeft -= iNumGenes;
    i++;
    break; /* Get out of For loop */
  }
      } /* End of For(j) loop */
      j++;

      iTmp = (int) ceil((double) iLowGene * 0.3);
      if(iNumGenesLeft < iTmp) { /* Not much left: Add it to last Bucket0 */
  iNumGenes += iNumGenesLeft;
  pBuck->iNumEl = iNumGenes;
  /* pBuck->iBinFirst = iBinStart; */ 
  pBuck->iBinLast = iNumBinsX - 1;
  pBuck->iNumBins = iNumBinsX - pBuck->iBinFirst;
  /* j += pBuck->iNumBins; */
  /* No change to beginning */
  /* pBuck->Limits.iMinX = iBinXfirst + iBinStart; */
  pBuck->Limits.iMaxX = iBinXlast;
  if(iNumGenes > iMaxGene) iMaxGene = iNumGenes;
  if(iNumGenes < iMinGene) iMinGene = iNumGenes;
  iNumGenesLeft = 0;
  iFlagAddBuck = FALSE;
  break; /* Get out of While loop */
      }
    } /* End of While(iFlagAddBuck) loop */

    pBucketsX->iNumBucks = iNumBucksX; /* Final number of Buckets0 */
    iTmp = iNumBucksX - iNumBucksOld;
    vcl_fprintf(stderr, "\tAdded %d new BucketsX to the initial %d.\n", iTmp,
      iNumBucksOld);
  }

  vcl_fprintf(stderr, "\tAll (%d) BucketsX set.\n", i);
  vcl_fprintf(stderr, "\t\tMax Number of Generators per BucketX = %d\n", iMaxGene);
  vcl_fprintf(stderr, "\t\tMin Number of Generators per BucketX = %d\n", iMinGene);

  pBucketsX->iMaxGene = iMaxGene;
  pBucketsX->iMinGene = iMinGene;

#if MY_DEBUG
  if(j < iNumBins) {
    vcl_fprintf(stderr, "WARNING(%s):\n\tVisited %d BinsX instead of %d .\n",
      pcFName, j, iNumBins);
  }
#endif
  vcl_fprintf(stderr, "\n");

  return;
}

/*------------------------------------------------------------*\
 * Function: FillBinsInXYZ
 *
 * Usage: Fill-in bins (delimited by integer coordinates along
 *    the X, Y & Z axes)
 *    Each XYZ-bin has unit size in X, Y & Z, i.e., the size
 *    of a voxel.
 *
\*------------------------------------------------------------*/

void
FillBinsInXYZ(InputDataB *pInData, BinB *pBins, DimensionB *pDim)
{
  const char    *pcFName = "FillBinsInXYZ";
  register int  i, j, k;
  int    iNumBins, iNumGeneTot, iSliceSize, iXdim, iX,iY,iZ;
  int    iBin, iNum, iMin,iMax, iBinEmpty, iNumMax;
  int    *piBinGene, *piBinGeneOld;
  float    fX,fY,fZ;
  InputDataB  *pGene;
  BinB    *pBin, *pTmpBins, *pBinOld;

  iNumBins = pDim->iWSpaceSize;
  iNumGeneTot = pDim->iNum3dPts;
  vcl_fprintf(stderr,
    "MESG(%s): %d BinsXYZ (voxels) to fill-in from %d points...\n",
    pcFName, iNumBins, iNumGeneTot);


  /* Temp Buffer of bins : we do not assume a priori *\
  \*     how many generators per bin we may have     */
  pTmpBins = NULL;
  if((pTmpBins = (BinB *) calloc(iNumBins, sizeof(BinB))) == NULL) {
    vcl_fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for pTmpBins[%d].\n",
      pcFName, iNumBins);
    exit(-2); }

  iNumMax = 10;
  pBin = pTmpBins;
  for(i = 0; i < iNumBins; i++) {
    pBin->piGenes = NULL;
    if((pBin->piGenes = (int *) calloc(iNumMax, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
        "ERROR(%s):\n\tCALLOC failed for pBin[%d]->piGenes[%d].\n",
        pcFName, i, iNumMax);
      exit(-2); }
    pBin->iNumEl = 0;
    pBin->iMaxEl = iNumMax;
    pBin++;
  }

  iSliceSize = pDim->iSliceSize;
  iXdim = pDim->iXdim;
  pGene = pInData-1;
  for(i = 0; i < iNumGeneTot; i++) {
    pGene++;

    fX = pGene->Sample.fPosX;
    iX = (int) floor((double) fX);
    fY = pGene->Sample.fPosY;
    iY = (int) floor((double) fY);
    fZ = pGene->Sample.fPosZ;
    iZ = (int) floor((double) fZ);

    iBin = iZ * iSliceSize + iY * iXdim + iX;
    if(iBin > iNumBins) {
      vcl_fprintf(stderr, "ERROR(%s):\n\tOverflow: BinB number %d > max = %d\n",
        pcFName, iBin, iNumBins);
      exit(-3);
    }

    pBin = pTmpBins+iBin;
    iNum = pBin->iNumEl;
    iMax = pBin->iMaxEl;
    piBinGene = pBin->piGenes+iNum;
    *piBinGene = i; /* Gene label */
    pBin->iNumEl++;
    iNum++;
    if(iNum >= iMax) {
      iMax += iMax;
      if((pBin->piGenes =
    (int *) realloc((int *) pBin->piGenes,
        iMax * sizeof(int))) == NULL) {
  vcl_fprintf(stderr,
    "ERROR(%s):\n\tREALLOC fails on pBin[%d]->piGenes[%d].\n",
    pcFName, iBin, iMax);
  exit(-2); }
      pBin->iMaxEl = iMax;
    }
  }

  /* Transfer data from TmpBins */

  iMin = iNumGeneTot;
  iBinEmpty = iMax = 0;
  pBin = pBins-1;
  pBinOld = pTmpBins-1;
  k = 0;
  for(i = 0; i < iNumBins; i++) {
    pBinOld++;
    pBin++;
    iNum = pBinOld->iNumEl;
    if(iNum < iMin)
      iMin = iNum;
    if(iNum > iMax)
      iMax = iNum;
    if(iNum < 1)
      iBinEmpty++;

    pBin->piGenes = NULL;
    if((pBin->piGenes = (int *) calloc(iNum, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
        "ERROR(%s):\n\tCALLOC failed for pBin[%d]->piGenes[%d].\n",
        pcFName, i, iNum);
      exit(-2); }
    pBin->iNumEl = iNum;
    pBin->iMaxEl = iNum;

    piBinGeneOld = pBinOld->piGenes;
    piBinGene = pBin->piGenes;
    for(j = 0; j < iNum; j++) {
      *piBinGene++ = *piBinGeneOld++;
    }
    k += iNum;
  }

  vcl_fprintf(stderr, "\t(Min , Max) genes per bin = (%d , %d)\n",
    iMin, iMax);
  vcl_fprintf(stderr, "\tNumber of empty bins = %d\n", iBinEmpty);
  pDim->iNumBinEmpty = iBinEmpty;

  if(k != iNumGeneTot) {
    vcl_fprintf(stderr,
      "ERROR(%s): Transferred %d genes instead of %d from TmpBins.\n",
      pcFName, k, iNumGeneTot);
    exit(-3);
  }

  /* Clean-up */
  pBinOld = pTmpBins;
  for(i = 0; i < iNumBins; i++) {
    free(pBinOld->piGenes);
    pBinOld++;
  }
  free(pTmpBins);

  return;
}


/* ---- EoF ---- */


// ########################### SORT3DPTS_IO.C ################################

/*------------------------------------------------------------*\
 * File: sort3dpts_io.c
 *
 * Functions:
 *  SetInFile
 * IVToG3DPoints
 *  SaveDataBucks
 *  SaveTimings
 *  SaveDataMetaBuck
 *  SaveG3dMetaBuck
 *  SaveTimingsMetaBuck
 *  SaveAllDataMetaBuck
 *  SaveG3dAll
 *
 * History:
 *  #0: July 2001, by F.Leymarie (from previous version)
 *  #1: April 2003: slight update
 *  #2: July 2003: Added SaveDataMetaBuck, SaveG3dMetaBuck,
 *    SaveTimingsMetaBuck, SaveAllDataMetaBuck
 * #1: Nov 24, 2003, by MingChing: add IVToG3DPoints to convert *.iv to *.g3d
 *
\*------------------------------------------------------------*/

///#include "sort3dpts_io.h"
///#include "string.h"
///#include "assert.h"

/*------------------------------------------------------------*\
 * Function: SetInFile
 *
 * Usage: Input data from UPenn file & sets DimensionB struct
 *
 * Note: We shift the origin of the dataset by
 *   (Off-MinX,Off-MinY,Off-MinZ), so that the origin of
 *   the system is always near (Off, Off, Off).
 *   Then we assume a "sausage" around the Data of width Off
 *   We also initialize some space for back pointers to Shocks
 *
 * Data: p3dPts: coordinates of 3 points
 *   p3dRGB: RGB color of points
 *   pDim: struct with all relevant dimension parameters
\*------------------------------------------------------------*/

int SetInFile (const char *pcUPennFile, InputDataB *pInData, DimensionB *pDim,
          int iFlagDuplic)
{
  const char    *pcFName = "SetInFile";
  FILE    *fp1;
  int  i, j;
  int    iNum3dPts = 0, iID = 0, iOffset = 0, iFlagNew;
  int    iXdim,iYdim,iZdim, iNumSamples, iNumDuplic;
  float    fPosX,fPosY,fPosZ, fColR,fColG,fColB, fMvX,fMvY,fMvZ;
  float    fMaxX,fMaxY,fMaxZ, fMinX,fMinY,fMinZ, fOff;
  float    fOldPosX,fOldPosY,fOldPosZ, fX,fY,fZ, fDistSq;
  static float  fEpsilon = (float) D_EPSILON;
  Pt3dCoord  *pGene;
  InputDataB     *pData;
  
  if ((fp1 = fopen(pcUPennFile, "r")) == NULL) {
    vul_printf (vcl_cout, "ERROR(%s): Can't open input UPenn file %s\n",
          pcFName, pcUPennFile);
    return(FALSE); 
  }
  
  fscanf(fp1, "%d", &iID);
  fscanf(fp1, "%d", &iNum3dPts);
  vul_printf (vcl_cout, "MESG(%s): There are %d 3D points (input).\n",
        pcFName, iNum3dPts);
  
  /* Read-in 1st point coordinates */
  if (fscanf (fp1, "%f %f %f %f %f %f",
          &fPosX, &fPosY, &fPosZ, &fColR, &fColG, &fColB)) {
    fMinX = fMaxX = fPosX;
    fMinY = fMaxY = fPosY;
    fMinZ = fMaxZ = fPosZ;
    pGene = &(pInData->Sample);
    pGene->fPosX = fPosX;
    pGene->fPosY = fPosY;
    pGene->fPosZ = fPosZ;
  }
  else {
    vcl_fprintf(stderr, "ERROR(%s): Data input on line no.0\n", pcFName);
    exit(-2); 
  }
  
  /* --- Read-in remaining points --- */
  
  iNumSamples = 1;
  if (iFlagDuplic) {
    iNumDuplic = 0;
    for (i = 1; i < iNum3dPts; i++) {
      if (fscanf (fp1, "%lf %lf %lf %lf %lf %lf",
              &fPosX, &fPosY, &fPosZ, &fColR, &fColG, &fColB)) {
  
        /* Compare with unique points already read to avoid duplicates */
        iFlagNew = TRUE;
        for (j = 0; j < iNumSamples; j++) {
          pGene = &((pInData+j)->Sample);
          fOldPosX = pGene->fPosX;
          fOldPosY = pGene->fPosY;
          fOldPosZ = pGene->fPosZ;
      
          fX = fPosX - fOldPosX;
          fY = fPosY - fOldPosY;
          fZ = fPosZ - fOldPosZ;
      
          fDistSq = fX*fX + fY*fY + fZ*fZ;
          if (fDistSq < fEpsilon) { /* Duplicate */
            iFlagNew = FALSE;
            iNumDuplic++;
            break;
          }
        }
        if(!iFlagNew) continue; /* Skip this duplicate */
      
        if(fPosX > fMaxX) fMaxX = fPosX;
        if(fPosX < fMinX) fMinX = fPosX;
        if(fPosY > fMaxY) fMaxY = fPosY;
        if(fPosY < fMinY) fMinY = fPosY;
        if(fPosZ > fMaxZ) fMaxZ = fPosZ;
        if(fPosZ < fMinZ) fMinZ = fPosZ;
      
        pGene = &((pInData+iNumSamples)->Sample);
        pGene->fPosX = fPosX;
        pGene->fPosY = fPosY;
        pGene->fPosZ = fPosZ;
        iNumSamples++;
      }
      else {
        vul_printf (vcl_cout, "ERROR(%s): Data input on line no.%d\n",
              pcFName, i); 
      }
    }//end for i
  } /* End of case: Check for Duplicates */
  else {
    for (i = 1; i < iNum3dPts; i++) {
      if (fscanf (fp1, "%f %f %f %f %f %f",
              &fPosX, &fPosY, &fPosZ, &fColR, &fColG, &fColB)) {
        if(fPosX > fMaxX) fMaxX = fPosX;
        if(fPosX < fMinX) fMinX = fPosX;
        if(fPosY > fMaxY) fMaxY = fPosY;
        if(fPosY < fMinY) fMinY = fPosY;
        if(fPosZ > fMaxZ) fMaxZ = fPosZ;
        if(fPosZ < fMinZ) fMinZ = fPosZ;
      
        pGene = &((pInData+iNumSamples)->Sample);
        pGene->fPosX = fPosX;
        pGene->fPosY = fPosY;
        pGene->fPosZ = fPosZ;
        iNumSamples++;
      }
      else {
        vul_printf (vcl_cout, "ERROR(%s): Data input on line no.%d\n",
              pcFName, i); 
      }
    } /* i++ */
  }
  fclose(fp1);
  
  if (iFlagDuplic) { //report duplicates
    vul_printf (vcl_cout, "\tFound %d unique input samples; %d duplicated.\n",
          iNumSamples, iNumDuplic);
    iNum3dPts = iNumSamples;
  }
  else {
    vcl_fprintf(stderr, "\tLoaded %d input samples.\n", i);
  }
  
  /* We shift the origin of the dataset by (Off-MinX,Off-MinY,Off-MinZ) */
  /*    So that the origin of the system is always near (Off, Off, Off) */
  /*    Then we assume a "sausage" around the Data of width Off    */
  /* We also initialize some space for back pointers to Shocks */
  
  fOff = pDim->fOff;
  fMvX = pDim->fMoveX = fOff - floor(fMinX);
  fMvY = pDim->fMoveY = fOff - floor(fMinY);
  fMvZ = pDim->fMoveZ = fOff - floor(fMinZ);
  
  pDim->fMinX = fMinX + fMvX;  pDim->fMaxX = fMaxX + fMvX;
  pDim->fMinY = fMinY + fMvY;  pDim->fMaxY = fMaxY + fMvY;
  pDim->fMinZ = fMinZ + fMvZ;  pDim->fMaxZ = fMaxZ + fMvZ;
  
  pData = pInData;
  for (i = 0; i < iNum3dPts; i++) {
    pGene  = &(pData->Sample);
    fPosX = pGene->fPosX;
    fPosY = pGene->fPosY;
    fPosZ = pGene->fPosZ;
    pGene->fPosX = fPosX + fMvX;
    pGene->fPosY = fPosY + fMvY;
    pGene->fPosZ = fPosZ + fMvZ;
    pData++;
  }
  
  iOffset = (int) ceil(fOff);
  pDim->iXdim = iXdim = 2*iOffset + (int) ceil(pDim->fMaxX - pDim->fMinX);
  pDim->iYdim = iYdim = 2*iOffset + (int) ceil(pDim->fMaxY - pDim->fMinY);
  pDim->iZdim = iZdim = 2*iOffset + (int) ceil(pDim->fMaxZ - pDim->fMinZ);
  pDim->iSliceSize = iYdim*iXdim;
  pDim->iWSpaceSize = iZdim*iYdim*iXdim;
  pDim->iNum3dPts = iNum3dPts;
  
  /* -- Theoretical maximum number of Initial Blue (sheet) Shocks -- */
  pDim->iMaxBlueShocks = iNum3dPts * (iNum3dPts - 1);
  pDim->iMaxBlueShocks >>= 1; /* Division by 2 */
  
  vcl_fprintf(stderr, "\tDimensions: X = %d, Y = %d, Z = %d, Sausage = %d\n",
      iXdim, iYdim, iZdim, iOffset);
  vcl_fprintf(stderr, "\tMove = (%lf , %lf , %lf)\n", pDim->fMoveX, pDim->fMoveY,
      pDim->fMoveZ);
  vcl_fprintf(stderr, "\tMin = (%lf , %lf , %lf)\n", pDim->fMinX, pDim->fMinY,
      pDim->fMinZ);
  vcl_fprintf(stderr, "\tMax = (%lf , %lf , %lf)\n", pDim->fMaxX, pDim->fMaxY,
      pDim->fMaxZ);
  vcl_fprintf(stderr, "\tMax (theoretical) number of initial blue shocks = %d\n",
      pDim->iMaxBlueShocks);
  
  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: SaveDataBucks
 * File:     .Bucks.Data.txt
 * Usage:    Save Bucket0 Data in an ASCII format.
 *
\*------------------------------------------------------------*/

void SaveDataBucks (char *pcInFile, char *pcOutFile, InputDataB *pInData,
              Buckets0 *pBucketsX, Buckets0 *pBucketsXY, Buckets0 *pBucketsXYZ,
              DimensionB *pDim)
{
  const char    *pcFName = "SaveDataBucks";
  register int  i, j;
  int    iNum, iNumGene, iTmp;
  int    *pGene;
  float    fTmp;
  FILE    *f1;
  Pt3dCoord  *p3dPts;
  Bucket0  *pBuck;
  BinB    *pBin;
  
  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if (f1 == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); 
  }
  
  vcl_fprintf(stderr, "MESG(%s):\n\tSaving data in %s.\n", pcFName, pcOutFile);
  
  /* -- First: Save Initial Source Data -- */
  
  vcl_fprintf(f1, "Input filename: %s\n", pcInFile);
  
  iNum = pDim->iNum3dPts;
  vcl_fprintf (f1, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n",
        pDim->iXdim, pDim->iYdim, pDim->iZdim);
  vcl_fprintf (f1, "(Xmove, Ymove, Zmove) = (%lf , %lf , %lf).\n",
        pDim->fMoveX, pDim->fMoveY, pDim->fMoveZ);
  vcl_fprintf (f1, "Offset = %lf.\n", pDim->fOffset);
  vcl_fprintf (f1, "Number of input 3D pts (sources): %d\n", iNum);
  vcl_fprintf (f1, "ID: Coords (x,y,z)\n");
  vcl_fprintf (f1, "-----------\n");
  
  for (i = 0; i < iNum; i++) {
    p3dPts = &((pInData+i)->Sample);
    vcl_fprintf (f1, "%d: %lf , %lf , %lf\n", i, p3dPts->fPosX,
          p3dPts->fPosY, p3dPts->fPosZ);
  }
  
  /* -- Second: Save BucketsXYZ -- */
  
  iNum = pBucketsXYZ->iNumBucks;
  vcl_fprintf (f1, "-----------\n");
  vcl_fprintf (f1,
        "BUCKETS XYZ: Total = %d , AvgGene (Target) = %d , Tolerance = %lf\n",
        iNum, pBucketsXYZ->iAvgGene, pBucketsXYZ->fTolerance);
  vcl_fprintf (f1, " Empty buckets = %d of which %d are virtual.\n",
        pBucketsXYZ->iEmpty, pBucketsXYZ->iVirtual);
  vcl_fprintf (f1,
        "ID: (1stBin NumBins) (MinX MaxX : MinY MaxY : MinZ MaxZ) NumGene\n");
  vcl_fprintf (f1, "-----------\n");
  
  pBuck = pBucketsXYZ->pBucket;
  for (i = 0; i < iNum; i++) {
    vcl_fprintf (f1,
          "%d: (%d %d) (%d %d : %d %d : %d %d) %d\n", i, pBuck->iBinFirst,
          pBuck->iNumBins, pBuck->Limits.iMinX, pBuck->Limits.iMaxX,
          pBuck->Limits.iMinY, pBuck->Limits.iMaxY,
          pBuck->Limits.iMinZ, pBuck->Limits.iMaxZ, pBuck->iNumEl);
    pBuck++;
  }
  
  /* -- Third: Save BinsXYZ -- */
  
  iNum = pBucketsXYZ->iNumBins;
  vcl_fprintf (f1, "-----------\n");
  vcl_fprintf (f1, "BINS XYZ: Total = %d\n", iNum);
  vcl_fprintf (f1, "ID: NumGene: List of Sources\n");
  vcl_fprintf (f1, "-----------\n");
  
  iTmp = 0;
  pBin = pBucketsXYZ->pBin;
  for (i = 0; i < iNum; i++) {
    iNumGene = pBin->iNumEl;
    vcl_fprintf (f1, "%d: %d: ", i, iNumGene);
    if (iNumGene > 0) {
      pGene = pBin->piGenes;
      for (j = 0; j < iNumGene; j++) {
        vcl_fprintf(f1, "%d ", *pGene++);
      }
    }
    else 
      iTmp++;
    vcl_fprintf(f1, "\n");
    pBin++;
  }
  
  vcl_fprintf (f1, "-----------\n");
  vcl_fprintf (f1, "Number of Empty Bins = %d\n", iTmp);
  vcl_fprintf (f1, "-----------\n");
  
  fTmp = 100.0 * iTmp / iNum;
  pBucketsXYZ->iBinEmpty = iTmp;
  vul_printf (vcl_cout, "\tNumber of Empty Bins = %d out of %d (%.2f percent).\n",
        iTmp, iNum, fTmp);
  
  fclose(f1);
  
  return;
}

/*------------------------------------------------------------*\
 * Function: SaveTimings
 * FIle:     .Bucks.Timings.txt
 * Usage:   Save Timing Data
\*------------------------------------------------------------*/

void SaveTimings (char *pcInFile, char *pcOutFile, TimingsB *pTime,
      Buckets0 *pBucketsX, Buckets0 *pBucketsXY, Buckets0 *pBucketsXYZ,
      DimensionB *pDim)
{
  const char    *pcFName = "SaveTimings";
  FILE    *f1;
  float    fTmpSys,fTmpUser,fTmpMem;
  
  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if (f1 == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); }
  
  vcl_fprintf(stderr, "MESG(%s): Saving data in\n\t%s.\n", pcFName, pcOutFile);
  
  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "Input filename: %s\n", pcInFile);
  vcl_fprintf(f1, "Number of input 3D pts (genes) = %d\n", pDim->iNum3dPts);
  vcl_fprintf(f1, "BUCKETS XYZ: Total = %d , AvgGene = %d , Tolerance = %lf\n",
      pBucketsXYZ->iNumBucks, pBucketsXYZ->iAvgGene,
      pBucketsXYZ->fTolerance);
  vcl_fprintf(f1, "\t(Max , Min) genes per bucket = (%d , %d).\n",
      pBucketsXYZ->iMaxGene, pBucketsXYZ->iMinGene);
  vcl_fprintf(f1, "\tEmpty buckets = %d of which %d are virtual.\n",
      pBucketsXYZ->iEmpty, pBucketsXYZ->iVirtual);
  vcl_fprintf(f1, "BINS XYZ: Total = %d , Empty = %d \n",
      pBucketsXYZ->iNumBins, pBucketsXYZ->iBinEmpty);
  vcl_fprintf(f1, "BUCKETS XY: Total = %d , AvgGene = %d\n",
      pBucketsXY->iNumBucks, pBucketsXY->iAvgGene);
  vcl_fprintf(f1, "\t(Max , Min) genes per bucket = (%d , %d).\n",
      pBucketsXY->iMaxGene, pBucketsXY->iMinGene);
  vcl_fprintf(f1, "BUCKETS X: Total = %d , AvgGene = %d\n",
      pBucketsX->iNumBucks, pBucketsX->iAvgGene);
  vcl_fprintf(f1, "\t(Max , Min) genes per bucket = (%d , %d).\n",
      pBucketsX->iMaxGene, pBucketsX->iMinGene);
  vcl_fprintf(f1, "-----------\n");
  
  vcl_fprintf(stderr, " Timings in seconds:\n");
  vcl_fprintf(f1, "Timings in seconds:\n");
  
  fTmpSys  = pTime->fSysEnd;
  fTmpUser = pTime->fUserEnd;
  fTmpMem  = pTime->fMemEnd;
  vcl_fprintf(stderr, " MAIN:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "MAIN:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  
  fTmpSys = pTime->fSysQHull;
  fTmpUser = pTime->fUserQHull;
  fTmpMem = pTime->fMemQHull;
  vcl_fprintf(stderr, " QHull:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "QHull:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  
  fTmpSys = pTime->fSysFillXYZ;
  fTmpUser = pTime->fUserFillXYZ;
  fTmpMem = pTime->fMemFillXYZ;
  vcl_fprintf(stderr, " FillXYZ: Total = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "FillXYZ: Total = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  
  fTmpSys = pTime->fSysFillXY;
  fTmpUser = pTime->fUserFillXY;
  fTmpMem = pTime->fMemFillXY;
  vcl_fprintf(stderr, " FillXY: Total = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "FillXY: Total = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  
  fTmpSys = pTime->fSysFillX;
  fTmpUser = pTime->fUserFillX;
  fTmpMem = pTime->fMemFillX;
  vcl_fprintf(stderr, " FillX:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "FillX:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  
  fTmpSys  = pTime->fSysInit;
  fTmpUser = pTime->fUserInit;
  fTmpMem  = pTime->fMemInit;
  vcl_fprintf(stderr, " INIT:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "INIT:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
      fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  
  vcl_fprintf(f1, "-----------\n");
  
  fclose(f1);
  
  return;
}

/*------------------------------------------------------------*\
 * Function: SaveDataMetaBuck
 *
 * Usage:  Save Bucket0 Data in an ASCII format.
 *
\*------------------------------------------------------------*/

void SaveDataMetaBuck (const char *pcInFile, char *pcOutFile, 
                InputDataB *pInData,
                Buckets0 *pBucketsX, Buckets0 *pBucketsXY,
                Buckets0 *pBucketsXYZ, DimensionB *pDim)
{
  const char    *pcFName = "SaveDataMetaBuck";
  register int  i, j;
  int    iNumGenes, iTmp, iNumBinXYZ, iNumTotGene, iNumGeneInBin;
  int    iXdim,iYdim,iZdim, iXmin,iXmax, iYmin,iYmax, iZmin,iZmax;
  int    iGene, iNumBuckXYZ;
  int    *piGene;
  float    fTmp;
  FILE    *f1;
  Pt3dCoord  *p3dPts;
  Bucket0  *pBuck;
  BinB    *pBinXYZ;
  
  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if(f1 == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); }
  
  vcl_fprintf(stderr, "MESG(%s):\n\tSaving data in %s.\n", pcFName, pcOutFile);
  
  /* -- First: Save Initial Source Data -- */
  
  vcl_fprintf(f1, "Input filename: %s\n", pcInFile);
  vcl_fprintf(f1, "Data for MetaBucket no. %d (out of %d).\n",
      pDim->iMetaBuckId, pDim->iNumMetaBuckets);
  
  iNumGenes = pBucketsXYZ->iTotGene;
  iXmin = pBucketsXYZ->Limits.iMinX;
  iXmax = pBucketsXYZ->Limits.iMaxX;
  iXdim = iXmax - iXmin;
  iYmin = pBucketsXYZ->Limits.iMinY;
  iYmax = pBucketsXYZ->Limits.iMaxY;
  iYdim = iYmax - iYmin;
  iZmin = pBucketsXYZ->Limits.iMinZ;
  iZmax = pBucketsXYZ->Limits.iMaxZ;
  iZdim = iZmax - iZmin;
  
  vcl_fprintf(f1, "Min grid coords: X = %d , Y = %d , Z = %d\n",
      iXmin, iYmin, iZmin);
  vcl_fprintf(f1, "Max grid coords: X = %d , Y = %d , Z = %d\n",
      iXmax, iYmax, iZmax);
  vcl_fprintf(f1, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n",
      iXdim, iYdim, iZdim);
  vcl_fprintf(f1, "(Xmove, Ymove, Zmove) = (%lf , %lf , %lf).\n",
      pDim->fMoveX, pDim->fMoveY, pDim->fMoveZ);
  vcl_fprintf(f1, "Offset = %lf.\n", pDim->fOffset);
  vcl_fprintf(f1, "Number of generators: %d (out of %d)\n", iNumGenes,
      pDim->iNum3dPts);
  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "Original ID : Coords (x,y,z) : BinXYZ label\n");
  vcl_fprintf(f1, "-----------\n");
  
  pBinXYZ = pBucketsXYZ->pBin;
  iNumBinXYZ = pBucketsXYZ->iNumBins;
  iNumTotGene = 0;
  for(i = 0; i < iNumBinXYZ; i++) {
    iNumGeneInBin = pBinXYZ->iNumEl;
    piGene = pBinXYZ->piGenes;
    for(j = 0; j < iNumGeneInBin; j++) {
      iGene = *piGene++;
      p3dPts = &((pInData+iGene)->Sample);
      vcl_fprintf(f1, "%d : (%lf , %lf , %lf) : %d\n", iGene,
          p3dPts->fPosX, p3dPts->fPosY, p3dPts->fPosZ, i);
    }
    iNumTotGene += iNumGeneInBin;
    pBinXYZ++;
  } /* Next BinXYZ: i++ */
  if(iNumTotGene != iNumGenes) {
    vcl_fprintf(stderr, "ERROR(%s): Expected to save %d genes, not %d .\n",
        pcFName, iNumGenes, iNumTotGene);
    exit(-4);
  }
  
  /* -- Second: Save BucketsXYZ -- */
  
  iNumBuckXYZ = pBucketsXYZ->iNumBucks;
  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "BUCKETS XYZ: Total = %d , AvgGene (Target) = %d , Tolerance = %lf\n",
      iNumBuckXYZ, pBucketsXYZ->iAvgGene, pBucketsXYZ->fTolerance);
  vcl_fprintf(f1, " Empty buckets = %d of which %d are virtual.\n",
      pBucketsXYZ->iEmpty, pBucketsXYZ->iVirtual);
  vcl_fprintf(f1, "ID: (1stBin NumBins) (MinX MaxX : MinY MaxY : MinZ MaxZ) : NumGene\n");
  vcl_fprintf(f1, "-----------\n");
  
  pBuck = pBucketsXYZ->pBucket;
  for(i = 0; i < iNumBuckXYZ; i++) {
    vcl_fprintf(f1,
        "%d: (%d %d) (%d %d : %d %d : %d %d) :\t%d\n", i, pBuck->iBinFirst,
        pBuck->iNumBins, pBuck->Limits.iMinX, pBuck->Limits.iMaxX,
        pBuck->Limits.iMinY, pBuck->Limits.iMaxY,
        pBuck->Limits.iMinZ, pBuck->Limits.iMaxZ, pBuck->iNumEl);
    pBuck++;
  }
  
  /* -- Third: Save BinsXYZ -- */
  
  iNumBinXYZ = pBucketsXYZ->iNumBins;
  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "BINS XYZ: Total = %d\n", iNumBinXYZ);
  vcl_fprintf(f1, "ID: Number of Genes\n");
  vcl_fprintf(f1, "-----------\n");
  
  iTmp = 0;
  pBinXYZ = pBucketsXYZ->pBin;
  for(i = 0; i < iNumBinXYZ; i++) {
    iNumGeneInBin = pBinXYZ->iNumEl;
    vcl_fprintf(f1, "%d: %d\n", i, iNumGeneInBin);
    if(iNumGeneInBin < 1)
      iTmp++;
    pBinXYZ++;
  }
  
  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "Number of Empty Bins = %d\n", iTmp);
  vcl_fprintf(f1, "-----------\n");
  
  fTmp = 100.0 * (float) iTmp / (float) iNumBinXYZ;
  pBucketsXYZ->iBinEmpty = iTmp;
  vcl_fprintf(stderr, "\tNumber of Empty Bins = %d out of %d (%.2f percent).\n",
      iTmp, iNumBinXYZ, fTmp);
  
  fclose(f1);
  
  return;
}

/*------------------------------------------------------------*\
 * Function: SaveG3dMetaBuck
 *
 * Usage:  Save Bucket0 Data in an ASCII format for display purpose.
 *
\*------------------------------------------------------------*/

void
SaveG3dMetaBuck(char *pcOutFile, InputDataB *pInData,
    Buckets0 *pBucketsXYZ, DimensionB *pDim)
{
  const char    *pcFName = "SaveG3dMetaBuck";
  register int  i, j;
  int    iNumGenes, iNumBinXYZ, iNumTotGene, iNumGeneInBin;
  int    iGene;
  int    *piGene;
  FILE    *f1;
  Pt3dCoord  *p3dPts;
  BinB    *pBinXYZ;
  
  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if(f1 == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); }
  
  vcl_fprintf(stderr, "MESG(%s):\n\tSaving G3D data in %s.\n", pcFName, pcOutFile);
  
  /* -- First: Save Initial Source Data -- */
  
  vcl_fprintf(f1, "3\n");
  iNumGenes = pBucketsXYZ->iTotGene;
  vcl_fprintf(f1, "%d\n", iNumGenes);
  
  pBinXYZ = pBucketsXYZ->pBin;
  iNumBinXYZ = pBucketsXYZ->iNumBins;
  iNumTotGene = 0;
  for(i = 0; i < iNumBinXYZ; i++) {
    iNumGeneInBin = pBinXYZ->iNumEl;
    piGene = pBinXYZ->piGenes;
    for(j = 0; j < iNumGeneInBin; j++) {
      iGene = *piGene++;
      p3dPts = &((pInData+iGene)->Sample);
      vcl_fprintf(f1, "%lf %lf %lf 1.0 1.0 1.0\n",
          p3dPts->fPosX, p3dPts->fPosY, p3dPts->fPosZ);
    }
    iNumTotGene += iNumGeneInBin;
    pBinXYZ++;
  } /* Next BinXYZ: i++ */
  
  fclose(f1);
  
  if(iNumTotGene != iNumGenes) {
    vcl_fprintf(stderr, "ERROR(%s): Expected to save %d genes, not %d .\n",
        pcFName, iNumGenes, iNumTotGene);
    exit(-4);
  }
  
  return;
}

/*------------------------------------------------------------*\
 * Function: SaveTimingsMetaBuck
 *
 * Usage:  Save Timing Data and Global information for Meta Buckets0
\*------------------------------------------------------------*/

void
SaveTimingsMetaBuck(const char *pcInFile, char *pcOutFile, TimingsB *pTime,
        Buckets0 *pMetaBucksXYZ, MetaBuckInfoB *pMBuckInfo,
        DimensionB *pDim)
{
  const char    *pcFName = "SaveTimingsMetaBuck";
  FILE    *f1;
  register int  i;
  int    iNumMBucks, iNumGeneTot, iNumGeneAcc, iNumGenes;
  float    fTmpSys,fTmpUser,fTmpMem;
  Bucket0  *pMBuck;

  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if(f1 == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); }
  
  vcl_fprintf(stderr, "MESG(%s): Saving data in\n\t%s.\n", pcFName, pcOutFile);

  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "Input filename: %s\n", pcInFile);
  iNumGeneTot = pDim->iNum3dPts;
  vcl_fprintf(f1, "Total Number of input generators = %d\n", iNumGeneTot);
  vcl_fprintf(f1, "\t(Xmove, Ymove, Zmove) = (%lf , %lf , %lf).\n",
    pDim->fMoveX, pDim->fMoveY, pDim->fMoveZ);
  vcl_fprintf(f1, "\tOffset = %.2f\n", pDim->fOffset);

  iNumMBucks = pMetaBucksXYZ->iNumBucks;
  vcl_fprintf(f1, "MetaBuckets XYZ: Total = %d , AvgGene = %d , Tolerance = %.2f\n",
    iNumMBucks, pMetaBucksXYZ->iAvgGene, pMetaBucksXYZ->fTolerance);
  vcl_fprintf(f1, "\t(Min , Max) genes per MetaBucket = (%d , %d).\n",
    pMetaBucksXYZ->iMinGene, pMetaBucksXYZ->iMaxGene);
  vcl_fprintf(f1, "\tEmpty MetaBuckets = %d of which %d are virtual.\n",
    pMetaBucksXYZ->iEmpty, pMetaBucksXYZ->iVirtual);

  vcl_fprintf(f1, "\tBins XYZ: Total = %d , Empty = %d \n",
    pMetaBucksXYZ->iNumBins, pMetaBucksXYZ->iBinEmpty);
  vcl_fprintf(f1, "\tCoord ranges: %d < X < %d , %d < Y < %d , %d < Z < %d\n",
    pMetaBucksXYZ->Limits.iMinX, pMetaBucksXYZ->Limits.iMaxX,
    pMetaBucksXYZ->Limits.iMinY, pMetaBucksXYZ->Limits.iMaxY,
    pMetaBucksXYZ->Limits.iMinZ, pMetaBucksXYZ->Limits.iMaxZ);

  vcl_fprintf(f1, "-----------\n");

  vcl_fprintf(f1, "MetaBucket no. : Bucks (Empty, Bins) : Genes (Avg, Min, Max)\n");
  vcl_fprintf(f1, "\tRanges (Xmin < X < Xmax) : (Ymin < Y < Ymax) : (Zmin < Z < Zmax)\n");
  vcl_fprintf(f1, "-----------\n");

  pMBuck = pMetaBucksXYZ->pBucket;
  iNumGeneAcc = 0;
  for(i = 0; i < iNumMBucks; i++) {

    iNumGenes = *(pMBuckInfo->piNumGenes+i);
    vcl_fprintf(f1, "%d : %d (%d , %d) : %d (%d , %d , %d)\n",
      i, *(pMBuckInfo->piNumBuckets+i),
      *(pMBuckInfo->piNumBuckEmpty+i),
      *(pMBuckInfo->piNumBins+i),
      iNumGenes,
      *(pMBuckInfo->piAvgGene+i),
      *(pMBuckInfo->piMinGene+i),
      *(pMBuckInfo->piMaxGene+i));
    iNumGeneAcc += iNumGenes;

    vcl_fprintf(f1, "\t%d < X < %d , %d < Y < %d , %d < Z < %d\n",
      pMBuck->Limits.iMinX, pMBuck->Limits.iMaxX,
      pMBuck->Limits.iMinY, pMBuck->Limits.iMaxY,
      pMBuck->Limits.iMinZ, pMBuck->Limits.iMaxZ);
    pMBuck++;
  }
  vcl_fprintf(f1, "-----------\n");

  vcl_fprintf(stderr, " Timings in seconds, memory in Megs:\n");
  vcl_fprintf(f1, "Timings in seconds, memory in Megs:\n");

  fTmpSys  = pTime->fSysEnd;
  fTmpUser = pTime->fUserEnd;
  fTmpMem  = pTime->fMemEnd;
  vcl_fprintf(stderr, " MAIN: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "MAIN: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);

  fTmpSys -= (pTime->fSysFillXYZ + pTime->fSysFillXY + pTime->fSysFillX);
  fTmpUser -= (pTime->fUserFillXYZ + pTime->fUserFillXY + pTime->fUserFillX);
  vcl_fprintf(stderr,
    " FillBuckets: Total = %lf (Sys = %lf , User = %lf)\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser);
  vcl_fprintf(f1, "FillBuckets: Total = %lf (Sys = %lf , User = %lf)\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser);

  fTmpSys = pTime->fSysFillXYZ;
  fTmpUser = pTime->fUserFillXYZ;
  fTmpMem = pTime->fMemFillXYZ;
  vcl_fprintf(stderr,
    " FillMetaXYZ: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "FillMetaXYZ: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);

  fTmpSys = pTime->fSysFillXY;
  fTmpUser = pTime->fUserFillXY;
  fTmpMem = pTime->fMemFillXY;
  vcl_fprintf(stderr,
    " FillMetaXY: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "FillMetaXY: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);

  fTmpSys = pTime->fSysFillX;
  fTmpUser = pTime->fUserFillX;
  fTmpMem = pTime->fMemFillX;
  vcl_fprintf(stderr,
    " FillMetaX: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "FillMetaX: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);

  fTmpSys  = pTime->fSysInit;
  fTmpUser = pTime->fUserInit;
  fTmpMem  = pTime->fMemInit;
  vcl_fprintf(stderr, " INIT: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "INIT: Total = %lf (Sys = %lf , User = %lf), Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);

#if FALSE
  fTmpSys = pTime->fSysQHull;
  fTmpUser = pTime->fUserQHull;
  fTmpMem = pTime->fMemQHull;
  vcl_fprintf(stderr, " QHull:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
  vcl_fprintf(f1, "QHull:\tTotal = %lf (Sys = %lf , User = %lf) , Mem = %.2f\n",
    fTmpSys+fTmpUser, fTmpSys, fTmpUser, fTmpMem);
#endif

  vcl_fprintf(f1, "-----------\n");

  fclose(f1);

  if(iNumGeneAcc != iNumGeneTot) {
    vcl_fprintf(stderr, "ERROR(%s): The number of genes accumulated\n", pcFName);
    vcl_fprintf(stderr, "\tfor the %d MetaBuckets = %d\n", iNumMBucks,
      iNumGeneAcc);
    vcl_fprintf(stderr, "\trather than the initial %d\n", iNumGeneTot);
    exit(-5);
  }

  return;
}

/*------------------------------------------------------------*\
 * Function: SaveAllDataMetaBuck
 *
 * Usage:  Save Voxel Data in an ASCII format.
 *
\*------------------------------------------------------------*/

void
SaveAllDataMetaBuck(const char *pcInFile, char *pcOutFile, InputDataB *pInData,
        BinB *pBins, DimensionB *pDim)
{
  const char    *pcFName = "SaveAllDataMetaBuck";
  register int  i, j;
  int    iNumGenes, iNumBinXYZ, iNumTotGene, iNumGeneInBin;
  int    iGene, iBinEmpty, iMin, iMax;
  int    *piGene;
  float    fTmp;
  FILE    *f1;
  Pt3dCoord  *p3dPts;
  BinB    *pBinXYZ;

  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if(f1 == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); }
  
  vcl_fprintf(stderr, "MESG(%s):\n\tSaving data in %s.\n", pcFName, pcOutFile);

  /* -- First: Save Initial Source Data -- */

  vcl_fprintf(f1, "Input filename: %s\n", pcInFile);
  iNumGenes  = pDim->iNum3dPts;
  iNumBinXYZ = pDim->iWSpaceSize;
  vcl_fprintf(f1, "Voxel Data for (all) %d generators and %d voxels (bins),\n",
    iNumGenes, iNumBinXYZ);
  vcl_fprintf(f1, "\tof which %d are empty.\n", pDim->iNumBinEmpty);
  vcl_fprintf(f1, "Min grid coords: X = %lf , Y = %lf , Z = %lf\n",
    pDim->fMinX, pDim->fMinY, pDim->fMinZ);
  vcl_fprintf(f1, "Max grid coords: X = %lf , Y = %lf , Z = %lf\n",
    pDim->fMaxX, pDim->fMaxY, pDim->fMaxZ);
  vcl_fprintf(f1, "(Xdim, Ydim, Zdim) = (%d , %d , %d)\n",
    pDim->iXdim, pDim->iYdim, pDim->iZdim);
  vcl_fprintf(f1, "(Xmove, Ymove, Zmove) = (%lf , %lf , %lf).\n",
    pDim->fMoveX, pDim->fMoveY, pDim->fMoveZ);
  vcl_fprintf(f1, "Offset = %lf.\n", pDim->fOffset);
  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "Voxel ID : Number of Genes > 0\n");
  vcl_fprintf(f1, "-----------\n");

  pBinXYZ = pBins-1;
  iBinEmpty = 0;
  iMax = 0;
  iMin = iNumGenes;
  for(i = 0; i < iNumBinXYZ; i++) {
    pBinXYZ++;
    iNumGeneInBin = pBinXYZ->iNumEl;
    if(iNumGeneInBin < 1) {
      if(iMin > iNumGeneInBin)
  iMin = iNumGeneInBin;
      iBinEmpty++;
      continue;
    }
    if(iMax < iNumGeneInBin)
      iMax = iNumGeneInBin;
    vcl_fprintf(f1, "%d : %d\n", i, iNumGeneInBin);
  }
  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "Number of empty voxels = %d\n", iBinEmpty);
  vcl_fprintf(f1, "(Min , Max) number of genes per voxel = (%d , %d)\n",
    iMin, iMax);
  vcl_fprintf(f1, "-----------\n");
  vcl_fprintf(f1, "Original Gene ID : Coords (x,y,z) : Bin label\n");
  vcl_fprintf(f1, "-----------\n");

  pBinXYZ = pBins-1;
  iNumTotGene = 0;
  for(i = 0; i < iNumBinXYZ; i++) {
    pBinXYZ++;
    iNumGeneInBin = pBinXYZ->iNumEl;
    if(iNumGeneInBin < 1)
      continue;
    piGene = pBinXYZ->piGenes;
    for(j = 0; j < iNumGeneInBin; j++) {
      iGene = *piGene++;
      p3dPts = &((pInData+iGene)->Sample);
      vcl_fprintf(f1, "%d : (%lf , %lf , %lf) : %d\n", iGene,
        p3dPts->fPosX, p3dPts->fPosY, p3dPts->fPosZ, i);
    }
    iNumTotGene += iNumGeneInBin;
  } /* Next BinXYZ: i++ */

  vcl_fprintf(f1, "-----------\n");

  fclose(f1);

  fTmp = 100.0 * (float) iBinEmpty / (float) iNumBinXYZ;
  vcl_fprintf(stderr,
    "\tNumber of Empty Bins = %d out of %d (%.2f percent).\n",
    iBinEmpty, iNumBinXYZ, fTmp);
  vcl_fprintf(stderr,
    "\t(Min , Max) number of genes per voxel = (%d , %d)\n",
    iMin, iMax);

  if(iNumTotGene != iNumGenes) {
    vcl_fprintf(stderr, "ERROR(%s): Expected to save %d genes, not %d .\n",
      pcFName, iNumGenes, iNumTotGene);
    exit(-4);
  }

  return;
}

/*------------------------------------------------------------*\
 * Function: SaveG3dAll
 *
 * Usage: Save All Data in an ASCII format for display purpose.
 *    Color is set as follows:
 *    Red = Bin label (normalized)
 *    Green = Gene label (normalized)
 *    Blue = 255 - Red value
 *
\*------------------------------------------------------------*/

void
SaveG3dAll(char *pcOutFile, InputDataB *pInData, BinB *pBins, DimensionB *pDim)
{
  const char    *pcFName = "SaveG3dAll";
  register int  i, j;
  int    iNumGenes, iNumBinXYZ, iNumTotGene, iNumGeneInBin;
  int    iGene;
  int    *piGene;
  float    fTmp, fTmpB, fTmpC;
  FILE    *f1;
  Pt3dCoord  *p3dPts;
  BinB    *pBinXYZ;

  f1 = NULL;
  f1 = fopen(pcOutFile, "w");
  if(f1 == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open file %s.\n", pcFName, pcOutFile);
    exit(-6); }
  
  vcl_fprintf(stderr, "MESG(%s):\n\tSaving G3D data in %s.\n", pcFName, pcOutFile);

  /* -- First: Save Initial Source Data -- */

  iNumBinXYZ = pDim->iWSpaceSize;
  iNumGenes  = pDim->iNum3dPts;
  vcl_fprintf(f1, "%d\n", iNumBinXYZ);
  vcl_fprintf(f1, "%d\n", iNumGenes);

  pBinXYZ = pBins-1;
  iNumTotGene = 0;
  for(i = 0; i < iNumBinXYZ; i++) {
    pBinXYZ++;
    iNumGeneInBin = pBinXYZ->iNumEl;
    piGene = pBinXYZ->piGenes;
    if(iNumGeneInBin < 1)
      continue;
    fTmp = (float) i / (float) iNumBinXYZ;
    fTmp *= 255;
    fTmpB = 255.0 - fTmp;
    for(j = 0; j < iNumGeneInBin; j++) {
      iGene = *piGene++;
      fTmpC = (float) iGene / (float) iNumGenes;
      fTmpC *= 255;
      p3dPts = &((pInData+iGene)->Sample);
      vcl_fprintf(f1, "%lf %lf %lf %lf %lf %lf\n",
        p3dPts->fPosX, p3dPts->fPosY, p3dPts->fPosZ,
        fTmp, fTmpC, fTmpB);
    }
    iNumTotGene += iNumGeneInBin;
  } /* Next BinXYZ: i++ */

  fclose(f1);

  if(iNumTotGene != iNumGenes) {
    vcl_fprintf(stderr, "ERROR(%s): Expected to save %d genes, not %d .\n",
      pcFName, iNumGenes, iNumTotGene);
    exit(-4);
  }

  return;
}


// ################################ SORT3DPTS_UTIL.C ##################

/*------------------------------------------------------------*\
 * File: sort3dpts_utils.c
 *
 * Functions:
 *  CleanUpBuckBin
 *  CheckBuck
 *  FillSingleMetaBucketInXYZ
 *
 * History:
 *  #0: July 2003, by F.Leymarie
 *  #1: Nov. 2003: Added FillSingleMetaBucketInXYZ
 *
\*------------------------------------------------------------*/

///#include "sort3dpts_utils.h"

/*------------------------------------------------------------*\
 * Function: CleanUpBuckBin
 *
 * Usage:  Frees space for buckets and bins.
 *
\*------------------------------------------------------------*/

void CleanUpBuckBin (Buckets0 *pBucketsX, Buckets0 *pBucketsXY,
              Buckets0 *pBucketsXYZ)
{
  const char    *pcFName = "CleanUpBuckBin";
  register int  i;
  int    iNumBinsX, iNumBinsXY, iNumBinsXYZ;
  BinB    *pBin;
  
  iNumBinsX   = pBucketsX->iNumBins;
  iNumBinsXY  = pBucketsXY->iNumBins;
  iNumBinsXYZ = pBucketsXYZ->iNumBins;
  
  pBin = pBucketsX->pBin;
  for(i = 0; i < iNumBinsX; i++) {
    free(pBin->piGenes);
    pBin++;
  }
  
  pBin = pBucketsXY->pBin;
  for(i = 0; i < iNumBinsXY; i++) {
    free(pBin->piGenes);
    pBin++;
  }
  
  pBin = pBucketsXYZ->pBin;
  for(i = 0; i < iNumBinsXYZ; i++) {
    free(pBin->piGenes);
    pBin++;
  }
  
  return;
}

/*------------------------------------------------------------*\
 * Function: CheckBuck
 *
 * Usage:  Checks that the correct number of generators were loaded.
 *
\*------------------------------------------------------------*/

int
CheckBuck(Buckets0 *pBuckets)
{
  const char    *pcFName = "CheckBuck";
  register int  i, j;
  int    iNumBins, iNumBucks, iNumGenes, iNumGeneTot;
  int    iBinFirst;
  BinB    *pBin;
  Bucket0  *pBuck;
  
  iNumBins = pBuckets->iNumBins;
  pBin = pBuckets->pBin;
  iNumGenes = 0;
  for(i = 0; i < iNumBins; i++) {
    iNumGenes += pBin->iNumEl;
    pBin++;
  }
  if(iNumGenes != pBuckets->iTotGene) {
    vcl_fprintf(stderr, "ERROR(%s): There are %d genes in %d bins,\n",
        pcFName, iNumGenes, iNumBins);
    vcl_fprintf(stderr, "\trather than an expected total of %d\n",
        pBuckets->iTotGene);
    return(FALSE);
  }
  
  iNumBucks = pBuckets->iNumBucks;
  pBuck = pBuckets->pBucket;
  iNumGeneTot = 0;
  for(i = 0; i < iNumBucks; i++) {
    iBinFirst = pBuck->iBinFirst;
    iNumBins = pBuck->iNumBins;
    pBin = pBuckets->pBin+iBinFirst;
    iNumGenes = 0;
    for(j = 0; j < iNumBins; j++) {
      iNumGenes += pBin->iNumEl;
      pBin++;
    }
    if(iNumGenes != pBuck->iNumEl) {
      vcl_fprintf(stderr, "ERROR(%s): There are %d genes in %d bins,\n",
          pcFName, iNumGenes, iNumBins);
      vcl_fprintf(stderr, "\trather than %d for bucket %d (out of %d).\n",
          pBuck->iNumEl, i, iNumBucks);
      return(FALSE);
    }
    pBuck++;
    iNumGeneTot += iNumGenes;
  }
  if(iNumGeneTot != pBuckets->iTotGene) {
    vcl_fprintf(stderr, "ERROR(%s): There are %d genes in %d buckets,\n",
        pcFName, iNumGenes, iNumBucks);
    vcl_fprintf(stderr, "\trather than an expected total of %d\n",
        pBuckets->iTotGene);
    return(FALSE);
  }
  
  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: FillSingleMetaBucketInXYZ
 *
 * Usage: Fill-in XYZ bins (delimited by integer coordinates along
 *    the X, Y & Z axes), which are nothing but voxels in this
 *    case (of a single Meta Bucket0).
 *
\*------------------------------------------------------------*/

void
FillSingleMetaBucketInXYZ(InputDataB *pInData,  Buckets0 *pBucketsXYZ,
        DimensionB *pDim)
{
  const char    *pcFName = "FillSingleMetaBucketInXYZ";
  register int  i,j;
  int    iNumGeneTot, iNumBinsXYZ, iXdim, iSliceSize, iWSpaceSize;
  int    iMax, iX,iY,iZ, iBin, iNum, iTmp, iNumMin,iNumMax, iEmpty;
  int    *piBinGene, *piBinGeneOld;
  float    fX,fY,fZ;
  Bucket0  *pMetaBuck;
  BinB    *pTmpBins, *pBinOld, *pBin;
  InputDataB  *pGene;

  iNumGeneTot = pBucketsXYZ->iTotGene;
  iNumBinsXYZ = pBucketsXYZ->iNumBins;
  iXdim = pDim->iXdim;
  iSliceSize = pDim->iSliceSize;
  iWSpaceSize = pDim->iWSpaceSize;

  vcl_fprintf(stderr, "MESG(%s):\n", pcFName);
  vcl_fprintf(stderr, "\tSet %d generators in %d Bins (voxels).\n",
    iNumGeneTot, iNumBinsXYZ);

  /* Temp Buffer of bins : we do not know a priori *\
  \*   how many generators per voxel we may have   */
  pTmpBins = NULL;
  if((pTmpBins = (BinB *) calloc(iNumBinsXYZ, sizeof(BinB))) == NULL) {
    vcl_fprintf(stderr, "ERROR(%s):\n\tCALLOC failed for pTmpBins[%d].\n",
      pcFName, iNumBinsXYZ);
    exit(-2); }
  pBin = pTmpBins;
  iMax = 10;
  for(i = 0; i < iNumBinsXYZ; i++) {
    pBin->piGenes = NULL;
    if((pBin->piGenes = (int *) calloc(iMax, sizeof(int))) == NULL) {
      vcl_fprintf(stderr,
        "ERROR(%s):\n\tCALLOC failed for pBin[%d]->piGenes[%d].\n",
        pcFName, i, iMax);
      exit(-2); }
    pBin->iNumEl = 0;
    pBin->iMaxEl = iMax;
    pBin++;
  }

  pGene = pInData-1;
  for(i = 0; i < iNumGeneTot; i++) {
    pGene++;

    fX = pGene->Sample.fPosX;
    iX = (int) floor((double) fX);
    fY = pGene->Sample.fPosY;
    iY = (int) floor((double) fY);
    fZ = pGene->Sample.fPosZ;
    iZ = (int) floor((double) fZ);

    iBin = iZ * iSliceSize  + iY * iXdim + iX;
    if(iBin > iWSpaceSize || iBin < 0) {
      vcl_fprintf(stderr,
        "ERROR(%s):\n\tVoxel Position = %d out of range.\n",
        pcFName, iBin);
      exit(-4); }

    pBin = pTmpBins+iBin;
    iNum = pBin->iNumEl;
    iMax = pBin->iMaxEl;
    piBinGene = pBin->piGenes+iNum;
    *piBinGene = i;
    pBin->iNumEl++;
    iNum++;
    if(iNum >= iMax) {
      iMax += iMax;
      if((pBin->piGenes =
    (int *) realloc((int *) pBin->piGenes,
        iMax * sizeof(int))) == NULL) {
  vcl_fprintf(stderr,
    "ERROR(%s):\n\tREALLOC fails on pBin[%d]->piGenes[%d].\n",
    pcFName, iBin, iMax);
  exit(-2); }
      pBin->iMaxEl = iMax;
    }

  } /* Next gene: i++ */

  /* Transfer data from Tmp Buffer */
  iNumMin = iNumGeneTot;
  iTmp = iNumMax = iEmpty = 0;
  pBinOld = pTmpBins;
  pBin = pBucketsXYZ->pBin;

  for(i = 0; i < iNumBinsXYZ; i++) {
    iNum = pBinOld->iNumEl;
    if(iNum < iNumMin) iNumMin = iNum;
    if(iNum > iNumMax) iNumMax = iNum;
    pBin->iMaxEl = pBin->iNumEl = iNum;
    pBin->piGenes = NULL;
    if(iNum < 1)
      iEmpty++;
    else {
      if((pBin->piGenes = (int *) calloc(iNum, sizeof(int))) == NULL) {
  vcl_fprintf(stderr,
    "ERROR(%s):\n\tCALLOC failed for pBin[%d]->piGenes[%d].\n",
    pcFName, i, iNum);
  exit(-2); }
      piBinGene = pBin->piGenes;
      piBinGeneOld = pBinOld->piGenes;
      for(j = 0; j < iNum; j++) {
  *piBinGene++ = *piBinGeneOld++;
      }
    }

    pBin++;
    pBinOld++;
    iTmp += iNum;

  } /* Next bin: i++ */

  if(iTmp != iNumGeneTot) {
    vcl_fprintf(stderr,
      "ERROR(%s):\n\tExpected to load %d genes in XYZ-Bins, not %d\n",
      pcFName, iNumGeneTot, iTmp);
    exit(-2); }

  /* Clean-up */
  pBinOld = pTmpBins;
  for(i = 0; i < iNumBinsXYZ; i++) {
    free (pBinOld->piGenes);
    pBinOld++;
  }
  free (pTmpBins);

  pBucketsXYZ->iMaxGene = iNumMax;
  pBucketsXYZ->iMinGene = iNumMin;
  pBucketsXYZ->iBinEmpty = iEmpty;

  vcl_fprintf(stderr, "\tOut of %d voxels, %d are empty.\n",
    iNumBinsXYZ, iEmpty);
  vcl_fprintf(stderr, "\t(Min , Max) gene per voxel = (%d , %d).\n",
    iNumMin, iNumMax);

  /* --- Set up some parameters for Single Meta Bucket0 --- */

  pMetaBuck = pBucketsXYZ->pBucket;
  pMetaBuck->iNumEl = iNumGeneTot;
  
  pMetaBuck->Limits.iMinX = 0;
  pMetaBuck->Limits.iMaxX = pDim->iXdim;
  pMetaBuck->Limits.iMinY = 0;
  pMetaBuck->Limits.iMaxY = pDim->iYdim;
  pMetaBuck->Limits.iMinZ = 0;
  pMetaBuck->Limits.iMaxZ = pDim->iZdim;
  pMetaBuck->iNumBins = iNumBinsXYZ;
  pMetaBuck->iBinLast = iNumBinsXYZ - 1;

  return;
}

/* --- EoF --- */


