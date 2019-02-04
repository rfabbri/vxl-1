#include "dbdet_bvis1_util.h"

#include <vul/vul_file.h>
#include <vnl/vnl_vector_fixed.h>
#include <bsold/bsold_file_io.h>
#include <dbdet/algo/dbdet_cem_file_io.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_util.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>


#define MANAGER bvis1_manager::instance()

void dbdet_bvis1_util::
load_edgemaps_into_frames(const std::vector<std::string> &edgemaps_fnames,
    bool use_filename)
{
  for (unsigned v=0; v < edgemaps_fnames.size(); ++v) {
    std::cout << "Reading " << edgemaps_fnames[v] << std::endl;
    bool bSubPixel = true;
    double scale=1.0;
    dbdet_edgemap_sptr em;

    bool retval = dbdet_load_edg(
        edgemaps_fnames[v],
        bSubPixel,
        scale,
        em);

    if (!retval) {
      std::cerr << "Could not open edge file " << edgemaps_fnames[v] << std::endl;
      return;
    }
    std::cout << "N edgels: " << em->num_edgels() << std::endl;

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

void dbdet_bvis1_util::
load_curve_frags_into_frames(const std::vector<std::string> &cfrags_fnames, bool use_filenames)
{
  std::cerr << "XXX load curve frags\n";
  std::cerr << "XXX load curve frags :::: size" << cfrags_fnames.size() << "\n";
  for (unsigned v=0; v < cfrags_fnames.size(); ++v) {
    std::vector< vsol_spatial_object_2d_sptr > contours;

    std::string ext = vul_file::extension(cfrags_fnames[v]);
    std::cerr << "XXX load curve frags :::: ext" << ext << "\n";
    if (ext == ".vsl") { // binary format
      vsl_b_ifstream bp_in(cfrags_fnames[v].c_str());
      if (!bp_in) {
        std::cout << " Error opening file  " << cfrags_fnames[v] << std::endl;
        return;
      }

      std::cout << "Opened vsl file " << cfrags_fnames[v] <<  " for reading" << std::endl;

      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      output_vsol->b_read(bp_in);

      //: clone

      vidpro1_vsol2D_storage_sptr output_vsol_2;
      output_vsol_2.vertical_cast(output_vsol->clone());

      output_vsol_2->set_frame(-10); //:< means its not in rep
      // try to copy by hand if doesnt work

      MANAGER->repository()->store_data(output_vsol_2);
      MANAGER->add_to_display(output_vsol);
    } else if (ext == ".cem") {
      // create the sel storage class
      dbdet_sel_storage_sptr os= dbdet_sel_storage_new();

      //get pointers to the data structures in it
      dbdet_curve_fragment_graph &CFG = os->CFG();
      dbdet_edgemap_sptr EM = dbdet_load_cem(cfrags_fnames[v], CFG);
      os->set_EM(EM);

      if (!EM) { 
        std::cerr << "SEL storage read problem" << std::endl;
        return;
      }
      
      if (use_filenames)
        os->set_name(cfrags_fnames[v]);
      else
        os->set_name("original_cfrags");  // this exact name is used by tools

      MANAGER->repository()->store_data(os);
      MANAGER->add_to_display(os);

      //create the edgemap storage class
      // dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
      // output_edgemap->set_edgemap(EM);

      // storage_list.push_back(output_edgemap);
    } else {
      // try cemv style (like '-bvsol' in dbdet_load_cem_process
      // if you alternatively want to load your .cem as vsols, just 
      // use .cemv on the same files
      std::cerr << "XXX .cemv loadinmg" << "load_curve_frags" << std::endl;
      bool retval = bsold_load_cem(contours, cfrags_fnames[v]);
      if (!retval) {
        std::cerr << "Could not open frag file " << cfrags_fnames[v] << std::endl;
        return;
      }
      std::cout << "N curves: " << contours.size() << std::endl;

      vidpro1_vsol2D_storage_sptr cs = vidpro1_vsol2D_storage_new();
      cs->add_objects(contours, cfrags_fnames[v]);
      if (use_filenames)
        cs->set_name(cfrags_fnames[v]);
      else
        cs->set_name("original_cfrags");  // this exact name is used by tools

      MANAGER->repository()->store_data(cs);
      MANAGER->add_to_display(cs);
    }
    MANAGER->next_frame();
  }
  MANAGER->first_frame();
}


void dbdet_bvis1_util::
load_img_edg(
    const std::vector<std::string> &imgs_orig, 
    const std::vector<std::string> &edges, 
    const std::vector<std::string> &frags,
    bool repeat_img)
{
  std::vector<std::string> imgs(imgs_orig);
  vnl_vector<unsigned> v(3);
  v[0] = imgs.size(); 
  v[1] = edges.size();
  v[2] = frags.size();

  unsigned nframes=v.max_value();
  std::cout << "nframes = " << nframes << std::endl;
  for (unsigned i=0; i < nframes; ++i) {
    MANAGER->add_new_frame();
    if (repeat_img && imgs.size() < nframes && imgs.size())
      imgs.push_back(imgs.front());
  }
  MANAGER->first_frame();

  bvis1_util::load_imgs_into_frames(imgs, true);
  dbdet_bvis1_util::load_edgemaps_into_frames(edges, true);
  dbdet_bvis1_util::load_curve_frags_into_frames(frags, true);

  MANAGER->post_redraw();
}
