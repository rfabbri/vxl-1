#ifndef vol3d_radius_detection_h_
#define vol3d_radius_detection_h_
//: 
// \file  vol3d_radius_detection.h
// \brief class of radius detection 
// \author    Kongbin Kang
// \date        2006-06-15
//
// \verbatim
//  Modifications
//  
//   sept.2006 - pradeep - refined the criteria for the radius labeling
// \endverbatim
// 
#include <vbl/vbl_array_3d.h>
#include <vol3d/vol3d_radius_filter.h>
#include <vil3d/vil3d_image_resource.h>

class vol3d_radius_detection
{
  public:
    
    //: minium radii should be larger than 1
    vol3d_radius_detection(int min_rad, int max_rad);

    ~vol3d_radius_detection();
    
    //: given a volume data, return radius on each point. 
    //NOTICE: density_mean is the mean of density inside the cylindric structure.
    //        density_sigma is the variance of the density
    vbl_array_3d<double> radius(vil3d_image_resource_sptr vol_sptr, 
        double density_mean, double density_sigma);
    
    //: use d(rou)/dr to decide radius on each point
    vbl_array_3d<double> radius(vil3d_image_resource_sptr vol_sptr, double sigma);

    vbl_array_3d<double> radius(vil3d_image_resource_sptr vol_sptr,double sigma,float threshold);

      vbl_array_3d<double> radius(vbl_array_3d<double>vol, 
        double density_mean, double density_sigma);
    
    //: use d(rou)/dr to decide radius on each point
    vbl_array_3d<double> radius(vbl_array_3d<double>vol, double sigma);

    vbl_array_3d<double> radius(vbl_array_3d<double>vol,double sigma,float threshold);


    vcl_valarray<double> probes() {return rp_;}

  private:
    // radius probes
    vcl_valarray<double> rp_;


    vol3d_radius_filter *rf_;

    //: a hack because msvc doesn't support log2
    double mylog2(double);
};
#endif
