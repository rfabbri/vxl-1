#ifndef imgr_scan_images_resource_h_
#define imgr_scan_images_resource_h_
//: 
// \file  imgr_scan_images_resource.h
// \brief  an interface to provide information of camera model (scan) and image resource
// \author    Kongbin Kang
// \date        2005-05-12
// 

#include <xscan/xscan_scan.h>
#include <vcl_vector.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource_sptr.h>
#include "imgr_image_view_3d_base_sptr.h"

class imgr_scan_images_resource
{
  public:
    virtual xscan_scan get_scan() const=0;

    virtual void set_scan(xscan_scan scan) = 0;

    virtual vcl_vector<vil_image_resource_sptr> get_images() const =0;

    //:view access, for now we return only a bounded view
    // the full dataset will not fit into memory, so bound to the 3-d region of
    // interest
    imgr_image_view_3d_base_sptr
      get_bounded_view(vgl_box_3d<double> const& bounds) const;
    
    //added virtual destructor
    virtual ~imgr_scan_images_resource(){}

  protected:
    xscan_scan cached_scan_;
};
#endif
