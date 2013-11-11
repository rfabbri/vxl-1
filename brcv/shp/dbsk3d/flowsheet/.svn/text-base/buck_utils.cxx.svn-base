/*------------------------------------------------------------*\
 * File: buck_utils.c
 *
 * Functions:
 *  Set3dLookUpTable
 *  Set3dLookUpTableForMBuck
 *  Set3dLookUpTableForMBucks
 *  SetBuckNgbs
 *  SetBuckNgbsForMBuck
 *  SetBuckNgbsForMBucks
 *  SetBuckSub
 *  ResetBuckFlags
 *  SetBuckSubQhull
 *
 * History:
 *  #0: August 2001, by F.Leymarie
 *  #1: Oct. 2001: Added SetBuckSub() & ResetBuckFlags()
 *  #2: Oct. 2001: Added SetBuckSubQhull()
 *  #3: July 2003: Added Set3dLookUpTableForMBuck() and
 *    SetBuckNgbsForMBuck().
 *
\*------------------------------------------------------------*/

#include "buck_utils.h"

/*------------------------------------------------------------*\
 * Function: Set3dLookUpTable
 *
 * Usage: Using the BucketsXYZ Data, init a 3D lookup table.
 *
\*------------------------------------------------------------*/

void
Set3dLookUpTable(short *pisLookUp, Buckets *pBucksXYZ, Dimension *pDim)
{
  char    *pcFName = "Set3dLookUpTable";
  register int  i, j, k, l, m;
  int    iXdim, iSliceSize, iNumBucks, iX1,iX2, iY1,iY2, iZ1,iZ2;
  int    iZjump, iYjump, iPos, iMax, iMinX,iMinY,iMinZ;
  int    iWSpaceSize;
  Bucket  *pBuck;

  /* Preliminaries */

  iNumBucks = pBucksXYZ->iNumBucks;
  fprintf(stderr, "MESG(%s):\n\tfor %d Buckets....\n", pcFName,
    iNumBucks);

  iXdim = pDim->iXdim;
  iSliceSize = pDim->iSliceSize;
  iWSpaceSize = pDim->iWSpaceSize;
  m = 0;
  iMax = (int) MAXSHORT;
  if(iNumBucks >= iMax) {
    fprintf(stderr,
      "ERROR(%s): Number of Buckets = %d exceeds MaxShort = %d\n",
      pcFName, iNumBucks, iMax);
    exit(-2);
  }

  iMinX = pBucksXYZ->Limits.iMinX;
  iMinY = pBucksXYZ->Limits.iMinY;
  iMinZ = pBucksXYZ->Limits.iMinZ;

  pBuck = pBucksXYZ->pBucket;
  for(i = 0; i < iNumBucks; i++) {

    iX1 = pBuck->Limits.iMinX - iMinX;
    iX2 = pBuck->Limits.iMaxX - iMinX;
    iY1 = pBuck->Limits.iMinY - iMinY;
    iY2 = pBuck->Limits.iMaxY - iMinY;
    iZ1 = pBuck->Limits.iMinZ - iMinZ;
    iZ2 = pBuck->Limits.iMaxZ - iMinZ;

    for(j = iZ1; j < iZ2; j++) {
      iZjump = j * iSliceSize;
      for(k = iY1; k < iY2; k++) {
  iYjump = k * iXdim;
  for(l = iX1; l < iX2; l++) {
    iPos = iZjump + iYjump + l;
    if(iPos > iWSpaceSize || iPos < 0) {
      fprintf(stderr,
        "ERROR(%s):\n\tPosition = %d out of range.\n",
        pcFName, iPos);
      exit(-4);
    }
    pisLookUp[iPos] = (short) i; /* Bucket label */
    m++; /* one more entry in the LookUpTable */
  }
      }
    }
    
    pBuck++;
  }

  if(m != iWSpaceSize) {
    fprintf(stderr,
      "ERROR(%s): Set %d entries in the table instead of %d .\n",
      pcFName, m, pDim->iWSpaceSize);
    exit(-2);
  }

  fprintf(stderr, "\tSet all %d entries in the table.\n", m);

  return;
}

