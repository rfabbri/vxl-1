// This is brcv/rec/dbskr/vis/dbskr_debug_scurve_tool.cxx

//:
// \file

#include <vgui/vgui.h>
#include <vcl_algorithm.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include "dbskr_debug_scurve_tool.h"

#include <vgl/vgl_distance.h>

#define DIST_THRESHOLD 1

dbskr_debug_scurve_tool::dbskr_debug_scurve_tool()
{
  select_edge = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  add_edges = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  reverse_sampling_cmd  = vgui_event_condition(vgui_key('r'), vgui_CTRL, true);
  select_tree_edge = vgui_event_condition(vgui_RIGHT, vgui_SHIFT, true);
  update_splice_map = vgui_event_condition(vgui_key('u'), vgui_MODIFIER_NULL, true);

  selected_edges.clear();
  start_node = 0;
  cur_scurve = 0;

  binterpolate_ = true;
  interpolate_ds_ = 1.0;
  subsample_ = true;
  subsample_ds_ = 5.0;

  reverse_sampling_ = false;
  display_spliced_contours_ = false;
  output_selected_scurve_info_ = false;

  selected_dart_ = -1;

  add_splice_ = false;
  threshold_ = 5.0f;

  construct_circular_ends_ = false;
  elastic_splice_cost_ = false;
  find_splice_cost_using_dpmatch_combined_ = false;

  R_ = 6.0;
}

void
dbskr_debug_scurve_tool::activate()
{
  if (!tableau()) {
    vcl_cout << " dbskr_debug_scurve_tool::activate() - tableau is not set!\n";
    return;
  }

  dbsk2d_shock_graph_sptr sg = tableau()->get_shock_graph();
  if( sg.ptr() == 0 ) {if (tableau()->get_shock_graph())
    vcl_cout << "shock graph pointer is zero!\n";
    return;
  }
  
  tree_ = new dbskr_tree(subsample_ds_, interpolate_ds_);
  tree_->acquire(sg, elastic_splice_cost_, construct_circular_ends_, find_splice_cost_using_dpmatch_combined_);  
  dart_cnt_ = tree_->size();
  vcl_cout << "tree # of darts: " << dart_cnt_ << " sorting costs...\n";

  //: keep a sorted map of all darts based on splice costs to determine which get pruned 
  //  and which stay
  for (unsigned i = 0; i<dart_cnt_; i++) {
    vcl_pair<double, unsigned> p(tree_->subtree_delete_cost(i), i);
    dart_splice_map_[p] = true;
  }
  update_dart_splice_map();
  vcl_cout << "printing shock graph: " << vcl_endl;
  print_shock_graph(sg);
  vcl_cout << "-----------------------" << vcl_endl;
  vcl_cout << "tree total reconstructed boundary length: " << tree_->total_reconstructed_boundary_length() << vcl_endl;

  /*dbskr_tree_sptr new_tree = new dbskr_tree(1.0f);
  new_tree->acquire_and_prune(sg, threshold_, true);
  */
}

void 
dbskr_debug_scurve_tool::update_dart_splice_map() {
  vcl_map< vcl_pair<double, unsigned>, bool>::iterator iter;
  for (iter = dart_splice_map_.begin(); iter != dart_splice_map_.end(); iter++) {
    iter->second = true;
  }
    /*if (add_splice_) {
      double sum = 0.0f;
      for (iter = dart_splice_map_.begin(); iter != dart_splice_map_.end(); iter++) {
        sum += (iter->first).first;
        if (sum > threshold)
          break;
        iter->second = false;
      }
    } else {*/
  double sum = 0.0f;
  
  for (iter = dart_splice_map_.begin(); iter != dart_splice_map_.end(); iter++) {
    if ((iter->first).first > threshold_)
      break;
    iter->second = false;
    sum += tree_->delete_cost((iter->first).second);
  }
  
    //}
#if 1
    for (iter = dart_splice_map_.begin(); iter != dart_splice_map_.end(); iter++) {
      if (iter->second)
        vcl_cout << (iter->first).first << "\td: " << (iter->first).second << " stays\n";
      else
        vcl_cout << (iter->first).first << "\td: " << (iter->first).second << " goes\n";
    }
#endif
  vcl_cout << "Total pruning: " << sum << vcl_endl;  

  
}

