#include <dbskr/dbskr_tree_edit_pmi.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_dpmatch.h>
#include <vcl_cstdlib.h>
#include <vbl/vbl_array_1d.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_sm_cor.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>

#include <dbru/dbru_rcor.h>
#include <dbru/algo/dbru_rcor_generator.h>

//#include <vcl_algorithm.h> 
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cstdio.h>

#define NEWEPS     float(1e-5)
//#define MIN_OF_THREE(X,Y,Z)   ((X) < (Y) ? ((X) < (Z) ? (X) : (Z)) : ((Y) < (Z) ? (Y) : (Z)))
#define MIN_OF_THREE(X,Y,Z)  (isL((X),(Y),NEWEPS) ? (isL((X),(Z),NEWEPS) ? (X) : (Z)) : (isL((Y),(Z),NEWEPS) ? (Y) : (Z)))
//#define MIN(X,Y)   ((X) < (Y) ? (X) : (Y))
#define MIN(X,Y)   (isL((X),(Y),NEWEPS) ? (X) : (Y))

dbskr_tree_edit_pmi::dbskr_tree_edit_pmi(dbskr_tree_sptr tree1, 
                                         dbskr_tree_sptr tree2,                   
                                         dbinfo_observation_sptr obs1,
                                         dbinfo_observation_sptr obs2) : dbskr_tree_edit(tree1, tree2)
{
  obs1_ = obs1;
  obs2_ = obs2;
  rcor_ = new dbru_rcor(obs1, obs2);
  sm_cor_ = new dbskr_sm_cor(this->tree1(), this->tree2());

  LARGE = (-1e7);
  MIN_VALUE = 0.0f;
  LARGE_COST_RATIO = 0.3f;
  //LARGE_BOUNDARY_COST = (-150.0f);
  //VERY_LARGE_BOUNDARY_COST = (-300.0f);
}

