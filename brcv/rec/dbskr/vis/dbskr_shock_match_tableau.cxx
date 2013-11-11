// This is brcv/rec/dbskr/vis/dbskr_shock_match_tableau.cxx

//:
// \file

#include <float.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <bvis1/bvis1_manager.h>

#include "dbskr_shock_match_tableau.h"

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_node.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_sample.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>

#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <dbskr/dbskr_dpmatch_combined.h>

#include <dbskr/dbskr_sm_cor.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_localize_match.h>

#define DIST_THRESHOLD  1

//: Constructor
dbskr_shock_match_tableau::dbskr_shock_match_tableau() : sm_cor_(0)
{
  gl_mode = GL_RENDER;

  //fill in the randomized color table
  for (int i=0; i<100; i++){
    for (int j=0; j<3;j++)
      rnd_colormap[i][j] = (rand() % 256)/256.0;
  }

  //: assign black as last color
  rnd_colormap[100][0] = 0.0f;
  rnd_colormap[100][1] = 0.0f;
  rnd_colormap[100][2] = 0.0f;
  //: assign blue as well 
  rnd_colormap[101][0] = 0.0f;
  rnd_colormap[101][1] = 0.0f;
  rnd_colormap[101][2] = 1.0f;
  select_color_ = 100;
  selected_curve1_ = 0;
  selected_curve2_ = 0;
  selected_dart_ = -1;

  //fill in the randomized color table2
  for (int i=0; i<5000; i++){
    for (int j=0; j<3;j++)
      rnd_colormap2[i][j] = (rand() % 256)/256.0;
  }

  //offsets
  offset_x1 = 0; offset_y1 = 0;
  offset_x2 = 300; offset_y2 = 0;

  //display flags
  display_corresponding_bnd_points_ = true;
  display_boundary_curves_ = true;
  display_shock_samples_ = false;
  display_spliced_edges = false;
  display_orig_shock_graphs_ = false;
  draw_shock_nodes_ = false;
  display_visual_frags_ = false;
  draw_fragments_with_alpha_ = false;
  display_intrinsic_coords_ = false;
  output_selected_scurve_info_ = false;

  sampling_ds_=static_cast<int>(1.0f);
  NR_=5;

  gesture_select = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture_splice1 = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  gesture_splice2 = vgui_event_condition(vgui_RIGHT, vgui_SHIFT, true);
  gesture_match_spliced_leaves = vgui_event_condition(vgui_MIDDLE, vgui_SHIFT, true);

  gesture_select_next = vgui_event_condition(vgui_CURSOR_RIGHT, vgui_MODIFIER_NULL, true);
  gesture_select_previous = vgui_event_condition(vgui_CURSOR_LEFT, vgui_MODIFIER_NULL, true);
  
}

void dbskr_shock_match_tableau::set_sm_cor(dbskr_sm_cor_sptr sm_cor)
{ 
  sm_cor_ = sm_cor; 
  dbsk2d_shock_graph_sptr shock1 = sm_cor_->get_tree1()->get_shock_graph();
  dbsk2d_shock_graph_sptr shock2 = sm_cor_->get_tree2()->get_shock_graph();

  dbsk2d_compute_bounding_box(shock1);
  dbsk2d_compute_bounding_box(shock2);

  vsol_box_2d_sptr b1 = shock1->get_bounding_box();
  vsol_box_2d_sptr b2 = shock2->get_bounding_box();

  // TODO: Avoid the margin addition by finding the bounding box 
  //       using plus and minus boundaries as well in dbsk2d_algo
  shock1_box_ = new vsol_box_2d;
  shock1_box_->add_point(b1->get_min_x()+offset_x1, b1->get_min_y()+offset_y1);
  shock1_box_->add_point(b1->get_max_x()+offset_x1, b1->get_max_y()+offset_y1);

  offset_x2 = shock1_box_->get_max_x()+10; 
  
  shock2_box_ = new vsol_box_2d;
  shock2_box_->add_point(b2->get_min_x()+offset_x2, b2->get_min_y()+offset_y2);
  shock2_box_->add_point(b2->get_max_x()+offset_x2, b2->get_max_y()+offset_y2);

  if (sm_cor_->edit_params_.combined_edit_) {
    selected_match_ = new dbskr_dpmatch_combined();
    selected_match_->set_R(sm_cor->edit_params_.curve_matching_R_);
  } else {
    selected_match_ = new dbskr_dpmatch();
    selected_match_->set_R(sm_cor->edit_params_.curve_matching_R_);
  }
}

