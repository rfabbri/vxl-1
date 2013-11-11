/*------------------------------------------------------------*\
 * File: shock_connect.c
 *
 * Functions:
 *  ConnectBlue2RedAndGreen
 *
 * History:
 *  #0: June 2004, by F.Leymarie
 *
\*------------------------------------------------------------*/

#include "shock_connect.h"

/*------------------------------------------------------------*\
 * Function: ConnectBlue2RedAndGreen
 *
 * Usage: Go through all Red vertices and identifies associated
 *    Blue sheets (via Green curves). For each such attached
 *    Blue sheet, adds a pointer to the Red vertex if not
 *    already on list. Repeat with Green curves.
 *
\*------------------------------------------------------------*/

void
ConnectBlue2RedAndGreen(ShockScaffold *pScaffold, InputData *pInData,
      Dimension *pDim)
{
  char    *pcFName = "ConnectBlue2RedAndGreen";
  register int  i, j, k, l;
  int    iNumRed, iNumBlue, iGreen, iBlue, iTmpRed, iFlagNew;
  int    iRed, iMax, iMaxRed, iMinRed, iNumWithNoRed, iNumGreen;
  int    iMaxGreen, iMinGreen, iNumWithNoGreen, iTmpGreen;
  int    *piRedList, *piGreenList;
  ShockSheet  *pBlue;
  ShockCurve  *pGreen;
  ShockVertex  *pRed;

  iNumRed = pScaffold->iNumRedLast;
  iNumGreen = pScaffold->iGreenActiveLast;
  iNumBlue = pScaffold->iBlueActiveLast;
  fprintf(stderr,
    "\nMESG(%s):\n\tStart by processing %d Red vertices...\n",
    pcFName, iNumRed);

  pRed = pScaffold->pRed-1;
  for(i = 0; i < iNumRed; i++) {
    pRed++;
    for(j = 0; j < 4; j++) {
      iGreen = pRed->iGreen[j];
      if(iGreen < 0) continue;
      pGreen = pScaffold->pGreen+iGreen;
      for(k = 0; k < 3; k++) {
  iBlue = pGreen->iBlue[k];
  pBlue = pScaffold->pBlue+iBlue;
  /* if(pBlue->isBounded) continue; */
  iTmpRed = (int) pBlue->isNumRed;
  if(iTmpRed > 0) {
    piRedList = pBlue->piRedList;
    iFlagNew = TRUE;
    for(l = 0; l < iTmpRed; l++) {
      iRed = *piRedList++;
      if(iRed == i) {
        iFlagNew = FALSE;
        break;
      }
    }
    if(!iFlagNew) continue;
    /* Else: found a new Red vertex for Blue sheet iBlue */
    iMax = iTmpRed + 1;
    if((pBlue->piRedList =
        (int *) realloc((int *) pBlue->piRedList,
            iMax * sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): ReAlloc on pBlue(%d)->piRedList[%d].\n",
        pcFName, iBlue, iMax);
      exit(-4); }
    pBlue->piRedList[iTmpRed] = i;
    pBlue->isNumRed = (short) iMax;
  }
  else { /* 1st Red vertex for Blue sheet iBlue */ 
    pBlue->piRedList = NULL;
    if((pBlue->piRedList = (int *)
        calloc(1, sizeof(int))) == NULL) {
      fprintf(stderr,
        "ERROR(%s): CALLOC fails on pBlue[%d]->piRedList[1].\n",
        pcFName, iBlue);
      exit(-4); }
    pBlue->piRedList[0] = i;
    pBlue->isNumRed = 1;
  }
  
      } /* Next Blue sheet for Green j^th curve iGreen : k++ */

    } /* Next Green curve for i^th Red vertex : j++ */
    
  } /* Next Red vertex: i++ */

  fprintf(stderr, "\tNow processing %d Green curves...\n", iNumGreen);

  pGreen = pScaffold->pGreen-1;
  for(i = 0; i < iNumGreen; i++) {
    pGreen++;
    for(j = 0; j < 3; j++) {
      iBlue = pGreen->iBlue[j];
      if(iBlue < 0) continue;
      pBlue = pScaffold->pBlue+iBlue;
      iTmpGreen = (int) pBlue->isNumGreen;
      if(iTmpGreen > 0) {
  piGreenList = pBlue->piGreenList;
  iFlagNew = TRUE;
  for(k = 0; k < iTmpGreen; k++) {
    iGreen = *piGreenList++;
    if(iGreen == i) {
      iFlagNew = FALSE;
      break;
    }
  }
  if(!iFlagNew) continue;
  /* Else: found a new Green curve for Blue sheet iBlue */
  iMax = iTmpGreen + 1;
  if((pBlue->piGreenList =
      (int *) realloc((int *) pBlue->piGreenList,
          iMax * sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): ReAlloc on pBlue(%d)->piGreenList[%d].\n",
      pcFName, iBlue, iMax);
    exit(-4); }
  pBlue->piGreenList[iTmpGreen] = i;
  pBlue->isNumGreen = (short) iMax;
      }
      else { /* 1st Green curve for Blue sheet iBlue */ 
  pBlue->piGreenList = NULL;
  if((pBlue->piGreenList = (int *)
      calloc(1, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC fails on pBlue[%d]->piGreenList[1].\n",
      pcFName, iBlue);
    exit(-4); }
  pBlue->piGreenList[0] = i;
  pBlue->isNumGreen = 1;
      }

    } /* Next Blue sheet for i^th green curve : j++ */

  } /* Next Green curve: i++ */

  fprintf(stderr, "\tNow checking %d Blue sheets...\n", iNumBlue);
  iMinRed = iMinGreen = 10000;
  iMaxRed = iMaxGreen = iNumWithNoRed = iNumWithNoGreen = 0;
  pBlue = pScaffold->pBlue-1;
  for(i = 0; i < iNumBlue; i++) {
    pBlue++;
    iTmpRed = (int) pBlue->isNumRed;
    iTmpGreen = (int) pBlue->isNumGreen;
    if(iTmpRed < iMinRed) iMinRed = iTmpRed;
    if(iTmpRed > iMaxRed) iMaxRed = iTmpRed;
    if(iTmpRed < 1) {
      iNumWithNoRed++;
#if FALSE
      if(iTmpGreen > 1) {
  fprintf(stderr,
    "WARNING: Blue sheet %d has no Red vertex, but %d > 1 Green curves.\n",
    i, iTmpGreen);
      }
#endif
    }
    if(iTmpGreen < iMinGreen) iMinGreen = iTmpGreen;
    if(iTmpGreen > iMaxGreen) iMaxGreen = iTmpGreen;
    if(iTmpGreen < 1) {
      iNumWithNoGreen++;
      if(iTmpRed > 1) {
  fprintf(stderr,
    "WARNING: Blue sheet %d has no Green curve, but %d > 1 Red vertices.\n",
    i, iTmpRed);
      }
    }
  }

  fprintf(stderr,
    "\tRed vertices per Blue sheet: (min,max) = (%d , %d)\n",
    iMinRed, iMaxRed);
  fprintf(stderr,
    "\tGreen curves per Blue sheet: (min,max) = (%d , %d)\n",
    iMinGreen, iMaxGreen);
  if(iNumWithNoRed > 0) {
    fprintf(stderr,
      "\t There are %d Blue sheets with no Red vertex.\n",
      iNumWithNoRed);
  }
  if(iNumWithNoGreen > 0) {
    fprintf(stderr,
      "\t There are %d Blue sheets with no Green curve.\n",
      iNumWithNoGreen);
  }

#if FALSE
  /* ======== Generate boundary for each Blue sheet ========== */

  piGreenListUsed = NULL;
  if((piGreenListUsed = (int *)
      calloc(iMaxGreen, sizeof(int))) == NULL) {
    fprintf(stderr,
      "ERROR(%s): CALLOC on piGreenListUsed[%d].\n",
      pcFName, iMaxGreen);
    exit(-4); }

  pBlue = pScaffold->pBlue-1;
  for(i = 0; i < iNumBlue; i++) {
    pBlue++;
    iTmpRed = (int) pBlue->isNumRed;
    iTmpGreen = (int) pBlue->isNumGreen;
    if(iTmpGreen < 1) { /* No boundary */
      pBlue->isNumBoundArcs = 0;
      pBlue->isBounded = FALSE;
      continue;
    }

    piGreenList = pBlue->piGreenList;
    for(j = 0; j < iTmpGreen; j++) {
      /* Propagate a boundary arc from a Green curve */
      iFlag = piGreenListUsed[j];
      if(iFlag) continue;
      piGreenListUsed[j] = TRUE;

      iGreen = *piGreenList++;
      pGreen = pScaffold->pGreen+iGreen;

      

    } /* Next Green curve on list: j++ */


  } /* Next Blue sheet: i++ */
#endif


  return;
}

/* --- EoF --- */