dbskr_debug_scurve_tool::~dbskr_debug_scurve_tool()
{
}

vcl_string
dbskr_debug_scurve_tool::name() const
{
  return "Debug Scurve Tool";
}

bool
dbskr_debug_scurve_tool::handle( const vgui_event & e, 
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

  if (update_splice_map(e) && tree_)  {
    update_dart_splice_map();
    tableau()->post_overlay_redraw();
    return true;
  }

  if (select_tree_edge(e) && tree_) 
  {
    if (current_){
      dbsk2d_shock_edge_sptr sel_edge = get_selected_edge();
      if (sel_edge){
        for (unsigned i = 0; i<dart_cnt_; i++) {
          for (unsigned j=0; j<tree_->get_shock_edges(i).size(); j++){
            if (tree_->get_shock_edges(i)[j]->id() == sel_edge->id())
            {
              //selected dart found
              selected_dart_ = i;
              int selected_dart_mate_ = tree_->mate(selected_dart_);

              vcl_cout << "selected_dart:\t\t" << selected_dart_ << "\t splice: " << tree_->delete_cost(selected_dart_) << "\tsubtree delete:\t" << tree_->subtree_delete_cost(selected_dart_) << "\tcontract:\t" << tree_->contract_cost(selected_dart_) << vcl_endl; 
              vcl_cout << "selected_dart_mate:\t" << selected_dart_mate_ << "\t splice: " << tree_->delete_cost(selected_dart_mate_) << "\tsubtree delete:\t" << tree_->subtree_delete_cost(selected_dart_mate_) << "\tcontract:\t" << tree_->contract_cost(selected_dart_mate_) << vcl_endl; 
              
              tableau()->post_overlay_redraw();
              return true;
            }
          }
        }
      }
    }
  }

  if (select_edge(e)) 
  {
    //reset vars
    selected_edges.clear();
    start_node = 0;
    reverse_sampling_ = false;

    if (current_){
      current_->getInfo();

      dbsk2d_shock_edge_sptr sel_edge = get_selected_edge();
      if (sel_edge){
        selected_edges.push_back(sel_edge);
        start_node = selected_edges.front()->target();

        //construct the scurve from these edges
        cur_scurve = get_scurve();
        if (cur_scurve) {
          vcl_cout << "splice cost = " << cur_scurve->splice_cost_new(R_, construct_circular_ends_, find_splice_cost_using_dpmatch_combined_) << vcl_endl;
          vcl_cout << "\t plus bnd length: " << cur_scurve->boundary_plus_length() << " minus bnd length: ";
          vcl_cout << cur_scurve->boundary_minus_length() << " total length: " << cur_scurve->boundary_plus_length() + cur_scurve->boundary_minus_length() << vcl_endl;         
          output_scurve();
        }
      }
    }
    
    tableau()->post_overlay_redraw();
    return true;
  }
  else if (add_edges(e)){

    if (current_){
      current_->getInfo();

      dbsk2d_shock_edge_sptr sel_edge = get_selected_edge();
      if (sel_edge && selected_edges.size()>0){
        selected_edges.push_back(sel_edge);
        //pick the right starting node
        if (selected_edges[0]->source() == selected_edges[1]->source() ||
            selected_edges[0]->source() == selected_edges[1]->target())
          start_node = selected_edges.front()->target();
        else
          start_node = selected_edges.front()->source();

        //construct the scurve from these edges
        cur_scurve = get_scurve();
        if (cur_scurve) {
          vcl_cout << "splice cost = " << cur_scurve->splice_cost_new(R_, construct_circular_ends_, find_splice_cost_using_dpmatch_combined_) << vcl_endl;
          output_scurve();
          vcl_cout << "\t plus bnd length: " << cur_scurve->boundary_plus_length() << " minus bnd length: ";
          vcl_cout << cur_scurve->boundary_minus_length() << " total length: " << cur_scurve->boundary_plus_length() + cur_scurve->boundary_minus_length() << vcl_endl;
        }
      }
    }
    
    tableau()->post_overlay_redraw();
    return true;
  }
  else if (reverse_sampling_cmd(e)){
    //if single edge change the start node
    if (selected_edges.size()==1){
      if (start_node == selected_edges[0]->source())
        start_node = selected_edges[0]->target();
      else
        start_node = selected_edges[0]->source();

      //construct the scurve from these edges
      cur_scurve = get_scurve();
      if (cur_scurve) {
        vcl_cout << "splice cost = " << cur_scurve->splice_cost_new(R_, construct_circular_ends_, find_splice_cost_using_dpmatch_combined_) << vcl_endl;
        vcl_cout << "\t plus bnd length: " << cur_scurve->boundary_plus_length() << " minus bnd length: ";
        vcl_cout << cur_scurve->boundary_minus_length() << " total length: " << cur_scurve->boundary_plus_length() + cur_scurve->boundary_minus_length() << vcl_endl;
        output_scurve();
      }
    }
    else if (selected_edges.size()>0){ 
      //reverse the order of the edges in the selection 
      //and choose a new start node
      vcl_reverse (selected_edges.begin(), selected_edges.end());

      //pick the right starting node
      if (selected_edges[0]->source() == selected_edges[1]->source() ||
          selected_edges[0]->source() == selected_edges[1]->target())
        start_node = selected_edges.front()->target();
      else
        start_node = selected_edges.front()->source();

      //construct the scurve from these edges
      cur_scurve = get_scurve();
      if (cur_scurve) {
        vcl_cout << "splice cost = " << cur_scurve->splice_cost_new(R_, construct_circular_ends_, find_splice_cost_using_dpmatch_combined_) << vcl_endl;
        vcl_cout << "\t plus bnd length: " << cur_scurve->boundary_plus_length() << " minus bnd length: ";
        vcl_cout << cur_scurve->boundary_minus_length() << " total length: " << cur_scurve->boundary_plus_length() + cur_scurve->boundary_minus_length() << vcl_endl;
        output_scurve();
      }
    }

    tableau()->post_overlay_redraw();
    return true;
  }

  if( e.type == vgui_OVERLAY_DRAW ) {
     draw_scurves();

     if (selected_dart_ >= 0) {
      glBegin( GL_POINTS );
      //glPointSize( 20.0 );
      glColor3f( 0 , 1 , 0 );
      glVertex2f( selected_dart_pt_.x(), selected_dart_pt_.y() );
      glColor3f( 0.1f , 0.1f , 0.1f );
      glVertex2f( selected_dart_end_.x(), selected_dart_end_.y() );
      glEnd();
    }

    draw_spliced_shock_branches();

  }

  return dbsk2d_ishock_highlight_tool::handle(e, view);
}

