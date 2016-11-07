#include <vcl_iostream.h>
#include <vcl_streambuf.h>
#include <math.h>
#include <dbecl/dbecl_epipole.h>
#include <dbecl/dbecl_epiband.h>



#include "mex.h"

extern void _main();

static
void mymex(
      const double *x,
      const double *y,
      mwSize n,
      double pole_x,
      double pole_y,
      mxArray **p_theta_plus,
      mxArray **p_theta_minus,
      mxArray **p_theta_mean
      )
{

  dbecl_epipole ep(vgl_point_2d<double>(pole_x,pole_y));
  vgl_polygon<double> poly(x,y,n);
  
  double theta_plus;
  double theta_minus;
  double theta_mean;
  dbecl_epiband::get_bounds(poly, ep, theta_plus, theta_minus, theta_mean);

  double *tmp;
  *p_theta_plus= mxCreateDoubleMatrix(1,1,mxREAL);
  tmp = mxGetPr(*p_theta_plus);
  *tmp = theta_plus;

  *p_theta_minus= mxCreateDoubleMatrix(1,1,mxREAL);
  tmp = mxGetPr(*p_theta_minus);
  *tmp = theta_minus;

  *p_theta_mean= mxCreateDoubleMatrix(1,1,mxREAL);
  tmp = mxGetPr(*p_theta_mean);
  *tmp = theta_mean;

  return;
}



// Gets the angular bounds of a polygon with respect to a point e.g. the 'pole' in a polar coordinate
// system. This is done in a way that is immune to angular discontinuities
//
// [theta_plus,theta_minus,theta_mean] = poly_get_bounds(x,y,pole_x,pole_y)
void mexFunction(
     int          nlhs,
     mxArray      *plhs[],
     int          nrhs,
     const mxArray *prhs[]
     )
{
  if (nrhs != 4)
    mexErrMsgTxt("Exactly 4 inputs are required.\n");
      
  if (nlhs > 3) {
    mexErrMsgTxt("Too many output arguments\n");
  }

  mwSize arg_nrows = mxGetM(prhs[0]);
  mwSize arg_ncols = mxGetN(prhs[0]);
  mwSize x_nrows = arg_nrows;
  if (arg_nrows!= 1 && arg_ncols != 1 || !mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]))
    mexErrMsgTxt("Input must be noncomplex double 1D vector.\n");

  arg_nrows = mxGetM(prhs[1]);
  arg_ncols = mxGetN(prhs[1]);
  mwSize y_nrows = arg_nrows;
  if (arg_nrows!= 1 && arg_ncols != 1 || !mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]))
    mexErrMsgTxt("Input must be noncomplex double  1D vector.\n");

  if (x_nrows != y_nrows)
    mexErrMsgTxt("First two inputs must have same number of elements.\n");

  arg_nrows = mxGetM(prhs[2]);
  arg_ncols = mxGetN(prhs[2]);
  if (arg_nrows!= 1 || arg_ncols != 1 || !mxIsDouble(prhs[2]) || mxIsComplex(prhs[2]))
    mexErrMsgTxt("Input must be noncomplex double  scalar.\n");

  arg_nrows = mxGetM(prhs[3]);
  arg_ncols = mxGetN(prhs[3]);
  if (arg_nrows!= 1 || arg_ncols != 1 || !mxIsDouble(prhs[3]) || mxIsComplex(prhs[3]))
    mexErrMsgTxt("Input must be noncomplex double  scalar.\n");


  mymex(mxGetPr(prhs[0]),mxGetPr(prhs[1]),x_nrows,*(mxGetPr(prhs[2])),*(mxGetPr(prhs[3])),&(plhs[0]),&(plhs[1]),&(plhs[2]));

  return;
}
