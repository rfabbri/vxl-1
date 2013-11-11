#ifndef _3DSHOCK_FRE_FLOWSHEET_H_
#define _3DSHOCK_FRE_FLOWSHEET_H_

#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_cstring.h>

#include <vcl_vector.h>

#include <dbmsh3d/dbmsh3d_utils.h>

typedef struct {
  float    fPosX, fPosY, fPosZ;
} Pt3dCoord;

/* -------------- Macros --------- */

#define SECS(tv)  (tv.tv_sec + tv.tv_usec / 1000000.0)

#ifndef UCHAR
#define UCHAR unsigned char
#endif

#ifndef MAXSHORT
#define MAXSHORT 30000
#endif

/* -------------- Structures --------------- */

typedef struct {
  float    fSysInit, fUserInit, fMemInit;
  float    fSysSub, fUserSub, fMemSub;
  float    fSysBlueInt, fUserBlueInt, fMemBlueInt;
  float    fSysAllInt, fUserAllInt, fMemAllInt;
  float    fSysBlueFst, fUserBlueFst, fMemBlueFst;
  float    fSysAllFst, fUserAllFst, fMemAllFst;
  float    fSysBlueOth, fUserBlueOth, fMemBlueOth;
  float    fSysAllOth, fUserAllOth, fMemAllOth;
  float    fSysItera, fUserItera, fMemItera;
  float    fSysEnd, fUserEnd, fMemEnd;
} Timings;

/*typedef struct {
  float    fPosX, fPosY, fPosZ;
} Pt3dCoord;*/

typedef struct {
  float    fX, fY, fZ;
  float    fLengthSq;  /* Length of vector, squared */
} Vector3d;

typedef struct {
  short    iMinX, iMaxX, iMinY, iMaxY, iMinZ, iMaxZ;
} GridRange;

typedef struct {
  short    isFlagPermute, isFlagRad;
  int    iZdim, iYdim, iXdim, iWSpaceSize, iSliceSize;
  int    iNum3dPts, iNumBuck, iMaxItera, iMemDiv;
  int    iNumShockSheetIntAll, iNumShockSheetIntSub;
  int    iNumShockSheetExtAll, iNumShockSheetExtSub;
  int    iNumSubSamples;
  int    iNumShockSheet, iMaxBlueShocks;
  /* int  iNumShockSheetV, iNumShockCurveV, iNumShockVertexV; */
  int    iMaxProxiPerBlueShock, iTotalBlueProxi;
  int    iNumShockCurve, iMaxGreenShocks;
  int    iNumShockGreen2Blue, iNumCurveActiveMax;
  int    iMaxProxiPerGreenShock, iTotalGreenProxi;
  int    iNumShockVertex, iMaxRedShocks;
  int    iNumShockRed2Green, iNumShockRed2Blue;
  int    iMetaBuckId, iNumMBucks;
  float    fOffset, fMoveX,fMoveY,fMoveZ, fRadMax;
  float    fMinX,fMinY,fMinZ, fMaxX,fMaxY,fMaxZ;
  float    fBallRadMax, fBallRadMin;
  GridRange  Limits; /* Bounding box integer coordinates */
} Dimension;

/* Data structure for each input sample point */
typedef struct {
  short    isFlagPermute;  /* Used to avoid permutations of n-tuples */
  short    isNumBlue,  isMaxBlue;
  short    isNumGreen, isMaxGreen;
  short    isNumRed,   isMaxRed;
  /* short  isNumNorm, isMaxNorm; Number of normals (constraints) */
  int    iID;    /* Original label/order as input */
  int    iBuckXYZ;  /* Label of associated BucketXYZ */
  int    iBinXYZ;  /* Label of associated BinXYZ */
  Pt3dCoord  Sample;
  int    *piBlueList;  /* Backpointers to Sheets */
  int    *piGreenList;  /* Backpointers to Curves */
  int    *piRedList;  /* Backpointers to Vertices */
  Vector3d  *pvNormal;  /* Vector coord of Pb - Pa with length^2 */
} InputData;

typedef struct {
  short    isNumGenes;
  int    *piGenes;
} Voxel;

typedef struct {
  short    isNumGenes;
  short    isMaxGenes;
  int    *piGenes;
} Bin;

typedef struct {
  short    isBuck; /* Buck Label */
  short    isBuckFrom; /* Generator Buck Label */
  short    isItera; /* Iteration at which bucket become a viz. ngb. */
  short    isNumGeneViz; /* Number of visible generators */
  short    isNumVisit; /* Number of times visible generators are added */
  int    *piGenes; /* Labels of visible sources */
} NgbBuck;

