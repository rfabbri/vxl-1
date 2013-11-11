// This is brcv/rec/dbskr/dbskr_sm_cor.cxx

//:
// \file

#include <dbskr/dbskr_sm_cor.h>


#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_ctime.h>

#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_dpmatch_combined.h>
#include <dbskr/dbskr_localize_match.h>
#include <dbskr/dbskr_scurve.h>

#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_float_3x3.h>
#include <vnl/vnl_matrix.h>

#include <vgl/vgl_point_3d.h>
//#include <bmvl/brct/brct_algos.h>
#include <rgrl/rgrl_est_similarity2d.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_trans_similarity.h>

#define WRITE_OUTPUT  0

//: Constructor
//  edit_params_ is constructed with default constructor
dbskr_sm_cor::dbskr_sm_cor() :
  tree1_(0), tree2_(0)
{
  dart_path_map_.clear();
  pathtable_saved_ = false;
  dart_path_map_has_node_ids_ = false;
  //contstruct_circular_ends_ = true;
  fine_final_norm_costs_computed_ = false;
  //use_dpmatch_combined_ = false;
  //localize_match_ = false;
  trans_matrix_ = 0;
}

//: edit_params_ is constructed with default constructor
dbskr_sm_cor::dbskr_sm_cor(dbskr_tree_sptr t1, dbskr_tree_sptr t2) 
{
  tree1_ = t1;
  tree2_ = t2;
  dart_path_map_.clear();
  pathtable_saved_ = false;
  dart_path_map_has_node_ids_ = false;
  //contstruct_circular_ends_ = true;  
  fine_final_norm_costs_computed_ = false;
  //use_dpmatch_combined_ = false;
  //localize_match_ = false;
  //curve_matching_R_ = 6.0f; // this is the default, if correspondence will be read from a file, this should be set before
                            // calling read_and_construct_from_shgm(vcl_string fname);
  trans_matrix_ = 0;
}

void 
dbskr_sm_cor::set_tree_edit_params(float scurve_sample_ds, 
                            float scurve_interpolate_ds, 
                            bool localized_edit, 
                            bool elastic_splice_cost,
                            bool circular_ends,
                            bool combined_edit, 
                            bool coarse_edit,
                            double curve_matching_R)
{
  edit_params_.scurve_sample_ds_ = scurve_sample_ds;
  edit_params_.scurve_interpolate_ds_ = scurve_interpolate_ds;
  edit_params_.localized_edit_ = localized_edit;
  edit_params_.elastic_splice_cost_ = elastic_splice_cost;
  edit_params_.circular_ends_ = circular_ends;
  edit_params_.coarse_edit_ = coarse_edit;
  edit_params_.curve_matching_R_ = curve_matching_R;
}

//: return a new instance with all the maps reversed
dbskr_sm_cor_sptr 
dbskr_sm_cor::new_sm_reverse_maps()
{
  dbskr_sm_cor_sptr new_sm = new dbskr_sm_cor();
  
  vcl_vector<pathtable_key> map;
  for (unsigned i = 0; i < dart_path_map_.size(); i++) {
    pathtable_key key = dart_path_map_[i];
    pathtable_key new_key;
    
    new_key.first.first = key.second.first;
    new_key.first.second = key.second.second;
    new_key.second.first = key.first.first;
    new_key.second.second = key.first.second;

    map.push_back(new_key);
  }

  new_sm->set_tree_edit_params(edit_params_);
  new_sm->set_final_cost(final_cost());
  new_sm->set_final_norm_cost(final_norm_cost());
  new_sm->set_map(map);
  //new_sm->contstruct_circular_ends_ = contstruct_circular_ends_;

  return new_sm;
}

