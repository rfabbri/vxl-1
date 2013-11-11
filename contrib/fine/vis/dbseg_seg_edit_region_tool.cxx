#include "dbseg_seg_edit_region_tool.h"

//#include <vcl_limits.h>
//#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
//#include <vgui/vgui_style.h>
//#include <vgui/vgui_dialog.h>

#include <bpro1/bpro1_storage_sptr.h>
#include <bvis1/bvis1_manager.h>

#include <vgl/vgl_polygon.h>


dbseg_seg_edit_region_tool::dbseg_seg_edit_region_tool() :  
  seg_tab_(0),
  seg_object_(0),
  //left_click(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  //shift_left_click(vgui_event_condition(vgui_LEFT, vgui_SHIFT, true)),
  shift_right_click(vgui_event_condition(vgui_RIGHT, vgui_SHIFT, true)),
  home_key(vgui_event_condition(vgui_HOME, vgui_MODIFIER_NULL, true)),
  //middle_click(vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true)),
  //enter_key(vgui_event_condition(vgui_HOME, vgui_MODIFIER_NULL, true)),
  //end_button(vgui_event_condition(vgui_END, vgui_MODIFIER_NULL, true)),
  active(false),
  dis(false)
  //point_list(0),
  //init(true),
  //last_x(0.0), last_y(0.0)
{
}

vcl_string dbseg_seg_edit_region_tool::name() const
{
  return "Edit Region";
}

//: Set the tableau associated with the current view
bool dbseg_seg_edit_region_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  dbseg_seg_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;

  bpro1_storage_sptr storage = bvis1_manager::instance()->storage_from_tableau(tableau);
  if (!storage.ptr())
    return false;

  //make sure its an seg storage class
  if (storage->type() == "seg"){
    dbseg_seg_storage_sptr seg_storage;
    seg_storage.vertical_cast(storage);
    seg_object_= static_cast<dbseg_seg_object<vxl_byte>* >( seg_storage->get_object() );
    if (seg_tab_ != temp_tab) {
        active = false;
        dis = false;
    }
    seg_tab_ = temp_tab;
    
    return true;
  }
  return false;
}

bool dbseg_seg_edit_region_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{

  double ix, iy;
  float ix2, iy2;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix2, iy2);
  ix = (double) ix2;
  iy = (double) iy2;




      if (shift_right_click(e)) {
        //draw a line from the mouse to the center of the bounding box of the discontinuous region
        int discontinuous = seg_tab_->get_discontinuous();
        if (discontinuous != 0) {
            disx1 = ix;
            disy1 = iy;
            disx2 = (seg_tab_->get_object()->get_object_list()[discontinuous]->get_left()+seg_tab_->get_object()->get_object_list()[discontinuous]->get_right())/2;
            disy2 = (seg_tab_->get_object()->get_object_list()[discontinuous]->get_top()+seg_tab_->get_object()->get_object_list()[discontinuous]->get_bottom())/2;
            seg_tab_->set_discontinuous_coords(disx1, disy1, disx2, disy2);
            //dis = !dis;
            //if (dis) {
            seg_tab_->post_overlay_redraw();
            //}
            
            
            return true;

        }


    }

  if( !active && home_key(e) ) {
      int a = seg_tab_->get_outlined_regions().size();
      list<int> tempList = seg_tab_->get_outlined_regions();
          
      if (tempList.size() > 1) {
          vcl_cout << "Error Editing Region - more than one region selected" << vcl_endl;
          return true;
      }
      if (tempList.size() < 1) {
          vcl_cout << "Error Editing Region - no region selected" << vcl_endl;
          return true;
      }

      int tempID = tempList.front();//seg_object_->get_pixel_ID(ix, iy, seg_tab_->get_depth());
    if (seg_object_->get_child_count(tempID) == 0) {//if (seg_object_->get_max_depth() == seg_object_->get_object_list()[tempID]->get_depth()) {
        vcl_cout << "Error Editing Region - Cannot edit a base level region" << vcl_endl;
        return true;
    }
    else {
        active = true;
        seg_tab_->set_editing(tempID);
    }
    
    /*point_list.clear();
    point_list.push_back(vgl_point_2d<double>(ix, iy));*/
    
    start_editing();
    return true;
  } 
  if (active && home_key(e)) {
        //clear discontinuous region coordinates
        seg_tab_->set_discontinuous_coords();
      
      stop_editing();
     
      return true;
  }
 /* if (e.type == vgui_OVERLAY_DRAW) {
    if (dis) {
        glLineWidth(2);
        glColor3f(1,0,0);
        glBegin(GL_LINE_STRIP);
        glVertex2d(disx1, disy1);
        glVertex2d(disx2, disy2);

        glEnd();
        return true;
    }
  }*/
  
    
  return false;
}

void dbseg_seg_edit_region_tool::start_editing() {
    
    seg_tab_->clear_all_outlines();
    //outline the children of the region to be edited
    list<int> tempList = seg_object_->get_children_list()[seg_tab_->get_editing()];
    list<int>::iterator i;
    for (i = tempList.begin(); i != tempList.end(); ++i) {
        seg_object_->toggle_outlined(*i);
    }
    seg_tab_->post_redraw();
    //seg_tab_->display_image(seg_tab_->get_depth());
    
}

void dbseg_seg_edit_region_tool::stop_editing() {
    /*list<int> tempList = seg_object_->get_valid_IDs();
    list<int>::iterator i;
    list<int> childList;
    for (i = tempList.begin(); i != tempList.end(); ++i) {
        if (seg_object_->is_outlined(*i)) {
            childList.push_back(*i);
        }

    }
    */
    if (seg_object_->edit_children(seg_object_->get_outlined_regions(), seg_tab_->get_editing())) {
        cancel();
    }
    /*active = false;
      seg_tab_->set_editing(0);
       seg_tab_->clear_all_outlines();
       */
}



void dbseg_seg_edit_region_tool::deactivate() {
    cancel();
}

void dbseg_seg_edit_region_tool::activate() {
    if (seg_tab_)
        seg_tab_->clear_all_outlines();
}

void dbseg_seg_edit_region_tool::cancel() {
    seg_tab_->set_editing(0);
    active = false;
    dis = false;
    seg_tab_->clear_all_outlines();
    seg_tab_->post_redraw();
}


