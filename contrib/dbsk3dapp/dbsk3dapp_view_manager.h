// dbsk3dapp_view_manager.h
// Name: Ming-Ching Chang
//       Alexander Bowman akb

#ifndef dbsk3dapp_view_manager_h_
#define dbsk3dapp_view_manager_h_

#include <vgui/vgui_grid_tableau.h> 
#include <bgui3d/bgui3d_examiner_tableau.h>

class SoSelection;

class dbsk3dapp_view_manager
{
protected:
  //: the master root of the scenegraph
  SoSeparator*                              master_root_;

  //: the array of tableaus used
  vcl_vector<bgui3d_examiner_tableau_sptr>  tableaus_;

  //: the grid tableau, should be greater than the tableaus_.size()
  vgui_grid_tableau_sptr                    grid_tableau_;

  //: each view's scenegraph drawing root 
  vcl_vector<SoSelection*>                  view_drawing_SoSelections_;

  SoCamera* _camera;

public:
  dbsk3dapp_view_manager();
  virtual ~dbsk3dapp_view_manager();
  
  void set_active_tableau_text (const vcl_string& string);

  SoSeparator* master_root() { return master_root_; }

  //: Assume the sonode has only one parent scenegraph group. Return it.
  SoGroup* get_parent_group (SoNode* sonode);

  void setup_scene (vgui_grid_tableau_sptr grid);

  bgui3d_examiner_tableau_new _add_one_view_tableau ();

  int get_grid_size() { 
    return grid_tableau_->rows()*grid_tableau_->cols(); 
  }

  void add_to_view (unsigned int i, SoNode* node); 
  void clear_view (unsigned int i);
  void clear_all_views ();

  //: Note that these two functions need to be use together
  //  You can choose 2x2 = 4 views but only set 3 views from them.
  void choose_nxn_grid (unsigned int num);
  void add_tableaus_to_grid ();

  void resize_grid (unsigned int rows, unsigned int cols);

  void view_selected_tableaus ();
  void unselect_all_tableaus ();

   //: ======== view control functions ======== 
  void view_all ();
  void view_subgraph (SoNode* node);  
};


#endif
