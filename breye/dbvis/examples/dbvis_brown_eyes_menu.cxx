//This is dbvis/dbvis_brown_eyes_menu.cxx

#include "dbvis_brown_eyes_menu.h"
#include <bvis/bvis_video_manager.h>
#include <bvis/menu/bvis_command.h>

#include <dbgui/dbgui_utils.h>

#include <vidpro/vidpro_process_manager.h>

#include <bvis/menu/bvis_file_menu.h>
#include <bvis/menu/bvis_view_menu.h>
#include <bvis/menu/bvis_video_menu.h>
#include <bvis/menu/bvis_repository_menu.h>
#include <bvis/menu/video/bvis_video_add_menu.h>
#include <bvis/menu/bvis_process_macro_menu.h>
#include <bvis/menu/bvis_tableau_menu.h>
//#include <dbvis/menu/dbvis_batch_menu.h>

#include <bvis/bvis_tool.h>
#include <bvis/tool/bvis_vsol2D_tools.h>
#include <bvis/tool/bvis_soview2D_tools.h>
#include <bvis/tool/bvis_correspond_tools.h>

#include <dbru/vis/dbru_rcor_tool.h>
#include <dbru/vis/dbru_labeling_tool.h>

#include <dbmrf/vis/dbmrf_bmrf_tools.h>
#include <dbvrl/vis/dbvrl_region_tools.h>
#include <dbecl/vis/dbecl_episeg_tools.h>
#include <dbetl/vis/dbetl_episeg_tools.h>
#include <dbctrk/vis/dbctrk_get_info_tool.h>
#include <dbctrk/vis/dbctrk_write_info_tool.h>
#include <dbctrk/vis/dbctrk_clustering_tool.h>
#include <dbctrk/vis/dbctrk_clustering_process_tool.h>
#include <dbctrk/vis/dbctrk_matching_tool.h>
#include <dbctrk/vis/dbctrk_transform_tool.h>
#include <dbctrk/vis/dbctrk_ground_truth_tool.h>
#include <dbctrk/vis/dbctrk_alignment_viewer_tool.h>
#include <dbetrk/vis/dbetrk_edge_inspector_tools.h>
#include <dbetrk/vis/dbetrk_particles_tool.h>
#include <dbsk2d/vis/dbsk2d_ishock_detector_debug_tool.h>

#include <dvpgl/vis/dvpgl_epipolar_tool.h>



#ifdef HAS_BGUI3D
#include <dbmrf/vis/dbmrf_curvel_3d_tools.h>
#endif
#ifdef HAS_MATLAB
#include <dbctrk/vis/dbctrk_intensity_prune.h>
#include <dbctrk/vis/dbctrk_intensity_tool.h>
#endif

dbvis_brown_eyes_menu::dbvis_brown_eyes_menu()
{
}

static void render_ps()
{
  dbgui_utils::render_to_ps(bvis_video_manager::instance());
}

