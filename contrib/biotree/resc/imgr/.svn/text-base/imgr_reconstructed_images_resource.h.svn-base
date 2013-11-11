#ifndef imgr_reconstructed_images_resource_h_
#define imgr_reconstructed_images_resource_h_
//: 
// \file  imgr_reconstructed_images_resource.h
// \brief  an interface to provide information from FBP recosntruction coordinates to Bio-tree standard Coordinates
// \author    Kongbin Kang
// \date        2006-02-17
// 

#include <vgl/vgl_point_3d.h>

class imgr_reconstructed_images_resource
{
  public:
    //: reconstructed 3d image coordinates to bio-tree standard coordinates
    virtual vgl_point_3d<double> fbpc_to_bsc(vgl_point_3d<double> const& pt)=0;
    
    virtual vgl_point_3d<double> bsc_to_fbpc(vgl_point_3d<double> const& pt)=0;

    virtual ~imgr_reconstructed_images_resource(){};
  protected:
};
#endif
