// This is brcv/rec/dbskr/vis/dbskr_shock_patch_match_tableau.cxx

//:
// \file

#include <float.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>

#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <bvis1/bvis1_manager.h>

#include "dbskr_shock_patch_match_tableau.h"

#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/algo/dbskr_rec_algs.h>

#include <dbcvr/dbcvr_cv_cor_sptr.h>
#include <dbcvr/dbcvr_cv_cor.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>

#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <bsol/bsol_algs.h>

//#include <dbil/algo/dbil_color_conversions.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>
//#include <vil/vil_image_resource.h>

//#define DIST_THRESHOLD  1

class dbskr_shock_patch_match_tableau_command : public vgui_command
{
 public:
  dbskr_shock_patch_match_tableau_command(dbskr_shock_patch_match_tableau* tab, dbskr_shock_patch_match_sptr match, int cmd): 
     match_(match), match_tableau(tab), cmd_(cmd) {}

  void execute() 
  { 
    switch (cmd_)
    {
      case 1:  //compute normalization based on reconstructed boundary length
      {
        if (!match_->compute_length_norm_costs_of_cors())
          vcl_cout << "problems in compute_length_norm_costs_of_cors()\n";
        break;
      }
      case 2:  // compute normalization based on total splice cost
      {
        if (!match_->compute_splice_norm_costs_of_cors())
          vcl_cout << "problems in compute_splice_norm_costs_of_cors()\n";
        break;
      }
      case 3: //resort with respect to normalized costs
      {
        match_->resort_wrt_norm_cost();
        break;
      }
      case 4: //resort with respect to costs
      { 
        match_->resort_wrt_cost();
        break;
      }
      case 5: // compute detection box
      {
        if (!match_->detect_instance(match_tableau->detection_box_, match_tableau->N_, match_tableau->k_, match_tableau->threshold_))
          vcl_cout << "problems in detection box computation\n";
        else {
          if (!match_tableau->detection_box_)
            vcl_cout << " No detection box with this threshold\n";
          else
            vcl_cout << "box: " << *(match_tableau->detection_box_) << vcl_endl;
        }
        break;
      }
      case 6: // clear detection box
      {
        match_tableau->detection_box_ = 0;
        break;
      }
      
      default:
        break;
    }
    match_tableau->post_redraw(); 
  }

  dbskr_shock_patch_match_sptr match_;
  dbskr_shock_patch_match_tableau *match_tableau;
  int cmd_;
};



//: Constructor
dbskr_shock_patch_match_tableau::dbskr_shock_patch_match_tableau() : dbskr_shock_match_tableau() 
{
  gl_mode = GL_RENDER;

  //display flags
  display_left_ = true;
  display_right_ = true;

  offset_x2 = 100; offset_y2 = 0; 

  //display flags
  display_outer_boundary_ = true;
  display_real_boundaries_ = true;
  display_traced_boudary_ = false;

  display_curve_match_ = false;

  display_shocks_ = false;
  display_corresponding_bnd_points_ = true;
  display_boundary_curves_ = true;
  display_shock_samples_ = false;
  display_spliced_edges = false;
  display_orig_shock_graphs_ = true;
  draw_shock_nodes_ = false;
  display_visual_frags_ = false;
  draw_fragments_with_alpha_ = false;
  display_intrinsic_coords_ = false;
  output_selected_scurve_info_ = false;

  gesture_select_point_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  next_patch_ = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
  next_match_ = vgui_event_condition(vgui_MIDDLE, vgui_SHIFT, true);
  gesture_create_match_storage_ = vgui_event_condition(vgui_key('i'), vgui_SHIFT, true);
  gesture_select_match_triplet_ = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);
  gesture_clear_match_triplets_ = vgui_event_condition(vgui_key('r'), vgui_MODIFIER_NULL, true);
  
  //best_select_point_ = vgui_event_condition(vgui_LEFT, vgui_CTRL, true);
  
  //create_storage_ = vgui_event_condition(vgui_MIDDLE, vgui_SHIFT, true);
  match_with_circular_completions_ = false;
  use_combined_edit_ = false;

  display_boxes_ = false;

  display_on_query_ = true;
  offset_x3 = 100;
  offset_y3 = 100; /// < offset of the display on model
  use_shock_points_to_compute_homography_ = false; // if false it uses correspondence of plus and minus reconstructed boundary points
  use_ransac_to_compute_homography_ = false;        // if true it uses ransac to compute the homography, otherwise plain SVD
  use_similarity_transform_ = true;        // if true it finds a similarity transform via rgrl estimator otherwise computes the homography

  sampling_interval_ = 5;

  threshold_ = 0.3f;
  N_ = 3;
  k_ = 2;
  detection_box_ = 0;
}

void dbskr_shock_patch_match_tableau::set_match(dbskr_shock_patch_match_sptr m) 
{ 
  match_ = m; 
  match_->left_ = 0;
  match_->right_ = 0;
  match_->cor_ = 0;
  vcl_map<int, dbskr_shock_patch_sptr>& map1 = m->get_id_map1();
  model_box_ = new vsol_box_2d();
  model_poly_ = 0;
  for (vcl_map<int, dbskr_shock_patch_sptr>::iterator iter = map1.begin(); iter != map1.end(); iter++) {
    model_box_->grow_minmax_bounds(iter->second->bounding_box());
    if (!model_poly_)
      model_poly_ = iter->second->get_outer_boundary();
    else {
      if (model_poly_->size() < iter->second->get_outer_boundary()->size())
        model_poly_ = iter->second->get_outer_boundary();
    }
  }
  model_box_poly_ = bsol_algs::poly_from_box(model_box_);

  image_box_ = new vsol_box_2d();
  vcl_map<int, dbskr_shock_patch_sptr>& map2 = m->get_id_map2();
  for (vcl_map<int, dbskr_shock_patch_sptr>::iterator iter = map2.begin(); iter != map2.end(); iter++) {
    image_box_->grow_minmax_bounds(iter->second->bounding_box());
  }
  
  trans_model_box_ = 0;
  trans_model_poly_ = 0;
} 

