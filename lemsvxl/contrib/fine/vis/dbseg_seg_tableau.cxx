#include "dbseg_seg_tableau.h"
//:
// \file
#include <bgui/bgui_vsol_soview2D.h>
#include <vgui/vgui.h>
#include <vgui/vgui_style.h>

/*#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vdgl/vdgl_digital_curve.h>
*/
#include <cassert>



/*class dbseg_seg_tableau_create_region_command : public vgui_command {
public:
    dbseg_seg_tableau_create_region_command(dbseg_seg_tableau* tab) : tab_(tab) {


    }
    void execute() {
        if (tab_->get_depth() != 1) {
            std::cerr << "Error creating region - not at depth level 1" << std::endl;
            return;
        }
        list<int>::iterator i;
        list<int> tempList = tab_->get_object()->get_depth_list(1);
        list<int> childList;
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            if (tab_->get_object()->is_outlined(*i)) {
                childList.push_back(*i);
            }
        }
        if (childList.size() > 1) {
            tab_->get_object()->create_region(childList);
        }
        else {
            std::cerr << "Error creating region - fewer than 2 regions selected" << std::endl;
        }

        //tab_->post_redraw();
        tab_->clear_all_outlines();
        tab_->display_image(tab_->get_depth());

    }

    dbseg_seg_tableau* tab_;

};

*/

class dbseg_seg_tableau_retrace_command : public vgui_command {
public:
    dbseg_seg_tableau_retrace_command(dbseg_seg_tableau* tab) : tab_(tab) {


    }
    void execute() {
        list<int> tempList = tab_->get_outlined_regions();
        list<int>::iterator i;
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            tab_->get_object()->trace(*i, tab_->get_object()->RETRACE);
        }
        tab_->post_redraw();
    }

    dbseg_seg_tableau* tab_;

};


class dbseg_seg_tableau_clear_outlines_command : public vgui_command {
public:
    dbseg_seg_tableau_clear_outlines_command(dbseg_seg_tableau* tab) : tab_(tab) {


    }
    void execute() {
        tab_->clear_all_outlines();

    }

    dbseg_seg_tableau* tab_;

};

class dbseg_seg_tableau_outline_all_command : public vgui_command {
public:
    dbseg_seg_tableau_outline_all_command(dbseg_seg_tableau* tab) : tab_(tab) {


    }
    void execute() {
        tab_->clear_all_outlines();
        list<int> regions = tab_->get_object()->get_depth_list(tab_->get_depth());
        list<int>::iterator i;
        for (i = regions.begin(); i != regions.end(); ++i) {
            tab_->get_object()->toggle_outlined(*i);
            //tab_->toggle(*i);
        }
        

    }

    dbseg_seg_tableau* tab_;

};




class dbseg_seg_tableau_toggle_display_background_command : public vgui_command {
public:
    dbseg_seg_tableau_toggle_display_background_command(dbseg_seg_tableau* tab) : tab_(tab) {


    }
    void execute() {
        tab_->toggle_display_background();

    }

    dbseg_seg_tableau* tab_;

};

class dbseg_seg_tableau_toggle_display_disk_pro_command : public vgui_command {
public:
    dbseg_seg_tableau_toggle_display_disk_pro_command(dbseg_seg_tableau* tab) : tab_(tab) {


    }
    void execute() {
        tab_->toggle_display_disk_pro();

    }

    dbseg_seg_tableau* tab_;

};


class dbseg_seg_tableau_change_depth_command : public vgui_command {
public:
    dbseg_seg_tableau_change_depth_command(dbseg_seg_tableau* tab, int dep) : tab_(tab), dep_(dep) {


    }
    void execute() {
        /*if (dep_ < 1) {
            std::cerr << "Cannot decrease depth - already viewing shallowest level" << std::endl;
        }
        else if (dep_ > tab_->get_object()->get_max_depth()) {
            std::cerr << "Cannot increase depth - already viewing deepest level" << std::endl;
        }
        else {
            tab_->set_depth(dep_);
            std::cout << "Depth set to level: "<<dep_<<std::endl;
            tab_->display_image(tab_->get_depth());
        }*/
        tab_->set_depth(dep_);
    }
    int dep_;
    dbseg_seg_tableau* tab_;

};


