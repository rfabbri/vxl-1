#ifndef det_edge_nonmaxium_suppression_h_
#define det_edge_nonmaxium_suppression_h_

//: 
// \file    det_edge_nonmaximum_suppression.h
// \brief   doing the non-maximum suppression in the edge response field on a 3x3 cube
//          theta is the azimuth angle, phi is the tilt angle everywhere inside the code
// \author  H. Can Aras (can@lems.brown.edu)
// \date    2006-01-19
// 
#include "det_edge_map.h"
#include <vnl/vnl_int_3.h>
#include <vnl/vnl_int_4.h>
#include <vnl/vnl_double_2.h>

struct table2_entry
{
  int face_index;
  vnl_int_4 voxels;
  char coord_plane[3];
};

class det_edge_nonmaximum_suppression
{
  public:
    det_edge_nonmaximum_suppression();
    vnl_int_3 vertex_offset_indices(int n)
    {
      return table1_[n];
    };
    void apply_at_a_voxel();
    det_edge_map apply(det_edge_map const& input);
    // direction must be normalized
    vnl_double_2 find_angles(vgl_vector_3d<double> direction);
    // find the intersected face index
    int find_intersected_face_index(vnl_double_2 &angles);
    int find_opposite_intersected_face_index(int face_index);
    double find_s_parameter(int face_index, vgl_vector_3d<double> direction);
    double interpolate_value_on_face_intersection_point(int face_index, vgl_point_3d<double> point, 
                                                        det_edge_map const& input, int i, int j, int k);
    void find_subvoxel_maximum_s_parameter(double s[3], double f[3], double &subpixel_s, double &subpixel_strength);
  protected:
    void construct_vertex_offset_indices_lookup_table();
    void construct_intersection_face_lookup_table();
  public:

  protected:
    // lookup table for the offset indices of the voxels with respect to the central voxel
    vnl_int_3 table1_[26];
    // lookup table for the intersected face
    struct table2_entry table2_[24];
};

#endif
