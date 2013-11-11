#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbskr/dbskr_sm_cor_sptr.h>
#include <dbskr/dbskr_utilities.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_tree_edit_combined.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_scurve.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_box_2d.h>

#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon.h>

#include <bsol/bsol_algs.h>
#include <dbdet/tracer/dbdet_contour_tracer.h>
#include <vgl/vgl_polygon_scan_iterator.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>

#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>

#include <Manifold_extraction/Lie_group_operations.h>

//#include <dbru/algo/dbru_object_matcher.h>

void dbskr_shock_patch_match::clear()
{
   for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      v_ptr->clear();
      delete v_ptr;
   }
   patch_cor_map_.clear();

   for (vcl_map<int, vcl_vector<vcl_pair<int, float> >* >::const_iterator  iter = patch_cor_info_map_.begin(); 
     iter != patch_cor_info_map_.end(); iter++) {
     vcl_vector<vcl_pair<int, float> >* v_ptr = iter->second;
     v_ptr->clear();
     delete v_ptr;
   }
   patch_cor_info_map_.clear();

   map1_.clear();
   map2_.clear();

   patch_set2_.clear();

   //if (left_v_)//: not there before visualization
   // delete left_v_;

   return;
}

void dbskr_shock_patch_match::clear_sm_map_lists()
{
   for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      for (unsigned i = 0; i < v_ptr->size(); i++) {
        (*v_ptr)[i].second->clear_map_list();
      }
   }

   return;
}

void dbskr_shock_patch_match::clear_edit_costs()
{
   for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      delete v_ptr;
   }
   patch_cor_map_.clear();
   return;
}

inline bool
norm_cost_less(const vcl_pair<int, dbskr_sm_cor_sptr>& p1,
               const vcl_pair<int, dbskr_sm_cor_sptr>& p2) {
  return (p1.second->final_norm_cost() < p2.second->final_norm_cost());
}

inline bool
cost_less(const vcl_pair<int, dbskr_sm_cor_sptr>& p1,
          const vcl_pair<int, dbskr_sm_cor_sptr>& p2) {        
  return (p1.second->final_cost() < p2.second->final_cost());
}

//: sort again wrt norm costs
void dbskr_shock_patch_match::resort_wrt_norm_cost()
{
  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), norm_cost_less);
   }

}

//: sort wrt costs
void dbskr_shock_patch_match::resort_wrt_cost()
{
  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), cost_less);
   }
}


//: compute normalized fine costs of the corresponding branches
bool dbskr_shock_patch_match::compute_norm_fine_costs_of_cors()
{
  if (!map1_.size() || !map2_.size()) {
    vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - shock patch id maps are not available!\n";
    return false;
  }

  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      vcl_map<int, dbskr_shock_patch_sptr>::iterator spi = map1_.find(iter->first);
      dbskr_shock_patch_sptr sp1;
      if (spi == map1_.end()) {
        vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - an id in match vector could not be found in map1_\n";
        return false;
      } else
        sp1 = spi->second;

      
      //: recompute each matchings cost
      for (unsigned j = 0; j < (*v_ptr).size(); j++) {

        dbskr_sm_cor_sptr sm = (*v_ptr)[j].second; 

        dbskr_tree_sptr tree1 = sp1->tree();  // compute if not there

        vcl_map<int, dbskr_shock_patch_sptr>::iterator spi2 = map2_.find((*v_ptr)[j].first);
        dbskr_shock_patch_sptr sp2;
        if (spi2 == map2_.end()) {
          vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - an id in match vector could not be found in map2_\n";
          return false;
        } else
          sp2 = spi2->second;

        dbskr_tree_sptr tree2 = sp2->tree();  // compute if not there
        sm->clear_map_list(); // to force recomputation of fine matches (elastic curve matching) between the corresponding branches
        sm->set_tree1(tree1);
        sm->set_tree2(tree2);  // in this method it calls recover_node_ids which recomputes the matches if necessary (fi map_list_ is empty)
      }
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), norm_cost_less);
  }

  return true;
}

//: compute normalized costs of the corresponding branches
//  normalize by the length of the total length of the reconstructed scurves used
bool dbskr_shock_patch_match::compute_length_norm_costs_of_cors()
{
  if (!map1_.size() || !map2_.size()) {
    vcl_cout << "dbskr_shock_patch_match::compute_length_norm_costs_of_cors() - shock patch id maps are not available!\n";
    return false;
  }

  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      vcl_map<int, dbskr_shock_patch_sptr>::iterator spi = map1_.find(iter->first);
      dbskr_shock_patch_sptr sp1;
      if (spi == map1_.end()) {
        vcl_cout << "dbskr_shock_patch_match::compute_length_norm_fine_costs_of_cors() - an id in match vector could not be found in map1_\n";
        return false;
      } else
        sp1 = spi->second;

      
      //: recompute each matchings cost
      for (unsigned j = 0; j < (*v_ptr).size(); j++) {
        dbskr_sm_cor_sptr sm = (*v_ptr)[j].second; 

        dbskr_tree_sptr tree1 = sp1->tree();  // compute if not there

        vcl_map<int, dbskr_shock_patch_sptr>::iterator spi2 = map2_.find((*v_ptr)[j].first);
        dbskr_shock_patch_sptr sp2;
        if (spi2 == map2_.end()) {
          vcl_cout << "dbskr_shock_patch_match::compute_length_norm_costs_of_cor() - an id in match vector could not be found in map2_\n";
          return false;
        } else
          sp2 = spi2->second;

        dbskr_tree_sptr tree2 = sp2->tree();  // compute if not there
        sm->set_final_norm_cost(sm->final_cost()/(tree1->total_reconstructed_boundary_length()+tree2->total_reconstructed_boundary_length()));
      }
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), norm_cost_less);
  }
  return true;
}

//: compute normalized costs of the corresponding branches
//  normalize by the sum of total splice costs of the trees
bool dbskr_shock_patch_match::compute_splice_norm_costs_of_cors()
{
  if (!map1_.size() || !map2_.size()) {
    vcl_cout << "dbskr_shock_patch_match::compute_splice_norm_costs_of_cors() - shock patch id maps are not available!\n";
    return false;
  }

  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      vcl_map<int, dbskr_shock_patch_sptr>::iterator spi = map1_.find(iter->first);
      dbskr_shock_patch_sptr sp1;
      if (spi == map1_.end()) {
        vcl_cout << "dbskr_shock_patch_match::compute_splice_norm_costs_of_cors() - an id in match vector could not be found in map1_\n";
        return false;
      } else
        sp1 = spi->second;

      
      //: recompute each matchings cost
      for (unsigned j = 0; j < (*v_ptr).size(); j++) {
        dbskr_sm_cor_sptr sm = (*v_ptr)[j].second; 

        dbskr_tree_sptr tree1 = sp1->tree();  // compute if not there

        vcl_map<int, dbskr_shock_patch_sptr>::iterator spi2 = map2_.find((*v_ptr)[j].first);
        dbskr_shock_patch_sptr sp2;
        if (spi2 == map2_.end()) {
          vcl_cout << "dbskr_shock_patch_match::compute_splice_norm_costs_of_cor() - an id in match vector could not be found in map2_\n";
          return false;
        } else
          sp2 = spi2->second;

        dbskr_tree_sptr tree2 = sp2->tree();  // compute if not there
        sm->set_final_norm_cost(sm->final_cost()/(tree1->total_splice_cost()+tree2->total_splice_cost()));
      }
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), norm_cost_less);
  }
  return true;
}


//: compute normalized fine costs of the corresponding branches
//  after finding the bug in total_splice_cost_ computation method of tree (in find_subtree_delete_cost() method)
bool dbskr_shock_patch_match::compute_just_norm_fine_costs_of_cors()
{
  if (!map1_.size() || !map2_.size()) {
    vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - shock patch id maps are not available!\n";
    return false;
  }

  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      vcl_map<int, dbskr_shock_patch_sptr>::iterator spi = map1_.find(iter->first);
      dbskr_shock_patch_sptr sp1;
      if (spi == map1_.end()) {
        vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - an id in match vector could not be found in map1_\n";
        return false;
      } else
        sp1 = spi->second;

      
      //: recompute each matchings cost
      for (unsigned j = 0; j < (*v_ptr).size(); j++) {
        dbskr_sm_cor_sptr sm = (*v_ptr)[j].second; 

        dbskr_tree_sptr tree1 = sp1->tree();  // compute if not there

        vcl_map<int, dbskr_shock_patch_sptr>::iterator spi2 = map2_.find((*v_ptr)[j].first);
        dbskr_shock_patch_sptr sp2;
        if (spi2 == map2_.end()) {
          vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - an id in match vector could not be found in map2_\n";
          return false;
        } else
          sp2 = spi2->second;

        dbskr_tree_sptr tree2 = sp2->tree();  // compute if not there
        sm->set_final_norm_cost(sm->final_cost()/(tree1->total_splice_cost()+tree2->total_splice_cost()));
      }
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), norm_cost_less);
  }

  return true;
}

//: change the norm costs in sm_cor's so that the value reflects the relative significance of this
//  similarity value when the total area of the matching patches are concerned
bool dbskr_shock_patch_match::weight_norm_cost_wrt_area()
{
  if (!map1_.size() || !map2_.size()) {
    vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - shock patch id maps are not available!\n";
    return false;
  }

  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
      vcl_map<int, dbskr_shock_patch_sptr>::iterator spi = map1_.find(iter->first);
      dbskr_shock_patch_sptr sp1;
      if (spi == map1_.end()) {
        vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - an id in match vector could not be found in map1_\n";
        return false;
      } else
        sp1 = spi->second;

      
      //: recompute each matchings cost
      for (unsigned j = 0; j < (*v_ptr).size(); j++) {
        dbskr_sm_cor_sptr sm = (*v_ptr)[j].second; 

        dbskr_tree_sptr tree1 = sp1->tree();  // compute if not there

        vcl_map<int, dbskr_shock_patch_sptr>::iterator spi2 = map2_.find((*v_ptr)[j].first);
        dbskr_shock_patch_sptr sp2;
        if (spi2 == map2_.end()) {
          vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - an id in match vector could not be found in map2_\n";
          return false;
        } else
          sp2 = spi2->second;

        dbskr_tree_sptr tree2 = sp2->tree();  // compute if not there
        sm->clear_map_list(); // to force recomputation of fine matches (elastic curve matching) between the corresponding branches
        sm->set_tree1(tree1);
        sm->set_tree2(tree2);  // in this method it calls recover_node_ids which recomputes the matches if necessary (fi map_list_ is empty)
      }
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), norm_cost_less);
  }

  return true;

}

vcl_pair<int, dbskr_sm_cor_sptr>& dbskr_shock_patch_match::get_best_match(int patch_id) 
{
  patch_cor_map_iterator iter = patch_cor_map_.find(patch_id);
  if (iter == patch_cor_map_.end()) {
    vcl_pair<int, dbskr_sm_cor_sptr> *p = new vcl_pair<int, dbskr_sm_cor_sptr>(-1, (dbskr_sm_cor*) 0);
    vcl_cout << "FOR THIS PATCH: " << patch_id << " NO MATCHES ARE AVAILABLE!!!!!\n";
    return (*p);
  } else {
    if ((*(iter->second)).size() > 0)
      return (*(iter->second))[0]; 
    else {
      vcl_pair<int, dbskr_sm_cor_sptr> *p = new vcl_pair<int, dbskr_sm_cor_sptr>(-1, (dbskr_sm_cor*) 0);
      vcl_cout << "FOR THIS PATCH: " << patch_id << " there exists a vector in the map but it is empty!! NO MATCHES ARE AVAILABLE!!!!!\n";
      return (*p);
    }
  }
}

//: get the best match of the patch with this id
vcl_pair<int, float>& dbskr_shock_patch_match::get_best_match_info(int patch_id) 
{ 
  patch_cor_info_map_iterator iter = patch_cor_info_map_.find(patch_id);
  if (iter == patch_cor_info_map_.end()) {
    vcl_pair<int, float> *p = new vcl_pair<int, float>(-1, 0);
    vcl_cout << "FOR THIS PATCH: " << patch_id << " NO MATCHES ARE AVAILABLE!!!!!\n";
    return (*p);
  } else
    return (*(iter->second))[0]; 
}

  //: get the top n best match of the patch with this id
vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >* dbskr_shock_patch_match::get_best_n_match(int patch_id, int n)
{
  vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >* temp = new vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >();
  temp->insert(temp->begin(), (*patch_cor_map_[patch_id]).begin(), (*patch_cor_map_[patch_id]).begin()+n);
  return temp;
}

