// This is lemsvxlsrc/contrib/bwm_lidar/algo/lidar_labeler.h

//:
// \file
// \brief Main Class for the classification LIDAR images
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#ifndef lidar_labeler_t_
#define lidar_labeler_t_


#include "get_connected_components.h"

///#include "all_includes.h"

// The class for that contains parameters for lidar labeling
class lidar_labeling_params 
{
public:
  lidar_labeling_params(
    double pixel_size = 0.75,         // Size of the ground pixel
    double gnd_max_variation = 2.0,   // Maximum variation allowed inside a ground pixel
    double gnd_max_diff = 5.0,        // Maximum vertical distance of a ground pixel from the common ground level
    double bld_max_variation = 2.0,   // Maximum variation inside a building pixel
    double bld_min_height = 5.0,      // Minimum height for a building
    int bld_min_area = 250,           // Minimum area for a building region
    double veg_min_variation = 2.0,   // Minimum variation for a vegetation pixel
    int veg_min_area = 50)            // Minimum area for vegetation component
    : pixel_size_(pixel_size),
    gnd_max_variation_(gnd_max_variation),
    gnd_max_diff_(gnd_max_diff), 
    bld_max_variation_(bld_max_variation), 
    bld_min_height_(bld_min_height),
    bld_min_area_(bld_min_area),
    veg_min_variation_(veg_min_variation),
    veg_min_area_(veg_min_area){}

  double pixel_size_;
  double gnd_max_variation_; 
  double gnd_max_diff_; 
  double bld_max_variation_; 
  double bld_min_height_; 
  int bld_min_area_; 
  double veg_min_variation_; 
  int veg_min_area_; 
};

class lidar_labeler {
public:
  static void label_lidar(const vnl_matrix<double>& first_ret, 
                                 const vnl_matrix<double>& last_ret, 
                                 const lidar_labeling_params params,
                                 const double min_elev,
                                 const double max_elev,
                                 const vcl_string input_path,
                                 vil_image_view<int>& labeled);
  static void join_buildings_and_vegetation(vil_image_view<int>& bld, vil_image_view<int>& veg, vil_image_view<int>& joined);
  static void save_classification_image(const vcl_string out_path, const vnl_matrix<int>& classification);
};

#endif
