#include "dbseg_seg_label_region_tool.h"

//#include <vcl_limits.h>
//#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
//#include <vgui/vgui_style.h>
//#include <vgui/vgui_dialog.h>

#include <bpro1/bpro1_storage_sptr.h>
#include <bvis1/bvis1_manager.h>

#include <vgl/vgl_polygon.h>


dbseg_seg_label_region_tool::dbseg_seg_label_region_tool() :  
  seg_tab_(0),
  seg_object_(0),
  left_click(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  //shift_left_click(vgui_event_condition(vgui_LEFT, vgui_SHIFT, true)),
  shift_right_click(vgui_event_condition(vgui_RIGHT, vgui_SHIFT, true)),
  home_key(vgui_event_condition(vgui_key('m'), vgui_MODIFIER_NULL, true)),
  down_arrow(vgui_event_condition(vgui_END, vgui_MODIFIER_NULL, true)),
  up_arrow(vgui_event_condition(vgui_HOME, vgui_MODIFIER_NULL, true)),
  //middle_click(vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true)),
  //enter_key(vgui_event_condition(vgui_HOME, vgui_MODIFIER_NULL, true)),
  //end_button(vgui_event_condition(vgui_END, vgui_MODIFIER_NULL, true)),
  active(false),
  dis(false),
  current_label(0)
  //point_list(0),
  //init(true),
  //last_x(0.0), last_y(0.0)
{
}

vcl_string dbseg_seg_label_region_tool::name() const
{
  return "Label Region";
}

//: Set the tableau associated with the current view
bool dbseg_seg_label_region_tool::set_tableau( const vgui_tableau_sptr& tableau )
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
        current_label = 0;
    }
    seg_tab_ = temp_tab;
    
    return true;
  }
  return false;
}

