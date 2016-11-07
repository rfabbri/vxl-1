#include "dbdet_sel_contour_explorer_tool.h"

#include <vcl_limits.h>
#include <vcl_algorithm.h>

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h>
#include <bgui/bgui_vsol_soview2D.h>

#include <mbl/mbl_stats_1d.h>

#include <bvis1/bvis1_manager.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbdet/vis/dbdet_sel_tableau.h>

#include <dbgl/algo/dbgl_arc_algo.h>

class dbdet_sel_tableau_set_display_params_double_command : public vgui_command
{
 public:
  dbdet_sel_tableau_set_display_params_double_command(dbdet_sel_tableau* tab, 
    const vcl_string& name, const void* dref) : sel_tableau(tab),  dref_((double*)dref), name_(name) {}

  void execute() 
  { 
    double param_val = *dref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *dref_ = param_val;
    sel_tableau->post_redraw(); 
  }

  dbdet_sel_tableau* sel_tableau;
  double* dref_;
  vcl_string name_;
};

dbdet_sel_contour_explorer_tool::dbdet_sel_contour_explorer_tool():  
  sel_storage_(0), cur_edgel(0), cur_chain(0), 
  plot_curvature(false), plot_intensity(false), plot_strength(false), plot_d2f(false), 
  dist_wrt_cur(false), use_moving_avg(true), win_len(7), auto_scale_val(true)
{
  gesture0_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}


vcl_string dbdet_sel_contour_explorer_tool::name() const
{
  return "SEL contour explorer";
}

bool dbdet_sel_contour_explorer_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  dbdet_sel_tableau_sptr temp_tab;
  temp_tab.vertical_cast(tableau);
  if (!temp_tab.ptr())
    return false;

  bpro1_storage_sptr storage = bvis1_manager::instance()->storage_from_tableau(tableau);
  if (!storage.ptr())
    return false;

  //make sure its an edgemap storage class
  if (storage->type() == "sel"){
    dbdet_sel_storage_sptr sel_storage;
    sel_storage.vertical_cast(storage);

    sel_storage_ = sel_storage;
    sel_tab_ = temp_tab;
    return true;
  }
  return false;
}

bool dbdet_sel_contour_explorer_tool::handle( const vgui_event & e, 
                                             const bvis1_view_tableau_sptr& /*view*/ )
{
  if (gesture0_(e)) //left click
  {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
    //vcl_cout << "ix,iy: " << ix << "," << iy << vcl_endl;

    // I) Find edgel closest to ix,iy
    cur_edgel = get_nearest_edgel(vgl_point_2d<double>(ix, iy));

    cur_chain = 0;
    if (cur_edgel)
    {
      //the edgel chains are not anchored to any extrinsic structure
      //so we have to go over the list of edgel chains to find the current chain

      dbdet_edgel_chain_list_iter f_it = sel_storage_->CFG().frags.begin();
      for (; f_it != sel_storage_->CFG().frags.end(); f_it++)
      {  
        dbdet_edgel_chain* chain = (*f_it);

        //go over all the edgels in the chain to see if the current edgel is on it
        for (unsigned k=0; k<chain->edgels.size(); k++){
          if (chain->edgels[k]==cur_edgel){
            cur_chain = chain;
            break;
          }
        }

        if (cur_chain)//chain found already
          break;
      }
    }

    sel_tab_->post_overlay_redraw();
  }

  if( e.type == vgui_OVERLAY_DRAW ) {
    //if edgel selected, highlight the contour that it is a part of
    if (cur_chain){
      draw_edgel_chain(cur_chain);   

      //output some information about this edgel
      vcl_cout << "Selected edge Summary: " << vcl_endl;
      vcl_cout << "------------------------" << vcl_endl;
      vcl_cout << "L-int = " << cur_edgel->left_app->value() << ",  R-int = " << cur_edgel->right_app->value();
      vcl_cout << ",  Local Contrast = " << vcl_fabs(cur_edgel->left_app->value()-cur_edgel->right_app->value());
      vcl_cout << ",  Edge Strength = " << cur_edgel->strength << ", d2f = " << cur_edgel->deriv << vcl_endl;

      graph_parameters_along_chain(cur_chain);
    }
  }

  return false;
}

