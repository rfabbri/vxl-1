// This is brcv/rec/dbru/vis/dbru_rcor_tableau.cxx

//:
// \file

#include <float.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_distance.h>

#include "dbru_rcor_tableau.h"
#include <dbru/dbru_rcor.h>

class dbru_rcor_tableau_toggle_command : public vgui_command
{
 public:
  dbru_rcor_tableau_toggle_command(dbru_rcor_tableau* tab, const void* boolref) : 
       rcor_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    rcor_tableau->post_redraw(); 
  }

  dbru_rcor_tableau *rcor_tableau;
  bool* bref;
};

class dbru_rcor_tableau_set_display_params_command : public vgui_command
{
 public:
  dbru_rcor_tableau_set_display_params_command(dbru_rcor_tableau* tab, 
    const vcl_string& name, const void* intref) : rcor_tableau(tab), iref_((int*)intref), name_(name) {}

  void execute() 
  { 
    int param_val = *iref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *iref_ = param_val;
    rcor_tableau->post_redraw(); 
  }

  dbru_rcor_tableau *rcor_tableau;
  int* iref_;
  vcl_string name_;
};

//: Constructor
dbru_rcor_tableau::dbru_rcor_tableau(): rcor_(0)
{
  display_grid_map_ = true;
  display_grid1_ = true;
  display_grid2_ = false;
  display_continuous_grid_ = false;
  display_colored_grid_ = false;

  grid_int_ = 3; //default grid interval

  //high contrast colors for the grid
  CM[0][0] = 229; CM[0][1] = 0;   CM[0][2] = 27;
  CM[1][0] = 27;  CM[1][1] = 229; CM[1][2] = 0;
  CM[2][0] = 38;  CM[2][1] = 0;   CM[2][2] = 229;
  CM[3][0] = 229; CM[3][1] = 134; CM[3][2] = 0;
  CM[4][0] = 228; CM[4][1] = 0;   CM[4][2] = 229;
  CM[5][0] = 229; CM[5][1] = 229; CM[5][2] = 0;
  CM[6][0] = 0;   CM[6][1] = 229; CM[6][2] = 209;
}

bool dbru_rcor_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW ) 
  {
    if (display_grid_map_)
      draw_grid_mapping();
    return true;
  }
  return false;
}

dbru_rcor_tableau::~dbru_rcor_tableau()
{
}

