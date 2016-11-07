#include "cedt3d_utils.h"

/*------------------------------------------------------------*\
 * Function: AllocHeapAndCedt
 *
 * Usage:  Allocate memory space for the heap & Cedt structures
\*------------------------------------------------------------*/

void
AllocHeapAndCedt(Heap *pTheHeap, Cedt3d *pTheCedt, int iWSpaceSize)
{
  pTheHeap->index = new int[iWSpaceSize];
  pTheHeap->loc   = new int[iWSpaceSize];

  pTheCedt->sourceLoc   = new int[iWSpaceSize];

  pTheCedt->iDir  = new dir_t[iWSpaceSize];
  return;
}

/*------------------------------------------------------------*\
 * Function: DeAllocHeapAndCedt
 *
 * Usage:  Free memory space for the heap & Cedt structures
\*------------------------------------------------------------*/

void
DeAllocHeapAndCedt(Heap *pTheHeap, Cedt3d *pTheCedt)
{
  delete [] pTheHeap->index;
  delete [] pTheHeap->loc;
  delete [] pTheCedt->iDir;

  return;
}

/*------------------------------------------------------------*\
 * Function: InitHeapAndCedt
 *
 * Usage:  Initialize the Heap & Cedt structures
\*------------------------------------------------------------*/

void
InitHeapAndCedt(Heap *pTheHeap, Cedt3d *pTheCedt, int iWSpaceSize,
        dist_sq_t *pdDtArray)
{
  register int    i;

  pTheHeap->N = 0;
  pTheHeap->data = pdDtArray;
  pTheCedt->pdDtArray = pdDtArray;

  for(i = 0; i < iWSpaceSize; i++)
    {
      pTheHeap->loc[i] = -1;
      pTheHeap->index[i] = 0;

      pTheCedt->sourceLoc[i] = -1;

      pTheCedt->iDir[i] = I_DIR_ERR;  /* Initially: no directions */
    }

  return;
}

/*------------------------------------------------------------*\
 * Function: InitDirDSPM
 *
 * Usage:  Set initial directions for a 3d cube of orientations
 *      26 Directly Supported Propagation Masks (DSPM)
\*------------------------------------------------------------*/
int
InitDirDSPM(int idx, int idy, int idz)
{
  if(idz== 0) {    /* Plane Z = 0 */
    if (idx == 1 &&  idy == 0)
      return I_DSPM_X;
    else if (idx == 1 &&  idy == -1)
      return I_DSPM_Xj;
    else if (idx == 0 &&  idy == -1)
      return I_DSPM_j;
    else if (idx == -1 &&  idy == -1)
      return I_DSPM_ij;
    else if (idx == -1 &&  idy == 0)
      return I_DSPM_i;
    else if (idx == -1 &&  idy == 1)
      return I_DSPM_iY;
    else if (idx == 0 &&  idy == 1)
      return I_DSPM_Y;
    else if (idx == 1 &&  idy == 1)
      return I_DSPM_XY;
    else
      return I_DIR_ERR;
  }
  else if(idz == -1) {    /* Neg. Z Hemisphere */
    if (idx == 0 &&  idy == 0)
      return I_DSPM_k;
    else if (idx == 1 &&  idy == 0)
      return I_DSPM_Xk;
    else if (idx == 1 &&  idy == -1)
      return I_DSPM_Xjk;
    else if (idx == 0 &&  idy == -1)
      return I_DSPM_jk;
    else if (idx == -1 &&  idy == -1)
      return I_DSPM_ijk;
    else if (idx == -1 &&  idy == 0)
      return I_DSPM_ik;
    else if (idx == -1 &&  idy == 1)
      return I_DSPM_iYk;
    else if (idx == 0 &&  idy == 1)
      return I_DSPM_Yk;
    else if (idx == 1 &&  idy == 1)
      return I_DSPM_XYk;
    else
      return I_DIR_ERR;
  }
  else if(idz == 1) {        /* Pos. Z Hemisphere */
    if (idx == 0 &&  idy == 0)
      return I_DSPM_Z;
    else if (idx == 1 &&  idy == 0)
      return I_DSPM_XZ;
    else if (idx == 1 &&  idy == -1)
      return I_DSPM_XjZ;
    else if (idx == 0 &&  idy == -1)
      return I_DSPM_jZ;
    else if (idx == -1 &&  idy == -1)
      return I_DSPM_ijZ;
    else if (idx == -1 &&  idy == 0)
      return I_DSPM_iZ;
    else if (idx == -1 &&  idy == 1)
      return I_DSPM_iYZ;
    else if (idx == 0 &&  idy == 1)
      return I_DSPM_YZ;
    else if (idx == 1 &&  idy == 1)
      return I_DSPM_XYZ;
    else
      return I_DIR_ERR;
  }
  else
    return I_DIR_ERR;
}

