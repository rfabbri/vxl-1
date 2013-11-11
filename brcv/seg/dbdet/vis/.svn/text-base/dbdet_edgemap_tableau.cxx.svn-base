// This is brcv/seg/dbdet/vis/dbdet_edgemap_tableau.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h>
#include <vcl_limits.h>

#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_distance.h>

#include <gl2ps/gl2ps.h>

#include "dbdet_edgemap_tableau.h"

class dbdet_edgemap_tableau_toggle_command : public vgui_command
{
 public:
  dbdet_edgemap_tableau_toggle_command(dbdet_edgemap_tableau* tab, const void* boolref) : 
       edge_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    edge_tableau->post_redraw(); 
  }

  dbdet_edgemap_tableau *edge_tableau;
  bool* bref;
};

class dbdet_edgemap_tableau_set_int_params_command : public vgui_command
{
 public:
  dbdet_edgemap_tableau_set_int_params_command(dbdet_edgemap_tableau* tab, 
    const vcl_string& name, const void* intref) : edge_tableau(tab),  iref_((int*)intref), name_(name) {}

  void execute() 
  { 
    int param_val = *iref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *iref_ = param_val;
    edge_tableau->post_redraw(); 
  }

  dbdet_edgemap_tableau *edge_tableau;
  int* iref_;
  vcl_string name_;
};

class dbdet_edgemap_tableau_set_display_params_command : public vgui_command
{
 public:
  dbdet_edgemap_tableau_set_display_params_command(dbdet_edgemap_tableau* tab, 
    const vcl_string& name, const void* floatref) : edge_tableau(tab),  floatref_((float*)floatref), name_(name) {}

  void execute() 
  { 
    float param_val = *floatref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *floatref_ = param_val;
    edge_tableau->post_redraw(); 
  }

  dbdet_edgemap_tableau *edge_tableau;
  float* floatref_;
  vcl_string name_;
};

class dbdet_edgemap_tableau_set_current_color_command : public vgui_command
{
 public:
  dbdet_edgemap_tableau_set_current_color_command(dbdet_edgemap_tableau* tab) : edge_tableau(tab) {}

  void execute() 
  { 
    vgui_dialog style_dlg("Change Style");
    char color[50];
    vcl_sprintf (color, "%.3f %.3f %.3f", edge_tableau->curr_color(0), 
                                          edge_tableau->curr_color(1), 
                                          edge_tableau->curr_color(2));
    vcl_string col(color);

    float point_size = edge_tableau->point_size();
    float line_width = edge_tableau->line_width();
    float line_length = edge_tableau->line_length();
    style_dlg.inline_color("Color", col);
    style_dlg.field("Point Size", point_size);
    style_dlg.field("Line Width", line_width);
    style_dlg.field("Line Length", line_length);

    if(!style_dlg.ask())
      return;

    vcl_istringstream color_strm(col);
    color_strm >> color_[0] >> color_[1] >> color_[2];

    edge_tableau->set_style(color_, point_size, line_width, line_length);
    edge_tableau->post_redraw(); 
  }

  dbdet_edgemap_tableau *edge_tableau;
  vnl_vector_fixed<float,3> color_;
};

class dbdet_edgemap_tableau_set_mixed_threshold_command : public vgui_command
{
 public:
  dbdet_edgemap_tableau_set_mixed_threshold_command(dbdet_edgemap_tableau* tab) : edge_tableau(tab) {}

  void execute() 
  { 
    vgui_dialog style_dlg("Set Mixed Threshold");
    
    float l1, l2, mix_thresh;
    edge_tableau->get_mixed_threshold(l1, l2, mix_thresh);

    style_dlg.field("lambda 1", l1);
    style_dlg.field("lambda 2", l2);
    style_dlg.field("Threshold", mix_thresh);

    if(!style_dlg.ask())
      return;

    edge_tableau->set_mixed_threshold(l1, l2, mix_thresh);
    edge_tableau->post_redraw(); 
  }

  dbdet_edgemap_tableau *edge_tableau;
};

class dbdet_edgemap_parameter_bars
{
public:
  dbdet_edgemap_parameter_bars(dbdet_edgemap_tableau* tab) : edge_tab(tab) {}

