// This is vxl_mex_utils.h
#ifndef vxl_mex_utils_h
#define vxl_mex_utils_h
//:
//\file
//\brief Utilities to help interface vxl and matlab.
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 11/06/2008 06:06:11 PM EST
//

#include <vgl/vgl_fwd.h>
#include <mex.h>
#include <vcl_vector.h>

class dbgl_circ_arc;

void matlab_to_vgl(const double *pts, 
    vcl_vector<vgl_point_2d<double> > *pts_vxl_ptr);

mxArray *vgl_to_matlab(const vcl_vector<vgl_point_2d<double> > &pts_vxl);

mxArray *vgl_to_matlab(
    const vcl_vector<vgl_conic_segment_2d<double> > &pts_vxl);

mxArray *vgl_to_matlab(const vcl_vector<dbgl_circ_arc> &pts_vxl);


#endif // vxl_mex_utils_h