//: set the shock patch matching correspondence
void dbskr_shock_patch_match_tableau::set_curve_match(dbskr_shock_patch_curve_match_sptr m)
{
  curve_match_ = m; 
  curve_match_->left_ = 0;
  curve_match_->right_ = 0;
}

bool dbskr_shock_patch_match_tableau::handle( const vgui_event & e )
{
  if ( e.type == vgui_DRAW )
      draw_render();

  if (match_) {
    if (gesture_create_match_storage_(e)) {
      if (match_->left_) {

        vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
        if(!res) {
          vcl_cout << "Could not access repository!\n";
          return false;
        }

        dbskr_shock_match_storage_sptr st = dbskr_shock_match_storage_new();
        vcl_set<bpro1_storage_sptr> st_set = res->get_all_storage_classes(res->current_frame());
        vcl_string name_initial = "patch_match_str";
        int len = name_initial.length();
        int max = 0;
        for (vcl_set<bpro1_storage_sptr>::iterator iter = st_set.begin();
          iter != st_set.end(); iter++) {
            if ((*iter)->type() == st->type() && 
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
        st->set_name(name_initial);
        match_->cor_->clear_map_list();
        if (match_->cor_->recover_dart_ids_and_scurves())
          vcl_cout << "recovered dart ids and scurves\n";
        else 
          vcl_cout << "problems in recovery!!\n";

        st->set_sm_cor(match_->cor_);
        res->store_data(st);

        bvis1_manager::instance()->add_to_display(st);
        bvis1_manager::instance()->display_current_frame();

      } else {
        vcl_cout << " select a match first!!\n";
      }
    }

  if (gesture_select_point_(e)) {

    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix_, iy_);
    //vcl_cout << "ix: " << ix_ << " iy: " << iy_ << " ";
    mouse_pt_ = vgl_point_2d<double>(ix_, iy_);

    bool found_it = false;
    vcl_map<int, dbskr_shock_patch_sptr>& id_map1 = match_->get_id_map1();
    vcl_map<int, dbskr_shock_patch_sptr>& id_map2 = match_->get_id_map2();
    match_->left_ = 0;
    patch_cor_map_type& map = match_->get_map();
    
    vcl_map<int, dbskr_shock_patch_sptr>::iterator itr = id_map1.begin();
    for ( ; itr != id_map1.end(); itr++) {
      dbskr_shock_patch_sptr sp = itr->second;
      if (sp->inside(ix_, iy_)) {
          match_->left_ = sp;
        found_it = true;
        break;
      }
    }

    if (found_it) {
      patch_cor_map_iterator iter = map.find(match_->left_->id());  
      if (iter == map.end()) {        
         match_->left_->set_tree_parameters(match_->edit_params_.elastic_splice_cost_, match_->edit_params_.circular_ends_, 
                                         match_->edit_params_.combined_edit_, 
                                         match_->edit_params_.scurve_sample_ds_, match_->edit_params_.scurve_interpolate_ds_);
        for (unsigned iii = 0; iii < match_->patch_set2_.size(); iii++) 
          match_->patch_set2_[iii]->set_tree_parameters(match_->edit_params_.elastic_splice_cost_, match_->edit_params_.circular_ends_, 
                                                      match_->edit_params_.combined_edit_, 
                                                      match_->edit_params_.scurve_sample_ds_, match_->edit_params_.scurve_interpolate_ds_);

        if(find_patch_correspondences(match_->left_, match_->patch_set2_, map, match_->edit_params_)) 
        {
          vcl_cout << "Matched the patch to the second set using edit distance parameters in match_\n";
        } else {
          vcl_cout << "Problems during matching\n";
          return false;
        }
      }
    
      //: make sure the tree parameters are set during if loaded the dbskr_patch_match_sptr from a file via a process
      match_->left_v_ = map[match_->left_->id()];
      if (match_->left_v_->size() > 0) {
        match_->right_ = id_map2[(*match_->left_v_)[0].first];
        match_->cor_ = (*match_->left_v_)[0].second;
        match_->current_left_v_id_ = 0;

        if (!match_->cor_->get_tree1())
          match_->cor_->set_tree1(match_->left_->tree());
        if (!match_->cor_->get_tree2())
          match_->cor_->set_tree2(match_->right_->tree());

        vgl_h_matrix_2d<double> H;
        trans_poly_ = 0;
        trans_model_box_ = 0;
        bool found_ok = false;
        if (use_similarity_transform_) {
          if (match_->cor_->compute_similarity2D(H, true, sampling_interval_, use_shock_points_to_compute_homography_)) 
            found_ok = true;
        } else {
          if (match_->cor_->compute_homography(H, true, sampling_interval_,
                                               use_shock_points_to_compute_homography_,
                                               use_ransac_to_compute_homography_))   // tree1_to_tree2 = false --> find homography that maps right (tree2) to left (tree1) 
            found_ok = true;
        }
        
        if (found_ok) {
          vsol_polygon_2d_sptr left_poly = match_->left_->get_outer_boundary();
          vsol_point_2d_sptr center = new vsol_point_2d(0, 0);
          trans_poly_ = bsol_algs::transform_about_point(left_poly, center, H); 
          trans_model_poly_ = bsol_algs::transform_about_point(model_poly_, center, H);

          vsol_polygon_2d_sptr trans_model_box_poly = bsol_algs::transform_about_point(model_box_poly_, center, H); 
          trans_model_box_ = new vsol_box_2d();
          for (unsigned jjj = 0; jjj < trans_model_box_poly->size(); jjj++) 
            trans_model_box_->add_point(trans_model_box_poly->vertex(jjj)->x(), trans_model_box_poly->vertex(jjj)->y());
        }

        vcl_cout << "------------ First match ------------\n";
        vcl_cout << " left: " << match_->left_->id() << " right: " << match_->right_->id() << " cost: ";
        vcl_cout << match_->cor_->final_cost() << " norm: " << match_->cor_->final_norm_cost() << "\n";
        vcl_cout << "homography mapping right to left:\n" << H << vcl_endl;
        double angle, dist;
        match_->get_rotation_angle_degree(H, angle);
        match_->get_Lie_dist_to_identity(H, dist);
        vcl_cout << "rot angle: " << angle << " degree Lie dist to identity: " << dist << "\n";
        vcl_cout << "-------------------------------------\n";

      } else 
        match_->current_left_v_id_ = -1;

    } else 
      vcl_cout << "Current mouse position is not covered by any of the shock patches!\n";

    bvis1_manager::instance()->post_redraw();
    //bvis1_manager::instance()->regenerate_all_tableaux();
    //bvis1_manager::instance()->display_current_frame();
  }

  if (next_match_(e)) {
     vcl_map<int, dbskr_shock_patch_sptr>& id_map2 = match_->get_id_map2();
    if (match_->current_left_v_id_ >= 0) {
      match_->current_left_v_id_ = (match_->current_left_v_id_ + 1)%match_->left_v_->size();
      match_->right_ = id_map2[(*match_->left_v_)[match_->current_left_v_id_].first];
      match_->cor_ = (*match_->left_v_)[match_->current_left_v_id_].second;

      if (!match_->cor_->get_tree1())
          match_->cor_->set_tree1(match_->left_->tree());
        if (!match_->cor_->get_tree2())
          match_->cor_->set_tree2(match_->right_->tree());

        vgl_h_matrix_2d<double> H;
        trans_poly_ = 0;
        trans_model_box_ = 0;
        bool found_ok = false;
        if (use_similarity_transform_) {
          if (match_->cor_->compute_similarity2D(H, true, sampling_interval_, use_shock_points_to_compute_homography_)) 
            found_ok = true;
        } else {
          if (match_->cor_->compute_homography(H, true, sampling_interval_,
                                               use_shock_points_to_compute_homography_,
                                               use_ransac_to_compute_homography_))   // tree1_to_tree2 = false --> find homography that maps right (tree2) to left (tree1) 
            found_ok = true;
        }
        
        if (found_ok) {
          vsol_polygon_2d_sptr left_poly = match_->left_->get_outer_boundary();
          vsol_point_2d_sptr center = new vsol_point_2d(0, 0);
          trans_poly_ = bsol_algs::transform_about_point(left_poly, center, H); 
          trans_model_poly_ = bsol_algs::transform_about_point(model_poly_, center, H);

          vsol_polygon_2d_sptr trans_model_box_poly = bsol_algs::transform_about_point(model_box_poly_, center, H); 
          trans_model_box_ = new vsol_box_2d();
          for (unsigned jjj = 0; jjj < trans_model_box_poly->size(); jjj++) 
            trans_model_box_->add_point(trans_model_box_poly->vertex(jjj)->x(), trans_model_box_poly->vertex(jjj)->y());
        }

      vcl_cout << "------------ " << match_->current_left_v_id_ << " match ------------\n";
      vcl_cout << " left: " << match_->left_->id() << " right: " << match_->right_->id() << " cost: ";
      vcl_cout << match_->cor_->final_cost() << " norm: " << match_->cor_->final_norm_cost() << "\n";
      vcl_cout << "homography mapping right to left:\n" << H << vcl_endl;
      double angle, dist;
      match_->get_rotation_angle_degree(H, angle);
      match_->get_Lie_dist_to_identity(H, dist);
      vcl_cout << "rot angle: " << angle << " degree Lie dist to identity: " << dist << "\n";
      vcl_cout << "-------------------------------------\n";
    }

    bvis1_manager::instance()->post_redraw();
    //bvis1_manager::instance()->regenerate_all_tableaux();
    //bvis1_manager::instance()->display_current_frame();
  }

  if (next_patch_(e)) {
    
    if (match_->left_) {
      vcl_map<int, dbskr_shock_patch_sptr>& id_map1 = match_->get_id_map1();
      vcl_map<int, dbskr_shock_patch_sptr>& id_map2 = match_->get_id_map2();
      patch_cor_map_type& map = match_->get_map();
      vcl_map<int, dbskr_shock_patch_sptr>::iterator itr_main;
      vcl_map<int, dbskr_shock_patch_sptr>::iterator itr = id_map1.begin();
      for ( ; itr != id_map1.end(); itr++) {
        dbskr_shock_patch_sptr sp = itr->second;
        if (match_->left_ == sp) {
          itr_main = itr;
          break;
        }
      }

      bool found_it = false;
      for ( itr++; itr != id_map1.end(); itr++) {
        dbskr_shock_patch_sptr sp = itr->second;
        if (sp->inside(ix_, iy_)) {
          match_->left_ = sp;
          found_it = true;
          break;
        }
      }

      if (!found_it) {
        for (itr = id_map1.begin(); itr != itr_main; itr++) {
          dbskr_shock_patch_sptr sp = itr->second;
          if (sp->inside(ix_, iy_)) {
            match_->left_ = sp;
            found_it = true;
            break;
          }
        }
      }
      if (found_it) {
        patch_cor_map_iterator iter = map.find(match_->left_->id());
        if (iter == map.end()) {
          match_->left_->set_tree_parameters(match_->edit_params_.elastic_splice_cost_, match_->edit_params_.circular_ends_, 
                                         match_->edit_params_.combined_edit_, 
                                         match_->edit_params_.scurve_sample_ds_, match_->edit_params_.scurve_interpolate_ds_);
          for (unsigned iii = 0; iii < match_->patch_set2_.size(); iii++) 
            match_->patch_set2_[iii]->set_tree_parameters(match_->edit_params_.elastic_splice_cost_, match_->edit_params_.circular_ends_, 
                                                      match_->edit_params_.combined_edit_, 
                                                      match_->edit_params_.scurve_sample_ds_, match_->edit_params_.scurve_interpolate_ds_);

          if(find_patch_correspondences(match_->left_, match_->patch_set2_, map, match_->edit_params_)) 
          {
            vcl_cout << "Matched the patch to the second set\n";
          } else {
            vcl_cout << "Problems during matching\n";
            return false;
          }
        }
        
        match_->left_v_ = map[match_->left_->id()];
        if (match_->left_v_->size() > 0) {
          match_->right_ = id_map2[(*match_->left_v_)[0].first];
          match_->cor_ = (*match_->left_v_)[0].second;
          match_->current_left_v_id_ = 0;

          if (!match_->cor_->get_tree1())
          match_->cor_->set_tree1(match_->left_->tree());
        if (!match_->cor_->get_tree2())
          match_->cor_->set_tree2(match_->right_->tree());

        vgl_h_matrix_2d<double> H;
        trans_poly_ = 0;
        trans_model_box_ = 0;
        bool found_ok = false;
        if (use_similarity_transform_) {
          if (match_->cor_->compute_similarity2D(H, true, sampling_interval_, use_shock_points_to_compute_homography_)) 
            found_ok = true;
        } else {
          if (match_->cor_->compute_homography(H, true, sampling_interval_,
                                               use_shock_points_to_compute_homography_,
                                               use_ransac_to_compute_homography_))   // tree1_to_tree2 = false --> find homography that maps right (tree2) to left (tree1) 
            found_ok = true;
        }
        
        if (found_ok) {
          vsol_polygon_2d_sptr left_poly = match_->left_->get_outer_boundary();
          vsol_point_2d_sptr center = new vsol_point_2d(0, 0);
          trans_poly_ = bsol_algs::transform_about_point(left_poly, center, H); 
          trans_model_poly_ = bsol_algs::transform_about_point(model_poly_, center, H);

          vsol_polygon_2d_sptr trans_model_box_poly = bsol_algs::transform_about_point(model_box_poly_, center, H); 
          trans_model_box_ = new vsol_box_2d();
          for (unsigned jjj = 0; jjj < trans_model_box_poly->size(); jjj++) 
            trans_model_box_->add_point(trans_model_box_poly->vertex(jjj)->x(), trans_model_box_poly->vertex(jjj)->y());
        }

          vcl_cout << "------------ First match ------------\n";
          vcl_cout << " left: " << match_->left_->id() << " right: " << match_->right_->id() << " cost: ";
          vcl_cout << match_->cor_->final_cost() << " norm: " << match_->cor_->final_norm_cost() << "\n";
          vcl_cout << "homography mapping right to left:\n" << H << vcl_endl;
          double angle, dist;
          match_->get_rotation_angle_degree(H, angle);
          match_->get_Lie_dist_to_identity(H, dist);
          vcl_cout << "rot angle: " << angle << " degree Lie dist to identity: " << dist << "\n";
          vcl_cout << "-------------------------------------\n";
        } else 
          match_->current_left_v_id_ = -1;
      } else 
        vcl_cout << "No next!\n";

      bvis1_manager::instance()->post_redraw();
      //bvis1_manager::instance()->regenerate_all_tableaux();
      //bvis1_manager::instance()->display_current_frame();
    } else 
      vcl_cout << "Select a left patch first!\n";
  }

  if (gesture_select_match_triplet_(e)) {
    if (match_->left_ && match_->right_ && trans_model_poly_) {
      if (display_traced_boudary_)
        left_patches_.push_back(match_->left_->get_traced_boundary());
      else
        left_patches_.push_back(match_->left_->get_outer_boundary());

      if (display_traced_boudary_)
        right_patches_.push_back(match_->right_->get_traced_boundary());
      else
        right_patches_.push_back(match_->right_->get_outer_boundary());

      transformed_models_.push_back(trans_model_poly_);
    }
    bvis1_manager::instance()->post_redraw();
  } 

  if (gesture_clear_match_triplets_(e)) {
    left_patches_.clear();
    right_patches_.clear();
    transformed_models_.clear();
    bvis1_manager::instance()->post_redraw();
  }

  } else if (curve_match_) {  // curve_match

    if (gesture_select_point_(e)) {

      vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix_, iy_);
      //vcl_cout << "ix: " << ix_ << " iy: " << iy_ << " ";
      mouse_pt_ = vgl_point_2d<double>(ix_, iy_);

      bool found_it = false;
      curve_match_->left_ = 0;
      vcl_map<int, dbskr_shock_patch_sptr>& id_map1 = curve_match_->get_id_map1();
      vcl_map<int, dbskr_shock_patch_sptr>& id_map2 = curve_match_->get_id_map2();
      patch_curve_cor_map_type& curve_map = curve_match_->get_map();
      
      vcl_map<int, dbskr_shock_patch_sptr>::iterator itr = id_map1.begin();
      for ( ; itr != id_map1.end(); itr++) {
        dbskr_shock_patch_sptr sp = itr->second;
        if (sp->inside(ix_, iy_)) {
            curve_match_->left_ = sp;
          found_it = true;
          break;
        }
      }

      if (found_it) {
        patch_curve_cor_map_iterator iter = curve_map.find(curve_match_->left_->id());
        if (iter == curve_map.end()) {
          if(find_patch_correspondences_curve(curve_match_->left_, curve_match_->patch_set2_, curve_map, curve_match_->n_)) 
          {
            vcl_cout << "Matched the patch to the second set\n";
          } else {
            vcl_cout << "Problems during matching\n";
            return false;
          }
        }
        
        curve_match_->left_curve_v_ = curve_map[curve_match_->left_->id()];
        if (curve_match_->left_curve_v_->size() > 0) {
          curve_match_->right_ = id_map2[(*curve_match_->left_curve_v_)[0].first];
          curve_match_->curve_cor_ = (*curve_match_->left_curve_v_)[0].second;
          curve_match_->current_left_v_id_ = 0;
          vcl_cout << "------------ First match ------------\n";
          vcl_cout << " left: " << curve_match_->left_->id() << " right: " << curve_match_->right_->id() << " cost: ";
          for (unsigned kk = 0; kk < curve_match_->curve_cor_.size(); kk++)
            vcl_cout << curve_match_->curve_cor_[kk]->final_cost_ << " norm: " << curve_match_->curve_cor_[kk]->final_norm_cost_ << "\n";
          vcl_cout << "-------------------------------------\n";
        } else 
          match_->current_left_v_id_ = -1;

      } else 
        vcl_cout << "Current mouse position is not covered by any of the shock patches!\n";

      bvis1_manager::instance()->post_redraw();
      //bvis1_manager::instance()->regenerate_all_tableaux();
      //bvis1_manager::instance()->display_current_frame();
    }

    if (next_match_(e)) {
       vcl_map<int, dbskr_shock_patch_sptr>& id_map2 = curve_match_->get_id_map2();
      if (curve_match_->current_left_v_id_ >= 0) {
        curve_match_->current_left_v_id_ = (curve_match_->current_left_v_id_ + 1)%curve_match_->left_curve_v_->size();
        curve_match_->right_ = id_map2[(*curve_match_->left_curve_v_)[curve_match_->current_left_v_id_].first];
        curve_match_->curve_cor_ = (*curve_match_->left_curve_v_)[curve_match_->current_left_v_id_].second;
        vcl_cout << "------------ " << curve_match_->current_left_v_id_ << " match ------------\n";
        vcl_cout << " left: " << curve_match_->left_->id() << " right: " << curve_match_->right_->id() << " cost: ";
        for (unsigned kk = 0; kk < curve_match_->curve_cor_.size(); kk++)
          vcl_cout << curve_match_->curve_cor_[kk]->final_cost_ << " norm: " << curve_match_->curve_cor_[kk]->final_norm_cost_ << "\n";
        vcl_cout << "-------------------------------------\n";
      }

      bvis1_manager::instance()->post_redraw();
      //bvis1_manager::instance()->regenerate_all_tableaux();
      //bvis1_manager::instance()->display_current_frame();
    }

    if (next_patch_(e)) {
      
      if (curve_match_->left_) {
        vcl_map<int, dbskr_shock_patch_sptr>& id_map1 = curve_match_->get_id_map1();
        vcl_map<int, dbskr_shock_patch_sptr>& id_map2 = curve_match_->get_id_map2();
        patch_curve_cor_map_type& curve_map = curve_match_->get_map();
        vcl_map<int, dbskr_shock_patch_sptr>::iterator itr_main;
        vcl_map<int, dbskr_shock_patch_sptr>::iterator itr = id_map1.begin();
        for ( ; itr != id_map1.end(); itr++) {
          dbskr_shock_patch_sptr sp = itr->second;
          if (curve_match_->left_ == sp) {
            itr_main = itr;
            break;
          }
        }

        bool found_it = false;
        for ( itr++; itr != id_map1.end(); itr++) {
          dbskr_shock_patch_sptr sp = itr->second;
          if (sp->inside(ix_, iy_)) {
            curve_match_->left_ = sp;
            found_it = true;
            break;
          }
        }

        if (!found_it) {
          for (itr = id_map1.begin(); itr != itr_main; itr++) {
            dbskr_shock_patch_sptr sp = itr->second;
            if (sp->inside(ix_, iy_)) {
              curve_match_->left_ = sp;
              found_it = true;
              break;
            }
          }
        }
        if (found_it) {
          patch_curve_cor_map_iterator iter = curve_map.find(curve_match_->left_->id());
          if (iter == curve_map.end()) {
            if(find_patch_correspondences_curve(curve_match_->left_, curve_match_->patch_set2_, curve_map, curve_match_->n_)) 
            {
              vcl_cout << "Matched the patch to the second set\n";
            } else {
              vcl_cout << "Problems during matching\n";
              return false;
            }
          }
          
          curve_match_->left_curve_v_ = curve_map[curve_match_->left_->id()];
          if (curve_match_->left_curve_v_->size() > 0) {
            curve_match_->right_ = id_map2[(*curve_match_->left_curve_v_)[0].first];
            curve_match_->curve_cor_ = (*curve_match_->left_curve_v_)[0].second;
            curve_match_->current_left_v_id_ = 0;
            vcl_cout << "------------ First match ------------\n";
            vcl_cout << " left: " << curve_match_->left_->id() << " right: " << curve_match_->right_->id() << " cost: ";
            for (unsigned kk = 0; kk < curve_match_->curve_cor_.size(); kk++)
              vcl_cout << curve_match_->curve_cor_[kk]->final_cost_ << " norm: " << curve_match_->curve_cor_[kk]->final_norm_cost_ << "\n";
            vcl_cout << "-------------------------------------\n";
          } else 
            curve_match_->current_left_v_id_ = -1;
        } else 
          vcl_cout << "No next!\n";

        bvis1_manager::instance()->post_redraw();
        //bvis1_manager::instance()->regenerate_all_tableaux();
        //bvis1_manager::instance()->display_current_frame();
      } else 
        vcl_cout << "Select a left patch first!\n";
    }

    }
 
  return false;
}

