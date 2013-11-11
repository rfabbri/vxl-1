// This is contrib/shock2d/gui/sel_gui_main.cxx

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#include "sel_gui_menu.h"

#include <bvis1/bvis1_macros.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_displayer_sptr.h>
#include <vidpro1/vidpro1_process_manager_sptr.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/vidpro1_repository.h>

// Storage type header files
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_sel_storage.h>

#include <edge_det/pro/edge_corr_storage.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>

#include <dbdet/vis/dbdet_edgemap_displayer.h>
#include <dbdet/vis/dbdet_sel_displayer.h>

#include <edge_det/vis/edge_corr_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_RGB_to_IHS_process.h>
#include <vidpro1/process/vidpro1_RGB_to_CIELab_process.h>
#include <vidpro1/process/vidpro1_split_into_RGB_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_resample_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
//#include <vidpro1/process/vidpro1_Rothwell_edge_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_background_diff_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>
#include <vidpro1/process/vidpro1_load_vsol_process.h>
#include <vidpro1/process/vidpro1_save_vsol_process.h>
//#include <vidpro1/process/vidpro1_smoothcem_process.h>
#include <dbsol/pro/dbsol_smooth_contours_process.h>


#include <dbil/pro/dbil_wshed2d_process.h>

#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_image_gradient_process.h>
#include <dbdet/pro/dbdet_generic_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_composite_edge_detection_process.h>
#include <dbdet/pro/dbdet_detect_topographic_curves_process.h>
#include <dbdet/pro/dbdet_generic_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>

#if OCTAVE_FOUND
#include <dbdet/pro/dbdet_pb_edge_detector_process.h>
#include <dbdet/pro/dbdet_kovesi_edge_linker_process.h>
#endif

#include <dbdet/pro/dbdet_compass_edge_detector_process.h>
#include <dbdet/pro/dbdet_compass_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_multiscale_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_multicue_edge_detector_process.h>
#include <dbdet/pro/dbdet_combine_contour_cues_process.h>
#include <dbdet/pro/dbdet_edgemap_from_edge_image_process.h>
#include <dbdet/pro/dbdet_prune_edgemap_process.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>
#include <dbdet/pro/dbdet_save_cem_process.h>
#include <dbdet/pro/dbdet_load_cvlet_map_process.h>
#include <dbdet/pro/dbdet_save_cvlet_map_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbdet/pro/dbdet_hybrid_linker_process.h>
#include <dbdet/pro/dbdet_evaluation_process.h>//By Yuliang Guo
#include <dbdet/pro/dbdet_convert_edgemap_to_image_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_prune_fragments_Logistic_Regression.h> // By Naman Kumar
#include <dbdet/pro/dbdet_subsample_edges.h> 

//local processes
#include <edge_det/pro/correlate_edge_maps_process.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);

  // Register the displayers
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );

  REG_DISPLAYER( dbdet_edgemap_displayer );  
  REG_DISPLAYER( dbdet_sel_displayer );

  REG_DISPLAYER( edge_corr_displayer );

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );

  REG_STORAGE( dbdet_edgemap_storage );
  REG_STORAGE( dbdet_sel_storage );

  REG_STORAGE( edge_corr_storage );
  
  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_save_image_process );
  REG_PROCESS( vidpro1_save_video_process );
  REG_PROCESS( vidpro1_brightness_contrast_process );
  REG_PROCESS( vidpro1_RGB_to_IHS_process );
  REG_PROCESS( vidpro1_RGB_to_CIELab_process );
  REG_PROCESS( vidpro1_split_into_RGB_process );
  REG_PROCESS( vidpro1_VD_edge_process );
  //REG_PROCESS( vidpro1_Rothwell_edge_process );
  REG_PROCESS( vidpro1_harris_corners_process );
  REG_PROCESS( vidpro1_resample_process );
  REG_PROCESS( vidpro1_gaussian_blur_process );
  REG_PROCESS( vidpro1_grey_image_process ); 
  REG_PROCESS( vidpro1_frame_diff_process );   
  REG_PROCESS( vidpro1_motion_process );
  REG_PROCESS( vidpro1_background_diff_process );
  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_load_cem_process );
  REG_PROCESS( vidpro1_save_cem_process );
  REG_PROCESS( vidpro1_save_con_process );
  REG_PROCESS( vidpro1_edgeprune_process );
  REG_PROCESS( vidpro1_load_vsol_process );
  REG_PROCESS( vidpro1_save_vsol_process );
  REG_PROCESS( dbsol_smooth_contours_process );

  // --- other processes ---
  REG_PROCESS( dbdet_contour_tracer_process );
  REG_PROCESS( dbdet_image_gradient_process );
  REG_PROCESS( dbdet_generic_edge_detector_process );
  REG_PROCESS( dbdet_third_order_edge_detector_process );
  REG_PROCESS( dbdet_composite_edge_detection_process );
  REG_PROCESS( dbdet_detect_topographic_curves_process );
  REG_PROCESS( dbdet_generic_color_edge_detector_process );
  REG_PROCESS( dbdet_third_order_color_edge_detector_process );
  
  

#if OCTAVE_FOUND
  REG_PROCESS( dbdet_pb_edge_detector_process );
  REG_PROCESS( dbdet_kovesi_edge_linker_process );
#endif

  REG_PROCESS( dbdet_generic_multiscale_edge_detector_process );
  REG_PROCESS( dbdet_generic_multicue_edge_detector_process );
  REG_PROCESS( dbdet_combine_contour_cues_process );
  REG_PROCESS( dbdet_compass_edge_detector_process );
  REG_PROCESS( dbdet_compass_color_edge_detector_process );
  REG_PROCESS( dbdet_edgemap_from_edge_image_process );
  REG_PROCESS( dbdet_prune_edgemap_process );
  REG_PROCESS( dbdet_load_edg_process );
  REG_PROCESS( dbdet_save_edg_process );
  REG_PROCESS( dbdet_load_cem_process );
  REG_PROCESS( dbdet_save_cem_process );
  REG_PROCESS( dbdet_load_cvlet_map_process );
  REG_PROCESS( dbdet_save_cvlet_map_process );
  REG_PROCESS( dbdet_sel_process );
  REG_PROCESS( dbil_wshed2d_process );
  REG_PROCESS( dbdet_sel_extract_contours_process );
  REG_PROCESS( dbdet_generic_linker_process );
  REG_PROCESS( dbdet_hybrid_linker_process );
  REG_PROCESS( dbdet_prune_curves_process );
  REG_PROCESS( dbdet_prune_fragments_Logistic_Regression );// Naman Kumar
  
  REG_PROCESS( dbdet_convert_edgemap_to_image_process ); //by Nhon, Added by Yuliang 2011/03/07
  REG_PROCESS( dbdet_evaluation_process ); //By Yuliang Guo

  REG_PROCESS( dbdet_subsample_edges );

  //local processes
  REG_PROCESS( correlate_edge_maps_process );


  sel_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 512, h = 512;
  vcl_string title = "Edge Detection and Linking GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );

  //Amir:: The status bar operations are confusing and often unpredictable
  //until we can get it right, I think it's best to get used to the Ctrl-middlemousebutton
  //to pan the views instead of using the scroll bars

  //win->set_statusbar(true);
  //win->enable_vscrollbar(true);
  //win->enable_hscrollbar(true);
  win->show();
  return vgui::run(); 
}

