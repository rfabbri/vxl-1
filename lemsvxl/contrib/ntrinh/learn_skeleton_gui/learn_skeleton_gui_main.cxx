// GUI main program for learn_skeleton_gui

#include "learn_skeleton_gui_menu.h"

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

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_average_xgraph_storage.h>

// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>

#include <dbsk2d/vis/dbsk2d_shock_displayer.h>
#include <dbsksp/vis/dbsksp_shock_displayer.h>
#include <dbsksp/vis/dbsksp_xgraph_displayer.h>
#include <dbsksp/vis/dbsksp_average_xgraph_displayer.h>

// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_image_sequence_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>

#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>

#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dbsk2d/pro/dbsk2d_save_esf_process.h>





#include <dbsksp/pro/dbsksp_save_shock_graph_xml_process.h>
#include <dbsksp/pro/dbsksp_load_shock_graph_xml_process.h>

#include <dbsksp/pro/dbsksp_save_xgraph_xml_process.h>
#include <dbsksp/pro/dbsksp_load_xgraph_xml_process.h>

#include <dbsksp/pro/dbsksp_trace_shock_boundary_process.h>
#include <dbsksp/pro/dbsksp_convert_shock_graph_to_xshock_graph_process.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>

#include <dbsksp/pro/dbsksp_build_shock_graph_process.h>
#include <dbsksp/pro/dbsksp_shock_snake_process.h>
#include <dbsksp/pro/dbsksp_deformable_shock_process.h>
#include <dbsksp/pro/dbsksp_morph_shock_graph_process.h>
#include <dbsksp/pro/dbsksp_morph_shock_graph_different_topology_process.h>
#include <dbsksp/pro/dbsksp_build_from_xshock_graph_process.h>

#include <dbsksp/pro/dbsksp_edit_distance_process.h>
#include <dbsksp/pro/dbsksp_average_two_xgraphs_process.h>
#include <dbsksp/pro/dbsksp_resample_xgraph_process.h>

#include <dbsksp/pro/dbsksp_normalize_xgraph_process.h>
#include <dbsksp/pro/dbsksp_mirror_xgraph_process.h>


int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );

  REG_DISPLAYER( dbsk2d_shock_displayer );
  REG_DISPLAYER( dbsksp_shock_displayer );
  REG_DISPLAYER( dbsksp_xgraph_displayer );
  REG_DISPLAYER( dbsksp_average_xgraph_displayer );

  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( vidpro1_vtol_storage );
  
  REG_STORAGE( dbsk2d_shock_storage );
  REG_STORAGE( dbsksp_shock_storage );
  REG_STORAGE( dbsksp_xgraph_storage );
  REG_STORAGE( dbsksp_average_xgraph_storage );

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_save_image_process );
  REG_PROCESS( vidpro1_load_image_sequence_process );
  
  REG_PROCESS( vidpro1_load_video_process );

  REG_PROCESS( vidpro1_load_con_process );
  REG_PROCESS( vidpro1_save_con_process );

  REG_PROCESS( dbsk2d_load_esf_process );
  REG_PROCESS( dbsk2d_save_esf_process );

  REG_PROCESS( dbsksp_save_xgraph_xml_process );
  REG_PROCESS( dbsksp_load_xgraph_xml_process );
  


  // shock shape
  REG_PROCESS( dbsksp_build_shock_graph_process );
  REG_PROCESS( dbsksp_shock_snake_process );
  REG_PROCESS( dbsksp_deformable_shock_process );
  REG_PROCESS( dbsksp_trace_shock_boundary_process );
  REG_PROCESS( dbsksp_save_shock_graph_xml_process );
  REG_PROCESS( dbsksp_load_shock_graph_xml_process );
  REG_PROCESS( dbsksp_morph_shock_graph_process );
  REG_PROCESS( dbsksp_morph_shock_graph_different_topology_process );
  REG_PROCESS( dbsksp_build_from_xshock_graph_process );
  REG_PROCESS( dbsksp_convert_shock_graph_to_xshock_graph_process );
  REG_PROCESS( dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process );
  REG_PROCESS( dbsksp_average_two_xgraphs_process );
  REG_PROCESS( dbsksp_edit_distance_process );
  REG_PROCESS( dbsksp_resample_xgraph_process );

  REG_PROCESS( dbsksp_normalize_xgraph_process );
  REG_PROCESS( dbsksp_mirror_xgraph_process );

  learn_skeleton_gui_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "Learn Skeleton GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}