//: construct the match for the second image, does not sort wrt any type of cost
dbskr_shock_patch_match_sptr dbskr_shock_patch_match::construct_match_just_cost()
{
  dbskr_shock_patch_match_sptr new_match = new dbskr_shock_patch_match();
  new_match->edit_params_ = edit_params_;
  //new_match->elastic_splice_cost_ = elastic_splice_cost_;
  //new_match->scurve_sample_ds_ = scurve_sample_ds_;
  new_match->shock_pruning_threshold_ = shock_pruning_threshold_;

  patch_cor_map_type& new_map = new_match->get_map();
  patch_cor_map_iterator it;
  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v = (iter->second);
      for (unsigned i = 0; i < v->size(); i++) {
        it = new_map.find((*v)[i].first);
        if (it == new_map.end()) { // insert it
          vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* new_v = new vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >();
          vcl_pair<int, dbskr_sm_cor_sptr> p;
          p.first = iter->first;
          p.second = (*v)[i].second;  // the scurve correspondence is totally wrong but the cost is fine
          new_v->push_back(p);
          new_map[(*v)[i].first] = new_v;
        } else {
          vcl_pair<int, dbskr_sm_cor_sptr> p;
          p.first = iter->first;
          p.second = (*v)[i].second;  // the scurve correspondence is totally wrong but the cost is find
          (it->second)->push_back(p);
        }
      }
  }

  patch_cor_info_map_type& new_info_map = new_match->get_info_map();
  patch_cor_info_map_iterator itr;
  for (vcl_map<int, vcl_vector<vcl_pair<int, float> >* >::const_iterator iter = patch_cor_info_map_.begin();
    iter != patch_cor_info_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, float> >* v = (iter->second);
      for (unsigned i = 0; i < v->size(); i++) {
        itr = new_info_map.find((*v)[i].first);
        if (itr == new_info_map.end()) { // insert it
          vcl_vector<vcl_pair<int, float> >* new_v = new vcl_vector<vcl_pair<int, float> >();
          vcl_pair<int, float> p;
          p.first = iter->first;
          p.second = (*v)[i].second;  
          new_v->push_back(p);
          new_info_map[(*v)[i].first] = new_v;
        } else {
          vcl_pair<int, float> p;
          p.first = iter->first;
          p.second = (*v)[i].second;  
          (itr->second)->push_back(p);
        }
      }
  }

  return new_match;
}

//: construct the match for the second set of patches, create new instances of dbskr_sm_cors with pathtable_key's reversed accordingly
dbskr_shock_patch_match_sptr dbskr_shock_patch_match::construct_match()
{
  dbskr_shock_patch_match_sptr new_match = new dbskr_shock_patch_match();
  new_match->edit_params_ = edit_params_;
  //new_match->elastic_splice_cost_ = elastic_splice_cost_;
  //new_match->scurve_sample_ds_ = scurve_sample_ds_;
  new_match->shock_pruning_threshold_ = shock_pruning_threshold_;
 
  patch_cor_map_type& new_map = new_match->get_map();
  patch_cor_map_iterator it;
  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v = (iter->second);
      for (unsigned i = 0; i < v->size(); i++) {
        it = new_map.find((*v)[i].first);
        if (it == new_map.end()) { // insert it
          vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* new_v = new vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >();
          vcl_pair<int, dbskr_sm_cor_sptr> p;
          p.first = iter->first;
          p.second = (*v)[i].second->new_sm_reverse_maps();  // the scurve correspondences reversed
          new_v->push_back(p);
          new_map[(*v)[i].first] = new_v;
        } else {
          vcl_pair<int, dbskr_sm_cor_sptr> p;
          p.first = iter->first;
          p.second = (*v)[i].second->new_sm_reverse_maps();  // the scurve correspondences reversed
          (it->second)->push_back(p);
        }
      }
  }
  resort_wrt_norm_cost();

  patch_cor_info_map_type& new_info_map = new_match->get_info_map();
  patch_cor_info_map_iterator itr;
  for (vcl_map<int, vcl_vector<vcl_pair<int, float> >* >::const_iterator iter = patch_cor_info_map_.begin();
    iter != patch_cor_info_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, float> >* v = (iter->second);
      for (unsigned i = 0; i < v->size(); i++) {
        itr = new_info_map.find((*v)[i].first);
        if (itr == new_info_map.end()) { // insert it
          vcl_vector<vcl_pair<int, float> >* new_v = new vcl_vector<vcl_pair<int, float> >();
          vcl_pair<int, float> p;
          p.first = iter->first;
          p.second = (*v)[i].second;  
          new_v->push_back(p);
          new_info_map[(*v)[i].first] = new_v;
        } else {
          vcl_pair<int, float> p;
          p.first = iter->first;
          p.second = (*v)[i].second;  
          (itr->second)->push_back(p);
        }
      }
  }

  return new_match;
}

//: Binary save self to stream.
void dbskr_shock_patch_match::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, patch_cor_map_.size());
  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {
    vsl_b_write(os, iter->first);
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v = (iter->second);
    vsl_b_write(os, v->size());
    for (unsigned i = 0; i < v->size(); i++) {
      vsl_b_write(os, (*v)[i].first);
      ((*v)[i].second)->b_write(os);
    }
  }

  vsl_b_write(os, shock_pruning_threshold_);
  //vsl_b_write(os, scurve_sample_ds_);
  //vsl_b_write(os, elastic_splice_cost_);
  edit_params_.b_write(os);

  return;
}

//: Binary load self from stream.
void dbskr_shock_patch_match::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
  case 2: {
        unsigned cnt;
        vsl_b_read(is, cnt);
        for (unsigned i = 0; i < cnt; i++) {
          int id1;
          vsl_b_read(is, id1);
          vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> > *v = new vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >();
          unsigned v_cnt;
          vsl_b_read(is, v_cnt);
          for (unsigned j = 0; j < v_cnt; j++) {
            vcl_pair<int, dbskr_sm_cor_sptr> pp;
            vsl_b_read(is, pp.first);
            dbskr_sm_cor_sptr sm = new dbskr_sm_cor();
            sm->b_read(is);
            pp.second = sm;
            v->push_back(pp);
          }
          patch_cor_map_[id1] = v;
        }
        
        vsl_b_read(is, shock_pruning_threshold_);
        //vsl_b_read(is, scurve_sample_ds_);
        //vsl_b_read(is, elastic_splice_cost_);
        edit_params_.b_read(is);
        break;
          }
    case 1:
      {
        unsigned cnt;
        vsl_b_read(is, cnt);
        for (unsigned i = 0; i < cnt; i++) {
          int id1;
          vsl_b_read(is, id1);
          vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> > *v = new vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >();
          unsigned v_cnt;
          vsl_b_read(is, v_cnt);
          for (unsigned j = 0; j < v_cnt; j++) {
            vcl_pair<int, dbskr_sm_cor_sptr> pp;
            vsl_b_read(is, pp.first);
            dbskr_sm_cor_sptr sm = new dbskr_sm_cor();
            sm->b_read(is);
            pp.second = sm;
            v->push_back(pp);
          }
          patch_cor_map_[id1] = v;
        }
        
        vsl_b_read(is, shock_pruning_threshold_);
        vsl_b_read(is, edit_params_.scurve_sample_ds_);
        vsl_b_read(is, edit_params_.elastic_splice_cost_);
        
        break;
      }
  }
}


inline bool
info_max(const vcl_pair<int, float>& p1, const vcl_pair<int, float>& p2) {
  return (p1.second > p2.second);
}

//: sort again wrt infos
void dbskr_shock_patch_match::resort_wrt_info()
{
  for (vcl_map<int, vcl_vector<vcl_pair<int, float> >* >::const_iterator iter = patch_cor_info_map_.begin(); 
    iter != patch_cor_info_map_.end(); iter++) {
      vcl_vector<vcl_pair<int, float> >* v_ptr = iter->second;
      vcl_sort((*v_ptr).begin(), (*v_ptr).end(), info_max);
   }
}


/*
//: mutual info methods
bool dbskr_shock_patch_match::compute_mutual_infos(void)
{
  if (!map1_.size() || !map2_.size()) {
    vcl_cout << "dbskr_shock_patch_match::compute_mutual_infos() - shock patch id maps are not available!\n";
    return false;
  }

  for (vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::const_iterator iter = patch_cor_map_.begin(); 
    iter != patch_cor_map_.end(); iter++) {

    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* v_ptr = iter->second;
    vcl_map<int, dbskr_shock_patch_sptr>::iterator spi = map1_.find(iter->first);
    dbskr_shock_patch_sptr sp1;
    if (spi == map1_.end()) {
      vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - an id in match vector could not be found in map1_\n";
      return false;
    } else
      sp1 = spi->second;


    dbinfo_observation_sptr obs1 = sp1->observation();  // compute if not there assuming images are set!!

    vcl_vector<vcl_pair<int, float> >* info_costs = new vcl_vector<vcl_pair<int, float> >();
    //: recompute each matchings cost
    for (unsigned j = 0; j < (*v_ptr).size(); j++) {
      dbskr_sm_cor_sptr sm = (*v_ptr)[j].second; 

      dbskr_tree_sptr tree1 = sp1->tree(sm->contstruct_circular_ends_, sm->use_dpmatch_combined_);  // compute if not there

      vcl_map<int, dbskr_shock_patch_sptr>::iterator spi2 = map2_.find((*v_ptr)[j].first);
      dbskr_shock_patch_sptr sp2;
      if (spi2 == map2_.end()) {
        vcl_cout << "dbskr_shock_patch_match::compute_norm_fine_costs_of_cor() - an id in match vector could not be found in map2_\n";
        return false;
      } else
        sp2 = spi2->second;

      dbskr_tree_sptr tree2 = sp2->tree(sm->contstruct_circular_ends_, sm->use_dpmatch_combined_);  // compute if not there
      sm->set_tree1(tree1);
      sm->set_tree2(tree2);  // in this method it calls recover_node_ids which recomputes the shock curves

      dbinfo_observation_sptr obs2 = sp2->observation();  // compute if not there assuming images are set!!
      double info_shock = 0;
      vil_image_resource_sptr img = dbru_object_matcher::minfo_shock_matching(obs1, obs2, sm, info_shock, false);
      //vcl_ostringstream oss1, oss2, oss3;
      //oss1 << sp1->id();  oss2 << sp2->id(); oss3 << info_shock;
      //vil_save_image_resource(img, vcl_string("c:\\temp\\"+oss1.str()+"-"+oss2.str()+"-"+oss3.str()+".png").c_str());
      info_costs->push_back(vcl_pair<int, float>(sp2->id(), float(info_shock)));
    }
    vcl_sort((*info_costs).begin(), (*info_costs).end(), info_max);
    patch_cor_info_map_[sp1->id()] = info_costs;
  }
  
  return true;

}

*/

bool dbskr_shock_patch_match::compute_similarity_transformations(int sampling_interval) {
  
  //: need to set the trees first
  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;

  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  for ( ; iter != patch_cor_map_.end(); iter++) {
    int id1 = iter->first;
    
    vcl_map<int, dbskr_shock_patch_sptr>::iterator it = map1_.find(id1);
    if (it == map1_.end()) {
      vcl_cout << " set the id to shock patch sptr maps in the match instance\n";
      return false;
    }
    dbskr_shock_patch_sptr sp1 = it->second;

    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
        
    if (train_patch_vec == 0)  // this model patch does not qualify as it does not have at least N matches
      continue;
    for (unsigned iii = 0; iii < train_patch_vec->size(); iii++) {
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];

      vcl_map<int, dbskr_shock_patch_sptr>::iterator it2 = map2_.find(p.first);
      if (it2 == map2_.end()) {
        vcl_cout << " set the id to shock patch sptr maps in the match instance\n";
        return false;
      }
      dbskr_shock_patch_sptr sp2 = it2->second;

      p.second->set_tree1(sp1->tree()); // assuming tree parameters are already set properly
      p.second->set_tree2(sp2->tree()); // assuming tree parameters are already set properly  // all the maps are recomputed based on dart correspondence

      vgl_h_matrix_2d<double> dummy_H; 
      if (!p.second->compute_similarity2D(dummy_H, true, sampling_interval, false, true))  // saving the transformation in dbskr_sm_cor
        vcl_cout << "Could not compute sim trans!!!!!!!\n";
      
      p.second->kill_trees();

      sp2->kill_tree();
    }
    sp1->kill_tree();
  }

  return true;
}
bool dbskr_shock_patch_match::clear_similarity_transformations() {
  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;

  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  for ( ; iter != patch_cor_map_.end(); iter++) {
    int id1 = iter->first;
    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
        
    if (train_patch_vec == 0)  // this model patch does not qualify as it does not have at least N matches
      continue;

    for (unsigned iii = 0; iii < train_patch_vec->size(); iii++) {
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];
      p.second->clear_transformation();
    }
  }

  return true;
}

inline bool
Lie_dist_less(const vcl_pair< vcl_pair<int, int>, double >& p1,
               const vcl_pair< vcl_pair<int, int>, double >& p2) {
  return (p1.second < p2.second);
}

