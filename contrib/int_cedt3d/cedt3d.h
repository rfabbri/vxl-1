#ifndef _CEDT3D_H_
#define _CEDT3D_H_

#include <vcl_cmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vil3d/vil3d_image_view.h>

/* #include <utils.h> */
//#include <3dUtils.h>
//#include <rawimageio.h>
/*
void slicePrint(int* arr,const int& numEl, const int& w, const int& h);
void slicePrint(double* arr,const int& numEl, const int& w, const int& h);
void slicePrintxyz(double* arr, int xdim, int ydim, int zdim);

void slicePrint(const vil3d_image_view<vxl_byte> arr);
void slicePrint(const vil3d_image_view<float> arr);
void slicePrint(const vil3d_image_view<double> arr);
*/
//double minDist(const int& x, const int& y, const int& z, const int& xdim, const int& ydim, const int& zdim, double* arr);


/* --------------------- Structures ------------------------*/

typedef int offset_t  ;
typedef unsigned int dist_sq_t;
typedef short int dir_t;

typedef struct {
  dir_t    *iDir;
  int *sourceLoc; 
  unsigned int    *pdDtArray;    /* Array of computed squared distances */
} Cedt3d;

typedef struct {
  dir_t    iDir;
  int    iz,iy,ix;
  int    iPos;    /* position in the 3D array */
} VectLoc;

typedef struct {
  offset_t    fdx, fdy, fdz;
  dist_sq_t    dd2;    /* Squared Euclidean distance */
} Distance;

typedef struct {
  int    iZdim, iYdim, iXdim;
  int    iWSpaceSize, iSliceSize;  
} Dimension;

typedef struct {
  short    iNumMaskEl; 
  dir_t iDir;
  short    iStepX[4];
  short    iStepY[4];
  short    iStepZ[4];
} Mask;

/*---------------------- Constants --------------------------*/

#define D_SOURCE_VAL    0
#define F_ZERO_DIST    0.0

#define INT_LARGE    90000     // 32 bit integers
#define DIST_LARGE    9999999 
#define F_LARGE        9999999.0  /* > 200 x 200 x 200 */

#define I_DIR_ERR    0
#define I_STEP        1

/***** Directional indices *****/

/** DSPM: 26 Directly Supported Propagation Masks **/

#define I_DSPM_X    1
#define I_DSPM_i        -1
#define I_DSPM_Y    I_DSPM_X + I_STEP
#define I_DSPM_j        I_DSPM_i - I_STEP
#define I_DSPM_Z    I_DSPM_X + (2*I_STEP)
#define I_DSPM_k    I_DSPM_i - (2*I_STEP)

#define I_DSPM_XY    I_DSPM_X + (3*I_STEP)
#define I_DSPM_ij        I_DSPM_i - (3*I_STEP)
#define I_DSPM_Xj    I_DSPM_X + (4*(I_STEP))
#define I_DSPM_iY    I_DSPM_i - (4*I_STEP)
#define I_DSPM_XZ    I_DSPM_X + (5*I_STEP)
#define I_DSPM_ik    I_DSPM_i - (5*I_STEP)
#define I_DSPM_Xk    I_DSPM_X + (6*I_STEP)
#define I_DSPM_iZ    I_DSPM_i - (6*I_STEP)
#define I_DSPM_YZ    I_DSPM_X + (7*I_STEP)
#define I_DSPM_jk    I_DSPM_i - (7*I_STEP)
#define I_DSPM_Yk    I_DSPM_X + (8*I_STEP)
#define I_DSPM_jZ    I_DSPM_i - (8*I_STEP)

#define I_DSPM_XYZ    I_DSPM_X + (9*I_STEP)
#define I_DSPM_ijk    I_DSPM_i - (9*I_STEP)
#define I_DSPM_XYk    I_DSPM_X + (10*I_STEP)
#define I_DSPM_XjZ    I_DSPM_i - (10*I_STEP)
#define I_DSPM_Xjk    I_DSPM_X + (11*I_STEP)
#define I_DSPM_iYZ    I_DSPM_i - (11*I_STEP)
#define I_DSPM_iYk    I_DSPM_X + (12*I_STEP)
#define I_DSPM_ijZ    I_DSPM_i - (12*I_STEP)

/** IGPM2: 24 Intra-Grid Propagation Masks in 2D **/
/**       for 3 planes x 4 diagonals x 2 sides  **/