void dbskr_debug_scurve_tool::output_scurve()
{
  if (cur_scurve && output_selected_scurve_info_){
    dbskr_scurve_sptr orig_sc = cur_scurve->get_original_scurve(construct_circular_ends_);
    orig_sc->writeData("sc1.txt");
    dbskr_scurve_sptr pruned_sc = cur_scurve->get_replacement_scurve(orig_sc->num_points());
    pruned_sc->writeData("sc2.txt");
    //compute the alignment
    dbskr_dpmatch prune_alignment(orig_sc, pruned_sc);
    prune_alignment.Match();
    //vcl_cout << "recomputed cost: " << prune_alignment.recompute_final_cost() << vcl_endl;
    
    //output the alignment mapping
    vcl_ofstream outfp("sc1-sc2-alignment.txt", vcl_ios::out);
    for (unsigned int j = 0; j<prune_alignment.finalMap()->size(); j++) {
      int k = (*prune_alignment.finalMap())[j].first;
      int m = (*prune_alignment.finalMap())[j].second;
      outfp << k << " " << m << vcl_endl;
    }
    outfp.close();
  }
}

dbsk2d_shock_edge_sptr 
dbskr_debug_scurve_tool::get_selected_edge()
{
  return dynamic_cast<dbsk2d_shock_edge*>(current_);
}

dbskr_scurve_sptr 
dbskr_debug_scurve_tool::get_scurve()
{
  //determine if this shock path ends in a leaf edge
  bool leaf_edge = (selected_edges.back()->source()->degree()==1 ||
                    selected_edges.back()->target()->degree()==1);

  //get the shock curve for the currently selected path 
  if (selected_edges.size()>0) {
    
    if (construct_circular_ends_)
      return dbskr_compute_scurve(start_node, selected_edges, leaf_edge, 
                                  binterpolate_, subsample_, 
                                  interpolate_ds_, subsample_ds_);
    
    else
      return dbskr_compute_scurve(start_node, selected_edges, false, 
                                  binterpolate_, subsample_, 
                                  interpolate_ds_, subsample_ds_);
  }
  else
    return 0;
}

