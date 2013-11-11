/*------------------------------------------------------------*\
 * File: fetch_shocks_fst_layer.c
 *
 * Functions:
 *  FetchBlueShocksExtFst
 *  FetchShocksExtFst
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie
 *  #1: July 2003: Upgraded for use with MetaBuckets and
 *    shock validation via voxels.
 *
\*------------------------------------------------------------*/

#include "fetch_shocks_fst_layer.h"
#include "blue_shocks_utils.h"
#include "fetch_green_shocks.h"
#include "fetch_red_shocks.h"

/*------------------------------------------------------------*\
 * Function: FetchBlueShocksExtFst
 *
 * Usage: Seek all isolated Sheet shocks - some are 
 *    part of the MA and thus, the source of a shock sheet,
 *    others are only part of the SS - BETWEEN buckets.
 *    This represent the FIRST LAYER of neighboring buckets
 *    "around" each bucket.
 *    Perform computations for ALL sources.
 *
\*------------------------------------------------------------*/

void
FetchBlueShocksExtFst(InputData *pInData, ShockScaffold *pScaf,
          short *pisLookUp, Buckets *pBucksXYZ,
          Pt3dCoord *pGeneCoord, Voxel *pVoxels,
          Dimension *pDimMBuck, Dimension *pDimAll)
{
   char    *pcFName = "FetchBlueShocksExtFst";
   register int  i, j, k, l, m;
   unsigned int  iNum, iNumBucks, iAcquire, iAcquireTotal, iMax;
   int    iNumShock, iMaxShock, iNumInvalid, iXdim;
   int    iPos, iPosXs,iPosYs,iPosZs, iNumGeneViz, iGeneA,iGeneB, iNew;
   int    iSliceSize, iNumGeneInBuckA,iNumGeneInBuckB, iNumNgbs, iCorner;
   int    iFlagVisible, iAdrNgb, iNumShockExternal, iNumNotVisible;
   int    iNewBlue, iItera, iIteraNext, iLabelBuckB, iNumNgbNew;
   int    iFlagVisibleBuckB, iNumGeneASeeingB, iNgbNew, iGeneViz;
   int    iNumGeneVizAdded, iFlagAcquire, iNewGeneVizToAdd;
   int    iFlagVisibleGene, iNumShockInit;
   int    *piGeneInBuckA,*piGeneInBuckB, *piNgbNew,*piNewNgbTmp;
   int    *piGeneASeeingBTmp, *piGeneVizTmp, *piTmp,*piTmpB;
   int    *piOldGeneViz;
   static double  fEpsilon = (double) D_EPSILON;
   double    fPosXa,fPosYa,fPosZa, fPosXb,fPosYb,fPosZb, fPx,fPy,fPz;
   double    fPosXs,fPosYs,fPosZs, fPABx,fPABy,fPABz, fDSq, fDist;
   double    fTimeSys, fTimeUser, fMemUse, fTmpSys, fTmpUser;
   double    fPxMin,fPyMin,fPzMin, fPxMax,fPyMax,fPzMax, fDistSqLargest;
   InputData  *pDataA, *pDataB;
   Pt3dCoord  *pGeneA, *pGeneB;
   Vector3d  *pvNorm;
   ShockSheet  *pBlue;
   Bucket  *pBuckA, *pBuckB;
   NgbBuck  *pNgbVizNew;
   ShockData  *pShockData;

   /* Preliminaries */
   
   pShockData = NULL;
   if((pShockData = (ShockData*)
       calloc(1, sizeof(ShockData))) == NULL) {
     fprintf(stderr, "ERROR(%s): CALLOC fails on pShockData[1].\n",
         pcFName);
     exit(-4); }
   pShockData->iGene[2] = pShockData->iGene[3] = IDLE;
   
   fprintf(stderr, "MESG(%s):\n", pcFName);
   iXdim = pDimMBuck->iXdim;
   iSliceSize = pDimMBuck->iSliceSize;
   iNumBucks = pBucksXYZ->iNumBucks;
   
   iNumShockInit = iNumShock = pScaf->iBlueActiveLast;
   pBlue = (pScaf->pBlue)+iNumShock;
   /* iMaxShock = pDim->iMaxBlueShocks; */
   iMaxShock = pScaf->iNumBlueMax;
   iNumInvalid = 0;
   
   fDistSqLargest = 0.0;
   
   /* --- Allocate Memory for Temporary structures --- */
   
   /* Will be used to grow ngb. in the main while loop */
   piNewNgbTmp = NULL;
   if((piNewNgbTmp = (int *)
       calloc(iNumBucks, sizeof(int))) == NULL) {
     fprintf(stderr, "ERROR(%s): CALLOC on piNewNgbTmp[%d].\n",
         pcFName, iNumBucks);
     exit(-2); }
   /* Set Memory to Collect Visible Sources per BucketB per Iteration */
   iMax = pBucksXYZ->iMaxGenes;
   piGeneASeeingBTmp = NULL;
   if((piGeneASeeingBTmp = (int *) calloc(iMax, sizeof(int))) == NULL) {
     fprintf(stderr,
         "ERROR(%s): CALLOC on piGeneASeeingBTmp[%d].\n",
         pcFName, iMax);
     exit(-2); }
   /* Set Memory to Collect Additional Visible Genes for Ngb */
   piGeneVizTmp = NULL;
   if((piGeneVizTmp = (int *) calloc(iMax, sizeof(int))) == NULL) {
     fprintf(stderr, "ERROR(%s): CALLOC on piGeneVizTmp[%d].\n",
         pcFName, iMax);
     exit(-2); }
   
   /* ---------------- Grow BucketsXYZ Neighborhoods  --------------- *\
    *    Seek "external" pairs from generators.
   \*    Use Visibility Constraints to limit search.       */
   
   iNewBlue = iNumNotVisible = iNumGeneVizAdded = 0;
   iItera = 0;
   iIteraNext = 1;
   
   /* -------------------- IteraNext 1 ------------------------ *\
   \* --------- Visit immediate ngbs of each Bucket A --------- */
   
   iAcquireTotal = 0;
   pBuckA = pBucksXYZ->pBucket-1;
   for(i = 0; i < iNumBucks; i++) { /* -- For each Bucket (A) -- */
      pBuckA++;
   
      iNumGeneInBuckA = pBuckA->isNumGenes;
      if(iNumGeneInBuckA < 1) {
         continue; 
      }
   
      iAcquire = 0;
      iNumNgbs = pBuckA->isNumNgbs;
   
      /* ------- Go through the list of Direct Ngb Buckets B ------- */
   
      for(j = 0; j < iNumNgbs; j++) {
         iLabelBuckB = *(pBuckA->piNgbs+j);
         pBuckB = pBucksXYZ->pBucket+iLabelBuckB;
         iNumGeneInBuckB = pBuckB->isNumGenes;
   
         if(iNumGeneInBuckB < 1) /* No Pairing needed */
           *(pBuckA->pucFlagPair+iLabelBuckB) = 2;
   
         iFlagVisibleBuckB = FALSE;
         /* Remains FALSE only if Non Visible from All GeneA */
   
         /* ------- For each ngb. BuckB, Go through the ------- *\
         \*   list of GeneA which all see this BuckB       */
   
         iNumGeneASeeingB = 0;
         piGeneInBuckA = pBuckA->piGenes;
         for(k = 0; k < iNumGeneInBuckA; k++) {
   
           iGeneA = *piGeneInBuckA++;
           pDataA = pInData+iGeneA;
           pGeneA = &(pDataA->Sample);
           fPosXa = pGeneA->fPosX;
           fPosYa = pGeneA->fPosY;
           fPosZa = pGeneA->fPosZ;
         
           /* -------- 1st Check Visibility of BucketB w/r GeneA --------- *\
            *    We only check if BucketB is ENTIRELY in the dead zone     *
            *    of a SINGLE Blue shock of GeneA. A more sophisticated     *
            *    solution would require to slice iteratively BucketB via   *
            *    each half-space and keep track of the remaining visible   *
            *    boundaries of the  bucket: these would be intercepts of    *
           \*    the slicing planes with the edges of bucket.    */
         
           iFlagVisible = TRUE;
         
           fPxMin = pBuckB->Limits.iMinX - fPosXa;
           fPyMin = pBuckB->Limits.iMinY - fPosYa;
           fPzMin = pBuckB->Limits.iMinZ - fPosZa;
           fPxMax = pBuckB->Limits.iMaxX - fPosXa;
           fPyMax = pBuckB->Limits.iMaxY - fPosYa;
           fPzMax = pBuckB->Limits.iMaxZ - fPosZa;
         
           pvNorm = pDataA->pvNormal-1;
           /* Number of Valid Blue Shocks thus far */
           iNum = pDataA->isNumBlue;
         
           /* For each known Blue Shock of GeneA */
           for(l = 0; l < iNum; l++) {
               pvNorm++;
               iCorner = 0; /* Label to 8 corners of Bucket */
               for(m = 0; m < 8; m++) { /* For each corner */
                  switch(m) { /* Each of the 8 corners */
                  case 0: /* Pk = (MinX , MinY , MinZ) */
                    fPx = fPxMin;
                    fPy = fPyMin;
                    fPz = fPzMin;
                    break;
                  case 1: /* Pk = (MinX , MinY , MaxZ) */
                    fPz = fPzMax;
                    break;
                  case 2: /* Pk = (MinX , MaxY , MaxZ) */
                    fPy = fPyMax;
                    break;
                  case 3: /* Pk = (MinX , MaxY , MinZ) */
                    fPz = fPzMin;
                    break;
                  case 4: /* Pk = (MaxX , MaxY , MinZ) */
                    fPx = fPxMax;
                    break;
                  case 5: /* Pk = (MaxX , MaxY , MaxZ) */
                    fPz = fPzMax;
                    break;
                  case 6: /* Pk = (MaxX , MinY , MaxZ) */
                    fPy = fPyMin;
                    break;
                  case 7: /* Pk = (MaxX , MinY , MinZ) */
                    fPz = fPzMin;
                    break;
                  }
                  
                  fDist = pvNorm->fX*fPx + pvNorm->fY*fPy + pvNorm->fZ*fPz
                    - pvNorm->fLengthSq;
                  if(fDist > 0.0) { /* Above plane ? */
                    iCorner++;
                  } /* Maybe use an Epsilon here or Robust arithm. */
                  else {   /* At least 1 corner of BuckB is visible from GeneA */
                    break; /* Get out of For(m) loop */
                  }
               } /* Next corner : m++ */
   
              if(iCorner > 7) {
                 iFlagVisible = FALSE;
                 break; /* Get out of For(l) loop */
              }
           } /* Next shock associated to GeneA : l++ */
   
           if(iFlagVisible) { /* OK: BuckB is Visible from GeneA */
              iFlagVisibleBuckB = TRUE;
   
              /* Keep track of GeneA seeing BuckB */
              *(piGeneASeeingBTmp+iNumGeneASeeingB) = iGeneA;
              iNumGeneASeeingB++;
   
               /* ----- Check PAIRINGS if there are genes in B ----- *\
               \* ----- and pairing with A not done yet        ----- */
               
               if(iNumGeneInBuckB > 0 && *(pBuckB->pucFlagPair+i) < 1) {
                  piGeneInBuckB = pBuckB->piGenes;
                  for (l = 0; l < iNumGeneInBuckB; l++) {
                     iGeneB = *piGeneInBuckB++;
                     pDataB = pInData+iGeneB;
                     pGeneB = &(pDataB->Sample);
                     fPosXb = pGeneB->fPosX;
                     fPosYb = pGeneB->fPosY;
                     fPosZb = pGeneB->fPosZ;
                     
                     fPABx = fPosXb - fPosXa;
                     fPABy = fPosYb - fPosYa;
                     fPABz = fPosZb - fPosZa;
                     
                     /* --- 1st: Make sure GeneB sees GeneA --- */
                     
                     pvNorm = pDataA->pvNormal-1;
                     /* Number of Valid Blue Shocks thus far */
                     iNum = pDataA->isNumBlue;
                     
                     iFlagVisibleGene = TRUE;
                     /* For each known Blue Shock of GeneA */
                     for(m = 0; m < iNum; m++) {
                       pvNorm++;
                       fDist = pvNorm->fX*fPABx + pvNorm->fY*fPABy +
                       pvNorm->fZ*fPABz - pvNorm->fLengthSq;
                       /* if(fDist > 0.0) {  Above plane ? */
                       if(fDist > fEpsilon) { /* Above plane ? */
                          iFlagVisibleGene = FALSE;
                          break; /* Get out of For(m) Loop */
                       } /* Maybe use an Epsilon here or Robust arithm. */
                     } /* m++ */
                     if(!iFlagVisibleGene)
                       continue; /* GeneA does not see GeneB : Goto next GeneB: l++ */
               
                     /* -- Make sure this sheet does not already exist -- *\
                     *    We need to compare only with sheets found at   *
                     \*    Sub. Res. & linked to srcs, via backpointers.  */
                     /* NOT DONE */
               
                     /* --- 2nd: GeneB sees GeneA : Construct Pair/Shock -- */
               
                     fDSq  = (fPABx*fPABx + fPABy*fPABy + fPABz*fPABz);
                     fDSq *= 0.25;  /* Divided by half^2 */
               
                     fPosXs = 0.5 * (fPosXa + fPosXb);
                     fPosYs = 0.5 * (fPosYa + fPosYb);
                     fPosZs = 0.5 * (fPosZa + fPosZb);
#if HIGH_MEM
                    pBlue->fDist2GeneSq = fDSq;
                    /* Potential Shock coords. */
                    pBlue->MidPt.fPosX = fPosXs;
                    pBlue->MidPt.fPosY = fPosYs;
                    pBlue->MidPt.fPosZ = fPosZs;
#endif
                     /* -------------------- VALIDATE --------------------- */
                     
                     iPosXs = (int) floor((double) fPosXs);
                     iPosYs = (int) floor((double) fPosYs);
                     iPosZs = (int) floor((double) fPosZs);
                     
                     /* Find Bucket where shock is located */
                     iPos = iPosZs * iSliceSize + iPosYs * iXdim + iPosXs;
                     pBlue->iBuckXYZ = *(pisLookUp+iPos);
                     
                     pShockData->iGene[0] = pBlue->iGene[0] = iGeneA;
                     pShockData->iGene[1] = pBlue->iGene[1] = iGeneB;
                     
                     pShockData->fDist2GeneSq = fDSq;
                     pShockData->ShockCoord.fPosX = fPosXs;
                     pShockData->ShockCoord.fPosY = fPosYs;
                     pShockData->ShockCoord.fPosZ = fPosZs;
                     
                     if(!ValidBlueShockViaVoxels(pShockData, pInData, pGeneCoord,
                      pVoxels, pDimAll)) {
                      iNumInvalid++;
                      continue; /* Part of SS only: Goto next pair : j++ */
                    }

                    /* ----- Found a New Valid Sheet Shock ----- */

                    if(fDSq > fDistSqLargest) fDistSqLargest = fDSq;
                  
                    pBlue->isFlagValid = TRUE;
                    pBlue->isInitType  = BLUE_INIT;
                    pBlue->isStatus    = ACTIVE;
                    pBlue->iGreen      = IDLE;
                    pBlue->iRed        = IDLE;
                    pBlue->isItera     = iIteraNext;
                  
                    /* -- Set Gene Data: Backpointers to Shock and Normal data -- *\
                     *    We do this immediately, so that visibility constraints  *
                    \*    are available immediately to rule out other candidates. */
                  
                    /* Put Info on GeneA */
                  
                    iNum = pDataA->isNumBlue;
                    iMax = pDataA->isMaxBlue - 2;
                    if(iNum > iMax) {
                      iMax = (iNum+1) * 2;
                      if((pDataA->piBlueList =
                          (int *) realloc((int *) pDataA->piBlueList,
                              iMax * sizeof(int))) == NULL) {
                        fprintf(stderr,
                          "ERROR(%s): ReAlloc(pDataA+%d)->piBlueList[%d]\n",
                          pcFName, iGeneA, iNum);
                        exit(-3); }
                      if((pDataA->pvNormal =
                          (Vector3d *) realloc((Vector3d *) pDataA->pvNormal,
                             iMax * sizeof(Vector3d))) == NULL) {
                        fprintf(stderr,
                          "ERROR(%s): ReAlloc(pDataA+%d)->pvNormal[%d]\n",
                          pcFName, iGeneA, iNum);
                        exit(-3); }
                      pDataA->isMaxBlue = iMax;
                      /* pDataA->isMaxNorm = iMax; */
                    }
                    /* Backpointer to shock */
                    pDataA->piBlueList[iNum] = iNumShock;
                  
                    pDataA->isNumBlue++;
                    pvNorm = pDataA->pvNormal+iNum;
                    pvNorm->fX = fPABx;
                    pvNorm->fY = fPABy;
                    pvNorm->fZ = fPABz;
                    pvNorm->fLengthSq = fDSq * 4.0;
                    /* pDataA->isNumNorm++; */
                  
                    /* Put Info on GeneB */
                  
                    iNum = pDataB->isNumBlue;
                    iMax = pDataB->isMaxBlue - 2;
                    if(iNum > iMax) {
                      iMax = (iNum+1) * 2;
                      if((pDataB->piBlueList =
                          (int *) realloc((int *) pDataB->piBlueList,
                              iMax * sizeof(int))) == NULL) {
                        fprintf(stderr,
                          "ERROR(%s): ReAlloc(pDataB+%d)->piBlueList[%d]\n",
                          pcFName, iGeneB, iNum);
                        exit(-3); }
                      if((pDataB->pvNormal =
                          (Vector3d *) realloc((Vector3d *) pDataB->pvNormal,
                             iMax * sizeof(Vector3d))) == NULL) {
                        fprintf(stderr,
                          "ERROR(%s): ReAlloc(pDataB+%d)->pvNormal[%d].\n",
                          pcFName, iGeneB, iNum);
                        exit(-3); }
                      pDataB->isMaxBlue = iMax;
                      /* pDataB->isMaxNorm = iMax; */
                    }

                     /* Backpointer to shock */
                     pDataB->piBlueList[iNum] = iNumShock;
                  
                     pDataB->isNumBlue++;
                     pvNorm = pDataB->pvNormal+iNum;
                     pvNorm->fX = -fPABx;
                     pvNorm->fY = -fPABy;
                     pvNorm->fZ = -fPABz;
                     pvNorm->fLengthSq = fDSq * 4.0;
                     /* pDataB->isNumNorm++; */
                  
                     iNumShock++;
                     iNewBlue++;
                     pBlue++;

                    if(iNumShock >= iMaxShock) {
                      fprintf(stderr, "ERROR(%s): Overflow: Blue Shocks = %d >= Max = %d\n",
                              pcFName, iNumShock, iMaxShock);
                      exit(-5);
                    }

                 } /* Goto next 2nd gene in BuckB to create a new pair: l++ */
              }
              /* else : BuckB contains No gene or Has been paired to A already */
              /* break; Get out of Loop(k) */
           } /* BuckB is visible from GeneA */
           /* else : BuckB is NOT visible from GeneA    *\
             *    We are not using this information *
            \*    later, but maybe we should/could. */

         } /* Goto next GeneA: Check Viz w/r B, then test new Pairs: k++ */

         /* ------------ All Genes of BuckA visited -------------- *\
          *     If BuckB is Not Visible from ALL GeneA, we do NOT  *
          *     acquire new bucket neighbors linked to it.    *
          *     If it is visible, we try to acquire new neighbors. *
          *     In both cases, we need to raise a flag for BuckB   *
         \*     to Avoid Pairing it latter with BuckA again.  */

         if(!iFlagVisibleBuckB) {
           iNumNotVisible++;

           *(pBuckA->pucFlagViz+iLabelBuckB)  = 2; /* B Not viz from A */
           *(pBuckA->pucFlagPair+iLabelBuckB) = 2; /* Pair not needed */

           if(iNumGeneInBuckB > 0) {
              *(pBuckB->pucFlagPair+i) = 2; /* No needs for Pairing with A */
              /* WARNING: Following is not absolutely true */
              /* *(pBuckB->pucFlagViz+i) = 2; BuckA not viz from B */
           }
           continue; /* Go check next ngb. Bucket B : j++ */
         }

         /* ------- Else: BuckB is visible from at least 1 GeneA ------- *\
         \*   Acquire new Ngbs & transmit genes visible        */
         
         *(pBuckA->pucFlagViz+iLabelBuckB)  = 1; /* BuckB viz from A */
         *(pBuckA->pucFlagPair+iLabelBuckB) = 2; /* Pairings done */
         pBuckA->isNumPair++;
         if(iNumGeneInBuckB > 0) {
           /* No needs for Pairing with A again */
           *(pBuckB->pucFlagPair+i) = 2;
         }

         /* -- Check immediate neighbors of BuckB to propagate further -- */

         piNgbNew = pBuckB->piNgbs;
         iNumNgbNew = pBuckB->isNumNgbs;
         iNew = 0;
         for(k = 0; k < iNumNgbNew; k++) {
           iNgbNew = *piNgbNew++;
           if(iNgbNew == i) 
               continue; /* Don't go back to A : k++ */

        /* Position in pNgbViz */
        iAdrNgb = *(pBuckA->piPtrNgb+iNgbNew);
        if(iAdrNgb > 0 && /* Not one of the nearest ngb. */
           *(pBuckA->pucFlagNgb+iNgbNew) > 0 && /* Been there */
           *(pBuckA->pucFlagPair+iNgbNew) < 1) { /* But no pair yet */
           /* Then check for New Viz Genes */
      
           pNgbVizNew = pBuckA->pNgbViz+iAdrNgb;
           iNumGeneViz = pNgbVizNew->isNumGeneViz;
      
           iNewGeneVizToAdd = 0;
           if(iNumGeneViz < iNumGeneInBuckA) {
              piTmpB = piGeneVizTmp;
              piTmp = piGeneASeeingBTmp;
              for(l = 0; l < iNumGeneASeeingB; l++) {
                 iGeneViz = *piTmp++;
                 iFlagAcquire = TRUE;
                 piOldGeneViz = pNgbVizNew->piGenes;
                 for(m = 0; m < iNumGeneViz; m++) {
                   if(iGeneViz == *piOldGeneViz++) {
                      iFlagAcquire = FALSE;
                      break; 
                     } /* Get out of For(m) loop */
                 } /* m++ */
                 if(iFlagAcquire) {
                   *piTmpB++ = iGeneViz;
                   iNewGeneVizToAdd++;    
                 }
              } /* l++ */
           }

           if(iNewGeneVizToAdd > 0) { /* New Viz GeneA to add */

              iMax = iNumGeneViz + iNewGeneVizToAdd;
              if((pNgbVizNew->piGenes =
                   (int *) realloc((int *) pNgbVizNew->piGenes,
                 iMax * sizeof(int))) == NULL) {
                 fprintf(stderr, "ERROR(%s): ReAlloc(pNgbVizNew->piGenes[%d])\n",
                         pcFName, iMax);
                 exit(-3); 
               }
              pNgbVizNew->isNumGeneViz = iMax;

              piTmpB = piGeneVizTmp;
              piTmp = pNgbVizNew->piGenes+iNumGeneViz;
              for(l = 0; l < iNewGeneVizToAdd; l++) {
                 *piTmp++ = *piTmpB++;
              }

              pNgbVizNew->isNumVisit++;
              iNumGeneVizAdded++;
           }

           continue; /* k++ */
        }

        if(*(pBuckA->pucFlagNgb+iNgbNew) == 0) {
           /* Not Been there from BuckA yet */
           /* -- OK : Found a New Ngb bucket to acquire -- */
           *(piNewNgbTmp+iNew) = iNgbNew;
           iNew++;
        }

      } /* Check next surrounding ngb. : k++ */

      /* --- Found New neighbors ? Acquire these for next layer --- */

      if(iNew > 0) {

        iAdrNgb = pBuckA->isMaxNgbs;
        iMax = iAdrNgb + iNew;
        if((pBuckA->pNgbViz =
           (NgbBuck *) realloc((NgbBuck *) pBuckA->pNgbViz,
              iMax * sizeof(NgbBuck))) == NULL) {
        fprintf(stderr,
          "ERROR(%s): ReAlloc(pBuckA->pNgbViz[%d])\n",
          pcFName, iMax);
        fprintf(stderr,
          "\tfor Bucks A=%d, B=%d, iItera = %d, NewNgbs = %d\n",
          i, iLabelBuckB, iItera, iNew);
        exit(-3); 
         }
        pBuckA->isMaxNgbs = iMax;

        pNgbVizNew = pBuckA->pNgbViz+iAdrNgb;
        for(k = 0; k < iNew; k++) {
           iNgbNew = *(piNewNgbTmp+k);    
           *(pBuckA->pucFlagNgb+iNgbNew) = 3;
           *(pBuckA->piPtrNgb+iNgbNew) = iAdrNgb;
            
           /* For each such new Ngb, transmit list of visible GeneA */
            
           if((pNgbVizNew->piGenes = (int *)
               calloc(iNumGeneASeeingB, sizeof(int))) == NULL) {
             fprintf(stderr,
                 "ERROR(%s): CALLOC on pNgbVizNew->piGenes[%d].\n",
                 pcFName, iNumGeneASeeingB);
             exit(-2); }
            
           piTmp = piGeneASeeingBTmp;
           for(l = 0; l < iNumGeneASeeingB; l++) {
              /* Keep track of sources */
              *(pNgbVizNew->piGenes+l) = *piTmp++;
           }
           pNgbVizNew->isNumGeneViz = iNumGeneASeeingB;
           pNgbVizNew->isBuck = iNgbNew;
           pNgbVizNew->isBuckFrom = iLabelBuckB;
           pNgbVizNew->isItera = iIteraNext;
           pNgbVizNew->isNumVisit = 1;
            
           pNgbVizNew++;
           iAcquire++; iAdrNgb++;
        } /* Acquire next New Ngb : k++ */

      } /* Acquired New Ngbs. From BuckB */

   } /* Goto next Neighboring BuckB to visit : j++ */

   /* -- All ngb. BuckB at the Initial Iteration are visited for BuckA -- */

   pBuckA->piNgbPerItera[1] = iAcquire; /* Ngb. newly acquired */
   if(iAcquire > 0) 
      pBuckA->piFstNgb[1] = 1; /* 1st Ngb. newly acquired */
   iAcquireTotal += iAcquire;

   } /* Goto next BuckA for this Initial iteration : i++ */

   /* ---------------- Initial Iteration Done ---------------------- */

   fprintf(stderr, "\t**** FIRST LAYER processed for pairs ****\n");


   iNumShockExternal = iNumShock - iNumShockInit;
   pDimMBuck->iNumShockSheetExtAll = iNumShockExternal;
   pDimMBuck->iNumShockSheet = iNumShock;
   /* pDimMBuck->iNumShockSheetV = iNumShock; */

   if(iNumShockExternal != iNewBlue) {
      fprintf(stderr, "ERROR(%s): iNumShockExternal (%d) != iNewBlue (%d).\n",
              pcFName, iNumShockExternal, iNewBlue);
      exit(-4);
   }
   pScaf->iNumBlueActive += iNewBlue;
   pScaf->iBlueActiveLast += iNewBlue;
   pScaf->iNumBlueIsolated += iNewBlue;
   pScaf->iNumBlueFst = iNewBlue;
   
   fprintf(stderr, "\tNumber of New Valid BLUE Shocks = %d\n", iNewBlue);
   fprintf(stderr,
       "\tTotal of %d VALID Blue shocks thus far (Max alloc size = %d).\n",
       iNumShock, iMaxShock);
   fprintf(stderr, "\tLargest distance found between genes = %f\n",
       (float) sqrt((double) fDistSqLargest));
   fprintf(stderr, "\tInvalid shocks = %d\n", iNumInvalid);
   
   fprintf(stderr, "\tNumber of Buckets Not Visible = %d\n",
       iNumNotVisible);
   fprintf(stderr, "\t\tTotal number of newly acquired neighbors = %d\n",
       iAcquireTotal);
   fprintf(stderr, "\t\tTotal number of times viz genes added = %d\n",
       iNumGeneVizAdded);
   
   free(piNewNgbTmp);
   free(piGeneASeeingBTmp);
   free(piGeneVizTmp);
   
   return;
}