//: compute the similarity transformation induced by this tree correspondence
//  if use_shock_points = true --> uses shock branch points for correspondence
//  otherwise uses plus and minus reconstructed boundary points
bool dbskr_sm_cor::compute_similarity2D(vgl_h_matrix_2d<double>& H, bool tree1_to_tree2, unsigned sampling_interval, bool use_shock_points, bool save_trans)
{
  //if (!tree1_ || !tree2_)
  //  return false;

  if (!map_list_.size()) {
    vcl_cout << " map list is empty\n";
    return false;
  }

  if (map_list_.size() != curve_list1_.size() || map_list_.size() != curve_list2_.size()) {
    vcl_cout << " map list size: " << map_list_.size() << " curve list1 size: " << curve_list1_.size() << " curve list2 size: " << curve_list2_.size() << "\n";
    return false;
  }

  vcl_vector< rgrl_feature_sptr > pts1;
  vcl_vector< rgrl_feature_sptr > pts2;
  for (unsigned i = 0; i < map_list_.size(); i++) {

    dbskr_scurve_sptr sc1 = curve_list1_[i];
    dbskr_scurve_sptr sc2 = curve_list2_[i];

    for (unsigned j = 0; j < map_list_[i].size(); j+=sampling_interval) {
      vcl_pair<int, int> cor = map_list_[i][j];

      vnl_vector<double> v(2);
      if (use_shock_points) {
        v[0] = sc1->sh_pt(cor.first).x();
        v[1] = sc1->sh_pt(cor.first).y();
        pts1.push_back(new rgrl_feature_point(v));

        v[0] = sc2->sh_pt(cor.second).x();
        v[1] = sc2->sh_pt(cor.second).y();
        pts2.push_back(new rgrl_feature_point(v));
      } else {
        v[0] = sc1->bdry_minus_pt(cor.first).x();
        v[1] = sc1->bdry_minus_pt(cor.first).y();
        pts1.push_back(new rgrl_feature_point(v));

        v[0] = sc2->bdry_minus_pt(cor.second).x();
        v[1] = sc2->bdry_minus_pt(cor.second).y();
        pts2.push_back(new rgrl_feature_point(v));

        v[0] = sc1->bdry_plus_pt(cor.first).x();
        v[1] = sc1->bdry_plus_pt(cor.first).y();
        pts1.push_back(new rgrl_feature_point(v));

        v[0] = sc2->bdry_plus_pt(cor.second).x();
        v[1] = sc2->bdry_plus_pt(cor.second).y();
        pts2.push_back(new rgrl_feature_point(v));
      }
    }
    
  }

  rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
  if (tree1_to_tree2) {
    for ( unsigned i=0; i < pts1.size(); ++i ) {
      ms->add_feature_and_match( pts1[i], 0, pts2[i] );
    }
  } else {
    for ( unsigned i=0; i < pts2.size(); ++i ) {
      ms->add_feature_and_match( pts2[i], 0, pts1[i] );
    }
  }

  rgrl_estimator_sptr est = new rgrl_est_similarity2d(2);
  rgrl_trans_similarity dummy_trans(2);
  rgrl_transformation_sptr trans = est->estimate( ms, dummy_trans );
  if (!trans) {
    vcl_cout << "transformation could not be estimated by rgrl\n";
    return false;
  }

  rgrl_trans_similarity* s_trans = dynamic_cast<rgrl_trans_similarity*>(trans.as_pointer());
  if (!s_trans) {
    vcl_cout << "transformation pointer could not be retrieved\n";
    return false;
  }
  
  H.set_identity();
  vnl_matrix_fixed<double, 3, 3> t_matrix;
  t_matrix(0,0) = s_trans->A()(0,0);
  t_matrix(0,1) = s_trans->A()(0,1);
  t_matrix(1,0) = s_trans->A()(1,0);
  t_matrix(1,1) = s_trans->A()(1,1);
  t_matrix(0,2) = 0;
  t_matrix(1,2) = 0;
  t_matrix(2,0) = 0;
  t_matrix(2,1) = 0;
  t_matrix(2,2) = 1;
  H.set(t_matrix);
  H.set_translation(s_trans->t()[0], s_trans->t()[1]);

  if (save_trans) {
    
    //vcl_cout << " saving the trans_matrix_\n";

    trans_matrix_ = vnl_matrix<double>(3,3,0.0);

    (trans_matrix_)(0,0) = H.get_matrix()(0,0);
    (trans_matrix_)(0,1) = H.get_matrix()(0,1);
    (trans_matrix_)(0,2) = H.get_matrix()(0,2);
    (trans_matrix_)(1,0) = H.get_matrix()(1,0);
    (trans_matrix_)(1,1) = H.get_matrix()(1,1);
    (trans_matrix_)(1,2) = H.get_matrix()(1,2);
    (trans_matrix_)(2,0) = H.get_matrix()(2,0);
    (trans_matrix_)(2,1) = H.get_matrix()(2,1);
    (trans_matrix_)(2,2) = H.get_matrix()(2,2);
  }

  return true;
}