bool dbskr_shock_match_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW )
      draw_render();

  if (gesture_select(e)) {
    if (!sm_cor_) {
      vcl_cout << "shock correspondence is not set!\n";
      return false;
    }

    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ixx_, iyy_);
    //vcl_cout << "ix: " << ix_ << " iy: " << iy_ << " ";
    mouse_ppt_ = vgl_point_2d<double>(ixx_, iyy_);
    selected_dart_ = -1;
    selected_curve1_ = 0;
    selected_curve2_ = 0;

    // check which bounding box contains the points
    bool first = false;
    if (shock1_box_->inside(ixx_, iyy_)) {
      vcl_cout << " in box1!\n";
      first = true;
    }
    else if (shock2_box_->inside(ixx_, iyy_))
      vcl_cout << " in box2!\n";
    else {
      vcl_cout << " outside boxes!\n";  
      return false;
    }

    // find the selected scurves in either of the shock graphs
    vcl_vector<dbskr_scurve_sptr>& curve_list1 = sm_cor_->get_curve_list1();
    vcl_vector<dbskr_scurve_sptr>& curve_list2 = sm_cor_->get_curve_list2();
    vcl_vector<vcl_vector < vcl_pair <int,int> > >& map_list = sm_cor_->get_map_list();
    vcl_vector<pathtable_key>& dart_path_map = sm_cor_->get_map();
    
    if (curve_list1.size() != curve_list2.size()) {
      vcl_cout << "different sizes in shock curve correspondence, not drawing selected curves!\n";
    } 
    else {  
      //int color = 0;
      //double min = 100000.0f;
      selected_i = -1;
      for (unsigned int i = 0; i<curve_list1.size(); i++) {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];

        // check the plus and minus boundary points
        vcl_vector< vcl_pair<int, int> > map = map_list[i];
        vgl_point_2d<double> mouse_pt1(ixx_-offset_x1, iyy_-offset_y1);
        vgl_point_2d<double> mouse_pt2(ixx_-offset_x2, iyy_-offset_y2);
        for (unsigned int j = 0; j<map.size(); j++) {
          int k = map[j].first;
          int m = map[j].second;
          vgl_point_2d<double> pt_p_1(sc1->bdry_plus_pt(k).x(), sc1->bdry_plus_pt(k).y());
          vgl_point_2d<double> pt_p_2(sc2->bdry_plus_pt(m).x(), sc2->bdry_plus_pt(m).y());

          vgl_point_2d<double> pt_m_1(sc1->bdry_minus_pt(k).x(), sc1->bdry_minus_pt(k).y());
          vgl_point_2d<double> pt_m_2(sc2->bdry_minus_pt(m).x(), sc2->bdry_minus_pt(m).y());

          vgl_point_2d<double> pt_s_1(sc1->sh_pt(k).x(), sc1->sh_pt(k).y());
          vgl_point_2d<double> pt_s_2(sc2->sh_pt(m).x(), sc2->sh_pt(m).y());

          if ((first && (vgl_distance(pt_p_1, mouse_pt1) < DIST_THRESHOLD ||
                         vgl_distance(pt_m_1, mouse_pt1) < DIST_THRESHOLD ||
                         vgl_distance(pt_s_1, mouse_pt1) < DIST_THRESHOLD) ) ||
              (!first && (vgl_distance(pt_p_2, mouse_pt2) < DIST_THRESHOLD ||
                          vgl_distance(pt_m_2, mouse_pt2) < DIST_THRESHOLD ||
                          vgl_distance(pt_s_2, mouse_pt2) < DIST_THRESHOLD)) )
          {
            /*selected_plus_pt1_ = pt_p_1;
            selected_plus_pt2_ = pt_p_2;

            selected_minus_pt1_ = pt_m_1;
            selected_minus_pt2_ = pt_m_2;

            selected_sh_pt1_ = pt_s_1;
            selected_sh_pt2_ = pt_s_2;
            */
            selected_i = i;
            /*selected_j = j;*/
          }
        }
      }
      if (selected_i >= 0) {
        if (curve_list1[selected_i] != selected_curve1_) {
          selected_curve1_ = curve_list1[selected_i];
          selected_curve2_ = curve_list2[selected_i];
          selected_match_->set_scurves(selected_curve1_, selected_curve2_);
          selected_match_->Match();
          vcl_vector< vcl_pair<int, int> > map = map_list[selected_i];  // this map and selected_match_ map are identically same
          vcl_vector< vcl_vector<double> >* DPCost = selected_match_->DPCost();
          selected_j = map.size()-1;
          int k = map[selected_j].first;
          int m = map[selected_j].second;
          vcl_cout << "(" << k << ", " << m << ") " << (*DPCost)[k][m] << " ";
          //: set the points
          vgl_point_2d<double> pt_p_1(selected_curve1_->bdry_plus_pt(k).x(), selected_curve1_->bdry_plus_pt(k).y());
          vgl_point_2d<double> pt_p_2(selected_curve2_->bdry_plus_pt(m).x(), selected_curve2_->bdry_plus_pt(m).y());

          vgl_point_2d<double> pt_m_1(selected_curve1_->bdry_minus_pt(k).x(), selected_curve1_->bdry_minus_pt(k).y());
          vgl_point_2d<double> pt_m_2(selected_curve2_->bdry_minus_pt(m).x(), selected_curve2_->bdry_minus_pt(m).y());

          vgl_point_2d<double> pt_s_1(selected_curve1_->sh_pt(k).x(), selected_curve1_->sh_pt(k).y());
          vgl_point_2d<double> pt_s_2(selected_curve2_->sh_pt(m).x(), selected_curve2_->sh_pt(m).y());

          selected_plus_pt1_ = pt_p_1;
          selected_plus_pt2_ = pt_p_2;

          selected_minus_pt1_ = pt_m_1;
          selected_minus_pt2_ = pt_m_2;

          selected_sh_pt1_ = pt_s_1;
          selected_sh_pt2_ = pt_s_2;
        }

        pathtable_key key = dart_path_map[selected_i];
        vcl_vector<int> dart_list = sm_cor_->get_tree1()->get_dart_path(key.first.first, key.first.second);
        //vcl_cout << "selected i: " << selected_i << " curve1: " << selected_curve1_ << " selected curve2: " << selected_curve2_ << "\ncurve1 darts:\t";
        vcl_cout << "selected i: " << selected_i << " key: " << key.first.first << " " << key.first.second << " " << key.second.first << " " << key.second.second << "\ncurve1 darts:\t";
        for (unsigned k = 0; k < dart_list.size(); k++)
          vcl_cout << dart_list[k] << " ";
        vcl_cout << "\ncurve2 darts:\t";
        dart_list = sm_cor_->get_tree2()->get_dart_path(key.second.first, key.second.second);
        for (unsigned k = 0; k < dart_list.size(); k++)
          vcl_cout << dart_list[k] << " ";
        vcl_cout << "\n";

        //vcl_cout << "dart map key: " << dart_path_map[selected_i];
        
        //output the shock curves on to text files for debug in MATLAB
        if (output_selected_scurve_info_){
          selected_curve1_->writeData("sc1.txt");
          selected_curve2_->writeData("sc2.txt");
          write_shock_alignment(map_list[selected_i], "sc1-sc2-alignment.txt");
        }

        float cost_d = sm_cor_->get_cost_d(selected_i);
        if (cost_d > 0)
          vcl_cout << " dpmatch cost for this pair: " << cost_d << vcl_endl;
        float cost = sm_cor_->get_cost(selected_i);
        if (sm_cor_->edit_params_.localized_edit_) {
          float cost_l = sm_cor_->get_cost_localized(selected_i);
          vcl_cout << " localized cost + init_dr + init_alp (used in edit distance): " << cost << vcl_endl;
        } else {
          if (cost > 0)
            vcl_cout << " dpmatch cost + init_dr + init_alp (used in edit distance): " << cost << vcl_endl;
        }
        float cost_init_dr = sm_cor_->get_cost_init_dr(selected_i);
        //if (cost_init_dr > 0)
          vcl_cout << " initial radius cost for this pair: " << cost_init_dr << vcl_endl;
        float cost_init_alp = sm_cor_->get_cost_init_alp(selected_i);
        //if (cost_init_alp > 0)
          vcl_cout << " initial alpha cost for this pair: " << cost_init_alp << "\n";
        vcl_cout << "\t\tsc1 + length: " << selected_curve1_->boundary_plus_length() << " - length: " << selected_curve1_->boundary_minus_length() << vcl_endl;
        vcl_cout << "\t\tsc2 + length: " << selected_curve2_->boundary_plus_length() << " - length: " << selected_curve2_->boundary_minus_length() << vcl_endl;
      }
    }
    bvis1_manager::instance()->post_redraw();
  }

  if (gesture_select_next(e) && selected_i >= 0 && selected_j >= 0 && selected_curve1_ && selected_curve2_) {
    vcl_vector< vcl_pair<int, int> >* map = selected_match_->finalMap();
    vcl_vector< vcl_vector<double> >* DPCost = selected_match_->DPCost();

    if (selected_j - 1 < 0)
      selected_j = map->size()-1;
    else
      selected_j = selected_j - 1;

    int k = (*map)[selected_j].first;
    int m = (*map)[selected_j].second;
    vcl_cout.precision(2);
    vcl_cout << "(" << k << ", " << m << ") " << (*DPCost)[k][m] << " ";
    if (selected_j < int(map->size()-1)) {
      int kp = (*map)[selected_j+1].first;
      int mp = (*map)[selected_j+1].second;
      vcl_cout << "interval:\n";
      selected_match_->computeIntervalCostPrint(k, kp, m, mp);
    }

    //: set the points
    vgl_point_2d<double> pt_p_1(selected_curve1_->bdry_plus_pt(k).x(), selected_curve1_->bdry_plus_pt(k).y());
    vgl_point_2d<double> pt_p_2(selected_curve2_->bdry_plus_pt(m).x(), selected_curve2_->bdry_plus_pt(m).y());

    vgl_point_2d<double> pt_m_1(selected_curve1_->bdry_minus_pt(k).x(), selected_curve1_->bdry_minus_pt(k).y());
    vgl_point_2d<double> pt_m_2(selected_curve2_->bdry_minus_pt(m).x(), selected_curve2_->bdry_minus_pt(m).y());

    vgl_point_2d<double> pt_s_1(selected_curve1_->sh_pt(k).x(), selected_curve1_->sh_pt(k).y());
    vgl_point_2d<double> pt_s_2(selected_curve2_->sh_pt(m).x(), selected_curve2_->sh_pt(m).y());

    selected_plus_pt1_ = pt_p_1;
    selected_plus_pt2_ = pt_p_2;

    selected_minus_pt1_ = pt_m_1;
    selected_minus_pt2_ = pt_m_2;

    selected_sh_pt1_ = pt_s_1;
    selected_sh_pt2_ = pt_s_2;

    bvis1_manager::instance()->post_redraw();
  }

  if (gesture_select_previous(e) && selected_i >= 0 && selected_j >= 0 && selected_curve1_ && selected_curve2_) {
    vcl_vector< vcl_pair<int, int> >* map = selected_match_->finalMap();
    vcl_vector< vcl_vector<double> >* DPCost = selected_match_->DPCost();
    selected_j = (selected_j + 1)%(map->size());
    
    int k = (*map)[selected_j].first;
    int m = (*map)[selected_j].second;
    vcl_cout.precision(2);
    vcl_cout << "(" << k << ", " << m << ") cost: " << (*DPCost)[k][m] << " ";
    if (selected_j < int(map->size()-1)) {
      int kp = (*map)[selected_j+1].first;
      int mp = (*map)[selected_j+1].second;
      vcl_cout << "interval:\n";
      selected_match_->computeIntervalCostPrint(k, kp, m, mp);
    }

    //: set the points
    vgl_point_2d<double> pt_p_1(selected_curve1_->bdry_plus_pt(k).x(), selected_curve1_->bdry_plus_pt(k).y());
    vgl_point_2d<double> pt_p_2(selected_curve2_->bdry_plus_pt(m).x(), selected_curve2_->bdry_plus_pt(m).y());

    vgl_point_2d<double> pt_m_1(selected_curve1_->bdry_minus_pt(k).x(), selected_curve1_->bdry_minus_pt(k).y());
    vgl_point_2d<double> pt_m_2(selected_curve2_->bdry_minus_pt(m).x(), selected_curve2_->bdry_minus_pt(m).y());

    vgl_point_2d<double> pt_s_1(selected_curve1_->sh_pt(k).x(), selected_curve1_->sh_pt(k).y());
    vgl_point_2d<double> pt_s_2(selected_curve2_->sh_pt(m).x(), selected_curve2_->sh_pt(m).y());

    selected_plus_pt1_ = pt_p_1;
    selected_plus_pt2_ = pt_p_2;

    selected_minus_pt1_ = pt_m_1;
    selected_minus_pt2_ = pt_m_2;

    selected_sh_pt1_ = pt_s_1;
    selected_sh_pt2_ = pt_s_2;

    bvis1_manager::instance()->post_redraw();
  }

  if (gesture_splice1(e)) {
    if (!sm_cor_) {
      vcl_cout << "shock correspondence is not set!\n";
      return false;
    }

    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ixx_, iyy_);
    //vcl_cout << "ix: " << ix_ << " iy: " << iy_ << " ";
    mouse_ppt_ = vgl_point_2d<double>(ixx_, iyy_);

    // check which bounding box contains the points
    bool first = false;
    if (shock1_box_->inside(ixx_, iyy_)) {
      vcl_cout << " in box1!\n";
      first = true;
    }
    else if (shock2_box_->inside(ixx_, iyy_))
      vcl_cout << " in box2!\n";
    else {
      vcl_cout << " outside boxes!\n";
      //selected_curve1_ = 0;
      //selected_curve2_ = 0;
      selected_dart_ = -1;
      return false;
    }

    // go over each dart in the selected tree to display splice cost of it
    dbskr_tree_sptr tree; 
    vgl_point_2d<double> mouse_pt;
    bool first_tree;
    if (first) {
      tree = sm_cor_->get_tree1();
      mouse_pt = vgl_point_2d<double>(ixx_-offset_x1, iyy_-offset_y1);
      first_tree = true;
    } else {
      tree = sm_cor_->get_tree2();
      mouse_pt = vgl_point_2d<double>(ixx_-offset_x2, iyy_-offset_y2);
      first_tree = false;
    }
    unsigned dart_cnt = tree->size();
    selected_dart_= -1;
    for (unsigned i = 0; i<dart_cnt; i++) {
      //if ((tree->leaf(i) || tree->leaf(tree->mate(i))) && tree->parent_dart(i) != static_cast<int>(i)) continue;
      if (!tree->leaf(i)) continue;
      dbskr_scurve_sptr sc = tree->get_curve(i, i, sm_cor_->edit_params_.circular_ends_);
      double min = 10000.0f;
      for (int j = 0; j< sc->num_points(); j++) {
        vgl_point_2d<double> pt_s(sc->sh_pt(j).x(), sc->sh_pt(j).y());
        double dist = vgl_distance(pt_s, mouse_pt);
        if (dist < min) {
          min = dist;
        }
      }
      if (min < DIST_THRESHOLD) {
        vgl_point_2d<double> offs;
        if (first) 
          offs = vgl_point_2d<double>(offset_x1, offset_y1);
        else
          offs = vgl_point_2d<double>(offset_x2, offset_y2);

        selected_dart_pt_ = vgl_point_2d<double> (sc->sh_pt(0).x()+offs.x(), sc->sh_pt(0).y()+offs.y());
        selected_dart_end_ = vgl_point_2d<double> (sc->sh_pt(sc->num_points()-1).x()+offs.x(), sc->sh_pt(sc->num_points()-1).y()+offs.y());
        selected_dart_ = i;
      }
    }
    if (selected_dart_ >= 0) {
      selected_dart_mate_ = tree->mate(selected_dart_);
      vcl_cout << "selected_dart: " << selected_dart_ << " splice cost: " << tree->delete_cost(selected_dart_) << " contract cost: " << tree->contract_cost(selected_dart_) << vcl_endl; 
      delete_cost_mate_ = tree->delete_cost(selected_dart_mate_);
      contract_cost_mate_ = tree->contract_cost(selected_dart_mate_);
      
      if (first_tree) {
        dbskr_sc_pair_sptr cp = tree->get_curve_pair(selected_dart_, selected_dart_, sm_cor_->edit_params_.circular_ends_);
        selected_curve1_ = cp->coarse;
        selected_curve1_pair_ = cp;
      } else {
        dbskr_sc_pair_sptr cp = tree->get_curve_pair(selected_dart_, selected_dart_, sm_cor_->edit_params_.circular_ends_);
        selected_curve2_ =  cp->coarse;
        selected_curve2_pair_ = cp;
      }
      //selected_curve2_ =  tree->get_curve(selected_dart_mate_, selected_dart_mate_, sm_cor_->contstruct_circular_ends_);

      //output the shock curves on to text files for debug in MATLAB
      if (output_selected_scurve_info_){
        dbskr_scurve_sptr orig_sc = selected_curve1_->get_original_scurve(sm_cor_->edit_params_.circular_ends_);
        orig_sc->writeData("sc1.txt");
        dbskr_scurve_sptr pruned_sc = selected_curve1_->get_replacement_scurve(orig_sc->num_points());
        pruned_sc->writeData("sc2.txt");
        //compute the alignment
        dbskr_dpmatch prune_alignment(orig_sc, pruned_sc);
        prune_alignment.set_R(sm_cor_->edit_params_.curve_matching_R_);
        prune_alignment.Match();
        write_shock_alignment(*prune_alignment.finalMap(), "sc1-sc2-alignment.txt");
      }
    }

    bvis1_manager::instance()->post_redraw();
    return false;
  }

  if (gesture_match_spliced_leaves(e)) {
    if (!selected_curve1_) 
      vcl_cout << "please select a spliced branch from the first tree!\n";
    else if (!selected_curve2_)
      vcl_cout << "please select a spliced branch from the second tree!\n";
    else {
      float init_dr, init_phi, coarse_match_cost, loc_match_cost = 0.0f;
      
      if (sm_cor_->edit_params_.combined_edit_) 
        selected_match_ = new dbskr_dpmatch_combined(selected_curve1_, selected_curve2_);
      else 
        selected_match_ = new dbskr_dpmatch(selected_curve1_, selected_curve2_);
    
      selected_match_->set_R(sm_cor_->edit_params_.curve_matching_R_);
      selected_match_->Match();
      init_dr = selected_match_->init_dr();
      init_phi = selected_match_->init_phi();
      coarse_match_cost = selected_match_->finalCost();
      vcl_vector<vcl_pair<int,int> > fmap = *selected_match_->finalMap();

      if (sm_cor_->edit_params_.localized_edit_ && selected_curve2_pair_ && selected_curve1_pair_) {  
        dbskr_localize_match lmatch(selected_curve1_, selected_curve2_, 
                                    selected_curve1_pair_->dense, selected_curve2_pair_->dense, 
                                    selected_curve1_pair_->c_d_map, selected_curve2_pair_->c_d_map, 
                                    fmap, (float)sm_cor_->edit_params_.curve_matching_R_);
        lmatch.match();
        loc_match_cost = lmatch.finalCost() + init_dr + init_phi;
        vcl_cout << "localized match cost: " << loc_match_cost << " total cost: " << loc_match_cost + init_dr + init_phi << "\n";
      }

      vcl_cout << "dpmatch cost for this pair: " << coarse_match_cost << vcl_endl;
      if (sm_cor_->edit_params_.localized_edit_ && selected_curve2_pair_ && selected_curve1_pair_) {
        vcl_cout << "localized cost + init_dr + init_alp (used in edit distance): " << loc_match_cost + init_dr + init_phi << vcl_endl;
      }
      vcl_cout << "initial radius cost for this pair: " << init_dr << vcl_endl;
      vcl_cout << "initial alpha cost for this pair: " << init_phi << vcl_endl;
      vcl_cout << "\t\tsc1 + length: " << selected_curve1_->boundary_plus_length() << " - length: " << selected_curve1_->boundary_minus_length();
      vcl_cout << " + nbr pts: " << selected_curve1_->bdry_plus().size() << " - nbr pts: " << selected_curve1_->bdry_minus().size() << vcl_endl;
      vcl_cout << "\t\tsc2 + length: " << selected_curve2_->boundary_plus_length() << " - length: " << selected_curve2_->boundary_minus_length();
      vcl_cout << " + nbr pts: " << selected_curve2_->bdry_plus().size() << " - nbr pts: " << selected_curve2_->bdry_minus().size() << vcl_endl;

      if (sm_cor_->edit_params_.localized_edit_ && selected_curve2_pair_ && selected_curve1_pair_) {
        vcl_cout << "\t\tsc1 dense + length: " << selected_curve1_pair_->dense->boundary_plus_length() << " - length: " << selected_curve1_pair_->dense->boundary_minus_length();
        vcl_cout << " + nbr pts: " << selected_curve1_pair_->dense->bdry_plus().size() << " - nbr pts: " << selected_curve1_pair_->dense->bdry_minus().size() << vcl_endl;
        vcl_cout << "\t\tsc2 + length: " << selected_curve2_pair_->dense->boundary_plus_length() << " - length: " << selected_curve2_pair_->dense->boundary_minus_length();
        vcl_cout << " + nbr pts: " << selected_curve2_pair_->dense->bdry_plus().size() << " - nbr pts: " << selected_curve2_pair_->dense->bdry_minus().size() << vcl_endl;
      }
    }
  }

  if (gesture_splice2(e) && selected_dart_ >= 0) {
    selected_dart_pt_ = selected_dart_end_;
    vcl_cout << "selected_mate: " << selected_dart_mate_ << " splice cost: " << delete_cost_mate_ << " contract cost: " << contract_cost_mate_ << vcl_endl; 

    //output the shock curves on to text files for debug in MATLAB
    if (output_selected_scurve_info_){
      dbskr_scurve_sptr orig_sc = selected_curve2_->get_original_scurve(sm_cor_->edit_params_.circular_ends_);
      orig_sc->writeData("sc1.txt");
      dbskr_scurve_sptr pruned_sc = selected_curve2_->get_replacement_scurve(orig_sc->num_points());
      pruned_sc->writeData("sc2.txt");
      //compute the alignment
      dbskr_dpmatch prune_alignment(orig_sc, pruned_sc);
      prune_alignment.set_R(sm_cor_->edit_params_.curve_matching_R_);
      prune_alignment.Match();
      write_shock_alignment(*prune_alignment.finalMap(), "sc1-sc2-alignment.txt");
    }

    bvis1_manager::instance()->post_redraw();
    return false;
  }

  return false;
}