  void handle(const vgui_event & e)
  {
    //draw axes
    int vp[4]; // get viewport
    glGetIntegerv(GL_VIEWPORT,(GLint*)vp);

    float xmin, xmax, ymin1, ymax1, ymin2, ymax2;
    vgui_projection_inspector().window_to_image_coordinates(vp[0]+10, vp[1]+10, xmin, ymax1);
    vgui_projection_inspector().window_to_image_coordinates(vp[2]-10, vp[1]+30, xmax, ymin1);
    vgui_projection_inspector().window_to_image_coordinates(vp[0]+10, vp[1]+35, xmin, ymax2);
    vgui_projection_inspector().window_to_image_coordinates(vp[2]-10, vp[1]+55, xmax, ymin2);

    //draw the bars
    glLineWidth(1);
    glColor3f(0.0f,0.0f,0.0f);

    //bar 1
    glBegin(GL_LINES); glVertex2f(xmin, ymin1); glVertex2f(xmin, ymax1); glEnd();
    glBegin(GL_LINES); glVertex2f(xmax, ymin1); glVertex2f(xmax, ymax1); glEnd();
    glBegin(GL_LINES); glVertex2f(xmin, (ymin1+ymax1)/2.0); glVertex2f(xmax, (ymin1+ymax1)/2.0); glEnd();
    //bar1 thresh: str(0-30) 
    float th1 = edge_tab->get_threshold()*(xmax-xmin)/30.0 + xmin;
    glLineWidth(2);
    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_LINES); glVertex2f(th1, ymin1); glVertex2f(th1, ymax1); glEnd();

    //bar 2
    glLineWidth(1);
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINES); glVertex2f(xmin, ymin2); glVertex2f(xmin, ymax2); glEnd();
    glBegin(GL_LINES); glVertex2f(xmax, ymin2); glVertex2f(xmax, ymax2); glEnd();
    glBegin(GL_LINES); glVertex2f(xmin, (ymin2+ymax2)/2.0); glVertex2f(xmax, (ymin2+ymax2)/2.0); glEnd();
    //bar2 thresh: d2f(0-10)
    float th2 = edge_tab->get_d2f_threshold()*(xmax-xmin)/10.0 + xmin;
    glLineWidth(2);
    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_LINES); glVertex2f(th2, ymin2); glVertex2f(th2, ymax2); glEnd();

    //only respond to events inside the boxes
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    //if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT)
    if ((e.type == vgui_MOTION || e.type == vgui_BUTTON_DOWN) && e.button == vgui_LEFT)
    {
      if (ix>xmin && ix<xmax && iy>ymin1 && iy<ymax1){
        double newth1 = 30*(ix - xmin)/(xmax-xmin);
        edge_tab->set_threshold(newth1);
        vcl_cout <<"thresh = " << newth1 << vcl_endl;
        edge_tab->post_redraw();
      }

      if (ix>xmin && ix<xmax && iy>ymin2 && iy<ymax2){
        double newth2 = 10*(ix - xmin)/(xmax-xmin);
        edge_tab->set_d2f_threshold(newth2);
        vcl_cout <<"d2d thresh = " << newth2 << vcl_endl;
        edge_tab->post_redraw();
      }
    }

  }

  dbdet_edgemap_tableau *edge_tab;
};

//: Constructor
dbdet_edgemap_tableau::dbdet_edgemap_tableau(): 
  edgemap_(0), 
  display_anchor_points_(false),
  display_points_(false),
  point_size_(5.0),
  line_width_(1.0),
  line_length_(1.0),
  curr_color_(0.0,0.0,1.0),
  threshold_(0.0),
  d2f_thresh_(0.0),
  use_mix_thresh_(false),
  l1_(1.0), l2_(1.0), mix_thresh_(1.0),
  left_click_(vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true)),
  cur_edgel(0),
  display_bars(false),
  bars(new dbdet_edgemap_parameter_bars(this)),
  local_zoom_factor(1),
  display_image_grid_(false),
  display_uncertainty_(false)
{
}

dbdet_edgemap_tableau::~dbdet_edgemap_tableau()
{
  delete bars;
}

