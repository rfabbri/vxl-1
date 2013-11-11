// This is brcv/rec/dbskr/vis/dbskr_visualize_path_scurve_tool.cxx

//:
// \file

#include <vgui/vgui.h>
#include <vcl_algorithm.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/algo/dbskr_shock_path_finder.h>
#include "dbskr_visualize_path_scurve_tool.h"
#include <vsol/vsol_polygon_2d.h>
#include <vgl/vgl_distance.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>

#define DIST_THRESHOLD 1

dbskr_visualize_path_scurve_tool::dbskr_visualize_path_scurve_tool()
{
  select_node1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  select_node2 = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  next_curve = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
  next_graph = vgui_event_condition(vgui_key('b'), vgui_MODIFIER_NULL, true);
  create_shock_storage = vgui_event_condition(vgui_key('a'), vgui_SHIFT, true);  // add the current patch's shock to the storage
  
  node1 = 0;
  node2 = 0;
  cur_scurve = 0;

  binterpolate_ = true;
  interpolate_ds_ = 1.0;
  subsample_ = true;
  subsample_ds_ = 1.0;

  construct_circular_ends_ = true;
  current_ind_ = 0;
  current_graph_ind_ = 0;

  patch_depth1_ = 1.0;
  patch_depth2_ = 2.0;

  length_thres_ = 100000.0;

  use_patches_ = true;
  patch_box_ = 0;

  display_outer_boundary_ = true;
  display_traced_boudary_ = false;
  display_patches_box_ = false;
}

void
dbskr_visualize_path_scurve_tool::activate()
{
  if (!tableau()) {
    vcl_cout << " dbskr_visualize_path_scurve_tool::activate() - tableau is not set!\n";
    return;
  }

  dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
  if( sg.ptr() == 0 ) {if (tableau()->get_shock_graph())
    vcl_cout << "shock graph pointer is zero!\n";
    return;
  }
  
  pf_ = new dbskr_shock_path_finder(sg);
  if (!pf_->construct_v()) {
    vcl_cout << " dbskr_visualize_path_scurve_tool::activate() - problems in v_graph construction!\n";
    return;
  }

  vcl_cout << "printing shock graph: " << vcl_endl;
  print_shock_graph(sg);
  vcl_cout << "-----------------------" << vcl_endl;
  cur_scurve = 0;

  vcl_cout << "printing v graph: " << vcl_endl;
  print_v_graph(pf_->get_v());

}

dbskr_visualize_path_scurve_tool::~dbskr_visualize_path_scurve_tool()
{
  current_graph_ = 0;
  cur_scurve = 0;
  pf_->clear();
}

vcl_string
dbskr_visualize_path_scurve_tool::name() const
{
  return "Visualize Shock Path Scurve Tool";
}

