#include <vcl_iostream.h>
#include <vcl_streambuf.h>
#include <math.h>
#include <mw/algo/mw_qualitative_epipolar.h>
#include <mw/mw_util.h>
#include <vgl/vgl_polygon.h>
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

static mxArray *vgl2matlab( const vcl_list<vgl_polygon<double> > &mypolys);

static
bool mymex(
      double *pts0,
      double *pts1,
      mwSize n,
      double *pbox,
      mxArray **p_mpoly0,
      mxArray **p_mpoly1
      )
{


  vcl_vector<vsol_point_2d_sptr> p0(n),p1(n);
  for (unsigned i=0; i < (unsigned)n; ++i) {
    p0[i] = new vsol_point_2d(pts0[i],pts0[i+n]);
//    mexPrintf("p0: %g,%g\t\t",p0[i]->x(),p0[i]->y());
    p1[i] = new vsol_point_2d(pts1[i],pts1[i+n]);
//    mexPrintf("p1: %g,%g\n",p1[i]->x(),p1[i]->y());
  }

  vgl_box_2d<double> bbox(pbox[0],pbox[1],pbox[2],pbox[3]);
//  mexPrintf("box: xmin:%g,xmax:%g,ymin:%g,ymax:%g\n",pbox[0],pbox[1],pbox[2],pbox[3]);


  mw_qualitative_epipolar epi(p0,p1,bbox);
  bool retval = epi.compute_epipolar_region();


  *p_mpoly0 = vgl2matlab(epi.polys0());
  *p_mpoly1 = vgl2matlab(epi.polys1());

  if(!retval) {
    return false;
  }

  // ---------------------------------------------------------------
  //  Classic Fundamental Matrix Estimation
  // ---------------------------------------------------------------

//  vcl_vector<vgl_point_2d<double> > vp0(n),vp1(n);
//  vcl_vector<vgl_homg_point_2d<double> > vp0_homg(n),vp1_homg(n);

//  for (unsigned i=0; i < (unsigned)n; ++i) {
//    vp0[i] = vgl_point_2d<double>(pts0[i],pts0[i+n]);
//    vp1[i] = vgl_point_2d<double>(pts1[i],pts1[i+n]);
//    vp0_homg[i] = vgl_homg_point_2d<double>(vp0[i]);
//    vp1_homg[i] = vgl_homg_point_2d<double>(vp1[i]);
//  }

  // Fundamental matrix in Matlab to be returned
//  mxArray *mx_fm = mxCreateDoubleMatrix(3,3,mxREAL);
//  double *p_fm = mxGetPr(mx_fm);

#if 0
  //  ---- VPGL algorithms ----
  {
    vpgl_fundamental_matrix<double> fm;
    // -- VPGL normalized 8point Using RANSAC
    /*
    {
      vpgl_fm_compute_ransac fm_estimator;
      retval = fm_estimator.compute(vp0,vp1,fm);

      if(!retval)
        mexWarnMsgTxt("Traditional 8-point RANSAC fundamental matrix estimation was NOT successful\n");
    }
    */
    
    // -- VPGL 8 point Code not using RANSAC at all
    {
      vpgl_fm_compute_8_point fmc8(true);
      retval = fmc8.compute(vp0_homg,vp1_homg,fm);

      if(!retval)
        mexWarnMsgTxt("Traditional 8-point fundamental matrix estimation was NOT successful\n");
    }
    
    /*
      // -- VPGL 7 point Code not using RANSAC at all
    {
      vcl_vector< vpgl_fundamental_matrix<double>* > v_fm;
      
      vpgl_fm_compute_7_point fmc7(true);
      retval = fmc7.compute(vp0_homg,vp1_homg,v_fm);

      if(!retval)
        mexWarnMsgTxt("Traditional 7-point fundamental matrix estimation was NOT successful\n");

      mexPrintf("VPGL 7pt algo returned %d solutions\n",v_fm.size());

      fm = *(v_fm[0]);
    }
    */

    vcl_cout << "Fundamental Matrix from VPGL \n" << fm.get_matrix() << vcl_endl;
    // Write the fundamental matrix to matlab
    //mexPrintf("Fm: ");
    for (unsigned i=0; i < 9; ++i) {
      //mexPrintf("%g\t",fm.get_matrix())
      p_fm[i] = fm.get_matrix()(i%3,i/3);
    }
  }
  
#endif
  //  ---- oxl/mvl 8 Point Linear Least Squares algorithm ----


#if 0

  {
    FMatrix f;
    vcl_vector<FMatrix *> l; 

    // Perform the fit using the normalised 8-point estimator.
    {
      FMatrixComputeLinear computor(true,true);
      f = computor.compute(vp0_homg, vp1_homg);

      vcl_cout << "FMatrixComputeLinear:\nF = " << f << vcl_endl;
    }


    /*
    {
      // Perform the fit using the non-normalised 7-point estimator.
      FMatrixCompute7Point computor(false,true);
      bool retval =  computor.compute(vp0_homg, vp1_homg, l);

      if (!retval)
        mexErrMsgTxt("MVL 7-point fundamental matrix estimation was NOT successful\n");

      mexPrintf("MVL 7pt algo returned %d solutions\n",l.size());

      f = *(l[0]);
      f.set_rank2_using_svd();

      vcl_cout << "FMatrixCompute7Point:\nF = " << f << vcl_endl;
    }
    */

    { // Refine the fit using non-linear minimization method in vpgl

      HomgInterestPointSet hips0(vp0_homg,0);
      HomgInterestPointSet hips1(vp1_homg,0);

      PairMatchSetCorner matches (&hips0, &hips1);

      FMatrixComputeNonLinear computor(&matches);

      bool retval = computor.compute(&f);
      if (!retval)
        mexErrMsgTxt("MVL non-linear fundamental matrix estimation was NOT successful\n");
    }


    double d = 0;
    for (unsigned int i = 0; i < vp0_homg.size(); ++i)
      d += f.image1_epipolar_distance_squared(vp0_homg[i], vp1_homg[i]);
    vcl_cout << "Error = " << d/vp0_homg.size() << vcl_endl;

    // Write the fundamental matrix to matlab
    //mexPrintf("Fm: ");
    for (unsigned i=0; i < 9; ++i) {
      //mexPrintf("%g\t",fm.get_matrix())
      p_fm[i] = f.get(i%3,i/3);
    }
  }
#endif


  // Return the fundamental matrix
//  *p_fmatrix = mx_fm;
  
  return true;
}


