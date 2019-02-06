#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_sequence_filename_map.h>
#include <buld/buld_arg.h>
#include <vnl/vnl_vector.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
//#include <vgui/vgui_command.h>
//#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
//#include <vgui/vgui_find.h>
#include "mw_stereo_menu.h"
#include <mw/app/show_contours_process.h>
#include <mw/app/mw_app.h>
//#include <mw/app/mw_data.h>
//#include <mw/app/ctspheres_app.h>
#include <bvis1/bvis1_macros.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_selector_tableau.h>
#include <bvis1/bvis1_displayer_sptr.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vidpro1/vidpro1_process_manager_sptr.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/vidpro1_repository.h>

// Storage type header files
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <vpgld/pro/vpgld_camera_storage.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_keypoint_storage.h>   
#include <dbkpr/pro/dbkpr_corr3d_storage.h>  
#include <bmcsd/pro/bmcsd_discrete_corresp_storage.h>
#include <bmcsd/pro/bmcsd_discrete_corresp_storage_3.h>
#include <bdifd/pro/bdifd_3rd_order_geometry_storage.h>


// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <bvis1/bvis1_util.h>
#include <dbdet/vis/dbdet_sel_displayer.h>
#include <dbdet/vis/dbdet_edgemap_displayer.h>
#include <dbdet/vis/dbdet_keypoint_displayer.h>


// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_resample_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>
#include <vidpro1/process/vidpro1_vsol_scale_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_load_vsol_process.h>
#include <vidpro1/process/vidpro1_save_vsol_process.h>
#include <dbdet/pro/dbdet_load_cvlet_map_process.h>
#include <dbdet/pro/dbdet_save_cvlet_map_process.h>
#include <bsold/pro/bsold_smooth_contours_process.h>





//: register planar regions between images
//#include <dbvrl/pro/dbvrl_region_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_generic_edge_detector_process.h>
#include <dbdet/pro/dbdet_lowe_keypoint_process.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_composite_edge_detection_process.h>
#include <dbdet/pro/dbdet_generic_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_multiscale_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_image_gradient_process.h>
#include <dbdet/pro/dbdet_detect_topographic_curves_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>
#include <dbdet/pro/dbdet_save_cem_process.h>
#include <dbdet/pro/dbdet_convert_edgemap_to_image_process.h>
#include <bild/pro/bild_distance_transform_process.h>
#include <dbkpr/pro/dbkpr_bbf_match_process.h>
#include <dbkpr/pro/dbkpr_span_match_process.h>
#include <dbkpr/pro/dbkpr_reconstruct_process.h>
#include <mw/pro/dbkpr_frenet_reconstruct_process.h>
#include <dbkpr/pro/dbkpr_interp_depth_process.h>
//#include <vpgld/algo/pro/vpgld_vsol_lens_warp_process.h>
//#include <vpgld/algo/pro/vpgld_image_lens_warp_process.h>
#include <bmcsd/pro/bmcsd_load_camera_process.h>
#include <bmcsd/pro/bmcsd_load_discrete_corresp_process.h>
#include <bmcsd/pro/bmcsd_save_discrete_corresp_process.h>
#include <bdifd/pro/bdifd_edgel_data_process.h>
#include <mw/pro/dbdet_combine_curves_process.h>
#include <mw/pro/dbkpr_projective_reconstruct_process.h>
#include <mw/pro/mw_synthetic_corresp_process.h>
#include <mw/pro/mw_project_cube_process.h>
#include <mw/pro/mw_project_polyline_process.h>
#include <bmcsd/pro/bmcsd_project_curve_sketch_process.h>
#include <mw/pro/dbdet_compute_linked_curves_process.h>
#include <mw/pro/mw_extend_corresp_process.h>
#include <mw/pro/dbmcs_nearest_edgels_process.h>
#include <mw/algo/mw_algo_util.h>
#include <dbdet/vis/dbdet_bvis1_util.h>

#ifdef HAS_BGUI3D
#include <bgui3d/bgui3d.h>
#include <dbkpr/vis/dbkpr_corr3d_displayer.h>
#endif

#define MANAGER bvis1_manager::instance()

bool 
on_ascii_dataset() {
  return vul_file::exists("crv-ids.txt");
} 

void
get_fname_vector(std::string pattern, std::vector<std::string> *fnames)
{
   vul_sequence_filename_map map(pattern);
   for (int i = 0; i < map.get_nviews(); ++i) {
     // std::cerr << "reading name: " << map.name(i) << std::endl;
     fnames->push_back(map.name(i));
   }
}

