/*------------------------------------------------------------*\
 * File: dt_by_3d_cedt.c
 *
 * Functions:
 *    DTby3dCEDT
 *
 * History:
 *    #0: mid-February 1998, by F.Leymarie
\*------------------------------------------------------------*/

#include "dt_by_3d_cedt.h"
#include "vcl_cassert.h"

/*------------------------------------------------------------*\
 * Function: DTby3dCEDT
 *
 * Usage: Main function to perform a 3d CEDT
 *
 * Inputs: pTheDim: carries all the dimensions values required
 *    pdDtArray: the input 3D image where Sources should be Null values
\*------------------------------------------------------------*/

void DTby3dCEDT(dist_sq_t *pdDtArray, Dimension *pTheDim, int iFlagV, double dDelta,
                offset_t*& fdx, offset_t*& fdy, offset_t*& fdz)
{
  int    iXdim,iYdim,iZdim, iWSpaceSize, iSliceSize;
  int    iz,iy,ix, idz,idy,idx, iDir, iHeapLoc;
  int    iPos, iposnew, iposx, iposy, iposz, iposZmax, iposYmax,iposXmax;
  int    iNel, iNelDSPM, iNelIGPM2, iNelIGPM3;
  offset_t    fDistX, fDistY, fDistZ;
  dist_sq_t    dDist, dDistMaxOnFront, dVal; 
  double            dDeltaMax, dDeltaMin;
  Distance    theDist;
  VectLoc    theVect;
  Heap        theHeap;
  Cedt3d    theCedt;


  iXdim = pTheDim->iXdim;
  iYdim = pTheDim->iYdim;
  iZdim = pTheDim->iZdim;
  iSliceSize = pTheDim->iSliceSize;
  iWSpaceSize = pTheDim->iWSpaceSize;


  AllocHeapAndCedt(&theHeap, &theCedt, iWSpaceSize);
  InitHeapAndCedt(&theHeap, &theCedt, iWSpaceSize, pdDtArray);

  /**** -------------  INITIALIZATION STARTS ----------------- ****/
  /**** -------------  Initialization of Contour/Sources------------- ****/
  
    if(dDelta <= 0.001) dDelta = 0.001;
    dDeltaMax = D_SOURCE_VAL + dDelta;
    dDeltaMin = D_SOURCE_VAL - dDelta;

    if(iFlagV) fprintf(stdout, "\nFront news: INITIALIZATION starts (delta = %f) ....\n\n", dDelta);


    for(iz=0; iz<iZdim; iz++) {
        for(iy=0; iy<iYdim; iy++) {
            for(ix=0; ix<iXdim; ix++) {
                theVect.iPos = iPos = iz*iSliceSize + iy*iXdim + ix;
                dVal = theCedt.pdDtArray[iPos];
                if((dVal > dDeltaMin) && (dVal < dDeltaMax)) {    // Initial source points 
                    theDist.fdx = 0;
                    theDist.fdy = 0;
                    theDist.fdz = 0;
                    theDist.dd2 = 0;
                    theVect.iDir = I_DIR_ERR;
                    theVect.iz = iz;
                    theVect.iy = iy;
                    theVect.ix = ix;

                    AddToFrontAndHeap(&theHeap, &theCedt, &theDist, &theVect,pTheDim);
                }
                else    
                {
                // Background pts. where propagation will take place 
                    theCedt.pdDtArray[iPos] = DIST_LARGE;
                }
            }
        }
    }


  //*** Initial distances & directions for the unit cube at each source point ****/

  if(iFlagV) fprintf(stdout, "\nFront news: No. source points = %d\n", theHeap.N);

  iposZmax = iZdim-1; iposYmax = iYdim-1; iposXmax = iXdim-1;
  iHeapLoc = theHeap.index[0];
    while(theHeap.data[iHeapLoc] == D_SOURCE_VAL) { // Initial source points 
        iPos = iHeapLoc;
        iz = iPos / iSliceSize;
        iy = (iPos - iSliceSize*iz)/iXdim;
        ix = (iPos - iSliceSize*iz)%iXdim;

        remove_from_minheap(&theHeap);

        if(iZdim < 8) { // Not enough slices, we consider it a 2D problem then 
            for(idy=-1; idy<2; idy++) {
                for(idx=-1; idx<2; idx++) {
                    if(idy != 0 || idx !=0) {
                        fDistX = idx;
                        fDistY = idy;
                        dDist = (fDistX*fDistX + fDistY*fDistY);
                        iposz = iz; iposy = iy+idy; iposx = ix+idx;
                        iposnew = iposz*iSliceSize + iposy*iXdim + iposx;
                        if (theCedt.pdDtArray[iposnew] > dDist) {
                            theDist.fdx = fDistX;
                            theDist.fdy = fDistY;
                            theDist.fdz = 0;
                            theDist.dd2 = dDist;
                            //**** Init DSPM only in 2D ****
                            theVect.iDir = theCedt.iDir[iposnew] = InitDirDSPM(idx, idy, 0);
                            theVect.iPos = iposnew;
                            theVect.iz = iposz;
                            theVect.iy = iposy;
                            theVect.ix = iposx;

                            AddToFrontAndHeap(&theHeap, &theCedt, &theDist, &theVect,pTheDim);
                        }
                    }
                }
            }
        }
        else {    // "Real" 3D case 
            for(idz=-1; idz<2; idz++) {
                for(idy=-1; idy<2; idy++) {
                    for(idx=-1; idx<2; idx++) {
                        if(idz !=0 || idy != 0 || idx !=0) {
                            iposz = iz+idz; iposy = iy+idy; iposx = ix+idx;
                            if(iposz >= 0 && iposz <= iposZmax && 
                               iposy >= 0 && iposy <= iposYmax && 
                               iposx >= 0 && iposx <= iposXmax) {
                                iposnew = iposz*iSliceSize + iposy*iXdim + iposx;
                                fDistX =  idx;
                                fDistY =  idy;
                                fDistZ =  idz;
                                dDist =  (fDistX*fDistX + fDistY*fDistY + fDistZ*fDistZ);
                                if (theCedt.pdDtArray[iposnew] > dDist) {
                                    theDist.fdx = fDistX;
                                    theDist.fdy = fDistY;
                                    theDist.fdz = fDistZ;
                                    theDist.dd2 = dDist;
                                    ///*** Init DSPM only in 3D
                                    theVect.iDir = theCedt.iDir[iposnew] = InitDirDSPM(idx, idy, idz);
                                    theVect.iPos = iposnew;
                                    theVect.iz = iposz;
                                    theVect.iy = iposy;
                                    theVect.ix = iposx;

                                    AddToFrontAndHeap(&theHeap, &theCedt, &theDist, &theVect,pTheDim);
                                }
                            }
                        }
                    }
                }
            }
        }
            iHeapLoc = theHeap.index[0];
    }

     if(iFlagV)  fprintf(stdout, "\nFront news: Dilated by a unit cube - No. of el. = %d\n", theHeap.N);

  ///*** Initial growing from the original surface dilated by a cube
  ///*** to set propagation of "intermediate" masks/directions

    iNelDSPM = iNelIGPM2 = iNelIGPM3 = 0;
    iposZmax = iZdim-1; iposYmax = iYdim-1; iposXmax = iXdim-1;
    iHeapLoc = theHeap.index[0];

    while(theHeap.N > 0 && theHeap.data[iHeapLoc] < 3.1) {

        iPos = iHeapLoc;
        iz = iPos / iSliceSize;
        iy = (iPos - iSliceSize*iz)/iXdim;
        ix = (iPos - iSliceSize*iz)%iXdim;

        dDist = theHeap.data[iHeapLoc];
        remove_from_minheap(&theHeap); // Goto next min el. in the Heap 

        theVect.iPos = iPos;
        theVect.iDir = iDir = theCedt.iDir[iPos];

        if(iZdim < 8) {    // We consider it is a 2D problem then 
            if (ix > 1 && ix<iposXmax && iy>1 && iy<iposYmax) {
                theVect.ix = ix;
                theVect.iy = iy;
                theVect.iz = iz;

                if (dDist == 1.0) {        // Propagation of first Front points 
                    iNel = theHeap.N;
                    CedtPropagateDist(&theHeap, &theCedt, &theVect, pTheDim);
                    iNel = theHeap.N - iNel;
                    iNelDSPM += iNel;
                }
                else if (dDist == 2.0) {    // Propagate diagonals in planes 
                    iNel = theHeap.N;
                    InitDiagPlanePropagateDist(&theHeap, &theCedt, &theVect, pTheDim);
                    iNel = theHeap.N - iNel;
                    iNelIGPM2 += iNel;
                }
                else if (dDist == 3.0) {    // Propagate diagonals of the cube 
                    iNel = theHeap.N;
                    InitDiagCubePropagateDist(&theHeap, &theCedt, &theVect, pTheDim);
                    iNel = theHeap.N - iNel;
                    iNelIGPM3 += iNel;
                }
            }
        }
        else {
            if (ix >= 0 && ix<=iposXmax && iy>=0 && iy<=iposYmax && iz>=0 && iz<=iposZmax) {
                theVect.ix = ix;
                theVect.iy = iy;
                theVect.iz = iz;

                if (dDist == 1.0) {    // Propagation of 1st Front points (on grid) 
                    iNel = theHeap.N;
                    CedtPropagateDist(&theHeap, &theCedt, &theVect, pTheDim);
                    iNel = theHeap.N - iNel;
                    iNelDSPM += iNel;
                }
                else if (dDist == 2.0) { // Propagate diagonals in planes 
                    iNel = theHeap.N;
                    InitDiagPlanePropagateDist(&theHeap, &theCedt, &theVect, pTheDim);
                    iNel = theHeap.N - iNel;
                    iNelIGPM2 += iNel;
                }
                else if (dDist == 3.0) { // Propagate diagonals of the cube 
                    iNel = theHeap.N;
                    InitDiagCubePropagateDist(&theHeap, &theCedt, &theVect, pTheDim);
                    iNel = theHeap.N - iNel;
                    iNelIGPM3 += iNel;
                }
            }
        }
        iHeapLoc = theHeap.index[0];
    }

    if(iFlagV) fprintf(stdout, "\nFront news: Dilated one more step - No. of el. = %d\n", theHeap.N);
    if(iFlagV) fprintf(stdout, "\t%d DSPM (grid) el., %d Planar Diag. el., %d Cubic Diag. el. \n", iNelDSPM, iNelIGPM2, iNelIGPM3);

  /**** -------------  INITIALIZATION ENDS ----------------- ****/
  
    /**** ------ MAIN LOOP for propagation from front(s) -------****/
    if(iFlagV) fprintf(stdout, "\nFront news: MAIN LOOP starts ....\n\n");
    iposZmax = iZdim-1; iposYmax = iYdim-1; iposXmax = iXdim-1;

    iHeapLoc = theHeap.index[0];
    dDistMaxOnFront = 3;
    while(theHeap.N > 0) {    // While the Heap is not empty 


        iPos = iHeapLoc;
        iz = iPos / iSliceSize;
        iy = (iPos - iSliceSize*iz)/iXdim;
        ix = (iPos - iSliceSize*iz)%iXdim;

        dDist = theHeap.data[iHeapLoc];
        if (dDist > dDistMaxOnFront) {
            if(iFlagV) fprintf(stdout, "Front news: No. el. = %d , Max Dist = %f\n", theHeap.N, (float) vcl_sqrt((float)dDist));
            dDistMaxOnFront = dDist;
        }
        remove_from_minheap(&theHeap); // Goto next min el. in the Heap 

        if(iZdim < 8) {    // We consider it is a 2D problem then 
            if (ix > 0 && ix<iposXmax && iy>0 && iy<iposYmax) {
                theVect.ix = ix;
                theVect.iy = iy;
                theVect.iz = iz;

                CedtPropagateDist(&theHeap, &theCedt, &theVect, pTheDim);
            }
        }
        else if (ix >= 0 && ix<=iposXmax && iy>=0 && iy<=iposYmax && iz>=0 && iz<=iposZmax) {
          theVect.iPos = iPos; // = iz*iSliceSize + iy*iXdim + ix;
          theVect.iDir = iDir = theCedt.iDir[iPos];

            theVect.ix = ix;
            theVect.iy = iy;
            theVect.iz = iz;

            CedtPropagateDist(&theHeap, &theCedt, &theVect, pTheDim);
        }

    iHeapLoc = theHeap.index[0]; // Process next element at top of stack 

    }
  /**** ------ MAIN LOOP ENDS -------****/

    //deallocate working memory before allocating offset arrays
    DeAllocHeapAndCedt(&theHeap, &theCedt);

//    AdjustDataArray(pdDtArray, pTheDim);
    fdx   = new offset_t[pTheDim->iWSpaceSize];
    fdy   = new offset_t[pTheDim->iWSpaceSize];
    fdz   = new offset_t[pTheDim->iWSpaceSize];
    for(int k = 2; k < iZdim-2; k++){
    for(int j = 2; j < iYdim-2; j++){
    for(int i = 2; i < iXdim-2; i++){
            int offset = k*iSliceSize + j*iXdim + i;
            int sloc = theCedt.sourceLoc[offset];
            int sloc_z = (sloc / pTheDim->iSliceSize);
            int sloc_y = (sloc - sloc_z*pTheDim->iSliceSize)/pTheDim->iXdim;
            int sloc_x = (sloc - sloc_z*pTheDim->iSliceSize)%pTheDim->iXdim;

            fdz[offset] = k - sloc_z;
            fdy[offset] = j - sloc_y;
            fdx[offset] = i - sloc_x;
    }
    }
    }

    //once offsets are saved, deallocate offset array 
  delete [] theCedt.sourceLoc;
  return;
}
/* ======================================================*/
