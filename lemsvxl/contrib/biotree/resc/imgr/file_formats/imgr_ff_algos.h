#ifndef imgr_ff_algos_h_
#define imgr_ff_algos_h_
//: 
// \file  imgr_algos.h
// \brief  algorithms used in file format related staff`
// \author    Kongbin Kang
// \date        2005-06-09
// 
#include <dbil/dbil_bounded_image_view.h>
#include <vil/vil_image_resource_sptr.h>
#include <vgl/vgl_box_2d.h>

class imgr_ff_algos
{
  public:
    //: allocate a new bounded image from old one and two calibration images
    static  dbil_bounded_image_view<double>* 
      calibrate_image(vil_image_resource_sptr const &v,
          vil_image_resource_sptr const &dark,
          vil_image_resource_sptr const &bright,
          vgl_box_2d<double> const& box);

};
#endif