bool dbskr_sm_cor::get_similarity_trans(vgl_h_matrix_2d<double>& H) {
  if (trans_matrix_.rows() != 3 && trans_matrix_.cols() != 3)
    return false;

  vnl_matrix_fixed<double, 3, 3> t_matrix;
  t_matrix(0,0) = (trans_matrix_)(0,0);
  t_matrix(0,1) = (trans_matrix_)(0,1);
  t_matrix(0,2) = (trans_matrix_)(0,2);
  t_matrix(1,0) = (trans_matrix_)(1,0);
  t_matrix(1,1) = (trans_matrix_)(1,1);
  t_matrix(1,2) = (trans_matrix_)(1,2);
  t_matrix(2,0) = (trans_matrix_)(2,0);
  t_matrix(2,1) = (trans_matrix_)(2,1);
  t_matrix(2,2) = (trans_matrix_)(2,2);
  H.set(t_matrix);
  return true;
}

//: compute the homography induced by this tree correspondence
bool dbskr_sm_cor::compute_homography(vgl_h_matrix_2d<double>& H, bool tree1_to_tree2, unsigned sampling_interval, bool use_shock_points, bool use_ransac)
{
  if (!tree1_ || !tree2_)
    return false;

  if (!map_list_.size())
    return false;

  if (map_list_.size() != curve_list1_.size() || map_list_.size() != curve_list2_.size())
    return false;

  vcl_vector<vgl_homg_point_2d<double> > points1, points2;
  for (unsigned i = 0; i < map_list_.size(); i++) {

    dbskr_scurve_sptr sc1 = curve_list1_[i];
    dbskr_scurve_sptr sc2 = curve_list2_[i];

    for (unsigned j = 0; j < map_list_[i].size(); j+=sampling_interval) {
      vcl_pair<int, int> cor = map_list_[i][j];

      if (use_shock_points) {
        vgl_homg_point_2d<double> ps1(sc1->sh_pt(cor.first));
        vgl_homg_point_2d<double> ps2(sc2->sh_pt(cor.second));
        points1.push_back(ps1);
        points2.push_back(ps2);
      } else {
        vgl_homg_point_2d<double> pm1(sc1->bdry_minus_pt(cor.first));
        vgl_homg_point_2d<double> pm2(sc2->bdry_minus_pt(cor.second));

        vgl_homg_point_2d<double> pp1(sc1->bdry_plus_pt(cor.first));
        vgl_homg_point_2d<double> pp2(sc2->bdry_plus_pt(cor.second));

        points1.push_back(pm1);
        points1.push_back(pp1);

        points2.push_back(pm2);
        points2.push_back(pp2);
      }
    }
    
  }

  if (use_ransac)
  {
	vcl_cout << "ERROR: 'use_rasac' option is not implemented.\n";
  }

  

  
  // The following code depends on brct, which no longer exists. Commented out by Nhon

  //if (!use_ransac) {
  //  
  //  vgl_h_matrix_2d_compute_linear comp;
  //  if (tree1_to_tree2)
  //    return comp.compute(points1, points2, H);
  //  else
  //    return comp.compute(points2, points1, H);

  //} else {
  //  
  //  vcl_vector<vgl_point_3d<double> > world_points;
  //  vcl_vector<vgl_point_2d<double> > image_points;

  //  if (tree1_to_tree2) {
  //    for (unsigned i = 0; i < points1.size(); i++) 
  //      world_points.push_back(vgl_point_3d<double>(points1[i].x(), points1[i].y(),0));
  //    for (unsigned i = 0; i < points2.size(); i++) 
  //      image_points.push_back(vgl_point_2d<double>(points2[i].x(), points2[i].y()));
  //  } else {
  //    for (unsigned i = 0; i < points2.size(); i++) 
  //      world_points.push_back(vgl_point_3d<double>(points2[i].x(), points2[i].y(),0));
  //    for (unsigned i = 0; i < points1.size(); i++) 
  //      image_points.push_back(vgl_point_2d<double>(points1[i].x(), points1[i].y()));
  //  }
  //    
  //  return brct_algos::homography_ransac(world_points, image_points, H, true);  // optimize 
  //}

  // Always use vgl_h_matrix_2d_compute_linear regardless of "use_ransac"
  {
    vgl_h_matrix_2d_compute_linear comp;
    if (tree1_to_tree2)
      return comp.compute(points1, points2, H);
    else
      return comp.compute(points2, points1, H);
  }

}