#define I_IGPM2_XjX    I_DSPM_X + (13*I_STEP)
#define I_IGPM2_Xjj    I_DSPM_i - (13*I_STEP)
#define I_IGPM2_iji    I_DSPM_X + (14*I_STEP)
#define I_IGPM2_ijj    I_DSPM_i - (14*I_STEP)
#define I_IGPM2_iYi    I_DSPM_X + (15*I_STEP)
#define I_IGPM2_iYY    I_DSPM_i - (15*I_STEP)
#define I_IGPM2_XYX    I_DSPM_X + (16*I_STEP)
#define I_IGPM2_XYY    I_DSPM_i - (16*I_STEP)
#define I_IGPM2_XkX    I_DSPM_X + (17*I_STEP)
#define I_IGPM2_Xkk    I_DSPM_i - (17*I_STEP)
#define I_IGPM2_iki    I_DSPM_X + (18*I_STEP)
#define I_IGPM2_ikk    I_DSPM_i - (18*I_STEP)
#define I_IGPM2_iZi    I_DSPM_X + (19*I_STEP)
#define I_IGPM2_iZZ    I_DSPM_i - (19*I_STEP)
#define I_IGPM2_XZX    I_DSPM_X + (20*I_STEP)
#define I_IGPM2_XZZ    I_DSPM_i - (20*I_STEP)
#define I_IGPM2_YkY    I_DSPM_X + (21*I_STEP)
#define I_IGPM2_Ykk    I_DSPM_i - (21*I_STEP)
#define I_IGPM2_jkj    I_DSPM_X + (22*I_STEP)
#define I_IGPM2_jkk    I_DSPM_i - (22*I_STEP)
#define I_IGPM2_jZj    I_DSPM_X + (23*I_STEP)
#define I_IGPM2_jZZ    I_DSPM_i - (23*I_STEP)
#define I_IGPM2_YZY    I_DSPM_X + (24*I_STEP)
#define I_IGPM2_YZZ    I_DSPM_i - (24*I_STEP)

/** IGPM3: 24 Intra-Grid Propagation Masks in 3D **/

#define I_IGPM3_X_XYZ    I_DSPM_X + (25*I_STEP)
#define I_IGPM3_i_ijk    I_DSPM_i - (25*I_STEP)
#define I_IGPM3_Y_XYZ    I_DSPM_X + (26*I_STEP)
#define I_IGPM3_j_ijk    I_DSPM_i - (26*I_STEP)
#define I_IGPM3_Z_XYZ    I_DSPM_X + (27*I_STEP)
#define I_IGPM3_k_ijk    I_DSPM_i - (27*I_STEP)

#define I_IGPM3_X_XYk    I_DSPM_X + (28*I_STEP)
#define I_IGPM3_i_ijZ    I_DSPM_i - (28*I_STEP)
#define I_IGPM3_Y_XYk    I_DSPM_X + (29*I_STEP)
#define I_IGPM3_j_ijZ    I_DSPM_i - (29*I_STEP)
#define I_IGPM3_k_XYk    I_DSPM_X + (30*I_STEP)
#define I_IGPM3_Z_ijZ    I_DSPM_i - (30*I_STEP)

#define I_IGPM3_X_XjZ    I_DSPM_X + (31*I_STEP)
#define I_IGPM3_i_iYk    I_DSPM_i - (31*I_STEP)
#define I_IGPM3_j_XjZ    I_DSPM_X + (32*I_STEP)
#define I_IGPM3_Y_iYk    I_DSPM_i - (32*I_STEP)
#define I_IGPM3_Z_XjZ    I_DSPM_X + (33*I_STEP)
#define I_IGPM3_k_iYk    I_DSPM_i - (33*I_STEP)

#define I_IGPM3_X_Xjk    I_DSPM_X + (34*I_STEP)
#define I_IGPM3_i_iYZ    I_DSPM_i - (34*I_STEP)
#define I_IGPM3_j_Xjk    I_DSPM_X + (35*I_STEP)
#define I_IGPM3_Y_iYZ    I_DSPM_i - (35*I_STEP)
#define I_IGPM3_k_Xjk    I_DSPM_X + (36*I_STEP)
#define I_IGPM3_Z_iYZ    I_DSPM_i - (36*I_STEP)

#define I_NO_SHOCK    0
#define I_SHOCK_1    1
#define I_SHOCK_1_Cross    -1
#define I_SHOCK_2    2
#define I_SHOCK_2_Cross    -2
#define I_SHOCK_3    3
#define I_SHOCK_3_Cross    -3
#define I_SHOCK_4    4
#define I_SHOCK_4_Cross    -4

#define I_SKEL        1
#define I_SKEL_Cross    -1
#define I_SKEL_NODE    2
#define I_SKEL_NODE_Cross -2



#define D_SHOCK_DT_VAL    -10.0

/* #define COMPUTE_SHOCK    1 */

/*----------------------------------------------------------*/

#endif /* _CEDT3D_H_ */