vsol_box_2d_sptr translate_box(vsol_box_2d_sptr box, double x, double y) {
  vsol_box_2d_sptr new_box = new vsol_box_2d();
  new_box->add_point(box->get_min_x() + x, box->get_min_y() + y);
  new_box->add_point(box->get_max_x() + x, box->get_max_y() + y);
  return new_box;
}

void dbskr_shock_patch_match_tableau::draw_render()
{
  
  if (display_left_) {
    if (match_ && match_->left_) 
      draw_patch(match_->left_, 0, 0);
    else if (curve_match_ && curve_match_->left_)
      draw_patch(curve_match_->left_, 0, 0);
  }

  if (display_right_) {
    if (match_ && match_->right_) 
      draw_patch(match_->right_, offset_x2, offset_y2);
    else if (curve_match_ && curve_match_->right_)
      draw_patch(curve_match_->right_, offset_x2, offset_y2);
  }

  if (display_boxes_) {
    if (model_box_) {
      glColor3f( 0.0, 0.0, 1.0 );
      draw_box(model_box_);
    } 
    if (image_box_) {
      glColor3f( 1.0, 0.0, 0.0 );
      draw_box(translate_box(image_box_, offset_x2, offset_y2));
    }
    if (trans_model_box_) {
      glColor3f( 0.0, 1.0, 0.0 );
      draw_box(translate_box(trans_model_box_, offset_x2, offset_y2));
    }
    if (detection_box_) {
      glColor3f( 1.0, 1.0, 0.0 );
      draw_box(translate_box(detection_box_, offset_x2, offset_y2));
    }
  }

  if (display_on_query_) {
    if (match_ && match_->left_) {
      //draw_poly_POLYGON(model_poly_, 0, offset_y3, 0.0f, 0.0f, 0.0f);
      draw_poly_LINE_STRIP(model_poly_, 0, offset_y3, 0.0f, 0.0f, 0.0f, 2.0f);
      //draw_patch_POLYGON(match_->left_, 0, offset_y3, 1.0f, 0.0f, 0.0f);
      draw_patch_LINE_STRIP(match_->left_, 0, offset_y3, 1.0f, 0.0f, 0.0f, 1.0f);
    }
    
    if (match_ && match_->left_ && match_->right_) {
      //if (trans_poly_) {
      //  draw_poly_LINE_STRIP(trans_poly_, offset_x3, offset_y3, 1.0f, 0.0f, 0.0f, 2.0f)
      //} else {
      //  vcl_cout << "transformed polygon is not computed ok!!!\n";
     // }
      if (trans_model_poly_) {
        draw_poly_LINE_STRIP(trans_model_poly_, offset_x3, offset_y3, 0.0f, 0.0f, 0.0f, 3.0f);
      }
      if (match_ && match_->right_) 
        //draw_patch_POLYGON(match_->right_, offset_x3, offset_y3, 1.0f, 0.0f, 0.0f);
        draw_patch_LINE_STRIP(match_->right_, offset_x3, offset_y3, 1.0f, 0.0f, 0.0f, 1.0f);
    }

    if (left_patches_.size() > 0 && right_patches_.size() > 0 && transformed_models_.size() > 0 && 
      transformed_models_.size() == left_patches_.size() &&  left_patches_.size() == right_patches_.size()) {
        draw_poly_LINE_STRIP(model_poly_, 0, 2*offset_y3, 0.0f, 0.0f, 0.0f, 2.0f);
        vcl_vector<float> col(3, 0);
        vcl_vector<vcl_vector<float> > cols(3, col);
        cols[0][0] = 1.0f; cols[0][1] = 0.0f; cols[0][2] = 0.0f; 
        cols[1][0] = 0.0f; cols[1][1] = 1.0f; cols[1][2] = 0.0f; 
        cols[2][0] = 0.0f; cols[2][1] = 0.0f; cols[2][2] = 1.0f; 
        for (unsigned i = 0; i < left_patches_.size(); i++) {
          if (i < cols.size()) {
            draw_poly_LINE_STRIP(left_patches_[i], 0, 2*offset_y3, cols[i][0], cols[i][1], cols[i][2], 1.0f);
            draw_poly_LINE_STRIP(right_patches_[i], 2*offset_x3, 2*offset_y3, cols[i][0], cols[i][1], cols[i][2], 1.0f);
            draw_poly_LINE_STRIP(transformed_models_[i], offset_x3, 2*offset_y3, cols[i][0], cols[i][1], cols[i][2], 1.0f);
          } else {
            draw_poly_LINE_STRIP(left_patches_[i], 0, 2*offset_y3, 1.0f, 1.0f, 0.0f, 1.0f);
            draw_poly_LINE_STRIP(right_patches_[i], 2*offset_x3, 2*offset_y3, 1.0f, 1.0f, 0.0f, 1.0f);
            draw_poly_LINE_STRIP(transformed_models_[i], offset_x3, 2*offset_y3, 1.0f, 1.0f, 0.0f, 1.0f);
          }
        }
    }
  }

  if (display_curve_match_ && curve_match_ && curve_match_->curve_cor_.size() > 0) {

    if (display_corresponding_bnd_points_)
    {

      for (unsigned kk = 0; kk < curve_match_->curve_cor_.size(); kk++)
      {
        dbcvr_cv_cor_sptr cor = curve_match_->curve_cor_[kk];
        vcl_vector<vgl_point_2d<double> > pts1 = cor->get_contour_pts1();
        vcl_vector<vgl_point_2d<double> > pts2 = cor->get_contour_pts2();

        int color = 0;
        for (unsigned int j = 0; j<pts1.size(); j++) {
          glColor3f( rnd_colormap2[color][0], rnd_colormap2[color][1] , rnd_colormap2[color][2] );
          glPointSize( 7.0 );
          glBegin( GL_POINTS );
            glVertex2f( pts1[j].x() + offset_x1, pts1[j].y() + offset_y1 );
            glVertex2f( pts2[j].x() + offset_x2, pts2[j].y() + offset_y2 );
          glEnd();
          color = (color+1) % 5000;  
        }
      }
    }

  }

  if (display_shocks_ && match_ && match_->cor_) {
    
    dbskr_tree_sptr tree1, tree2;
    
    if (!(match_->cor_)->get_tree1()) {
    tree1 = (match_->left_)->tree();  // forces computation of the tree if not available
      if (!tree1) {
        vcl_cout << "Tree is not available for patch: " << (match_->left_)->id() << " skipping\n" << vcl_endl;
      }
      match_->cor_->set_tree1(tree1);
    }
    
    if (!(match_->cor_)->get_tree2()) {
    tree2 = (match_->right_)->tree();  // forces computation of the tree if not available
      if (!tree2) {
        vcl_cout << "Tree is not available for patch: " << (match_->right_)->id() << " skipping\n" << vcl_endl;
      }
      match_->cor_->set_tree2(tree2);
    }
   
    sm_cor_ = match_->cor_;
    draw_matched_shock_graphs();

    if (display_corresponding_bnd_points_)
    {
      vcl_vector<dbskr_scurve_sptr>& curve_list1 = sm_cor_->get_curve_list1();
      vcl_vector<dbskr_scurve_sptr>& curve_list2 = sm_cor_->get_curve_list2();
      vcl_vector<vcl_vector < vcl_pair <int,int> > >& map_list = sm_cor_->get_map_list();
      vcl_vector<pathtable_key>& key_map = sm_cor_->get_map();

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

          pathtable_key key = key_map[i];
          vcl_cout << "in corresponding bnd pts, key first: " << key.first.first << ", " << key.first.second << " sc1 # pts: " << sc1->num_points() << " ";
          vcl_cout << ", key second: " << key.second.first << ", " << key.second.second << " sc2 # pts: " << sc2->num_points() << "\n";

          //vcl_cout << "curve1 size: " << sc1->num_points() << " curve2 size: " << sc2->num_points() << " drawing " << map.size() << " pair of points\n";

          for (unsigned int j = 0; j<map.size(); j++) {
            int k = map[j].first;
            int m = map[j].second;

            glColor3f( rnd_colormap2[color][0] , rnd_colormap2[color][1] , rnd_colormap2[color][2] );
            glPointSize( 5.0 );
            glBegin( GL_POINTS );
            glVertex2f( sc1->bdry_plus_pt(k).x() + offset_x1, sc1->bdry_plus_pt(k).y() + offset_y1 );
            glVertex2f( sc1->bdry_minus_pt(k).x() + offset_x1, sc1->bdry_minus_pt(k).y() + offset_y1 );
            
            glVertex2f( sc2->bdry_plus_pt(m).x() + offset_x2, sc2->bdry_plus_pt(m).y() + offset_y2 );
            glVertex2f( sc2->bdry_minus_pt(m).x() + offset_x2, sc2->bdry_minus_pt(m).y() + offset_y2 );
           glEnd();

            
          }
          color = (color+1) % 5000;
        }
      }
    }
  }

  

  glColor3f( 0 , 1 , 1 );
  glPointSize( 30.0 );
  glBegin( GL_POINTS );
  glVertex2f( mouse_pt_.x(), mouse_pt_.y() );
  glEnd();
}

