// This is brcv/rec/dbskr/vis/dbskr_subgraph_scurve_tool.cxx

//:
// \file

#include <vgui/vgui.h>
#include <vcl_algorithm.h>
//#include <dbskr/dbskr_compute_scurve.h>
#include <dbskr/dbskr_scurve.h>

#include <dbskr/algo/dbskr_rec_algs.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_extract_subgraph.h>

#include "dbskr_subgraph_scurve_tool.h"

#include <vgl/vgl_distance.h>

#include <vsol/vsol_polygon_2d.h>

#define DIST_THRESHOLD 1

dbskr_subgraph_scurve_tool::dbskr_subgraph_scurve_tool()
{
  select_edge = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  increase_depth = vgui_event_condition(vgui_key('i'), vgui_MODIFIER_NULL, true);
  decrease_depth = vgui_event_condition(vgui_key('u'), vgui_MODIFIER_NULL, true);
  reset_depth = vgui_event_condition(vgui_key('r'), vgui_MODIFIER_NULL, true);
  switch_drawing = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);
  
  binterpolate_ = true;
  interpolate_ds_ = 1.0;
  subsample_ = true;
  subsample_ds_ = 1.0;
  depth_ = 1;
  draw_boundary_ = true;
  poly_ = 0;
  poly_area_threshold_ = 20.0f;

  draw_with_circular_completions_ = false;
  color_r_ = 1;
  color_g_ = 1;
  color_b_ = 0;

  line_width_ = 6;
}

void
dbskr_subgraph_scurve_tool::activate()
{
  if (!tableau()) {
    vcl_cout << " dbskr_subgraph_scurve_tool::activate() - tableau is not set!\n";
    return;
  }

  dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
  if( sg.ptr() == 0 ) {if (tableau()->get_shock_graph())
    vcl_cout << "shock graph pointer is zero!\n";
    return;
  }

  cur_scurves_.clear();
  //visited_nodes_.clear();
  depth_ = 1;
}

dbskr_subgraph_scurve_tool::~dbskr_subgraph_scurve_tool()
{
}

vcl_string
dbskr_subgraph_scurve_tool::name() const
{
  return "Draw Subgraph Scurve Tool (from sampled coarse shock)";
}

