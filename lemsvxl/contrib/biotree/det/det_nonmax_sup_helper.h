#ifndef det_nonmaxium_suppression_helper_h_
#define det_nonmaxium_suppression_helper_h_
//: 
// \file  det_nonmaxium_suppression_helpers.h
// \brief  a class which hold all the functions needed for 
// nonmaxium_suppression 
// \author    Kongbin Kang kk at lems.brown.edu
// \date        2005-09-30
// 
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_vector.h>
#include "det_cylinder_map.h"

class det_nonmax_sup_helper
{
  public:
    //: mark those voxels which intersect with plane with 
    //direction dir and going throught the centerof the box. 
    //Here we assume cutting plane goes throught the origin 
static vbl_array_3d<bool> intersection_flags(
    vgl_vector_3d<double> const& dir, 
    vcl_vector<vgl_point_3d<double> > const &vertice,
    vcl_vector<vbl_array_3d<int> > const & vertice_index);

static vbl_array_2d<double> proj_axis_plane(
    det_cylinder_map const & input,
    int i, int j, int k, // center of box is placed in the input grid
    vgl_vector_3d<double> const & dir,
    vbl_array_3d<bool> const& intersect_flags
    );

//: find closest axis to the normal of cutting plane
static int closest_axis(vgl_vector_3d<double> const& dir);
};

#endif