bool dbseg_seg_tableau::display_background = true;
bool dbseg_seg_tableau::display_disk_pro = false;

dbseg_seg_tableau::dbseg_seg_tableau(dbseg_seg_object<vxl_byte>* object) :
    object_(object), 
    line_width_(3),
    left_click_(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
    shift_left_click_(vgui_event_condition(vgui_LEFT, vgui_SHIFT, true)),
    page_up_(vgui_event_condition(vgui_key('q'), vgui_MODIFIER_NULL, true)),
    page_down_(vgui_event_condition(vgui_key('a'), vgui_MODIFIER_NULL, true)),
    highlightedSection(0),
    editing(0),
    img_tab_(0),
    drawing_tab_(0),
    discontinuous(0),
    /*display_background(true),
    display_disk_pro(false),*/
    dx1(0),
    dx2(0),
    dy1(0),
    dy2(0),
    depth(1)
    { this->init(); }

  /*
dbseg_seg_tableau::dbseg_seg_tableau(vgui_image_tableau_sptr const& it,
                                         const char* n) :
  vgui_easy2D_tableau(it, n) { this->init(); }

dbseg_seg_tableau::dbseg_seg_tableau(vgui_tableau_sptr const& t,
                                         const char* n) :
  vgui_easy2D_tableau(t, n) { this->init(); }
*/

dbseg_seg_tableau::~dbseg_seg_tableau()
{
}


bool dbseg_seg_tableau::handle(const vgui_event &e) {


    if (page_down_(e)) {
        set_depth(depth+1);
        return true;
    }
    if (page_up_(e)) {
        set_depth(depth-1);
        return true;
    }

    if (e.type == vgui_DRAW) {
        if (display_background) {
            display_image(depth);
        }
        drawing_tab_->post_redraw();
    }
    if (e.type == vgui_OVERLAY_DRAW) {
        


    }



    float pointx, pointy;
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);
    int intx = (int)std::floor(pointx), inty = (int)std::floor(pointy);
    //if it's not inside the image then don't handle events relating to the image
    if (intx >= (object_->get_labeled_image()).ni() || intx < 0 || inty >= (object_->get_labeled_image()).nj() || inty < 0) {
        return vgui_composite_tableau::handle(e);//changed return vgui_image_tableau::handle(e);//changed return vgui_easy2D_tableau::handle(e);
    }


    /*if (e.type == vgui_MOTION) {
        int tempHighlightedSection = object_->get_pixel_ID(intx, inty, depth);
        if (tempHighlightedSection != highlightedSection) {
            highlightedSection = tempHighlightedSection;
            post_redraw();
        }
    }*/
    if (left_click_(e)) {
        int id = object_->get_pixel_ID(intx, inty, depth);
        /*easy_tab_->set_foreground(1,1,1);
        easy_tab_->set_line_width(10);
        easy_tab_->add_circle(100, 100, 50);
        easy_tab_->post_redraw();
        post_redraw();*/
        std::cout << "Query on Region " << id << std::endl;
        std::cout << "    Area:     " << object_->get_area(id) << std::endl;
        std::cout << "    Parent:   " << object_->get_tree()->get_parent(id) << std::endl;
        std::cout << "    Children:";// << object_->get_children_list()[id] << std::endl;
        list<int> tempList = object_->get_children_list()[id];
        list<int>::iterator i;
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            std::cout << " " << *i;
        }
        std::cout << std::endl;
        //temp show save text
        /*
        std::cout << std::endl;
        
        fstream out("test.txt", fstream::out);
        object_->save_text(out);//get_object_list()[id]->save_text(out);
        out.close();

        fstream in("test.txt", fstream::in);
        int id2, depth2, bottom2, top2, left2, right2, r2, g2, b2, size2;
        in >> id2;
        in >> depth2;
        in >> bottom2;
        in >> top2;
        in >> left2;
        in >> right2;
        in >> r2;
        in >> g2;
        in >> b2;
        in >> size2;
        std::cout << "Loaded: " << id2 << " " << depth2 << " " << bottom2 << " " << top2 << " " << left2 << " " << right2 << " " << r2 << " " << g2 << " " << b2 << " size: " << size2 << std::endl;
        

        in.close();
        */


        ///**/std::cout << "Area of Region #" << id << " is " << object_->get_area(id) << std::endl;

        return true;
    }

    if (shift_left_click_(e)) {
        int tempID = object_->get_pixel_ID(intx, inty, depth);
        //if clicking on an object that's being edited, outline it's children, not itself
        if (tempID == editing) {
            object_->toggle_outlined(object_->get_pixel_ID(intx, inty, depth+1));
        }
        else {
            object_->toggle_outlined(tempID);
        }
        /*if (object_->is_outlined(tempID)) {
            outlinedRegions.push_back(tempID);
        }
        else {
            outlinedRegions.remove(tempID);
        }*/

        
        //say whether the outlined regions form a continuous area
        /*list<int>::iterator i = object_->get_depth_list(depth).begin();
        i = object_->get_depth_list(depth).end();
        list<int> tempOutlined;
        int count = 0;
        int s = object_->get_depth_list(depth).size();
        list<int> tempList = object_->get_depth_list(depth);
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            count++;
            if (object_->is_outlined(*i)) {
                tempOutlined.push_back(*i);
            }
        }*/

        /*if (tempOutlined.size() > 1) {
            std::cout << "Is contiuous region? " << object_->is_continuous(tempOutlined) << std::endl;
        }*/
        drawing_tab_->post_overlay_redraw();
        //post_redraw();
        return true;
    }



    return vgui_composite_tableau::handle(e);
}