dbdet_edgel* dbdet_sel_contour_explorer_tool::get_nearest_edgel(vgl_point_2d<double> pt)
{
  dbdet_edgel* ne=0;

  // Find edgel closest to pt

  // a) find the cell that this point belongs to
  int xx = dbdet_round(pt.x());
  int yy = dbdet_round(pt.y());

  dbdet_edgemap_sptr edgemap = sel_storage_->EM();

  // b) find the closest edgel in the neighboring cells
  double dmin = vcl_numeric_limits<double>::infinity();
  for (int xcell = xx-2; xcell <= xx+2; xcell++){
    for (int ycell = yy-2; ycell <= yy+2; ycell++){
      if (xcell < 0 || ycell < 0 || xcell >= (int)edgemap->width() || ycell >= (int)edgemap->height()) 
        continue;

      for (unsigned i=0; i<edgemap->cell(xcell, ycell).size(); i++){
        dbdet_edgel* edgel = edgemap->cell(xcell, ycell)[i];

        double dx = edgel->pt.x() - pt.x(); 
        double dy = edgel->pt.y() - pt.y(); 
        double d = dx*dx+dy*dy;
        if (d < dmin){
          dmin = d;
          ne = edgel;
        }
      }
    }
  }

  return ne;
}

void dbdet_sel_contour_explorer_tool::draw_edgel_chain(dbdet_edgel_chain* chain)
{
  glColor3f(1.0, 1.0, 0.0);
  glLineWidth (3.0);
  glBegin(GL_LINE_STRIP);
  for (unsigned j=0; j<chain->edgels.size(); j++)
    glVertex2f(chain->edgels[j]->pt.x(), chain->edgels[j]->pt.y());
  glEnd();
  
}