/*------------------------------------------------------------*\
 * Function: FetchShocksExtFst
 *
 * Usage: Once Isolated Blue shocks (valid pairs) have been
 *    found for each bucket's FIRST surrounding LAYER, find
 *    other shocks (curves & nodes) using sheet flow.
 *    We only perform one pass of computations, i.e., 1st
 *    intersecting sheets to find curves, and intersecting
 *    curves to find nodes. The goal is to rapidly provide
 *    additional constraints when seeking pairings of sources
 *    between buckets; being exhaustive is not necessary at
 *    this stage, and would potentially slow things down
 *    too much (i.e., same strategy as for shocks internal
 *    to buckets).
 *
\*------------------------------------------------------------*/

void
FetchShocksExtFst(InputData *pInData, ShockScaffold *pScaf,
      short *pisLookUp, Buckets *pBucksXYZ,
      Pt3dCoord *pGeneCoord, Voxel *pVoxels,
      Dimension *pDimMBuck, Dimension *pDimAll)
{
  char    *pcFName = "FetchShocksExtFst";
  int    iNum, iTmp, iMax, iNumNewBlue, iNumNewGreen;
  ShockCurve  *pShockCurves;
  ShockVertex  *pShockVertices;

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

  /* Get triplets of Genes and compute Green nodes (Type I & II) *\
  \*    i.e., valid circumcenters of 3D triangles  */

  pDimMBuck->isFlagPermute++;

  if(pScaf->iNumBlueActive > 0) {
    FetchGreenShocksFstLayer(pInData, pScaf, pShockCurves,
           pisLookUp, pBucksXYZ, pGeneCoord, pVoxels,
           pDimMBuck, pDimAll);
  }
  else {
    fprintf(stderr, "MESG(%s):\n\tNo new (active) Blue sheets ...\n",
      pcFName);
  }
  /* Intersected Active Sheets --> Passive */
  /* SetBluePassive(pScaf);
     pScaf->iBluePassiveLast += pScaf->iNumBlueActive;
     pScaf->iNumBlueActive = 0; */

  /* ----- Set scaffold: New Green and Blue shocks ------ *\
   * For Green shocks, allocate more space than valids    *
  \* for up-coming curves from later iterations.    */

  iNum = 1 + 2 * pDimMBuck->iNumShockCurve;
  iMax = pScaf->iNumGreenMax;
  iTmp = iNum + pScaf->iGreenActiveLast;
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

  /* Make sure there is enough space for new sheets */

  iTmp = pScaf->iBlueActiveLast;
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

  /* -- Set Scaffold with new Valid Green curves -- */

  if(!SetValidCurvesFstLayer(pShockCurves, pScaf, pInData,
           pisLookUp, pBucksXYZ, pDimMBuck)) {
    fprintf(stderr, "ERROR(%s): Can't set curves on scaffold.\n",
      pcFName);
    exit(-2); }
  if(pShockCurves != NULL) {
    free(pShockCurves); pShockCurves = NULL; }
  fprintf(stderr, "MESG(%s):\n", pcFName);
  fprintf(stderr, "\tActive shocks: %d sheets, %d curves.\n",
    pScaf->iNumBlueActive, pScaf->iNumGreenActive);

  /*  TestShocksBlue(pScaf, pInData, pDim);
      TestShocksGreen(pScaf, pInData, pDim);
  */

  /* ---- Compute Tetra-Circumcenters: Shock nodes ---- *\
  \*  Initially, 10 times as many nodes as valid curves */
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

  /* pBucksXYZ->iInitCurveLabel = 0; */
  pDimMBuck->isFlagPermute++;

  if(pScaf->iNumGreenActive > 0) {
    FetchRedShocks(pInData, pScaf, pShockVertices,
       pisLookUp, pBucksXYZ, pGeneCoord, pVoxels,
       pDimMBuck, pDimAll);
  }
  else {
    fprintf(stderr, "MESG(%s):\n\tNo new (active) Green curves ...\n",
      pcFName);
  }
  /* ---- Set Scaffold: New Red, Green and Blue shocks ---- */
  /* For Red shocks, allocate more space than valids      */
  /* for up-coming nodes from iterations */

  iNum = 1 + 2 * pDimMBuck->iNumShockVertex;
  iMax = pScaf->iNumRedMax;
  iTmp = iNum + pScaf->iNumRedLast;
  if(iTmp >= iMax) {
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
  iMax = pScaf->iNumBlueMax;
  iTmp = iNumNewBlue + pScaf->iBlueActiveLast;
  iTmp *= 2.0;
  if(iTmp >= iMax) {
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

  /* -- Set Scaffold with new Valid Red nodes -- */

  if(!SetValidVertices(pShockVertices, pScaf, pInData,
           pisLookUp, pBucksXYZ, pGeneCoord, pVoxels,
           pDimMBuck, pDimAll)) {
    fprintf(stderr, "ERROR(%s): Cannot set vertices on scaffold.\n",
      pcFName);
    exit(-2); }
  if(pShockVertices != NULL) {
    free(pShockVertices); pShockVertices = NULL; }

  /* TestInData(pInData, pDim); */

  fprintf(stderr, "MESG(%s):\n\tActive: %d Curves ",
    pcFName, pScaf->iNumGreenActive);
  fprintf(stderr, "and %d Sheets.\n", pScaf->iNumBlueActive);
  fprintf(stderr, "\tPassive: %d Curves ", pScaf->iNumGreenPassive);
  fprintf(stderr, "and %d Sheets.\n", pScaf->iNumBluePassive);

  return;
}

/* ---- EoF ---- */
