#include <dbbgm/dbbgm_distribution_image.h>


void vsl_add_to_binary_loader(dbbgm_dist_image_base const& b){
      vsl_binary_loader<dbbgm_dist_image_base >::instance().add(b);
    
}