void dbskr_shock_match_tableau::write_shock_alignment(vcl_vector<vcl_pair<int,int> > align, vcl_string fname)
{
  //output the alignment mapping
  vcl_ofstream outfp(fname.c_str(), vcl_ios::out);

  for (unsigned int j = 0; j<align.size(); j++) {
    int k = align[j].first;
    int m = align[j].second;

    outfp << k << " " << m << vcl_endl;
  }

  outfp.close();
}

void dbskr_shock_match_tableau::draw_render()
{
  if (!sm_cor_) {
    vcl_cout << "shock correspondence is not set!\n";
    return;
  }

  if (display_orig_shock_graphs_)
    draw_original_shock_graphs();
  else
    draw_matched_shock_graphs();

  if (display_corresponding_bnd_points_)
  {
    vcl_vector<dbskr_scurve_sptr>& curve_list1 = sm_cor_->get_curve_list1();
    vcl_vector<dbskr_scurve_sptr>& curve_list2 = sm_cor_->get_curve_list2();
    vcl_vector<vcl_vector < vcl_pair <int,int> > >& map_list = sm_cor_->get_map_list();

    //// draw corresponding points on the boundary curves in changing color
    if ((curve_list1.size() != curve_list2.size()) || (curve_list1.size() != map_list.size())) {
      vcl_cout << "different sizes in shock curve correspondence, not drawing boundary curves!\n";
    } 
    else {  // draw corresponding shock curves
      int color = 0;
      for (unsigned int i = 0; i<curve_list1.size(); i++) {
        dbskr_scurve_sptr sc1 = curve_list1[i];
        dbskr_scurve_sptr sc2 = curve_list2[i];
        vcl_vector< vcl_pair<int, int> > map = map_list[i];

        //vcl_cout << "curve1 size: " << sc1->num_points() << " curve2 size: " << sc2->num_points() << " drawing " << map.size() << " pair of points\n";

        if (selected_curve1_ == sc1 && selected_curve2_ == sc2) {
        for (unsigned int j = 0; j<map.size(); j++) {
          int k = map[j].first;
          int m = map[j].second;

          glColor3f( rnd_colormap2[color][0] , rnd_colormap2[color][1] , rnd_colormap2[color][2] );
          glPointSize( 20.0 );
          glBegin( GL_POINTS );
          glVertex2f( sc1->bdry_plus_pt(k).x() + offset_x1, sc1->bdry_plus_pt(k).y() + offset_y1 );
          glVertex2f( sc1->bdry_minus_pt(k).x() + offset_x1, sc1->bdry_minus_pt(k).y() + offset_y1 );
          
          glVertex2f( sc2->bdry_plus_pt(m).x() + offset_x2, sc2->bdry_plus_pt(m).y() + offset_y2 );
          glVertex2f( sc2->bdry_minus_pt(m).x() + offset_x2, sc2->bdry_minus_pt(m).y() + offset_y2 );
          glEnd();

          color = (color+1) % 5000;
        }
        
        }
      }
    }
  }

  //display selected points and curves
  if (selected_curve1_ || selected_curve2_) {
    glColor3f( 0 , 0 , 0 );
    glPointSize( 30.0 );
    glBegin( GL_POINTS );
    glVertex2f( selected_plus_pt1_.x() + offset_x1, selected_plus_pt1_.y() + offset_y1 );
    glVertex2f( selected_plus_pt2_.x() + offset_x2, selected_plus_pt2_.y() + offset_y2 );        
    glVertex2f( selected_minus_pt1_.x() + offset_x1, selected_minus_pt1_.y() + offset_y1 );
    glVertex2f( selected_minus_pt2_.x() + offset_x2, selected_minus_pt2_.y() + offset_y2 );
    glVertex2f( selected_sh_pt1_.x() + offset_x1, selected_sh_pt1_.y() + offset_y1 );
    glVertex2f( selected_sh_pt2_.x() + offset_x2, selected_sh_pt2_.y() + offset_y2 );
    glColor3f( 1 , 0 , 0 );
    glVertex2f( mouse_ppt_.x(), mouse_ppt_.y() );
    glEnd();

    glLineWidth( 5.0 );
    glColor3f( 0 , 0 , 0 );
    glBegin( GL_LINE_STRIP);
      glVertex2f( selected_plus_pt1_.x() + offset_x1, selected_plus_pt1_.y() + offset_y1 );
      glVertex2f( selected_sh_pt1_.x() + offset_x1, selected_sh_pt1_.y() + offset_y1 );
      glVertex2f( selected_minus_pt1_.x() + offset_x1, selected_minus_pt1_.y() + offset_y1 );
    glEnd();

    glLineWidth( 5.0 );
    glColor3f( 0 , 0 , 0 );
    glBegin( GL_LINE_STRIP);
      glVertex2f( selected_plus_pt2_.x() + offset_x2, selected_plus_pt2_.y() + offset_y2 );
      glVertex2f( selected_sh_pt2_.x() + offset_x2, selected_sh_pt2_.y() + offset_y2 );
      glVertex2f( selected_minus_pt2_.x() + offset_x2, selected_minus_pt2_.y() + offset_y2 );
    glEnd();
  }

  if (selected_dart_ >= 0) {
    glColor3f( 1 , 0 , 0 );
    glBegin( GL_POINTS );
    glVertex2f( mouse_ppt_.x(), mouse_ppt_.y() );
    glColor3f( 0 , 1 , 0 );
    glVertex2f( selected_dart_pt_.x(), selected_dart_pt_.y() );
    glEnd();
  }

  if (selected_curve1_)
    draw_boundary_curves(selected_curve1_, select_color_, offset_x1, offset_y1);
  if (selected_curve2_)
    draw_boundary_curves(selected_curve2_, select_color_, offset_x2, offset_y2);

  //display shock bounding boxes
  //draw_box(shock1_box_);
  //draw_box(shock2_box_);
}