bool dbdet_edgemap_tableau::handle( const vgui_event & e )
{
  //first send it to the bars
  if (display_bars)
    bars->handle(e);

  //apply local zoom
  glTranslatef(-0.5, -0.5, 0);
  glScalef(local_zoom_factor, local_zoom_factor, 1);
  glTranslatef(0.5, 0.5, 0);

  if (e.type == vgui_MOTION)
  {
    // Get X,Y position to display on status bar:
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    // Display on status bar:
    vgui::out << "(" << ix << ", " << iy << ")" << vcl_endl;
  }

  //handle queries
  if (left_click_(e))
  {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

    // I) Find edgel closest to ix,iy
    // a) find the cell that this point belongs to
    int xx = dbdet_round(ix);
    int yy = dbdet_round(iy);

    //vcl_cout << "ix,iy: " << ix << "," << iy << vcl_endl;

    //reset cur_edgel
    cur_edgel = 0;

    // b) find the closest edgel in the neighboring cells
    double dmin = vcl_numeric_limits<double>::infinity();
    for (int xcell = xx-2; xcell <= xx+2; xcell++){
      for (int ycell = yy-2; ycell <= yy+2; ycell++){
        if (xcell < 0 || ycell < 0 || xcell >= (int)edgemap_->width() || ycell >= (int)edgemap_->height()) 
          continue;

        for (unsigned i=0; i<edgemap_->cell(xcell, ycell).size(); i++){
          dbdet_edgel* edgel = edgemap_->cell(xcell, ycell)[i];

          double dx = edgel->pt.x() - (double)ix; 
          double dy = edgel->pt.y() - (double)iy; 
          double d = dx*dx+dy*dy;
          if (d < dmin){
            dmin = d;
            cur_edgel = edgel;
          }
        }
      }
    }
    //output edgel info
    if (cur_edgel){
      vcl_cout << "Edgel " << cur_edgel->id << " : (x, y, theta, strength) = (";
      vcl_cout << cur_edgel->pt.x() << ", " << cur_edgel->pt.y() << ", " ;
      vcl_cout << cur_edgel->tangent << ", " << cur_edgel->strength << ")" << vcl_endl; 
      vcl_cout << "L Attr: " << cur_edgel->left_app->print_info() << vcl_endl;
      vcl_cout << "R Attr: " << cur_edgel->right_app->print_info() << vcl_endl;
    }

    // hightlight the selected edgel
    post_overlay_redraw();

    return true;
  }

  //actually draw the selected edgel
  if( e.type == vgui_OVERLAY_DRAW ) 
  {
    //if edgel selected, draw the groups it forms
    if (cur_edgel)
    {
      if (cur_edgel->strength>=threshold_ &&
          vcl_fabs(cur_edgel->deriv) >= d2f_thresh_)
      {
        glColor3f( 1.0-curr_color_[0], 1.0-curr_color_[1], 1.0-curr_color_[2] );
        glLineWidth (line_width_);
        glBegin( GL_LINE_STRIP );
        glVertex2d(cur_edgel->pt.x() - 0.5*line_length_*vcl_cos(cur_edgel->tangent),
                   cur_edgel->pt.y() - 0.5*line_length_*vcl_sin(cur_edgel->tangent));
        
        glVertex2d(cur_edgel->pt.x() + 0.5*line_length_*vcl_cos(cur_edgel->tangent),
                   cur_edgel->pt.y() + 0.5*line_length_*vcl_sin(cur_edgel->tangent));
        glEnd();
      }
    }

    return true;
  }

  //handle standard display
  if( e.type == vgui_DRAW )
  {
    if (display_image_grid_)
      display_image_grid();

    draw_edgels();
    return true;
  }

  return false;
}

