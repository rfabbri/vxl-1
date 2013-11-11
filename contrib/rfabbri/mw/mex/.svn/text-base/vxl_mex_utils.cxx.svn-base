#include "vxl_mex_utils.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_conic_segment_2d.h>
#include <dbgl/algo/dbgl_circ_arc.h>

// Assumes pts_vxl_ptr already has the right length.
void 
matlab_to_vgl(const double *pts, vcl_vector<vgl_point_2d<double> > *pts_vxl_ptr)
{
  vcl_vector<vgl_point_2d<double> > &pts_vxl = *pts_vxl_ptr;

  unsigned n = pts_vxl.size();
  for (unsigned i=0; i < n; ++i)
    pts_vxl[i] = vgl_point_2d<double>(pts[i],pts[i+n]);
}

mxArray *
vgl_to_matlab(const vcl_vector<vgl_point_2d<double> > &pts_vxl)
{
  unsigned n = pts_vxl.size();

  mxArray *mx_pts = mxCreateDoubleMatrix(n,2,mxREAL);

  double *pts = mxGetPr(mx_pts);

  for (unsigned i=0; i < n; ++i) {
    pts[i] = pts_vxl[i].x();
    pts[i+n] = pts_vxl[i].y();
  }

  return mx_pts;
}

// Given a vcl_vector of conic segments, outputs into a matlab nc x 6 arrays
// with the coefficients of the conics for each segment.
//
// The order of the coefficients is: $X^2$, $XY$, $Y^2$, $XW$, $YW$, $W^2$,
// where $W$ is the homogeneous coordinate (sometimes denoted by $Z$).
mxArray *
vgl_to_matlab(const vcl_vector<vgl_conic_segment_2d<double> > &pts_vxl)
{
  unsigned n = pts_vxl.size();

  mxArray *mx_pts = mxCreateDoubleMatrix(n,6,mxREAL);

  double *pts = mxGetPr(mx_pts);

  for (unsigned i=0; i < n; ++i) {
    const vgl_conic<double> &c = pts_vxl[i].conic();
    pts[i] = c.a();
    pts[i+n] = c.b();
    pts[i+2*n] = c.c();
    pts[i+3*n] = c.d();
    pts[i+4*n] = c.e();
    pts[i+5*n] = c.f();
  }

  return mx_pts;
}

// Given a vcl_vector of circular arc segments, outputs into a matlab nc x 6
// arrays with the coefficients of the arcs as conics, for each segment.
//
// The order of the coefficients is: $X^2$, $XY$, $Y^2$, $XW$, $YW$, $W^2$,
// where $W$ is the homogeneous coordinate (sometimes denoted by $Z$).
mxArray *
vgl_to_matlab(const vcl_vector<dbgl_circ_arc> &pts_vxl)
{
  unsigned n = pts_vxl.size();

  mxArray *mx_pts = mxCreateDoubleMatrix(n,6,mxREAL);

  double *pts = mxGetPr(mx_pts);

  for (unsigned i=0; i < n; ++i) {
    double r = 1.0/pts_vxl[i].k();
    vgl_homg_point_2d<double> center(pts_vxl[i].center());
    const vgl_conic<double> c(center, r, r, 0);
    pts[i] = c.a();
    pts[i+n] = c.b();
    pts[i+2*n] = c.c();
    pts[i+3*n] = c.d();
    pts[i+4*n] = c.e();
    pts[i+5*n] = c.f();
  }

  return mx_pts;
}