void
load_edgemaps_into_frames_ascii(
    const std::vector<std::string> &imgs_fnames,
    const std::vector<std::string> &pts_fnames,
    const std::vector<std::string> &tgts_fnames
    )
{
  assert(pts_fnames.size() == tgts_fnames.size());
  assert(pts_fnames.size() == imgs_fnames.size());
  
  for (unsigned v=0; v < pts_fnames.size(); ++v) {
    std::cout << "Reading " << pts_fnames[v] << std::endl;
    bool bSubPixel = true;
    double scale=1.0;
    dbdet_edgemap_sptr em;

    bool retval = mw_algo_util::dbdet_load_edg_ascii_separate_files(
        imgs_fnames[v],
        pts_fnames[v],
        tgts_fnames[v],
        bSubPixel,
        scale,
        em);

    if (!retval) {
      std::cerr << "Could not open edge file (and correp tgt file)" << pts_fnames[v] << std::endl;
      return;
    }
    std::cout << "N edgels: " << em->num_edgels() << std::endl;

    dbdet_edgemap_storage_sptr es = dbdet_edgemap_storage_new();
    es->set_edgemap(em);
    es->set_name(pts_fnames[v]);

    MANAGER->repository()->store_data(es);
    MANAGER->add_to_display(es);
    MANAGER->next_frame();
  }
  
  MANAGER->first_frame();

  std::vector<int> view_ids;
  view_ids.push_back(42);
  view_ids.push_back(54);
  view_ids.push_back(62);

  for (unsigned i=0; i < view_ids[0]; ++i)
    MANAGER->next_frame();

  MANAGER->add_new_view(view_ids[1], true);
  MANAGER->add_new_view(view_ids[2], true);
  MANAGER->display_current_frame();

  std::vector< bvis1_view_tableau_sptr > views;
  views = MANAGER->get_views();
  // make curves active
  for (unsigned v=0; v < views.size(); ++v) {
    vgui_selector_tableau &selector = *(views[v]->selector());
    selector.set_active("frame_00" + std::to_string(view_ids[v]) + "-pts-2D.txt");
    selector.active_to_top();
  }

//  MANAGER->display_current_frame();
  MANAGER->post_redraw();
}



/*
 * for eg synthdata dataset, where all images and edgels are in ascii format
 */
void 
load_edg_data_ascii(
    const std::vector<std::string> &imgs_orig, 
    bool repeat_img)
{
  std::vector<std::string> imgs(imgs_orig);
  unsigned nframes = imgs.size();
  std::vector<std::string> pts(imgs_orig);
  std::vector<std::string> tgts(imgs_orig);
  
  std::cout << "nframes = " << nframes << std::endl;
  for (unsigned i=0; i < nframes; ++i) {
    MANAGER->add_new_frame();
    if (repeat_img && imgs.size() < nframes && imgs.size())
      imgs.push_back(imgs.front());
    imgs[i] += std::string(".png");
    pts[i] += std::string("-pts-2D.txt");
    tgts[i] += std::string("-tgts-2D.txt");
  }
  MANAGER->first_frame();

  bvis1_util::load_imgs_into_frames(imgs, true);
  load_edgemaps_into_frames_ascii(imgs, pts, tgts);

  MANAGER->post_redraw();
}

