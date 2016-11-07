/**************************************************************/
/*  Name: Alexander Bowman akb
//  File: dbsk3dapp_menu.h
//  Asgn:
//  Date: Feb 28, 2005  Modified by Ming-Ching Chang
***************************************************************/

#ifndef Shock3d_Menu_h_
#define Shock3d_Menu_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

#include "dbsk3dapp_manager.h" 

class dbsk3dapp_menu 
{
public:
  dbsk3dapp_menu () {
  }
  virtual ~dbsk3dapp_menu() {
  }
  vgui_menu add_to_menu (vgui_menu& menu);

  //: FILE MENU
  static void file_new()   { dbsk3dapp_manager::instance()->file_new(); }
  static void file_open()  { dbsk3dapp_manager::instance()->file_open(); }
  static void file_save()  { dbsk3dapp_manager::instance()->file_save(); }
  static void file_save_view_as_iv()  { dbsk3dapp_manager::instance()->file_save_view_as_iv(); }
  static void fileQuit()  { dbsk3dapp_manager::instance()->fileQuit(); }
  
  //: EDIT MENU
  static void edit_clear_all_views() { dbsk3dapp_manager::instance()->edit_clear_all_views(); }
  static void edit_add_pointgene()  { }
  static void edit_del_pointgene()  { }
  static void edit_add_noise()      { }
  // Ming: all other functions in preprocess p3d

  //: INTERACTIVITY
  static void clickSelection()      { dbsk3dapp_manager::instance()->clickSelection(); }
  static void clickDeletion()       { dbsk3dapp_manager::instance()->clickDeletion(); }
  static void clickInterrogation()  { dbsk3dapp_manager::instance()->clickInterrogation(); }
  static void clickAddition()       { dbsk3dapp_manager::instance()->clickAddition(); }
  static void clickAlteration()     { dbsk3dapp_manager::instance()->clickAlteration(); }

  static void search_fs_elm()       { dbsk3dapp_manager::instance()->search_fs_elm(); }

  //: VIEW MENU
  static void view_show_hide()              { dbsk3dapp_manager::instance()->view_show_hide(); }
  static void view_selected()               { dbsk3dapp_manager::instance()->view_selected(); }
  static void view_one()                    { dbsk3dapp_manager::instance()->view_set_num_views(1); }
  static void view_two()                    { dbsk3dapp_manager::instance()->view_set_num_views(2); }
  static void view_four()                   { dbsk3dapp_manager::instance()->view_set_num_views(4); }
  static void view_eight()                  { dbsk3dapp_manager::instance()->view_set_num_views(8); }
  static void view_sixteen()                { dbsk3dapp_manager::instance()->view_set_num_views(16); }

  static void view_object_boundary_options()  { dbsk3dapp_manager::instance()->view_object_boundary_options(); }
  static void view_shock_element_options()    { dbsk3dapp_manager::instance()->view_shock_element_options(); }
  static void view_scaffold_hypergraph_options()  { dbsk3dapp_manager::instance()->view_scaffold_hypergraph_options(); }

  static void view_background_color()       { dbsk3dapp_manager::instance()->view_background_color(); }


  //: PROCESSES
  static void add_bndsphere()                   { dbsk3dapp_manager::instance()->add_bndsphere(); }
  static void compute_voronoi_via_qhull()       { dbsk3dapp_manager::instance()->compute_voronoi_via_qhull(); }
  static void shock_recovery()                  { dbsk3dapp_manager::instance()->shock_recovery(); }
  static void surface_meshing()                 { dbsk3dapp_manager::instance()->surface_meshing(); }
  static void rmin_shock_prune() { dbsk3dapp_manager::instance()->rmin_shock_prune(); }
  static void build_ms_hypg()       { dbsk3dapp_manager::instance()->build_ms_hypg(); }
  static void build_sg_sa_from_cms()            { dbsk3dapp_manager::instance()->build_sg_sa_from_cms(); }
  
  static void search_A13_A13() { dbsk3dapp_manager::instance()->search_A13_A13(); }
  static void search_A13_A3() { dbsk3dapp_manager::instance()->search_A13_A3(); }
  static void search_A1A3_A13() { dbsk3dapp_manager::instance()->search_A1A3_A13(); }
  static void search_A1A3_A1A3() { dbsk3dapp_manager::instance()->search_A1A3_A1A3(); }

  static void remove_cms_transitions()          { dbsk3dapp_manager::instance()->remove_cms_transitions(); }
  static void build_sg_sa_from_cms_after_trans()  { dbsk3dapp_manager::instance()->build_sg_sa_from_cms_after_trans(); }
  static void remove_sg_transitions()           { dbsk3dapp_manager::instance()->remove_sg_transitions(); }
  static void smooth_scaffold_graph()           { dbsk3dapp_manager::instance()->smooth_scaffold_graph(); }
  static void show_flow()                       { dbsk3dapp_manager::instance()->show_flow(); }
  static void reconstruct()                     { dbsk3dapp_manager::instance()->reconstruct(); }

  //: TASK MENU
  static void compute_fullshock()            { dbsk3dapp_manager::instance()->compute_fullshock(); }
  static void scaffold_hypergraph_from_fs()  { dbsk3dapp_manager::instance()->scaffold_hypergraph_from_fs(); }
  static void compute_scaffold_hypergraph()  { dbsk3dapp_manager::instance()->compute_scaffold_hypergraph(); }
  static void scaffold_hypergraph_plus_transitions()  { dbsk3dapp_manager::instance()->scaffold_hypergraph_plus_transitions(); }
  static void run_file_scaffold_hypergraph_plus_transitions()  { dbsk3dapp_manager::instance()->run_file_scaffold_hypergraph_plus_transitions(); } 
  
  //: MATCHING
  static void match_shock_matching()    { dbsk3dapp_manager::instance()->match_shock_matching(); }
  static void match_registration()      { dbsk3dapp_manager::instance()->match_registration(); }
  static void match_icp()               { dbsk3dapp_manager::instance()->match_icp(); }
  static void matching_groundtruth()    { dbsk3dapp_manager::instance()->matching_groundtruth(); }

  //: OPTION
  static void option_do_validations() { dbsk3dapp_manager::instance()->option_do_validations(); }
  static void draw_object_boundary_options() { dbsk3dapp_manager::instance()->draw_object_boundary_options(); }
  static void draw_shock_element_options() { dbsk3dapp_manager::instance()->draw_shock_element_options(); }
  static void draw_scaffold_hypergraph_options() { dbsk3dapp_manager::instance()->draw_scaffold_hypergraph_options(); }

  //: HELP MENU
  static void helpInteraction()     { dbsk3dapp_manager::instance()->helpInteraction(); }
  static void help_vxl_object_size()     { dbsk3dapp_manager::instance()->help_vxl_object_size(); }  
  static void help_object_size()     { dbsk3dapp_manager::instance()->help_object_size(); }  
  static void help_test_object_size()     { dbsk3dapp_manager::instance()->help_test_object_size(); }  
  static void helpAbout()           { dbsk3dapp_manager::instance()->helpAbout(); }

  static void search( int type, unsigned int idNumber );

};

#endif
