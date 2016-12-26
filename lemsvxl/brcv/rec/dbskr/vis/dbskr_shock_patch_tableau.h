// This is brcv/rec/dbskr/vis/dbskr_shock_patch_tableau.h
#ifndef dbskr_shock_patch_tableau_h_
#define dbskr_shock_patch_tableau_h_

//:
// \file
// \brief A tableau to display dbskr_shock_patch_storage objects.
// \author Ozge Can Ozcanli
// \date March 17 2007
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>

#include <dbskr/dbskr_shock_patch_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>

#include <dbskr/vis/dbskr_shock_patch_tableau_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>

#include <vil/vil_image_view.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>

//: A tableau to display dbskr_shock_patch_storage objects
class dbskr_shock_patch_tableau : public vgui_tableau 
{
private:
  GLenum gl_mode;
protected:
  
  vcl_vector<dbskr_shock_patch_sptr> patches_;
  vcl_map<int, dbskr_shock_patch_sptr> id_sptr_map_;

public:
  
  dbskr_shock_patch_tableau();
  virtual ~dbskr_shock_patch_tableau() {}
  
  virtual bool handle( const vgui_event & );
  
  //: set the patch vector
  void set_patches(vcl_vector<dbskr_shock_patch_sptr>& patches) { patches_ = patches; } 

  //: get the patch vector
  vcl_vector<dbskr_shock_patch_sptr>& get_patches(void) { return patches_; }

  //: set the patch map
  void set_map(vcl_map<int, dbskr_shock_patch_sptr>& map) { id_sptr_map_ = map; } 

  //: get the patch map
  vcl_map<int, dbskr_shock_patch_sptr>& get_map(void) { return id_sptr_map_; }

  void draw_render();
  void draw_patch(dbskr_shock_patch_sptr shock_patch);
  
  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

protected:
  dbskr_shock_patch_sptr current_patch_;

  bool display_outer_boundary_;   ///< display the outer polygon
  bool display_all_;
  bool display_traced_boudary_;
  bool display_real_boundaries_;
  double shock_pruning_threshold_;

  vgl_point_2d<double> mouse_pt_;
  float ix_, iy_;

  vgui_event_condition gesture_select_point_;
  vgui_event_condition next_patch_;
  vgui_event_condition create_storage_;

  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;
  bool color_image_;

  float rnd_color[100][3]; 
};

//: Create a smart-pointer to a dbsk2d_shock_tableau.
struct dbskr_shock_patch_tableau_new : public dbskr_shock_patch_tableau_sptr
{
  typedef dbskr_shock_patch_tableau_sptr base;

  //: Constructor - creates a pointer to a dbskr_shock_match_tableau
  dbskr_shock_patch_tableau_new() : base(new dbskr_shock_patch_tableau()) { }
};

class dbskr_sp_tableau_toggle_command : public vgui_command
{
 public:
  dbskr_sp_tableau_toggle_command(dbskr_shock_patch_tableau* tab, const void* boolref) : 
       match_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    match_tableau->post_redraw(); 
  }

  dbskr_shock_patch_tableau *match_tableau;
  bool* bref;
};

class dbskr_sp_tableau_set_display_params_command : public vgui_command
{
 public:
  dbskr_sp_tableau_set_display_params_command(dbskr_shock_patch_tableau* tab, 
    const vcl_string& name, const void* intref) : match_tableau(tab), iref_((int*)intref), name_(name) {}

  void execute() 
  { 
    int param_val = *iref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *iref_ = param_val;
    match_tableau->post_redraw(); 
  }

  dbskr_shock_patch_tableau *match_tableau;
  int* iref_;
  vcl_string name_;
};

#endif //dbskr_shock_patch_tableau_h_