/*void dbseg_seg_tableau::display_outlines(int dep) {
    list<vsol_polygon_2d_sptr> l = object_->get_voutlines(dep);
    list<vsol_polygon_2d_sptr>::iterator i;
    for (i = l.begin(); i != l.end(); ++i) {
        dbseg_seg_tableau::add_vsol_polygon_2d(*i);
    }

    //dbseg_seg_tableau::add_vsol_polygon_2d(l.back());
}*/

void dbseg_seg_tableau::display_image(int dep) {
    //changed img_tab_ = vgui_image_tableau_new(object_->get_display(dep, editing));
    //std::cout << display_background << std::endl;
        vil_image_view<vxl_byte> temp = object_->get_image();
        


    if (display_background) {
        img_tab_->set_image_view(object_->get_display(dep, editing));//changed set_child(img_tab_); 
        
    }
    else {
        img_tab_->set_image_view(temp);
    }
    img_tab_->post_redraw();

    post_redraw();
    /*else {
        vil_image_view<vxl_byte> blank;
        img_tab_->
        img_tab_->set_image_view(blank);
    }*/
    
    //add_image(0, 0, object_->get_display(dep, editing));
    //drawing_tab_->post_redraw();

}


void dbseg_seg_tableau::init()
{
    depth = 1;
    img_tab_ = vgui_image_tableau_new(object_->get_display(depth, editing));
    drawing_tab_ = dbseg_seg_drawing_tableau_new(this);
    
    
    this->add_child(img_tab_);
    this->add_child(drawing_tab_);

    vil_image_view<vxl_byte> temp = object_->get_image();
        
    if (display_background) {
        img_tab_->set_image_view(object_->get_display(depth, editing));//changed set_child(img_tab_); 
        
    }
    else {
        img_tab_->set_image_view(temp);
    }
    //display_outlines(1);
      

  //define default soview styles
  //these can be overridden by later set_*_syle commands prior to drawing.
  //
  point_style_                = vgui_style::new_style(0.0f, 1.0f, 0.0f, 3.0f, 1.0f);
  line_style_                 = vgui_style::new_style(0.8f, 0.2f, 0.9f, 1.0f, 3.0f);
  conic_style_                 = vgui_style::new_style(0.2f, 0.8f, 0.1f, 1.0f, 3.0f);
  //polyline_style_             = vgui_style::new_style(0.8f, 0.2f, 0.9f, 1.0f, 3.0f);
  polyline_style_             = vgui_style::new_style((float)255, (float)0, (float)0, (float) 3, (float) 5);
  digital_curve_style_        = vgui_style::new_style(0.8f, 0.0f, 0.8f, 1.0f, 3.0f);
  dotted_digital_curve_style_ = vgui_style::new_style(0.8f, 0.0f, 0.8f, 3.0f, 3.0f);
  edgel_curve_style_          = vgui_style::new_style(0.0f, 0.5f, 0.8f, 1.0f, 3.0f);
  dotted_edgel_curve_style_   = vgui_style::new_style(0.0f, 0.5f, 0.8f, 3.0f, 3.0f);
}



