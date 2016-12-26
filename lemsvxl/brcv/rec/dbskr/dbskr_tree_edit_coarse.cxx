#include <dbskr/dbskr_tree_edit_coarse.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_sm_cor.h>

//  In addition doing a coarse alignment another
//  difference between normal edit distance and this version is that:
//  all the shock curves are treated as non-leaf scurves so that circular completions 
//  at the end nodes are eliminated
float dbskr_tree_edit_coarse::get_cost(int td1, int d1, int td2, int d2) {
  
  pathtable_key key;
  key.first.first = tree1_->tail(td1);
  key.first.second = tree1_->head(d1);
  key.second.first = tree2_->tail(td2);
  key.second.second = tree2_->head(d2);

  pathtable_key key2;
  key2.first.first = key.first.second;
  key2.first.second = key.first.first;
  key2.second.first = key.second.second;
  key2.second.second = key.second.first;

  float match_cost;
  vcl_map<pathtable_key, float>::iterator iter;
  iter = pathtable_.find(key);   // searching for either key or key2 is enough
  if (iter == pathtable_.end()) {

    //: passing the construct circular ends flag as false, so the scurves won't have circular completions at the ends
    //  even if they are end nodes
    dbskr_scurve_sptr sc1 = tree1_->get_curve(td1, d1, construct_circular_ends_);
    dbskr_scurve_sptr sc2 = tree2_->get_curve(td2, d2, construct_circular_ends_);

    dbskr_dpmatch d(sc1, sc2);

    // test if it is worth computing this match
    double approx_cost = d.approx_cost();
    
    // Amir: This is risky: it should be a function of the size of the shapes
    if (approx_cost > VERY_LARGE_BOUNDARY_COST)
      match_cost = LARGE;
    else if (approx_cost > LARGE_BOUNDARY_COST)
      match_cost = float(4*approx_cost);
    else {

      //: test if it worths computing this match
      float cost1 = tree1_->get_splice_cost_for_merge(td1, d1);
      float cost2 = tree2_->get_splice_cost_for_merge(td2, d2);
      if (cost1 < LARGE_COST_RATIO*tree1_->total_splice_cost() && cost2 < LARGE_COST_RATIO*tree2_->total_splice_cost()) {
        
        match_cost = d.coarse_match();  // (includes init_dr and init_phi)
        vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());

        //: shock curve map is using darts as key
        pathtable_key key_scm;
        key_scm.first.first = td1;
        key_scm.first.second = d1;
        key_scm.second.first = td2;
        key_scm.second.second = d2;
        pathtable_key key2_scm;
        key2_scm.first.first = key_scm.first.second;
        key2_scm.first.second = key_scm.first.first;
        key2_scm.second.first = key_scm.second.second;
        key2_scm.second.second = key_scm.second.first;
        shock_curve_map_[key_scm] = fmap;
        shock_curve_map_[key2_scm] = fmap;
      } else {
        match_cost = LARGE;
      }
    }

    //: CAUTION we may choose not to cache in the first case as this might make the pathtable huge!!
    pathtable_[key] = match_cost;
    pathtable_[key2] = match_cost;
  
  } else 
    match_cost = iter->second;
    
  return match_cost;
}

dbskr_sm_cor_sptr dbskr_tree_edit_coarse::get_correspondence(bool save_pathtable) {

  dbskr_sm_cor_sptr cor;
  if (switched_) 
    cor = new dbskr_sm_cor(tree2(), tree1());
  else
    cor = new dbskr_sm_cor(tree1(), tree2());

  vcl_vector<dbskr_scurve_sptr>& curve_list1 = cor->get_curve_list1();
  vcl_vector<dbskr_scurve_sptr>& curve_list2 = cor->get_curve_list2();
  vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list = cor->get_map_list();
  vcl_vector< pathtable_key >& path_map = cor->get_map();
  if (!get_correspondence(curve_list1, curve_list2, map_list, path_map))
    return 0;
  
  if (save_pathtable)
    cor->save_pathtable(pathtable_);

  cor->set_final_cost(final_cost_);
  cor->set_tree_edit_params(tree1()->scurve_sample_ds(), tree1()->interpolate_ds(), localize_match_, tree1()->elastic_splice_cost(),
                            construct_circular_ends_, false, true, curve_matching_R_);
  return cor;
}