//: Destructor
dbskr_sm_cor::~dbskr_sm_cor() 
{
  dart_path_map_.clear();
  map_list_.clear();
  curve_list1_.clear();
  curve_list2_.clear();
  pathtable_.clear();
  if (tree1_)
    tree1_ = 0;
  if (tree2_)
    tree2_ = 0;

  trans_matrix_.clear();

  match_costs_.clear();
  match_costs_d_.clear();
  match_costs_localized_.clear();
  match_costs_init_dr_.clear();
  match_costs_init_alp_.clear();
}

//void dbskr_sm_cor::write_shgm(double matching_cost, vcl_string fname) 
void dbskr_sm_cor::write_shgm(vcl_string fname) 
{
  if (!dart_path_map_.size()) {
    vcl_cout << "Paths are not computed, sm_cor object is empty!" << vcl_endl;
    return;
  }

  vcl_ofstream tf(fname.c_str());
  
  if (!tf) {
    vcl_cout << "Unable to open shgm file " << fname << " for write " << vcl_endl;
    return;
  }

  tf << "match output version 1.0\n";
  // names are unknown to this method but they are unimportant so just output the following
  // to comply with the format of the shgm file
  tf << "name1.shg\n";
  tf << "name2.shg\n";
  //tf << matching_cost << vcl_endl;
  tf << final_cost_ << vcl_endl;

  for (unsigned int i = 0; i<dart_path_map_.size(); i++) {
    pathtable_key key = dart_path_map_[i];
    //: find node paths
    vcl_vector<int> nodes1 = tree1_->find_node_path(key.first.first, key.first.second);
    vcl_vector<int> nodes2 = tree2_->find_node_path(key.second.first, key.second.second);

    for (unsigned int j = 0; j<nodes1.size(); j++) 
      tf << nodes1[j] << " ";
    tf << vcl_endl;
    for (unsigned int j = 0; j<nodes2.size(); j++) 
      tf << nodes2[j] << " ";
    tf << vcl_endl;
    tf << vcl_endl;    
  }

  tf.close();
  return;
}

