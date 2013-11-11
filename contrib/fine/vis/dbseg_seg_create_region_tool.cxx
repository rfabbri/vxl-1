#include "dbseg_seg_create_region_tool.h"

dbseg_seg_create_region_tool::dbseg_seg_create_region_tool() :  
  seg_tab_(0),
  seg_object_(0),
  left_click(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  middle_click(vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true)),
  home_key(vgui_event_condition(vgui_HOME, vgui_MODIFIER_NULL, true)),
  shift_right_click(vgui_event_condition(vgui_RIGHT, vgui_SHIFT, true)),
  active(false),
  dis(false),
  point_list(0),
  last_x(0.0), last_y(0.0)
{
}

vcl_string dbseg_seg_create_region_tool::name() const
{
  return "Create Region";
}

//: Set the tableau associated with the current view
bool dbseg_seg_create_region_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  dbseg_seg_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;

  bpro1_storage_sptr storage = bvis1_manager::instance()->storage_from_tableau(tableau);
  if (!storage.ptr())
    return false;

  //make sure its a seg storage class
  if (storage->type() == "seg"){
    dbseg_seg_storage_sptr seg_storage;
    seg_storage.vertical_cast(storage);
    seg_object_= static_cast<dbseg_seg_object<vxl_byte>* >( seg_storage->get_object() );
    //if switching between frames then reset variables
    if (seg_tab_ != temp_tab) {
        point_list.clear();
        if (seg_tab_) {
            seg_tab_->set_discontinuous_coords();
        }
    }
    seg_tab_ = temp_tab;
    return true;
  }
  return false;
}

bool dbseg_seg_create_region_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& view ) {
    double ix, iy;
    float ix2, iy2;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix2, iy2);
    ix = (double) ix2;
    iy = (double) iy2;

    //create a higher region from the selected regions    
    if (home_key(e)) {
        //clear discontinuous region coordinates
        seg_tab_->set_discontinuous_coords();
        
        list<int>::iterator i;
        list<int> tempList = seg_tab_->get_outlined_regions();//seg_tab_->get_object()->get_valid_IDs();
        list<int> childList;
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            if (seg_tab_->get_object()->get_object_list()[*i]->get_depth() != 1) {
                vcl_cerr << "Error creating region - not all selected regions at depth level 1" << vcl_endl;
                return true;
            }
            childList.push_back(*i);
        }
        if (childList.size() > 1) {
            if (!seg_tab_->get_object()->create_region(childList)) {
                return true;
            }
        }
        else {
            vcl_cerr << "Error creating region - fewer than 2 regions selected" << vcl_endl;
            return true;
        }
        dis = false;
        seg_tab_->clear_all_outlines();
        seg_tab_->display_image(seg_tab_->get_depth());    
        return true;
    }
    
    //set up the coordinates to draw a line from the mouse to the center of the bounding box of the discontinuous region
    if (shift_right_click(e)) {
        int discontinuous = seg_tab_->get_discontinuous();
        if (discontinuous != 0) {
            disx1 = ix;
            disy1 = iy;
            disx2 = (seg_tab_->get_object()->get_object_list()[discontinuous]->get_left()+seg_tab_->get_object()->get_object_list()[discontinuous]->get_right())/2;
            disy2 = (seg_tab_->get_object()->get_object_list()[discontinuous]->get_top()+seg_tab_->get_object()->get_object_list()[discontinuous]->get_bottom())/2;
            seg_tab_->set_discontinuous_coords(disx1, disy1, disx2, disy2);
            //dis = !dis;
            seg_tab_->post_overlay_redraw();
            return true;
        }
    }

    //set up coordinates for the moving part of the polygon
    if( e.type == vgui_MOTION ) {
        last_x = ix;
        last_y = iy;
        if( active ) {
            seg_tab_->post_overlay_redraw();
        }
    }

    //start the polygon or add a point to it
    if (left_click(e)) {
        if (active) {
            point_list.push_back(vgl_point_2d<double>(ix, iy));
            return true;
        }
        else {
            active = true;
            seg_tab_->clear_all_outlines();
            point_list.clear();
            point_list.push_back(vgl_point_2d<double>(ix, iy));
            return true;
        }
    }

    //close the polygon
    if (middle_click(e) && active) {
        point_list.push_back(vgl_point_2d<double>(ix, iy));

        if (point_list.size() < 3) {
            vcl_cerr << "Error Creating Region - Polygon has fewer than 3 points" << vcl_endl;
            return true;
        }

        //outline the regions contained by this polygon
        select_regions();

        seg_tab_->post_redraw();
        active = false;
        point_list.clear();      
        return true;
    }

    if (e.type == vgui_OVERLAY_DRAW) {
        //draw the polygon
        if (active) {
            glLineWidth(2);
            glColor3f(1,0,0);
            glBegin(GL_LINE_LOOP);
            for (unsigned i=0; i<point_list.size(); ++i) {
                glVertex2f(point_list[i].x(), point_list[i].y() );
            }
            glVertex2f(last_x,last_y);
            glEnd();
            return true;
        }
        
        //draw the line pointing to the discontinuous region
        /*if (dis) {
            glLineWidth(2);
            glColor3f(1,0,0);
            glBegin(GL_LINE_STRIP);
            glVertex2d(disx1, disy1);
            glVertex2d(disx2, disy2);
            glEnd();
            seg_tab_->post_overlay_redraw();
            return true;
        }*/
        
    }
    return false;
}

void dbseg_seg_create_region_tool::select_regions()    {
    vgl_polygon<double> cur_poly(point_list);
    
    // go through all the outlines at the top depth level and see which ones this encloses
    list<int> depthList = seg_object_->get_depth_list(1);
    list<int>::iterator i;
    int count = 0;
    for (i = depthList.begin(); i != depthList.end(); ++i) {
        vgl_polygon<double> tempOutline = *(seg_object_->get_outline(*i).front());
        bool complete = true;
        for (unsigned int s = 0; s < tempOutline.num_sheets() && complete; ++s) {
            for (unsigned int p = 0; p < tempOutline[s].size() && complete; ++p) {
                if (!cur_poly.contains(tempOutline[s][p].x(), tempOutline[s][p].y())) {
                    complete = false;
                    break;
                }
            }
            if (!complete) {
                break;
            }
        }
        if (complete) {
            seg_object_->toggle_outlined(*i);
            count++;
        }
    }
    vcl_cout << count << " regions selected" << vcl_endl;
}

void dbseg_seg_create_region_tool::deactivate() {
    point_list.clear();
}

void dbseg_seg_create_region_tool::activate() {
    if (seg_tab_)
        seg_tab_->clear_all_outlines();
}

