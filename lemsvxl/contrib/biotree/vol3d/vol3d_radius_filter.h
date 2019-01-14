#ifndef vol3d_radius_filter_h_
#define vol3d_radius_filter_h_

//: 
// \file  vol3d_radius_filter.h
// \brief  radius filter
// \author    Kongbin Kang
// \date        2006-05-25
// \verbatim
//  Modifications:
//   Pradeep aug 28, 2006    added support for vil3d_image_view
// \endverbatim

#include <vector>
#include <valarray>
#include <vbl/vbl_array_3d.h>
#include <vil3d/vil3d_image_view.h>

class vol3d_radius_filter
{
  public:

    // construct a radius with maximal radius r
    vol3d_radius_filter(int r);

    ~vol3d_radius_filter();

    // return densities on each shell at point (i,j,k)
    std::valarray<double> densities(vbl_array_3d<double> const & vol, 
        int i, int j, int k);

    // densities on each shell with radus specified in rprobes
    std::valarray<double> densities(vbl_array_3d<double> const & vol,
        std::valarray<double> const &rprobes, 
        int i, int j, int k);

    // calculating densities for a vil3d_image_view
    std::valarray<double> densities(vil3d_image_view<vxl_uint_16> const & vol_view,
        std::valarray<double> const &rprobes,
        int i,int j,int k);

    //: computer density inside the a sphere of radius r
    double density(vbl_array_3d<double> const & vol, 
        int i, int j, int k, double r);

    double density(vil3d_image_view<vxl_uint_16> const & vol_view,
        int i, int j, int k, int r);

  private:

    struct quadret
    { 
      int x_; int y_; int z_;double d_;
      quadret(int x, int y, int z,double d) : x_(x), y_(y), z_(z),d_(d) {}
    };
    
    vbl_array_3d<double> shells_;
   
    typedef std::vector<quadret > voxels_t; 
    std::vector< voxels_t* > lookup_;

    //: diameter of the sphere size
    int dim_;
};

#endif