/*------------------------------------------------------------*\
 * Function: SetMask
 *
 * Usage:  Sets the X, Y and Z steps for each sub-mask elements (1, 2 or 4)
 *      in function of the Direction of propagation.
\*------------------------------------------------------------*/

void
SetMask(Mask *pTheMask)
{
  switch(pTheMask->iDir)
    {
    case  I_DIR_ERR:
      pTheMask->iNumMaskEl = 0;
      break;
      /*******************  Plane Z = 0  *******************/
    case I_DSPM_X:    /* x = 1  , y = 0 , z = 0 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      break;
    case I_DSPM_Xj:    /* x = 1  , y = -1 , z = 0 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      break;
    case I_IGPM2_XjX:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 0;
      break;
    case I_IGPM2_Xjj:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 0;
      break;
    case I_DSPM_j:    /* x = 0  , y = -1 , z = 0 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      break;
    case I_DSPM_ij:    /* x = -1  , y = -1 , z = 0 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      break;
    case I_IGPM2_iji:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 0;
      break;
    case I_IGPM2_ijj:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 0;
      break;
    case I_DSPM_i:    /* x = -1  , y = 0 , z = 0 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      break;
    case I_DSPM_iY:    /* x = -1  , y = 1 , z = 0 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      break;
    case I_IGPM2_iYi:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 0;
      break;
    case I_IGPM2_iYY:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 0;
      break;
    case I_DSPM_Y:    /* x = 0  , y = 1 , z = 0 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      break;
    case I_DSPM_XY:    /* x = 1  , y = 1 , z = 0 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      break;
    case I_IGPM2_XYX:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 0;
      break;
    case I_IGPM2_XYY:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 0;
      break;
      /******************* Negative Z Hemisphere  *******************/
    case I_DSPM_k:    /* x = 0  , y = 0 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_DSPM_Xk:    /* x = 1  , y = 0 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_IGPM2_XkX:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = -1;
      break;
    case I_IGPM2_Xkk:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = -1;
      break;
    case I_DSPM_Xjk:    /* x = 1  , y = -1 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_IGPM3_X_Xjk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 0;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = 1;
      pTheMask->iStepY[3] = 0;
      pTheMask->iStepZ[3] = -1;
      break;
    case I_IGPM3_j_Xjk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = -1;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = 1;
      pTheMask->iStepY[3] = -1;
      pTheMask->iStepZ[3] = 0;
      break;
    case I_IGPM3_k_Xjk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = -1;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = 0;
      pTheMask->iStepY[3] = -1;
      pTheMask->iStepZ[3] = -1;
      break;
    case I_DSPM_jk:    /* x = 0  , y = -1 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_IGPM2_jkj:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = -1;
      break;
    case I_IGPM2_jkk:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = -1;
      break;
    case I_DSPM_ijk:    /* x = -1  , y = -1 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_IGPM3_i_ijk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = -1;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = -1;
      pTheMask->iStepY[3] = -1;
      pTheMask->iStepZ[3] = 0;
      break;
    case I_IGPM3_j_ijk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 0;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = 0;
      pTheMask->iStepY[3] = -1;
      pTheMask->iStepZ[3] = -1;
      break;
    case I_IGPM3_k_ijk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = -1;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = -1;
      pTheMask->iStepY[3] = 0;
      pTheMask->iStepZ[3] = -1;
      break;
    case I_DSPM_ik:    /* x = -1  , y = 0 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_IGPM2_iki:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = -1;
      break;
    case I_IGPM2_ikk:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = -1;
      break;
    case I_DSPM_iYk:    /* x = -1  , y = 1 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_IGPM3_i_iYk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 0;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = -1;
      pTheMask->iStepY[3] = 0;
      pTheMask->iStepZ[3] = -1;
      break;
    case I_IGPM3_Y_iYk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = -1;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = -1;
      pTheMask->iStepY[3] = 1;
      pTheMask->iStepZ[3] = 0;
      break;
    case I_IGPM3_k_iYk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = -1;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = 0;
      pTheMask->iStepY[3] = 1;
      pTheMask->iStepZ[3] = -1;
      break;
    case I_DSPM_Yk:    /* x = 0  , y = 1 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_IGPM2_YkY:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = -1;
      break;
    case I_IGPM2_Ykk:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = -1;
      break;
    case I_DSPM_XYk:    /* x = 1  , y = 1 , z = -1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = -1;
      break;
    case I_IGPM3_X_XYk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = -1;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = 1;
      pTheMask->iStepY[3] = 1;
      pTheMask->iStepZ[3] = 0;
      break;
    case I_IGPM3_Y_XYk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 0;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = 0;
      pTheMask->iStepY[3] = 1;
      pTheMask->iStepZ[3] = -1;
      break;
    case I_IGPM3_k_XYk:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = -1;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = -1;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = -1;
      pTheMask->iStepX[3] = 1;
      pTheMask->iStepY[3] = 0;
      pTheMask->iStepZ[3] = -1;
      break;
      /*******************  Positive Z Hemisphere  *******************/
    case I_DSPM_Z:    /* x = 0  , y = 0 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_DSPM_XZ:    /* x = 1  , y = 0 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_IGPM2_XZX:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = 1;
      break;
    case I_IGPM2_XZZ:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = 1;
      break;
    case I_DSPM_XjZ:    /* x = 1  , y = -1 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_IGPM3_X_XjZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = 1;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = 1;
      pTheMask->iStepY[3] = -1;
      pTheMask->iStepZ[3] = 0;
      break;
    case I_IGPM3_j_XjZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 0;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = 0;
      pTheMask->iStepY[3] = -1;
      pTheMask->iStepZ[3] = 1;
      break;
    case I_IGPM3_Z_XjZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 1;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = 1;
      pTheMask->iStepY[3] = 0;
      pTheMask->iStepZ[3] = 1;
      break;
    case I_DSPM_jZ:    /* x = 0  , y = -1 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_IGPM2_jZj:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 1;
      break;
    case I_IGPM2_jZZ:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 1;
      break;
    case I_DSPM_ijZ:    /* x = -1  , y = -1 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_IGPM3_i_ijZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 0;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = -1;
      pTheMask->iStepY[3] = 0;
      pTheMask->iStepZ[3] = 1;
      break;
    case I_IGPM3_j_ijZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = -1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = -1;
      pTheMask->iStepZ[1] = 1;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = -1;
      pTheMask->iStepY[3] = -1;
      pTheMask->iStepZ[3] = 0;
      break;
    case I_IGPM3_Z_ijZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = 1;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = -1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = 0;
      pTheMask->iStepY[3] = -1;
      pTheMask->iStepZ[3] = 1;
      break;
    case I_DSPM_iZ:    /* x = -1  , y = 0 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_IGPM2_iZi:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = 1;
      break;
    case I_IGPM2_iZZ:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = 1;
      break;
    case I_DSPM_iYZ:    /* x = -1  , y = 1 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_IGPM3_i_iYZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = -1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = 1;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = -1;
      pTheMask->iStepY[3] = 1;
      pTheMask->iStepZ[3] = 0;
      break;
    case I_IGPM3_Y_iYZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = -1;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 0;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = 0;
      pTheMask->iStepY[3] = 1;
      pTheMask->iStepZ[3] = 1;
      break;
    case I_IGPM3_Z_iYZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 1;
      pTheMask->iStepX[2] = -1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = -1;
      pTheMask->iStepY[3] = 0;
      pTheMask->iStepZ[3] = 1;
      break;
    case I_DSPM_YZ:    /* x = 0  , y = 1 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_IGPM2_YZY:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 1;
      break;
    case I_IGPM2_YZZ:
      pTheMask->iNumMaskEl = 2;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 1;
      break;
    case I_DSPM_XYZ:    /* x = 1  , y = 1 , z = 1 */ 
      pTheMask->iNumMaskEl = 1;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 1;
      break;
    case I_IGPM3_X_XYZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 1;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 0;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = 1;
      pTheMask->iStepY[3] = 0;
      pTheMask->iStepZ[3] = 1;
      break;
    case I_IGPM3_Y_XYZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 1;
      pTheMask->iStepZ[0] = 0;
      pTheMask->iStepX[1] = 0;
      pTheMask->iStepY[1] = 1;
      pTheMask->iStepZ[1] = 1;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = 1;
      pTheMask->iStepY[3] = 1;
      pTheMask->iStepZ[3] = 0;
      break;
    case I_IGPM3_Z_XYZ:
      pTheMask->iNumMaskEl = 4;
      pTheMask->iStepX[0] = 0;
      pTheMask->iStepY[0] = 0;
      pTheMask->iStepZ[0] = 1;
      pTheMask->iStepX[1] = 1;
      pTheMask->iStepY[1] = 0;
      pTheMask->iStepZ[1] = 1;
      pTheMask->iStepX[2] = 1;
      pTheMask->iStepY[2] = 1;
      pTheMask->iStepZ[2] = 1;
      pTheMask->iStepX[3] = 0;
      pTheMask->iStepY[3] = 1;
      pTheMask->iStepZ[3] = 1;
      break;

    default:
      pTheMask->iNumMaskEl = 0;
      break;
    }
  return;
}