float dbskr_sm_cor::read_and_construct_from_shgm(vcl_string fname, bool recover_dart_ids_scurves) 
{
  //clear everything
  clear();
  clear_lists();

  // first load the pathmap from the shgm file
  vcl_ifstream tf(fname.c_str());
  
  if (!tf) {
    vcl_cout << "Unable to open file " << fname << " for write " << vcl_endl;
    return -1;
  }

  char buffer[1000];
  tf.getline(buffer, 1000);  // version
  tf.getline(buffer, 1000);  // name 1
  vcl_string line = buffer;
  tf.getline(buffer, 1000);  // name 2
  line = buffer;

  //float cost;
  //tf >> cost;
  tf >> final_cost_;
  //vcl_cout << "matching cost: " << final_cost_ << "\n";
  
  vcl_map<vcl_string, vcl_string> paths;
  vcl_string line2;

  while (!tf.eof()) {
    tf.getline(buffer, 1000);
    line = buffer;
    if (line.length() < 2) // this is new line character or eof
      continue;
    // read the second line
    tf.getline(buffer, 1000);
    line2 = buffer;
    if (line2.length() < 2) {  // something is wrong with this file
      vcl_cout << "T1 and T2 corresponding paths are not on consecutive lines in this file\n!!";
      return -1;  // return as it is
    }

    char * pch;
    char tmp[1000];
    vcl_sprintf(tmp, "%s", line.c_str());
    //pch = strtok (line.c_str()," ");
    pch = strtok(tmp," ");
    int first = atoi(pch);
    int id = 0;
    while (pch != NULL)
    {
      id = atoi(pch);
      pch = strtok (NULL, " ");
    }

    pathtable_key key;
    key.first.first = first;
    key.first.second = id;

    vcl_sprintf(tmp, "%s", line2.c_str());
    pch = strtok(tmp," ");
    first = atoi(pch);
    
    while (pch != NULL)
    {
      id = atoi(pch);
      pch = strtok (NULL, " ");
    }
    key.second.first = first;
    key.second.second = id;

    dart_path_map_.push_back(key);
  }

  tf.close();

#if WRITE_OUTPUT
  vcl_set<int> dart_set1, dart_set2;
#endif 

  dart_path_map_has_node_ids_ = true;
  if (recover_dart_ids_scurves)
    recover_dart_ids_and_scurves();

  return final_cost_;
}

