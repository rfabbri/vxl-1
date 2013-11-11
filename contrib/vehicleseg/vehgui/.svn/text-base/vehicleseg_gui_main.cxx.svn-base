// This is contrib/vehicleseg/gui/vehicleseg_gui_main.cxx

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

#include "vehicleseg_gui_menu.h"

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
#include <dbvrl/pro/dbvrl_region_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/vis/dbbgm_distribution_image_displayer.h>
#include <dbbgm/pro/dbbgm_process.h>
#include <dbbgm/pro/dbbgm_storage.h>

//#include <dbctrk/pro/dbctrk_pairing_curves_process.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <vidpro1/process/vidpro1_brightness_contrast_process.h>
#include <vidpro1/process/vidpro1_harris_corners_process.h>
#include <vidpro1/process/vidpro1_gaussian_blur_process.h>
#include <vidpro1/process/vidpro1_frame_average_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_VD_edge_process.h>
#include <vidpro1/process/vidpro1_grey_image_process.h>
#include <vidpro1/process/vidpro1_motion_process.h>
#include <vidpro1/process/vidpro1_horn_schunck_process.h>
#include <vidpro1/process/vidpro1_frame_diff_process.h>
#include <vidpro1/process/vidpro1_background_diff_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_load_edg_process.h>
#include <vidpro1_robust_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process.h>

#include <vidpro1_fgdetector_process.h>
#include <dbvrl/pro/dbvrl_compute_homography_process.h>
#include <dbvrl/pro/dbvrl_compute_homography_fast_process.h>
#include <dbvrl/pro/dbvrl_register_images_process.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_edgeprune_process.h>
#include <vidpro1/process/vidpro1_smoothcem_process.h>
#include <vidpro1/process/vidpro1_kl_process.h>
#include <vidpro1/process/vidpro1_horn_schunck_process.h>

#include <dbbgm/pro/dbbgm_process.h>
#include <dbvidl/pro/vidpro1_load_boulder_video_process.h>

#include <dbinfo/pro/dbinfo_load_tracks_process.h>
#include <dbinfo/pro/dbinfo_save_tracks_process.h>

#include <dbinfo/pro/dbinfo_multi_track_process.h>
#include <vcl_cstdio.h>




int main(int argc, char** argv)
{

#if defined(VCL_WIN32)
    vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
    _setmaxstdio(2048);
#endif
    vgui::init(argc, argv);

    // Register the displayers
    REG_DISPLAYER( bvis1_image_displayer );
    REG_DISPLAYER( bvis1_vsol2D_displayer );
    REG_DISPLAYER( bvis1_vtol_displayer );
    REG_DISPLAYER( dbbgm_distribution_image_displayer );


    // Register the storage types
    REG_STORAGE( vidpro1_image_storage );
    REG_STORAGE( vidpro1_vsol2D_storage );
    REG_STORAGE( vidpro1_vtol_storage );
    // --- dvidpro storage types ---
    REG_STORAGE( dbbgm_storage );
    REG_STORAGE( dbvrl_region_storage );
    REG_STORAGE( dbbgm_distribution_image_storage );

    // Register all the processes
    REG_PROCESS( vidpro1_load_image_process );
    REG_PROCESS( vidpro1_save_video_process );
    REG_PROCESS( vidpro1_load_video_process );
    REG_PROCESS( vidpro1_brightness_contrast_process );
    REG_PROCESS( vidpro1_VD_edge_process );
    REG_PROCESS( vidpro1_harris_corners_process );
    REG_PROCESS( vidpro1_gaussian_blur_process );
    REG_PROCESS( vidpro1_frame_average_process );
    REG_PROCESS( vidpro1_grey_image_process ); 
    REG_PROCESS( vidpro1_frame_diff_process );  
    REG_PROCESS( vidpro1_motion_process );
    REG_PROCESS( vidpro1_horn_schunck_process );
    REG_PROCESS( vidpro1_background_diff_process );
    REG_PROCESS( vidpro1_load_con_process );
    REG_PROCESS( vidpro1_load_edg_process );
    REG_PROCESS( vidpro1_load_cem_process );
    REG_PROCESS( vidpro1_save_cem_process );
    REG_PROCESS( vidpro1_save_con_process );

    REG_PROCESS( dbbgm_aerial_bg_model_process );
    REG_PROCESS( dbbgm_aerial_fg_uncertainity_detect_process);

    REG_PROCESS( dbvrl_compute_homography_process );
    REG_PROCESS( dbvrl_compute_homography_fast_process );
    REG_PROCESS( dbdet_blob_finder_process );
    REG_PROCESS( dbvrl_register_images_process);
    REG_PROCESS( vidpro1_edgeprune_process );
    REG_PROCESS( vidpro1_smoothcem_process );
    REG_PROCESS( vidpro1_kl_process );
    REG_PROCESS( vidpro1_load_boulder_video_process );
    REG_PROCESS( vidpro1_horn_schunck_process );
    REG_PROCESS( dbinfo_multi_track_process );
      REG_PROCESS( dbinfo_load_tracks_process );
  REG_PROCESS( dbinfo_save_tracks_process );


    // --- other processes ---
    REG_PROCESS( dbbgm_process );

    vehicleseg_gui_menu menubar;
    vgui_menu menu_holder = menubar.setup_menu(); 
    unsigned w = 800, h = 500;
    vcl_string title = "VEHICLE SEGMENTATION 1.3";
    vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
    win->get_adaptor()->set_tableau( bvis1_manager::instance() );

    win->show();
    return vgui::run(); 
}