typedef struct {
  short    isNumBins;
  short    isNumGenes; /* Num of genes in Bucket */
  short    isNumSubGenes; /* Num of genes sub-sampled */
  short    isNumNgbs; /* First layer of ngb buckets */
  short    isMaxNgbs; /* Space allocated pNgbViz (other layers) */
  short    isNumPair; /* Number of buckets used in pairings */
  short    isLastItera;
  int    iInitSheetLabel;
  int    iInitCurveLabel;
  int    iInitVertexLabel;
  int    iBinFirst;
  int    iBinLast;
  GridRange  Limits;
  int    *piGenes; /* Labels of genes in the bucket */ 
  int    *piSubGenes; /* Labels of sub-sampled genes in the bucket */ 
  int    *piNgbs; /* List of neighboring buckets (1st layer) */
  int    *piNgbPerItera; /* Number of Ngbs per Iteration */
  int    *piFstNgb; /* Pointer to 1st Ngb in list for each Itera */
  int    *piPtrNgb; /* Pointer to pNgbViz in terms of Buck label */
  UCHAR    *pucFlagNgb; /* 0=No , 1=Self , 2=DirNgb , 3=OtherNgb */
  UCHAR    *pucFlagViz; /* 0 = Not yet , 1 = Visible , 2 = Not Visible */
  UCHAR    *pucFlagPair; /* 0 = Not yet , 1 = Open , 2 = Closed */
  NgbBuck  *pNgbViz; /* List of visible ngb. buckets (other layers) */
} Bucket;

typedef struct {
  int    iInitSheetLabel; /* Highest Label thus far */
  int    iInitCurveLabel;
  int    iInitVertexLabel;
  int    iMinCoord; /* Integer lower coordinate for first bin/bucket */
  int    iNumBins;
  int    iNumBucks;
  int    iNumGenes, iAvgGenes, iMaxGenes, iMinGenes;
  int    iEmpty, iBinEmpty;
  int    iVirtual;
  float    fTolerance;
  GridRange  Limits;
  Bin    *pBin;
  Bucket  *pBucket;
} Buckets;

typedef struct {
  int    iNumMetaBucks; /* Total number of XYZ-MetaBuckets */
  int    iNumMetaBins;  /* Total number of XYZ-bins used */
  /* --- Info for each MetaBucket --- */
  int    *piNumBuckets; /* Number of XYZ-buckets */
  int    *piNumBuckEmpty; /* Number of empty XYZ-buckets */
  int    *piNumBins;    /* Number of XYZ-bins */
  int    *piNumGenes;   /* Number of generators */
  int    *piAvgGenes;   /* Average Number of genes per bucket */
  int    *piMinGenes;   /* Minimum Number of genes per bucket */
  int    *piMaxGenes;   /* Maximum Number of genes per bucket */
} MetaBuckInfo;


/* -- Meta Buckets and their reduced Bucket structure for Validation -- */

typedef struct {
  int    iInitSheetLabel;
  int    iInitCurveLabel;
  int    iInitVertexLabel;
  int    iBinFirst;
  int    iBinLast;
  short    isNumBins;
  short    isNumGenes; /* Num of genes in Bucket */
  short    isNumNgbs; /* Number of ngb buckets in 1st layer */
  GridRange  Limits;
  int    *piGenes; /* Labels of genes in the bucket */ 
  int    *piNgbs; /* List of neighboring buckets (1st layer) */
} MBucket;

typedef struct {
  int    iInitSheetLabel; /* Highest Label thus far */
  int    iInitCurveLabel;
  int    iInitVertexLabel;
  int    iNumBins, iNumBucks;
  int    iNumGenes, iMaxGenes, iMinGenes;
  int    iXdim,iYdim,iZdim, iSliceSize, iWSpaceSize;
  short    isNumNgbs; /* Number of ngb MetaBuckets in 1st layer */
  GridRange  Limits;
  Pt3dCoord  *pGeneCoord; /* List of genes in MetaBucket */
  Bin    *pBin;
  MBucket  *pBucket; /* List of buckets making the MetaBucket */
  short    *pisLookUp; /* 3D Look-up table of buckets */
  int    *piNgbs; /* List of neighboring MetaBuckets (1st layer) */
} MetaBucket;

typedef struct {
  int    iMBuckID;      /* Currently processed MetaBucket  */
  int    iNumMetaBucks; /* Total number of XYZ-MetaBuckets */
  int    iMBuckEmpty;
  int    iNumBins;
  int    iNumGenes, iMaxGenes, iMinGenes;
  int    iXdim,iYdim,iZdim, iSliceSize, iWSpaceSize;
  GridRange  Limits;
  MetaBucket  *pMBucks;      /* List of MetaBuckets containing Buckets */
  short    *pisLookUp;    /* 3D Look-up table of MetaBuckets */
} MetaBuckTop;


typedef struct {
  short    iNumTri, iMaxTri;
  int    *piTriList;  /* pointers to list of triplets */
} Triplets;