void dbskr_debug_scurve_tool::draw_scurves()
{
  if (cur_scurve){
    if (display_spliced_contours_){
      //get the new scurves from the current scurve that includes the closed off end
      dbskr_scurve_sptr old_scurve = cur_scurve->get_original_scurve(construct_circular_ends_);
      draw_an_scurve(old_scurve, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
     
      //get the replacement scurve
      dbskr_scurve_sptr rep_scurve = cur_scurve->get_replacement_scurve(old_scurve->num_points());
      draw_an_scurve(rep_scurve, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
    }
    else {
      draw_an_scurve(cur_scurve, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    }
    
    //also draw the start node of this grouping
    glColor3f( 1.0f , 0.0f , 1.0f );
    glPointSize( 5.0 );
    glBegin( GL_POINTS );
    glVertex2f(cur_scurve->sh_pt_x(0), cur_scurve->sh_pt_y(0)); 
    glEnd();

  }
}

void dbskr_debug_scurve_tool::draw_spliced_shock_branches()
{
  vcl_map< vcl_pair<double, unsigned>, bool>::iterator iter;
  for (iter = dart_splice_map_.begin(); iter != dart_splice_map_.end(); iter++) {
    if (iter->second) continue;
    int dart = (iter->first).second;
    if ((tree_->leaf(dart) || tree_->leaf(tree_->mate(dart))) && tree_->parent_dart(dart) != dart) 
      continue;
    dbskr_scurve_sptr cur_scurve = tree_->get_curve(dart, dart, construct_circular_ends_);
    glColor3f( 0.01f, 0.01f, 0.01f );
    glLineWidth (3.0);
    glBegin( GL_LINE_STRIP );
    for( int j = 0 ; j < cur_scurve->num_points() ; j++ ) {
      glVertex2f( cur_scurve->sh_pt(j).x(), cur_scurve->sh_pt(j).y() );
    }
    glEnd();
  }
}

void dbskr_debug_scurve_tool::draw_an_scurve(dbskr_scurve_sptr cur_scurve, 
                                             float rp, float gp, float bp, 
                                             float rm, float gm, float bm)
{
  // draw plus and minus side of the scurve
  glColor3f( rp, gp, bp );
  glLineWidth (2.0);
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
}

void 
dbskr_debug_scurve_tool::get_popup( const vgui_popup_params& params, 
                                          vgui_menu &menu )
{
  vcl_string on = "[x] ", off = "[ ] ";

  menu.add( ((display_spliced_contours_)?on:off)+"Show Spliced Contours ", 
            bvis1_tool_toggle, (void*)(&display_spliced_contours_) );

  menu.add( "Set interpolate ds", 
            bvis1_tool_set_param, (void*)(&interpolate_ds_) );

  menu.add( "Set subsample ds", 
            bvis1_tool_set_param, (void*)(&subsample_ds_) );

  menu.add( "Set splice threshold", 
            bvis1_tool_set_param, (void*)(&threshold_) );

  menu.add( "R", 
            bvis1_tool_set_param, (void*)(&R_) );

  menu.add( ((binterpolate_)?on:off)+"Interpolate Scurve ", 
            bvis1_tool_toggle, (void*)(&binterpolate_) );

  menu.add( ((subsample_)?on:off)+"Subsample Scurve ", 
            bvis1_tool_toggle, (void*)(&subsample_) );
  
  menu.add( ((output_selected_scurve_info_)?on:off)+ "Save selected scurves...", 
            bvis1_tool_toggle, (void*)(&output_selected_scurve_info_) );

  menu.add( ((construct_circular_ends_)?on:off)+"Construct normal circular ends?", 
            bvis1_tool_toggle, (void*)(&construct_circular_ends_ ));

  menu.add( ((find_splice_cost_using_dpmatch_combined_)?on:off)+"Find splice cost using dpmatch combined?", 
            bvis1_tool_toggle, (void*)(&find_splice_cost_using_dpmatch_combined_ ));
  
  menu.add( ((elastic_splice_cost_)?on:off)+"Find elastic splice costs for tree leaves?", 
            bvis1_tool_toggle, (void*)(&elastic_splice_cost_ ));
}
