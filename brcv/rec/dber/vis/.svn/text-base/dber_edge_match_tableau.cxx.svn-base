// This is brcv/rec/dber/vis/dber_edge_match_tableau.cxx

//:
// \file

#include <float.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <bvis1/bvis1_manager.h>

#include "dber_edge_match_tableau.h"

//#include <dbskr/dbskr_sm_cor.h>
#include <dber/dber_match.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

#define DIST_THRESHOLD  1

//: Constructor
dber_edge_match_tableau::dber_edge_match_tableau(vcl_vector<vsol_line_2d_sptr>& l1, 
                                                 vcl_vector<vsol_line_2d_sptr>& l2,
                                                 vcl_vector<unsigned>& a) : lines1_(l1), lines2_(l2), assign_(a)
{
  gl_mode = GL_RENDER;

  //fill in the vcl_randomized color table
  for (int i=0; i<100; i++){
    for (int j=0; j<3;j++)
      rnd_colormap[i][j] = (vcl_rand() % 256)/256.0;
  }

  //: assign black as last color
  rnd_colormap[100][0] = 0.0f;
  rnd_colormap[100][1] = 0.0f;
  rnd_colormap[100][2] = 0.0f;
  select_color_ = 100;
  
  //fill in the vcl_randomized color table2
  for (int i=0; i<5000; i++){
    for (int j=0; j<3;j++)
      rnd_colormap2[i][j] = (vcl_rand() % 256)/256.0;
  }

  //offsets
  box1_ = new vsol_box_2d();
  for (unsigned i = 0; i<lines1_.size(); i++) {
    vsol_point_2d_sptr p = lines1_[i]->middle();
    box1_->add_point(p->x(), p->y());
  }
  
  offset_x1 = 0; offset_y1 = 0;
  offset_x2 = box1_->width()*1.2; offset_y2 = 0;

  selected_edgel_ = false;

  gesture_select = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture_splice1 = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  gesture_splice2 = vgui_event_condition(vgui_RIGHT, vgui_SHIFT, true);

  dominant_dir1_ = vgl_line_2d<double>();
  dominant_dir2_ = vgl_line_2d<double>();
  
}

bool dber_edge_match_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW )
      draw_render();

  if (gesture_select(e)) {
 
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix_, iy_);
    //vcl_cout << "ix: " << ix_ << " iy: " << iy_ << " ";
    vgl_point_2d<double> mouse_pt = vgl_point_2d<double>(ix_, iy_);

    // find the closest edgel in the first set,
    double min_d = 10000.0f;
    unsigned min_i = 0;
    vgl_point_2d<double> temp(0,0);
    for (unsigned i = 0; i<lines1_.size(); i++) {
      vsol_point_2d_sptr mid = lines1_[i]->middle();
      vgl_point_2d<double> m = mid->get_p();
      double d = vgl_distance(m, mouse_pt);
      if (d < min_d) {
        min_d = d;
        selected_edgel_ = true;
        selected_pt1_ = m;
        if (assign_[i] < 0 || assign_[i] >= lines2_.size())
          selected_pt2_ = temp;
        else
          selected_pt2_ = lines2_[assign_[i]]->middle()->get_p();       
      }
    }

    bvis1_manager::instance()->post_redraw();
  }

  return false;
}