vgui_menu
dbvis_brown_eyes_menu::setup_menu()
{
  vgui_menu MainMenu;

  /////////////// File Menu ///////////////
  bvis_file_menu<bvis_video_manager> file_menu;

  //AMIR:added these to make loading and saving more intuitive
  vgui_menu load_menu;
  vgui_menu save_menu;

  //these have to be customized to each application which is why they cannot
  //be defined in bvis_file_menu

  FILE_MENU_ADD_PROCESS_NAME( "Video", "Load Video", load_menu, bvis_video_manager );
  FILE_MENU_ADD_PROCESS_NAME( "Image", "Load Image", load_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Load .bnd File", load_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Load .CON File", load_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( ".EDG", "Load .EDG File", load_menu, bvis_video_manager );
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Load .CEM File", load_menu, bvis_video_manager );

  
  FILE_MENU_ADD_PROCESS_NAME( ".bnd", "Save .bnd File", save_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( ".CEM", "Save .CEM File", save_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( ".CON", "Save .CON File", save_menu, bvis_video_manager);
  FILE_MENU_ADD_PROCESS_NAME( ".PS", "Save .PS File", save_menu, bvis_video_manager);

  file_menu.add("Load..", load_menu);
  file_menu.add("Save..", save_menu);
  file_menu.add("Save to PostScript", render_ps);
  file_menu.separator();

  file_menu.add_to_menu( MainMenu );

  /////////////// Video Menu ///////////////
  bvis_video_menu video_menu;
  video_menu.add_to_menu( MainMenu );

  /////////////// Repository Menu ////////////////////
  bvis_repository_menu<bvis_video_manager> repository_menu;

  repository_menu.add_to_menu( MainMenu );

  /////////////// Add Menu ////////////////////
  bvis_video_add_menu add_menu;

  add_menu.add_to_menu( MainMenu );

  /////////////// Processes Menu ///////////////
  vgui_menu processes_menu;

  
  vgui_menu  image_adjustment_group_menu; // Image Adjustment
    MENU_ADD_PROCESS_NAME( "Brightness/Contrast", image_adjustment_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Gaussian Blur",       image_adjustment_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Convert to Grey",     image_adjustment_group_menu, bvis_video_manager );

  vgui_menu  motion_detection_group_menu; // Motion Detection
    MENU_ADD_PROCESS_NAME( "Frame Difference", motion_detection_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Background Difference", motion_detection_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Compute Motion", motion_detection_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Register Video", motion_detection_group_menu, bvis_video_manager );

  vgui_menu  edge_detection_group_menu; // Edge Detection
    MENU_ADD_PROCESS_NAME( "VD EdgeDetector", edge_detection_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Watershed Transform 2D",  edge_detection_group_menu, bvis_video_manager );
    //MENU_ADD_PROCESS_NAME( "Susan",           edge_detection_group_menu );
    //MENU_ADD_PROCESS_NAME( "ContourTracing",  edge_detection_group_menu );
    MENU_ADD_PROCESS_NAME( "Prune Edges",  edge_detection_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Smooth Edges", edge_detection_group_menu, bvis_video_manager);

  vgui_menu  shock_group_menu; // Shock
    MENU_ADD_PROCESS_NAME( "Compute Shocks",        shock_group_menu, bvis_video_manager );
    //MENU_ADD_PROCESS_NAME( "Sample Shock", shock_group_menu );

  vgui_menu  curve_group_menu; // Curve
    //MENU_ADD_PROCESS_NAME( "Curve Matching", curve_group_menu );
    //MENU_ADD_PROCESS_NAME( "Graduated Assignment", curve_group_menu );
    MENU_ADD_PROCESS_NAME( "Curve Distance Map", curve_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Curve Matching", curve_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Interpolated Curve Matching", curve_group_menu, bvis_video_manager );

  vgui_menu  network_group_menu; // Network
    MENU_ADD_PROCESS_NAME( "Build MRF Network", network_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Build 3D Curves", network_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Extract 2D Curves", network_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Build Episegments", network_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "track Epi-Points", network_group_menu, bvis_video_manager );

  vgui_menu  affine_patch_group_menu; // Affine Patch
    //MENU_ADD_PROCESS_NAME( "Lowe Keypoints",  affine_patch_group_menu );
    //MENU_ADD_PROCESS_NAME( "Match Keypoints", affine_patch_group_menu );
    MENU_ADD_PROCESS_NAME( "Register Region", affine_patch_group_menu, bvis_video_manager );

  vgui_menu  stereo_group_menu;
    //MENU_ADD_PROCESS_NAME( "Stereo Rectification",  stereo_group_menu );
    //MENU_ADD_PROCESS_NAME( "Stereo Dense matching",  stereo_group_menu );

  vgui_menu  other_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Harris Corners", other_group_menu, bvis_video_manager );
    //MENU_ADD_PROCESS_NAME( "Finger algorithm", other_group_menu );


  vgui_menu  ctrk_group_menu; // Other
    MENU_ADD_PROCESS_NAME( "Curve tracking", ctrk_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "Edge tracking", ctrk_group_menu, bvis_video_manager );
    MENU_ADD_PROCESS_NAME( "KL tracking", ctrk_group_menu, bvis_video_manager );

  vgui_menu  eulerspiral_group_menu; // Eulerspiral group
    //MENU_ADD_PROCESS_NAME( "Euler Spiral", eulerspiral_group_menu );
    //MENU_ADD_PROCESS_NAME( "Analyze xshock of two Euler spirals", eulerspiral_group_menu );
    //MENU_ADD_PROCESS_NAME( "Draw contours with Euler spirals", eulerspiral_group_menu );
    //MENU_ADD_PROCESS_NAME( "Analyze ishock of ES contour", eulerspiral_group_menu );

  processes_menu.add( "Image Adjustment", image_adjustment_group_menu );
  processes_menu.add( "Motion Detection", motion_detection_group_menu );
  processes_menu.add( "Edge Detection",   edge_detection_group_menu );
  processes_menu.add( "Shock",            shock_group_menu );
  processes_menu.add( "Curve",            curve_group_menu );
  processes_menu.add( "Network",          network_group_menu);
  processes_menu.add( "Affine Patch",     affine_patch_group_menu);
  processes_menu.add( "tracking",   ctrk_group_menu);
  processes_menu.add( "Euler Spiral", eulerspiral_group_menu);
  processes_menu.add( "Other",            other_group_menu);
  processes_menu.add( "Stereo",            stereo_group_menu);

  

  MainMenu.add( "Processes" , processes_menu );
  

  /////////////// Process Macro Menu ///////////////
  bvis_process_macro_menu<bvis_video_manager> process_macro_menu;
  process_macro_menu.add_to_menu (MainMenu);

  /////////////// Tools Menu ///////////////
  vgui_menu tools;
   
  vgui_menu image_tools_menu;
  //MENU_ADD_TOOL(dbvis_image_intensity_inspector, image_tools_menu );

  vgui_menu vsol2D_tools_menu, vsol2D_draw_tools_menu;
    MENU_ADD_TOOL( bvis_vsol2D_line_tool,           vsol2D_draw_tools_menu, bvis_video_manager );
    MENU_ADD_TOOL( bvis_vsol2D_point_tool,          vsol2D_draw_tools_menu, bvis_video_manager );
    MENU_ADD_TOOL( bvis_vsol2D_polyline_tool,       vsol2D_draw_tools_menu, bvis_video_manager );
    MENU_ADD_TOOL( bvis_vsol2D_polygon_tool,        vsol2D_draw_tools_menu, bvis_video_manager );
    MENU_ADD_TOOL( bvis_vsol2D_digital_curve_tool,  vsol2D_draw_tools_menu, bvis_video_manager );
    vsol2D_tools_menu.add("Drawing", vsol2D_draw_tools_menu);
    MENU_ADD_TOOL( bvis_vsol2D_split_curve_tool,    vsol2D_tools_menu, bvis_video_manager );
    MENU_ADD_TOOL( bvis_correspond_point_tool,      vsol2D_tools_menu, bvis_video_manager );
    //MENU_ADD_TOOL( dbvis_edge_selection_tool,       vsol2D_tools_menu );
    //MENU_ADD_TOOL( dbvis_edge_transform_tool,       vsol2D_tools_menu );
    //MENU_ADD_TOOL( dbvis_classify_curve_tool,       vsol2D_tools_menu );
    //MENU_ADD_TOOL( bvis_vsol2D_crop_tool,      vsol2D_tools_menu );

  vgui_menu soview2D_tools_menu;
    MENU_ADD_TOOL( bvis_translate_tool, soview2D_tools_menu, bvis_video_manager );
    MENU_ADD_TOOL( bvis_style_tool,     soview2D_tools_menu, bvis_video_manager );

  vgui_menu bmrf_tools_menu;
    //////////////////////////////////////////////////////////MENU_ADD_TOOL( dbmrf_inspector_tool,  bmrf_tools_menu );
    //////////////////////////////////////////////////////////MENU_ADD_TOOL( dbmrf_inspect_3d_tool, bmrf_tools_menu );
    //////////////////////////////////////////////////////////MENU_ADD_TOOL( dbmrf_xform_tool,      bmrf_tools_menu );
  
#ifdef HAS_BGUI3D
    MENU_ADD_TOOL( dbmrf_curvel_3d_inspect_tool, bmrf_tools_menu, bvis_video_manager );
#endif

  vgui_menu epi_tools_menu;
    //////////////////////////////////////////////////////////MENU_ADD_TOOL( dbecl_episeg_inspector_tool,  epi_tools_menu );
    //////////////////////////////////////////////////////////MENU_ADD_TOOL( dbetl_epiprofile_tool,        epi_tools_menu );
    // tool to display epipolar lines:
    //////////////////////////////////////////////////////////MENU_ADD_TOOL( dvpgl_epipolar_tool,          epi_tools_menu);


  vgui_menu bvrl_tools_menu;
    //////////////////////////////////////////////////////////MENU_ADD_TOOL( dbvrl_region_transform_tool, bvrl_tools_menu );
    //////////////////////////////////////////////////////////MENU_ADD_TOOL( dbvrl_region_super_res_tool, bvrl_tools_menu );

  vgui_menu shock_tools_menu;
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbsk2d_ishock_detector_debug_tool, shock_tools_menu );
    //MENU_ADD_TOOL( dbvis_shock_bnd_line_tool, shock_tools_menu );
    //MENU_ADD_TOOL( dbvis_highlight_tool,      shock_tools_menu );

  vgui_menu livewire_tools_menu;
    //MENU_ADD_TOOL( dbvis_livewire_tool, livewire_tools_menu );

  vgui_menu labeling_tools_menu;
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbru_labeling_tool,              labeling_tools_menu );

  vgui_menu dbctrk_tools_menu;
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbctrk_get_info_tool,              dbctrk_tools_menu );
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbctrk_write_info_tool,            dbctrk_tools_menu );
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbctrk_inspector_tool,             dbctrk_tools_menu);
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbctrk_minspector_tool,            dbctrk_tools_menu);
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbctrk_foreground_background_tool, dbctrk_tools_menu );
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbctrk_transform_tool,             dbctrk_tools_menu);
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbctrk_clustering_process_tool,    dbctrk_tools_menu);
    //MENU_ADD_TOOL( dbctrk_ground_truth_tool,          dbctrk_tools_menu );
    //MENU_ADD_TOOL( dbctrk_alignment_viewer_tool,      dbctrk_tools_menu );