void dbskr_shock_match_tableau::draw_original_shock_graphs()
{
  //get the trees and shock graphs involved
  dbskr_tree_sptr tree1 = sm_cor_->get_tree1();
  dbskr_tree_sptr tree2 = sm_cor_->get_tree2(); 
  dbsk2d_shock_graph_sptr sg1 = tree1->get_shock_graph();
  dbsk2d_shock_graph_sptr sg2 = tree2->get_shock_graph();

  //temp data structures
  dbsk2d_shock_node_sptr start_node;
  vcl_vector<dbsk2d_shock_edge_sptr> edges;
  vcl_vector<int> dart_list;

  //---------------
  // Shock graph 1
  //---------------

  vcl_map<int, int> edge_color_map1;
 
  // iterate over the darts of this tree to draw everything
  for (int i=0; i < tree1->size(); i++)
  {
    //if leaf edge only draw leaf dart
    if ((tree1->leaf(i) || tree1->leaf(tree1->mate(i))) && tree1->parent_dart(i)!=i)
      continue;

    if (tree1->mate(i)<i)
      continue; //already drawn from the other side

    int color = i%100;

    //get shock edge list from this path
    dart_list.clear();
    dart_list.push_back(i);

    edges.clear();
    tree1->edge_list(dart_list, start_node, edges); 

    //assign the current color to all the shock edges in this path
    for (unsigned int j = 0; j<edges.size(); j++) 
      edge_color_map1[edges[j]->id()] = color;

    // get the shock curve corresponding to this dart
    dbskr_scurve_sptr sc1 = dbskr_compute_scurve(start_node, edges, tree1->leaf(i), true, true, 1.0, sampling_ds_);

    //draw the boundary curve
    draw_boundary_curves(sc1, color, offset_x1, offset_y1);

    // draw the intrinsic coordinates
    if (display_intrinsic_coords_)
      draw_intrinsic_coords(sc1, color, offset_x1, offset_y1);

    //draw the visual fragments now
    if (display_visual_frags_)
      draw_visual_frags(sc1, color, offset_x1, offset_y1);
  }

  //---------------
  // Shock graph 2
  //---------------

  vcl_map<int, int> edge_color_map2;

  // iterate over the darts of this tree to draw everything
  for (int i=0; i<tree2->size(); i++)
  {
    //if leaf edge only draw leaf dart
    if ((tree2->leaf(i) || tree2->leaf(tree2->mate(i))) && tree2->parent_dart(i)!=i)
      continue;

    if (tree2->mate(i)<i)
      continue; //already drawn from the other side

    int color = i%100;

    //get shock edge list from this path
    dart_list.clear();
    dart_list.push_back(i);

    edges.clear();
    tree2->edge_list(dart_list, start_node, edges); 

    //assign the current color to all the shock edges in this path
    for (unsigned int j = 0; j<edges.size(); j++) 
      edge_color_map2[edges[j]->id()] = color;

    // get the shock curve corresponding to this dart
    dbskr_scurve_sptr sc2 = dbskr_compute_scurve(start_node, edges, tree2->leaf(i), true, true, 1.0, sampling_ds_);

    //draw the boundary curve
    draw_boundary_curves(sc2, color, offset_x2, offset_y2);

    // draw the intrinsic coordinates
    if (display_intrinsic_coords_)
      draw_intrinsic_coords(sc2, color, offset_x2, offset_y2);

    //draw the visual fragments now
    if (display_visual_frags_)
      draw_visual_frags(sc2, color, offset_x2, offset_y2);
  }
    
  //-------------------------------------
  // Finally draw the shock graphs on top
  //-------------------------------------

  // draw shock graph 1
  draw_shock_graph(sg1, edge_color_map1, offset_x1, offset_y1);

  // draw shock graph 2
  draw_shock_graph(sg2, edge_color_map2, offset_x2, offset_y2);

}