//  modify this function so that it returns #of corresponding pixels in two regions x mutual info of two regions 
float dbskr_tree_edit_pmi::get_cost(int td1, int d1, int td2, int d2) {
  
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

    dbskr_scurve_sptr sc1 = tree1_->get_curve(td1, d1, construct_circular_ends_);
    dbskr_scurve_sptr sc2 = tree2_->get_curve(td2, d2, construct_circular_ends_);
    dbskr_dpmatch d(sc1, sc2);

    //: test if it worths computing this match
    double approx_cost = d.approx_cost();
    
    if (approx_cost > VERY_LARGE_BOUNDARY_COST)
      match_cost = LARGE;
    else if (approx_cost > LARGE_BOUNDARY_COST)
      match_cost = float(-4*approx_cost);

    else {

      //: test if it worths computing this match
      float cost1 = tree1_->get_splice_cost_for_merge(td1, d1);
      float cost2 = tree2_->get_splice_cost_for_merge(td2, d2);
      if (cost1 < LARGE_COST_RATIO*tree1_->total_splice_cost() && cost2 < LARGE_COST_RATIO*tree2_->total_splice_cost()) {
        //  THIS PART FINDS Correspondence using curve alignment
        
        d.Match();
        vcl_vector<vcl_pair<int,int> > fmap=*(d.finalMap());
        
        sm_cor_->clear_lists();
        sm_cor_->add_to_curve_list1(sc1); sm_cor_->add_to_curve_list2(sc2); sm_cor_->add_to_map_list(fmap);
        rcor_->clear_region_correspondence();
        dbru_rcor_generator::find_correspondence_shock(rcor_,sm_cor_);
        
        // Assume linear correspondence of shock curves and find region correspondence
        /*vcl_vector<vcl_pair<int,int> > fmap; 
        fmap.push_back(vcl_pair<int, int>(0,0));
        fmap.push_back(vcl_pair<int, int>(sc1->num_points()-1,sc2->num_points()-1));
        rcor_->clear_region_correspondence();
        dbru_rcor_generator::find_correspondence_shock(rcor_, sc1, sc2);
        */
        vcl_vector<vcl_pair<unsigned, unsigned> >& corrs = rcor_->get_correspondences();
        float info = dbinfo_observation_matcher::minfo(obs1_, obs2_, corrs, false);
        info = (info < 0.0f ? 0.0f:info);
        match_cost = corrs.size()*info;
        vcl_cout << "info: " << info << " corrs size: " << corrs.size() << " assigned " << match_cost << " to path1: " << td1 << " " << d1 << " path2: " << td2 << " " << d2 << "\n";

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

//: if next(d1) is down, there is a left branch of head(d1), try splicing it
float dbskr_tree_edit_pmi::maybe_left_splice_out_tree1_vec(int d1, int d2) {
  int d = tree1_->next(d1);
  if (tree1_->up(d)) {
    AP_[d1][d2] = AP_[d][d2];
    return A_[d][d2];
  }

  //: else d is left sibling of d1
  float val1 = A_[d][d2];
  int parent = tree1_->next(tree1_->mate(d));
  //float val2 = A_[parent][d2] + tree1_->subtree_delete_cost(d);
  float val2 = A_[parent][d2];
  
  if (isL(val1,val2,NEWEPS)) {
    AP_[d1][d2] = AP_[d][d2];
    return val1;
  } else {
    AP_[d1][d2] = AP_[parent][d2];
    return val2;
  }
}
float dbskr_tree_edit_pmi::maybe_left_splice_out_tree1(int d1, int d2) {
  int d = tree1_->next(d1);
  if (tree1_->up(d)) {
    return A_[d][d2];
  }

  //: else d is left sibling of d1
  float val1 = A_[d][d2];
  int parent = tree1_->next(tree1_->mate(d));
  //float val2 = A_[parent][d2] + tree1_->subtree_delete_cost(d);
  float val2 = A_[parent][d2];
  
  return MIN(val1, val2);
}

//: if next(d2) is down, there is a left branch of head(d2), try splicing it
float dbskr_tree_edit_pmi::maybe_left_splice_out_tree2_vec(int d1, int d2) {
  int d = tree2_->next(d2);
  if (tree2_->up(d)) {
    AP_[d1][d2] = AP_[d1][d];
    return A_[d1][d];
  }

  //: else d is left sibling of d2
  float val1 = A_[d1][d];
  int parent = tree2_->next(tree2_->mate(d));
  //float val2 = A_[d1][parent] + tree2_->subtree_delete_cost(d);
  float val2 = A_[d1][parent];
  
  if (isL(val1,val2,NEWEPS)) {
    AP_[d1][d2] = AP_[d1][d];
    return val1;
  } else {
    AP_[d1][d2] = AP_[d1][parent];
    return val2;
  }
}
float dbskr_tree_edit_pmi::maybe_left_splice_out_tree2(int d1, int d2) {
  int d = tree2_->next(d2);
  if (tree2_->up(d)) {
    return A_[d1][d];
  }

  //: else d is left sibling of d2
  float val1 = A_[d1][d];
  int parent = tree2_->next(tree2_->mate(d));
  //float val2 = A_[d1][parent] + tree2_->subtree_delete_cost(d);
  float val2 = A_[d1][parent];
  
  return MIN(val1, val2);
}


float dbskr_tree_edit_pmi::merge_down_tree1(int a1, int a2, int d1, int d2, int td1, int td2) {
  vcl_vector<int>& ch = tree1_->children(d1);
  float min_cost = LARGE;
  // changing the order of children processing
  //for (unsigned int i = 0; i<ch.size(); i++) {
  for (int i = ch.size()-1; i>=0; i--) {
    int d = ch[i];
    float cost = witht(a1, a2, d, d2, td1, td2);
    
    // just add to this cost the cost of splicing siblings of d
    //for (unsigned int j = 0; j<ch.size(); j++) {
    //  if (ch[j] == d) continue;
    //  cost += tree1_->subtree_delete_cost(ch[j]);
   // }

    //if (cost < min_cost) {
    if (isL(cost,min_cost,NEWEPS)) {
      min_cost = cost;
    }
  }
  return min_cost;
}

float dbskr_tree_edit_pmi::merge_down_tree1(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec) {
  
  vcl_vector<int>& ch = tree1_->children(d1);
  float min_cost = LARGE;
  vcl_vector<pathtable_key> tmp;
  vcl_vector<vcl_vector<pathtable_key> > big_tmp(ch.size(), tmp);
  int min_id = -1;
  //for (unsigned int i = 0; i<ch.size(); i++) {
  for (int i = ch.size()-1; i>=0; i--) {
    int d = ch[i];
    //vcl_vector<pathtable_key> tmp;  
    float cost = witht(a1, a2, d, d2, td1, td2, big_tmp[i]);
    
    // just add to this cost the cost of splicing siblings of d
    //for (unsigned int j = 0; j<ch.size(); j++) {
    //  if (ch[j] == d) continue;
    //  cost += tree1_->subtree_delete_cost(ch[j]);
    //}

    //if (cost < min_cost) {
    if (isL(cost,min_cost,NEWEPS)) {
      min_cost = cost;
      //vec = tmp;
      min_id = i;
    }
  }
  if (min_id >= 0)
    vec = big_tmp[min_id];
  return min_cost;
}

float dbskr_tree_edit_pmi::merge_down_tree2(int a1, int a2, int d1, int d2, int td1, int td2) {
  vcl_vector<int>& ch = tree2_->children(d2);
  float min_cost = LARGE;
  for (int i = ch.size()-1; i>=0; i--) {
    int d = ch[i];
    float cost = witht(a1, a2, d1, d, td1, td2);
    
    // just add to this cost the cost of splicing siblings of d
    //for (unsigned int j = 0; j<ch.size(); j++) {
    //  if (ch[j] == d) continue;
    //  cost += tree2_->subtree_delete_cost(ch[j]);
    //}

    //if (cost < min_cost) 
    if (isL(cost,min_cost,NEWEPS)) 
      min_cost = cost;
  }
  return min_cost;
}

float dbskr_tree_edit_pmi::merge_down_tree2(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec) {
  vcl_vector<int>& ch = tree2_->children(d2);
  float min_cost = LARGE;
  vcl_vector<pathtable_key> tmp;
  
  vbl_array_1d<vcl_vector<pathtable_key> > big_tmp(ch.size(), tmp);
  int min_id = -1;
  for (int i = ch.size()-1; i>=0; i--) {
    int d = ch[i];
    float cost = witht(a1, a2, d1, d, td1, td2, big_tmp[i]);
    
    // just add to this cost the cost of splicing siblings of d
    //for (unsigned int j = 0; j<ch.size(); j++) {
    //  if (ch[j] == d) continue;
    //  cost += tree2_->subtree_delete_cost(ch[j]);
    //}

    if (isL(cost,min_cost,NEWEPS)) {
      min_cost = cost;
      min_id = i;
    }
  }
  if (min_id >= 0)
    vec = big_tmp[min_id];
  return min_cost;
}

//: this operation is only legal if d1 has a left sibling d, and if d's head is not a leaf,
//  and d's tail has degree at least three
//
//  This procedure splices out the right branch assuming the parent edge was contracted, not matched.
//  It checks this by checking whether the head of a1 is the tail of d1.
//  After splicing off the right branch, it contracts the remaining child edge d.
//  For this reason, it must also check that d's head is not a leaf.
//  CAUTION: if we restrict contractions to involve high degree, this gets more complicated.
float dbskr_tree_edit_pmi::maybe_right_splice_out_tree1(int a1, int d1, int d2) {
  int d = tree1_->next(tree1_->mate(d1));
  if (!tree1_->up(d) &&                       // if d is up, d1 has no left sibling
      tree1_->head(a1) != tree1_->tail(d1) && // illegal after parent edge matched
      !tree1_->leaf(d)) {                     // d must be contracted, so not a leaf 
    
    //: case: no right splice
    //float val1 = A_[tree1_->next(d1)][d2] + tree1_->contract_cost(d1);
    float val1 = A_[tree1_->next(d1)][d2];
    //: case: right splice the branch of d1 (then contract d, its left sibling)
    //float val2 = A_[tree1_->next(d)][d2] + tree1_->subtree_delete_cost(d1) + tree1_->contract_cost(d);
    float val2 = A_[tree1_->next(d)][d2];
    return MIN(val1, val2);
  
  } else { // right splice is not legal
    //return A_[tree1_->next(d1)][d2] + tree1_->contract_cost(d1);
    return A_[tree1_->next(d1)][d2];
  }
}
vcl_vector<pathtable_key>& dbskr_tree_edit_pmi::maybe_right_splice_out_tree1_vec(int a1, int d1, int d2) {
  int d = tree1_->next(tree1_->mate(d1));
  if (!tree1_->up(d) &&                       // if d is up, d1 has no left sibling
      tree1_->head(a1) != tree1_->tail(d1) && // illegal after parent edge matched
      !tree1_->leaf(d)) {                     // d must be contracted, so not a leaf 
    
    //: case: no right splice
    //float val1 = A_[tree1_->next(d1)][d2] + tree1_->contract_cost(d1);
    float val1 = A_[tree1_->next(d1)][d2];
    //: case: right splice the branch of d1 (then contract d, its left sibling)
    //float val2 = A_[tree1_->next(d)][d2] + tree1_->subtree_delete_cost(d1) + tree1_->contract_cost(d);
    float val2 = A_[tree1_->next(d)][d2];
    
    //return MIN(val1, val2);
    //if (val1 < val2) {
    if (isL(val1,val2,NEWEPS)) {
      return AP_[tree1_->next(d1)][d2];
    } else {
      return AP_[tree1_->next(d)][d2];
    }
  
  } else { // right splice is not legal
    return AP_[tree1_->next(d1)][d2];
  }
}

float dbskr_tree_edit_pmi::maybe_right_splice_out_tree2(int a2, int d2, int d1) {
  int d = tree2_->next(tree2_->mate(d2));
  if (!tree2_->up(d) &&                       // if d is up, d1 has no left sibling
      tree2_->head(a2) != tree2_->tail(d2) && // illegal after parent edge matched
      !tree2_->leaf(d)) {                     // d must be contracted, so not a leaf 
    
    //: case: no right splice
    //float val1 = A_[d1][tree2_->next(d2)] + tree2_->contract_cost(d2);
    float val1 = A_[d1][tree2_->next(d2)];
    //: case: right splice the branch of d2 (then contract d, its left sibling)
    //float val2 = A_[d1][tree2_->next(d)] + tree2_->subtree_delete_cost(d2) + tree2_->contract_cost(d);
    float val2 = A_[d1][tree2_->next(d)];
    
    return MIN(val1, val2);

  } else { // right splice is not legal
    //return A_[d1][tree2_->next(d2)] + tree2_->contract_cost(d2);
      return A_[d1][tree2_->next(d2)];
  }
}
vcl_vector<pathtable_key>& dbskr_tree_edit_pmi::maybe_right_splice_out_tree2_vec(int a2, int d2, int d1) {
  int d = tree2_->next(tree2_->mate(d2));
  if (!tree2_->up(d) &&                       // if d is up, d1 has no left sibling
      tree2_->head(a2) != tree2_->tail(d2) && // illegal after parent edge matched
      !tree2_->leaf(d)) {                     // d must be contracted, so not a leaf 
    
    //: case: no right splice
    //float val1 = A_[d1][tree2_->next(d2)] + tree2_->contract_cost(d2);
    float val1 = A_[d1][tree2_->next(d2)];
    //: case: right splice the branch of d2 (then contract d, its left sibling)
    //float val2 = A_[d1][tree2_->next(d)] + tree2_->subtree_delete_cost(d2) + tree2_->contract_cost(d);
    float val2 = A_[d1][tree2_->next(d)];
    
    //return MIN(val1, val2);
    //if (val1 < val2) {
    if (isL(val1,val2,NEWEPS)) {
      return AP_[d1][tree2_->next(d2)];
    } else {
      return AP_[d1][tree2_->next(d)];
    }

  } else { // right splice is not legal
    return AP_[d1][tree2_->next(d2)];
  }
}

