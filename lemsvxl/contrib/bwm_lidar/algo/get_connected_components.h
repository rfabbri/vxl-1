// This is lemsvxlsrc/contrib/bwm_lidar/algo/get_connected_components.h

//:
// \file
// \brief Some morphological operations related to Connected Component Analysis
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#ifndef get_connected_components_t_
#define get_connected_components_t_

#include "all_includes.h"

#include "set_operations.h"

class get_connected_components {
public:
  static void label_image(const vil_image_view<bool> &src_image, vil_image_view<int> &dest_image, int min_label_area);
  static void seperate_buildings(const vil_image_view<int> &labeled_image, const vnl_matrix<double>& first_ret, const vnl_matrix<double>& last_ret, vil_image_view<int> &dest_image);
  static void colorize_image(const vil_image_view<int> &src_image, vil_image_view<unsigned char> &dest_image);
  static void remove_empty_labels(vil_image_view<int> &labeled_image);
  static void fill_holes(vil_image_view<int> &labeled_image);
};

#endif