typedef struct {
  short    iNumQuad, iMaxQuad;
  int    *piQuadList;  /* pointers to list of quadruplets */
} Quadruplets;

#define HIGH_MEM  1  /* If TRUE: Save float shock data */

typedef struct {
  short    isFlagValid;
  short    isFlagPermute;  /* Used to avoid permutations in intercepts */
  /* short  isFlagSolid;   Consolid. vertex = TRUE; Attractor = FALSE */
  short    isStatus;  /* Active or Passive */
  short    isItera;  /* Iteration s.t. sheet is created */
  short    isInitType;  /* Sub, Initial, GreenII, RedIII */
  /* short  isSolid;    ID of attractor */
  short    isBounded;  /* TRUE: closed loop of curves */
  short    isNumRed;  /* Number of vertices bounding the sheet */
  short    isNumGreen;  /* Number of curves bounding the sheet */
  int    iBuckXYZ;  /* Label of associated BucketXYZ */
  int    iGene[2];
  int    iGreen;    /* Backpointer to source shock */
  int    iRed;    /* Backpointer to source shock */
  int    *piGreenList;  /* Green curves bounding the shock sheet */
  int    *piRedList;  /* Red vertices bounding the shock sheet */
#if HIGH_MEM
  float    fDist2GeneSq;  /* Squared distance to sources = Radius^2 */
  Pt3dCoord  MidPt;    /* Absolute Shock coordinates */
#endif
} ShockSheet;

#define BLUE_SUB         -1 /* Low Res. Shock */
#define BLUE_INIT    1
#define BLUE_FROM_GREENII  2
#define BLUE_FROM_REDIII  3
#define BLUE_MISS    4

#define DUPLICATE      -3
#define OFFSHOOT       -2
#define IDLE         -1
#define ACTIVE    1
#define PASSIVE         -1

typedef struct {
  short    isFlagValid;
  short    isFlagPermute;  /* Used to avoid permutations in intercepts */
  /* short  isFlagSolid;   Consolid. vertex = TRUE; Attractor = FALSE */
  /* short  isSolid;  ID of attractor */
  short    isStatus;  /* Active or Passive */
  short    isNumEndPts;  /* Number of End Points (from 0 to 2) */
  short    isItera;  /* Iteration at which the curve is created */
  short    isType;    /* Green I, II or III */
  short    isInitType;  /* Blue, RedII, RedIII */
  int    iBuckXYZ;  /* Label of associated BucketXYZ */
  int    iGene[3];
  int    iBlue[3];  /* Back pointers to Sheets */
  int    iRed[2];  /* Vertex track.: 2 ends or 1 end, 1 begin */
#if HIGH_MEM
  float    fDist2GeneSq;  /* Squared distance to sources */
  float    fTriCoord[3];  /* Trilinear Coords of Shock */
  float    fTriArea;  /* Area of associated triangle */
  float    fMaxSideLengthSq; /* Max of fTriSides[3] */
  float    fMinSideLengthSq;
  float    fPerimeter;
  Pt3dCoord  CircumTri;  /* Absolute Shock coordinates */
#endif
} ShockCurve;

#define GREEN_FROM_BLUE    1
#define GREEN_FROM_REDII  2
#define GREEN_FROM_REDIII  3
#define GREEN_FROM_RED_MISS  4 /* Created from a Red shock, but incoming */

#define GREEN_I    1  /* Non-obtuse triangle */
#define GREEN_II  2  /* Obtuse */
#define GREEN_III  3  /* Right */

typedef struct {
  short    isFlagValid;
  /* short  isFlagSolid;  Consolid. vertex = TRUE; Attractor = FALSE */
  short    isStatus;  /* Active or Passive */
  /* short  isSolid;   ID of attractor */
  short    isItera;  /* Iteration at which the vertex is created */
  short    isType;    /* Red I, II or III */
  int    iBuckXYZ;  /* Label of associated BucketXYZ */
  int    iGene[4];
  int    iGreen[4];  /* Back pointers to Curves */
#if HIGH_MEM
  float    fDist2GeneSq;  /* Squared distance to sources */
  float    fQuadCoord[4];  /* Quadrilinear Coords of Shock */
  float    fTetVolume;  /* Volume of associated Tetrahedron */
  Pt3dCoord  CircumSphere;  /* Absolute Shock coordinates */
#endif
} ShockVertex;

#define RED_I    1  /* Reg. Tet.: No green exits */
#define RED_II    2  /* Cap: One green exit */
#define RED_III    3  /* Sliver: Two green exits */

typedef struct {
  int    iGene[4];  /* Up to 4 genes */
  float    fDist2GeneSq;  /* Squared distance to sources */
  float    fBaryCoord[4];  /* Barycentric Coords of Shock */
  float    fGeom;    /* Volume, Area */
  Pt3dCoord  ShockCoord;  /* Absolute Shock coordinates */
} ShockData;


