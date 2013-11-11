#include <vcl_iostream.h>
#include <vcl_streambuf.h>
#include <vcl_sstream.h>
#include <math.h>
#include <vgl/algo/vgl_fit_conics_2d.h>
#include <dbgl/algo/dbgl_conic_arc.h>
#include <dbgl/algo/dbgl_fit_circ_arc_spline.h>

#include "vxl_mex_utils.h"

extern void _main();

static void 
mymex(
      double *pts,
      mwSize pts_size,
      const vcl_string &method,
      mwSize min_length,
      double subsample_step_size,
      double tol,
      mxArray **ptr_subsamples,
      mxArray **ptr_endpoints,
      mxArray **ptr_conics
      )
{
  vcl_vector<vgl_point_2d<double> > pts_vxl(pts_size);

  matlab_to_vgl(pts, &pts_vxl);
  
  vcl_cout << "Method is conic\n\n";

  // Core processing.
  if (method == "conic") { 
    vgl_fit_conics_2d<double> fit;
    fit.set_min_fit_length(min_length);
    fit.set_rms_error_tol(tol);
    fit.add_curve(pts_vxl);
    bool retval = fit.fit();

    if (!retval) {
      vcl_cout << "Error in fit()\n";
      vcl_cerr << "Error in fit()\n";
    }
    assert(retval);

    vcl_vector<vgl_conic_segment_2d<double> > segs = fit.get_conic_segs();

    vcl_cout << "Number of segments: " << segs.size() << vcl_endl;

    vcl_vector<vgl_point_2d<double> > subsamples;
    subsamples.reserve(segs.size()*static_cast<unsigned>(ceil(1.0/subsample_step_size+10)));

    vcl_vector<vgl_point_2d<double> > endpoints;
    endpoints.reserve(2*segs.size());
    // transform to a parametric curve and finely sample it.
    for (unsigned is=0; is < segs.size(); ++is) {
      dbgl_conic_arc s;
      s.set_from(segs[is]);
      for (double t=0; t <= 1; t += subsample_step_size) {
        subsamples.push_back(s.point_at(t));
      }
      vcl_cout << "Switching samples at " << subsamples.size() << vcl_endl;

      endpoints.push_back(segs[is].point1());
      endpoints.push_back(segs[is].point2());
    }

    *ptr_subsamples = vgl_to_matlab(subsamples);
    *ptr_endpoints  = vgl_to_matlab(endpoints);
    *ptr_conics     = vgl_to_matlab(segs);

  } else {
    // Circular arc fitting

    vcl_vector<dbgl_circ_arc> segs;

    vcl_cout << "tolerance = " << tol << vcl_endl;
    bool retval = dbgl_fit_circ_arc_spline_to_polyline(segs, pts_vxl, tol);
    if (!retval) {
      vcl_cout << "Error in fit()\n";
      vcl_cerr << "Error in fit()\n";
    }
    assert(retval);

    vcl_cout << "Number of segments: " << segs.size() << vcl_endl;

    vcl_vector<vgl_point_2d<double> > subsamples;
    subsamples.reserve(
        segs.size()*static_cast<unsigned>(ceil(1.0/subsample_step_size+10)));

    vcl_vector<vgl_point_2d<double> > endpoints;
    endpoints.reserve(2*segs.size());
    // transform to a parametric curve and finely sample it.
    for (unsigned is=0; is < segs.size(); ++is) {
      for (double t=0; t <= 1; t += subsample_step_size) {
        subsamples.push_back(segs[is].point_at(t));
      }
      endpoints.push_back(segs[is].point1());
      endpoints.push_back(segs[is].point2());
    }

    *ptr_subsamples = vgl_to_matlab(subsamples);
    *ptr_endpoints  = vgl_to_matlab(endpoints);
    *ptr_conics     = vgl_to_matlab(segs);
  }

  return;
}


