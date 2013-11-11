#include <vcl_iostream.h>
#include <vcl_streambuf.h>
#include <vcl_sstream.h>
#include <math.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/vgl_conic.h>
#include </home/rfabbri/src/GeometricTools/WildMagic4/SDK/Include/Wm4DistVector3Quadratic3.h>
#include </home/rfabbri/src/GeometricTools/WildMagic4/SDK/Include/Wm4Vector3.h>

#include "mex.h"

extern void _main();

static
void mymex(
      double *pts,
      double *q,
      mwSize n,
      mxArray **out_d,
      mxArray **out_cpts
      )
{

  // Fundamental matrix in Matlab to be returned
  mxArray *mx_d = mxCreateDoubleMatrix(n,1,mxREAL);
  double *p_d = mxGetPr(mx_d);

  mxArray *mx_cpts = mxCreateDoubleMatrix(n,3,mxREAL);
  double *p_cpts = mxGetPr(mx_cpts);

//  vgl_conic<double> myconic(c);
  for (unsigned i=0; i < n; ++i) {
//    p_d[i] = vgl_homg_operators_2d<double>::distance_squared(myconic,vgl_homg_point_2d<double>(pts[i],pts[i+n]));

    Wm4::Vector3<double> wm_pt (pts[i], pts[i+n], pts[i+2*n]);

    Wm4::Quadratic3<double> wm_q(q);

    Wm4::DistVector3Quadratic3<double> dist_computer( wm_pt, wm_q);

    p_d[i] = dist_computer.GetSquared();

    Wm4::Vector3<double> pt = dist_computer.GetClosestPoint1 ();
    p_cpts[i] = pt[0];
    p_cpts[i+n] = pt[1];
    p_cpts[i+2*n] = pt[2];
  }


  // Return the fundamental matrix
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
   double **q,
   unsigned *npts)
{

  vcl_string method;
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

  if (p0_ncols != 3)
    mexErrMsgTxt("Point array must have three columns for x,y,z coordinates.\n");

  if (p1_ncols != 1 && p1_nrows != 1)
    mexErrMsgTxt("Quadric argument must be 1D vector of coefficients.\n");

  for (unsigned i=0; i < (unsigned)2; ++i) {
    if (!mxIsDouble(prhs[i]) || mxIsComplex(prhs[i]))
      mexErrMsgTxt("Input must be noncomplex double arrays.\n");
  }

  if (p1_nrows*p1_ncols != 10)
    mexErrMsgTxt("Quadric argument must have 10 elements.\n");

  *pts= (double *) mxGetPr(prhs[0]);
  *q= (double *) mxGetPr(prhs[1]);
  *npts = p0_nrows;
}



// Squared distance of point to conic.
//
// [d,cpts] = p2conic_dist(pts, q)
//
// pts: nx3 vector of points
// d: n - vector: d(i) ==  dist(pts(i,:),q)
// q: coeffs of quadric:
//   Q(x,y,z) = q0 + q1*x + q2*y + q3*z + q4*x*x + q5*x*y + q6*x*z +
//              q7*y*y + q8*y*z + q9*z*z
//
// cpts: nx3 vector of closest points
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

  double *pts, *q; unsigned npts;
  get_args(nlhs,plhs,nrhs,prhs,&pts,&q,&npts);


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
  mymex(pts,q,npts,&(plhs[0]),&(plhs[1]));



  // Finalize buffering stuff for error messages
  mexPrintf("%s",myout_sbuf.str().c_str());
  mexPrintf("%s",myerr_sbuf.str().c_str());

  vcl_cout.rdbuf(cout_sbuf);
  vcl_cerr.rdbuf(cerr_sbuf);
  vcl_flush(vcl_cout);
  vcl_flush(vcl_cerr);

  return;
}