void dbdet_sel_contour_explorer_tool::graph_parameters_along_chain(dbdet_edgel_chain* chain)
{
  //draw axes
  int vp[4]; // get viewport
  glGetIntegerv(GL_VIEWPORT,(GLint*)vp);

  float xmin, xmax, ymin, ymax;
  vgui_projection_inspector().window_to_image_coordinates(vp[0]+20, vp[1]+20, xmin, ymin);
  vgui_projection_inspector().window_to_image_coordinates(vp[2]-20, vp[1]+20+200, xmax, ymax);

  //draw the box
  glLineWidth(1);
  glColor3f(0.0f,0.0f,0.0f);
    
  glBegin(GL_LINE_STRIP);
  glVertex2f(xmin, ymin);glVertex2f(xmax, ymin);glVertex2f(xmax, ymax);glVertex2f(xmin, ymax); glVertex2f(xmin, ymin);
  glEnd();

  //now prepare to draw the graph of the parameters
  int cur_edgel_ind = -1;
  for (unsigned j=0; j<chain->edgels.size(); j++){
    if (chain->edgels[j]==cur_edgel)
      cur_edgel_ind = j;
  }

  //array to store the values being graphed
  vcl_vector<double> lvals(chain->edgels.size());
  vcl_vector<double> rvals(chain->edgels.size());

  vcl_deque<double> Lwin;
  vcl_deque<double> Rwin;

  //collect the appropriate data
  for (unsigned j=0; j<chain->edgels.size(); j++)
  {
    if (plot_curvature){
      if (chain->edgels.size()<3){
        lvals[j] = 0.0;
        rvals[j] = 0.0;
      }
      else{
        unsigned k1; //index of the first of 3 pts
        if (j<1)                            k1=0;
        else if  (j>chain->edgels.size()-2) k1=chain->edgels.size()-3;
        else                                k1 = j-1;

        //compute curvature
        lvals[j] = 1/dbgl_arc_algo::compute_arc_radius_from_three_points(chain->edgels[k1]->pt, chain->edgels[k1+1]->pt, chain->edgels[k1+2]->pt);
        rvals[j] = lvals[j];
      }
    }
    else if (plot_intensity){
      lvals[j] = chain->edgels[j]->left_app->value();
      rvals[j] = chain->edgels[j]->right_app->value();
    }
    else if (plot_strength){
      lvals[j] = chain->edgels[j]->strength;
      rvals[j] = chain->edgels[j]->strength;
    }
    else if (plot_d2f){ //second derivatives are negative
      lvals[j] = -chain->edgels[j]->deriv;
      rvals[j] = -chain->edgels[j]->deriv;
    }
    else { 
      //compute gradients of appearance along the curve
      if (dist_wrt_cur){
        lvals[j] = chain->edgels[cur_edgel_ind]->left_app->dist(*chain->edgels[j]->left_app);
        rvals[j] = chain->edgels[cur_edgel_ind]->right_app->dist(*chain->edgels[j]->right_app);
      }
      else { 
        if (j==0){ //first sample
          lvals[0] = chain->edgels[0]->left_app->dist(*chain->edgels[1]->left_app);
          rvals[0] = chain->edgels[0]->right_app->dist(*chain->edgels[1]->right_app);
          
          Lwin.push_back(lvals[0]);
          Lwin.push_back(rvals[0]);
        }
        else {
          if (use_moving_avg){
            Lwin.push_back(chain->edgels[j-1]->left_app->dist(*chain->edgels[j]->left_app));
            Rwin.push_back(chain->edgels[j-1]->right_app->dist(*chain->edgels[j]->right_app));

            if (Lwin.size()>win_len) Lwin.pop_front();
            if (Rwin.size()>win_len) Rwin.pop_front();
            
            mbl_stats_1d Ldata, Rdata;
            for (unsigned i=0; i< vcl_min(win_len, static_cast<unsigned>(Lwin.size())); i++) Ldata.obs(Lwin[i]);
            for (unsigned i=0; i< vcl_min(win_len, static_cast<unsigned>(Rwin.size())); i++) Rdata.obs(Rwin[i]);

            lvals[j] = Ldata.mean();
            rvals[j] = Rdata.mean();
          }
          else {
            lvals[j] = chain->edgels[j-1]->left_app->dist(*chain->edgels[j]->left_app);
            rvals[j] = chain->edgels[j-1]->right_app->dist(*chain->edgels[j]->right_app);
          }
        }
      }
    }
  }

  //determine bounds for the parameters
  float lvmin = 1000.0, lvmax = -1000.0;
  float rvmin = 1000.0, rvmax = -1000.0;

  //compute the stats of the parameters
  mbl_stats_1d Ldata(lvals);
  mbl_stats_1d Rdata(rvals);

  if (plot_curvature){
    vcl_cout << "Curve fragment summary: " << vcl_endl;
    vcl_cout << "------------------------" << vcl_endl;
    vcl_cout << "Mean K = " << Ldata.mean() << vcl_endl;
  }

  if (plot_intensity){
    vcl_cout << "Curve fragment summary: " << vcl_endl;
    vcl_cout << "------------------------" << vcl_endl;
    vcl_cout << "L-mean = " << vcl_floor(Ldata.mean()) << ",  R-mean = " << vcl_floor(Rdata.mean()) << ",  Mean Contrast = " << vcl_floor(vcl_fabs(Ldata.mean()-Rdata.mean())) << vcl_endl;
    vcl_cout << "L-std = " << vcl_floor(Ldata.sd()) << ",  R-std = " << vcl_floor(Rdata.sd()) << vcl_endl;
  }

  if (plot_d2f){
    vcl_cout << "Curve fragment summary: " << vcl_endl;
    vcl_cout << "------------------------" << vcl_endl;
    vcl_cout << "Mean d2f = " << Ldata.mean() << vcl_endl;
  }

  if (plot_strength){
    vcl_cout << "Curve fragment summary: " << vcl_endl;
    vcl_cout << "------------------------" << vcl_endl;
    vcl_cout << "Mean Edge Strength = " << Ldata.mean() << vcl_endl;
  }

  //determine mins and maxes
  lvmin = Ldata.min(); lvmax = Ldata.max();
  rvmin = Rdata.min(); rvmax = Rdata.max();

  if (plot_intensity){ //fixed [0 255] scale for intensity (special case)
    lvmin = 0.0; lvmax = 255.0;
    rvmin = 0.0; rvmax = 255.0;
    vmin = 0.0; vmax = 255.0;
  }

  if (auto_scale_val){
    vmin = vcl_min(lvmin, rvmin);
    vmax = vcl_max(lvmax, rvmax);

    vcl_cout << "scale : (" << vmin << " - " << vmax << ")" << vcl_endl;
  }
  else {
    //vmax = 255.0; comes from the dialog box now
    vmin = 0.0;
  }

  //draw left parameters
  float x_ratio = (xmax-xmin)/(chain->edgels.size()-1);
  float v_ratio = (ymax-ymin)/(vmax-vmin);

  glColor3f(0.0f,1.0f,0.0f);
  glBegin(GL_LINE_STRIP);
  for (unsigned j=0; j<lvals.size(); j++)
    glVertex2f(j*x_ratio + xmin, (lvals[j]-vmin)*v_ratio + ymin);
  glEnd();

  //draw right parameters
  glColor3f(0.0f,0.0f,1.0f);
  glBegin(GL_LINE_STRIP);
  for (unsigned j=0; j<rvals.size(); j++)
    glVertex2f(j*x_ratio + xmin, (rvals[j]-vmin)*v_ratio + ymin);
  glEnd();

  //mark the current edgel by a vertical line in the graph
  glColor3f(1.0f,0.0f,0.0f);
  glLineWidth(1);
  glBegin(GL_LINE_STRIP);
  glVertex2f(cur_edgel_ind*x_ratio + xmin, ymin);
  glVertex2f(cur_edgel_ind*x_ratio + xmin, ymax);
  glEnd();

  //Also mark the range of a typical curvelet 
  glColor3f(0.0f,1.0f,0.0f);
  glLineWidth(1);
  glBegin(GL_LINE_STRIP);
  glVertex2f((cur_edgel_ind-7/2.0)*x_ratio + xmin, ymin);
  glVertex2f((cur_edgel_ind-7/2.0)*x_ratio + xmin, ymax);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glVertex2f((cur_edgel_ind+7/2.0)*x_ratio + xmin, ymin);
  glVertex2f((cur_edgel_ind+7/2.0)*x_ratio + xmin, ymax);
  glEnd();
}