bool
dbskr_subgraph_scurve_tool::handle( const vgui_event & e, 
                                    const bvis1_view_tableau_sptr& view )
{
  float pointx, pointy;
  vgui_projection_inspector p_insp;
  p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);

  if (e.type == vgui_MOTION)
  {
    int intx = (int)vcl_floor(pointx), inty = (int)vcl_floor(pointy);
    vgui::out << "[" << intx << " " << inty << "] : (" << pointx << " " << pointy << ") \n";
  }

  if (switch_drawing(e)) {
    draw_boundary_ = !draw_boundary_;
    tableau()->post_overlay_redraw();
    return true;
  }

  if (select_edge(e)) 
  {
    //reset vars
    cur_scurves_.clear();
    //visited_nodes_.clear();
    end_scurve_.clear();
    depth_ = 1.0f;

    if (current_){
      current_->getInfo();


      dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
      dbsk2d_shock_edge* sedge = dynamic_cast<dbsk2d_shock_edge*>(current_);
      if (sedge) {
        if (sedge->target()->degree() >= 3) { // use this node
          vcl_cout << "Selected edge has a degree three target, using this node...\n";
          //get_scurves(sedge->target(), 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, sedge->target(), cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, sedge->target(), depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else if (sedge->source()->degree() >= 3) {
          vcl_cout << "Selected edge has a degree three source, using this node...\n";
          //get_scurves(sedge->source(), 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, sedge->source(), cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, sedge->source(), depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else {
          vcl_cout << "Selected edge does not have a degree three source or target! Select another edge with a degree three source or target or a degree three node!\n";
        }

      } else {

        dbsk2d_shock_node* snode = dynamic_cast<dbsk2d_shock_node*>(current_);
        if (snode && snode->degree() >= 3) {
          //get_scurves(snode, 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, snode, cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, snode, depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else {
          vcl_cout << "Selected node is not a degree three node! Select another node!\n";
        }
      }
    }
    tableau()->post_overlay_redraw();
    return true;
  }

  if (increase_depth(e)) 
  {
    //reset vars
    cur_scurves_.clear();
    //visited_nodes_.clear();
    end_scurve_.clear();

    depth_ = depth_ + 1;
    vcl_cout << "depth increased to " << depth_ << vcl_endl;

    if (current_){
      current_->getInfo();

      dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
      dbsk2d_shock_edge* sedge = dynamic_cast<dbsk2d_shock_edge*>(current_);
      if (sedge) {
        if (sedge->target()->degree() >= 3) { // use this node
          vcl_cout << "Selected edge has a degree three target, using this node...\n";
          //get_scurves(sedge->target(), 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, sedge->target(), cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, sedge->target(), depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else if (sedge->source()->degree() >= 3) {
          vcl_cout << "Selected edge has a degree three source, using this node...\n";
          //get_scurves(sedge->source(), 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, sedge->source(), cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, sedge->source(), depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else {
          vcl_cout << "Selected edge does not have a degree three source or target! Select another edge with a degree three source or target or a degree three node!\n";
        }

      } else {

        dbsk2d_shock_node* snode = dynamic_cast<dbsk2d_shock_node*>(current_);
        if (snode && snode->degree() >= 3) {
          //get_scurves(snode, 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, snode, cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, snode, depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else {
          vcl_cout << "Selected node is not a degree three node! Select another node!\n";
        }
      }
    }
    tableau()->post_overlay_redraw();
    return true;
  }

  if (decrease_depth(e)) 
  {
    //reset vars
    cur_scurves_.clear();
    //visited_nodes_.clear();
    end_scurve_.clear();

    if (depth_ > 1) {
      depth_ = depth_ - 1;
      vcl_cout << "depth decreased to " << depth_ << vcl_endl;
    } else 
      vcl_cout << "depth is 1 already, not decreased!\n";

    if (current_){
      current_->getInfo();

      dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
      dbsk2d_shock_edge* sedge = dynamic_cast<dbsk2d_shock_edge*>(current_);
      if (sedge) {
        if (sedge->target()->degree() >= 3) { // use this node
          vcl_cout << "Selected edge has a degree three target, using this node...\n";
          //get_scurves(sedge->target(), 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, sedge->target(), cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, sedge->target(), depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else if (sedge->source()->degree() >= 3) {
          vcl_cout << "Selected edge has a degree three source, using this node...\n";
          //get_scurves(sedge->source(), 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, sedge->source(), cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, sedge->source(), depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else {
          vcl_cout << "Selected edge does not have a degree three source or target! Select another edge with a degree three source or target or a degree three node!\n";
        }

      } else {

        dbsk2d_shock_node* snode = dynamic_cast<dbsk2d_shock_node*>(current_);
        if (snode && snode->degree() >= 3) {
          //get_scurves(snode, 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
          get_scurves(sg, snode, cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
          if (draw_boundary_)
            poly_ = trace_boundary_from_subgraph(sg, snode, depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
        } else {
          vcl_cout << "Selected node is not a degree three node! Select another node!\n";
        }
      }
    }
      tableau()->post_overlay_redraw();
      return true;
  }

    if (reset_depth(e)) 
    {
      //reset vars
      cur_scurves_.clear();
      //visited_nodes_.clear();
      end_scurve_.clear();

      depth_ = 1.0f;
      vcl_cout << "depth is reset to 1\n";

      if (current_){
        current_->getInfo();

        dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
        dbsk2d_shock_edge* sedge = dynamic_cast<dbsk2d_shock_edge*>(current_);
        if (sedge) {
          if (sedge->target()->degree() >= 3) { // use this node
            vcl_cout << "Selected edge has a degree three target, using this node...\n";
            //get_scurves(sedge->target(), 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
            get_scurves(sg, sedge->target(), cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
            if (draw_boundary_)
              poly_ = trace_boundary_from_subgraph(sg, sedge->target(), depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
          } else if (sedge->source()->degree() >= 3) {
            vcl_cout << "Selected edge has a degree three source, using this node...\n";
            //get_scurves(sedge->source(), 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
            get_scurves(sg, sedge->source(), cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
            if (draw_boundary_)
              poly_ = trace_boundary_from_subgraph(sg, sedge->source(), depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
          } else {
            vcl_cout << "Selected edge does not have a degree three source or target! Select another edge with a degree three source or target or a degree three node!\n";
          }

        } else {

          dbsk2d_shock_node* snode = dynamic_cast<dbsk2d_shock_node*>(current_);
          if (snode && snode->degree() >= 3) {
            //get_scurves(snode, 0, cur_scurves_, visited_nodes_, end_scurve_, depth_);
            get_scurves(sg, snode, cur_scurves_, end_scurve_, depth_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_);
            if (draw_boundary_)
              poly_ = trace_boundary_from_subgraph(sg, snode, depth_, draw_with_circular_completions_, binterpolate_, subsample_, interpolate_ds_, subsample_ds_, poly_area_threshold_);
          } else {
            vcl_cout << "Selected node is not a degree three node! Select another node!\n";
          }
        }
      }
      tableau()->post_overlay_redraw();
      return true;
    }
  
  if( e.type == vgui_OVERLAY_DRAW ) {
     draw_scurves();
  }

  return dbsk2d_ishock_highlight_tool::handle(e, view);
}


void dbskr_subgraph_scurve_tool::draw_scurves()
{
  if (draw_boundary_ && poly_) {
    glColor3f( color_r_, color_g_, color_b_ );
    glLineWidth (line_width_);
    glBegin( GL_LINE_STRIP );
    unsigned j;
    for(j = 0 ; j < poly_->size() ; j++ ) {
      glVertex2f( poly_->vertex(j)->x(), poly_->vertex(j)->y() );
    }
    glVertex2f( poly_->vertex(0)->x(), poly_->vertex(0)->y() );
    glEnd();
    
  } else {
    for (unsigned i = 0; i < cur_scurves_.size(); i++) {
      draw_an_scurve(cur_scurves_[i], 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, end_scurve_[i]);
      
      //also draw the start node of this grouping
      glColor3f( 1.0f , 0.0f , 1.0f );
      glPointSize( 5.0 );
      glBegin( GL_POINTS );
      glVertex2f(cur_scurves_[i]->sh_pt_x(0), cur_scurves_[i]->sh_pt_y(0)); 
      glEnd();
    }
  }
}

void dbskr_subgraph_scurve_tool::draw_an_scurve(dbskr_scurve_sptr cur_scurve, 
                                             float rp, float gp, float bp, 
                                             float rm, float gm, float bm, bool end_scurve)
{
  // draw plus and minus side of the scurve
  glColor3f( rp, gp, bp );
  glLineWidth (10.0);
  glBegin( GL_LINE_STRIP );
  int j;
  for( j = 0 ; j < cur_scurve->num_points() ; j++ ) {
    glVertex2f( cur_scurve->bdry_plus_pt(j).x(), cur_scurve->bdry_plus_pt(j).y() );
  }
  glEnd();
  
  if (end_scurve) {
    int id = cur_scurve->num_points()-1;
    glColor3f( 1.0, 1.0, 0 );
    glLineWidth (7.0);
    glBegin( GL_LINE_STRIP );
    glVertex2f( cur_scurve->bdry_plus_pt(id).x(), cur_scurve->bdry_plus_pt(id).y() );
    glVertex2f( cur_scurve->sh_pt_x(id), cur_scurve->sh_pt_y(id) );
    glVertex2f( cur_scurve->bdry_minus_pt(id).x(), cur_scurve->bdry_minus_pt(id).y() );
    glEnd();
  }

  glColor3f( rm, gm, bm );
  glLineWidth (10.0);
  glBegin( GL_LINE_STRIP );
  for(int j = cur_scurve->num_points()-1 ; j >=0 ; j-- ) {
    glVertex2f( cur_scurve->bdry_minus_pt(j).x(), cur_scurve->bdry_minus_pt(j).y() );
  }
  glEnd();

  //glColor3f( 0, 0, 0 );
  //glLineWidth (3.0);
  //glBegin( GL_LINE_STRIP );
  //glVertex2f( cur_scurve->bdry_plus_pt(0).x(), cur_scurve->bdry_plus_pt(0).y() );
  //glVertex2f( cur_scurve->sh_pt_x(0), cur_scurve->sh_pt_y(0) );
  //glVertex2f( cur_scurve->bdry_minus_pt(0).x(), cur_scurve->bdry_minus_pt(0).y() );
  //glEnd();

  glColor3f( 0.0, 1.0, 1.0 );
  glBegin( GL_LINE_STRIP );
  glLineWidth (10.0);
  for( int j = 0 ; j < cur_scurve->num_points() ; j++ ) {
    glVertex2f( cur_scurve->sh_pt_x(j), cur_scurve->sh_pt_y(j) );
  }
  glEnd();
}

void 
dbskr_subgraph_scurve_tool::get_popup( const vgui_popup_params& params, 
                                          vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add( "Set interpolate ds", 
            bvis1_tool_set_param, (void*)(&interpolate_ds_) );

  menu.add( "Set subsample ds", 
            bvis1_tool_set_param, (void*)(&subsample_ds_) );

  menu.add( "Set depth", 
            bvis1_tool_set_param, (void*)(&depth_) );

  menu.add( "Set color r", bvis1_tool_set_param, (void*)(&color_r_) );
  
  menu.add( "Set color g", bvis1_tool_set_param, (void*)(&color_g_) );
  
  menu.add( "Set color b", bvis1_tool_set_param, (void*)(&color_b_) );

  menu.add( "Set line width", bvis1_tool_set_param, (void*)(&line_width_) );

  menu.add( "Set polygon area threshold", 
            bvis1_tool_set_param, (void*)(&poly_area_threshold_) );

  menu.add( ((binterpolate_)?on:off)+"Interpolate Scurve ", 
            bvis1_tool_toggle, (void*)(&binterpolate_) );

  menu.add( ((subsample_)?on:off)+"Subsample Scurve ", 
            bvis1_tool_toggle, (void*)(&subsample_) );

  menu.add( ((draw_with_circular_completions_)?on:off)+"Draw with circular completions at leaves?", 
            bvis1_tool_toggle, (void*)(&draw_with_circular_completions_) );

  
  
}