class dbskr_spm_tableau_toggle_command : public vgui_command
{
 public:
  dbskr_spm_tableau_toggle_command(dbskr_shock_patch_match_tableau* tab, const void* boolref) : 
       match_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    match_tableau->post_redraw(); 
  }

  dbskr_shock_patch_match_tableau *match_tableau;
  bool* bref;
};

class dbskr_spm_tableau_set_display_params_command : public vgui_command
{
 public:
  dbskr_spm_tableau_set_display_params_command(dbskr_shock_patch_match_tableau* tab, 
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

  dbskr_shock_patch_match_tableau *match_tableau;
  int* iref_;
  vcl_string name_;
};

class dbskr_spm_tableau_set_display_float_params_command : public vgui_command
{
 public:
  dbskr_spm_tableau_set_display_float_params_command(dbskr_shock_patch_match_tableau* tab, 
    const vcl_string& name, const void* floatref) : match_tableau(tab), fref_((float*)floatref), name_(name) {}

  void execute() 
  { 
    float param_val = *fref_;
    vgui_dialog param_dlg("Set Display Param");
    param_dlg.field(name_.c_str(), param_val);
    if(!param_dlg.ask())
      return;

    *fref_ = param_val;
    match_tableau->post_redraw(); 
  }

  dbskr_shock_patch_match_tableau *match_tableau;
  float* fref_;
  vcl_string name_;
};

void 
dbskr_shock_patch_match_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( ((display_left_)?on:off)+"Display Left", 
               new dbskr_spm_tableau_toggle_command(this, &display_left_));

