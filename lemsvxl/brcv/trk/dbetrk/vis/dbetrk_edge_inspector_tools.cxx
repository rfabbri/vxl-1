#include <dbetrk/vis/dbetrk_edge_inspector_tools.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_view_tableau.h>
#include <vgui/vgui_style.h>
#include <dbetrk/dbetrk_edge.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>

dbetrk_edge_inspector_tools::dbetrk_edge_inspector_tools()
{
neighbor_style_ = vgui_style::new_style(1.0, 0.0, 0.0, 3.0, 3.0);
}

dbetrk_edge_inspector_tools::~dbetrk_edge_inspector_tools()
{
}

bool
dbetrk_edge_inspector_tools::set_tableau( const vgui_tableau_sptr& tableau )
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
dbetrk_edge_inspector_tools::set_storage ( const bpro1_storage_sptr& storage)
{
  if (!storage.ptr())
    return false;
  //make sure its a vsol storage class
  if (storage->type() == "edgetrk"){
    storage_.vertical_cast(storage);
    return true;
  }
  return false;
}

bgui_vsol2D_tableau_sptr
dbetrk_edge_inspector_tools::tableau()
{
  return tableau_;
}

dbetrk_storage_sptr
dbetrk_edge_inspector_tools::storage()
{
  dbetrk_storage_sptr edgetrk_storage;
  edgetrk_storage.vertical_cast(storage_);
  return edgetrk_storage;
}

bool
dbetrk_edge_inspector_tools::handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  
  if( e.type == vgui_MOTION){
  tableau_->motion(e.wx, e.wy);
    vgui_soview2D* curr_obj =  (vgui_soview2D*)tableau_->get_highlighted_soview();
 
      object_ = (dbetrk_soview2D*)curr_obj;
      curr_edge_node_ = NULL;
      if( curr_obj && curr_obj->type_name() == "dbetrk_soview2D"){   
        curr_edge_node_ = object_->edgetrk_sptr();
      }
      bvis1_manager::instance()->post_overlay_redraw();
    
  }

   if( e.type == vgui_DRAW_OVERLAY){
    if(!curr_edge_node_)
        return false;

   

   }
  return false;
}



//: Return the name of this tool
vcl_string 
dbetrk_edge_inspector_tools::name() const
{

  return "Edge Node Inspector";

}