void dbskr_shock_match_tableau::draw_matched_shock_graphs()
{
  //get the trees and shock graphs involved
  dbskr_tree_sptr tree1 = sm_cor_->get_tree1();
  dbskr_tree_sptr tree2 = sm_cor_->get_tree2();
  dbsk2d_shock_graph_sptr sg1 = tree1->get_shock_graph();
  dbsk2d_shock_graph_sptr sg2 = tree2->get_shock_graph();

  if (sg1 == NULL || sg2 == NULL || tree1 == NULL || tree2 == NULL) {
    vcl_cout << "shock graphs or trees are not set!\n";
    return;
  }

  //get dart path mapping from the shock correspondence 
  vcl_vector<pathtable_key>& dart_path_map = sm_cor_->get_map();

  // assign unique colors to each corresponding path in each shock graph
  vcl_map<int, int> edge_color_map1, edge_color_map2;

  //temp data structures
  dbsk2d_shock_node_sptr start_node;
  vcl_vector<dbsk2d_shock_edge_sptr> edges;
  vcl_vector<int> dart_list;

  // go over all the corresponding paths and draw the corresponding 
  // boundary curves visual fragments and intrinsic fragement coordinates
  for (unsigned int i = 0; i<dart_path_map.size(); i++) 
  {
    int color = i%100; //pick a color for this path (may not be unique)

    //---------------
    // Shock graph 1
    //---------------

    //get dart path
    pathtable_key key = dart_path_map[i];
    dart_list.clear();
    dart_list = tree1->get_dart_path(key.first.first, key.first.second);
    
    //get shock edge list from this path
    edges.clear();
    tree1->edge_list(dart_list, start_node, edges); 

    //assign the current color to all the shock edges in this path
    for (unsigned int j = 0; j<edges.size(); j++) 
      edge_color_map1[edges[j]->id()] = color;

    dbskr_scurve_sptr sc1;
    if (sm_cor_->edit_params_.circular_ends_)
      sc1 = dbskr_compute_scurve(start_node, edges, tree1->leaf(dart_list.back()), true, true, vcl_min(tree1->scurve_sample_ds_, tree1->interpolate_ds_), tree1->scurve_sample_ds_);
    else
      sc1 = dbskr_compute_scurve(start_node, edges, false, true, true, vcl_min(tree1->scurve_sample_ds_, tree1->interpolate_ds_), tree1->scurve_sample_ds_);

    //vcl_cout << "in draw matched shocks, key first: " << key.first.first << ", " << key.first.second << " sc1 # pts: " << sc1->num_points() << " ";

    //---------------
    // Shock graph 2
    //---------------

    //get dart path
    dart_list.clear();
    dart_list = tree2->get_dart_path(key.second.first, key.second.second); 

    dbskr_scurve_sptr sc2 = 0;
    //get shock edge list from this path
    edges.clear();
    tree2->edge_list(dart_list, start_node, edges);

    //assign the current color to all the shock edges in this path
    for (unsigned int j = 0; j<edges.size(); j++) 
      edge_color_map2[edges[j]->id()] = color;

    if (sm_cor_->edit_params_.circular_ends_)
      sc2 = dbskr_compute_scurve(start_node, edges, tree2->leaf(dart_list.back()), true, true, vcl_min(tree2->scurve_sample_ds_, tree2->interpolate_ds_), tree2->scurve_sample_ds_);
    else
      sc2 = dbskr_compute_scurve(start_node, edges, false, true, true, vcl_min(tree2->scurve_sample_ds_, tree2->interpolate_ds_), tree2->scurve_sample_ds_);
    //vcl_cout << ", key second: " << key.second.first << ", " << key.second.second << " sc2 # pts: " << sc2->num_points() << "\n";
    
    //-----------------------
    // Now do all the drawing
    //-----------------------

    //draw the boundary curves
    if (display_boundary_curves_) {
      draw_boundary_curves(sc1, color, offset_x1, offset_y1);
      if (sc2)
        draw_boundary_curves(sc2, color, offset_x2, offset_y2);
    }

    // first draw the intrinsic coordinates
    if (display_intrinsic_coords_){
      draw_intrinsic_coords(sc1, color, offset_x1, offset_y1);
      if (sc2)
        draw_intrinsic_coords(sc2, color, offset_x2, offset_y2);
    }

    //draw the visual fragments now
    if (display_visual_frags_){
      draw_visual_frags(sc1, color, offset_x1, offset_y1);
      if (sc2)
        draw_visual_frags(sc2, color, offset_x2, offset_y2);
    }
  }

  //-------------------------------------
  // Finally draw the shock graphs on top
  //-------------------------------------

  // draw shock graph 1
  draw_shock_graph(sg1, edge_color_map1, offset_x1, offset_y1);

  // draw shock graph 2
  draw_shock_graph(sg2, edge_color_map2, offset_x2, offset_y2);

}

