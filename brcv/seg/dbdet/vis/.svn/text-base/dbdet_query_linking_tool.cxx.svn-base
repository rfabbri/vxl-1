// This is brcv/seg/dbdet/vis/dbdet_query_linking_tool.cxx
//:
// \file

#include "dbdet_query_linking_tool.h"

#include <vgui/vgui.h>
#include <vgui/vgui_style.h>

#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <dbgl/algo/dbgl_eulerspiral.h>

//Constructor
dbdet_query_linking_tool::dbdet_query_linking_tool()
{
  gesture_select_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture_select_ref_ = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  gesture_new_grouping_ = vgui_event_condition(vgui_key('n'), vgui_CTRL, false);
  gesture_zoom_in_ = vgui_event_condition(vgui_key('z'), vgui_CTRL, false);
  gesture_zoom_out_ = vgui_event_condition(vgui_key('a'), vgui_CTRL, false);

  gesture_pos_1 = vgui_event_condition(vgui_key('1'), vgui_CTRL, false);
  gesture_pos_2 = vgui_event_condition(vgui_key('2'), vgui_CTRL, false);
  gesture_pos_3 = vgui_event_condition(vgui_key('3'), vgui_CTRL, false);
  gesture_pos_4 = vgui_event_condition(vgui_key('4'), vgui_CTRL, false);
  gesture_pos_5 = vgui_event_condition(vgui_key('5'), vgui_CTRL, false);
  gesture_pos_6 = vgui_event_condition(vgui_key('6'), vgui_CTRL, false);
  gesture_pos_7 = vgui_event_condition(vgui_key('7'), vgui_CTRL, false);
  gesture_pos_8 = vgui_event_condition(vgui_key('8'), vgui_CTRL, false);
  gesture_pos_9 = vgui_event_condition(vgui_key('9'), vgui_CTRL, false);

  object_ = 0;

  ref_selected_ = false;
  edge_per_ = 4;

  grouping_valid_ = false;
  pairs_valid_ = false;

  zoom_factor_ = 1;

  dtheta_ = 0.1;
  dpos_ = 0.1;

  sel_pt_ = vgl_point_2d<double>(0,0);
  sel_tan_ = 0;

  edges_.clear();
  edge_so_.clear();
}


//: Destructor
dbdet_query_linking_tool::~dbdet_query_linking_tool()
{
}

//: Return the name of this tool
vcl_string
dbdet_query_linking_tool::name() const
{
    return "Query Linking";
}