inline bool
Lie_dist_less2(const vcl_pair< vcl_pair<int, int>, vcl_pair<double, vnl_matrix<double> > >& p1,
               const vcl_pair< vcl_pair<int, int>, vcl_pair<double, vnl_matrix<double> > >& p2) {
                 return (p1.second.first < p2.second.first);
}
#if 0
//: create a rank ordered vector of "other" model patches and their matching query patches wrt to 
//  their similarity transformation's similarity to the given pairs transformation
//  the distance between the transformations is computed via Lie Algebra and the Lie Distance
//  (similarity transformations form a Lie Group, hence we can find distances between them in the Lie Space)
//  vcl_vector<vcl_pair< vcl_pair<int, int>, double > > vector of < <mod_id, q_id>, distance to the given model_id, query_id >
bool dbskr_shock_patch_match::rank_order_other_patch_pairs_wrt_sim_trans_lie(
  int model_id, int query_id, 
  vcl_vector<vcl_pair< vcl_pair<int, int>, double > >& out_vec, float sim_threshold)
{
  // first find the sim trans of the input pair

  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;
  vnl_matrix<double> *input_pair_trans = 0;

  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  bool found = false;
  for ( ; iter != patch_cor_map_.end(); iter++) {
    int id1 = iter->first;
    if (id1 == model_id) {
      train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
          
      if (train_patch_vec == 0)  // this model patch does not qualify as it does not have at least N matches
        continue;

      for (unsigned iii = 0; iii < train_patch_vec->size(); iii++) {
        vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];
        if (p.first == query_id) {
          input_pair_trans = p.second->get_similarity_trans();
          found = true;
          break;
        }
      }
      if (found)
        break;
    }
  }
  if (!found || input_pair_trans == 0)
    return false;

  iter = patch_cor_map_.begin();
  for ( ; iter != patch_cor_map_.end(); iter++) {
    int id1 = iter->first;
    if (id1 == model_id) 
      continue;

    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly      
    if (train_patch_vec == 0)  
      continue;

    for (unsigned iii = 0; iii < train_patch_vec->size(); iii++) {
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];
      
      if (p.second->final_norm_cost() > sim_threshold)
        break;

      vnl_matrix<double> *pair_trans = p.second->get_similarity_trans();
      if (pair_trans == 0)
        continue;

      //: we want Lie distance of two transformation matrices M1 = input_pair_trans, M2 = pair_trans
      vnl_matrix<double>resultant = (*input_pair_trans)*vnl_matrix_inverse<double>(*pair_trans);
      vnl_matrix<double> resultant_alg = get_Lie_algebra(resultant);
      double dist = resultant_alg.frobenius_norm();

      vcl_pair<int, int> id_p(id1, p.first);
      vcl_pair<vcl_pair<int, int>, double> pppp(id_p, dist);
      out_vec.push_back(pppp);
/*
      vcl_cout << "-----------------\n";
      vcl_cout << "model_id: " << model_id << " query_id: " << query_id << " mod_id: " << id1 << " q_id: " << p.first << "\n";
      vcl_cout << *input_pair_trans << vcl_endl << vcl_endl;
      vcl_cout << *pair_trans << vcl_endl << vcl_endl;
      vcl_cout << "resultant: " << vcl_endl;
      vcl_cout << resultant << vcl_endl;
      vcl_cout << "resultant_algebra: " << vcl_endl;
      vcl_cout << resultant_alg << vcl_endl;
      vcl_cout << "dist: " << dist << vcl_endl;
      vcl_cout << "-----------------\n";
*/
    }
  }

  //: sort the out_vec
  vcl_sort(out_vec.begin(), out_vec.end(), Lie_dist_less);

  return true;
}
#endif

void get_values(vnl_matrix<double>& G, double& sin_theta, double& cos_theta, double& sx, double& sy, double& tx, double& ty) {  
  double tol = 1e-6;
  //get the scaling,rotation angle and translation values from G
  sx = vcl_sqrt(G.get(0,0)*G.get(0,0)+G.get(0,1)*G.get(0,1));
  sy = vcl_sqrt(G.get(1,0)*G.get(1,0)+G.get(1,1)*G.get(1,1));

  if (sy > tol)
      {
      sin_theta = G.get(1,0)/sy;
      cos_theta = vcl_sqrt(1-sin_theta*sin_theta);
      }
  else if (sx > tol)
      {
      cos_theta = G.get(0,0)/sx;
      sin_theta = vcl_sqrt(1-cos_theta*cos_theta);
      }
  else
      {
      cos_theta = 1;
      sin_theta = 0;
      }

  tx = G.get(0,2);
  ty = G.get(1,2);

  return;
}

bool dbskr_shock_patch_match::get_rotation_angle(vgl_h_matrix_2d<double>& G, double& theta) {
  vnl_matrix_fixed<double, 3, 3> V = G.get_matrix();
  vnl_matrix<double> VG = V;

  double sin_theta, cos_theta, sx, sy, tx, ty;
  get_values(VG, sin_theta, cos_theta, sx, sy, tx, ty);
  theta = vcl_asin(sin_theta); //+(vnl_math::pi/2.0);  // result of asin is in [-pi/2,pi/2] --> bring it to the interval [0,pi]
  if (vcl_abs(theta - vcl_acos(cos_theta)) < 0.1) {
    vcl_cout << "asin and acos do not match for the query\n";
    return false;
  }
  return true;
}

bool dbskr_shock_patch_match::get_rotation_angle_degree(vgl_h_matrix_2d<double>& G, double& theta) {
  vnl_matrix_fixed<double, 3, 3> V = G.get_matrix();
  vnl_matrix<double> VG = V;

  double sin_theta, cos_theta, sx, sy, tx, ty;
  get_values(VG, sin_theta, cos_theta, sx, sy, tx, ty);
  theta = vcl_asin(sin_theta); //+(vnl_math::pi/2.0);  // result of asin is in [-pi/2,pi/2] --> bring it to the interval [0,pi]
  if (vcl_abs(theta - vcl_acos(cos_theta)) < 0.1) {
    vcl_cout << "asin and acos do not match for the query\n";
    return false;
  }
  theta = (theta*180.0/vnl_math::pi);
  return true;
}

vnl_matrix<double> get_Lie_algebra(vnl_matrix<double> G)             
    {
    // 3x3 Lie algebra element
    vnl_matrix<double> g(3,3,0.0);

    double g11,g12,g21,g22,g13,g23,det_val,tol = 1e-6;;
    //2x2 Identity element
    vnl_matrix<double> I(2,2,0.0);
    I.put(0,0,1);
    I.put(1,1,1);

    //2x2 rotation matrix 
    vnl_matrix<double> R(2,2,0.0);
    //2x2 scaling matrix
    vnl_matrix<double> S(2,2,0.0);
    vnl_matrix<double> P(2,2,0.0);

    //Lie algebra element corresponding to 2x2 rotation matrix
    vnl_matrix<double> A(2,2,0.0);

    double sin_theta,cos_theta,sx,sy,tx,ty;

    //get the scaling,rotation angle and translation values from G
    sx = vcl_sqrt(G.get(0,0)*G.get(0,0)+G.get(0,1)*G.get(0,1));
    sy = vcl_sqrt(G.get(1,0)*G.get(1,0)+G.get(1,1)*G.get(1,1));

    if (sy > tol)
        {
        sin_theta = G.get(1,0)/sy;
        cos_theta = vcl_sqrt(1-sin_theta*sin_theta);
        }
    else if (sx > tol)
        {
        cos_theta = G.get(0,0)/sx;
        sin_theta = vcl_sqrt(1-cos_theta*cos_theta);
        }
    else
        {
        cos_theta = 1;
        sin_theta = 0;
        }

    tx = G.get(0,2);
    ty = G.get(1,2);

    //setting the rotation matrix
    R.put(0,0,cos_theta);
    R.put(0,1,-sin_theta);
    R.put(1,0,sin_theta);
    R.put(1,1,cos_theta);

    //setting the entries of S
    S.put(0,0,sx);
    S.put(1,1,sy);

    //setting the entries of the Lie algebra element
    A.put(0,1,-1*vcl_acos(cos_theta));
    A.put(1,0,vcl_acos(cos_theta));

    if (sx > tol)
        g11 = vcl_log(sx);
    else
        g11 = 0;


    g12 = A.get(0,1);
    g21 = A.get(1,0);

    if (sy > tol)
        g22 = vcl_log(sy);
    else
        g22 = 0;


    //setting the entries of P
    P.put(0,0,g11);
    P.put(0,1,g12);
    P.put(1,0,g21);
    P.put(1,1,g22);

    det_val = P.get(0,0)*P.get(1,1) - P.get(0,1)*P.get(1,0);

    vnl_matrix<double> Pinv(2,2,0.0);

    if (vcl_abs(det_val) > 1e-6 )
        Pinv = vnl_matrix_inverse<double>(P);

    vnl_matrix<double> result1 = S*R;
    vnl_matrix<double> result2 = result1-I;
    vnl_matrix<double> res_inv = vnl_matrix_inverse<double>(result2);
    vnl_matrix<double> res = res_inv*P;

    if (((vcl_abs(sx -1) < tol)||(vcl_abs(sy -1) < tol))&&(vcl_abs(sin_theta) < tol))
        {
        g13 = tx;
        g23 = ty;
        }  
    else
        {
        g13 = res.get(0,0)*tx+ res.get(0,1)*ty;
        g23 = res.get(1,0)*tx+ res.get(1,1)*ty;
        }


    g.put(0,0,g11);
    g.put(0,1,g12);
    g.put(0,2,g13);
    g.put(1,0,g21);
    g.put(1,1,g22);
    g.put(1,2,g23);
    g.put(2,2,0);

    return g;
    }

bool dbskr_shock_patch_match::get_Lie_dist_to_identity(vgl_h_matrix_2d<double>& G, double& dist)
{
  vnl_matrix_fixed<double, 3, 3> V = G.get_matrix();
  vnl_matrix<double> VG = V;
  
  //: we want Lie distance of two transformation matrices M1 = input_pair_trans, M2 = pair_trans
  //vnl_matrix<double>resultant = (*input_pair_trans)*vnl_matrix_inverse<double>(*pair_trans);
  //vnl_matrix<double> resultant_alg = get_Lie_algebra(resultant);
  vnl_matrix<double> resultant_alg = get_Lie_algebra(VG);
  dist = resultant_alg.frobenius_norm();
  return true;
}

bool dbskr_shock_patch_match::get_Lie_dist(vnl_matrix<double>& G1, vnl_matrix<double>& G2, double& dist)
{
  //: we want Lie distance of two transformation matrices M1 = input_pair_trans, M2 = pair_trans
  vnl_matrix<double>resultant = G1*vnl_matrix_inverse<double>(G2);
  vnl_matrix<double> resultant_alg = get_Lie_algebra(resultant);
  dist = resultant_alg.frobenius_norm();
  if (dist < 0)
    return false;
  return true;
}
/*
bool dbskr_shock_patch_match::get_Lie_dist_to_identity_only_rotation(vgl_h_matrix_2d<double>& G, double& dist)
{
  vnl_matrix_fixed<double, 3, 3> V = G.get_matrix();
  get_rotation_angle(
  vnl_matrix<double> VG(V);
  VG.
  
  vnl_matrix<double> resultant_alg = get_Lie_algebra(VG);
  dist = resultant_alg.frobenius_norm();
  return true;
}
*/

bool dbskr_shock_patch_match::get_rotation_angle(vnl_matrix<double>& G, double& theta) {
  double sin_theta, cos_theta, sx, sy, tx, ty;
  get_values(G, sin_theta, cos_theta, sx, sy, tx, ty);
  theta = vcl_asin(sin_theta); //+(vnl_math::pi/2.0);  // result of asin is in [-pi/2,pi/2] --> bring it to the interval [0,pi]
  if (vcl_abs(theta - vcl_acos(cos_theta)) < 0.1) {
    vcl_cout << "asin and acos do not match for the query\n";
    return false;
  }
  return true;
}

