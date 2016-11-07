// This is contrib/manifold_extraction/vis/outline_matching_visualizer.cxx

#include "outline_matching_menu.h"

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>

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
#include <dbcvr/pro/dbcvr_curvematch_storage.h>

// Process header files

#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>

#include <dbcvr/vis/dbcvr_curvematch_displayer.h>
#include <dbcvr/pro/dbcvr_curvematch_process.h>

#include <manifold_extraction/pro/Lie_contour_match_process.h>
#include <manifold_extraction/pro/Lie_contour_geodesic_process.h>
#include <manifold_extraction/pro/Lie_contour_classify_shapes.h>
#include <manifold_extraction/pro/Lie_contour_generate_geodesic_shapes.h>
#include <manifold_extraction/pro/Lie_contour_principal_geodesic_shapes.h>
#include <manifold_extraction/pro/Lie_contour_mean_process.h>
#include <manifold_extraction/pro/Lie_contour_discrete_mean_process.h>
#include <manifold_extraction/pro/Lie_spoke_mean_process.h>
#include <manifold_extraction/pro/Lie_spoke_geodesic_process.h>
#include <manifold_extraction/pro/Lie_spoke_principal_geodesic_shapes.h>
#include <manifold_extraction/pro/Lie_spoke_classify_shapes.h>
#include <manifold_extraction/pro/shape_articulation_process.h>
#include <manifold_extraction/pro/Lie_contour_match_storage.h>
#include <manifold_extraction/pro/Flip_spoke_process.h>
#include <manifold_extraction/vis/Lie_contour_match_displayer.h>
//#include <dbctrk/pro/dbctrk_pairing_curves_process.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>


#include <vcl_cstdio.h>


int main(int argc, char** argv)
{
    vgui::init(argc, argv);

    // Register the displayers
    REG_DISPLAYER( bvis1_image_displayer );
    REG_DISPLAYER( bvis1_vsol2D_displayer );
    REG_DISPLAYER( bvis1_vtol_displayer );
    REG_DISPLAYER( dbcvr_curvematch_displayer );
    REG_DISPLAYER( Lie_contour_match_displayer );


  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  REG_STORAGE( dbcvr_curvematch_storage );
  REG_STORAGE(Lie_contour_match_storage );
  
  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_save_con_process );
 

    // --- other processes ---
    REG_PROCESS(dbcvr_curvematch_process );
    REG_PROCESS(Lie_contour_match_process );
    REG_PROCESS(shape_articulation_process );
    REG_PROCESS(Lie_contour_geodesic_process );
    REG_PROCESS(Lie_contour_generate_geodesic_shapes );
    REG_PROCESS(Lie_contour_principal_geodesic_shapes );
    REG_PROCESS(Lie_contour_mean_process );
    REG_PROCESS(Lie_contour_discrete_mean_process );
    REG_PROCESS(Lie_contour_classify_shapes );
    REG_PROCESS(Lie_spoke_mean_process );
    REG_PROCESS(Lie_spoke_geodesic_process );
    REG_PROCESS(Lie_spoke_principal_geodesic_shapes );
    REG_PROCESS(Lie_spoke_classify_shapes );
    REG_PROCESS(Flip_spoke_process);
   

  outline_matching_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu(); 
  unsigned w = 512, h = 512;
  vcl_string title = "OUTLINE MATCHING";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );

  win->show();
  return vgui::run(); 
}

