#include <vcl_iostream.h>
#include <vcl_streambuf.h>
#include <math.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vpgl/algo/vpgl_fm_compute_ransac.h>
#include <vpgl/algo/vpgl_fm_compute_8_point.h>
#include <vpgl/algo/vpgl_fm_compute_7_point.h>
#include <mvl/FMatrix.h>
#include <mvl/FMatrixComputeLinear.h>
#include <mvl/FMatrixComputeNonLinear.h>
#include <mvl/FMatrixCompute7Point.h>
#include <mvl/FMatrixComputeRANSAC.h>
#include <mvl/HomgInterestPointSet.h>




#include "mex.h"

extern void _main();

static
void mymex(
      double *pts0,
      double *pts1,
      mwSize n,
      const vcl_string &method,
      mxArray **p_fmatrix
      )
{
//  vcl_cout << "Method: " << method << vcl_endl;

  vcl_vector<vsol_point_2d_sptr> p0(n),p1(n);
  for (unsigned i=0; i < (unsigned)n; ++i) {
    p0[i] = new vsol_point_2d(pts0[i],pts0[i+n]);
    p1[i] = new vsol_point_2d(pts1[i],pts1[i+n]);
  }

  vcl_vector<vgl_point_2d<double> > vp0(n),vp1(n);
  vcl_vector<vgl_homg_point_2d<double> > vp0_homg(n),vp1_homg(n);

  for (unsigned i=0; i < (unsigned)n; ++i) {
    vp0[i] = vgl_point_2d<double>(pts0[i],pts0[i+n]);
    vp1[i] = vgl_point_2d<double>(pts1[i],pts1[i+n]);
    vp0_homg[i] = vgl_homg_point_2d<double>(vp0[i]);
    vp1_homg[i] = vgl_homg_point_2d<double>(vp1[i]);
  }

  // ---------------------------------------------------------------
  //  Classic Fundamental Matrix Estimation
  // ---------------------------------------------------------------


  // Fundamental matrix in Matlab to be returned
  mxArray *mx_fm = mxCreateDoubleMatrix(3,3,mxREAL);
  double *p_fm = mxGetPr(mx_fm);

  if (method.substr(0,4) == "vpgl") {
    //
    //  ---- VPGL algorithms ----
    //
    vpgl_fundamental_matrix<double> fm;

    if (method == "vpglransac") {
      // -- VPGL normalized 8point Using RANSAC
      vpgl_fm_compute_ransac fm_estimator;
//      fm_estimator.set_trace_level(1);
      bool retval = fm_estimator.compute(vp0,vp1,fm);

      if(!retval)
        mexWarnMsgTxt("VPGL 8-point RANSAC fundamental matrix estimation was NOT successful\n");
    } else { 
      if (method == "vpgl8pt") {
        // -- VPGL 8 point Code not using RANSAC at all
        vpgl_fm_compute_8_point fmc8(true);
        bool retval = fmc8.compute(vp0_homg,vp1_homg,fm);

        if(!retval)
          mexWarnMsgTxt("Traditional 8-point fundamental matrix estimation was NOT successful\n");
      } else
        mexErrMsgTxt("Invalid method\n");
    }

    double d = 0;
    for (unsigned int i = 0; i < vp0_homg.size(); ++i) {
      vgl_homg_line_2d<double> lr =
        fm.r_epipolar_line( vp1_homg[i] );

      vgl_homg_line_2d<double> ll =
        fm.l_epipolar_line( vp0_homg[i] );

      d += vgl_homg_operators_2d<double>::perp_dist_squared( lr,
                       vp0_homg[i] )
                   + vgl_homg_operators_2d<double>::perp_dist_squared( ll,
                       vp1_homg[i] );
    }
//    vcl_cout << "Epipolar error = " << d << vcl_endl;

//    vcl_cout << "Fundamental Matrix from VPGL \n" << fm.get_matrix() << vcl_endl;
    // Write the fundamental matrix to matlab
    for (unsigned i=0; i < 9; ++i) {
      p_fm[i] = fm.get_matrix()(i%3,i/3);
    }
  } else  {
    if (method.substr(0,3) == "mvl") {
      //
      //  ---- MVL algorithms ----
      //

      FMatrix f;

      if (method == "mvl8pt") {
        // Perform the fit using the normalised 8-point estimator.
        FMatrixComputeLinear computor(true,true);
        f = computor.compute(vp0_homg, vp1_homg);

//        vcl_cout << "FMatrixComputeLinear:\nF = " << f << vcl_endl;
      } else {
        if (method == "mvlransac") {
          // Perform the fit using Phil Torr's RANSAC estimation of Fmatrix

          FMatrixComputeRANSAC computor(true,2);
            f = computor.compute(vp0_homg, vp1_homg);

        } else
          mexErrMsgTxt("Invalid method\n");
      }

//      double d = 0;
//      for (unsigned int i = 0; i < vp0_homg.size(); ++i) {
//        d += f.image1_epipolar_distance_squared(vp0_homg[i], vp1_homg[i]);
//        d += f.image2_epipolar_distance_squared(vp0_homg[i], vp1_homg[i]);
//      }
//      vcl_cout << "Epipolar error = " << d << vcl_endl;

      // Write the fundamental matrix to matlab
      for (unsigned i=0; i < 9; ++i) {
        p_fm[i] = f.get(i%3,i/3);
      }
    } else
      mexErrMsgTxt("Invalid method\n");
  }

  // Return the fundamental matrix
  *p_fmatrix = mx_fm;
  
  return;
}