double find_trans_dist(vnl_matrix<double>& inp_pair_trans, vnl_matrix<double>& pair_trans) {
  double inp_sin_theta,inp_cos_theta,inp_sx,inp_sy,inp_tx,inp_ty;
  get_values(inp_pair_trans, inp_sin_theta, inp_cos_theta, inp_sx, inp_sy, inp_tx, inp_ty);  
  double inp_theta = vcl_asin(inp_sin_theta) + (vnl_math::pi/2.0);  // result of asin is in [-pi/2,pi/2] --> bring it to the interval [0,pi]
  if (vcl_abs(inp_theta - (vcl_acos(inp_cos_theta) + (vnl_math::pi/2.0)) ) < 0.1) {
    vcl_cout << "asin and acos do not match\n";
    //return false;
  }
      
  double sin_theta,cos_theta,sx,sy,tx,ty;
  get_values(pair_trans, sin_theta, cos_theta, sx, sy, tx, ty);  
  double theta = vcl_asin(sin_theta)+(vnl_math::pi/2.0);  // result of asin is in [-pi/2,pi/2] --> bring it to the interval [0,pi]
  if (vcl_abs(theta - (vcl_acos(cos_theta)+(vnl_math::pi/2.0)) ) < 0.1) {
    vcl_cout << "asin and acos do not match for the query\n";
    //return false;
  }

  //: scale up theta range from ~[0,pi] = [0,3.1] to [0,6]
  //  scale up scale range from ~[0,1] to [0,6]
  //  scale up translation range from ~[0,200] to [0,5]  
  //  so that all the added values are roughly at the same value ranges and have equal importances
  double wt = 2, ws = 6, wtr = 1.0/40.0;
  double dist = wt *vcl_abs(inp_theta - theta) + 
                ws *(vcl_sqrt(vcl_pow(vcl_abs(inp_sx-sx),2)+vcl_pow(vcl_abs(inp_sy-sy),2))) +
                wtr*(vcl_sqrt(vcl_pow(vcl_abs(inp_tx-tx),2)+vcl_pow(vcl_abs(inp_ty-ty),2)));

  vcl_cout << "input theta: " << inp_theta << " sin_theta: " << inp_sin_theta << " cos_theta: " << inp_cos_theta << " sx: " << inp_sx << " sy: " << inp_sy << " tx: " << inp_tx << " ty: " << inp_ty << vcl_endl;
  vcl_cout << "      theta: " << theta << " sin_theta: " << sin_theta << " cos_theta: " << cos_theta << " sx: " << sx << " sy: " << sy << " tx: " << tx << " ty: " << ty << vcl_endl;
  vcl_cout << "theta dif: " << vcl_abs(inp_theta - theta) << " weighted (" << wt << "): " << wt *vcl_abs(inp_theta - theta) << vcl_endl;
  vcl_cout << "scale dif: " << (vcl_sqrt(vcl_pow(vcl_abs(inp_sx-sx),2)+vcl_pow(vcl_abs(inp_sy-sy),2))) << " weighted (" << ws << "): " << ws *(vcl_sqrt(vcl_pow(vcl_abs(inp_sx-sx),2)+vcl_pow(vcl_abs(inp_sy-sy),2))) << vcl_endl;
  vcl_cout << "trans dif: " << (vcl_sqrt(vcl_pow(vcl_abs(inp_tx-tx),2)+vcl_pow(vcl_abs(inp_ty-ty),2))) << " weighted (" << wtr << "): " << wtr*(vcl_sqrt(vcl_pow(vcl_abs(inp_tx-tx),2)+vcl_pow(vcl_abs(inp_ty-ty),2))) << vcl_endl;
      
  return dist;
}

//: create a rank ordered vector of "other" model patches and their matching query patches wrt to 
//  their similarity transformation's similarity to the given pairs transformation
//  the distance between the transformations is computed via a simple formula measuring angle, scale and translation differences
//  vcl_vector<vcl_pair< vcl_pair<int, int>, double > > vector of < <mod_id, q_id>, distance to the given model_id, query_id >
bool dbskr_shock_patch_match::rank_order_other_patch_pairs_wrt_sim_trans(
  int model_id, int query_id, 
//  vcl_vector<vcl_pair< vcl_pair<int, int>, double > >& out_vec, float sim_threshold)
  vcl_vector<vcl_pair< vcl_pair<int, int>, vcl_pair<double, vnl_matrix<double> > > >& out_vec, float sim_threshold)
{
  // first find the sim trans of the input pair

  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;
  vgl_h_matrix_2d<double> input_pair_H;
  vnl_matrix<double> input_pair_trans;

  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  bool found = false;
  for ( ; iter != patch_cor_map_.end(); iter++) {
    int id1 = iter->first;

    if (id1 == model_id) {
      train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
          
      if (train_patch_vec == 0)  // this model patch does not qualify as it does not have at least N matches
        continue;

      for (unsigned iii = 0; iii < train_patch_vec->size(); iii++) {
        vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];
        if (p.first == query_id) {
      
          p.second->get_similarity_trans(input_pair_H);
          input_pair_trans = p.second->get_similarity_trans();

          found = true;
          break;
        }
      }
      if (found)
        break;
    }
  }
  if (!found || input_pair_trans.rows() != 3 || input_pair_trans.cols() != 3)
    return false;

  iter = patch_cor_map_.begin();
  for ( ; iter != patch_cor_map_.end(); iter++) {
    int id1 = iter->first;
    if (id1 == model_id) 
      continue;

    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly      
    if (train_patch_vec == 0)  
      continue;

    vcl_map<int, dbskr_shock_patch_sptr>::iterator it = map1_.find(id1);
    if (it == map1_.end()) {
      vcl_cout << " set the id to shock patch sptr maps in the match instance\n";
      return false;
    }
    dbskr_shock_patch_sptr sp1 = it->second;

    for (unsigned iii = 0; iii < train_patch_vec->size(); iii++) {
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];
      
      if (p.second->final_norm_cost() > sim_threshold)
        break;

      vnl_matrix<double> pair_trans = p.second->get_similarity_trans();
      if (pair_trans.rows() != 3 || pair_trans.cols() != 3)
        continue;
      
      vcl_map<int, dbskr_shock_patch_sptr>::iterator it2 = map2_.find(p.first);
      if (it2 == map2_.end()) {
        vcl_cout << " set the id to shock patch sptr maps in the match instance\n";
        return false;
      }
      
      double sin_theta,cos_theta,sx,sy,tx,ty;
      get_values(pair_trans, sin_theta, cos_theta, sx, sy, tx, ty);  
      double theta = vcl_asin(sin_theta)+(vnl_math::pi/2.0);  // result of asin is in [-pi/2,pi/2] --> bring it to the interval [0,pi]
      if (vcl_abs(theta - (vcl_acos(cos_theta)+(vnl_math::pi/2.0)) ) < 0.1) {
        vcl_cout << "asin and acos do not match for the query\n";
        //return false;
      }

      vcl_cout << "-----------------\n";
      vcl_cout << "model_id: " << model_id << " query_id: " << query_id << " mod_id: " << id1 << " q_id: " << p.first << "\n";
      vcl_cout << input_pair_trans << vcl_endl << vcl_endl;
      vcl_cout << pair_trans << vcl_endl << vcl_endl;
      
      double dist = find_trans_dist(input_pair_trans, pair_trans);

      vcl_cout << "dist: " << dist << vcl_endl;
      vcl_cout << "-----------------\n";

      vcl_pair<int, int> id_p(id1, p.first);
      //vcl_pair<vcl_pair<int, int>, double> pppp(id_p, dist);
      vcl_pair<double, vnl_matrix<double> > dist_p(dist, pair_trans);
      vcl_pair<vcl_pair<int, int>, vcl_pair<double, vnl_matrix<double> > > pppp(id_p, dist_p);
      out_vec.push_back(pppp);
    }
  }

  //: sort the out_vec
  vcl_sort(out_vec.begin(), out_vec.end(), Lie_dist_less2);

  return true;
}


//: detect an instance of the category of the first storage in the second storage's image, implied by this match
// use top N matches of the image to each model patch
// put as a constraint that at least k model patches have top N matches less than threshold
// In BMVC07 paper N = 3 and k = 2
bool dbskr_shock_patch_match::detect_instance(vsol_box_2d_sptr& detection_box, int N, int k, float threshold) {

  int qualifying_model_patch_cnt = 0;  // <model patch id, # of matches less than threshold>
  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;

  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  
  for ( ; iter != patch_cor_map_.end(); iter++) {
    int id1 = iter->first;
    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
        
    if (train_patch_vec == 0 || !(int(train_patch_vec->size()) >= N) ) {  // this model patch does not qualify as it does not have at least N matches
      continue;
    }

    vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[N-1];
    if (p.second->final_norm_cost() > threshold)  // this model patch's top N matches are not less than the threshold
      continue;

    qualifying_model_patch_cnt++;

    //: collect the bounding boxes 
    for (unsigned iii = 0; int(iii) < N; iii++) {
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];
      dbskr_shock_patch_sptr sp = map2_[p.first];
      vsol_box_2d_sptr box = sp->shock_graph()->get_bounding_box(); 
      if (!box && !dbsk2d_compute_bounding_box(sp->shock_graph())) {
        vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << sp->id() << " in the second storage " << vcl_endl;
        return false;
      }
          
      box = sp->shock_graph()->get_bounding_box(); 
      if (!box) {
        vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << sp->id() << " in the second storage " << vcl_endl;
        return false;
      }

      if (!detection_box)
        detection_box = new vsol_box_2d();
      detection_box->grow_minmax_bounds(box);
    }
  }
  
  if (qualifying_model_patch_cnt < k)  // not enough model patches with at least N top matches less than threshold
    detection_box = 0;

  //vcl_cout << "there were " << qualifying_model_patch_cnt << " many qualifying model patches\n";

  return true;
}


//: detect an instance of the category of the first storage in the second storage's image, implied by this match
//  use the consistency similarity transformation's of patch pairs
//  algorithm explained in Kimia's NSF december proposal 2007
bool dbskr_shock_patch_match::detect_instance_wrt_trans(vcl_vector<vsol_box_2d_sptr>& detection_box, int k, float trans_threshold)
{
#if 0
  if (!map1_.size() || !map2_.size())
    return false;

  vcl_vector<int> ids1; vcl_map<int, unsigned> ids1_to_ind;
  for (vcl_map<int, dbskr_shock_patch_sptr>::iterator it = map1_.begin(); it != map1_.end(); it++) {
    ids1.push_back(it->first);
    ids1_to_ind[it->first] = ids1.size()-1;
  }

  vcl_vector<int> ids2; vcl_map<int, unsigned> ids2_to_ind;
  for (vcl_map<int, dbskr_shock_patch_sptr>::iterator it = map2_.begin(); it != map2_.end(); it++) {
    ids2.push_back(it->first);
    ids2_to_ind[it->first] = ids2.size()-1;
  }

  vcl_vector<vcl_vector<bool> > valid_pairs;  // holds in the order of ids in map1_ and map2_
  for (unsigned i = 0; i < ids1.size(); i++) {
    vcl_vector<bool> tmp(ids2.size(), false);
    valid_pairs.push_back(tmp);
  }

  vcl_vector<vcl_vector<vnl_matrix<double>* > > valid_pair_ts;  // holds in the order of ids in map1_ and map2_
  for (unsigned i = 0; i < ids1.size(); i++) {
    vcl_vector<vnl_matrix<double>* > tmp(ids2.size(), 0);
    valid_pair_ts.push_back(tmp);
  }

  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;
  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  for ( ; iter != patch_cor_map_.end(); iter++) {
    int id1 = iter->first;
    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
    if (train_patch_vec == 0) {  // this model patch does not qualify as it does not have at least N matches
      continue;
    }
    unsigned ind1 = ids1_to_ind[id1];

    for (unsigned iii = 0; iii < train_patch_vec->size(); iii++) {
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[iii];
      if (p.second->get_similarity_trans() != 0) {
        unsigned ind2 = ids2_to_ind[p.first];
        valid_pairs[ind1][ind2] = true;
        valid_pair_ts[ind1][ind2] = p.second->get_similarity_trans();
      }
    }
  }

  unsigned rows = ids1.size();
  unsigned cols = ids2.size();
  //: count the number of valid pairs
  int cnt = 0;
  for (unsigned i = 0; i < rows; i++) {
    int cnt_i = 0;
    for (unsigned j = 0; j < cols; j++) {
      cnt_i++;
      cnt++;
    }
    vcl_cout << "i: " << i << " model id: " << ids1[i] << " cnt_i: " << cnt_i << vcl_endl;
  }
  vcl_cout << " total valid pairs: " << cnt << vcl_endl;

  //: find all the pairs with transformation similarity less than the given 
  //: for each model (i) - query (j) pair, sort all the other pairs except the model row (i) and query column (j)
  for (unsigned i = 0; i < rows; i++) {
    for (unsigned j = 0; j < cols; j++) {

      if (!valid_pairs[i][j])
        continue;
      
      vnl_matrix<double>* pair_trans = valid_pair_ts[i][j];


      //: create the valid matrix for this pair
      vcl_vector<vcl_vector<bool> > valid_pairs_ij(valid_pairs);
      //: invalidate row i and column j
      for (unsigned jj = 0; jj < cols; jj++)
        valid_pairs_ij[i][jj] = false;
      for (unsigned ii = 0; ii < rows; ii++)
        valid_pairs_ij[ii][j] = false;

      //: find the distance of this pairs transformation to each other valid one 
      vcl_vector<vcl_pair< vcl_pair<int, int>, double > > dists;
      unsigned t_cnt = 0;
      for (unsigned ii = 0; ii < rows; ii++) {
        for (unsigned jj = 0; jj < cols; jj++)
          if (valid_pairs_ij[ii][jj]) {
            t_cnt++;
            double dist = find_trans_dist(pair_trans, valid_pair_ts[ii][jj]);
            vcl_pair<int, int> idp;
            idp.first = int(ii);
            idp.second = int(jj);
            vcl_pair<vcl_pair<int, int>, double> idpp;
            idpp.first = idp;
            idpp.second = dist;
            dists.push_back(idpp);
          }
      }
      if (dists.size() != t_cnt)
        return false;
      vcl_sort(dists.begin(), dists.end(), Lie_dist_less);
      
      if (!dists.size() || dists[0].second > trans_threshold) {
        vcl_cout << "this i: " << i << " j: " << j << " pair has no other consistent pairs\n";
        continue;  
      }

      //: find all the pairs
      //  if k = 1 then we need at least two consistent model patches (two consistent model-query pair, with different query patches)
      //           need just one other to declare a detection
      //  if k = 2 then we need at least three consistent model patches -- find two others
      for (int kkk = 0; kkk < k; kkk++) {

        for (unsigned dd = 0; dd < dists.size(); dd++) {
          if (!valid_pairs_ij[dists[dd].first.first][dists[dd].first.second])
            continue;

          //: caught a still valid match --> declare a detection and invalidate its row and column
          
        }

      }
    }

      

  }

  vsol_box_2d_sptr b = new vsol_box_2d();
  b->add_point(10, 10);
  b->add_point(100,100);
  detection_box.push_back(b);
#endif
  return true;
}