void dbru_rcor_tableau::draw_grid_mapping()
{
  if( rcor_.ptr() == 0 ) 
    return;

  //get the necessary info from the region correspondence computation
  int min_x_ = rcor_->get_min_x();
  int min_y_ = rcor_->get_min_y();
  int rcor_width_ = rcor_->get_upper1_x();
  int rcor_height_ = rcor_->get_upper1_y();
  vbl_array_2d< vgl_point_2d<int> > & rcor_map_ = rcor_->get_map();
  vbl_array_2d< vgl_point_2d<float> > & rcor_map_float_ = rcor_->get_map_float();

  if (display_grid1_) {
    //go over the first region and draw the grid
    for (int xx=grid_int_; xx<rcor_width_; xx+=grid_int_){
      for (int yy=grid_int_; yy<rcor_height_; yy+=grid_int_)
      {
        //left line
        if (rcor_map_[xx-grid_int_][yy].x()!=-1 && rcor_map_[xx][yy].x()!=-1){
          if (display_colored_grid_)
            glColor3f(CM[yy%7][0]/255.0, CM[yy%7][1]/255.0, CM[yy%7][2]/255.0);
          else
            glColor3f(0.3f, 0.3f, 0.3f);
          glLineWidth (1.0);
          glBegin( GL_LINE_STRIP );
          glVertex2f(xx-grid_int_+min_x_, yy+min_y_);
          glVertex2f(xx+min_x_, yy+min_y_);
          glEnd();
        }
        //top line
        if (rcor_map_[xx][yy-grid_int_].x()!=-1 && rcor_map_[xx][yy].x()!=-1){
          if (display_colored_grid_)
            glColor3f(CM[xx%7][0]/255.0, CM[xx%7][1]/255.0, CM[xx%7][2]/255.0);
          else
            glColor3f(0.3f, 0.3f, 0.3f);
          glLineWidth (1.0);
          glBegin( GL_LINE_STRIP );
          glVertex2f(xx+min_x_, yy-grid_int_+min_y_);
          glVertex2f(xx+min_x_, yy+min_y_);
          glEnd();
        }
      }
    }
  }

  if (display_grid2_){
    //go over the corresponding points in the second region and draw the grid
    for (int xx=grid_int_; xx<rcor_width_; xx+=grid_int_){
      for (int yy=grid_int_; yy<rcor_height_; yy+=grid_int_)
      {
        float curp_x, curp_y;
        if (display_continuous_grid_) {
          curp_x = (rcor_map_float_[xx][yy]).x();
          curp_y = (rcor_map_float_[xx][yy]).y();
        } else {
          curp_x = float((rcor_map_[xx][yy]).x());
          curp_y = float((rcor_map_[xx][yy]).y());
        }
        //left line

        if (rcor_map_[xx-grid_int_][yy].x()!=-1 && rcor_map_[xx][yy].x()!=-1){
          if (display_colored_grid_)
            glColor3f(CM[yy%7][0]/255.0, CM[yy%7][1]/255.0, CM[yy%7][2]/255.0);
          else
            glColor3f(0.3f, 0.3f, 0.3f);
          
          float p_x, p_y;
          if (display_continuous_grid_) {
            p_x = (rcor_map_float_[xx-grid_int_][yy]).x();
            p_y = (rcor_map_float_[xx-grid_int_][yy]).y();
          } else {
            p_x = float((rcor_map_[xx-grid_int_][yy]).x());
            p_y = float((rcor_map_[xx-grid_int_][yy]).y());
          }

          glLineWidth (1.0);
          glBegin( GL_LINE_STRIP );
          glVertex2f(p_x, p_y);
          glVertex2f(curp_x, curp_y);
          glEnd();
        }
        //top line

        if (rcor_map_[xx][yy-grid_int_].x()!=-1 && rcor_map_[xx][yy].x()!=-1){
          if (display_colored_grid_)
            glColor3f(CM[xx%7][0]/255.0, CM[xx%7][1]/255.0, CM[xx%7][2]/255.0);
          else
            glColor3f(0.3f, 0.3f, 0.3f);
          float p_x, p_y;
          if (display_continuous_grid_) {
            p_x = (rcor_map_float_[xx][yy-grid_int_]).x();
            p_y = (rcor_map_float_[xx][yy-grid_int_]).y();
          } else {
            p_x = float((rcor_map_[xx][yy-grid_int_]).x());
            p_y = float((rcor_map_[xx][yy-grid_int_]).y());
          }

          glLineWidth (1.0);
          glBegin( GL_LINE_STRIP );
          glVertex2f(p_x, p_y);
          glVertex2f(curp_x, curp_y);
          glEnd();
        }
      }
    }
  }
}

void 
dbru_rcor_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( ((display_grid_map_)?on:off)+"Draw Grid Mapping", 
               new dbru_rcor_tableau_toggle_command(this, &display_grid_map_));
  submenu.add( ((display_grid1_)?on:off)+"Draw Grid1", 
               new dbru_rcor_tableau_toggle_command(this, &display_grid1_));
  submenu.add( ((display_grid2_)?on:off)+"Draw Grid2", 
               new dbru_rcor_tableau_toggle_command(this, &display_grid2_));
  submenu.add( ((display_continuous_grid_)?on:off)+"Draw Continuous Grid2", 
               new dbru_rcor_tableau_toggle_command(this, &display_continuous_grid_));
  submenu.add( ((display_colored_grid_)?on:off)+"Draw Colored Grid", 
               new dbru_rcor_tableau_toggle_command(this, &display_colored_grid_));
  
  submenu.add( "Set Grid Interval (pixels)", 
               new dbru_rcor_tableau_set_display_params_command(this, "Grid Interval ", &grid_int_));

  //add this submenu to the popup menu
  menu.add("Region-Corr Tableau Options", submenu);
}