/* -- Main Shock Scaffold structure -- */

typedef struct {
  int    iNumBlueMax;    /* Max number of sheets allocated */
  /* int  iNumBlueSubMax;   * Sheets computed at Low Resolution */
  int    iNumBlueSub;      /* Still valid at the end */
  int    iNumBlueInt;      /* Sheets initiated INSIDE a Bucket */
  /* int  iNumBlueIntMax; */
  int    iNumBlueFst;      /* Sheets from FIRST Layer of buckets */
  /* int  iNumBlueFstMax; */
  int    iNumBlueOth;      /* Sheets from OTHER Layers of buckets */
  /* int  iNumBlueOthMax; */
  int    iNumBlueItera;    /* Sheets from OTHER Layers (iterations) */
  int    iNumBlueIsolated; /* Sheets not initiated by curves or vertices */
  int    iNumBlueFromGreen;
  int    iNumBlueFromRed;
  int    iNumBlueActive;
  int    iNumBluePassive;
  int    iNumBlueNew;    /* New Sheets found at iteration i */
  int    iBluePassiveLast; /* Last Sheet processed thus far */
  int    iBlueActiveLast;  /* Last Sheet to process */
  int    iMinBluePerGene;
  int    iMaxBluePerGene;
  ShockSheet  *pBlue;      /* Initial Valid Sheet Shocks */
  int    iNumGreenMax;    /* Max number of curves allocated */
  int    iNumGreenInt;     /* Curves initiated INSIDE a Bucket */
  int    iNumGreenFst;     /* Curves from FIRST Layer of buckets */
  int    iNumGreenOth;     /* Curves from OTHER Layers (itera 0) */
  int    iNumGreenItera;   /* Curves from OTHER Layers (iterations) */
  int    iNumTriAcute;
  int    iNumTriObtuse;
  int    iNumTriRight;
  int    iNumGreenFromSheets;
  int    iNumGreenFromCaps;
  int    iNumGreenFromSlivers;
  int    iNumGreenNoEnd;
  int    iNumGreenOneEnd;
  int    iNumGreenTwoEnds;
  int    iNumGreenActive;  /* Curves with open ends */
  int    iNumGreenPassive; /* Fully connected curves */
  int    iNumGreenNew;    /* New Curves found at iteration i */
  int    iGreenPassiveLast; /* Last Curve processed thus far */
  int    iGreenActiveLast; /* Position of Most recently added shock */
  int    iGreenActiveFst;  /* Position of 1st Active shock */
  int    iMinGreenPerGene;
  int    iMaxGreenPerGene;
  float    fTriAreaMin;
  float    fTriAreaMax;
  ShockCurve  *pGreen;  /* Initial Valid Curve Shocks */
  int    iNumRedMax;  /* Max number of vertices allocated */
  int    iNumRedInt;     /* Vertices initiated INSIDE a Bucket */
  int    iNumRedFst;     /* Vertices from FIRST Layer of buckets */
  int    iNumRedOth;     /* Vertices from OTHER Layers (itera 0) */
  int    iNumRedItera;   /* Vertices from OTHER Layers (iterations) */
  int    iNumTetRegulars; /* Tets with circumcenter inside */
  int    iNumTetCaps;
  int    iNumTetSlivers;
  int    iNumRedNew;  /* New Vertices found at iteration i */
  int    iNumRedLast;  /* Position of Last Vertex on Scaffold */
  int    iMinRedPerGene;
  int    iMaxRedPerGene;
  float    fTetVolMin;
  float    fTetVolMax;
  ShockVertex  *pRed;    /* Initial Valid Shock Vertex */
  short    isMaxItera;  /* Max space allocated for iterations */
  short    isLastItera;  /* Last iteration thus far */
  short    isBlueItera;  /* Iterations for finding Blue sheets */
  int    iTotalShocks;  /* Total number of shock vertices thus far */
  int    *piNumBluePerItera; /* New shocks per iteration */
  int    *piNumGreenPerItera; /* New shocks per iteration */
  int    *piNumGreenActivePerItera;
  int    *piNumGreenPassivePerItera;
  int    *piNumRedPerItera; /* New shocks per iteration */
  int    *piNumShocksPerItera; /* New (total) shocks per iteration */
  float    *pfTimePerItera;
} ShockScaffold;


/* -------------- Constants ---------------- */

#ifndef FALSE
#define FALSE    0
#define TRUE    1
#endif

#define MY_DEBUG  FALSE

#ifndef D_EPSILON
#define D_EPSILON  0.0001
#endif

#ifndef E_EPSILON
#define E_EPSILON  0.000001
#endif

int FreShockFlowSheet (const char* filePrefix);

#endif