int main(int argc, char** argv)
{

  vgui::init(argc, argv);

#ifdef HAS_BGUI3D
  bgui3d_init();
  //: Initialize specialized Coin3d nodes
  SoCurvel3D::initClass();
#endif

  vul_arg<std::vector<std::string> > a_edges("-edges", "load edgemap .edg(.gz) files (space-separated)");
  vul_arg<std::vector<std::string> > a_frags("-frags", "load curve fragments .cemv(.gz) files (space-separated)");
  vul_arg<std::vector<std::string> > a_imgs("-imgs", "load curve image files (space-separated)");
  vul_arg<std::vector<std::string> > a_cams("-cams", "load camera files (space-separated)");
  vul_arg<bool> a_repeat_img("-repeat_img", "whether to use the same images in every frame");
  vul_arg_parse(argc,argv);

  std::cout << "Number of args: " << std::endl;
  std::cout << a_edges.value_.size() << std::endl;
  std::cout << a_frags.value_.size() << std::endl;
  std::cout << a_imgs.value_.size() << std::endl;
  std::cout << a_cams.value_.size() << std::endl;

  // Register the displayers
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  REG_DISPLAYER( dbdet_edgemap_displayer );  
  REG_DISPLAYER( dbdet_sel_displayer );
  REG_DISPLAYER( dbdet_keypoint_displayer );
#ifdef HAS_BGUI3D
  REG_DISPLAYER( dbkpr_corr3d_displayer );
#endif

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( vpgld_camera_storage );
  REG_STORAGE( dbdet_sel_storage );
  REG_STORAGE( dbdet_edgemap_storage );
  REG_STORAGE( dbdet_keypoint_storage );
  REG_STORAGE( dbkpr_corr3d_storage );
  REG_STORAGE( bmcsd_discrete_corresp_storage );
  REG_STORAGE( bmcsd_discrete_corresp_storage_3 );
  REG_STORAGE( bdifd_3rd_order_geometry_storage );

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_save_image_process );
  REG_PROCESS( vidpro1_save_video_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  REG_PROCESS( vidpro1_vsol_scale_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_resample_process );
  REG_PROCESS( vidpro1_grey_image_process ); 
  REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_edgeprune_process );
  REG_PROCESS( bild_distance_transform_process);

  
  REG_PROCESS( bsold_smooth_contours_process );
  REG_PROCESS( dbdet_lowe_keypoint_process );
  REG_PROCESS( dbdet_generic_edge_detector_process );
  REG_PROCESS( dbdet_sel_process );
  REG_PROCESS( dbdet_sel_extract_contours_process );
  REG_PROCESS( dbdet_compute_linked_curves_process );
  REG_PROCESS( dbdet_contour_tracer_process );
  REG_PROCESS( dbdet_image_gradient_process );
  REG_PROCESS( dbdet_third_order_edge_detector_process );
  REG_PROCESS( dbdet_composite_edge_detection_process );
  REG_PROCESS( dbdet_generic_color_edge_detector_process );
  REG_PROCESS( dbdet_third_order_color_edge_detector_process );
  REG_PROCESS( dbdet_generic_multiscale_edge_detector_process );
  REG_PROCESS( dbdet_generic_linker_process );
  REG_PROCESS( dbdet_convert_edgemap_to_image_process );
  
  REG_PROCESS( dbdet_detect_topographic_curves_process );
  REG_PROCESS( dbdet_load_edg_process );
  REG_PROCESS( dbdet_save_edg_process );
  REG_PROCESS( dbdet_load_cem_process );
  REG_PROCESS( dbdet_save_cem_process );
  REG_PROCESS( dbdet_load_cvlet_map_process );
  REG_PROCESS( dbdet_save_cvlet_map_process );

  REG_PROCESS( dbdet_prune_curves_process );

  REG_PROCESS( dbkpr_bbf_match_process );
  REG_PROCESS( dbkpr_span_match_process );
  REG_PROCESS( dbkpr_reconstruct_process );
  REG_PROCESS( dbkpr_frenet_reconstruct_process );
  REG_PROCESS( dbkpr_projective_reconstruct_process );
  REG_PROCESS( dbkpr_interp_depth_process );
//  REG_PROCESS( vpgld_vsol_lens_warp_process );
//  REG_PROCESS( vpgld_image_lens_warp_process );
  REG_PROCESS( bmcsd_load_camera_process );
  REG_PROCESS( show_contours_process );

  REG_PROCESS( bmcsd_load_discrete_corresp_process );
  REG_PROCESS( bmcsd_save_discrete_corresp_process );
  REG_PROCESS( vidpro1_load_vsol_process );
  REG_PROCESS( vidpro1_save_vsol_process );
  REG_PROCESS( bdifd_edgel_data_process );
  REG_PROCESS( dbdet_combine_curves_process );
  REG_PROCESS( mw_synthetic_corresp_process );
  REG_PROCESS( mw_project_cube_process );
  REG_PROCESS( mw_project_polyline_process );
  REG_PROCESS( bmcsd_project_curve_sketch_process );
  REG_PROCESS( dbmcs_nearest_edgels_process );
  REG_PROCESS( mw_extend_corresp_process );

  mw_stereo_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 3200, h = 1000;
//  unsigned w = 1200, h = 700;
  std::string title = "Brown Eyes";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);

  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->show();

  // If we are on a .txt dataset, guess that and open it

  if (on_ascii_dataset() && !a_cams.set() && !a_imgs.set() && !a_edges.set() && !a_frags.set()) {
    std::cerr << "Loading ascii dataset (such as mcs synth) \n";
    std::vector<std::string> cam_fnames, img_fnames, pt_fnames, tgt_fnames;
//    get_fname_vector("frame_####.extrinsic", &cam_fnames);
    get_fname_vector("frame_####.png", &img_fnames);
  //  get_fname_vector("frame_####-pts-2D.txt", &pt_fnames);
//    get_fname_vector("frame_####-tgts-2D.txt", &tgt_fnames);
    load_edg_data_ascii(img_fnames,false);

//    load_cams_into_frames(cam_fnames, bmcsd_util::BMCS_INTRINSIC_EXTRINSIC);
  } else {
    dbdet_bvis1_util::load_img_edg(a_imgs.value_, a_edges.value_, a_frags.value_, a_repeat_img.value_);
    load_cams_into_frames(a_cams.value_, bmcsd_util::BMCS_3X4);
    // XXX remove
    mw_load_mcs_instance();
  }


  return vgui::run(); 
}


