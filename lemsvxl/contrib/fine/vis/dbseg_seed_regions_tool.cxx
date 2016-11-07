#include "dbseg_seed_regions_tool.h"

dbseg_seed_regions_tool::dbseg_seed_regions_tool() :  
  tab_(0),
  left_click(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  middle_click(vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true)),
  home_key(vgui_event_condition(vgui_HOME, vgui_MODIFIER_NULL, true)),
  shift_right_click(vgui_event_condition(vgui_RIGHT, vgui_SHIFT, true)),
  active(false),
  seeds(0),
  point_list(0),
  last_x(0.0), last_y(0.0)
{
}

vcl_string dbseg_seed_regions_tool::name() const
{
  return "Seed Regions";
}

//: Set the tableau associated with the current view
bool dbseg_seed_regions_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  vgui_image_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;

  bpro1_storage_sptr storage = bvis1_manager::instance()->storage_from_tableau(tableau);
  if (!storage.ptr())
    return false;

  //make sure its a seg storage class
  /*if (storage->type() == "seg"){
    dbseg_seg_storage_sptr seg_storage;
    seg_storage.vertical_cast(storage);
    seg_object_ = seg_storage->get_object();*/
    //if switching between frames then reset variables
    if (tab_ != temp_tab) {
        point_list.clear();
        /*if (seg_tab_) {
            seg_tab_->set_discontinuous_coords();
        }*/
    }
    tab_ = temp_tab;
    return true;
  //}
  return false;
}

bool dbseg_seed_regions_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& view ) {
    double ix, iy;
    float ix2, iy2;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix2, iy2);
    ix = (double) ix2;
    iy = (double) iy2;

    //save the seeds as an image
    if (home_key(e) && !active) {
        vgui_dialog topmatches_dlg("Save Region Seeds");
        string name;
        topmatches_dlg.field("Name",name);
        if(!topmatches_dlg.ask()) {
            return true;
        }



        int ni = tab_->get_image_view()->ni();
        int nj = tab_->get_image_view()->nj();
        vil_image_view<vxl_byte> seed_view = vil_image_view<vxl_byte>(ni,nj);
        for (int i = 0; i < ni; i++) {
            for (int j = 0; j < nj; j++) {
                seed_view(i,j) = 0;
            }
        }
        for (int k = 0; k < seeds.size(); k++) {
            vgl_polygon<double> tempPoly = vgl_polygon<double>(1);
            list<pair<double, double>>::iterator i;
            for (i = seeds[k].begin(); i != seeds[k].end(); ++i) {
                tempPoly.push_back(i->first, i->second);
            }
            vgl_polygon_scan_iterator<double> psi(tempPoly);
            for (psi.reset(); psi.next();) {
                int y = psi.scany();
                for (int x = psi.startx(); x <= psi.endx(); ++x) {
                    if (x >= 0 && x <= ni && y >= 0 && y < nj) {
                        seed_view(x,y) = k+1;
                    }
                }
            }
        }
        vil_image_resource_sptr resource = vil_new_image_resource_of_view(seed_view);
        vidpro1_image_storage_sptr img_stor = vidpro1_image_storage_new();
        img_stor->set_image(resource);
        img_stor->set_name(name);

        bvis1_manager::instance()->repository()->store_data(img_stor);

        return true;
    }
    
    //kill the current polygon
    if (shift_right_click(e)) {
        point_list.clear();
        active = false;
        tab_->post_overlay_redraw();
        return true;
    }

    //set up coordinates for the moving part of the polygon
    if( e.type == vgui_MOTION ) {
        last_x = ix;
        last_y = iy;
        //if( active ) {
            tab_->post_overlay_redraw();
        //}
    }

    //start the polygon or add a point to it
    if (left_click(e)) {
        if (active) {
            point_list.push_back(vgl_point_2d<double>(ix, iy));
            return true;
        }
        else {
            active = true;
            //seg_tab_->clear_all_outlines();
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
        //select_regions();
        //vector<vgl_point_2d<double>>::iterator i;
        list<pair<double, double>> tempList;
        for (int i = 0; i < point_list.size(); i++) {
            tempList.push_back(pair<double, double>(point_list[i].x(), point_list[i].y()));
        }
        seeds.push_back(tempList);
        tab_->post_redraw();
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
            //return true;
        }
        
        glLineWidth(1);
        glColor3f(1,0,0);
        
        for (int i = 0; i < seeds.size(); i++) {
            glBegin(GL_LINE_LOOP);
            list<pair<double, double>>::iterator j;
            for (j = seeds[i].begin(); j != seeds[i].end(); ++j) {
                glVertex2f(j->first, j->second);
            }
            glEnd();
        }

        return true;


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


void dbseg_seed_regions_tool::save_seeds() {

}


void dbseg_seed_regions_tool::create_seed() {

}



void dbseg_seed_regions_tool::deactivate() {
    point_list.clear();
    seeds.clear();
    tab_->post_overlay_redraw();
}

void dbseg_seed_regions_tool::activate() {
    
}