/*------------------------------------------------------------*\
 * Function: Set3dLookUpTableForMBuck
 *
 * Usage: Using the BucketsXYZ Data, init a 3D lookup table,
 *    for a given MetaBucket.
 *
\*------------------------------------------------------------*/
#if 0
void
Set3dLookUpTableForMBuck(MetaBucket *pMBuck)
{
  char    *pcFName = "Set3dLookUpTableForMBuck";
  register int  i, j, k, l, m;
  short    *pisLookUp;
  int    iXdim, iSliceSize, iNumBucks, iX1,iX2, iY1,iY2, iZ1,iZ2;
  int    iZjump, iYjump, iPos, iMax;
  MBucket  *pBuck;


  iNumBucks = pMBuck->iNumBucks;
  fprintf(stderr, "MESG(%s):\n\tfor %d Buckets....\n", pcFName,
    iNumBucks);

  iXdim = pMBuck->iXdim;
  iSliceSize = pMBuck->iSliceSize;
  m = 0;
  iMax = (int) MAXSHORT;
  if(iNumBucks >= iMax) {
    fprintf(stderr,
      "ERROR(%s): Number of Buckets = %d exceeds MaxShort = %d\n",
      pcFName, iNumBucks, iMax);
    exit(-2);
  }
  pBuck = pMBuck->pBucket;
  pisLookUp = pMBuck->pisLookUp;
  for(i = 0; i < iNumBucks; i++) {

    iX1 = pBuck->Limits.iMinX;
    iX2 = pBuck->Limits.iMaxX;
    iY1 = pBuck->Limits.iMinY;
    iY2 = pBuck->Limits.iMaxY;
    iZ1 = pBuck->Limits.iMinZ;
    iZ2 = pBuck->Limits.iMaxZ;

    for(j = iZ1; j < iZ2; j++) {
      iZjump = j * iSliceSize;
      for(k = iY1; k < iY2; k++) {
  iYjump = k * iXdim;
  for(l = iX1; l < iX2; l++) {
    iPos = iZjump + iYjump + l;
    pisLookUp[iPos] = (short) i; /* Bucket label */
    m++; /* one more entry in the LookUpTable */
  }
      }
    }
    
    pBuck++;
  }

  if(m != pMBuck->iWSpaceSize) {
    fprintf(stderr,
      "ERROR(%s): Set %d entries in the table instead of %d .\n",
      pcFName, m, pMBuck->iWSpaceSize);
    exit(-2);
  }

  /* fprintf(stderr, "MESG(%s):\n\tSet all %d entries in the table.\n",
     pcFName, m); */

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: Set3dLookUpTableForMBucks
 *
 * Usage: Using the BucketsXYZ Data, init a 3D lookup table,
 *    for all MetaBuckets.
 *
\*------------------------------------------------------------*/

void
Set3dLookUpTableForMBucks(MetaBuckTop *pMBuckTop)
{
  char    *pcFName = "Set3dLookUpTableForMBucks";
  register int  i, j, k, l, m;
  short    *pisLookUp;
  int    iXdim, iSliceSize, iNumMBucks, iX1,iX2, iY1,iY2, iZ1,iZ2;
  int    iZjump, iYjump, iPos, iMax;
  MetaBucket  *pMBuck;

  iNumMBucks = pMBuckTop->iNumMetaBucks;
  fprintf(stderr, "MESG(%s):\n\tfor %d MetaBuckets....\n", pcFName,
    iNumMBucks);

  iXdim = pMBuckTop->iXdim;
  iSliceSize = pMBuckTop->iSliceSize;
  m = 0;
  iMax = (int) MAXSHORT;
  if(iNumMBucks >= iMax) {
    fprintf(stderr,
      "ERROR(%s): Number of Buckets = %d exceeds MaxShort = %d\n",
      pcFName, iNumMBucks, iMax);
    exit(-2);
  }
  pMBuck = pMBuckTop->pMBucks;
  pisLookUp = pMBuckTop->pisLookUp;
  for(i = 0; i < iNumMBucks; i++) {

    iX1 = pMBuck->Limits.iMinX;
    iX2 = pMBuck->Limits.iMaxX;
    iY1 = pMBuck->Limits.iMinY;
    iY2 = pMBuck->Limits.iMaxY;
    iZ1 = pMBuck->Limits.iMinZ;
    iZ2 = pMBuck->Limits.iMaxZ;

    for(j = iZ1; j < iZ2; j++) {
      iZjump = j * iSliceSize;
      for(k = iY1; k < iY2; k++) {
  iYjump = k * iXdim;
  for(l = iX1; l < iX2; l++) {
    iPos = iZjump + iYjump + l;
    pisLookUp[iPos] = (short) i; /* Bucket label */
    m++; /* one more entry in the LookUpTable */
  }
      }
    }
    
    pMBuck++;
  }

  if(m != pMBuckTop->iWSpaceSize) {
    fprintf(stderr,
      "ERROR(%s): Set %d entries in the table instead of %d .\n",
      pcFName, m, pMBuckTop->iWSpaceSize);
    exit(-2);
  }

  /* fprintf(stderr, "MESG(%s):\n\tSet all %d entries in the table.\n",
     pcFName, m); */

  return;
}

/*------------------------------------------------------------*\
 * Function: SetBuckNgbs
 *
 * Usage: For each BucketsXYZ, find its direct neighbors, using
 *    the 3D lookup table to search a sausage around the
 *    bucket.
 *
\*------------------------------------------------------------*/

void
SetBuckNgbs(short *pisLookUp, Buckets *pBucksXYZ, Dimension *pDim)
{
  char    *pcFName = "SetBuckNgbs";
  register int  i, j, k, l, m;
  int    iXdim,iYdim,iZdim, iSliceSize, iNumBucks, iMax, iTotal;
  int    iX1,iX2, iY1,iY2, iZ1,iZ2, iZjump,iYjump, iPos, iNum;
  int    iXsize,iYsize,iZsize, iFlagNew, iLabel, iMin;
  int    iXstart,iYstart,iZstart, iXend,iYend,iZend, iMaxItera;
  int    iMinX,iMinY,iMinZ;
  int    *piTmpNgbs, *piNgb, *piBuckNgb;
  float    fAvg;
  Bucket  *pBuck;

  /* Preliminaries */
  iNumBucks = pBucksXYZ->iNumBucks;
  fprintf(stderr, "MESG(%s): for %d Buckets....\n", pcFName,
    iNumBucks);

  iXdim = pDim->iXdim;
  iYdim = pDim->iYdim;
  iZdim = pDim->iZdim;
  iSliceSize = pDim->iSliceSize;
  pBuck = pBucksXYZ->pBucket;
  iMax = iTotal = 0;
  iMin = iNumBucks;
  iMaxItera = pDim->iMaxItera;
  iMinX = pBucksXYZ->Limits.iMinX;
  iMinY = pBucksXYZ->Limits.iMinY;
  iMinZ = pBucksXYZ->Limits.iMinZ;

  /* ---- Process each BucketXYZ ---- */

  for(i = 0; i < iNumBucks; i++) {

    iX1 = pBuck->Limits.iMinX - iMinX - 1;
    iX2 = pBuck->Limits.iMaxX - iMinX;
    iY1 = pBuck->Limits.iMinY - iMinY - 1;
    iY2 = pBuck->Limits.iMaxY - iMinY;

    iZ1 = pBuck->Limits.iMinZ - iMinZ;
    iZ2 = pBuck->Limits.iMaxZ - iMinZ;
    if(iZ1 >= iZdim) { /* Virtual bucket ? */
      pBuck++;
      continue;
    }
    iZ1--;

    iXsize = iX2 - iX1 + 1;
    iYsize = iY2 - iY1 + 1;
    iZsize = iZ2 - iZ1 + 1;
    iNum = 2 * (iXsize * (iYsize + iZsize) + (iYsize * iZsize));
    piTmpNgbs = NULL;
    if((piTmpNgbs = (int *) calloc(iNum, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for piTmpNgbs[%d] (i = %d).\n",
        pcFName, iNum, i);
      exit(-2); }
    m = 0; /* Number of Bucket Label/Ngbs found */

    /* -- Six faces/slices to visit to generate the entire sausage -- */

    /* 1st: Z = Zmin - 1 = iZ1 , X & Y vary */

    if(iZ1 >= 0) { /* Otherwise: Skip this face */
      iZjump = iZ1 * iSliceSize;

      if(iY1 < 0) iYstart = 0;
      else iYstart = iY1;
      if(iY2 >= iYdim) iYend = iYdim - 1;
      else iYend = iY2;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iYjump = iYstart * iXdim;
      for(j = iYstart; j < iYend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (1).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iYjump += iXdim; /* Next Y : j++ */
      }
    }    

    /* 2nd: Z = Zmax = iZ2 , X & Y vary */

    if(iZ2 < iZdim) { /* Otherwise: Skip this face */
      iZjump = iZ2 * iSliceSize;

      if(iY1 < 0) iYstart = 0;
      else iYstart = iY1;
      if(iY2 >= iYdim) iYend = iYdim - 1;
      else iYend = iY2;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iYjump = iYstart * iXdim;
      for(j = iYstart; j < iYend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (2).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iYjump += iXdim; /* Next Y : j++ */
      }
    }

    /* 3rd: Y = Ymin - 1 = iY1 , X & Z vary */

    if(iY1 >= 0) { /* Otherwise: Skip this face */
      iYjump = iY1 * iXdim;

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (3).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 4th: Y = Ymax = iY2 , X & Z vary */

    if(iY2 < iYdim) { /* Otherwise: Skip this face */
      iYjump = iY2 * iXdim;

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (4).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 5th: X = Xmin - 1 = iX1 , Y & Z vary */

    if(iX1 >= 0) { /* Otherwise: Skip this face */

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iY1 < 0) iYstart = 1;
      else iYstart = iY1 + 1;
      if(iY2 >= iYdim) iYend = iYdim - 2;
      else iYend = iY2 - 1;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iYjump = iYstart * iXdim;
  for(k = iYstart; k <= iYend; k++) {
    iPos = iZjump + iYjump + iX1;
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (5).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iYjump += iXdim; /* Next Y : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 6th: X = Xmax = iX2 , Y & Z vary */

    if(iX2 < iXdim) { /* Otherwise: Skip this face */

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iY1 < 0) iYstart = 1;
      else iYstart = iY1 + 1;
      if(iY2 >= iYdim) iYend = iYdim - 2;
      else iYend = iY2 - 1;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iYjump = iYstart * iXdim;
  for(k = iYstart; k <= iYend; k++) {
    iPos = iZjump + iYjump + iX2;
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (6).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iYjump += iXdim; /* Next Y : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    if(m > iNum || m < 1) { /* At least 3 faces with neighbors */
      fprintf(stderr,
        "ERROR(%s):\n\tWrong number of neighbors found = %d\n",
        pcFName, m);
      fprintf(stderr, "\tat BucketXYZ[%d].\n", i);
      fprintf(stderr, "\tSizes: X = %d, Y = %d, Z = %d\n", iXsize,iYsize,
        iZsize);
      fprintf(stderr, "\tDims: X = %d, Y = %d, Z = %d\n", iXdim,iYdim,iZdim);
      exit(-3);
    }

    /* Set Bucket's list of neighbors */
    pBuck->piNgbs = NULL;
    if((pBuck->piNgbs = (int *) calloc(m, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->piNgbs[%d].\n",
        pcFName, i, m);
  exit(-2); }
    piBuckNgb = pBuck->piNgbs;
    piNgb = piTmpNgbs;
    for(l = 0; l < m; l++) { /* Transfer data */
      iLabel = *piNgb++;
      if(iLabel == i) {
  fprintf(stderr,
    "ERROR(%s): iLabel = %d = BucketXYZ being processed.\n",
    pcFName, iLabel);
  exit(-3); }
      *piBuckNgb++ = iLabel;
    }
    pBuck->isNumNgbs = m;

    pBuck->isNumPair = 0;
    pBuck->isLastItera = 0;

    if(pBuck->isNumGenes > 0) {
      *(pBuck->pucFlagNgb+i) = 1; /* Self-reference */
      piBuckNgb = pBuck->piNgbs;
      for(l = 0; l < m; l++) { /* Transfer data */
  iLabel = *piBuckNgb++;
  *(pBuck->pucFlagNgb+iLabel) = 2; /* Direct neighbor */
      }
    }

    pBuck->piNgbPerItera = NULL;
    if((pBuck->piNgbPerItera =
  (int *) calloc(iMaxItera, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->piNgbPerItera[%d].\n",
        pcFName, i, iMaxItera);
      exit(-2); }
    pBuck->piNgbPerItera[0] = m;

    pBuck->piFstNgb = NULL;
    if((pBuck->piFstNgb =
  (int *) calloc(iMaxItera, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->piFstNgb[%d].\n",
        pcFName, i, iMaxItera);
      exit(-2); }
    pBuck->piFstNgb[0] = 0;

    pBuck->pNgbViz = NULL;
    if((pBuck->pNgbViz =
  (NgbBuck *) calloc(1, sizeof(NgbBuck))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->pNgbViz[1].\n",
        pcFName, i);
      exit(-2); }
    pBuck->isMaxNgbs = 1;
    pBuck->pNgbViz->isBuck = i;
    pBuck->pNgbViz->isBuckFrom = i; /* Self-reference */

    pBuck++;

    free(piTmpNgbs);
    if(m > iMax) iMax = m;
    if(m < iMin) iMin = m;
    iTotal += m;
  } /* Next BucketXYZ : i++ */

  fprintf(stderr, "\tFound neighbors of all %d BucketsXYZ.\n", i);
  fAvg = (float) iTotal / (float) i;
  fprintf(stderr,
    "\tNgbs. per bucket: Max = %d , Min = %d , Avg = %lf .\n",
    iMax, iMin, fAvg);

  return;
}

/*------------------------------------------------------------*\
 * Function: SetBuckNgbsForMBuck
 *
 * Usage: For each MetaBucket's bucket, find its direct
 *    neighbors, using a 3D lookup table to search a
 *    sausage around the bucket.
 *
\*------------------------------------------------------------*/
#if 0
void
SetBuckNgbsForMBuck(MetaBucket *pMBuck)
{
  char    *pcFName = "SetBuckNgbsForMBuck";
  register int  i, j, k, l, m;
  short    *pisLookUp;
  int    iXdim,iYdim,iZdim, iSliceSize, iNumBucks, iMax, iTotal;
  int    iX1,iX2, iY1,iY2, iZ1,iZ2, iZjump,iYjump, iPos, iNum;
  int    iXsize,iYsize,iZsize, iFlagNew, iLabel, iMin;
  int    iXstart,iYstart,iZstart, iXend,iYend,iZend;
  int    *piTmpNgbs, *piNgb, *piBuckNgb;
  MBucket  *pBuck;

  /* Preliminaries */
  iNumBucks = pMBuck->iNumBucks;
  fprintf(stderr, "MESG(%s):\n\tfor %d Buckets....\n", pcFName,
    iNumBucks);

  iXdim = pMBuck->iXdim;
  iYdim = pMBuck->iYdim;
  iZdim = pMBuck->iZdim;
  iSliceSize = pMBuck->iSliceSize;
  iMax = iTotal = 0;
  iMin = iNumBucks;

  /* ---- Process each BucketXYZ ---- */

  pBuck = pMBuck->pBucket;
  pisLookUp = pMBuck->pisLookUp;
  for(i = 0; i < iNumBucks; i++) {

    iX1 = pBuck->Limits.iMinX - 1;
    iX2 = pBuck->Limits.iMaxX;
    iY1 = pBuck->Limits.iMinY - 1;
    iY2 = pBuck->Limits.iMaxY;

    iZ1 = pBuck->Limits.iMinZ;
    iZ2 = pBuck->Limits.iMaxZ;
    if(iZ1 >= iZdim) { /* Virtual bucket ? */
      pBuck++;
      continue;
    }
    iZ1--;

    iXsize = iX2 - iX1 + 1;
    iYsize = iY2 - iY1 + 1;
    iZsize = iZ2 - iZ1 + 1;
    iNum = 2 * (iXsize * (iYsize + iZsize) + (iYsize * iZsize));
    piTmpNgbs = NULL;
    if((piTmpNgbs = (int *) calloc(iNum, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC fails on piTmpNgbs[%d] (i = %d).\n",
        pcFName, iNum, i);
      exit(-2); }
    m = 0; /* Number of Bucket Label/Ngbs found */

    /* -- Six faces/slices to visit to generate the entire sausage -- */

    /* 1st: Z = Zmin - 1 = iZ1 , X & Y vary */

    if(iZ1 >= 0) { /* Otherwise: Skip this face */
      iZjump = iZ1 * iSliceSize;

      if(iY1 < 0) iYstart = 0;
      else iYstart = iY1;
      if(iY2 >= iYdim) iYend = iYdim - 1;
      else iYend = iY2;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iYjump = iYstart * iXdim;
      for(j = iYstart; j < iYend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (1).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iYjump += iXdim; /* Next Y : j++ */
      }
    }    

    /* 2nd: Z = Zmax = iZ2 , X & Y vary */

    if(iZ2 < iZdim) { /* Otherwise: Skip this face */
      iZjump = iZ2 * iSliceSize;

      if(iY1 < 0) iYstart = 0;
      else iYstart = iY1;
      if(iY2 >= iYdim) iYend = iYdim - 1;
      else iYend = iY2;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iYjump = iYstart * iXdim;
      for(j = iYstart; j < iYend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (2).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iYjump += iXdim; /* Next Y : j++ */
      }
    }

    /* 3rd: Y = Ymin - 1 = iY1 , X & Z vary */

    if(iY1 >= 0) { /* Otherwise: Skip this face */
      iYjump = iY1 * iXdim;

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (3).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 4th: Y = Ymax = iY2 , X & Z vary */

    if(iY2 < iYdim) { /* Otherwise: Skip this face */
      iYjump = iY2 * iXdim;

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (4).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 5th: X = Xmin - 1 = iX1 , Y & Z vary */

    if(iX1 >= 0) { /* Otherwise: Skip this face */

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iY1 < 0) iYstart = 1;
      else iYstart = iY1 + 1;
      if(iY2 >= iYdim) iYend = iYdim - 2;
      else iYend = iY2 - 1;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iYjump = iYstart * iXdim;
  for(k = iYstart; k <= iYend; k++) {
    iPos = iZjump + iYjump + iX1;
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (5).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iYjump += iXdim; /* Next Y : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 6th: X = Xmax = iX2 , Y & Z vary */

    if(iX2 < iXdim) { /* Otherwise: Skip this face */

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iY1 < 0) iYstart = 1;
      else iYstart = iY1 + 1;
      if(iY2 >= iYdim) iYend = iYdim - 2;
      else iYend = iY2 - 1;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iYjump = iYstart * iXdim;
  for(k = iYstart; k <= iYend; k++) {
    iPos = iZjump + iYjump + iX2;
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (6).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iYjump += iXdim; /* Next Y : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    if(m > iNum || m < 1) { /* At least 3 faces with neighbors */
      fprintf(stderr,
        "ERROR(%s):\n\tWrong number of neighbors found = %d\n",
        pcFName, m);
      fprintf(stderr, "\tat BucketXYZ[%d].\n", i);
      fprintf(stderr, "\tSizes: X = %d, Y = %d, Z = %d\n", iXsize,iYsize,
        iZsize);
      fprintf(stderr, "\tDims: X = %d, Y = %d, Z = %d\n", iXdim,iYdim,iZdim);
      exit(-3);
    }

    /* Set Bucket's list of neighbors */
    pBuck->piNgbs = NULL;
    if((pBuck->piNgbs = (int *) calloc(m, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->piNgbs[%d].\n",
        pcFName, i, m);
  exit(-2); }
    piBuckNgb = pBuck->piNgbs;
    piNgb = piTmpNgbs;
    for(l = 0; l < m; l++) { /* Transfer data */
      iLabel = *piNgb++;
      if(iLabel == i) {
  fprintf(stderr,
    "ERROR(%s): iLabel = %d = BucketXYZ being processed.\n",
    pcFName, iLabel);
  exit(-3); }
      *piBuckNgb++ = iLabel;
    }
    pBuck->isNumNgbs = m;

    pBuck++;

    free(piTmpNgbs);
    if(m > iMax) iMax = m;
    if(m < iMin) iMin = m;
    iTotal += m;
  } /* Next BucketXYZ : i++ */

#if FALSE
  fprintf(stderr, 
    "MESG(%s):\n\tFound neighbors of all %d BucketsXYZ.\n",
    pcFName, i);
  fAvg = (float) iTotal / (float) i;
  fprintf(stderr,
    "\tMaximum of ngbs. per bucket = %d , Min = %d , Avg = %lf .\n",
    iMax, iMin, fAvg);
#endif

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: SetBuckNgbsForMBucks
 *
 * Usage: For each MetaBucket, find its direct MetaBucket
 *    neighbors, using a 3D lookup table to search a
 *    sausage around the MetaBucket.
 *
\*------------------------------------------------------------*/

void
SetBuckNgbsForMBucks(MetaBuckTop *pMBuckTop)
{
  char    *pcFName = "SetBuckNgbsForMBucks";
  register int  i, j, k, l, m;
  short    *pisLookUp;
  int    iXdim,iYdim,iZdim, iSliceSize, iNumMBucks, iMax, iTotal;
  int    iX1,iX2, iY1,iY2, iZ1,iZ2, iZjump,iYjump, iPos, iNum;
  int    iXsize,iYsize,iZsize, iFlagNew, iLabel, iMin;
  int    iXstart,iYstart,iZstart, iXend,iYend,iZend;
  int    *piTmpNgbs, *piNgb, *piBuckNgb;
  MetaBucket  *pMBuck;

  /* Preliminaries */
  iNumMBucks = pMBuckTop->iNumMetaBucks;
  if(iNumMBucks < 2) {
    fprintf(stderr, "MESG(%s):\n\tonly %d MetaBucket...skip it.\n", pcFName,
      iNumMBucks);
    return;
  }
  fprintf(stderr, "MESG(%s):\n\tfor %d MetaBuckets....\n", pcFName,
    iNumMBucks);

  iXdim = pMBuckTop->iXdim;
  iYdim = pMBuckTop->iYdim;
  iZdim = pMBuckTop->iZdim;
  iSliceSize = pMBuckTop->iSliceSize;
  iMax = iTotal = 0;
  iMin = iNumMBucks;

  /* ---- Process each MetaBucketXYZ ---- */

  pMBuck = pMBuckTop->pMBucks;
  pisLookUp = pMBuckTop->pisLookUp;
  for(i = 0; i < iNumMBucks; i++) {

    iX1 = pMBuck->Limits.iMinX - 1;
    iX2 = pMBuck->Limits.iMaxX;
    iY1 = pMBuck->Limits.iMinY - 1;
    iY2 = pMBuck->Limits.iMaxY;

    iZ1 = pMBuck->Limits.iMinZ;
    iZ2 = pMBuck->Limits.iMaxZ;
    if(iZ1 >= iZdim) { /* Virtual MetaBucket ? */
      pMBuck++;
      continue;
    }
    iZ1--;

    iXsize = iX2 - iX1 + 1;
    iYsize = iY2 - iY1 + 1;
    iZsize = iZ2 - iZ1 + 1;
    iNum = 2 * (iXsize * (iYsize + iZsize) + (iYsize * iZsize));
    piTmpNgbs = NULL;
    if((piTmpNgbs = (int *) calloc(iNum, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC fails on piTmpNgbs[%d] (i = %d).\n",
        pcFName, iNum, i);
      exit(-2); }
    m = 0; /* Number of Bucket Label/Ngbs found */

    /* -- Six faces/slices to visit to generate the entire sausage -- */

    /* 1st: Z = Zmin - 1 = iZ1 , X & Y vary */

    if(iZ1 >= 0) { /* Otherwise: Skip this face */
      iZjump = iZ1 * iSliceSize;

      if(iY1 < 0) iYstart = 0;
      else iYstart = iY1;
      if(iY2 >= iYdim) iYend = iYdim - 1;
      else iYend = iY2;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iYjump = iYstart * iXdim;
      for(j = iYstart; j < iYend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (1).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iYjump += iXdim; /* Next Y : j++ */
      }
    }    

    /* 2nd: Z = Zmax = iZ2 , X & Y vary */

    if(iZ2 < iZdim) { /* Otherwise: Skip this face */
      iZjump = iZ2 * iSliceSize;

      if(iY1 < 0) iYstart = 0;
      else iYstart = iY1;
      if(iY2 >= iYdim) iYend = iYdim - 1;
      else iYend = iY2;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iYjump = iYstart * iXdim;
      for(j = iYstart; j < iYend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (2).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iYjump += iXdim; /* Next Y : j++ */
      }
    }

    /* 3rd: Y = Ymin - 1 = iY1 , X & Z vary */

    if(iY1 >= 0) { /* Otherwise: Skip this face */
      iYjump = iY1 * iXdim;

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (3).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 4th: Y = Ymax = iY2 , X & Z vary */

    if(iY2 < iYdim) { /* Otherwise: Skip this face */
      iYjump = iY2 * iXdim;

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iX1 < 0) iXstart = 0;
      else iXstart = iX1;
      if(iX2 >= iXdim) iXend = iXdim - 1;
      else iXend = iX2;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iPos = iZjump + iYjump + iXstart;
  for(k = iXstart; k < iXend; k++) {
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (4).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iPos++; /* Next X : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 5th: X = Xmin - 1 = iX1 , Y & Z vary */

    if(iX1 >= 0) { /* Otherwise: Skip this face */

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iY1 < 0) iYstart = 1;
      else iYstart = iY1 + 1;
      if(iY2 >= iYdim) iYend = iYdim - 2;
      else iYend = iY2 - 1;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iYjump = iYstart * iXdim;
  for(k = iYstart; k <= iYend; k++) {
    iPos = iZjump + iYjump + iX1;
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (5).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iYjump += iXdim; /* Next Y : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    /* 6th: X = Xmax = iX2 , Y & Z vary */

    if(iX2 < iXdim) { /* Otherwise: Skip this face */

      if(iZ1 < 0) iZstart = 1;
      else iZstart = iZ1 + 1;
      if(iZ2 >= iZdim) iZend = iZdim - 2;
      else iZend = iZ2 - 1;
      if(iY1 < 0) iYstart = 1;
      else iYstart = iY1 + 1;
      if(iY2 >= iYdim) iYend = iYdim - 2;
      else iYend = iY2 - 1;

      iZjump = iZstart * iSliceSize;
      for(j = iZstart; j <= iZend; j++) {
  iYjump = iYstart * iXdim;
  for(k = iYstart; k <= iYend; k++) {
    iPos = iZjump + iYjump + iX2;
    iLabel = (int) pisLookUp[iPos]; /* Bucket label */
    if(iLabel == i) {
      fprintf(stderr,
        "ERROR(%s): iLabel = %d = BucketXYZ being processed (6).\n",
        pcFName, iLabel);
      exit(-2); }
    piNgb = piTmpNgbs;
    iFlagNew = TRUE;
    for(l = 0; l < m; l++) { /* Check for duplicates */
      if(iLabel == *piNgb++) {
        iFlagNew = FALSE; break; }
    }
    if(iFlagNew == TRUE) {
      *piNgb = iLabel;
      m++; /* One more unique neighbor/label found */
    }
    iYjump += iXdim; /* Next Y : k++ */
  }
  iZjump += iSliceSize; /* Next Z : j++ */
      }
    }

    if(m > iNum || m < 1) { /* At least 3 faces with neighbors */
      fprintf(stderr,
        "ERROR(%s):\n\tWrong number of neighbors found = %d\n",
        pcFName, m);
      fprintf(stderr, "\tat BucketXYZ[%d].\n", i);
      fprintf(stderr, "\tSizes: X = %d, Y = %d, Z = %d\n", iXsize,iYsize,
        iZsize);
      fprintf(stderr, "\tDims: X = %d, Y = %d, Z = %d\n", iXdim,iYdim,iZdim);
      exit(-3);
    }

    /* Set MetaBucket's list of neighboring MetaBuckets */
    pMBuck->piNgbs = NULL;
    if((pMBuck->piNgbs = (int *) calloc(m, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC fails on pMBuck[%d]->piNgbs[%d].\n",
        pcFName, i, m);
  exit(-2); }
    piBuckNgb = pMBuck->piNgbs;
    piNgb = piTmpNgbs;
    for(l = 0; l < m; l++) { /* Transfer data */
      iLabel = *piNgb++;
      if(iLabel == i) {
  fprintf(stderr,
    "ERROR(%s): iLabel = %d = BucketXYZ being processed.\n",
    pcFName, iLabel);
  exit(-3); }
      *piBuckNgb++ = iLabel;
    }
    pMBuck->isNumNgbs = m;

    pMBuck++;

    free(piTmpNgbs);
    if(m > iMax) iMax = m;
    if(m < iMin) iMin = m;
    iTotal += m;
  } /* Next MetaBucketXYZ : i++ */

#if FALSE
  fprintf(stderr, 
    "MESG(%s):\n\tFound neighbors of all %d BucketsXYZ.\n",
    pcFName, i);
  fAvg = (float) iTotal / (float) i;
  fprintf(stderr,
    "\tMaximum of ngbs. per bucket = %d , Min = %d , Avg = %lf .\n",
    iMax, iMin, fAvg);
#endif

  return;
}

/*------------------------------------------------------------*\
 * Function: SetBuckSub
 *
 * Usage: For each BucketXYZ, select sub-samples.
 *
\*------------------------------------------------------------*/
#if 0
void
SetBuckSub(Buckets *pBucksXYZ, Dimension *pDim)
{
  char    *pcFName = "SetBuckSub";
  register int  i, j;
  int    iNumSub, iNumSubTot, iNumGene, iDelta, iStep, iNumBucks;
  int    iFull, iTmp;
  int    *piBuckGene, *piBuckSub;
  Bucket  *pBuck;


  iNumSub = pDim->iNumSubSamples;
  iNumSubTot = 0;

  fprintf(stderr, "MESG(%s): Collecting %d sub-samples per bucket...\n",
    pcFName, iNumSub);

  iFull = 0;
  iNumBucks = pBucksXYZ->iNumBucks;
  pBuck = pBucksXYZ->pBucket-1;
  for(i = 0; i < iNumBucks; i++) {
    pBuck++;
    pBuck->piSubGenes = NULL;
    pBuck->isNumSubGenes = 0;
    iNumGene = pBuck->isNumGenes;
    if(iNumGene < 1) continue;

    if(iNumGene > iNumSub) {
      iTmp = iNumSub;
      iDelta = (int) floor((double) iNumGene / (double) iNumSub);
    }
    else {
      iTmp = iNumGene;
      iDelta = 1;
      iFull++;
    }

    if((pBuck->piSubGenes = (int *)
  calloc(iTmp, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->piGenes[%d].\n",
        pcFName, i, iTmp);
      exit(-2); }

    piBuckGene = pBuck->piGenes;
    piBuckSub = pBuck->piSubGenes;
    *piBuckSub = *piBuckGene; /* 1st sub-sample */
    j = 1;
    iStep = 0;
    while(j < iTmp) { /* Collect other sub-samples */
      piBuckSub++;
      iStep += iDelta;
      *piBuckSub = *(piBuckGene+iStep);
      j++;
    }
    pBuck->isNumSubGenes = j;
    iNumSubTot += j;
  }

  fprintf(stderr, "\tCollected a total of %d sub-samples.\n", iNumSubTot);
  fprintf(stderr, "\t%d bucket at full resolution.\n", iFull);

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: ResetBuckFlags
 *
 * Usage: For each BucketXYZ, reset ngb and pairing flags
 *
\*------------------------------------------------------------*/
#if 0
void
ResetBuckFlags(Buckets *pBucksXYZ, Dimension *pDim)
{
  char    *pcFName = "ResetBuckFlags";
  register int  i, j;
  unsigned char  *pucFlag;
  int    iNumGene, iNumBucks, iMaxItera, iTmp;
  Bucket  *pBuck;

  fprintf(stderr, "MESG(%s):...\n", pcFName);

  iMaxItera = pDim->iMaxItera;

  iNumBucks = pBucksXYZ->iNumBucks;
  pBuck = pBucksXYZ->pBucket-1;
  for(i = 0; i < iNumBucks; i++) {
    pBuck++;

    iNumGene = pBuck->isNumGenes;
    if(iNumGene < 1) continue;

    pucFlag = pBuck->pucFlagNgb-1;
    for(j = 0; j < iNumBucks; j++) {
      pucFlag++;
      if(*pucFlag > 2) /* Flag "Other Ngb" set */
  *pucFlag = 0;
    }

    /* Reset to zero Pairing Flags */
    free(pBuck->pucFlagPair);
    pBuck->pucFlagPair = NULL;
    if((pBuck->pucFlagPair =
  (UCHAR *) calloc(iNumBucks, sizeof(UCHAR))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC on pBuck[%d]->pucFlagPair[%d].\n",
        pcFName, i, iNumBucks);
      exit(-2); }

    iTmp = pBuck->piNgbPerItera[0];
    free(pBuck->piNgbPerItera);
    pBuck->piNgbPerItera = NULL;
    if((pBuck->piNgbPerItera =
  (int *) calloc(iMaxItera, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC on pBuck[%d]->piNgbPerItera[%d].\n",
        pcFName, i, iMaxItera);
      exit(-2); }
    pBuck->piNgbPerItera[0] = iTmp;

    free(pBuck->piFstNgb);
    pBuck->piFstNgb = NULL;
    if((pBuck->piFstNgb =
  (int *) calloc(iMaxItera, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->piFstNgb[%d].\n",
        pcFName, i, iMaxItera);
      exit(-2); }
    pBuck->piFstNgb[0] = 0;

    free(pBuck->pNgbViz);
    pBuck->pNgbViz = NULL;
    if((pBuck->pNgbViz =
  (NgbBuck *) calloc(1, sizeof(NgbBuck))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->pNgbViz[1].\n",
        pcFName, i);
      exit(-2); }
    pBuck->isMaxNgbs = 1;
    pBuck->pNgbViz->isBuck = i;
    pBuck->pNgbViz->isBuckFrom = i; /* Self-reference */
    
  }

  /* fprintf(stderr, "\tDone.\n"); */

  return;
}
#endif
/*------------------------------------------------------------*\
 * Function: SetBuckSubQhull
 *
 * Usage: For each BucketXYZ, compute Convex Hull (via qhull)
 *    and then select sub-samples from the vertices of that
 *    Convex Hull.
 *
\*------------------------------------------------------------*/
#if 0
void
SetBuckSubQhull(Buckets *pBucksXYZ, Dimension *pDim)
{
  char    *pcFName = "SetBuckSubQhull";
  register int  i, j;
  int    iNumSub, iNumSubTot, iNumGene, iDelta, iStep, iNumBucks;
  int    iFull, iTmp;
  int    *piBuckGene, *piBuckSub;
  Bucket  *pBuck;


  iNumSub = pDim->iNumSubSamples;
  iNumSubTot = 0;

  fprintf(stderr, "MESG(%s): Collecting %d sub-samples per bucket...\n",
    pcFName, iNumSub);

  iFull = 0;
  iNumBucks = pBucksXYZ->iNumBucks;
  pBuck = pBucksXYZ->pBucket-1;
  for(i = 0; i < iNumBucks; i++) {
    pBuck++;
    pBuck->piSubGenes = NULL;
    pBuck->isNumSubGenes = 0;
    iNumGene = pBuck->isNumGenes;
    if(iNumGene < 1) continue;

    /* If not enough sources, just take all of them */
    if(iNumGene <= iNumSub) {
      iTmp = iNumGene;
      iDelta = 1;
      iFull++;
      if((pBuck->piSubGenes = (int *)
    calloc(iTmp, sizeof(int))) == NULL) {
  fprintf(stderr,
    "ERROR(%s): CALLOC failed for pBuck[%d]->piGenes[%d].\n",
    pcFName, i, iTmp);
  exit(-2); }
      piBuckGene = pBuck->piGenes;
      piBuckSub = pBuck->piSubGenes;
      *piBuckSub = *piBuckGene; /* 1st sub-sample */
      j = 1;
      iStep = 0;
      while(j < iTmp) { /* Collect other sub-samples */
  piBuckSub++;
  iStep += iDelta;
  *piBuckSub = *(piBuckGene+iStep);
  j++;
      }
      pBuck->isNumSubGenes = j;
      iNumSubTot += j;
    }

    /* OK : Compute Qhull */

    iTmp = iNumSub;
    if((pBuck->piSubGenes = (int *)
  calloc(iTmp, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC failed for pBuck[%d]->piGenes[%d].\n",
        pcFName, i, iTmp);
      exit(-2); }

    piBuckGene = pBuck->piGenes;
    piBuckSub = pBuck->piSubGenes;
    *piBuckSub = *piBuckGene; /* 1st sub-sample */
    j = 1;
    iStep = 0;
    while(j < iTmp) { /* Collect other sub-samples */
      piBuckSub++;
      iStep += iDelta;
      *piBuckSub = *(piBuckGene+iStep);
      j++;
    }
    pBuck->isNumSubGenes = j;
    iNumSubTot += j;
  }

  fprintf(stderr, "\tCollected a total of %d sub-samples.\n", iNumSubTot);
  fprintf(stderr, "\t%d bucket at full resolution.\n", iFull);

  return;
}
#endif

/* --- EoF --- */
