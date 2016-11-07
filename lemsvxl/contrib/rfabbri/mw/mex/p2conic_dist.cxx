#include <vcl_iostream.h>
#include <vcl_streambuf.h>
#include <vcl_sstream.h>
#include <math.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/vgl_conic.h>


#include "mex.h"

extern void _main();

static
void mymex(
      double *pts,
      double *c,
      mwSize n,
      mxArray **out_d,
      mxArray **out_cpts
      )
{

  // distance value in Matlab to be returned
  mxArray *mx_d = mxCreateDoubleMatrix(n,1,mxREAL);
  mxArray *mx_cpts = mxCreateDoubleMatrix(n,2,mxREAL);
  double *p_d = mxGetPr(mx_d);
  double *p_cpts = mxGetPr(mx_cpts);

  vgl_conic<double> myconic(c);
  for (unsigned i=0; i < n; ++i) {
    vgl_homg_point_2d<double> cpt = vgl_homg_operators_2d<double>::
      closest_point(myconic, vgl_homg_point_2d<double>(pts[i],pts[i+n]));

    p_d[i] = vgl_homg_operators_2d<double>::distance_squared(myconic, cpt);

    p_cpts[i]   = cpt.x()/cpt.w();
    p_cpts[i+n] = cpt.y()/cpt.w();
  }

  // Return the distance
  *out_d = mx_d;
  *out_cpts = mx_cpts;

  return;
}

void
get_args(
   int          nlhs,
   mxArray      *plhs[],
   int          nrhs,
   const mxArray *prhs[],
   double **pts,
   double **c,
   unsigned *npts)
{

  if (nrhs > 2) {
    mexErrMsgTxt("Too many inputs.\n");
  }
  if (nrhs < 2) {
    mexErrMsgTxt("Too few inputs.\n");
  }

  if (nlhs > 2) {
    mexErrMsgTxt("Too many output arguments\n");
  }

  // The input must be nx2 vector of points

  mwSize p0_nrows = mxGetM(prhs[0]);
  mwSize p0_ncols = mxGetN(prhs[0]);

  mwSize p1_nrows = mxGetM(prhs[1]);
  mwSize p1_ncols = mxGetN(prhs[1]);

  if (p0_ncols != 2)
    mexErrMsgTxt("Point array must have two columns.\n");

  if (p1_ncols != 1 && p1_nrows != 1)
    mexErrMsgTxt("Conic argument must be 1D vector.\n");

  for (unsigned i=0; i < (unsigned)2; ++i) {
    if (!mxIsDouble(prhs[i]) || mxIsComplex(prhs[i]))
      mexErrMsgTxt("Input must be noncomplex double arrays.\n");
  }

  if (p1_nrows*p1_ncols != 6)
    mexErrMsgTxt("Conic argument must have 6 elements.\n");

  *pts= (double *) mxGetPr(prhs[0]);
  *c= (double *) mxGetPr(prhs[1]);
  *npts = p0_nrows;
}



// Squared distance of point to conic.
//
// [d, cpt] = p2conic_dist(pts, c)
//
// pts: nx2 vector of points
// d: n - vector: d(i) ==  dist(pts(i,:),c)
// c: coeffs of (x^2, xy, y^2, xw, yw, w^2) where w is the homogeneous factor
// cpt: nx2 vector of closest points
//
//
// \return squared distances.
//
void mexFunction(
     int          nlhs,
     mxArray      *plhs[],
     int          nrhs,
     const mxArray *prhs[]
     )
{

  double *pts, *c; unsigned npts;
  get_args(nlhs,plhs,nrhs,prhs,&pts,&c,&npts);


  // Initialize buffering stuff for error messages
  vcl_cout.sync_with_stdio(true);
  vcl_cerr.sync_with_stdio(true);

  vcl_streambuf* cout_sbuf = vcl_cout.rdbuf();
  vcl_stringbuf myout_sbuf;
  vcl_cout.rdbuf(&myout_sbuf);

  vcl_streambuf* cerr_sbuf = vcl_cerr.rdbuf();
  vcl_stringbuf myerr_sbuf;
  vcl_cerr.rdbuf(&myerr_sbuf);



  // ---- Main code ----
  mymex(pts, c, npts, &(plhs[0]), &(plhs[1]));



  // Finalize buffering stuff for error messages
  mexPrintf("%s",myout_sbuf.str().c_str());
  mexPrintf("%s",myerr_sbuf.str().c_str());

  vcl_cout.rdbuf(cout_sbuf);
  vcl_cerr.rdbuf(cerr_sbuf);
  vcl_flush(vcl_cout);
  vcl_flush(vcl_cerr);

  return;
}