bool dbskr_sm_cor::recover_dart_ids_and_scurves() {

  if (dart_path_map_has_node_ids_) {
     // currently dart_path_map_ has keys with node ids on the tree, we need to turn them into dart ids on the tree
    //  using vcl_vector<int>& get_dart_path_from_nodes(int node1, int node2) method of tree

    for (unsigned int i = 0; i<dart_path_map_.size(); i++) {
      vcl_vector<int> dart_path1 = tree1_->get_dart_path_from_nodes(dart_path_map_[i].first.first, dart_path_map_[i].first.second);
      vcl_vector<int> dart_path2 = tree2_->get_dart_path_from_nodes(dart_path_map_[i].second.first, dart_path_map_[i].second.second);
      
      dart_path_map_[i].first.first = dart_path1[0];
      dart_path_map_[i].first.second = dart_path1[dart_path1.size()-1];

      dart_path_map_[i].second.first = dart_path2[0];
      dart_path_map_[i].second.second = dart_path2[dart_path2.size()-1];

#if WRITE_OUTPUT
      for (unsigned k = 0; k<dart_path1.size(); k++) {
        dart_set1.insert(dart_path1[k]);
        dart_set1.insert(tree1_->mate(dart_path1[k]));
      }
      for (unsigned k = 0; k<dart_path2.size(); k++) {
        dart_set2.insert(dart_path2[k]);
        dart_set2.insert(tree2_->mate(dart_path2[k]));
      }
#endif 
 
    }
    dart_path_map_has_node_ids_ = false;
  }

  if (tree1_ && tree2_) {

  // get all the correspondences from the path map
  //for (int i = dart_path_map_.size()-1; i>=0; i--) 
  if (!map_list_.size()) {  // recompute the matches
    vcl_cout << "previous final_cost_: " << final_cost_ << vcl_endl;
    final_cost_ = 0;
    curve_list1_.clear();
    curve_list2_.clear();
    match_costs_.clear(); 
    match_costs_d_.clear();
    match_costs_localized_.clear();
    match_costs_init_dr_.clear();
    match_costs_init_alp_.clear();

    vcl_vector<bool> tree1_used_darts(tree1_->size(), false), tree2_used_darts(tree2_->size(), false);
    for (unsigned int i = 0; i<dart_path_map_.size(); i++) 
    {
      pathtable_key key = dart_path_map_[i];

      vcl_cout << "key: " << key.first.first << ", " << key.first.second << " " << key.second.first << ", " << key.second.second << ":\n";
      
      //dbskr_scurve_sptr sc1 = tree1_->get_curve(key.first.first, key.first.second, contstruct_circular_ends_);
      //dbskr_scurve_sptr sc2 = tree2_->get_curve(key.second.first, key.second.second, contstruct_circular_ends_);
      dbskr_sc_pair_sptr curve_pair1 = tree1_->get_curve_pair(key.first.first, key.first.second, edit_params_.circular_ends_);
      dbskr_sc_pair_sptr curve_pair2 = tree2_->get_curve_pair(key.second.first, key.second.second, edit_params_.circular_ends_);
      dbskr_scurve_sptr sc1 = curve_pair1->coarse;
      dbskr_scurve_sptr sc2 = curve_pair2->coarse;

      vcl_cout << "\t sc1: " << sc1->num_points() << " pts, + bnd len: " << sc1->boundary_plus_length() << " - bnd len: " << sc1->boundary_minus_length() << " sh len: " << sc1->arclength(sc1->num_points()-1) << vcl_endl;
      vcl_cout << "\t sc2: " << sc2->num_points() << " pts, + bnd len: " << sc2->boundary_plus_length() << " - bnd len: " << sc2->boundary_minus_length() << " sh len: " << sc2->arclength(sc2->num_points()-1) << vcl_endl;
      vcl_cout << "\t sc1 dense: " << curve_pair1->dense->num_points() << " pts, + bnd len: " << curve_pair1->dense->boundary_plus_length() << " - bnd len: " << curve_pair1->dense->boundary_minus_length() << " sh len: " << curve_pair1->dense->arclength(curve_pair1->dense->num_points()-1) << vcl_endl;
      vcl_cout << "\t sc2 dense: " << curve_pair2->dense->num_points() << " pts, + bnd len: " << curve_pair2->dense->boundary_plus_length() << " - bnd len: " << curve_pair2->dense->boundary_minus_length() << " sh len: " << curve_pair2->dense->arclength(curve_pair2->dense->num_points()-1) << vcl_endl;

      curve_list1_.push_back(sc1);
      curve_list2_.push_back(sc2);

      float match_cost, dr, alp;
      vcl_vector<vcl_pair<int,int> > fmap;
      if (edit_params_.combined_edit_) {
        dbskr_dpmatch_combined d(sc1, sc2);
        d.set_R(edit_params_.curve_matching_R_);
        d.Match();        
        
        fmap=*(d.finalMap());
        map_list_.push_back(fmap);
        
        dr = d.init_dr(); alp = d.init_phi();
        match_cost = float(d.finalCost());
        
      } else {
        dbskr_dpmatch d(sc1, sc2);
        d.set_R(edit_params_.curve_matching_R_);
        d.Match();
        
        fmap=*(d.finalMap());
        map_list_.push_back(fmap);
        
        dr = d.init_dr(); alp = d.init_phi();
        match_cost = float(d.finalCost());
      }

      match_costs_.push_back(match_cost + dr + alp);  // cost used in edit distance
      match_costs_d_.push_back(match_cost);

      if (edit_params_.localized_edit_ && fmap.size()>3) {
          dbskr_localize_match lmatch(sc1, sc2, 
                                      curve_pair1->dense, curve_pair2->dense, 
                                      curve_pair1->c_d_map, curve_pair2->c_d_map, 
                                      fmap, (float)edit_params_.curve_matching_R_);
          lmatch.match();
          match_cost = lmatch.finalCost();
          match_costs_localized_.push_back(lmatch.finalCost() + dr + alp);
      }

      final_cost_ += match_cost + dr + alp;

      float tree1_sp_cost = tree1_->get_splice_cost_for_merge(key.first.first, key.first.second, tree1_used_darts);
      final_cost_ += tree1_sp_cost;
      float tree2_sp_cost = tree2_->get_splice_cost_for_merge(key.second.first, key.second.second, tree2_used_darts);
      final_cost_ += tree2_sp_cost;
      
      match_costs_init_dr_.push_back(dr);
      match_costs_init_alp_.push_back(alp);

      vcl_cout << "\t\t match cost: " << match_costs_d_[match_costs_d_.size()-1] << " init dr: " << dr << " init phi: " << alp << " total: " << match_costs_d_[match_costs_d_.size()-1] + dr + alp;
      if (edit_params_.localized_edit_ && fmap.size() > 3)
        vcl_cout << " total localized: " << match_costs_localized_[match_costs_localized_.size()-1] << vcl_endl;
      else
        vcl_cout << vcl_endl;
      vcl_cout << "\t\t splice cost for merge in tree1: " << tree1_sp_cost << " in tree2: " << tree2_sp_cost << vcl_endl;
    }

    float tree1_contract_cost = tree1_->get_contract_cost(tree1_used_darts);
    final_cost_ += tree1_contract_cost;
    float tree2_contract_cost = tree2_->get_contract_cost(tree2_used_darts);
    final_cost_ += tree2_contract_cost;
    vcl_cout << "\t\t total contract costs for tree1: " << tree1_contract_cost << " in tree2: " << tree2_contract_cost << vcl_endl;
    vcl_cout << "new final_cost_: " << final_cost_ << vcl_endl;
    vcl_cout << "previous final_norm_cost_: " << final_norm_cost_ << " new final_norm_cost_: ";
    final_norm_cost_ = final_cost_/(tree1_->total_splice_cost()+tree2_->total_splice_cost());
    vcl_cout << "tree1 total splice: " << tree1_->total_splice_cost() << " tree2 total splice: " << tree2_->total_splice_cost() << vcl_endl;
    vcl_cout << final_norm_cost_ << vcl_endl;
    fine_final_norm_costs_computed_ = true;
  } else if (!curve_list1_.size() || !curve_list2_.size()) 
  {  // matches were also read from the binary file, or the match file was not cleared intentionally for recomputation
    curve_list1_.clear();
    curve_list2_.clear();
    for (unsigned int i = 0; i<dart_path_map_.size(); i++) 
    {
      pathtable_key key = dart_path_map_[i];
      
      dbskr_scurve_sptr sc1 = tree1_->get_curve(key.first.first, key.first.second, edit_params_.circular_ends_);
      dbskr_scurve_sptr sc2 = tree2_->get_curve(key.second.first, key.second.second, edit_params_.circular_ends_);

      curve_list1_.push_back(sc1);
      curve_list2_.push_back(sc2);
    }
  }
  
  } else   // either the trees are not available or its already recovered
    return false;
  
  return true;
}