void dbskr_shock_match_tableau::
draw_boundary_curves(dbskr_scurve_sptr sk_path, int color, 
                     double off_x, double off_y)
{
  //draw the shock samples
  if (display_shock_samples_) {
    glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
    glLineWidth (1.0);
    for( int j = 0 ; j < sk_path->num_points() ; j++ ) {
      glBegin( GL_LINE_STRIP );
      glVertex2f( sk_path->bdry_plus_pt(j).x() + off_x, sk_path->bdry_plus_pt(j).y() + off_y );
      glVertex2f( sk_path->sh_pt(j).x() + off_x, sk_path->sh_pt(j).y() + off_y );
      glVertex2f( sk_path->bdry_minus_pt(j).x() + off_x, sk_path->bdry_minus_pt(j).y() + off_y );
      glEnd();
    }
  }

  // draw plus and minus boundary of the shapes
  glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
  glLineWidth (2.0);
  glBegin( GL_LINE_STRIP );
  for( int j = 0 ; j < sk_path->num_points() ; j++ ) {
    glVertex2f( sk_path->bdry_plus_pt(j).x() + off_x, sk_path->bdry_plus_pt(j).y() + off_y );
  }
  glEnd();

  glBegin( GL_LINE_STRIP );
  for( int j = 0 ; j < sk_path->num_points() ; j++ ) {
    glVertex2f( sk_path->bdry_minus_pt(j).x() + off_x, sk_path->bdry_minus_pt(j).y() + off_y );
  }
  glEnd();

}