//: detect an instance of the category of the first storage in the second storage's image, 
//  implied by this match and the given input shock graphs
//  find the best matching patch-quad such that p_1_i and p_1_j from storage 1 
//  is most "consistent" with p_2_k and p_2_m from storage 2 as implied by the
//  pathces that are formed in sg1 and sg2 using these 4 patches as anchor points
//  put an upper threshold constraint so that not all match pairs are searched in sg1 and sg2
//  if the best quad's similarity is less than threshold declare a detection
//  this method requires at least two model patch to match to query patches
//  constraint: all four patches should be different, i.e. k != m and i != j
//              and k,m  and i,j should have different root nodes even if they have different depth
//
//  Algo
//  find all possible quads (p_1_i, p_1_j, p_2_k, p_2_m) such that
//       sim(p_1_i, p_2_k) < upper_thres && sim(p_1_j, p_2_m) < upper_thres
bool 
dbskr_shock_patch_match::detect_instance(vsol_box_2d_sptr& detection_box, 
                                         dbskr_shock_path_finder& f1, 
                                         dbskr_shock_path_finder& f2, 
                                         float threshold, 
                                         float upper_threshold, 
                                         float interpolate_ds, float sample_ds, 
                                         dbskr_tree_edit_params& edit_params, 
                                         bool normalize, float alpha,
                                         vil_image_resource_sptr img1,
                                         vil_image_resource_sptr img2, vcl_string out_img)
{
  unsigned n1 = map1_.size();
  unsigned n2 = map2_.size();
  if (!n1 || !n2 || n1 != patch_cor_map_.size())
    return false;

  //: check path finders
  if (!f1.get_v() || !f1.construct_v())
    return false;

  if (!f2.get_v() || !f2.construct_v())
    return false;

  dbsk2d_shock_graph_sptr sg1 = f1.get_sg();
  dbsk2d_shock_graph_sptr sg2 = f2.get_sg();

  if (!sg1->number_of_vertices() || !sg2->number_of_vertices())
    return false;

  //: make a dummy map for indices
  vcl_map<int, unsigned> map2_dummy;
  unsigned cnt = 0;
  for (vcl_map<int, dbskr_shock_patch_sptr>::iterator it = map2_.begin(); it != map2_.end(); it++, cnt++) 
  {  map2_dummy[it->first] = cnt;  }

  //: let n1 = size of storage 1
  //      n2 = size of storage 2
  //  prepare an n1xn2 size bool table that keeps track of valid pairs
  vcl_vector<float> tmp(n2, 100000.0f);
  vcl_vector<vcl_vector<float> > valid(n1, tmp);
  
  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;
  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  cnt = 0; unsigned valid_cnt = 0; 
  for ( ; iter != patch_cor_map_.end(); iter++, cnt++) {
    int id1 = iter->first;
    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
    if (!train_patch_vec)  // this model patch does not qualify as it does not have any matches
      continue;
    
    //vcl_cout << "id1: " << id1 << " matches: ";
    for (unsigned j = 0; j < train_patch_vec->size(); j++) {
      //: check all the pairs with cost less than upper_threshold
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[j];
      //vcl_cout << p.second->final_norm_cost() << " ";
      if (p.second->final_norm_cost() > upper_threshold)  // this model patch's j th match does not satisfy 
        break;
      //find index of this patch using its id
      vcl_map<int, unsigned>::iterator it = map2_dummy.find(p.first);
      if (it == map2_dummy.end())
        continue;
      unsigned ind = it->second;
      valid[cnt][ind] = p.second->final_norm_cost();
      valid_cnt++;
    }
    //vcl_cout << "\n";
  }
  vcl_cout << "\t" << valid_cnt << " pairs < " << upper_threshold << "\n";
  if (!valid_cnt) {
    detection_box = 0;
    return true;
  }
  //: extract the v graphs from all valid pairs

  //: now find all valid quads and extract the v graphs from all valid pairs
  float min_sim = 10000000.0f, min_sim_i_k, min_sim_j_m;
  dbskr_shock_patch_sptr min_p_1_ij, min_p_2_km;
  dbskr_shock_patch_sptr min_p_1_i, min_p_1_j, min_p_2_k, min_p_2_m;
  dbskr_sm_cor_sptr min_sm_cor; dbsk2d_shock_graph_sptr min_sg_1, min_sg_2;

  vcl_map<int, dbsk2d_shock_node_sptr> sg1_map, sg2_map;
  get_node_map(sg1_map, sg1);
  get_node_map(sg2_map, sg2);

  vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_shock_patch_sptr>*, vcl_vector<float>*> > patch_map1; 
  vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_shock_patch_sptr>*, vcl_vector<float>*> > patch_map2; 

  unsigned quad_cnt = 0;
  vcl_vector<bool> tried_on_sg1(n1, false);
  vcl_map<int, dbskr_shock_patch_sptr>::iterator it1_i = map1_.begin();
  for (unsigned i = 0; it1_i != map1_.end(); it1_i++, i++) {
    dbskr_shock_patch_sptr p_1_i = it1_i->second;
          
    vcl_map<int, dbskr_shock_patch_sptr>::iterator it1_j = it1_i;
    it1_j++;
    for (unsigned j = i+1; it1_j != map1_.end(); it1_j++, j++) {
      
      //: reset the paths on query to save memory
      //f2.clear();

      dbskr_shock_patch_sptr p_1_j = it1_j->second;
      
      if (p_1_i->start_node_id() == p_1_j->start_node_id())  // put constraint that they have different root nodes
        continue;

      //: get all the v_graphs
      dbskr_v_graph_sptr vg_1_i = p_1_i->get_v_graph(); 
      dbsk2d_shock_node_sptr node_1_i = sg1_map[p_1_i->start_node_id()];
      if (!node_1_i)
        return false;
      if (!vg_1_i) {  // extract and set
        vg_1_i = construct_v_graph(sg1, node_1_i, p_1_i->depth());
        if (!vg_1_i)
          return false;
        p_1_i->set_v_graph(vg_1_i);
      }

      dbskr_v_graph_sptr vg_1_j = p_1_j->get_v_graph(); 
      dbsk2d_shock_node_sptr node_1_j = sg1_map[p_1_j->start_node_id()];
      if (!node_1_j)
        return false;
      if (!vg_1_j) {  // extract and set
        vg_1_j = construct_v_graph(sg1, node_1_j, p_1_j->depth());
        if (!vg_1_j)
          return false;
        p_1_j->set_v_graph(vg_1_j);
      }

      vcl_pair<unsigned, unsigned> p1(i, j);
      vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_shock_patch_sptr>*, vcl_vector<float>*> >::iterator it = patch_map1.find(p1); 
      vcl_vector<dbskr_shock_patch_sptr>* patches1;
      vcl_vector<float>* lengths1;
      if (it == patch_map1.end()) {
        vcl_vector<dbskr_v_graph_sptr> graphs1;
        vcl_vector<float> abs_lengths1;
        f1.get_all_v_graphs(node_1_i, node_1_j, vg_1_i, vg_1_j, graphs1, abs_lengths1, 1.0f, 10000.0f);  // do not restrict prototype by length 


        patches1 = new vcl_vector<dbskr_shock_patch_sptr>();
        lengths1 = new vcl_vector<float>();
        for (unsigned ii = 0; ii < graphs1.size(); ii++) {
          dbskr_shock_patch_sptr p_ii = extract_patch_from_v_graph(graphs1[ii], 0, 0, 0.05, true, true, true, interpolate_ds, sample_ds);
          p_ii->set_tree_parameters(edit_params.elastic_splice_cost_, edit_params.circular_ends_, edit_params.combined_edit_, edit_params.scurve_sample_ds_, edit_params.scurve_interpolate_ds_);
          if (p_ii->extract_simple_shock() && p_ii->prepare_tree()) {
            patches1->push_back(p_ii);
            lengths1->push_back(abs_lengths1[ii]);
          }
        }
        patch_map1[p1] = vcl_pair<vcl_vector<dbskr_shock_patch_sptr>*, vcl_vector<float>* >(patches1, lengths1);
      } else {
        patches1 = it->second.first;
        lengths1 = it->second.second;
      }

      if (!lengths1->size()) {
        //vcl_cout << "no patches1 for this quad!!\n";
        //continue;
        
        // find the union of p_1_i and p_1_j to be compared (this is a quick hack to get protos with badly generated patch sets to work, i.e. their patches all originate from the same root)
        //: just pick the larger patch as the union for now (assuming they overlap significantly)
        vsol_box_2d_sptr bi = p_1_i->bounding_box();
        vsol_box_2d_sptr bj = p_1_j->bounding_box();
        if (bi->area() > bj->area()) {
          
          if (tried_on_sg1[i])  // not to go through all of second set of pairs multiple times
            continue;
          tried_on_sg1[i] = true;
          
          p_1_i->set_tree_parameters(edit_params.elastic_splice_cost_, edit_params.circular_ends_, edit_params.combined_edit_, edit_params.scurve_sample_ds_, edit_params.scurve_interpolate_ds_);
          if (p_1_i->extract_simple_shock() && p_1_i->prepare_tree()) {
            patches1->push_back(p_1_i);
            lengths1->push_back((float)(2*bi->width() + 2*bi->height()));
          } else
            continue;
          
        } else {

          if (tried_on_sg1[j])  // not to go through all of second set of pairs multiple times
            continue;
          tried_on_sg1[j] = true;
          p_1_j->set_tree_parameters(edit_params.elastic_splice_cost_, edit_params.circular_ends_, edit_params.combined_edit_, edit_params.scurve_sample_ds_, edit_params.scurve_interpolate_ds_);
          
          if (p_1_j->extract_simple_shock() && p_1_j->prepare_tree()) {
            patches1->push_back(p_1_j);
            lengths1->push_back((float)(2*bj->width() + 2*bj->height()));
          } else
            continue;
          
        }
      } 

      // for now find the max lengths1
      float max_l = *(max_element(lengths1->begin(), lengths1->end()));
      //: find the box of p_1_i and p_1_j and normalize by its perimeter
      vsol_box_2d_sptr box = p_1_i->union_box(*p_1_j);
      float box_perim = (float)(2*box->width() + 2*box->height());
      //: set the threshold to be 1.5*max_length as normalized
      float norm_thres = 1.5f*max_l/box_perim;


      vcl_map<int, dbskr_shock_patch_sptr>::iterator it2_k = map2_.begin(); 
      for (unsigned k = 0; it2_k != map2_.end(); it2_k++, k++) {
        if (valid[i][k] > upper_threshold)
          continue;

        dbskr_shock_patch_sptr p_2_k = it2_k->second;
        vcl_map<int, dbskr_shock_patch_sptr>::iterator it2_m = it2_k;
        it2_m++;
        for (unsigned m = k+1; it2_m != map2_.end(); it2_m++, m++) {
          if (valid[j][m] > upper_threshold)
            continue;

          dbskr_shock_patch_sptr p_2_m = it2_m->second;

          if (p_2_k->start_node_id() == p_2_m->start_node_id())    // put constraint that they have different root nodes
            continue;

          //: now find the similarity of super-patches formed by this quad
          //vcl_cout << "quad: i: " << p_1_i->id() << " matches k: " << p_2_k->id() << " j: " << p_1_j->id() << " matches m: " << p_2_m->id() << vcl_endl;
 
         
          dbskr_v_graph_sptr vg_2_k = p_2_k->get_v_graph(); 
          dbsk2d_shock_node_sptr node_2_k = sg2_map[p_2_k->start_node_id()];
          if (!node_2_k)
            return false;
          if (!vg_2_k) {  // extract and set
            vg_2_k = construct_v_graph(sg2, node_2_k, p_2_k->depth());
            if (!vg_2_k)
              return false;
            p_2_k->set_v_graph(vg_2_k);
          }

          dbskr_v_graph_sptr vg_2_m = p_2_m->get_v_graph(); 
          dbsk2d_shock_node_sptr node_2_m = sg2_map[p_2_m->start_node_id()];
          if (!node_2_m)
            return false;
          if (!vg_2_m) {  // extract and set
            vg_2_m = construct_v_graph(sg2, node_2_m, p_2_m->depth());
            if (!vg_2_m)
              return false;
            p_2_m->set_v_graph(vg_2_m);
          }

          //: find the normalization factor for the second pair
          vsol_box_2d_sptr box2 = p_2_k->union_box(*p_2_m);
          float box2_perim = (float)(2*box2->width() + 2*box2->height());
          
          vcl_pair<unsigned, unsigned> p2(k, m);
          it = patch_map2.find(p2);
          vcl_vector<dbskr_shock_patch_sptr>* patches2;
          vcl_vector<float>* lengths2;
          //if (it == patch_map2.end()) {
            vcl_vector<dbskr_v_graph_sptr> graphs2;
            vcl_vector<float> abs_lengths2;
            f2.get_all_v_graphs(node_2_k, node_2_m, vg_2_k, vg_2_m, graphs2, abs_lengths2, box2_perim, norm_thres); 

            patches2 = new vcl_vector<dbskr_shock_patch_sptr>();
            lengths2 = new vcl_vector<float>();
            for (unsigned ii = 0; ii < graphs2.size(); ii++) {
              dbskr_shock_patch_sptr p_ii = extract_patch_from_v_graph(graphs2[ii], 0, 0, 0.05, true, true, true, interpolate_ds, sample_ds);
              p_ii->set_tree_parameters(edit_params.elastic_splice_cost_, edit_params.circular_ends_, edit_params.combined_edit_, edit_params.scurve_sample_ds_, edit_params.scurve_interpolate_ds_);
              if (p_ii->extract_simple_shock() && p_ii->prepare_tree()) {
                patches2->push_back(p_ii);
                lengths2->push_back(abs_lengths2[ii]);
              }
            }
           // patch_map2[p2] = vcl_pair<vcl_vector<dbskr_shock_patch_sptr>*, vcl_vector<float>* >(patches2, lengths2);
          //} else {
          //  patches2 = it->second.first;
          //  lengths2 = it->second.second;
          //}
          
          //: find the best matching graph pair, and assign its similarity
          for (unsigned ii = 0; ii < patches1->size(); ii++) {
            dbskr_shock_patch_sptr p_ii = (*patches1)[ii];
            dbskr_tree_sptr t_ii = p_ii->tree();
            for (unsigned jj = 0; jj < patches2->size(); jj++) {
              dbskr_shock_patch_sptr p_jj = (*patches2)[ii];
              dbskr_tree_sptr t_jj = p_jj->tree();


              //instantiate the edit distance algorithm
              dbskr_tree_edit* edit;
              if (edit_params.combined_edit_)
                edit = new dbskr_tree_edit_combined(t_ii, t_jj, edit_params.circular_ends_, edit_params.localized_edit_);  
              else
                edit = new dbskr_tree_edit(t_ii, t_jj, edit_params.circular_ends_, edit_params.localized_edit_);

              edit->save_path(true);
              if (!edit->edit()) {
                vcl_cout << "Problems in editing trees\n";
                continue;
              }
              float val = edit->final_cost();
              dbskr_sm_cor_sptr sm_cor = edit->get_correspondence_just_map();  // sets the parameters
              sm_cor->set_final_cost(val);
                
              if (normalize) {
                float norm_val = val/(t_ii->total_reconstructed_boundary_length()+t_jj->total_reconstructed_boundary_length());
                //vcl_cout << "norm cost: " << norm_val << " time: "<< t.real()/1000.0f << " secs.\n";
                sm_cor->set_final_norm_cost(norm_val);
                val = norm_val;
              }

              float final_sim = alpha*val + ((1.0f-alpha)/2.0f)*valid[i][k] + ((1.0f-alpha)/2.0f)*valid[j][m];
              if (final_sim < min_sim) {
                min_sim = final_sim;
                min_p_1_ij = p_ii;
                min_p_2_km = p_jj;
                min_p_1_i = p_1_i; min_p_1_j = p_1_j;
                min_p_2_k = p_2_k; min_p_2_m = p_2_m;
                min_sim_i_k = valid[i][k];
                min_sim_j_m = valid[j][m];
                min_sm_cor = sm_cor;
                min_sg_1 = p_ii->shock_graph();
                min_sg_2 = p_jj->shock_graph();
              }

              delete edit;

            }
          }
          
          patches2->clear();
          lengths2->clear();
          delete patches2;
          delete lengths2;
          quad_cnt++;
          
        }
      }
    }
  }
  //: clear the maps
  for (vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_shock_patch_sptr>*, vcl_vector<float>*> >::iterator it = patch_map1.begin(); it != patch_map1.end(); it++) {
    it->second.first->clear();
    it->second.second->clear();
    delete it->second.first;
    delete it->second.second;
  }

  for (vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_shock_patch_sptr>*, vcl_vector<float>*> >::iterator it = patch_map2.begin(); it != patch_map2.end(); it++) {
    it->second.first->clear();
    it->second.second->clear();
    delete it->second.first;
    delete it->second.second;
  }

  vcl_cout << quad_cnt << " quads\n";

  if (!min_p_1_ij || !min_p_2_km) {  // no min found, no valid quads with distinct root nodes
    detection_box = 0;
    return true;
  }

  vcl_cout << "\tmin_sim: " << min_sim << " sim i,k: " << min_sim_i_k << " sim j,m: " << min_sim_j_m << vcl_endl;

  if (img1 != 0 && img2 != 0) {  // prepare output images
    vcl_vector<dbskr_shock_patch_sptr> ps;  vcl_vector<vil_rgb<int> > colors;
    ps.push_back(min_p_1_i);  colors.push_back(vil_rgb<int>(1,0,0));
    ps.push_back(min_p_1_j);  colors.push_back(vil_rgb<int>(0,1,0));
    ps.push_back(min_p_1_ij); colors.push_back(vil_rgb<int>(0,0,1));
    create_ps_patches(out_img+"_1.ps", ps, colors, img1);
    dbsk2d_xshock_graph_fileio writer;
    //writer.save_xshock_graph(min_p_1_ij->shock_graph(), out_img+"_1.esf");
    writer.save_xshock_graph(min_sg_1, out_img+"_1.esf");

    ps.clear();
    ps.push_back(min_p_2_k);  
    ps.push_back(min_p_2_m);  
    ps.push_back(min_p_2_km); 
    create_ps_patches(out_img+"_2.ps", ps, colors, img2);
    //writer.save_xshock_graph(min_p_2_km->shock_graph(), out_img+"_2.esf");
    writer.save_xshock_graph(min_sg_2, out_img+"_2.esf");
    min_sm_cor->set_tree1(min_p_1_ij->tree());
    min_sm_cor->set_tree2(min_p_2_km->tree());
    min_sm_cor->write_shgm(out_img+"_match.shgm");
  }

  if (min_sim < threshold) {
    //: the output box is the union of boxes of the min sim patches on the query image
    detection_box = new vsol_box_2d();
    vsol_box_2d_sptr box = min_p_2_m->shock_graph()->get_bounding_box(); 
    if (!box && !dbsk2d_compute_bounding_box(min_p_2_m->shock_graph())) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_m->id() << " in the second storage " << vcl_endl;
      return false;
    }
    box = min_p_2_m->shock_graph()->get_bounding_box(); 
    if (!box) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_m->id() << " in the second storage " << vcl_endl;
      return false;
    }
    detection_box->grow_minmax_bounds(box);

    box = min_p_2_k->shock_graph()->get_bounding_box(); 
    if (!box && !dbsk2d_compute_bounding_box(min_p_2_k->shock_graph())) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_k->id() << " in the second storage " << vcl_endl;
      return false;
    }
    box = min_p_2_k->shock_graph()->get_bounding_box(); 
    if (!box) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_k->id() << " in the second storage " << vcl_endl;
      return false;
    }
    detection_box->grow_minmax_bounds(box);

    box = min_p_2_km->shock_graph()->get_bounding_box(); 
    if (!box && !dbsk2d_compute_bounding_box(min_p_2_km->shock_graph())) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_km->id() << " in the second storage " << vcl_endl;
      return false;
    }
    box = min_p_2_km->shock_graph()->get_bounding_box(); 
    if (!box) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_km->id() << " in the second storage " << vcl_endl;
      return false;
    }
    detection_box->grow_minmax_bounds(box);
    //vcl_cout << "box area: " << detection_box->area() << "\n";
  } else {
    detection_box = 0;
  }
  
  return true;
}

