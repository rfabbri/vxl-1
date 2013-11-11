#include "dbseg_seg_drawing_tableau.h"
//:
// \file

dbseg_seg_drawing_tableau::dbseg_seg_drawing_tableau(dbseg_seg_tableau* tab) : 
tab_(tab), 
outline_width_(3), 
inner_contour_width_(1), 
highlight_width_(1), 
highlighted(0)
{}

dbseg_seg_drawing_tableau::~dbseg_seg_drawing_tableau() {

}

bool dbseg_seg_drawing_tableau::handle(const vgui_event &e) {
    dbseg_seg_object<vxl_byte>* object_ = tab_->get_object();
    int depth = tab_->get_depth();
    int editing = tab_->get_editing();
    double ix, iy;
    float ix2, iy2;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix2, iy2);
    ix = (double) ix2;
    iy = (double) iy2;

    if (e.type == vgui_MOTION) {
        //if the mouse is outside of the image then nothing should be highlighted
        if (ix >= (object_->get_labeled_image()).ni() || ix < 0 || iy >= (object_->get_labeled_image()).nj() || iy < 0) {
            highlighted = 0;
            return vgui_easy2D_tableau::handle(e);
        }
        //otherwise highlight the region that the mouse is over
        int tempID = object_->get_pixel_ID(ix, iy, depth);
        highlighted = tempID;
        post_overlay_redraw();
    }

    if (e.type == vgui_OVERLAY_DRAW) {
        // draw the highlighted region
        if (highlighted != 0) {
            glColor3f(0,1,0);
            glLineWidth(highlight_width_);
            glBegin( GL_LINE_LOOP );
            vgl_polygon<double> tempOutline = *(object_->get_outline(highlighted).front());
                            
            for (unsigned int p = 0; p < tempOutline[0].size(); p++) {
                glVertex2d(tempOutline[0][p].x(),
                           tempOutline[0][p].y());    
            }
            
            //close it with the last point
            glVertex2d(tempOutline[0][0].x(),
                           tempOutline[0][0].y());
            glEnd();
        }
        
        //draw the line to the discontinous region
        double dx1, dx2, dy1, dy2;
        tab_->get_discontinuous_coords(dx1, dy1, dx2, dy2);
        if (!(dx1==0 && dy1==0 && dx2 == 0 && dy2 == 0) ) {
            glLineWidth(2);
            glColor3f(1,0,0);
            glBegin(GL_LINE_STRIP);
            glVertex2d(dx1, dy1);
            glVertex2d(dx2, dy2);
            glEnd();
            //seg_tab_->post_overlay_redraw();            


        }

        //draw the disk protrusions
        if (tab_->get_display_disk_pro()) {
            for (int j = 1; j < 5; j++) {
                vgl_polygon<double> poly = object_->get_disk_pro_poly(j);
                if (poly[0].size() != 0) {
                    glLineWidth(2);
                    glColor3f(1,0,0);
                    glBegin(GL_LINE_STRIP);
                    
                    for (int i = 0; i < poly[0].size(); i++) {
                        glVertex2d(poly[0][i].x(), poly[0][i].y());
                    }
                    glVertex2d(poly[0][0].x(), poly[0][0].y());
                    glEnd();
                    glColor3f(0,1,0);
                    glBegin(GL_LINE_STRIP);
                    double x1, x2, y1, y2;
                    object_->get_disk_corners(j, x1, y1, x2, y2);
                    glVertex2d(x1, y1);
                    glVertex2d(x2, y2);
                    //poly = object_->get_disk_pro_poly(2);
                    object_->get_disk_pro(j, x1, y1);
                    glVertex2d(x1, y1);
                    glEnd();
                }
            }
        }

        //create list of outlines to draw
        list<list<vgl_polygon<double>*>> l = object_->get_outlines(depth, tab_->get_outlined_regions());
        list<vgl_polygon<double>*>::iterator i;
        list<list<vgl_polygon<double>*>>::iterator j;
        // if an object is being edited then draw its children's outlines if they are on
        if (editing != 0) { 
            list<int> childrenBeingEdited = object_->get_children_list()[editing];
            list<int>::iterator itr;
            for (itr = childrenBeingEdited.begin(); itr != childrenBeingEdited.end(); ++itr) {
                if (object_->get_object_list()[*itr]->is_outlined()) {
                    l.push_back(object_->get_outline(*itr));
                }
            }
        }
        
        //draw the outlines
        glColor3f( 1.0, 0, 0);
        for (j = l.begin(); j != l.end(); ++j) {
            for (i = (*j).begin(); i != (*j).end(); ++i) {
                if (i == (*j).begin()) {
                    glLineWidth (outline_width_);
                }
                else {
                    glLineWidth (inner_contour_width_);
                }
                glBegin( GL_LINE_LOOP );
                for (unsigned int p = 0; p < (**i)[0].size(); p++) {
                    glVertex2d((*(*i))[0][p].x(),
                               (*(*i))[0][p].y());                            
                }
                
                //close it with the last point
                glVertex2d((*(*i))[0][0].x(),
                           (*(*i))[0][0].y());
                glEnd();
            }
        }
    }
    return vgui_easy2D_tableau::handle(e);
}