void dbskr_shock_match_tableau::
draw_shock_graph(dbsk2d_shock_graph_sptr sg, vcl_map<int, int> &edge_color_map, 
                 double off_x, double off_y)
{
  // draw shock graph edges
  for ( dbsk2d_shock_graph::edge_iterator curE = sg->edges_begin();
        curE != sg->edges_end();
        curE++ ) 
  {
    dbsk2d_shock_edge_sptr selm = (*curE);

    //use the correspondence color scheme
    int color;
    vcl_map<int, int>::iterator iter = edge_color_map.find(selm->id());
    if (iter == edge_color_map.end() ){
      if (display_spliced_edges) 
        color = 100;                  // if no correspondence assign black
      else
        continue;                     // skip the spliced edges
    }
    else { 
      if (display_visual_frags_ || display_intrinsic_coords_) //draw in black when overlapping
        color = 100;
      else
        color = iter->second;           //assign the match color
    }

    //draw the edge
    glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
    if (display_spliced_edges)
      glLineWidth (2.0);
    else
      glLineWidth (5.0);
    glBegin( GL_LINE_STRIP );
    for( unsigned int i = 0 ; i < selm->ex_pts().size() ; i++ )
      glVertex2f( selm->ex_pts()[i].x() + off_x, selm->ex_pts()[i].y() + off_y );
    glEnd();
  }

  if (draw_shock_nodes_){
    // draw shock graph nodes
    for ( dbsk2d_shock_graph::vertex_iterator curN = sg->vertices_begin();
        curN != sg->vertices_end();
        curN++ ) 
    {
      dbsk2d_shock_node_sptr snode = (*curN);
      glColor3f( 1.0 , 1.0 , 1.0 );
      glPointSize( 5.0 );
      glBegin( GL_POINTS );
      glVertex2f(snode->ex_pts()[0].x() + off_x, snode->ex_pts()[0].y() + off_y); 
      glEnd();
    }
  }
}

float rescale(float x)
{
  return vcl_sqrt(2*x-x*x);
}