  submenu.add( ((display_right_)?on:off)+"Display Right", 
               new dbskr_spm_tableau_toggle_command(this, &display_right_));

  submenu.add( ((display_outer_boundary_)?on:off)+"Display outer boundary", 
               new dbskr_spm_tableau_toggle_command(this, &display_outer_boundary_));

  submenu.add( ((display_real_boundaries_)?on:off)+"Display real boundaries", 
               new dbskr_spm_tableau_toggle_command(this, &display_real_boundaries_));

  submenu.add( ((display_traced_boudary_)?on:off)+"Display traced boundary", 
               new dbskr_spm_tableau_toggle_command(this, &display_traced_boudary_));

  submenu.add( ((display_shocks_)?on:off)+"Display Shocks", 
               new dbskr_spm_tableau_toggle_command(this, &display_shocks_));

  submenu.add( ((draw_shock_nodes_)?on:off)+"Show Shock Nodes", 
               new dbskr_spm_tableau_toggle_command(this, &draw_shock_nodes_));

  submenu.add( ((display_corresponding_bnd_points_)?on:off)+"Show Bnd Pts", 
               new dbskr_spm_tableau_toggle_command(this, &display_corresponding_bnd_points_));

  submenu.add( ((display_orig_shock_graphs_)?on:off)+"Show Original Shock Graphs", 
               new dbskr_spm_tableau_toggle_command(this, &display_orig_shock_graphs_));