void dber_edge_match_tableau::draw_render()
{
  /*if (!assign_.size() || !lines1_.size() || !lines2_.size()) {
    vcl_cout << "shock correspondence is not set!\n";
    return;
  }*/

  glBegin(GL_LINES);
  glColor3f(0.0,0.0,0.0);
  //glColor3f(1.0,0.0,0.0);
  glLineWidth(15.0);
  for (unsigned i = 0; i < lines1_.size(); i++) {  
    vsol_point_2d_sptr pt1 = lines1_[i]->p0();
    vsol_point_2d_sptr pt2 = lines1_[i]->p1();
    glVertex2f(pt1->x(), pt1->y());
    glVertex2f(pt2->x(), pt2->y());
  }
  glEnd();

  glBegin(GL_LINES);
  glColor3f(0.0,0.0,0.0);
  //glColor3f(0.0,0.0,1.0);
  glLineWidth(15.0);
  for (unsigned i = 0; i < lines2_.size(); i++) {
    vsol_point_2d_sptr pt1 = lines2_[i]->p0();
    vsol_point_2d_sptr pt2 = lines2_[i]->p1();
    glVertex2f(pt1->x()+offset_x2, pt1->y()+offset_y2);
    glVertex2f(pt2->x()+offset_x2, pt2->y()+offset_y2);
    //glVertex2f(pt1->x(), pt1->y());
    //glVertex2f(pt2->x(), pt2->y());
  }
  glEnd();
  
  glBegin(GL_LINES);
  glLineWidth(10.0);
  unsigned smaller = lines1_.size() < assign_.size() ? lines1_.size() : assign_.size();
  for (unsigned i = 0; i < smaller; i++) {
    if (assign_[i] < 0) continue;
    if (assign_[i] >= lines2_.size()) {
      //vcl_cout << "warning assignment i: " << i << " exceeds lines2 size\n";
      continue;
    }
    glColor3f(rnd_colormap[i%100][0],rnd_colormap[i%100][1],rnd_colormap[i%100][2]);
    vsol_point_2d_sptr pt1 = lines1_[i]->p0();
    vsol_point_2d_sptr pt2 = lines1_[i]->p1();
    glVertex2f(pt1->x(), pt1->y());
    glVertex2f(pt2->x(), pt2->y());
    //glEnd();

    //glBegin(GL_LINES);
    //glColor3f(rnd_colormap[i%100][0],rnd_colormap[i%100][1],rnd_colormap[i%100][2]);
    //glLineWidth(10.0);
    vsol_point_2d_sptr pt11 = lines2_[assign_[i]]->p0();
    vsol_point_2d_sptr pt22 = lines2_[assign_[i]]->p1();
    glVertex2f(pt11->x()+offset_x2, pt11->y()+offset_y2);
    glVertex2f(pt22->x()+offset_x2, pt22->y()+offset_y2);
  }
  glEnd();
/*
  glBegin(GL_POINTS);
  glPointSize(10.0);
  unsigned smaller = lines1_.size() < assign_.size() ? lines1_.size() : assign_.size();
  for (unsigned i = 0; i < smaller; i++) {
    if (assign_[i] < 0) continue;
    if (assign_[i] >= lines2_.size()) {
      //vcl_cout << "warning assignment i: " << i << " exceeds lines2 size\n";
      continue;
    }
    glColor3f(rnd_colormap[i%100][0],rnd_colormap[i%100][1],rnd_colormap[i%100][2]);
    vsol_point_2d_sptr pt1 = lines1_[i]->middle();
    glVertex2f(pt1->x(), pt1->y());
    vsol_point_2d_sptr pt11 = lines2_[assign_[i]]->middle();
    glVertex2f(pt11->x()+offset_x2, pt11->y()+offset_y2);
  }
  glEnd();
*/
  //draw the selected edgel and its match,
  if (selected_edgel_) {
    //glColor3f( 1 , 0 , 0 );
    glBegin( GL_POINTS );
    glPointSize(5.0);
    glColor3f( 1 , 0 , 0 );
    glVertex2f( selected_pt1_.x(), selected_pt1_.y() );
    glVertex2f( selected_pt2_.x()+offset_x2, selected_pt2_.y()+offset_y2 );
    glEnd();
  }

  //: draw the dominant directions
  glBegin(GL_LINES);
  glLineWidth(5.0);
  glColor3f(0,0,0);
  vgl_point_2d<double> p1, p2;
  dominant_dir1_.get_two_points(p1, p2);
  glVertex2f(p1.x(), p1.y());
  glVertex2f(p2.x(), p2.y());
  dominant_dir2_.get_two_points(p1, p2);
  glVertex2f(p1.x(), p1.y());
  glVertex2f(p2.x(), p2.y());
  glEnd();

}

class dbskr_sm_tableau_toggle_command : public vgui_command
{
 public:
  dbskr_sm_tableau_toggle_command(dber_edge_match_tableau* tab, const void* boolref) : 
       match_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    match_tableau->post_redraw(); 
  }

  dber_edge_match_tableau *match_tableau;
  bool* bref;
};

class dbskr_sm_tableau_set_display_params_command : public vgui_command
{
 public:
  dbskr_sm_tableau_set_display_params_command(dber_edge_match_tableau* tab, 
    const vcl_string& name, const void* intref) : match_tableau(tab), name_(name), iref_((int*)intref) {}

  void execute() 
  { 
    int param_val = *iref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *iref_ = param_val;
    match_tableau->post_redraw(); 
  }

  dber_edge_match_tableau *match_tableau;
  int* iref_;
  vcl_string name_;
};

void 
dber_edge_match_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";
/*
  submenu.add( ((display_shock_samples_)?on:off)+"Show Shock Samples", 
               new dbskr_sm_tableau_toggle_command(this, &display_shock_samples_));

  submenu.add( "Set sampling ds", 
               new dbskr_sm_tableau_set_display_params_command(this, "Sampling ds ", &sampling_ds_));
*/

  //add this submenu to the popup menu
  menu.add("Matching Display Options", submenu);
}
/*
//: if the distance of current selected point to this curve is less than 
//  a threshold than this one is selected 
//  there might be problems at the corners!!
double
dber_edge_match_tableau::selected(dbskr_scurve_sptr curve, double x, double y) 
{
  double min_d = 100000.0f;
  for( int j = 0 ; j < curve->num_points()-1 ; j++ ) {
    double val1 = vgl_distance_to_linesegment(curve->sh_pt(j).x(), curve->sh_pt(j).y(),
                                             curve->sh_pt(j+1).x(), curve->sh_pt(j+1).y(),
                                             x, y);
    double val2 = vgl_distance_to_linesegment(curve->bdry_plus_pt(j).x(), curve->bdry_plus_pt(j).y(),
                                             curve->bdry_plus_pt(j+1).x(), curve->bdry_plus_pt(j+1).y(),
                                             x, y);
    double val3 = vgl_distance_to_linesegment(curve->bdry_minus_pt(j).x(), curve->bdry_minus_pt(j).y(),
                                             curve->bdry_minus_pt(j+1).x(), curve->bdry_minus_pt(j+1).y(),
                                             x, y);
    double val = (val1 < val2 ? (val1 < val3 ? val1 : val3) : (val2 < val3 ? val2 : val3));
    if (val < min_d)
      min_d = val;
  }

  //if (min_d < DIST_THRESHOLD)
  //  return true;
  
  //return false;
  return min_d;
}

void 
dber_edge_match_tableau::draw_box(vsol_box_2d_sptr box)
{
  double minx = box->get_min_x();
  double miny = box->get_min_y();

  double maxx = box->get_max_x();
  double maxy = box->get_max_y();

  glBegin( GL_LINE_STRIP );
  glVertex2f(minx, miny); 
  glVertex2f(maxx, miny); 
  glVertex2f(maxx, maxy); 
  glVertex2f(minx, maxy); 
  glVertex2f(minx, miny); 
  glEnd();

}
*/

