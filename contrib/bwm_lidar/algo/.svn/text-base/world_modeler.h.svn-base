// This is lemsvxlsrc/contrib/bwm_lidar/algo/world_modeler.h

//:
// \file
// \brief Main engine for World Modeling
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#ifndef world_modeler_t_
#define world_modeler_t_

#include "all_includes.h"

#include "flimap_reader.h"
#include "lidar_labeler.h"

#include <vgl/vgl_box_2d.h>

class world_modeler {
public:

  //: fill infinite values with the median of its 8 neighborhood
  static void median_fill(vil_image_view<double>& image);

  //: read one or more FLIMAP ASCII files and build a pair of LIDAR images (like Buckeye format)
  static void generate_lidar_images(const vcl_string& glob, const vgl_box_2d<double>& bbox,
                                          vil_image_view<double>& return1,
                                          vil_image_view<double>& return2,
                                          vil_image_view<vxl_byte>& rgb_img);

  static void generate_model(const vcl_string input_path, 
                             const lidar_labeling_params params, 
                             vil_image_view<int>& labels, 
                             vil_image_view<unsigned char>& labels_colored, 
                             vil_image_view<unsigned char>& colors, 
                             vnl_matrix<double>& height, 
                             vnl_matrix<int>& occupied);
private:
};

#endif
