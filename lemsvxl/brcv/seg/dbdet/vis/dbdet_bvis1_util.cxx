#include "dbdet_bvis1_util.h"

#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_manager.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>

#define MANAGER bvis1_manager::instance()

void dbdet_bvis1_util::
load_edgemaps_into_frames(const vcl_vector<vcl_string> &edgemaps_fnames,
    bool use_filename)
{
  for (unsigned v=0; v < edgemaps_fnames.size(); ++v) {
    vcl_cout << "Reading " << edgemaps_fnames[v] << vcl_endl;
    bool bSubPixel = true;
    double scale=1.0;
    dbdet_edgemap_sptr em;

    bool retval = dbdet_load_edg(
        edgemaps_fnames[v],
        bSubPixel,
        scale,
        em);

    if (!retval) {
      vcl_cerr << "Could not open edge file " << edgemaps_fnames[v] << vcl_endl;
      return;
    }
    vcl_cout << "N edgels: " << em->num_edgels() << vcl_endl;

    dbdet_edgemap_storage_sptr es = dbdet_edgemap_storage_new();
    es->set_edgemap(em);
    if (use_filename)
      es->set_name(edgemaps_fnames[v]);
    else
      es->set_name("edgemap116");

    MANAGER->repository()->store_data(es);
    MANAGER->add_to_display(es);
    MANAGER->next_frame();
  }
  MANAGER->first_frame();
}