/*------------------------------------------------------------*\
 * Function: SetPlaneOffDiagMasks
 *
 * Usage:  Identfies the 2 new masks for planar off-diagonals
 *      propagation, in function of the DSPM.
 *      Defines (3 planes times 4 diagonals times 2 sides)
 *      24 Intra-Grid Propagation Masks in 2D (IGPM2)
\*------------------------------------------------------------*/

void
SetPlaneOffDiagMasks(dir_t iDiagDir, dir_t *piNew2dMasks)
{
  switch(iDiagDir)
    {
      /*******************  IGPM2 : Plane Z = 0  *******************/
    case I_DSPM_Xj:    /* x = 1  , y = -1 , z = 0 */ 
      *piNew2dMasks++ = I_IGPM2_XjX;
      *piNew2dMasks = I_IGPM2_Xjj;
      break;
    case I_DSPM_ij:    /* x = -1  , y = -1 , z = 0 */ 
      *piNew2dMasks++ = I_IGPM2_iji;
      *piNew2dMasks = I_IGPM2_ijj;
      break;
    case I_DSPM_iY:    /* x = -1  , y = 1 , z = 0 */ 
      *piNew2dMasks++ = I_IGPM2_iYi;
      *piNew2dMasks = I_IGPM2_iYY;
      break;
    case I_DSPM_XY:    /* x = 1  , y = 1 , z = 0 */ 
      *piNew2dMasks++ = I_IGPM2_XYX;
      *piNew2dMasks = I_IGPM2_XYY;
      break;
      /*******************  IGPM2 : Neg. Z Hemisphere  *******************/
    case I_DSPM_Xk:    /* x = 1  , y = 0 , z = -1 */ 
      *piNew2dMasks++ = I_IGPM2_XkX;
      *piNew2dMasks = I_IGPM2_Xkk;
      break;
    case I_DSPM_jk:    /* x = 0  , y = -1 , z = -1 */ 
      *piNew2dMasks++ = I_IGPM2_jkj;
      *piNew2dMasks = I_IGPM2_jkk;
      break;
    case I_DSPM_ik:    /* x = -1  , y = 0 , z = -1 */ 
      *piNew2dMasks++ = I_IGPM2_iki;
      *piNew2dMasks = I_IGPM2_ikk;
      break;
    case I_DSPM_Yk:    /* x = 0  , y = 1 , z = -1 */ 
      *piNew2dMasks++ = I_IGPM2_YkY;
      *piNew2dMasks = I_IGPM2_Ykk;
      break;
      /*******************  IGPM2 : Pos. Z Hemisphere  *******************/
    case I_DSPM_XZ:    /* x = 1  , y = 0 , z = 1 */ 
      *piNew2dMasks++ = I_IGPM2_XZX;
      *piNew2dMasks = I_IGPM2_XZZ;
      break;
    case I_DSPM_jZ:    /* x = 0  , y = -1 , z = 1 */ 
      *piNew2dMasks++ = I_IGPM2_jZj;
      *piNew2dMasks = I_IGPM2_jZZ;
      break;
    case I_DSPM_iZ:    /* x = -1  , y = 0 , z = 1 */ 
      *piNew2dMasks++ = I_IGPM2_iZi;
      *piNew2dMasks = I_IGPM2_iZZ;
      break;
    case I_DSPM_YZ:    /* x = 0  , y = 1 , z = 1 */ 
      *piNew2dMasks++ = I_IGPM2_YZY;
      *piNew2dMasks = I_IGPM2_YZZ;
      break;

    default:
      *piNew2dMasks++ = I_DIR_ERR;
      *piNew2dMasks = I_DIR_ERR;
      break;
    }
  return;
}

