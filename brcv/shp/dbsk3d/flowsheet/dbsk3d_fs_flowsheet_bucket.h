#ifndef _dbsk3d_fs_flowsheet_bucket_h_
#define _dbsk3d_fs_flowsheet_bucket_h_


#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_cstring.h>

#include <vcl_vector.h>

#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbsk3d/flowsheet/dbsk3d_fs_flowsheet.h>

//#################################### SORT3DPTS_SPACE.H

/* -------------- Macros --------- */
#define SECS(tv)  (tv.tv_sec + tv.tv_usec / 1000000.0)


/* -------------- Structures --------------- */
typedef struct {
  float    fSysInit, fUserInit, fMemInit;
  float    fSysFillX, fUserFillX, fMemFillX;
  float    fSysFillXY, fUserFillXY, fMemFillXY;
  float    fSysFillXYZ, fUserFillXYZ, fMemFillXYZ;
  float    fSysQHull, fUserQHull, fMemQHull;
  float    fSysEnd, fUserEnd, fMemEnd;
} TimingsB;

typedef struct {
  int    iZdim, iYdim, iXdim, iWSpaceSize, iSliceSize;
  int    iNum3dPts, iNumBuck, iMetaBuckSize, iNumBinEmpty;
  int    iNumMetaBuckets; /* Total number of MetaBuckets */
  int    iMetaBuckId; /* Current MetaBucket being processed */
  int    iMaxBlueShocks, iMaxPairs;
  float    fOffset, fMoveX,fMoveY,fMoveZ, fRadMax, fOff;
  float    fMaxX,fMaxY,fMaxZ, fMinX,fMinY,fMinZ;
} DimensionB;

///typedef struct {
///  float    fPosX, fPosY, fPosZ;
////} Pt3dCoord;

///typedef struct {
///  int    iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ;
///} GridRange;

/* Data structure for each input sample point */
typedef struct {
  Pt3dCoord  Sample;
} InputDataB;

typedef struct {
  int    iNumEl;
  int    iMaxEl;
  int    *piGenes;
} BinB;

typedef struct {
  int    iNumEl;    /* Total number of generators in Bucket0 */
  int    iBinFirst;
  int    iBinLast;
  int    iNumBins;
  int    iNumCHgenes;  /* Num. of genes being Convex Hull vertices */
  GridRange  Limits;
  int    *piGenes;  /* List of generators in Bucket0 */
  int    *piCHgenes;  /* List of CH vertices */
} Bucket0;

typedef struct {
  /* int       iMinCoord; Integer lower coordinate for first bin/bucket */
  int    iNumBins;
  int    iNumBucks;
  int    iTotGene; /* Total number of genes for all buckets */
  int    iAvgGene;
  int    iMaxGene; /* Max number of generators in any given bucket */
  int    iMinGene; /* Min number of generators in any given bucket */
  int    iEmpty, iBinEmpty;
  int    iVirtual;
  float    fTolerance;
  GridRange  Limits;
  BinB    *pBin;
  Bucket0  *pBucket;
} Buckets0;

typedef struct {
  int    iNumMetaBucks; /* Total number of XYZ-MetaBuckets */
  int    iNumMetaBins;  /* Total number of XYZ-bins used */
  /* --- Info for each MetaBucket --- */
  int    *piNumBuckets; /* Number of XYZ-buckets */
  int    *piNumBuckEmpty; /* Number of empty XYZ-buckets */
  int    *piNumBins;    /* Number of XYZ-bins */
  int    *piNumGenes;   /* Number of generators */
  int    *piAvgGene;    /* Average Number of genes per bucket */
  int    *piMinGene;    /* Minimum Number of genes per bucket */
  int    *piMaxGene;    /* Maximum Number of genes per bucket */
} MetaBuckInfoB;


typedef struct {
  int    iBucket;
  int    iGene[2];
  /*  float    fDist; */
} Pair;

typedef struct {
  int    iNum, iMax;
  Pair    *pPair;
} Pairs;

/* -------------- Constants ---------------- */

#ifndef FALSE
#define FALSE    0
#define TRUE    1
#endif

#define MY_DEBUG  FALSE

#ifndef D_EPSILON
#define D_EPSILON  0.001
#endif

#ifndef D_LARGE
#define D_LARGE    50000000
#endif


//#################################### _FILL_BUCKS_H_
void FillFirstBucketsInX (InputDataB *pInData, Buckets0 *pBuckets, DimensionB *pDim);

void FillBucketsInXY (InputDataB *pInData, Buckets0 *pBucketsX,
                      Buckets0 *pBucketsXY, DimensionB *pDim);

void FillBucketsInXYZ (InputDataB *pInData, Buckets0 *pBucketsXY,
                       Buckets0 *pBucketsXYZ, DimensionB *pDim);

void FillBucketsInXFromMetaBucket (InputDataB *pInData, BinB *pBins,
                                   Bucket0 *pMetaBuck, Buckets0 *pBucketsX, DimensionB *pDim);

void FillBinsInXYZ (InputDataB *pInData, BinB *pBins, DimensionB *pDim);


//#################################### SORT3DPTS_IO.H

int SetInFile (const char *pcUPennFile, InputDataB *pInData, DimensionB *pDim, int iFlagDuplic);

void SaveDataBucks (char *pcInFile, char *pcOutFile, InputDataB *pInData,
                    Buckets0 *pBucketsX, Buckets0 *pBucketsXY, Buckets0 *pBucketsXYZ, DimensionB *pDim);

void SaveTimings (char *pcInFile, char *pcOutFile, TimingsB *pTime,
                  Buckets0 *pBucketsX, Buckets0 *pBucketsXY, Buckets0 *pBucketsXYZ, DimensionB *pDim);

void SaveDataMetaBuck (const char *pcInFile, char *pcOutFile, InputDataB *pInData,
                       Buckets0 *pBucketsX, Buckets0 *pBucketsXY,
                       Buckets0 *pBucketsXYZ, DimensionB *pDim);

void SaveG3dMetaBuck (char *pcOutFile, InputDataB *pInData,
                      Buckets0 *pBucketsXYZ, DimensionB *pDim);

void SaveTimingsMetaBuck (const char *pcInFile, char *pcOutFile, TimingsB *pTime,
                          Buckets0 *pMetaBucksXYZ, MetaBuckInfoB *pMBuckInfo,
                          DimensionB *pDim);

void SaveAllDataMetaBuck (const char *pcInFile, char *pcOutFile, InputDataB *pInData,
                          BinB *pBins, DimensionB *pDim);

void SaveG3dAll (char *pcOutFile, InputDataB *pInData, BinB *pBins, DimensionB *pDim);


//#################################### SORT3DPTS_UTIL.H
void CleanUpBuckBin (Buckets0 *pBucketsX, Buckets0 *pBucketsXY, Buckets0 *pBucketsXYZ);

int CheckBuck (Buckets0 *pBuckets);

void FillSingleMetaBucketInXYZ (InputDataB *pInData,  Buckets0 *pBucketsXYZ, DimensionB *pDim);


//=========== FlowSheet ===========
void run_flowsheet_bucketing (dbmsh3d_mesh* geneset,
                              const char* input_file);

#endif