// [fm] = fmatrix(pts0,pts1,method,ransacparam1,ransacparam2,etc);
// 
// method can be 'vpglransac', 'mvlransac', 'vpgl8pt', 'mvl8pt','vpgl7pt','mvl7pt' etc
//
void mexFunction(
     int          nlhs,
     mxArray      *plhs[],
     int          nrhs,
     const mxArray *prhs[]
     )
{
  vcl_string method;
  if (nrhs > 3) {
    mexErrMsgTxt("Too many inputs.\n");
  }

  if (nrhs == 2) {
    method = vcl_string("vpglransac");
  } else {
    if (nrhs > 2) {
      mxClassID category = mxGetClassID(prhs[2]);
      if (category != mxCHAR_CLASS)
        mexErrMsgTxt("Third argument must be a string.\n");

      char *buf;
      mwSize buflen = mxGetNumberOfElements(prhs[2]) + 1;
      buf = (char *) mxCalloc(buflen, sizeof(char));

      if (mxGetString(prhs[2], buf, buflen) != 0)
        mexErrMsgTxt("Could not convert string data.\n");

      method = vcl_string(buf);
    } else
      mexErrMsgTxt("Inputs (pts0,pts1) are required.\n");
  }

  if (nlhs > 1) {
    mexErrMsgTxt("Too many output arguments\n");
  }

  // The input must be a nx2 vectors of points

  mwSize p0_nrows = mxGetM(prhs[0]);
  mwSize p0_ncols = mxGetN(prhs[0]);

  mwSize p1_nrows = mxGetM(prhs[1]);
  mwSize p1_ncols = mxGetN(prhs[1]);

  if (p0_ncols != 2 || p1_ncols != 2)
    mexErrMsgTxt("Point arrays must have two columns.\n");

  for (unsigned i=0; i < (unsigned)2; ++i) {
    if (!mxIsDouble(prhs[i]) || mxIsComplex(prhs[i]))
      mexErrMsgTxt("Input must be noncomplex double arrays.\n");
  }

  if (p0_nrows != p1_nrows)
    mexErrMsgTxt("Input point arrays of the same size is required.\n");

  double *pts0= (double *) mxGetPr(prhs[0]);
  double *pts1= (double *) mxGetPr(prhs[1]);


  // Initialize buffering stuff for error messages
  vcl_cout.sync_with_stdio(true);
  vcl_cerr.sync_with_stdio(true);

  vcl_streambuf* cout_sbuf = vcl_cout.rdbuf();
  vcl_stringbuf myout_sbuf;
  vcl_cout.rdbuf(&myout_sbuf);

  vcl_streambuf* cerr_sbuf = vcl_cerr.rdbuf();
  vcl_stringbuf myerr_sbuf;
  vcl_cerr.rdbuf(&myerr_sbuf);


  // Main code
  mymex(pts0,pts1,p0_nrows,method,&(plhs[0]));


  // Finalize buffering stuff for error messages
  mexPrintf("%s",myout_sbuf.str().c_str());
  mexPrintf("%s",myerr_sbuf.str().c_str());

  vcl_cout.rdbuf(cout_sbuf);
  vcl_cerr.rdbuf(cerr_sbuf);
  vcl_flush(vcl_cout);
  vcl_flush(vcl_cerr);

  return;
}
