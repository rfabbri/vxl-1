// Alexander K. Bowman
//

#include <vgui/vgui.h> 

#include "dbsk3dapp_menu.h"
///#include "dbsk3dapp_view_manager.h"

vgui_menu dbsk3dapp_menu::add_to_menu (vgui_menu& topbar_menu)
{
  // ################################################################
  vgui_menu file_menu;
  file_menu.add ("New...", file_new);
  file_menu.add ("Open...", file_open);
  file_menu.add ("Save...", file_save);
  file_menu.separator();
  file_menu.add ("Save View as IV...", file_save_view_as_iv);
  file_menu.separator();
  file_menu.add ("Exit...", fileQuit);
  topbar_menu.add ("File", file_menu);

  // ################################################################
  vgui_menu edit_menu;
  edit_menu.add ("Clear All Views", edit_clear_all_views);
  edit_menu.separator();
  edit_menu.add ("Add Point Generator...", edit_add_pointgene);
  edit_menu.add ("Delete Point Generator...", edit_del_pointgene);
  edit_menu.add ("Add Noise to Point Generators...", edit_add_noise);
  edit_menu.separator();
  edit_menu.add( "Select", clickSelection );
  edit_menu.add( "Delete", clickDeletion );
  edit_menu.add( "Interrogate", clickInterrogation );
  edit_menu.add( "Add", clickAddition );
  edit_menu.add( "Alteration", clickAlteration );
  edit_menu.separator();
  edit_menu.add( "Search fine-scale (FS) shock element", search_fs_elm );
  topbar_menu.add ("Edit", edit_menu);

  // ################################################################
  vgui_menu view_menu;

  vgui_menu grid_menu;
    grid_menu.add("One", view_one );
    grid_menu.add("Two", view_two );
    grid_menu.add("Four", view_four );
    grid_menu.add("Eight", view_eight );
    grid_menu.add("Sixteen", view_sixteen );
  view_menu.add("Grid Size", grid_menu );
  view_menu.add ("View Selected Tableaus", view_selected );
  view_menu.separator();
  view_menu.add ("Show/Hide Visualization...", view_show_hide);
  view_menu.separator();
  view_menu.add( "Object Boundary Mesh View Options...", view_object_boundary_options );
  view_menu.add( "Shock Element View Options...", view_shock_element_options );
  view_menu.add( "Shock Scaffold Hypergraph View Options...", view_scaffold_hypergraph_options );
  view_menu.separator();
  view_menu.add( "Choose Background Color...", view_background_color );
  topbar_menu.add ("View", view_menu);

  // ################################################################
  vgui_menu process_menu;
  process_menu.add ("Add Bounding Sphere to Generators...", add_bndsphere);
  process_menu.add ("Compute Voronoi Diagram via QHull...", compute_voronoi_via_qhull);
  process_menu.separator();
  process_menu.add ("Surface Meshing...", surface_meshing);
  process_menu.add ("Surface Smoothing/Shock Pruning...", rmin_shock_prune);
  process_menu.add ("Build Shock Scaffold Hypergraph (*.cms)...", build_ms_hypg);
  process_menu.add ("Build Scaffold Graph (*.cms -> *.sg)...", build_sg_sa_from_cms);
  process_menu.separator();

  vgui_menu detect_trans_menu;
  detect_trans_menu.add ("Search close-by A13 Curve -> A13 Curve (A14 Transition R->C)...", search_A13_A13);
  detect_trans_menu.add ("Search close-by A13 Curve -> A3 Curve (A1A3-II Transition L->C)...", search_A13_A3);
  detect_trans_menu.add ("Search close-by A1A3 Point -> A13 Curve (A12A3-II Transition L->C)...", search_A1A3_A13);
  detect_trans_menu.add ("Search close-by A1A3 Point -> A1A3 Point (A1A3-II Transition R->C)...", search_A1A3_A1A3);
  process_menu.add ("Detect Transitions", detect_trans_menu);

  process_menu.separator();
  process_menu.add ("Remove Scaffold Hypergraph Transitions (*.cms -> *-ht.cms)...", remove_cms_transitions);
  process_menu.add ("Build Scaffold Graph After Transitions (*-ht.cms -> *-ht.sg)...", build_sg_sa_from_cms_after_trans);
  process_menu.separator();
  process_menu.add ("Remove Scaffold Graph Transitions (*-ht.sg -> *-ht-gt.sg)...", remove_sg_transitions);
  process_menu.add ("Smooth Scaffold Graph (*-ht-gt.sg -> *-ht-gt-.sm?.sg)...", smooth_scaffold_graph); 
  process_menu.separator();  
  process_menu.add ("Show flow", show_flow );
  process_menu.add("Reconstruct", reconstruct );
  topbar_menu.add( "Process", process_menu );
  
  // ################################################################
  vgui_menu task_menu;
  //composite processes
  task_menu.add ("Shock Recovery (*.vor -> *.fs)...", shock_recovery);
  task_menu.add ("Compute FullShock...", compute_fullshock);
  task_menu.separator();
  task_menu.add ("FullShock to Scaffold Hypergraph (*.fs -> *.cms/sg)...", scaffold_hypergraph_from_fs);
  task_menu.add ("Compute Shock Scaffold Hypergraph (*.p3d -> *.cms/sg)...", compute_scaffold_hypergraph);  
  task_menu.separator();
  task_menu.add ("Compute Scaffold Hypergraph + Transitions (all in one)...", scaffold_hypergraph_plus_transitions);
  task_menu.separator();
  task_menu.add ("Execute RunFile: Scaffold Hypergraph + Transitions...", run_file_scaffold_hypergraph_plus_transitions);
  topbar_menu.add( "Task", task_menu );

  // ################################################################
  vgui_menu match_menu;
  match_menu.add ("Shock Matching...", match_shock_matching);
  match_menu.add ("Registration...", match_registration);
  match_menu.add ("Iterative Closest Point...", match_icp);
  match_menu.add ("Validation against Groundtruth...", matching_groundtruth);
  topbar_menu.add ("Match", match_menu );

  // ################################################################
  vgui_menu option_menu;

  option_menu.add ("Debug Do Validations...", option_do_validations);
  option_menu.separator();
  option_menu.add ("Object Boundary Mesh Drawing Options...", draw_object_boundary_options);
  option_menu.add ("Shock Element Drawing Options...", draw_shock_element_options);
  option_menu.add ("Shock Scaffold Hypergraph Drawing Options...", draw_scaffold_hypergraph_options);

  topbar_menu.add ("Option", option_menu);
  // ################################################################
  vgui_menu help_menu;
  help_menu.add ("Instructions on Interaction...", helpInteraction);
  help_menu.add ("VXL Object Memory Size...", help_vxl_object_size);  
  help_menu.add ("Object Memory Size...", help_object_size);
  help_menu.add ("Test Object Memory Size...", help_test_object_size);
  help_menu.add ("About...", helpAbout);
  topbar_menu.add ("Help", help_menu);
      
  return topbar_menu;
}