bool
dbskr_visualize_path_scurve_tool::handle( const vgui_event & e, 
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

  if (next_curve(e))
  {
    current_ind_ = (scurves_.size() > current_ind_ + 1) ? current_ind_ + 1 : 0;
    
    if (scurves_.size() > current_ind_) {
      cur_scurve = scurves_[current_ind_];
      for (unsigned i = 0; i < paths_[current_ind_].size(); i++) {
        vcl_cout << paths_[current_ind_][i]->id_ << " ";
      }
      vcl_cout << vcl_endl;
    } 

    tableau()->post_overlay_redraw();
    return true;
  }

  if (next_graph(e))
  {
    current_graph_ind_ = (graphs_.size() > current_graph_ind_ + 1) ? current_graph_ind_ + 1 : 0;
    
    if (graphs_.size() > current_graph_ind_) {
      current_graph_ = graphs_[current_graph_ind_];
      vcl_cout << "length of the path for current graph: " << abs_lengths_[current_graph_ind_];

      sp_middle_ = extract_patch_from_v_graph(current_graph_, 0, 0, 0.05, construct_circular_ends_, true, true, interpolate_ds_, subsample_ds_);
    } 

    tableau()->post_overlay_redraw();
    return true;
  }

  if (select_node1(e)) 
  {
    node1 = 0;

    if (current_){
      current_->getInfo();

      dbsk2d_shock_node_sptr sel_node = get_selected_node();
      if (sel_node && sel_node->degree() != 2) {
        node1 = sel_node;
      } else {
        dbsk2d_shock_edge_sptr sel_edge = get_selected_edge();
        if (sel_edge) {
          if (sel_edge->source()->degree() != 2)
            node1 = sel_edge->source();
          else if (sel_edge->target()->degree() != 2)
            node1 = sel_edge->target();
        }
      }
      
      if (!node1) {
        vcl_cout << "Not a valid node!! Please select a degree 3 or 1 node!\n";
      }
    }
    
    tableau()->post_overlay_redraw();
    return true;
  } else if (select_node2(e)) {
    node2 = 0;

    if (current_){
      current_->getInfo();

      dbsk2d_shock_node_sptr sel_node = get_selected_node();
      if (sel_node && sel_node->degree() != 2){
        node2 = sel_node;
      } else {
        dbsk2d_shock_edge_sptr sel_edge = get_selected_edge();
        if (sel_edge) {
          if (sel_edge->source()->degree() != 2)
            node2 = sel_edge->source();
          else if (sel_edge->target()->degree() != 2)
            node2 = sel_edge->target();
        }
      }
      if (!node2) {
        vcl_cout << "selected node is not a valid node! Please select a degree 1 or 3 node!\n";
      }

    }

    if (node1 && node2) {
      scurves_.clear();
      paths_.clear();
      graphs_.clear();
      abs_lengths_.clear();

      if (use_patches_) {
        
        dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
        sp1_ = extract_patch_from_subgraph(sg, node1, (int)patch_depth1_, 0.05, construct_circular_ends_, true, true, interpolate_ds_, subsample_ds_);
        sp2_ = extract_patch_from_subgraph(sg, node2, (int)patch_depth2_, 0.05, construct_circular_ends_, true, true, interpolate_ds_, subsample_ds_);
        
        patch_box_ = sp1_->union_box(*sp2_);
        float box_perim = (float)(2*patch_box_->width() + 2*patch_box_->height());
        vcl_cout << "patch box perimeter: " << box_perim << vcl_endl;

        //: set the threshold to be 1.5*max_length as normalized
        //float norm_thres = 1.5f*max_l/box_perim;
        
        pf_->get_all_v_graphs(node1, node2, sp1_->get_v_graph(), sp2_->get_v_graph(), graphs_, abs_lengths_, 1.0f, length_thres_);
        if (graphs_.size() > 0) {
          current_graph_ = graphs_[0];
          //print_v_graph(current_graph_);
          sp_middle_ = extract_patch_from_v_graph(current_graph_, node1->id()*10000+ node2->id(), 0, 0.05, construct_circular_ends_, true, true, interpolate_ds_, subsample_ds_);
          current_graph_ind_ = 0;
          outer_edges_.clear();
          get_edges_on_outer_face(current_graph_, outer_edges_);
          vcl_cout << "path length for this graph: " << abs_lengths_[0] << vcl_endl;
          //vcl_cout << "edges on outer_edges_: " << vcl_endl;
          //for (unsigned i = 0; i < outer_edges_.size(); i++) {
          //  vcl_cout << "i: " << i << " s: " << outer_edges_[i]->source()->id_ << " t: " << outer_edges_[i]->target()->id_ << vcl_endl;
          //}
          vcl_cout << "-----------------\n";

        } else {
          current_graph_ = 0;
        }

        vcl_vector<float> abs_lengths2;
        pf_->get_all_v_node_paths(node1, node2, paths_, abs_lengths2, 1.0f, length_thres_);
        
      } else {  
        vcl_vector<float> abs_lengths2;
        pf_->get_all_v_node_paths(node1, node2, paths_, abs_lengths2, 1.0f, length_thres_);
      }
        
      for (unsigned i = 0; i < paths_.size(); i++) {
        vcl_vector<dbsk2d_shock_edge_sptr> edges;
        pf_->get_edges_on_path(paths_[i], edges);
        if (paths_[i].size() > 0 && edges.size() > 0) {
          //: test getting the shock paths
          dbskr_scurve_sptr sc = dbskr_compute_scurve(node1, edges, construct_circular_ends_, true, true, interpolate_ds_, subsample_ds_);
          vcl_cout << "sc arclength: " << sc->arclength(sc->num_points()-1) << vcl_endl;
          scurves_.push_back(sc);
        } else
          scurves_.push_back(0);
        current_ind_ = 0;
        if (scurves_.size() > 0) {
          cur_scurve = scurves_[0];
        } else {
          cur_scurve = 0;
        }
      }
      
    }
    
    tableau()->post_overlay_redraw();
    return true;
  } else if (create_shock_storage(e) && sp_middle_) {
    vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
    if(!res) {
      vcl_cout << "Could not access repository!\n";
      return false;
    }

    if (!sp_middle_->get_traced_boundary())
      sp_middle_->trace_outer_boundary();
      
    sp_middle_->shock_pruning_threshold_ = 1.0f;
    
    if (!sp_middle_->shock_graph())
      sp_middle_->extract_simple_shock();

    if (sp_middle_->get_traced_boundary() && sp_middle_->shock_graph()) {
    
      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      vcl_set<bpro1_storage_sptr> st_set = res->get_all_storage_classes(res->current_frame());
      vcl_string name_initial = "patch_vsol";
      int len = name_initial.length();
      int max = 0;
      for (vcl_set<bpro1_storage_sptr>::iterator iter = st_set.begin();
        iter != st_set.end(); iter++) {
          if ((*iter)->type() == output_vsol->type() && 
              (*iter)->name().find(name_initial) != vcl_string::npos) {
            vcl_string name = (*iter)->name();
            vcl_string numbr = name.substr(len, 3);
            int n = atoi(numbr.c_str());
            if (n > max)
              max = n;
          }
      }
      vcl_ostringstream oss;
      oss.width(3);
      oss.fill('0');
      oss << name_initial << max+1;
      name_initial = oss.str();
      output_vsol->set_name(name_initial);
      output_vsol->add_object(sp_middle_->get_traced_boundary()->cast_to_spatial_object());
      res->store_data(output_vsol);

      // add the shock as well 
      dbsk2d_shock_graph_sptr sg = sp_middle_->shock_graph();  // forces computation of the shock graph if not available
      dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
      name_initial = "patch_shock";
      len = name_initial.length();
      max = 0;
      for (vcl_set<bpro1_storage_sptr>::iterator iter = st_set.begin();
        iter != st_set.end(); iter++) {
          if ((*iter)->type() == output_shock->type() && 
              (*iter)->name().find(name_initial) != vcl_string::npos) {
            vcl_string name = (*iter)->name();
            vcl_string numbr = name.substr(len, 3);
            int n = atoi(numbr.c_str());
            if (n > max)
              max = n;
          }
      }
      vcl_ostringstream oss2;
      oss2.width(3);
      oss2.fill('0');
      oss2 << name_initial << max+1;
      name_initial = oss2.str();
      output_shock->set_name(name_initial);
      output_shock->set_shock_graph(sg);
      res->store_data(output_shock);

      bvis1_manager::instance()->add_to_display(output_vsol);
      bvis1_manager::instance()->add_to_display(output_shock);
      bvis1_manager::instance()->display_current_frame();
    }
  }

  if( e.type == vgui_OVERLAY_DRAW ) {
     
     //draw_shock_path(edges_, 1.0f, 1.0f, 0.0f);
     if (use_patches_) {
       //for (unsigned ii = 0; ii < graphs_.size(); ii++) 
         //draw_v_graph_nodes(graphs_[ii], 0.0f, 0.0f, 0.1f, 20.0f);
       if (current_graph_) {
          draw_v_graph_nodes(current_graph_, 0.0f, 0.0f, 0.1f, 30.0f);
          if (outer_edges_.size() > 0) {
            for (unsigned i = 0; i < outer_edges_.size(); i++) {
              if (outer_edges_[i]->source()->id_ > 0 && outer_edges_[i]->target()->id_ > 0) {
                for (unsigned j = 0; j < outer_edges_[i]->edges_.size(); j++) {
                  draw_base_gui_geometry((outer_edges_[i]->edges_[j]).ptr(), 0.0f, 0.0f, 0.0f, 10.0f);
                }
              }
            }
          }
       }
       if (sp_middle_)
         draw_patch(sp_middle_, 0.0f, 0.0f, 0.1f);
       if (sp1_) {
         draw_patch(sp1_, 0.7f, 0.6f, 0.2f);
         draw_v_graph_nodes(sp1_->get_v_graph(), 0.7f, 0.6f, 0.2f, 10.0f);
       }
       if (sp2_) {
         draw_patch(sp2_, 0.2f, 0.5f, 0.1f);
         draw_v_graph_nodes(sp2_->get_v_graph(), 0.2f, 0.5f, 0.1f, 10.0f);
       }

       if (display_patches_box_ && patch_box_) {
         draw_box(patch_box_, 1.0f, 0.0f, 0.0f);
       }
       
     }

     draw_scurve();
  }

  return dbsk2d_ishock_highlight_tool::handle(e, view);
}