void dbskr_shock_patch_match::find_valid_pairs(float upper_threshold, vcl_vector<vcl_vector<float> >& valid, unsigned int& valid_cnt) 
{
  //: make a dummy map for indices
  vcl_map<int, unsigned> map2_dummy;
  unsigned cnt = 0;
  for (vcl_map<int, dbskr_shock_patch_sptr>::iterator it = map2_.begin(); it != map2_.end(); it++, cnt++) 
  {  map2_dummy[it->first] = cnt;  }

  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;
  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  cnt = 0; 
  for ( ; iter != patch_cor_map_.end(); iter++, cnt++) {
    int id1 = iter->first;
    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
    if (!train_patch_vec)  // this model patch does not qualify as it does not have any matches
      continue;
    
    //vcl_cout << "id1: " << id1 << " matches: ";
    for (unsigned j = 0; j < train_patch_vec->size(); j++) {
      //: check all the pairs with cost less than upper_threshold
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[j];
      //vcl_cout << p.second->final_norm_cost() << " ";
      if (p.second->final_norm_cost() > upper_threshold)  // this model patch's j th match does not satisfy 
        break;
      //find index of this patch using its id
      vcl_map<int, unsigned>::iterator it = map2_dummy.find(p.first);
      if (it == map2_dummy.end())
        continue;
      unsigned ind = it->second;
      valid[cnt][ind] = p.second->final_norm_cost();
      valid_cnt++;
    }
    //vcl_cout << "\n";
  }
}

//: takes in the list of valid pairs and generates a list of consistent quads
//  the consistancy measure is very simple: compute the geometric transformation induced by each matching pair
//                                          and compute the Lie distance between the matrices
bool dbskr_shock_patch_match::find_geom_consistent_quads(vcl_vector<vcl_vector<float> >& valid, 
             vcl_map<vcl_pair<vcl_pair<unsigned, unsigned>, vcl_pair<unsigned, unsigned> >, bool>& cons_quads, 
             float upper_threshold, float Lie_dist_threshold) {
               
  unsigned n1 = map1_.size();
  unsigned n2 = map2_.size();

  if (!n1 || !n2 || n1 != patch_cor_map_.size())
    return false;

  //: make a dummy map for indices
  vcl_map<int, unsigned> map2_dummy;
  unsigned cnt = 0;
  for (vcl_map<int, dbskr_shock_patch_sptr>::iterator it = map2_.begin(); it != map2_.end(); it++, cnt++) 
  {  map2_dummy[it->first] = cnt;  }

  //: first compute the similarity transformations
  //compute_similarity_transformations();
  cons_quads.clear();

  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* train_patch_vec = 0;
  vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator iter = patch_cor_map_.begin();
  cnt = 0; unsigned valid_cnt = 0; 
  vcl_map<vcl_pair<unsigned, unsigned>, vnl_matrix<double> > sim_map;
  for ( ; iter != patch_cor_map_.end(); iter++, cnt++) {
    int id1 = iter->first;
    train_patch_vec = iter->second;  // this vector is sorted with respect to the costs so check directly
    if (!train_patch_vec)  // this model patch does not qualify as it does not have any matches
      continue;

    vcl_map<int, dbskr_shock_patch_sptr>::iterator it1 = map1_.find(id1);
    if (it1 == map1_.end()) {
      vcl_cout << " set the id to shock patch sptr maps in the match instance\n";
      return false;
    }
    dbskr_shock_patch_sptr sp1 = it1->second;
    
    //vcl_cout << "id1: " << id1 << " matches: ";
    for (unsigned j = 0; j < train_patch_vec->size(); j++) {
      //: check all the pairs with cost less than upper_threshold
      vcl_pair<int, dbskr_sm_cor_sptr> p = (*train_patch_vec)[j];
      //vcl_cout << p.second->final_norm_cost() << " ";
      if (p.second->final_norm_cost() > upper_threshold)  // this model patch's j th match does not satisfy 
        break;
      //find index of this patch using its id
      vcl_map<int, unsigned>::iterator it = map2_dummy.find(p.first);
      if (it == map2_dummy.end())
        continue;

      vcl_map<int, dbskr_shock_patch_sptr>::iterator it2 = map2_.find(p.first);
      if (it2 == map2_.end()) {
        vcl_cout << " set the id to shock patch sptr maps in the match instance\n";
        return false;
      }
      dbskr_shock_patch_sptr sp2 = it2->second;

      p.second->set_tree1(sp1->tree()); // assuming tree parameters are already set properly
      p.second->set_tree2(sp2->tree()); 

      vgl_h_matrix_2d<double> dummy_H; 
      if (!p.second->compute_similarity2D(dummy_H, true, 5, false, true))  // saving the transformation in dbskr_sm_cor
        continue;

      unsigned ind = it->second;
      valid[cnt][ind] = p.second->final_norm_cost();
      valid_cnt++;

      vcl_pair<unsigned, unsigned> pair(cnt, ind);
      sim_map[pair] = p.second->get_similarity_trans();
    }
    //vcl_cout << "\n";
  }
  vcl_cout << "\t" << valid_cnt << " pairs < " << upper_threshold << " ";
  
  int quad_cnt = 0;
  vcl_map<int, dbskr_shock_patch_sptr>::iterator it1_i = map1_.begin();
  for (unsigned i = 0; it1_i != map1_.end(); it1_i++, i++) {
    dbskr_shock_patch_sptr p_1_i = it1_i->second;

    vcl_map<int, dbskr_shock_patch_sptr>::iterator it1_j = it1_i;
    it1_j++;
    for (unsigned j = i+1; it1_j != map1_.end(); it1_j++, j++) {
      dbskr_shock_patch_sptr p_1_j = it1_j->second;
      if (p_1_i->start_node_id() == p_1_j->start_node_id())  // put constraint that they have different root nodes
        continue;

      vcl_map<int, dbskr_shock_patch_sptr>::iterator it2_k = map2_.begin(); 
      for (unsigned k = 0; it2_k != map2_.end(); it2_k++, k++) {  

        if (valid[i][k] > upper_threshold)
          continue;

        dbskr_shock_patch_sptr p_2_k = it2_k->second;

        vcl_pair<unsigned, unsigned> pair(i, k);
        vnl_matrix<double> M1 = sim_map[pair];

        vcl_map<int, dbskr_shock_patch_sptr>::iterator it2_m = it2_k;
        it2_m++;
        for (unsigned m = k+1; it2_m != map2_.end(); it2_m++, m++) {
          if (valid[j][m] > upper_threshold)
            continue;

          dbskr_shock_patch_sptr p_2_m = it2_m->second;
          if (p_2_k->start_node_id() == p_2_m->start_node_id()) {   // put constraint that they have different root nodes 
            //vcl_cout << "p_2_k and p_2_m share same node skipping!\n";
            continue;
          }

          quad_cnt++;

          vcl_pair<unsigned, unsigned> pair2(j, m);
          vnl_matrix<double> M2 = sim_map[pair2];

          double dist;
          if (!get_Lie_dist(M1, M2, dist)) 
            continue;
          
          if (dist < Lie_dist_threshold) {
            vcl_pair<vcl_pair<unsigned, unsigned>, vcl_pair<unsigned, unsigned> > pp(pair, pair2);
            cons_quads[pp] = true;
          }
          
        }
      }
    }
  }

  sim_map.clear();

  vcl_cout << "\t" << cons_quads.size() << " valid among " << quad_cnt << " quads\n";
  clear_similarity_transformations();

  return true;
}