  submenu.add( ((display_boundary_curves_)?on:off)+"Show Bnd Curves", 
               new dbskr_spm_tableau_toggle_command(this, &display_boundary_curves_));

  submenu.add( ((display_shock_samples_)?on:off)+"Show Shock Samples", 
               new dbskr_spm_tableau_toggle_command(this, &display_shock_samples_));

  submenu.add( ((display_boxes_)?on:off)+"Display boxes", 
               new dbskr_spm_tableau_toggle_command(this, &display_boxes_));

  //submenu.add( "Set shock curve sampling ds", 
  //             new dbskr_spm_tableau_set_display_params_command(this, "Shock curve sampling ds", &scurve_sample_ds_));

  submenu.add( ((match_with_circular_completions_)?on:off)+"Match with circular completions at leaf scurves", 
               new dbskr_spm_tableau_toggle_command(this, &match_with_circular_completions_));

  submenu.add( ((use_combined_edit_)?on:off)+"Match with combined width and orientation costs in edit distance algorithm", 
               new dbskr_spm_tableau_toggle_command(this, &use_combined_edit_));

  submenu.add( ((display_on_query_)?on:off)+"Show the transformed model patch on query patch?", 
              new dbskr_spm_tableau_toggle_command(this, &display_on_query_));

  submenu.add( ((use_shock_points_to_compute_homography_)?on:off)+"Compute a similarity transformation (otherwise computes a homography)?", 
              new dbskr_spm_tableau_toggle_command(this, &use_similarity_transform_));  