void dbskr_shock_match_tableau::
draw_visual_frags(dbskr_scurve_sptr sk_path, int color,
                  double off_x, double off_y)
{
  // display visual fragments corresponding to this shock edge
  if (draw_fragments_with_alpha_)
    glColor4f( rnd_colormap[color][0], rnd_colormap[color][1], rnd_colormap[color][2], 0.5);
  else
    glColor3f( rnd_colormap[color][0], rnd_colormap[color][1], rnd_colormap[color][2]);
  
  glBegin( GL_POLYGON );
  //first point on the shock
  vgl_point_2d<double> pts = sk_path->fragment_pt(0, 0);
  glVertex2f(pts.x()+ off_x, pts.y()+ off_y);

  //plus_curve
  for (int i=0; i<sk_path->num_points(); i++){
    double r_ip = sk_path->interp_radius(i);
    vgl_point_2d<double> pt = sk_path->fragment_pt(i, r_ip);
    glVertex2f(pt.x()+ off_x, pt.y()+ off_y); 
  }

  //last point on the shock
  vgl_point_2d<double> pte = sk_path->fragment_pt(sk_path->num_points()-1, 0);
  glVertex2f(pte.x()+ off_x, pte.y()+ off_y);

  //minus curve (backwards)
  glBegin( GL_LINE_STRIP );
  for (int i=sk_path->num_points()-1; i>=0; i--){
    double r_ip = sk_path->interp_radius(i, i+1, 1, 0);
    vgl_point_2d<double> pt = sk_path->fragment_pt(i, -r_ip);
    glVertex2f(pt.x()+ off_x, pt.y()+ off_y); 
  }
  glEnd();
  
}

void dbskr_shock_match_tableau::
draw_intrinsic_coords(dbskr_scurve_sptr sk_path, int color,
                      double off_x, double off_y)
{
  glColor3f( rnd_colormap[color][0], rnd_colormap[color][1], rnd_colormap[color][2] );
  glLineWidth (1.0);

  //draw the oars first
  for (int i=0; i<sk_path->num_points(); i++)
  {
    double r_ip = sk_path->interp_radius(i);
    vgl_point_2d<double> l1_spt = sk_path->fragment_pt(i, 0);
    vgl_point_2d<double> l1_ept = sk_path->fragment_pt(i, r_ip);
    vgl_point_2d<double> l2_spt = sk_path->fragment_pt(i, 0);
    vgl_point_2d<double> l2_ept = sk_path->fragment_pt(i, -r_ip);

    glBegin( GL_LINE_STRIP );
    glVertex2f(l1_spt.x()+ off_x, l1_spt.y()+ off_y); 
    glVertex2f(l1_ept.x()+ off_x, l1_ept.y()+ off_y);
    glEnd();

    glBegin( GL_LINE_STRIP );
    glVertex2f(l2_spt.x()+ off_x, l2_spt.y()+ off_y); 
    glVertex2f(l2_ept.x()+ off_x, l2_ept.y()+ off_y);
    glEnd();
  }

  //draw the parallel curves
  for (int r=0; r<=NR_; r++)
  {
    //plus_curve
    glBegin( GL_LINE_STRIP );
    for (int i=0; i<sk_path->num_points(); i++){
      double r_ip = sk_path->interp_radius(i);
      vgl_point_2d<double> pt = sk_path->fragment_pt(i, r_ip*r/NR_);
      glVertex2f(pt.x()+ off_x, pt.y()+ off_y); 
    }
    glEnd();

    //minus curve
    glBegin( GL_LINE_STRIP );
    for (int i=0; i<sk_path->num_points(); i++){
      double r_ip = sk_path->interp_radius(i, i+1, 1, 0);
      vgl_point_2d<double> pt = sk_path->fragment_pt(i, -r_ip*r/NR_);
      glVertex2f(pt.x()+ off_x, pt.y()+ off_y); 
    }
    glEnd();
  }

}

class dbskr_sm_tableau_toggle_command : public vgui_command
{
 public:
  dbskr_sm_tableau_toggle_command(dbskr_shock_match_tableau* tab, const void* boolref) : 
       match_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    match_tableau->post_redraw(); 
  }

  dbskr_shock_match_tableau *match_tableau;
  bool* bref;
};

class dbskr_sm_tableau_set_display_params_command : public vgui_command
{
 public:
  dbskr_sm_tableau_set_display_params_command(dbskr_shock_match_tableau* tab, 
    const vcl_string& name, const void* intref) : match_tableau(tab), iref_((int*)intref), name_(name) {}

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

  dbskr_shock_match_tableau *match_tableau;
  int* iref_;
  vcl_string name_;
};

void 
dbskr_shock_match_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( ((display_spliced_edges)?on:off)+"Show Spliced edges", 
               new dbskr_sm_tableau_toggle_command(this, &display_spliced_edges));

  submenu.add( ((display_visual_frags_)?on:off)+"Show Visual Fragments", 
               new dbskr_sm_tableau_toggle_command(this, &display_visual_frags_));

  submenu.add( ((draw_fragments_with_alpha_)?on:off)+"Draw Translucent Fragments", 
               new dbskr_sm_tableau_toggle_command(this, &draw_fragments_with_alpha_));

  submenu.add( ((display_intrinsic_coords_)?on:off)+"Show Intrinsic Coordinates", 
               new dbskr_sm_tableau_toggle_command(this, &display_intrinsic_coords_));

  submenu.add( ((display_orig_shock_graphs_)?on:off)+"Show Original Shock Graphs", 
               new dbskr_sm_tableau_toggle_command(this, &display_orig_shock_graphs_));

  submenu.add( ((draw_shock_nodes_)?on:off)+"Show Shock Nodes", 
               new dbskr_sm_tableau_toggle_command(this, &draw_shock_nodes_));

  submenu.add( ((display_corresponding_bnd_points_)?on:off)+"Show Bnd Pts", 
               new dbskr_sm_tableau_toggle_command(this, &display_corresponding_bnd_points_));

  submenu.add( ((display_boundary_curves_)?on:off)+"Show Bnd Curves", 
               new dbskr_sm_tableau_toggle_command(this, &display_boundary_curves_));

  submenu.add( ((display_shock_samples_)?on:off)+"Show Shock Samples", 
               new dbskr_sm_tableau_toggle_command(this, &display_shock_samples_));

  submenu.add( "Set sampling ds", 
               new dbskr_sm_tableau_set_display_params_command(this, "Sampling ds ", &sampling_ds_));

  submenu.add( "Set # parallel curves", 
    new dbskr_sm_tableau_set_display_params_command(this, "# of parallel curves ", &NR_));

  submenu.add( ((output_selected_scurve_info_)?on:off)+"Save selected Scurves", 
               new dbskr_sm_tableau_toggle_command(this, &output_selected_scurve_info_));

  //add this submenu to the popup menu
  menu.add("Matching Display Options", submenu);
}

//: if the distance of current selected point to this curve is less than 
//  a threshold than this one is selected 
//  there might be problems at the corners!!
double
dbskr_shock_match_tableau::selected(dbskr_scurve_sptr curve, double x, double y) 
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
dbskr_shock_match_tableau::draw_box(vsol_box_2d_sptr box)
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