//: return the corresponding shock curves and their maps
bool dbskr_tree_edit_coarse::get_correspondence_just_map_helper(vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
                                   vcl_vector< pathtable_key >& path_map)
{
  if (final_a1_ < 0) {
    vcl_cout << "Paths are not computed, call edit()!" << vcl_endl;
    return false;
  }

  map_list.clear();

  for (int i = CP_[final_a1_].size()-1; i>=0; i--) {
    //: CP holds darts
    pathtable_key key = CP_[final_a1_][i];

    dbskr_scurve_sptr sc1 = tree1_->get_curve(key.first.first, key.first.second, construct_circular_ends_);
    dbskr_scurve_sptr sc2 = tree2_->get_curve(key.second.first, key.second.second, construct_circular_ends_);

    if (switched_) {
      //: redo the matching
      dbskr_dpmatch d(sc2, sc1);

      d.coarse_match();  // (includes init_dr and init_phi)
      vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());

      map_list.push_back(fmap);

      pathtable_key k;
      k.first.first = key.second.first;
      k.first.second = key.second.second;
      k.second.first = key.first.first;
      k.second.second = key.first.second;
      path_map.push_back(k);

    } else {
      
      path_map.push_back(key);

      vcl_map<pathtable_key, vcl_vector < vcl_pair <int,int> > >::iterator iter;
      //: shock_curve_map_ also uses darts as key (NOT NODES as in pathtable_)
      iter = shock_curve_map_.find(key);   // searching for either key or key2 is enough
      if (iter == shock_curve_map_.end()) {  // redo the matching, its not saved!!
        //vcl_cout << "!!!!!!!!!!!!1MISMATCH IN shock_curve_map_\n";
        dbskr_dpmatch d(sc1, sc2);
        d.coarse_match();
        vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
        map_list.push_back(fmap);

      } else {
        map_list.push_back(iter->second);
      }
    }
  }

  return true;

}
dbskr_sm_cor_sptr dbskr_tree_edit_coarse::get_correspondence_just_map()
{
  dbskr_sm_cor_sptr cor = new dbskr_sm_cor();
  
  vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list = cor->get_map_list();
  vcl_vector< pathtable_key >& path_map = cor->get_map();
  if (!get_correspondence_just_map_helper(map_list, path_map))
    return 0;

  cor->set_final_cost(final_cost_);
  cor->set_tree_edit_params(tree1()->scurve_sample_ds(), tree1()->interpolate_ds(), localize_match_, tree1()->elastic_splice_cost(),
                            construct_circular_ends_, false, true, curve_matching_R_);
  
  return cor;
}
//: return the corresponding shock curves and their maps
bool dbskr_tree_edit_coarse::get_correspondence(vcl_vector<dbskr_scurve_sptr>& curve_list1, 
                        vcl_vector<dbskr_scurve_sptr>& curve_list2, 
                        vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
                        vcl_vector< pathtable_key >& path_map) {
  if (final_a1_ < 0) {
    vcl_cout << "Paths are not computed, call edit()!" << vcl_endl;
    return false;
  }

  curve_list1.clear();
  curve_list2.clear();
  map_list.clear();

  for (int i = CP_[final_a1_].size()-1; i>=0; i--) {
    //: CP holds darts
    pathtable_key key = CP_[final_a1_][i];

    dbskr_scurve_sptr sc1 = tree1_->get_curve(key.first.first, key.first.second, construct_circular_ends_);
    dbskr_scurve_sptr sc2 = tree2_->get_curve(key.second.first, key.second.second, construct_circular_ends_);

    if (switched_) {
      curve_list1.push_back(sc2);
      curve_list2.push_back(sc1);
      //: redo the matching
      dbskr_dpmatch d(sc2, sc1);
      d.coarse_match();
      vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
      map_list.push_back(fmap);

      pathtable_key k;
      k.first.first = key.second.first;
      k.first.second = key.second.second;
      k.second.first = key.first.first;
      k.second.second = key.first.second;
      path_map.push_back(k);

    } else {
      curve_list1.push_back(sc1);
      curve_list2.push_back(sc2);

      path_map.push_back(key);

      vcl_map<pathtable_key, vcl_vector < vcl_pair <int,int> > >::iterator iter;
      //: shock_curve_map_ also uses darts as key (NOT NODES as in pathtable_)
      iter = shock_curve_map_.find(key);   // searching for either key or key2 is enough
      if (iter == shock_curve_map_.end()) {  // redo the matching, its not saved!!
        //vcl_cout << "!!!!!!!!!!!!1MISMATCH IN shock_curve_map_\n";
        dbskr_dpmatch d(sc1, sc2);
        d.coarse_match();
        vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
        map_list.push_back(fmap);

      } else {
        map_list.push_back(iter->second);
      }
    }
  }

  return true;
}