  submenu.add( ((use_shock_points_to_compute_homography_)?on:off)+"Use shock points to compute transformation (otherwise + & - bnd)?", 
              new dbskr_spm_tableau_toggle_command(this, &use_shock_points_to_compute_homography_));

  submenu.add( ((use_ransac_to_compute_homography_)?on:off)+"Use RANSAC to compute homogography (otherwise SVD)?", 
              new dbskr_spm_tableau_toggle_command(this, &use_ransac_to_compute_homography_));

  submenu.add( "Set sampling interval to compute homography", 
               new dbskr_spm_tableau_set_display_params_command(this, "sampling_interval_", &sampling_interval_));

  submenu.separator();

  submenu.add( "compute normalization based on reconstructed boundary length", new dbskr_shock_patch_match_tableau_command(this, this->match_, 1));
  
  submenu.add( "compute normalization based on total splice cost", new dbskr_shock_patch_match_tableau_command(this, this->match_, 2));

  submenu.add( "resort with respect to normalized costs", new dbskr_shock_patch_match_tableau_command(this, this->match_, 3));

  submenu.add( "resort with respect to costs", new dbskr_shock_patch_match_tableau_command(this, this->match_, 4));

  submenu.separator();

  submenu.add( "Set detection box threshold", 
               new dbskr_spm_tableau_set_display_float_params_command(this, "threshold_", &threshold_));
  submenu.add( "Set N (at least top N matches less than threshold)", 
               new dbskr_spm_tableau_set_display_params_command(this, "N_", &N_));
  submenu.add( "Set k (at least k model patches have N matches less than threshold)", 
               new dbskr_spm_tableau_set_display_params_command(this, "k_", &k_));