/*------------------------------------------------------------*\
 * Function: SetCubicOffDiagMasks
 *
 * Usage:  Identifies the 3 new masks for 3D off-(cubic)diagonals propagation.
 *      in function of the (cubic) DSPM Direction of propagation.
 *      The coding of the IGPM3 masks is as follows:
 *         1) first index indicates to which face of the cube of directions
 *             we attach the 4 voxels making-up one mask
 *         2) the second set of 3 indices indicates the cone of directions
 *             for the 4 voxels making-up the mask.
\*------------------------------------------------------------*/

void
SetCubicOffDiagMasks(dir_t iDiagDir, dir_t *piNew3dMasks)
{
  switch(iDiagDir)
    {
    case I_DSPM_Xjk:    /* x = 1  , y = -1 , z = -1 */ 
      *piNew3dMasks++ = I_IGPM3_X_Xjk;
      *piNew3dMasks++ = I_IGPM3_j_Xjk;
      *piNew3dMasks = I_IGPM3_k_Xjk;
      break;
    case I_DSPM_ijk:    /* x = -1  , y = -1 , z = -1 */ 
      *piNew3dMasks++ = I_IGPM3_i_ijk;
      *piNew3dMasks++ = I_IGPM3_j_ijk;
      *piNew3dMasks = I_IGPM3_k_ijk;
      break;
    case I_DSPM_iYk:    /* x = -1  , y = 1 , z = -1 */ 
      *piNew3dMasks++ = I_IGPM3_i_iYk;
      *piNew3dMasks++ = I_IGPM3_Y_iYk;
      *piNew3dMasks = I_IGPM3_k_iYk;
      break;
    case I_DSPM_XYk:    /* x = 1  , y = 1 , z = -1 */ 
      *piNew3dMasks++ = I_IGPM3_X_XYk;
      *piNew3dMasks++ = I_IGPM3_Y_XYk;
      *piNew3dMasks = I_IGPM3_k_XYk;
      break;
    case I_DSPM_XjZ:    /* x = 1  , y = -1 , z = 1 */ 
      *piNew3dMasks++ = I_IGPM3_X_XjZ;
      *piNew3dMasks++ = I_IGPM3_j_XjZ;
      *piNew3dMasks = I_IGPM3_Z_XjZ;
      break;
    case I_DSPM_ijZ:    /* x = -1  , y = -1 , z = 1 */ 
      *piNew3dMasks++ = I_IGPM3_i_ijZ;
      *piNew3dMasks++ = I_IGPM3_j_ijZ;
      *piNew3dMasks = I_IGPM3_Z_ijZ;
      break;
    case I_DSPM_iYZ:    /* x = -1  , y = 1 , z = 1 */ 
      *piNew3dMasks++ = I_IGPM3_i_iYZ;
      *piNew3dMasks++ = I_IGPM3_Y_iYZ;
      *piNew3dMasks = I_IGPM3_Z_iYZ;
      break;
    case I_DSPM_XYZ:    /* x = 1  , y = 1 , z = 1 */ 
      *piNew3dMasks++ = I_IGPM3_X_XYZ;
      *piNew3dMasks++ = I_IGPM3_Y_XYZ;
      *piNew3dMasks = I_IGPM3_Z_XYZ;
      break;
    default:
      *piNew3dMasks++ = I_DIR_ERR;
      *piNew3dMasks++ = I_DIR_ERR;
      *piNew3dMasks = I_DIR_ERR;
      break;
    }
  return;
}

/*------------------------------------------------------------*\
 * Function: AdjustDataArray
 *
 * Usage:  Any element with F_LARGE value as well as [0] (set by the Heap)
 *    is reset to 0; otherwise, take the square root of the squared
 *    Euclidean DT
\*------------------------------------------------------------*/
void
AdjustDataArray(double *pdDtArray, Dimension *pTheDim)
{
  register int    i, iWSpaceSize;

  iWSpaceSize = pTheDim->iWSpaceSize;

  pdDtArray[0] = 0.0;
  for(i = 0; i < iWSpaceSize; i++) {
    if(pdDtArray[i] == F_LARGE)
      pdDtArray[i] = 0.0;
    else
      pdDtArray[i] = vcl_sqrt(pdDtArray[i]);
  }
  return;
}
/*=========================================================*/