dbsk2d_shock_node_sptr 
dbskr_visualize_path_scurve_tool::get_selected_node()
{
  return dynamic_cast<dbsk2d_shock_node*>(current_);
}

dbsk2d_shock_edge_sptr 
dbskr_visualize_path_scurve_tool::get_selected_edge()
{
  return dynamic_cast<dbsk2d_shock_edge*>(current_);
}

void dbskr_visualize_path_scurve_tool::draw_scurve()
{
  if (cur_scurve){
      draw_an_scurve(cur_scurve, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0f, 1.0f, 0.0f);
    
    //also draw the start node of this grouping
    glColor3f( 1.0f , 0.0f , 1.0f );
    glPointSize( 5.0 );
    glBegin( GL_POINTS );
    glVertex2f(cur_scurve->sh_pt_x(0), cur_scurve->sh_pt_y(0)); 
    glEnd();

  }
}

void dbskr_visualize_path_scurve_tool::draw_v_graph_nodes(dbskr_v_graph_sptr v, float r, float g, float b, float point_size)
{
  if (v) {
    glColor3f( r , g , b );
    glPointSize( point_size );
    glBegin( GL_POINTS );
      for (dbskr_v_graph::vertex_iterator v_itr = v->vertices_begin(); v_itr != v->vertices_end(); v_itr++) {
        dbsk2d_shock_node_sptr sn = (*v_itr)->original_shock_node_;
        if (sn > 0)
          glVertex2f(sn->ex_pts()[0].x(), sn->ex_pts()[0].y()); 
      }
    glEnd();
  }
}