  submenu.add( "compute detection box", new dbskr_shock_patch_match_tableau_command(this, this->match_, 5));
  submenu.add( "clear detection box", new dbskr_shock_patch_match_tableau_command(this, this->match_, 6));




  //add this submenu to the popup menu
  menu.add("Matching Display Options", submenu);
}

void dbskr_shock_patch_match_tableau::
draw_poly_LINE_STRIP(vsol_polygon_2d_sptr poly, int off_x, int off_y, float r, float g, float b, float line_width)
{
  if (poly) {
    //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
    glBegin( GL_LINE_STRIP );
    glColor3f( r , g , b );
    glLineWidth (line_width);
    for( unsigned int i = 0 ; i < poly->size() ; i++ )
      glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
    glVertex2f( poly->vertex(0)->x()+off_x, poly->vertex(0)->y()+off_y );
    glEnd();
  }
}

void dbskr_shock_patch_match_tableau::
draw_poly_POLYGON(vsol_polygon_2d_sptr poly, int off_x, int off_y, float r, float g, float b)
{
  if (poly) {
    //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
    //glLineWidth (line_width);
    glBegin( GL_POLYGON );
    glColor3f( r , g , b );
    for( unsigned int i = 0 ; i < poly->size() ; i++ )
      glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
    //glVertex2f( poly->vertex(0)->x()+off_x, poly->vertex(0)->y()+off_y );
    glEnd();
  }
}


void dbskr_shock_patch_match_tableau::
draw_patch(dbskr_shock_patch_sptr shock_patch, int off_x, int off_y)
{
  if (display_outer_boundary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_outer_boundary();
    if (poly) {
      //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glColor3f( 1.0 , 1.0 , 0.0 );
      glLineWidth (3.0);
      glBegin( GL_LINE_STRIP );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
      glVertex2f( poly->vertex(0)->x()+off_x, poly->vertex(0)->y()+off_y );
      glEnd();
    }
  }

  if (display_real_boundaries_) {
    vcl_vector<vsol_polyline_2d_sptr>& rbs = shock_patch->get_real_boundaries();
    int color = 0;
    for (unsigned i = 0; i < rbs.size(); i++) {
      vsol_polyline_2d_sptr poly = rbs[i];
      //glColor3f( 0.0 , 0.0 , 1.0 );
      glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glLineWidth (4.0);
      glBegin( GL_LINE_STRIP );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
      glEnd();
      color = (color+1)%100;
    }
  }

  if (display_traced_boudary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_traced_boundary();
    if (poly) {
      //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glBegin( GL_LINE_STRIP );
      glColor3f( 1.0 , 0.0 , 1.0 );
      glLineWidth (3.0);
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
      glVertex2f( poly->vertex(0)->x()+off_x, poly->vertex(0)->y()+off_y );
      glEnd();
    }
  }
}

void dbskr_shock_patch_match_tableau::
draw_patch_POLYGON(dbskr_shock_patch_sptr shock_patch, int off_x, int off_y, float r, float g, float b)
{
  if (display_traced_boudary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_traced_boundary();
    if (poly) {
      //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
      glBegin( GL_POLYGON );
      glColor3f( r , g , b );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
      //glVertex2f( poly->vertex(0)->x()+off_x, poly->vertex(0)->y()+off_y );
      glEnd();
    }
  } else if (display_outer_boundary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_outer_boundary();
    if (poly) {
      //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glBegin( GL_POLYGON );
      glColor3f( r , g , b );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
      //glVertex2f( poly->vertex(0)->x()+off_x, poly->vertex(0)->y()+off_y );
      glEnd();
    }
  }
}

void dbskr_shock_patch_match_tableau::
draw_patch_LINE_STRIP(dbskr_shock_patch_sptr shock_patch, int off_x, int off_y, float r, float g, float b, float line_width)
{
  if (display_traced_boudary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_traced_boundary();
    if (poly) {
      //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glBegin( GL_LINE_STRIP );
      glColor3f( r , g , b );
      glLineWidth (line_width);
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
      glVertex2f( poly->vertex(0)->x()+off_x, poly->vertex(0)->y()+off_y );
      glEnd();
    }
  } else if (display_outer_boundary_) {
    vsol_polygon_2d_sptr poly = shock_patch->get_outer_boundary();
    if (poly) {
      //glColor3f( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      glBegin( GL_LINE_STRIP );
      glLineWidth (line_width);
      glColor3f( r , g , b );
      for( unsigned int i = 0 ; i < poly->size() ; i++ )
        glVertex2f( poly->vertex(i)->x()+off_x, poly->vertex(i)->y()+off_y );
      glVertex2f( poly->vertex(0)->x()+off_x, poly->vertex(0)->y()+off_y );
      glEnd();
    }
  }
}