//: Vgl list of polygons to matlab cell representation
static mxArray *vgl2matlab(
    const vcl_list<vgl_polygon<double> > &mypolys
    )
{

  mxArray *sheets_cell;
  mxArray *xy;
  mwSize n_sheets; 
  unsigned npts;

  mxArray *mpoly;

  vcl_list< vgl_polygon<double> > :: const_iterator itr=mypolys.begin();

  mwSize n_polys = mypolys.size();
  mpoly = mxCreateCellArray(1,&n_polys);

  mwSize iply=0;
  for (; itr != mypolys.end(); ++itr,++iply) { // for each polygon
    n_sheets = itr->num_sheets();
    assert(n_sheets!=0);
    sheets_cell = mxCreateCellArray(1,&n_sheets);
    mxSetCell(mpoly, iply, sheets_cell);
    for (unsigned is=0; is < itr->num_sheets(); ++is) { // for each sheet
      npts = (*itr)[is].size();
      assert(npts!=0);
//      mexPrintf("Sheet: %i of %i\tNumber of pts:%i\n",is+1,n_sheets,npts);
      xy = mxCreateDoubleMatrix(npts,2,mxREAL);
      double *p=mxGetPr(xy);
      for (unsigned ipt=0; ipt < (*itr)[is].size(); ++ipt) { // for each point
        p[ipt] = (*itr)[is][ipt].x();
        p[ipt+npts] = (*itr)[is][ipt].y();
//        mexPrintf("\t\t%g,%g\n",p[ipt],p[ipt+npts]);
      }
      mxSetCell(sheets_cell, is, xy);
    }
  }

  return mpoly;
}



// Purpose: [poly0,poly1,fmatrix] = episearch(pts0,pts1,box);
void mexFunction(
     int          nlhs,
     mxArray      *plhs[],
     int          nrhs,
     const mxArray *prhs[]
     )
{
  if (nrhs != 3) {
    mexErrMsgTxt("Inputs (pts0,pts1,box) are required.");
  }

  if (nlhs > 2) {
    mexErrMsgTxt("Too many output arguments");
  }

  // The input must be a nx2 vectors of points

  mwSize p0_nrows = mxGetM(prhs[0]);
  mwSize p0_ncols = mxGetN(prhs[0]);

  mwSize p1_nrows = mxGetM(prhs[1]);
  mwSize p1_ncols = mxGetN(prhs[1]);

  mwSize box_nrows = mxGetM(prhs[2]);
  mwSize box_ncols = mxGetN(prhs[2]);

  if (p0_ncols != 2 || p1_ncols != 2)
    mexErrMsgTxt("Point arrays must have two columns.");

  if (box_ncols != 2 || box_nrows != 2)
    mexErrMsgTxt("Box must be 2x2 array [xmin ymin; xmax ymax].");

  for (unsigned i=0; i < (unsigned)nrhs; ++i) {
    if (!mxIsDouble(prhs[i]) || mxIsComplex(prhs[i]))
      mexErrMsgTxt("Input must be noncomplex double arrays.");
  }

  if (p0_nrows != p1_nrows)
    mexErrMsgTxt("Input point arrays of the same size is required.");

  double *pts0= (double *) mxGetPr(prhs[0]);
  double *pts1= (double *) mxGetPr(prhs[1]);
  double *box = (double *) mxGetPr(prhs[2]);

  vcl_cout.sync_with_stdio(true);
  vcl_cerr.sync_with_stdio(true);

  vcl_streambuf* cout_sbuf = vcl_cout.rdbuf();
  vcl_stringbuf myout_sbuf;
  vcl_cout.rdbuf(&myout_sbuf);

  vcl_streambuf* cerr_sbuf = vcl_cerr.rdbuf();
  vcl_stringbuf myerr_sbuf;
  vcl_cerr.rdbuf(&myerr_sbuf);

  bool retval = mymex(pts0,pts1,p0_nrows,box,&(plhs[0]),&(plhs[1]));


  mexPrintf("%s",myout_sbuf.str().c_str());
  mexPrintf("%s",myerr_sbuf.str().c_str());

  vcl_cout.rdbuf(cout_sbuf);
  vcl_cerr.rdbuf(cerr_sbuf);
  vcl_flush(vcl_cout);
  vcl_flush(vcl_cerr);

  if (!retval)
    mexErrMsgTxt("Problem with computing epipolar region\n");

  return;
}