bool 
dbskr_shock_patch_match::detect_instance_using_paths(vsol_box_2d_sptr& detection_box, 
    dbskr_shock_path_finder& f1, 
    dbskr_shock_path_finder& f2, 
    float threshold, float upper_threshold, float interpolate_ds, float sample_ds, dbskr_tree_edit_params& edit_params, 
    bool normalize, bool use_approx_cost, bool impose_geom_consistency, float Lie_dist_threshold, float alpha,
    vil_image_resource_sptr img1, vil_image_resource_sptr img2, vcl_string out_img)
{
  
  unsigned n1 = map1_.size();
  unsigned n2 = map2_.size();
  if (!n1 || !n2 || n1 != patch_cor_map_.size())
    return false;

  //: check path finders
  if (!f1.get_v() || !f1.construct_v())
    return false;

  if (!f2.get_v() || !f2.construct_v())
    return false;

  dbsk2d_shock_graph_sptr sg1 = f1.get_sg();
  dbsk2d_shock_graph_sptr sg2 = f2.get_sg();

  if (!sg1->number_of_vertices() || !sg2->number_of_vertices())
    return false;

  

  //: let n1 = size of storage 1
  //      n2 = size of storage 2
  //  prepare an n1xn2 size bool table that keeps track of valid pairs
  vcl_vector<float> tmp(n2, 100000.0f);
  vcl_vector<vcl_vector<float> > valid(n1, tmp);  unsigned valid_cnt = 0; 
  
  vcl_map<vcl_pair<vcl_pair<unsigned, unsigned>, vcl_pair<unsigned, unsigned> >, bool> cons_quads;
  if (impose_geom_consistency) {
    find_geom_consistent_quads(valid, cons_quads, upper_threshold, Lie_dist_threshold);
    if (!cons_quads.size()) {
      detection_box = 0;
      return true;
    }
  } else {
    find_valid_pairs(upper_threshold, valid, valid_cnt);
    vcl_cout << "\t" << valid_cnt << " pairs < " << upper_threshold << "\n";
    
    for (unsigned i = 0; i < n1; i++) {
      for (unsigned j = i+1; j < n1; j++) {
        for (unsigned k = 0; k < n2; k++) {

          if (valid[i][k] > upper_threshold)
            continue;
          vcl_pair<unsigned, unsigned> pair(i,k);

          for (unsigned m = k+1; m < n2; m++) {
            vcl_pair<unsigned, unsigned> pair2(j,m);

            vcl_pair<vcl_pair<unsigned, unsigned>, vcl_pair<unsigned, unsigned> > pp(pair, pair2);
            cons_quads[pp] = true;
          }
        }
      }
    }
    vcl_cout << " \tthere are " << cons_quads.size() << " quads\n";
    if (!valid_cnt || !cons_quads.size()) {
      detection_box = 0;
      return true;
    }
  }
  
  float min_sim = 10000000.0f, min_sim_i_k, min_sim_j_m;
  dbskr_scurve_sptr min_p_1_ij, min_p_2_km;
  dbskr_shock_patch_sptr min_p_1_i, min_p_1_j, min_p_2_k, min_p_2_m;

  vcl_map<int, dbsk2d_shock_node_sptr> sg1_map, sg2_map;
  get_node_map(sg1_map, sg1);
  get_node_map(sg2_map, sg2);

  vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_scurve_sptr>*, vcl_vector<float>*> > patch_map1; 
  vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_scurve_sptr>*, vcl_vector<float>*> > patch_map2; 

  unsigned quad_cnt = 0;
  vcl_vector<bool> tried_on_sg1(n1, false);
  vcl_map<int, dbskr_shock_patch_sptr>::iterator it1_i = map1_.begin();
  for (unsigned i = 0; it1_i != map1_.end(); it1_i++, i++) {
    dbskr_shock_patch_sptr p_1_i = it1_i->second;

    dbsk2d_shock_node_sptr node_1_i = sg1_map[p_1_i->start_node_id()];
    if (!node_1_i)
      return false;
          
    vcl_map<int, dbskr_shock_patch_sptr>::iterator it1_j = it1_i;
    it1_j++;
    for (unsigned j = i+1; it1_j != map1_.end(); it1_j++, j++) {

      dbskr_shock_patch_sptr p_1_j = it1_j->second;
      
      if (p_1_i->start_node_id() == p_1_j->start_node_id())  // put constraint that they have different root nodes
        continue;
      
      dbsk2d_shock_node_sptr node_1_j = sg1_map[p_1_j->start_node_id()];
      if (!node_1_j)
        return false;


      vcl_map<int, dbskr_shock_patch_sptr>::iterator it2_k = map2_.begin(); 
      for (unsigned k = 0; it2_k != map2_.end(); it2_k++, k++) {
        if (valid[i][k] > upper_threshold)
          continue;

        dbskr_shock_patch_sptr p_2_k = it2_k->second;
        dbsk2d_shock_node_sptr node_2_k = sg2_map[p_2_k->start_node_id()];
        if (!node_2_k)
          return false;

        vcl_map<int, dbskr_shock_patch_sptr>::iterator it2_m = it2_k;
        it2_m++;
        for (unsigned m = k+1; it2_m != map2_.end(); it2_m++, m++) {
          if (valid[j][m] > upper_threshold)
            continue;

          dbskr_shock_patch_sptr p_2_m = it2_m->second;

          //: now find the similarity of super-patches formed by this quad
          //vcl_cout << "quad: i: " << p_1_i->id() << " matches k: " << p_2_k->id() << " j: " << p_1_j->id() << " matches m: " << p_2_m->id() << vcl_endl;
 
          dbsk2d_shock_node_sptr node_2_m = sg2_map[p_2_m->start_node_id()];
          if (!node_2_m)
            return false;

          if (p_2_k->start_node_id() == p_2_m->start_node_id()) {   // put constraint that they have different root nodes 
            //vcl_cout << "p_2_k and p_2_m share same node skipping!\n";
            continue;
          }
 
          vcl_pair<unsigned, unsigned> p_ij(i, j);
          vcl_pair<unsigned, unsigned> p_km(k, m);

          vcl_pair<unsigned, unsigned> p_ik(i, k);
          vcl_pair<unsigned, unsigned> p_jm(j, m);
          
          vcl_pair<vcl_pair<unsigned, unsigned>, vcl_pair<unsigned, unsigned> > pp(p_ik, p_jm);
          //: check if this is a consistent quad
          vcl_map<vcl_pair<vcl_pair<unsigned, unsigned>, vcl_pair<unsigned, unsigned> >, bool>::iterator itq = cons_quads.find(pp);
          if (itq == cons_quads.end()) {
            continue;
          }          


          vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_scurve_sptr>*, vcl_vector<float>*> >::iterator it = patch_map1.find(p_ij); 
          vcl_vector<dbskr_scurve_sptr>* scurves1;
          vcl_vector<float>* lengths1;
          if (it == patch_map1.end()) {
            scurves1 = new vcl_vector<dbskr_scurve_sptr>();
            lengths1 = new vcl_vector<float>();
            f1.get_all_scurves(node_1_i, node_1_j, *scurves1, *lengths1, 1.0f, 100000.0f, interpolate_ds, sample_ds);

            patch_map1[p_ij] = vcl_pair<vcl_vector<dbskr_scurve_sptr>*, vcl_vector<float>* >(scurves1, lengths1);
          } else {
            scurves1 = it->second.first;
            lengths1 = it->second.second;
          }

          if (!lengths1->size()) {
            vcl_cout << "no patches1 for this quad!!\n";
            continue;
          } 

          // for now find the max lengths1
          float max_l = *(max_element(lengths1->begin(), lengths1->end()));
          //: find the box of p_1_i and p_1_j and normalize by its perimeter
          vsol_box_2d_sptr box = p_1_i->union_box(*p_1_j);
          float box_perim = (float)(2*box->width() + 2*box->height());
          //: set the threshold to be 1.5*max_length as normalized
          float norm_thres = 1.5f*max_l/box_perim;

          //: find the normalization factor for the second pair
          vsol_box_2d_sptr box2 = p_2_k->union_box(*p_2_m);
          float box2_perim = (float)(2*box2->width() + 2*box2->height());
          

          
          it = patch_map2.find(p_km);
          vcl_vector<dbskr_scurve_sptr>* scurves2;
          vcl_vector<float>* lengths2;
          if (it == patch_map2.end()) {
            scurves2 = new vcl_vector<dbskr_scurve_sptr>();
            lengths2 = new vcl_vector<float>();
            f2.get_all_scurves(node_2_k, node_2_m, *scurves2, *lengths2, box2_perim, norm_thres, interpolate_ds, sample_ds);
            
            patch_map2[p_km] = vcl_pair<vcl_vector<dbskr_scurve_sptr>*, vcl_vector<float>* >(scurves2, lengths2);
          } else {
            scurves2 = it->second.first;
            lengths2 = it->second.second;
          }

          //: find the best matching graph pair, and assign its similarity
          for (unsigned ii = 0; ii < scurves1->size(); ii++) {
            dbskr_scurve_sptr p_ii = (*scurves1)[ii];

            //vcl_vector<dbskr_scurve_sptr> scurves2;
            //vcl_vector<float> lengths2;       
            //float max_l = (*lengths1)[ii];
            //float norm_thres = 1.5f*max_l/box_perim;
            //f2.get_all_scurves(node_2_k, node_2_m, scurves2, lengths2, box2_perim, norm_thres, interpolate_ds, sample_ds);

            for (unsigned jj = 0; jj < scurves2->size(); jj++) {
              dbskr_scurve_sptr p_jj = (*scurves2)[jj];
              float norm_factor = (float)(p_ii->boundary_plus_length()+p_ii->boundary_minus_length()+p_jj->boundary_plus_length()+p_jj->boundary_minus_length());
              
              dbskr_dpmatch d(p_ii, p_jj);
              d.set_R(edit_params.curve_matching_R_);

              float match_val;

              if (use_approx_cost) {
                // test if it is worth computing this match
                double approx_cost = d.coarse_match();
                //vcl_cout << "approx: " << approx_cost << " approx norm: " << approx_cost/norm_factor << " ";
                match_val = normalize ? (float)approx_cost/norm_factor : (float)approx_cost;
              } else {
                d.Match();
                float init_dr = d.init_dr();
                float init_alp = d.init_phi();
                float match_val = float(d.finalCost() + init_dr + init_alp);
                  
                //vcl_cout << " cost: " << match_val << " " << " norm cost: " << match_val/norm_factor << " ";
                match_val = normalize ? match_val/norm_factor : match_val;
              }

              //vcl_cout << " sim1: " << valid[i][k] << " sim2: " << valid[j][m] << "\n";
              float final_sim = alpha*match_val + ((1.0f-alpha)/2.0f)*valid[i][k] + ((1.0f-alpha)/2.0f)*valid[j][m];
              d.clear();
              if (final_sim < min_sim) {
                min_sim = final_sim;
                min_p_1_ij = p_ii;
                min_p_2_km = p_jj;
                min_p_1_i = p_1_i; min_p_1_j = p_1_j;
                min_p_2_k = p_2_k; min_p_2_m = p_2_m;
                min_sim_i_k = valid[i][k];
                min_sim_j_m = valid[j][m];
              }
            }
          }
         
          quad_cnt++;
        }
      }
    }
  }
  //: clear the maps
  for (vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_scurve_sptr>*, vcl_vector<float>*> >::iterator it = patch_map1.begin(); it != patch_map1.end(); it++) {
    it->second.first->clear();
    it->second.second->clear();
    delete it->second.first;
    delete it->second.second;
  }

  for (vcl_map<vcl_pair<unsigned, unsigned>, vcl_pair<vcl_vector<dbskr_scurve_sptr>*, vcl_vector<float>*> >::iterator it = patch_map2.begin(); it != patch_map2.end(); it++) {
    it->second.first->clear();
    it->second.second->clear();
    delete it->second.first;
    delete it->second.second;
  }

  vcl_cout << quad_cnt << " quads\n";

  if (!min_p_1_ij || !min_p_2_km) {  // no min found, no valid quads with distinct root nodes
    detection_box = 0;
    return true;
  }

  vcl_cout << "\tmin_sim: " << min_sim << " sim i,k: " << min_sim_i_k << " sim j,m: " << min_sim_j_m << vcl_endl;

  if (img1 != 0 && img2 != 0) {  // prepare output images
    vcl_vector<dbskr_shock_patch_sptr> ps;  vcl_vector<vil_rgb<int> > colors;
    ps.push_back(min_p_1_i);  colors.push_back(vil_rgb<int>(1,0,0));
    ps.push_back(min_p_1_j);  colors.push_back(vil_rgb<int>(0,1,0));
    vcl_vector<vil_rgb<int> > curve_colors; 
    curve_colors.push_back(vil_rgb<int>(0,0,1)); curve_colors.push_back(vil_rgb<int>(1,1,0)); curve_colors.push_back(vil_rgb<int>(1,0,1));
    create_ps_patches_with_scurve(out_img+"_with_curves_1.ps", ps, colors, min_p_1_ij, curve_colors, img1);

    ps.clear();
    ps.push_back(min_p_2_k);  
    ps.push_back(min_p_2_m);  
    create_ps_patches_with_scurve(out_img+"_with_curves_2.ps", ps, colors, min_p_2_km, curve_colors, img2);
  }

  if (min_sim < threshold) {
    //: the output box is the union of boxes of the min sim patches on the query image
    detection_box = new vsol_box_2d();
    vsol_box_2d_sptr box = min_p_2_m->shock_graph()->get_bounding_box(); 
    if (!box && !dbsk2d_compute_bounding_box(min_p_2_m->shock_graph())) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_m->id() << " in the second storage " << vcl_endl;
      return false;
    }
    box = min_p_2_m->shock_graph()->get_bounding_box(); 
    if (!box) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_m->id() << " in the second storage " << vcl_endl;
      return false;
    }
    detection_box->grow_minmax_bounds(box);

    box = min_p_2_k->shock_graph()->get_bounding_box(); 
    if (!box && !dbsk2d_compute_bounding_box(min_p_2_k->shock_graph())) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_k->id() << " in the second storage " << vcl_endl;
      return false;
    }
    box = min_p_2_k->shock_graph()->get_bounding_box(); 
    if (!box) {
      vcl_cout << "dbskr_shock_patch_match::detect_instance() - unable to compute shock graph bounding box of patch with id: " << min_p_2_k->id() << " in the second storage " << vcl_endl;
      return false;
    }
    detection_box->grow_minmax_bounds(box);

    //vcl_cout << "box area: " << detection_box->area() << "\n";
  } else {
    detection_box = 0;
  }
  
  return true;
}