bool dbseg_seg_label_region_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{

  double ix, iy;
  float ix2, iy2;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix2, iy2);
  ix = (double) ix2;
  iy = (double) iy2;



  if (shift_right_click(e)) { // display what is currently labeled
        vcl_cout << "******* Labeled Regions of the Spine *******" << vcl_endl;
        vcl_cout << get_label_name(0) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->C1) << vcl_endl;
        vcl_cout << get_label_name(1) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->D1) << vcl_endl;
        vcl_cout << get_label_name(4) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->C2) << vcl_endl;
        vcl_cout << get_label_name(5) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->D2) << vcl_endl;
        vcl_cout << get_label_name(8) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->C3) << vcl_endl;
        vcl_cout << get_label_name(9) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->D3) << vcl_endl;
        vcl_cout << get_label_name(12) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->C4) << vcl_endl;
        vcl_cout << get_label_name(13) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->D4) << vcl_endl;
        vcl_cout << get_label_name(16) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->C5) << vcl_endl;
        vcl_cout << get_label_name(17) << " is region " << seg_tab_->get_object()->get_spine_part(seg_tab_->get_object()->SPINE) << vcl_endl;
        //for (int i = 1; i < 5; i++) {
            double x1, y1, x2, y2;
            seg_tab_->get_object()->get_disk_corners(1, x1, y1, x2, y2);
            seg_tab_->set_discontinuous_coords(x1, y1, x2, y2);    
            seg_tab_->post_overlay_redraw();
        //}
        return true;
  }
      
      /*if (shift_right_click(e)) {
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


    }*/

  if (left_click(e) ) {
      seg_tab_->set_discontinuous_coords();    
      seg_tab_->post_overlay_redraw();
    if (current_label == 2 ||  current_label == 3 || current_label == 6 || current_label == 7 || current_label == 10 || current_label == 11 || current_label == 14 || current_label == 15) {
        switch (current_label) {
            case 2:
                seg_tab_->get_object()->set_disk_corner(1, false, ix, iy);
                break;
            case 3:
                seg_tab_->get_object()->set_disk_corner(1, true, ix, iy);
                break;
            case 6:
                seg_tab_->get_object()->set_disk_corner(2, false, ix, iy);
                break;
            case 7:
                seg_tab_->get_object()->set_disk_corner(2, true, ix, iy);
                break;
            case 10:
                seg_tab_->get_object()->set_disk_corner(3, false, ix, iy);
                break;
            case 11:
                seg_tab_->get_object()->set_disk_corner(3, true, ix, iy);
                break;
            case 15:
                seg_tab_->get_object()->set_disk_corner(4, false, ix, iy);
                break;
            case 16:
                seg_tab_->get_object()->set_disk_corner(4, true, ix, iy);
                break;
        }
        vcl_cout << "point set" << vcl_endl;
        return true;
    }
  }

  if( home_key(e) ) {
      if (current_label == 2 ||  current_label == 3 || current_label == 6 || current_label == 7 || current_label == 10 || current_label == 11 || current_label == 14 || current_label == 15) {
          vcl_cout << "currently labeling a point, not a region" << vcl_endl;
      }
      else {
          int a = seg_tab_->get_outlined_regions().size();
          list<int> tempList = seg_tab_->get_outlined_regions();
              
          if (tempList.size() > 1) {
              vcl_cout << "Error Labeling Region - more than one region selected" << vcl_endl;
              return true;
          }
          if (tempList.size() < 1) {
              vcl_cout << "Error Labeling Region - no region selected" << vcl_endl;
              return true;
          }

          int tempID = tempList.front();//seg_object_->get_pixel_ID(ix, iy, seg_tab_->get_depth());
        switch (current_label) {
            case 0: //C1
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->C1, tempID);
                break;
            case 1: //D1
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->D1, tempID);
                break;
            case 4: //C2:
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->C2, tempID);
                break;
            case 5: //D2:
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->D2, tempID);
                break;
            case 8: //C3:
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->C3, tempID);
                break;
            case 9: //D3:
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->D3, tempID);
                break;
            case 12: //C4:
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->C4, tempID);
                break;
            case 13: //D4:
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->D4, tempID);
                break;
            case 16: //C5:
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->C5, tempID);
                break;
            case 17: //SPINE:
                seg_tab_->get_object()->set_spine_part(seg_tab_->get_object()->SPINE, tempID);
                break;
        }        
        vcl_cout << "Region " << tempID << " set as " << get_label_name(current_label) << "." << vcl_endl;
        seg_tab_->clear_all_outlines();
      }
    /*point_list.clear();
    point_list.push_back(vgl_point_2d<double>(ix, iy));*/
    
    return true;
  } 

  if ( up_arrow(e) ) {
    current_label--;
    if (current_label < 0) {
        current_label = 17;
    }
    vcl_cout << "Ready to label the " << get_label_name(current_label) << " region." << vcl_endl;
    if (seg_tab_->get_display_background()) {
        seg_tab_->toggle_display_background();
    }
    return true;
  }
  if ( down_arrow(e) ) {
    current_label++;
    if (current_label > 17) {
        current_label = 0;
    }
    vcl_cout << "Ready to label the " << get_label_name(current_label) << " region." << vcl_endl;
    
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

void dbseg_seg_label_region_tool::start_editing() {
    
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

void dbseg_seg_label_region_tool::stop_editing() {
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



void dbseg_seg_label_region_tool::deactivate() {
    cancel();
}

void dbseg_seg_label_region_tool::activate() {
    if (seg_tab_) {
        seg_tab_->clear_all_outlines();
        if (seg_tab_->get_display_background()) {
            seg_tab_->toggle_display_background();
        }
    }

}

void dbseg_seg_label_region_tool::cancel() {
    /*seg_tab_->set_editing(0);
    active = false;
    dis = false;
    seg_tab_->clear_all_outlines();
    seg_tab_->post_redraw();*/
}

vcl_string dbseg_seg_label_region_tool::get_label_name(int i) {
    switch (i) {
        case 0: //C1
            return "C1";
            break;
        case 1: //D1
            return "D1";
            break;
        case 2: 
            return "D1 upper point";
            break;
        case 3: 
            return "D1 lower point";
            break;
        case 4: //C2:
            return "C2";
            break;
        case 5: //D2:
            return "D2";
            break;
        case 6: 
            return "D2 upper point";
            break;
        case 7: 
            return "D2 lower point";
            break;
        case 8: //C3:
            return "C3";
            break;
        case 9: //D3:
            return "D3";
            break;
        case 10: 
            return "D3 upper point";
            break;
        case 11: 
            return "D3 lower point";
            break;
        case 12: //C4:
            return "C4";
            break;
        case 13: //D4:
            return "D4";
            break;
        case 14: 
            return "D4 upper point";
            break;
        case 15: 
            return "D4 lower point";
            break;
        case 16: //C5:
            return "C5";
            break;
        case 17: //SPINE:
            return "Spine";
            break;
    }        
    return "";
}