//: Binary save self to stream.
void dbskr_sm_cor::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());

  vsl_b_write(os, fine_final_norm_costs_computed_);
  vsl_b_write(os, final_norm_cost_);
  vsl_b_write(os, final_cost_);
  //vsl_b_write(os, contstruct_circular_ends_);
  edit_params_.b_write(os);
  vsl_b_write(os, dart_path_map_.size());
  
  for (unsigned i = 0; i < dart_path_map_.size(); i++) {
    pathtable_key key = dart_path_map_[i];
    //: save the dart ids
    vsl_b_write(os, key.first.first); vsl_b_write(os, key.first.second);
    vsl_b_write(os, key.second.first); vsl_b_write(os, key.second.second);
  }
  
  //vcl_vector<vcl_vector < vcl_pair <int,int> > > map_list_;
  vsl_b_write(os, map_list_.size());
  for (unsigned i = 0; i < map_list_.size(); i++) {
    vsl_b_write(os, map_list_[i].size());
    for (unsigned j = 0; j < map_list_[i].size(); j++) {
      vsl_b_write(os, map_list_[i][j].first);
      vsl_b_write(os, map_list_[i][j].second);
    }
  }
  return;
}

//: Binary load self from stream.
void dbskr_sm_cor::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
  case 4:
    {
      vsl_b_read(is, fine_final_norm_costs_computed_);
      vsl_b_read(is, final_norm_cost_);
      vsl_b_read(is, final_cost_);
      edit_params_.b_read(is);

      unsigned cnt;
      vsl_b_read(is, cnt);
      for (unsigned i = 0; i < cnt; i++) {
       pathtable_key key;
       vsl_b_read(is, key.first.first); vsl_b_read(is, key.first.second); 
       vsl_b_read(is, key.second.first); vsl_b_read(is, key.second.second); 
       dart_path_map_.push_back(key);
      }
      vsl_b_read(is, cnt);
      for (unsigned i = 0; i < cnt; i++) {
       vcl_vector < vcl_pair <int,int> > v;
       unsigned cnt2;
       vsl_b_read(is, cnt2);
       for (unsigned j = 0; j < cnt2; j++) {
         vcl_pair <int,int> p;
         vsl_b_read(is, p.first); vsl_b_read(is, p.second);
         v.push_back(p);
       }
       map_list_.push_back(v);
      }
      dart_path_map_has_node_ids_ = false;
      recover_dart_ids_and_scurves();  // if the trees are not available at this stage, this method will return not doing anything
      break;
    }
  case 3:
      vsl_b_read(is, fine_final_norm_costs_computed_);
    case 2:
      vsl_b_read(is, final_norm_cost_);
    case 1:
      {
       vsl_b_read(is, final_cost_);
       vsl_b_read(is, edit_params_.circular_ends_);
       unsigned cnt;
       vsl_b_read(is, cnt);
       for (unsigned i = 0; i < cnt; i++) {
        pathtable_key key;
        vsl_b_read(is, key.first.first); vsl_b_read(is, key.first.second); 
        vsl_b_read(is, key.second.first); vsl_b_read(is, key.second.second); 
        
        dart_path_map_.push_back(key);
       }
       vsl_b_read(is, cnt);
       for (unsigned i = 0; i < cnt; i++) {
        vcl_vector < vcl_pair <int,int> > v;
        unsigned cnt2;
        vsl_b_read(is, cnt2);
        for (unsigned j = 0; j < cnt2; j++) {
          vcl_pair <int,int> p;
          vsl_b_read(is, p.first); vsl_b_read(is, p.second);
          v.push_back(p);
        }
        map_list_.push_back(v);
       }
       dart_path_map_has_node_ids_ = false;
       recover_dart_ids_and_scurves();  // if the trees are not available at this stage, this method will return not doing anything
       break;
      }
  }
}

