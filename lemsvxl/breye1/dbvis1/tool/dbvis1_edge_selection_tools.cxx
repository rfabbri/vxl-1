#include <dbvis1/tool/dbvis1_edge_selection_tools.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_style.h>

dbvis1_edge_selection_tool::dbvis1_edge_selection_tool()
{
    gesture_prune_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
}

dbvis1_edge_selection_tool::~dbvis1_edge_selection_tool()
{
}

bool
dbvis1_edge_selection_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  bgui_vsol2D_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;
    
  if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau))){
    tableau_ = temp_tab;
    return true;
  }
  return false;
}

bool
dbvis1_edge_selection_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class
  if (storage->type() == "vsol2D"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

bgui_vsol2D_tableau_sptr
dbvis1_edge_selection_tool::tableau()
{
  return tableau_;
}

vidpro1_vsol2D_storage_sptr
dbvis1_edge_selection_tool::storage()
{
  vidpro1_vsol2D_storage_sptr vsol_storage;
  vsol_storage.vertical_cast(storage_);
  return vsol_storage;
}

bool
dbvis1_edge_selection_tool::handle( const vgui_event & e, 
                                   const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if ( gesture_prune_(e) ) {

    vcl_vector<vgui_soview*> all_objects;
    all_objects = tableau_->get_all();
    
    int deleted = 0;
    for (unsigned int i = 0; i<all_objects.size(); i++) {
      if (!tableau_->is_selected(all_objects[i]->get_id())) {
        tableau_->remove(all_objects[i]);
        deleted++;
      }
    }

    tableau_->deselect_all();
    vcl_cout << deleted << " objects are deleted out of " << all_objects.size()-1 << " objects from storage!\n";
    
    //: update storage class
    all_objects.clear();
    all_objects = tableau_->get_all();
    storage_->clear_all();
    for (unsigned int i = 0; i<all_objects.size(); i++) {
      if (((bgui_vsol_soview2D_polyline*)all_objects[i])->type_name_() == "bgui_vsol_soview2D_polyline") {
        storage_->add_object(((bgui_vsol_soview2D_polyline*)all_objects[i])->sptr()->cast_to_spatial_object() , "remainings" );

      } else 
          vcl_cout << "types do not match, storage class is not updated!!\n";
    }

    //storage_ = bvis1_manager::instance()->storage_from_tableau(tableau_);
    //this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau_));

    tableau_->post_redraw();
  }

  return false;

}

//: Return the name of this tool
vcl_string 
dbvis1_edge_selection_tool::name() const
{
  return "Edge Selection";
}


