
#ifndef vol3d_reader_h
#define vol3d_reader_h
//: 
// \file  biosim_cylinder_pyramid.h
// \brief  class for reading a set of images and creating a 3-D view based on the 2-D bounding box
// \author   Pradeep
// \date     2006-07-20
// 
// \verbatim
//  Modifications:
//   Pradeep aug 29, 2006    added a constructor which creates a 3-D view from an input text file
// \endverbatim

#include <vil/vil_image_resource_sptr.h>
#include <vil3d/vil3d_image_resource.h>
#include <vcl_vector.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_pixel_format.h>


class vol3d_reader
{ 
  public:
    //creating a 3d view from an image resource smart pointer & the bounding box
    vol3d_reader(vcl_vector<vil_image_resource_sptr> & img_vec,vgl_box_2d<int> const & roi);

    //creating a 3d view from an input text file
    vol3d_reader(vcl_string input_txt_file);

    //: creating a 3D view from an image source, by the 3D bounding box
    // the name of the input file is expected to end as ####.tif (or any other
    // extension, or a number of 4 digits like 0125.tif
    // if the box is empty, it reads the whole area in the image source
    vol3d_reader(vcl_string input_txt_file, vgl_box_3d<int> const &roi);

    vil3d_image_resource_sptr vil_3d_image_resource() {return res_;}
  
  protected:
    vil3d_image_resource_sptr  res_;

 };

#endif
  