//: Handle events
bool
dbdet_query_linking_tool::handle( const vgui_event & e, const bvis1_view_tableau_sptr& /*selector*/ )
{ 
  if (gesture_zoom_in_(e)){
    zoom_factor_++;
    tableau()->post_overlay_redraw();
  }

  if (gesture_zoom_out_(e)){
    if (zoom_factor_>1)
      zoom_factor_--;
    tableau()->post_overlay_redraw();
  }

  if (gesture_new_grouping_(e))
  {
    pairs_valid_ = false;
    grouping_valid_ = false;

    edge_so_.clear();
    edges_.clear();
  }

  if (gesture_pos_1(e)){
    edge_per_ = 0;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }
  else if (gesture_pos_2(e)){
    edge_per_ = 1;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }
  else if (gesture_pos_3(e)){
    edge_per_ = 2;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }
  else if (gesture_pos_4(e)){
    edge_per_ = 3;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }
  else if (gesture_pos_5(e)){
    edge_per_ = 4;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }
  else if (gesture_pos_6(e)){
    edge_per_ = 5;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }
  else if (gesture_pos_7(e)){
    edge_per_ = 6;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }
  else if (gesture_pos_8(e)){
    edge_per_ = 7;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }
  else if (gesture_pos_9(e)){
    edge_per_ = 8;
    form_edgel_grouping();
    tableau()->post_overlay_redraw();
  }

  if( tableau_.ptr() && gesture_select_(e) )
  {
    object_ = (bgui_vsol_soview2D*)tableau_->get_highlighted_soview();

    if( object_) {
      bgui_vsol_soview2D* object_ptr=(bgui_vsol_soview2D *)object_;
      if (object_ptr->type_name()=="bgui_vsol_soview2D_line_seg"){
        vsol_line_2d_sptr cur_line_seg = ((bgui_vsol_soview2D_line_seg*)object_ptr)->sptr();

        edges_.push_back(cur_line_seg);
        edge_so_.push_back((bgui_vsol_soview2D_line_seg*)object_ptr);
        vcl_cout << edges_.size() << "..";

        if (edges_.size()>1){
          ref_edge_ind_ = 0;
          form_edgel_grouping();
        }

        tableau()->post_redraw();
      }

      return true;
    }
  }

  if( tableau_.ptr() && gesture_select_ref_(e) )
  {
    object_ = (bgui_vsol_soview2D*)tableau_->get_highlighted_soview();

    if( object_) {
      bgui_vsol_soview2D* object_ptr=(bgui_vsol_soview2D *)object_;
      if (object_ptr->type_name()=="bgui_vsol_soview2D_line_seg"){
        vsol_line_2d_sptr cur_edge = ((bgui_vsol_soview2D_line_seg*)object_ptr)->sptr();

        //check to see if this is one of the edges selected
        for (unsigned i=0; i<edges_.size(); i++){
          if (cur_edge.ptr() == edges_[i].ptr()) 
          {
            ref_edge_ind_ = i;
            form_edgel_grouping();
            tableau()->post_redraw();
            break;
          }
        }

      }
    }
  }

  if( e.type == vgui_OVERLAY_DRAW ) 
  {
    //draw the current quad in green
    glLineWidth(3);
    glColor3f(0.0f,1.0f,0.0f);
    for (unsigned i=0; i<edge_so_.size(); i++){
      edge_so_[i]->draw();
    }

    glLineWidth(1);
    glColor3f(0.0f,1.0f,0.0f);
    //draw the links between the edgels
    glBegin(GL_LINE_STRIP);
    for (unsigned i=0; i<edges_.size(); i++){
      glVertex2f(edges_[i]->middle()->x(), edges_[i]->middle()->y());
    }
    glEnd();

    //draw the circles around the edgels
    for (unsigned i=0; i<edges_.size(); i++){
      if (i!=ref_edge_ind_){
        //draw a circle of radius dpos_
        double cx = edges_[i]->middle()->x();
        double cy = edges_[i]->middle()->y();

        glLineWidth(1);
        glColor3f(0.0f,1.0f,0.0f);
        glBegin(GL_LINE_STRIP);
        for (unsigned i=0; i<37; i++){
          glVertex2f(cx + dpos_*vcl_cos(i*2*vnl_math::pi/36.0), cy + dpos_*vcl_sin(i*2*vnl_math::pi/36.0));
        }
        glEnd();
      }
    }

    //draw the reference edgel in the perturbed position
    glLineWidth(3);
    glColor3f(0.0f,0.0f,1.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(sel_pt_.x()+0.25*vcl_cos(sel_tan_), sel_pt_.y()+0.25*vcl_sin(sel_tan_));
    glVertex2f(sel_pt_.x()-0.25*vcl_cos(sel_tan_), sel_pt_.y()-0.25*vcl_sin(sel_tan_));
    glEnd();
    //draw the box 

    int vp[4]; // get viewport
    glGetIntegerv(GL_VIEWPORT,(GLint*)vp);

    int plot_size = (int)zoom_factor_*100;

    float xmin, xmax, ymin, ymax;
    vgui_projection_inspector().window_to_image_coordinates(vp[0]+20, vp[1]+20, xmin, ymin);
    vgui_projection_inspector().window_to_image_coordinates(vp[0]+20+plot_size, vp[1]+20+plot_size, xmax, ymax);

    //draw the box for the k-gamma plot
    glLineWidth(1);
    if (pairs_valid_)
      glColor3f(0.0f,0.0f,0.0f);
    else
      glColor3f(1.0f,0.0f,0.0f);

    glBegin(GL_LINE_STRIP);
    glVertex2f(xmin, ymin);glVertex2f(xmax, ymin);glVertex2f(xmax, ymax);glVertex2f(xmin, ymax); glVertex2f(xmin, ymin);
    glEnd();

    if (pairs_valid_)
    {
      //draw the polygons corresponding to the curve bundles
      //at the bottom left corner of the screen

      const float kmin=-0.5, kmax=0.5, gmin=-0.5, gmax=0.5;
      float k_ratio = (xmax-xmin)/(kmax-kmin);
      float g_ratio = (ymin-ymax)/(gmax-gmin);

      //draw each of the polygons
      for (unsigned i=0; i<polys.size(); i++)
      {
        glColor3f(0.0f,1.0f,0.0f);
        glBegin(GL_LINE_STRIP);
        for (unsigned j=0; j<polys[i][0].size(); j++)
          glVertex2f((polys[i][0][j].x()-kmin)*k_ratio+xmin, (polys[i][0][j].y()-gmin)*g_ratio+ymax);
        glVertex2f((polys[i][0][0].x()-kmin)*k_ratio+xmin, (polys[i][0][0].y()-gmin)*g_ratio+ymax);
        glEnd();
      }

      if (grouping_valid_){
        //p_int
        glColor3f(1.0f,0.0f,0.0f);
        glBegin(GL_POLYGON);
        for (unsigned i=0; i<p_int[0].size(); i++)
          glVertex2f((p_int[0][i].x()-kmin)*k_ratio+xmin, (p_int[0][i].y()-gmin)*g_ratio+ymax);
        glEnd();

        if (fit_spline_){
          for (unsigned i=0; i<p_int[0].size(); i++){
            dbgl_eulerspiral es_fit(sel_pt_, sel_tan_, p_int[0][i].x(), p_int[0][i].y(), 3);

            glColor3f(1.0f,0.0f,0.0f);
            glBegin(GL_LINE_STRIP);
            for (double s=-4.0; s<0; s+=0.1)
              glVertex2f(es_fit.point_at_length(s).x(), es_fit.point_at_length(s).y());
            glEnd();

            glColor3f(1.0f,0.0f,0.0f);
            glBegin(GL_LINE_STRIP);
            for (double s=0; s<4.0; s+=0.1)
              glVertex2f(es_fit.point_at_length(s).x(), es_fit.point_at_length(s).y());
            glEnd();
          }
        }
      }
    }

    return true;
  }

  return false;
}



//: form an edgel chain from the current selection of the edges
void dbdet_query_linking_tool::form_edgel_grouping()
{
//#define NPerturb 1
//  //form edgels
//  vcl_vector<dbdet_edgel> edgels;
//  for (unsigned i=0; i<edges_.size(); i++)
//    edgels.push_back(form_edgel_from_line(edges_[i]));
//
//  //repostion the reference edgel at the desired perturbation
//  dbdet_edgel ref_edgel;
//
//  int pos_i = edge_per_/NPerturb;
//  int pos_j = edge_per_%NPerturb;
//
//  #if(NPerturb>1)
//    double dp = dpos_*(2*pos_i-NPerturb+1)/(NPerturb-1);
//    double dt = dtheta_*(2*pos_j-NPerturb+1)/(NPerturb-1);
//  #else
//    double dp = 0;
//    double dt = 0;
//  #endif
//
//  //compute perturbed point and tangent corresponding to this position
//  ref_edgel.pt = edgels[ref_edge_ind_].pt + vgl_vector_2d<double>(dp*vcl_cos(edgels[ref_edge_ind_].tangent+vnl_math::pi/2), 
//                                                                  dp*vcl_sin(edgels[ref_edge_ind_].tangent+vnl_math::pi/2));
//  ref_edgel.tangent = edgels[ref_edge_ind_].tangent + dt;
// 
//  //record this edgel position to be drawn
//  sel_pt_ = ref_edgel.pt;
//  sel_tan_ = ref_edgel.tangent;
//
//  //instantiate the edge linker class
//  dbdet_sel1 edge_linker(0, 0, 3, dtheta_, dpos_);
//
//  //reset this flag
//  pairs_valid_ = true;
//
//  //clear the polygons first
//  polys.clear();
//  vgl_polygon<double> cur_poly;
//
//  //now for the curve bundles with this perturbation of the reference edgel
//  for (unsigned i=0; i<edges_.size(); i++)
//  {
//    if (i<ref_edge_ind_)
//    {
//      bool p_valid = edge_linker.edgel_pair_legal2(&edgels[i], &ref_edgel, &ref_edgel, cur_poly);
//      pairs_valid_ = pairs_valid_ && p_valid;
//      vcl_cout << "P" << i+1 << ": " << (p_valid?"Y":"N") << vcl_endl;
//      polys.push_back(cur_poly);
//    }
//    else if (i>ref_edge_ind_){
//      bool p_valid = edge_linker.edgel_pair_legal2(&ref_edgel, &edgels[i], &ref_edgel, cur_poly);
//      pairs_valid_ = pairs_valid_ && p_valid;
//      vcl_cout << "P" << i+1 << ": " << (p_valid?"Y":"N") << vcl_endl;
//      polys.push_back(cur_poly);
//    }
//  }
//
//  if (pairs_valid_)
//  {
//    //compute the intersections of these bundles
//    grouping_valid_ = true;
//
//    p_int.clear();
//    p_int.new_sheet();
//    p_int.push_back(-0.5, -0.5);
//    p_int.push_back(0.5, -0.5);
//    p_int.push_back(0.5, 0.5);
//    p_int.push_back(-0.5, 0.5);
//    
//    for (unsigned i=0; i<polys.size(); i++){
//      vgl_polygon<double> p_int_new;
//      grouping_valid_ = grouping_valid_ && dbdet_intersect_curve_bundles(p_int, polys[i], p_int_new);
//      p_int = p_int_new;
//    }
//
//    vcl_cout << "Grouping valid: " << (grouping_valid_?"Yes.":"No.") << vcl_endl;
//  }
//
//#undef NPerturb

}

dbdet_edgel 
dbdet_query_linking_tool::form_edgel_from_line(vsol_line_2d_sptr line)
{
  vgl_point_2d<double> spt(line->p0()->x(), line->p0()->y());
  vgl_point_2d<double> ept(line->p1()->x(), line->p1()->y());
  vgl_point_2d<double> pt(line->middle()->x(), line->middle()->y());
  double tan = dbdet_vPointPoint(spt, ept);

  return dbdet_edgel(pt, tan, 0.0);
}

void
dbdet_query_linking_tool::deactivate()
{
}

void 
dbdet_query_linking_tool::get_popup( const vgui_popup_params& /*params*/, 
                                            vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add( "Set dpos", 
            bvis1_tool_set_param, (void*)(&dpos_) );

  menu.add( "Set dtheta", 
            bvis1_tool_set_param, (void*)(&dtheta_) );

  menu.add( "Plot: Zoom in ", 
            bvis1_tool_inc, (void*)(&zoom_factor_) );

  menu.add( "Plot: Zoom out", 
            bvis1_tool_dec, (void*)(&zoom_factor_) );

  menu.add( ((fit_spline_)?on:off)+"Fit Spline ", 
            bvis1_tool_toggle, (void*)(&fit_spline_) );
}