void dbskr_visualize_path_scurve_tool::
draw_patch(dbskr_shock_patch_sptr shock_patch, float r, float g, float b)
{
  if (display_outer_boundary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_outer_boundary();
    if (poly) {
      glColor3f( r , g , b );
      glLineWidth (8.0);
      glBegin( GL_LINE_STRIP );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x(), poly->vertex(i)->y() );
      glVertex2f( poly->vertex(0)->x(), poly->vertex(0)->y() );
      glEnd();
    }
  }

  if (display_traced_boudary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_traced_boundary();
    if (poly) {
      glColor3f( r , g , b );
      glLineWidth (8.0);
      glBegin( GL_LINE_STRIP );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x(), poly->vertex(i)->y() );
      glVertex2f( poly->vertex(0)->x(), poly->vertex(0)->y() );
      glEnd();
    }
  }
}

void dbskr_visualize_path_scurve_tool::draw_an_scurve(dbskr_scurve_sptr cur_scurve, 
                                             float rp, float gp, float bp, 
                                             float rm, float gm, float bm,
                                             float rs, float gs, float bs)
{
  // draw plus and minus side of the scurve
  glColor3f( rp, gp, bp );
  glLineWidth (4.0);
  glBegin( GL_LINE_STRIP );
  for( int j = 0 ; j < cur_scurve->num_points() ; j++ ) {
    glVertex2f( cur_scurve->bdry_plus_pt(j).x(), cur_scurve->bdry_plus_pt(j).y() );
  }
  glEnd();

  glColor3f( rm, gm, bm );
  glBegin( GL_LINE_STRIP );
  for( int j = 0 ; j < cur_scurve->num_points() ; j++ ) {
    glVertex2f( cur_scurve->bdry_minus_pt(j).x(), cur_scurve->bdry_minus_pt(j).y() );
  }
  glEnd();

  glColor3f( rs, gs, bs );
  glBegin( GL_LINE_STRIP );
  for( int j = 0 ; j < cur_scurve->num_points() ; j++ ) {
    glVertex2f( cur_scurve->sh_pt(j).x(), cur_scurve->sh_pt(j).y() );
  }
  glEnd();
}

