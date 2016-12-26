// This is brcv/rec/dbru/vis/dbru_osl_tools.cxx
//:
// \file

#include "dber_instance_tools.h"
#include <vcl_sstream.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h> 
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_mapper.h>
#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbru/vis/dbru_osl_displayer.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>


#include <dber/pro/dber_instance_storage.h>
#include <dbru/dbru_label.h>

#include <dbru/dbru_multiple_instance_object.h>

//get storage from the storage pointer
static dber_instance_storage_sptr get_instance_storage(bpro1_storage_sptr const& sto)
{
  dber_instance_storage_sptr ins_storage;
  ins_storage.vertical_cast(sto);
  return ins_storage;
}

static bpro1_storage_sptr storage()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  bpro1_storage_sptr data = res->get_data_at("dber_instance_storage", -1);
  return data;
}



//============================== Label Video Instances Tool ============================

bool dber_instance_label_tool::label_all_selected(void)
{
  if (!tableau_) {
    vcl_cout << " get_current_database() - tableau_ is not set in dber_instance_label_tool\n";
    return false;
  }

  vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames = ins_storage_->get_frames();
  vcl_vector<int> *col_pos = new vcl_vector<int>();
  vcl_vector<int> *row_pos = new vcl_vector<int>();
  vcl_vector<int> *times = new vcl_vector<int>();
  tableau_->get_selected_positions(col_pos, row_pos, times);

  for (unsigned i = 0; i<col_pos->size(); i++) {
    if ((*row_pos)[i] < 0 || (*row_pos)[i] < 0)
      continue;
    
    vcl_pair<unsigned, unsigned> p;
    int col = (*col_pos)[i];
    int row = (*row_pos)[i];
    if (col < int(frames.size()))
      if (row < int(frames[col].size())) {
        dbru_multiple_instance_object_sptr o = frames[col][row];
        o->set_label(current_label_.ptr());
      }
  }

  for (unsigned i = 0; i<col_pos->size(); i++) {
    if ((*row_pos)[i] < 0 || (*row_pos)[i] < 0)
      continue;
    //deselect the list after labeling
    tableau_->set_selected((*row_pos)[i], (*col_pos)[i], false);
  }

  col_pos->clear();
  row_pos->clear();
  times->clear();
  delete col_pos;
  delete row_pos;
  delete times;
  return true;
}

//Constructor
dber_instance_label_tool::dber_instance_label_tool() :
  active_(false), tableau_(NULL)
{
  gesture_label_ = vgui_event_condition(vgui_key('l'), vgui_MODIFIER_NULL, true);
  gesture_display_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  prepare_label_ = vgui_event_condition(vgui_key('p'), vgui_MODIFIER_NULL, true);
  current_label_ = new dbru_label();
}


//: Destructor
dber_instance_label_tool::~dber_instance_label_tool()
{
}


//: Return the name of this tool
vcl_string dber_instance_label_tool::name() const
{
  return "Label Selected Video Instances";
}


//: Set the tableau to work with
bool
dber_instance_label_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}

//: Handle events
bool
dber_instance_label_tool::handle( const vgui_event & e,
                                  const bvis1_view_tableau_sptr& view )
{
  if (gesture_display_(e))
    if (tableau_) {
      vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames = ins_storage_->get_frames();
      unsigned col_pos, row_pos;
      tableau_->get_active_position(&col_pos, &row_pos);
      if (col_pos < frames.size())
        if (row_pos < frames[col_pos].size()) {
          dbru_label_sptr lbl = frames[col_pos][row_pos]->get_label();
          if (lbl)
            vcl_cout << *lbl;
          else 
            vcl_cout << "no label has been set for this instance\n";
        }
    }

  if (gesture_label_(e))
      label_all_selected();

  if (prepare_label_(e)) {
    static int motion_bin = -1;
    static int view_bin = -1;
    static int shadow_bin = -1;
    static int shadow_length = -1;
    vcl_string category;
    vgui_dialog param_dlg("Prepare Label");
    vcl_vector<vcl_string> cats;
    cats.push_back("null");
    cats.push_back("car");
    cats.push_back("pickup");
    cats.push_back("minivan");
    cats.push_back("suv");
    cats.push_back("utility");
    int choice = 0;
    param_dlg.choice("Category", cats, choice);
    param_dlg.field("motion bin", motion_bin);
    param_dlg.field("view bin", view_bin);
    param_dlg.field("shadow bin", shadow_bin);
    param_dlg.field("shadow length", shadow_length);
    
    if(!param_dlg.ask())
      return false;

    current_label_ = new dbru_label(cats[choice], motion_bin, view_bin, shadow_bin, shadow_length);
  }

  return false;
}

void dber_instance_label_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  ins_storage_ = get_instance_storage(junk);
  if(!ins_storage_) {
    vcl_cout << "Failed to activate dber_instance_tools\n";
    return;
  }

  vcl_cout << "label_instances tool active\n";
  vcl_cout << "USAGE: First create a label, then selected instances will be labeled\n";

  if (!tableau_) {
    vcl_cout << " dber_instance_label_tool::activate() - tableau_ is not set in dber_instance_label_tool\n";
    return;
  }

  bvis1_manager::instance()->display_current_frame(true);
  return;
}