void dbdet_edgemap_tableau::draw_edgels()
{
  if( edgemap_.ptr() == 0 ) 
    return;

  //setup GL
  glColor3f( curr_color_[0], curr_color_[1], curr_color_[2] );
  glLineWidth (line_width_);
  //gl2psLineWidth(line_width_);
  glPointSize(point_size_);
  //gl2psPointSize(point_size_);

  //go over each cell and draw each edgel in the cell
  dbdet_edgemap_const_iter it = edgemap_->edge_cells.begin();
  for (; it!=edgemap_->edge_cells.end(); it++){
    for (unsigned j=0; j<(*it).size(); j++){
      dbdet_edgel* e = (*it)[j];

      //apply the selected thresholds
      if (( use_mix_thresh_ && e->strength*vcl_fabs(e->deriv) > mix_thresh_) || 
          (!use_mix_thresh_ && e->strength>=threshold_&& vcl_fabs(e->deriv) >= d2f_thresh_))
      {
        if (display_points_) {
          glBegin(GL_POINTS);
          glVertex2d(e->pt.x(), e->pt.y());
          glEnd();
        }
        else {
          //glColor3f(1.0f, 0.0f, 0.0f); //red points
          //glBegin(GL_POINTS);
          //glVertex2d(e->pt.x(), e->pt.y());
          //glEnd();

          glColor3f( curr_color_[0], curr_color_[1], curr_color_[2] );
          glBegin( GL_LINE_STRIP );
          glVertex2d(e->pt.x() - 0.5*line_length_*vcl_cos(e->tangent),
                     e->pt.y() - 0.5*line_length_*vcl_sin(e->tangent));
          
          glVertex2d(e->pt.x() + 0.5*line_length_*vcl_cos(e->tangent),
                     e->pt.y() + 0.5*line_length_*vcl_sin(e->tangent));
          glEnd();

          
        }

        if (display_uncertainty_ && e->uncertainty >0.0)
        {
          //draw the bounding circles around each edgel
          glLineWidth (1.0);
          glBegin( GL_LINE_STRIP );
          for (int th=0; th<=20; th++){
            double theta = th*2*vnl_math::pi/20.0;
            glVertex2f(e->pt.x() + e->uncertainty*vcl_cos(theta), 
                       e->pt.y() + e->uncertainty*vcl_sin(theta));
          }
          glEnd();
        }
      }
    }
  }

  if (display_anchor_points_) {
    glColor3f( curr_color_[0], curr_color_[1], curr_color_[2]);
    glPointSize(point_size_);
    //gl2psPointSize(point_size_);
    glBegin( GL_POINTS );

    //go over each cell and draw each edgel in the cell
    //draw the point at the gird locations
    for (unsigned i=0; i<edgemap_->width(); i++){
      for (unsigned j=0; j<edgemap_->height(); j++){
        for (unsigned k=0; k<edgemap_->edge_cells(j,i).size(); k++){
          dbdet_edgel* e = edgemap_->edge_cells(j,i)[k];

          if (e->strength>threshold_)
            glVertex2d(i, j);
        }
      }
    }
    glEnd();
    //also draw connecting lines from the grid points to the subpixel locations
    glColor3f( 0.5, 0.5, 0.5 );
    glLineWidth (1);

    for (unsigned i=0; i<edgemap_->width(); i++){
      for (unsigned j=0; j<edgemap_->height(); j++){
        for (unsigned k=0; k<edgemap_->edge_cells(j,i).size(); k++){
          dbdet_edgel* e = edgemap_->edge_cells(j,i)[k];

          if (e->strength>threshold_){
            //gl2psLineWidth(line_width_);
            glBegin( GL_LINE_STRIP );
            glVertex2d(i, j);
            glVertex2d(e->pt.x(), e->pt.y());
            glEnd();
          }
        }
      }
    }


    ////draw the points at the edgel subixel location
    //dbdet_edgemap_const_iter it = edgemap_->edge_cells.begin();
    //for (; it!=edgemap_->edge_cells.end(); it++){
    //  for (unsigned j=0; j<(*it).size(); j++){
    //    dbdet_edgel* e = (*it)[j];

    //    if (e->strength>threshold_)
    //      glVertex2d(e->pt.x(), e->pt.y());
    //  }
    //}
    //glEnd();
  }
}

void dbdet_edgemap_tableau::display_image_grid()
{
  glColor3f( 0.7f, 0.7f, 0.7f );
  glLineWidth (1);
  for (unsigned i=0; i<=edgemap_->width(); i++){
    glBegin(GL_LINE_STRIP);
    glVertex2d(i, 0);
    glVertex2d(i, edgemap_->height());
    glEnd();
  }

  for (unsigned j=0; j<=edgemap_->height(); j++){
    glBegin(GL_LINE_STRIP);
    glVertex2d(0, j);
    glVertex2d(edgemap_->width(), j);
    glEnd();
  }
}

void 
dbdet_edgemap_tableau::get_popup(const vgui_popup_params& /*params*/, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( "Set edge threshold", new dbdet_edgemap_tableau_set_display_params_command(this, "Edge strength", &threshold_));

  submenu.add( "Set d2f threshold", new dbdet_edgemap_tableau_set_display_params_command(this, "Edge d2f", &d2f_thresh_));

  submenu.add( ((use_mix_thresh_)?on:off)+"Use Mixture Threshol", 
               new dbdet_edgemap_tableau_toggle_command(this, &use_mix_thresh_));

  submenu.add( "Set (str*d2f) threshold", new dbdet_edgemap_tableau_set_mixed_threshold_command(this));

  submenu.separator();

  submenu.add( ((display_anchor_points_)?on:off)+"Show Anchor Points", 
               new dbdet_edgemap_tableau_toggle_command(this, &display_anchor_points_));

  submenu.add( ((display_points_)?on:off)+"Show Edgel Points Only", 
               new dbdet_edgemap_tableau_toggle_command(this, &display_points_));

  submenu.add( "Set Style", new dbdet_edgemap_tableau_set_current_color_command(this));

  submenu.add( ((display_image_grid_)?on:off)+"Display Image Grid", 
               new dbdet_edgemap_tableau_toggle_command(this, &display_image_grid_));

  submenu.add( ((display_uncertainty_)?on:off)+"Display edgel Uncertainty", 
               new dbdet_edgemap_tableau_toggle_command(this, &display_uncertainty_));

  submenu.separator();

  submenu.add( ((display_bars)?on:off)+"Display Interactive Threshold Bars", 
               new dbdet_edgemap_tableau_toggle_command(this, &display_bars));

  submenu.add( "Set Local Zoom Factor", new dbdet_edgemap_tableau_set_int_params_command(this, "Zoom Factor", &local_zoom_factor));

  //add this submenu to the popup menu
  menu.add("Tableau Options", submenu);
}
