#include "bvis1_util.h"

#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_manager.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#define MANAGER bvis1_manager::instance()

void bvis1_util::
load_imgs_into_frames(const std::vector<std::string> &imgs_fnames, bool use_filenames)
{
  for (unsigned v=0; v < imgs_fnames.size(); ++v) {

    std::string image_filename = imgs_fnames[v];

    vil_image_resource_sptr loaded_image 
      = vil_load_image_resource( image_filename.c_str() );
    if( !loaded_image ) {
      std::cerr << "Failed to load image file" << image_filename << std::endl;
      return;
    }
    vidpro1_image_storage_sptr is = vidpro1_image_storage_new();
    is->set_image(loaded_image);
    if (use_filenames)
      is->set_name(image_filename);
    else
      is->set_name("original_image");

    MANAGER->repository()->store_data(is);
    MANAGER->add_to_display(is);
    MANAGER->next_frame();
  }
  MANAGER->first_frame();
}