void dbdet_sel_contour_explorer_tool::get_popup( const vgui_popup_params& /*params*/, 
                                                vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add( ((plot_curvature)?on:off)+"Plot Curvature", 
            bvis1_tool_toggle, (void*)(&plot_curvature) );

  menu.add( ((plot_intensity)?on:off)+"Plot Intensity", 
            bvis1_tool_toggle, (void*)(&plot_intensity) );

  menu.add( ((plot_strength)?on:off)+"Plot Strength", 
            bvis1_tool_toggle, (void*)(&plot_strength) );

  menu.add( ((plot_d2f)?on:off)+"Plot D2f", 
            bvis1_tool_toggle, (void*)(&plot_d2f) );

  menu.separator();

  menu.add( ((dist_wrt_cur)?on:off)+"Compute Dist wrt current edgel", 
            bvis1_tool_toggle, (void*)(&dist_wrt_cur) );

  menu.add( ((use_moving_avg)?on:off)+"Use Moving Average", 
            bvis1_tool_toggle, (void*)(&use_moving_avg) );

  menu.add( "Set window size", 
            new dbdet_sel_tableau_set_display_params_double_command((dbdet_sel_tableau*)sel_tab_.ptr(), "Win Size", &win_len));

  menu.separator();

  menu.add( ((auto_scale_val)?on:off)+"AutoScale Value-Axis", 
            bvis1_tool_toggle, (void*)(&auto_scale_val) );

  menu.add( "Set max val", 
            new dbdet_sel_tableau_set_display_params_double_command((dbdet_sel_tableau*)sel_tab_.ptr(), "Max Value", &vmax));
  

  //menu.add( "Min order to display", 
  //          bvis1_tool_set_int_param, (void*)(&min_order_to_display_) );

  //menu.add( ((display_immediate_links_only_)?on:off)+"Display immeidate links only", 
  //          bvis1_tool_toggle, (void*)(&display_immediate_links_only_) );

}

