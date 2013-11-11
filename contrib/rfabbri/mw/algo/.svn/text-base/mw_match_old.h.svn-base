// This is mw_point_matcher.h
#ifndef mw_match_old_h
#define mw_match_old_h
//:
//\file
//\brief Multivew sparse correspondence code
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 04/25/2005 10:03:34 AM EDT
//

#include <vgl/vgl_homg_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

bool trinocular_match();

bool
nearest_match_along_line (
    const vsol_point_2d &pt,
    const vgl_homg_line_2d<double> &l, 
    vsol_polyline_2d_sptr crv, 
    vsol_point_2d *match_pt,
    double maxdist=1.4
    );


#endif // mw_match_old_h