void
get_args(
   int          nlhs,
   mxArray      ** /*plhs[]*/,
   int          nrhs,
   const mxArray *prhs[],
   double **pts,
   mwSize *pts_size,
   vcl_string *method,
   double *subsample_step_size,
   mwSize *min_length,
   double *tol)
{

  if (nrhs > 5) {
    mexErrMsgTxt("Too many inputs.\n");
  }
  if (nrhs < 1) {
    mexErrMsgTxt("Too few inputs.\n");
  }

  if (nlhs > 3) {
    mexErrMsgTxt("Too many output arguments\n");
  }

  // The input must be nx2 vector of points

  mwSize p0_nrows = mxGetM(prhs[0]);
  mwSize p0_ncols = mxGetN(prhs[0]);

  if (p0_ncols != 2)
    mexErrMsgTxt("Point array must have two columns.\n");

  for (unsigned i=0; i < (unsigned)nrhs; ++i) {
    if (i == 1) {
      mxClassID category = mxGetClassID(prhs[1]);
      if (category != mxCHAR_CLASS)
        mexErrMsgTxt("Third argument must be a string.\n");
    } else {
      if (!mxIsDouble(prhs[i]) || mxIsComplex(prhs[i]))
        mexErrMsgTxt("Input must be noncomplex double arrays.\n");
    }
  }

  for (unsigned i=2; i < (unsigned)nrhs; ++i) {
    mwSize arg_nrows = mxGetM(prhs[i]);
    mwSize arg_ncols = mxGetM(prhs[i]);
    if (arg_nrows!= 1 || arg_ncols != 1)
      mexErrMsgTxt("2nd and up arguments must be double scalars.\n");
  }

  // Read 1 by 1

  *pts= (double *) mxGetPr(prhs[0]);
  *pts_size = p0_nrows;

  // Defaults
  *subsample_step_size = 0.1;
  *min_length = 10;
  *tol = 0.01;
  *method = "conic";


  if (nrhs > 1) {

    char *buf;
    mwSize buflen = mxGetNumberOfElements(prhs[1]) + 1;
    buf = (char *) mxCalloc(buflen, sizeof(char));

    if (mxGetString(prhs[1], buf, buflen) != 0)
      mexErrMsgTxt("Could not convert string data.\n");

    *method = vcl_string(buf);

    if (nrhs > 2) {
      *subsample_step_size = *(mxGetPr(prhs[2]));
      if (nrhs > 3) {
        *min_length = static_cast<unsigned>(*(mxGetPr(prhs[3])));
        if (nrhs > 4) {
          *tol = *(mxGetPr(prhs[4]));
        }
      }
    }
  }
}


// Fits a contiguous set of conic segments to a sampled curve.
// A conic segment is incrementally fit to the curve until the tolerance
// is exceeded. When the tolerance is exceeded, the conic segment is 
// output and a new conic fit is started.
// 
// [subsamples, endpoints, conics] = fit_conics(curve, method, ...
//                                   subsample_step_size, min_length, tol)
// 
// Input
//
// curve: nx2 array of points
// method: 'circular', 'conic'
// subsample_step_size: number of steps with which to sample the resuting fit.
// min_length: the smallest number of points to fit with an conic
// tol: the threshold on mean square distance from points to the conic
//
// Output
//
// subsamples: nx2 fine sampling of the conic model
// conics: ns x 6 where ns is the number of conic segments.
//
// The order of the coefficients is: $X^2$, $XY$, $Y^2$, $XW$, $YW$, $W^2$,
// where $W$ is the homogeneous coordinate (sometimes denoted by $Z$).
//
// endpoints: 2ns x 2 where ns is the number of segments. Outputs the endpoints
// for each conic segment.
//
void mexFunction(
     int          nlhs,
     mxArray      *plhs[],
     int          nrhs,
     const mxArray *prhs[]
     )
{
  double *pts; double subsample_step_size, tol;
  mwSize min_length; 
  mwSize pts_size;
  vcl_string method;

  get_args(nlhs, plhs, nrhs, prhs, 
      &pts, &pts_size, &method,
      &subsample_step_size, &min_length,  &tol);

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
  mymex(pts, pts_size, method, min_length, subsample_step_size, tol,
      &(plhs[0]), &(plhs[1]), &(plhs[2]));

  // Finalize buffering stuff for error messages
  mexPrintf("%s",myout_sbuf.str().c_str());
  mexPrintf("%s",myerr_sbuf.str().c_str());

  vcl_cout.rdbuf(cout_sbuf);
  vcl_cerr.rdbuf(cerr_sbuf);
  vcl_flush(vcl_cout);
  vcl_flush(vcl_cerr);
  return;
}