bool 
dbskr_shock_patch_match::
create_match_ps_images(vcl_string image_dir, 
                       vcl_string name1, 
                       dbsk2d_shock_graph_sptr base_sg1, 
                       vcl_string name2, 
                       dbsk2d_shock_graph_sptr base_sg2)
{
  //vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* > 
  patch_cor_map_iterator iter;
  for (iter = patch_cor_map_.begin(); iter != patch_cor_map_.end(); iter++) {
    dbskr_shock_patch_sptr msp = map1_[iter->first];
    vcl_ostringstream oss1;
    oss1 << msp->id();

    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;
    for (vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >::iterator it_m = match_vec->begin(); it_m != match_vec->end(); it_m++) {
      dbskr_shock_patch_sptr qsp = map2_[it_m->first];
      vcl_ostringstream oss;
      oss << qsp->id();
    
      vcl_string match_image_file = image_dir + name1 + "_" + oss1.str() + "_" + name2 + "_" + oss.str() + ".ps";      
      if (!create_ps_shock_matching(match_image_file, base_sg1, msp->tree(), base_sg2, qsp->tree(), it_m->second)) {
        vcl_cout << "In dbskr_shock_patch_match::create_match_ps_images() -- cannot create ps image: " << match_image_file << vcl_endl;
        return false;
      }
    } 
  }

  return true;
}

//: create the html table with similarities for this match
//  image width is in pixels
bool dbskr_shock_patch_match::create_html_table(vcl_string image_dir, 
                                                vcl_string name1, 
                                                vcl_string name2, 
                                                vcl_string out_html, 
                                                vcl_string table_caption, 
                                                vcl_string image_ext, int image_width, bool put_match_images)
{
  vcl_ofstream tf(out_html.c_str(), vcl_ios::app);
  
  if (!tf) {
    vcl_cout << "Unable to open output html file " << out_html << " for write " << vcl_endl;
    return false;
  }
  vcl_stringstream wstr, wstr2;
  wstr << image_width;
  wstr2 << 2*image_width;

  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\">" << table_caption << "</caption>\n";
  tf << "<TR> <TH>   ";  // leave the first cell of first row empty

  // write the first row
  for (unsigned i = 0; int(i) < map2_.size(); i++) 
    tf << "<TH> q patch " << i+1 << " ";
  tf << "</TH> </TR>\n";
  
  //vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* > 
  patch_cor_map_iterator iter;
  for (iter = patch_cor_map_.begin(); iter != patch_cor_map_.end(); iter++) {
    dbskr_shock_patch_sptr msp = map1_[iter->first];
    vcl_ostringstream oss1;
    oss1 << msp->id();
    vcl_string patch_image_file = image_dir + name1 + "_" + oss1.str() + image_ext;
    vcl_cout << "putting model image: " << patch_image_file << vcl_endl;
    tf << "<TR> <TD> <img src=\"" << patch_image_file << "\" width=" << wstr.str() << "> ";
    tf << msp->id() << " </TD> "; 
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;

    //: find the similarity to all query patches using match_vec
    for (vcl_map<int, dbskr_shock_patch_sptr>::iterator itq = map2_.begin(); itq != map2_.end(); itq++) {
      //: find the similarity of this query to the current model patch
      dbskr_sm_cor_sptr current_sm = 0;
      dbskr_shock_patch_sptr current_tsp = itq->second;
      for (unsigned kk = 0; kk < match_vec->size(); kk++)
        if ((*match_vec)[kk].first == itq->first) {
          current_sm = (*match_vec)[kk].second;
          break;
        }

      if (!current_sm)
       tf << "<TD> <img src=\"unknown\"> </TD> ";
      else {
        vcl_ostringstream oss;
        oss << current_tsp->id();
        vcl_string patch_image_file = image_dir + name2 + "_" + oss.str() + image_ext;
        tf << "<TD> <img src=\"" << patch_image_file << "\" width=" << wstr.str() << "> ";
        tf << current_tsp->id() << " sim: " << current_sm->final_norm_cost() << " </TD> ";
      }
    }
    tf << "</TR>\n";
    if (put_match_images) {
      tf << "<TR> <TD> </TD> "; 
      vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;

      //: find the similarity to all query patches using match_vec
      for (vcl_map<int, dbskr_shock_patch_sptr>::iterator itq = map2_.begin(); itq != map2_.end(); itq++) {
        //: find the similarity of this query to the current model patch
        dbskr_sm_cor_sptr current_sm = 0;
        dbskr_shock_patch_sptr current_tsp = itq->second;
        for (unsigned kk = 0; kk < match_vec->size(); kk++)
          if ((*match_vec)[kk].first == itq->first) {
            current_sm = (*match_vec)[kk].second;
            break;
          }

        if (!current_sm)
          tf << "<TD> <img src=\"unknown\"> </TD> ";
        else {
          vcl_ostringstream oss;
          oss << current_tsp->id();
          vcl_string patch_match_image_file = image_dir + name1 + "_" + oss1.str() + "_" + name2 + "_" + oss.str() + image_ext;
          tf << "<TD> <img src=\"" << patch_match_image_file << "\" width=" << wstr2.str() << "> </TD> ";
        }
      }
      tf << "</TR>\n";
    }
  }
  
  tf << "</TABLE>\n";
  tf.close();
  return true;
}

//: create the html table with similarities for this match
//  image width is in pixels
bool dbskr_shock_patch_match::create_html_rank_order_table(
    vcl_string image_dir, 
    vcl_string name1, 
    vcl_string name2, 
    vcl_string out_html, 
    vcl_string table_caption, 
    vcl_string image_ext,  
    bool put_match_images)
{
  vcl_ofstream tf(out_html.c_str(), vcl_ios::trunc);
  
  if (!tf) {
    vcl_cout << "Unable to open output html file " 
             << out_html << " for write " << vcl_endl;
    return false;
  }
  
  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\">" << table_caption << "</caption>\n";
  tf << "<TR> <TH>   ";  // leave the first cell of first row empty

  // write the first row
  for (unsigned i = 0; int(i) < map2_.size(); i++) 
  {
    tf << "<TH> Match " << i+1 << " ";
  }
  tf << "</TH> </TR>\n";
  
  //vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* > 
  patch_cor_map_iterator iter;
  for (iter = patch_cor_map_.begin(); iter != patch_cor_map_.end(); iter++) 
  {
    dbskr_shock_patch_sptr msp = map1_[iter->first];
    vcl_ostringstream oss1;
    oss1 << msp->id();
    vcl_string patch_image_file = image_dir + name1 + "_" + oss1.str() 
        + image_ext;
    vcl_cout << "putting model image: " << patch_image_file << vcl_endl;  

    tf << "<TR> <TD> <img src=\"" << patch_image_file 
       << "\" > ";
    tf << msp->id() << " </TD> "; 
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;

    // Loop over the model patches that are sorted
    // and find the query patch in map 2 that has the same id
    for (unsigned kk = 0; kk < match_vec->size(); kk++)
    {
        
        dbskr_sm_cor_sptr current_sm = 0;
        dbskr_shock_patch_sptr current_tsp = 0;
         
        //: find the similarity to all query patches using match_vec
        for (
            vcl_map<int, dbskr_shock_patch_sptr>::iterator itq = map2_.begin(); 
            itq != map2_.end(); itq++
            ) 
        {
            //: find the similarity of this query to the current model patch
            current_sm = 0;
            current_tsp = itq->second;

            // If a match we have found query and break
            if ((*match_vec)[kk].first == itq->first) 
            {
                current_sm = (*match_vec)[kk].second;
                break;
            }
        }

        if (!current_sm)
        {
            tf << "<TD> <img src=\"unknown\"> </TD> ";
        }
        else 
        {
            vcl_ostringstream oss;
            oss << current_tsp->id();
            vcl_string patch_image_file = image_dir + name2 + "_" 
                + oss.str() + image_ext;

            tf << "<TD> <img src=\"" 
               << patch_image_file 
               << "\" > ";

            tf << current_tsp->id() 
               << " sim: " 
               << current_sm->final_norm_cost() 
               << " </TD> ";

        }
       
        
    }
    tf << "</TR>\n";

    if (put_match_images)
    {
        tf << "<TR> <TD> </TD> ";
        // Loop over the model patches that are sorted
        // and find the query patch in map 2 that has the same id
        for (unsigned kk = 0; kk < match_vec->size(); kk++)
        {
        
            dbskr_sm_cor_sptr current_sm = 0;
            dbskr_shock_patch_sptr current_tsp = 0;
         
            //: find the similarity to all query patches using match_vec
            for (
                vcl_map<int, dbskr_shock_patch_sptr>::iterator itq 
                    = map2_.begin(); 
                itq != map2_.end(); itq++
                ) 
            {
                //: find the similarity of this query to the current model patch
                current_sm = 0;
                current_tsp = itq->second;

                // If a match we have found query and break
                if ((*match_vec)[kk].first == itq->first) 
                {
                    current_sm = (*match_vec)[kk].second;
                    break;
                }
            }

            if (!current_sm)
            {
                tf << "<TD> <img src=\"unknown\"> </TD> ";
            }
            else 
            {
                vcl_ostringstream oss;
                oss << current_tsp->id();
                vcl_string patch_match_image_file = image_dir + name1 + 
                    "_" + oss1.str() + "_" + name2 + "_" + oss.str() 
                    + image_ext;

                tf << "<TD> <img src=\"" << patch_match_image_file 
                   << "\" > </TD> ";
   
   
            }
        }
        tf << "</TR>\n";
    }
  }
     
  
  tf << "</TABLE>\n";
  tf.close();
  return true;
}