void dbskr_sm_cor::save_pathtable(vcl_map<pathtable_key, float>& table)
{ 
  pathtable_ = table; 
  pathtable_saved_ = true;
}

//: query into the table
//  CAUTION: the key should be formed by NODE IDS (not dart ids)
float dbskr_sm_cor::get_pathtable_val(pathtable_key key)
{
  if (!pathtable_saved_)
    return -1;

  vcl_map<pathtable_key, float>::iterator iter = pathtable_.find(key);   // searching for either key or key2 is enough
  if (iter == pathtable_.end()) {  // recompute
    return -1;
  }
  else
    return iter->second;
}


//: Binary save self to stream.
void dbskr_tree_edit_params::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, scurve_sample_ds_);
  vsl_b_write(os, scurve_interpolate_ds_);
  vsl_b_write(os, localized_edit_);
  vsl_b_write(os, elastic_splice_cost_);
  vsl_b_write(os, curve_matching_R_);
  vsl_b_write(os, circular_ends_);
  vsl_b_write(os, combined_edit_);
  vsl_b_write(os, coarse_edit_);
  return;
}

//: Binary load self from stream.
void dbskr_tree_edit_params::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        vsl_b_read(is, scurve_sample_ds_);
        vsl_b_read(is, scurve_interpolate_ds_);
        vsl_b_read(is, localized_edit_);
        vsl_b_read(is, elastic_splice_cost_);
        vsl_b_read(is, curve_matching_R_);
        vsl_b_read(is, circular_ends_);
        vsl_b_read(is, combined_edit_);
        vsl_b_read(is, coarse_edit_);
       break;
      }
  }
}