#ifdef HAS_MATLAB
    MENU_ADD_TOOL( dbctrk_intensity_tool,             dbctrk_tools_menu, bvis_video_manager );
    MENU_ADD_TOOL( dbctrk_intensity_prune_tool,       dbctrk_tools_menu, bvis_video_manager );
#endif

  vgui_menu dbetrk_tools_menu;
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbetrk_edge_inspector_tools, dbetrk_tools_menu );
    ////////////////////////////////////////////////////////////MENU_ADD_TOOL( dbetrk_particles_tool,       dbetrk_tools_menu );

  vgui_menu stereo_tools_menu;

  
  // vgui_menu shockshape_tools_menu;
  //  MENU_ADD_TOOL( dbvis_shockshape_tool, shockshape_tools_menu );
  //  MENU_ADD_TOOL( dbvis_triangularblob_tool, shockshape_tools_menu );
  tools.add("vsol2D",      vsol2D_tools_menu );
  tools.add("2D Geometry", soview2D_tools_menu);
  tools.add("MRF Network", bmrf_tools_menu);
  tools.add("Epipolar",    epi_tools_menu);
  tools.add("Shock",       shock_tools_menu);
  tools.add("Registration", bvrl_tools_menu);
  tools.add("Livewire", livewire_tools_menu);
  tools.add("Curve tracking", dbctrk_tools_menu);
  tools.add("Edge tracking", dbetrk_tools_menu);

  tools.add("Stereo", stereo_tools_menu);
  // tools.add("Shape Modeling", shockshape_tools_menu);
  tools.add("Labeling", labeling_tools_menu);

  MainMenu.add( "Tools", tools );

  /////////////// Tableau Menu ///////////////
  bvis_tableau_menu<bvis_video_manager> tableau_menu;
  tableau_menu.add_to_menu( MainMenu );

  /////////////// Batch Menu ///////////////
  //dbvis_batch_menu batch_menu;
  //batch_menu.add_to_menu( MainMenu );

  /////////////// View Menu ////////////////
  bvis_view_menu<bvis_video_manager> view_menu;
  view_menu.add_to_menu(MainMenu);

  return MainMenu;
}
