#ifndef _SHEET_FLOW_BUCKS_IO_H_
#define _SHEET_FLOW_BUCKS_IO_H_


#include "sheet_flow_mbuck_mres.h"
///#include "time.h"

int
LoadDataMBuck(char *pcInFile, InputData *pInData, Buckets *pBucksXYZ,
        Dimension *pDim, int iMaxSize);

///void
///SaveTimings(char *pcInFile, char *pcOutFile, Timings *pTime,
///      ShockScaffold *pScaffold, Dimension *pDim, time_t *pTimeInit);

void
SaveShockData(char *pcInFile, char *pcOutFile, InputData *pInData,
        ShockScaffold *pScaf, Dimension *pDimMBuck,
        Dimension *pDimAll);


bool save_fs_ply2 (char *pcInFile, char *pcOutFile, InputData *pInData,
        ShockScaffold *pScaf, Dimension *pDimMBuck,
        Dimension *pDimAll);

void
SaveBuckData(char *pcInFile, char *pcOutFile, Buckets *pBucksXYZ,
       Dimension *pDim);

void
CleanUpInData(InputData *pInData, Dimension *pDim);

void
CleanUpMBuck(Buckets *pBucksXYZ, Dimension *pDim);

///void
///TestInData(InputData *pInData, Dimension *pDim);

void
FillVoxels(char *pcInFile, Pt3dCoord *pGeneCoord, Voxel *pVoxels,
     Dimension *pDim);

#endif /* _SHEET_FLOW_BUCKS_IO_H_ */