void dbskr_visualize_path_scurve_tool::draw_shock_path(vcl_vector<dbsk2d_shock_edge_sptr> &edges, float r, float g, float b)
{
  glColor3f( r, g, b );
  glLineWidth (2.0);
  glBegin( GL_LINE_STRIP );
  for (unsigned i = 0; i < edges.size(); i++) {
    dbsk2d_shock_edge_sptr e = edges[i];
    for( unsigned int j = 0 ; j < e->ex_pts().size() ; j++ ) {
      glVertex2f( e->ex_pts()[j].x() , e->ex_pts()[j].y() );
    }
  }
  glEnd();
}

void dbskr_visualize_path_scurve_tool::draw_box(vsol_box_2d_sptr box, float r, float g, float b)
{
  glColor3f( r, g, b );
  glLineWidth (2.0);
  glBegin( GL_LINE_STRIP );
  glVertex2f( box->get_min_x(), box->get_min_y() );
  glVertex2f( box->get_min_x(), box->get_max_y() );
  glVertex2f( box->get_max_x(), box->get_max_y() );
  glVertex2f( box->get_max_x(), box->get_min_y() );
  glEnd();
}

void 
dbskr_visualize_path_scurve_tool::get_popup( const vgui_popup_params& params, 
                                          vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add( "Set interpolate ds", 
            bvis1_tool_set_param, (void*)(&interpolate_ds_) );

  menu.add( "Set subsample ds", 
            bvis1_tool_set_param, (void*)(&subsample_ds_) );

  menu.add( ((binterpolate_)?on:off)+"Interpolate Scurve ", 
            bvis1_tool_toggle, (void*)(&binterpolate_) );

  menu.add( ((subsample_)?on:off)+"Subsample Scurve ", 
            bvis1_tool_toggle, (void*)(&subsample_) );
  
  menu.add( ((construct_circular_ends_)?on:off)+"Construct normal circular ends?", 
            bvis1_tool_toggle, (void*)(&construct_circular_ends_ ));

  menu.separator();

  menu.add( ((use_patches_)?on:off)+"Use Patches to limit paths?", 
            bvis1_tool_toggle, (void*)(&use_patches_) );

  menu.add( "Set patch 1 depth", 
            bvis1_tool_set_param, (void*)(&patch_depth1_) );

  menu.add( "Set patch 2 depth", 
            bvis1_tool_set_param, (void*)(&patch_depth2_) );

  menu.add( "Set length threshold to prune paths", bvis1_tool_set_param, (void*)(&length_thres_) );

  menu.add( ((display_patches_box_)?on:off)+"display_patches_box_?", 
            bvis1_tool_toggle, (void*)(&display_patches_box_) );

  menu.add( ((display_outer_boundary_)?on:off)+"display_outer_boundary_?", 
            bvis1_tool_toggle, (void*)(&display_outer_boundary_) );

  menu.add( ((display_traced_boudary_)?on:off)+"display_traced_boudary_?", 
            bvis1_tool_toggle, (void*)(&display_traced_boudary_) );

 }
