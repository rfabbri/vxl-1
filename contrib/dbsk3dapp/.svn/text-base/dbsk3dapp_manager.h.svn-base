/**************************************************************/
/*  Name: Alexander Bowman
//  File: dbsk3dapp_manager.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef _dbsk3dapp_manager_h_
#define _dbsk3dapp_manager_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

#include <dbsk3d/pro/dbsk3d_process.h>

//note the order here
#include "dbsk3dapp_view_manager.h"
#include "dbsk3dapp_action.h"

#include <dbsk3d/vis/dbsk3d_process_vis.h>
#include <dbsk3dr/vis/dbsk3dr_process_vis.h>

class dbsk3dapp_manager 
{
private:
  static dbsk3dapp_manager* instance_;
  
  //: The dual boundary-shock set that contains processes on them
  dbsk3d_pro_vis*        pv1_;

  //: The dual pair for shape matching
  dbsk3dr_pro_vis*  spvr_;

  dbsk3dapp_view_manager*     view_manager_;
  dbsk3dapp_action*           action_;

  vcl_string                  file_opened_cache_;
  vcl_string                  file_saved_cache_;

public:  
  dbsk3d_pro_vis* dual_bnd_shock_vis() { 
    return pv1_;
  }
  dbsk3dapp_view_manager* view_manager() { 
    return view_manager_; 
  }

  dbsk3dapp_action* action() { 
    return action_; 
  }

  void clear_all_reset () {
    if (pv1_)
      delete pv1_;
    pv1_ = new dbsk3d_pro_vis ();

  }

  //==================== Menu Handling Functions ====================
  // FILE MENU
  void file_new();                           
  void file_open();                          
  void file_save();
  void file_save_view_as_iv();
  void fileQuit();                          

  // EDIT MENU
  void edit();                              
  void edit_clear_all_views();    

  // CLICK MENU  
  void clickInterrogation()  { 
    action()->set_action( dbsk3dapp_action::ACT_INTERROGATION ); 
  }
  void clickSelection()      { 
    action()->set_action( dbsk3dapp_action::ACT_SELECTION ); 
  }
  void clickDeletion()       { 
    action()->set_action( dbsk3dapp_action::ACT_DELETION ); 
  }
  void clickAlteration()     { 
    action()->set_action( dbsk3dapp_action::ACT_ALTERATION ); 
  }
  void clickAddition()       { 
    action()->set_action( dbsk3dapp_action::ACT_ADDITION ); 
  }

  void search_fs_elm ();

  // VIEW MENU
  void viewChangeCubeSize();

  void choose_view (vgui_dialog &params, int &pick);

  void view_set_num_views (unsigned int num);
  void view_show_hide();

  void view_selected() { view_manager()->view_selected_tableaus(); }

  void view_object_boundary_options ();
  void view_shock_element_options ();
  void view_scaffold_hypergraph_options();
  void view_background_color();

  void search_A13_A13 ();
  void search_A13_A3 ();
  void search_A1A3_A13 ();
  void search_A1A3_A1A3 ();

  // PROCESS MENU
  void add_bndsphere();
  void compute_voronoi_via_qhull();
  void shock_recovery();

  void surface_meshing();  
  void rmin_shock_prune();
  void build_ms_hypg ();
  void build_sg_sa_from_cms();

  void remove_cms_transitions();
  void build_sg_sa_from_cms_after_trans();

  void remove_sg_transitions();

  void smooth_scaffold_graph();

  void show_flow();
  void reconstruct();

  // TASK MENU
  void compute_fullshock();
  void scaffold_hypergraph_from_fs();
  void compute_scaffold_hypergraph(); 
  void scaffold_hypergraph_plus_transitions();
  void run_file_scaffold_hypergraph_plus_transitions();
    
  // MATCHING MENU
  void match_shock_matching();                     
  void match_registration();
  void match_icp();        
  void matching_groundtruth();             
    
  // OPTION MENU
  void option_do_validations ();
  void draw_object_boundary_options ();
  void draw_shock_element_options ();
  void draw_scaffold_hypergraph_options ();

  // HELP MENU
  void helpInteraction();
  void help_vxl_object_size();
  void help_object_size();
  void help_test_object_size();
  void helpAbout();

  dbsk3dapp_manager();
  virtual ~dbsk3dapp_manager();
  static dbsk3dapp_manager* instance();
};

#endif
