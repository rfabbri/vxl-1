#ifndef det_nonmaxium_suppression_h_
#define det_nonmaxium_suppression_h_

//: 
// \file  det_nonmaxium_suppression.h
// \brief  doing the nonmaixium suppression in the response field
// \author    Kongbin Kang  ( kk at lems.brown.edu )
// \date        2005-09-09
// 
#include "det_cylinder_map.h"
#include <vcl_vector.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_double_2.h>

class det_nonmaxium_suppression
{
  private:
    //: number of vertice in one dimension
    int nv_;

    //: vetice indice for each voxel
    vcl_vector<vbl_array_3d<int> > vertice_map_;

    //: vertice in the suppression box
    vcl_vector<vgl_point_3d<double> > vertice_; 

    //: changing from (i, j, k) index to 1d vector index
    inline int vertice_index(int i, int j, int k)
    {
      return i*nv_*nv_ + j*nv_ + k;
    }    

    //: whether the center point of a 2d array is the maxium 
    inline bool is_maxium(const vbl_array_2d<double> & a) const;

    //: use a second degree monger term to interprelate location
    vnl_double_2 quadratic_interp_loc(const vbl_array_2d<double> &a) const;

    void create_vertices(int n, vcl_vector<vbl_array_3d<int> > &vertice_map, 
      vcl_vector<vgl_point_3d<double> > &vertice);

  public:

    //: construct a nonmaxium_compuression class with n x n x n neighbor
    det_nonmaxium_suppression(int n);

    //: apply non-maxium suppression on a cylinder map
    det_cylinder_map apply(det_cylinder_map const& map, double threshold = 0);

    det_cylinder_map apply_with_radius(det_cylinder_map const& input,
                                                              vbl_array_3d<double> radius_map,
                                                              double threshold=0);
    
};

#endif