const int dbseg_seg_tableau::get_depth() {
    return depth;
}

void dbseg_seg_tableau::set_depth(int d) {
    if (editing != 0) {
        std::cerr << "Cannot change depth while editing a region" << std::endl;
        return;
    }
    
    if (d < 1) {
        std::cerr << "Cannot decrease depth - already viewing shallowest level" << std::endl;
    }
    else if (d > object_->get_max_depth()) {
        std::cerr << "Cannot increase depth - already viewing deepest level" << std::endl;
    }
    else {
        depth = d;
        std::cout << "Depth set to level: "<<d<<std::endl;
        display_image(depth);
    }



    //depth = d;
}

dbseg_seg_object<vxl_byte>* dbseg_seg_tableau::get_object() {
    return object_;
}

void dbseg_seg_tableau::get_popup(const vgui_popup_params &params, vgui_menu &menu) {
    vgui_menu submenu;
    std::string on = "[x] ", off = "[ ] ";
    submenu.add( "Clear all outlines", new dbseg_seg_tableau_clear_outlines_command(this));
    submenu.add( "Outline all regions", new dbseg_seg_tableau_outline_all_command(this));
    submenu.add( "Retrace outlined regions", new dbseg_seg_tableau_retrace_command(this));

    submenu.separator();

    submenu.add( "Increase Depth", new dbseg_seg_tableau_change_depth_command(this, depth+1));
    submenu.add( "Decrease Depth", new dbseg_seg_tableau_change_depth_command(this, depth-1));
    
    submenu.separator();

    submenu.add( ((display_background)?on:off)+"Display View", 
               new dbseg_seg_tableau_toggle_display_background_command(this));

    submenu.add( ((display_disk_pro)?on:off)+"Display Disk Protrusion", 
               new dbseg_seg_tableau_toggle_display_disk_pro_command(this));
    //submenu.add( "Create Region", new dbseg_seg_tableau_create_region_command(this));

    menu.add("dbseg_seg_tableau", submenu);

}

void dbseg_seg_tableau::clear_all_outlines(int d) {
    list<int>::iterator i;
    list<int> tempList;
    if (d == 0) {
        tempList = get_outlined_regions();
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            object_->toggle_outlined(*i);
        }
        //outlinedRegions.clear();
    }
    else {
        tempList = object_->get_depth_list(d);
    
        for (i = tempList.begin(); i != tempList.end(); ++i) {
            if (object_->get_object_list()[*i]->is_outlined()) {
                object_->get_object_list()[*i]->toggle_outlined();
                //outlinedRegions.remove(*i);
            }

        }    
    }
    post_redraw();
}

int dbseg_seg_tableau::get_editing() {
    return editing;
}

void dbseg_seg_tableau::set_editing(int e) {
    editing = e;
}

void dbseg_seg_tableau::toggle_display_background() {
    display_background = !display_background;
    /*if (!display_background) {
        remove_child(img_tab_);
    }
    else {
        add_child(img_tab_);
    }*/
    display_image(depth);
    post_redraw();
}

void dbseg_seg_tableau::toggle_display_disk_pro() {
    display_disk_pro = !display_disk_pro;
    /*if (!display_background) {
        remove_child(img_tab_);
    }
    else {
        add_child(img_tab_);
    }*/
    if (display_disk_pro) {






    

    }
    post_redraw();
}

bool dbseg_seg_tableau::get_display_disk_pro() {
    return display_disk_pro;
}


list<int> dbseg_seg_tableau::get_outlined_regions() {
    return object_->get_outlined_regions();
}



int dbseg_seg_tableau::get_discontinuous() {
    return object_->get_discontinuous();
}

void dbseg_seg_tableau::set_discontinuous_coords(double x1, double y1, double x2, double y2) {
    dx1 = x1;
    dy1 = y1;
    dx2 = x2;
    dy2 = y2;
}

void dbseg_seg_tableau::get_discontinuous_coords(double& x1, double& y1, double& x2, double& y2) {
    x1 = dx1;
    x2 = dx2;
    y1 = dy1;
    y2 = dy2;
}

bool dbseg_seg_tableau::get_display_background() {
    return display_background;
}



