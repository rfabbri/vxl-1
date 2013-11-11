//:
// \file
// \author Ozge C. Ozcanli (Brown)
// \date   July 23, 2009
//
#include "dbrec_part_selector.h"
#include "dbrec_part.h"
#include "dbrec_part_context.h"
#include "dbrec_image_visitors.h"
#include <dbfs/dbfs_selector.h>
#include <vcl_cmath.h>
#include <vcl_limits.h> 

dbrec_part_selection_measure::dbrec_part_selection_measure(const vcl_vector<dbrec_part_sptr>& parts, int class_cnt) : class_cnt_(class_cnt), total_training_area_(0.0f), parts_(parts)
{
  vcl_map<int, vcl_pair<float, float> > feature_map;
  for (unsigned i = 0; i < parts_.size(); i++) {
    vcl_pair<float, float> empty_pair(0.0f, 0.0f);
    feature_map[parts_[i]->type()] = empty_pair;
  }
  // for each class and for each part, initialize the area measures to zero
  for (int i = 0; i < class_cnt_; i++) {
    areas_.push_back(feature_map);
    vcl_pair<float, float> empty_pair(0.0f, 0.0f);
    class_areas_.push_back(empty_pair);
  }
}

//: measure the areas for the parts
//  only the pixels which are true in the valid_region_mask are used in the measures, 
//  the pixels with values larger than 0.9f in the obj_map are used in the measures as object regions, if a ground-truth map is being used, it should be converted to a float image and passed as obj_map
void dbrec_part_selection_measure::measure_training_image(dbrec_context_factory_sptr cf, const vil_image_view<bool>& valid_region_mask, const vil_image_view<float>& obj_map, int class_id)
{
  //: class area is the total area of the images in the training set from this class, class_obj_area is the total area of the instances of the classes in the training set
  float class_area = 0.0f;
  float class_obj_area = 0.0f;
  for (unsigned i = 0; i < valid_region_mask.ni(); i++) {
    for (unsigned j = 0; j < valid_region_mask.nj(); j++) {
      if (valid_region_mask(i,j)) {
        class_area += 1.0f;
        if (obj_map(i,j) > 0.9f) {
          class_obj_area += 1.0f;
        }
      }
    }
  }
  class_areas_[class_id].first += class_obj_area;
  class_areas_[class_id].second += class_area;
  total_training_area_ += class_area;

  for (unsigned pi = 0; pi < parts_.size(); pi++) {
    dbrec_part_sptr p = parts_[pi];
    //: create the receptive field map for this part
    dbrec_part_context_sptr pc = cf->get_context(p->type());
    if (!pc) {
      vcl_cout << "In dbrec_part_selection_measure::measure_training_image() -- Problems in retrieving context of type: " << p->type() << "!\n";
      throw 0;
    }

    vil_image_resource_sptr cfm = pc->get_posterior_map(0, obj_map.ni(), obj_map.nj());

    if (!cfm) {
      //vcl_cout << "In dbrec_part_selection_measure::measure_training_image() -- No parts detected for part: " << p->type() << " no measurements!\n";
      continue;
    }
    
    vil_image_view<float> out = cfm->get_view();
 
    dbrec_mark_receptive_fields_visitor mrfv(pc, out);
    p->accept(&mrfv);

    vil_image_view<float> out_with_receptive_fields = mrfv.get_rec_field_map();

    //: measure the area that overlaps with the object
    float obj_area = 0.0f;
    float total_area = 0.0f;
    for (unsigned i = 0; i < obj_map.ni(); i++) {
      for (unsigned j = 0; j < obj_map.nj(); j++) {
        if (valid_region_mask(i,j)) {
          total_area += out_with_receptive_fields(i,j);
          // we only use the regions exactly on top of the object for now but we may try to include close vicinity into the  measuring region using something like this: obj_map_value = g->fg_prob_operator(prob_img_, i,j);
          if (obj_map(i,j) > 0.9f) {
            obj_area += out_with_receptive_fields(i,j);
          }
        }
      }
    }
    vcl_map<int, vcl_pair<float, float> >::iterator it = (areas_[class_id]).find(p->type());
    it->second.first += obj_area;
    it->second.second += total_area;
  }
  
}


//: measure the probability of observing this feature on the images of this class
//  we define this as area_obj/total_area where total_area is the total area of training images
//  and the area_obj is the total region of "objects" from this class in the training images marked with this part's receptive fields
//  so parts covering the object areas are preferred
//  the feature_id here is the type id of the part
float dbrec_part_selection_measure::prob(int class_id, int feature_id)
{
  vcl_map<int, vcl_pair<float, float> >::iterator it = (areas_[class_id]).find(feature_id);
  if (it == areas_[class_id].end())
    throw 0;
  if (it->second.first < float(1.0e-5))
    return vcl_numeric_limits<float>::epsilon();  // don't return zero!
  return it->second.first/total_training_area_;
}

//: the total area of the objects in the training images from this class over total area of the images in this class
float dbrec_part_selection_measure::prob_class(int class_id)   // marginal prob of the class
{
  if (class_id >= (int)class_areas_.size())
    throw 0;
  if (class_areas_[class_id].first < float(1.0e-5))
    return vcl_numeric_limits<float>::epsilon();  // don't return zero!
  return class_areas_[class_id].first/total_training_area_;
}
//: the total area of the feature in each class over total area of the training set
float dbrec_part_selection_measure::prob_feature(int feature_id)   // marginal prob of the feature
{
  float total_feature_area = 0.0f;
  //: sum up the areas of the feature in all classes
  for (unsigned i = 0; i < class_areas_.size(); i++) {
    vcl_map<int, vcl_pair<float, float> >::iterator it = (areas_[i]).find(feature_id);
    if (it == areas_[i].end())
      throw 0;
    total_feature_area += it->second.second;
  }
  if (total_feature_area < float(1.0e-5))
    return vcl_numeric_limits<float>::epsilon();  // don't return zero!
  return total_feature_area/total_training_area_;
}
float dbrec_part_selection_measure::mutual_info_class_average(int feature_id)  // I(C, F=f)
{
  float sum = 0.0f;
  for (int i = 0; i < class_cnt_; i++) {
    sum += this->mutual_info(i, feature_id);
  }
  return sum;
}

dbrec_part_selector::dbrec_part_selector(dbrec_part_selection_measure_sptr sm, unsigned selection_algo_type, vcl_vector<dbrec_part_sptr>& parts, dbrec_hierarchy_sptr h, int class_cnt) : h_(h)
{
  vcl_vector<int> part_ids;
  for (unsigned i = 0; i < parts.size(); i++) {
    int type = parts[i]->type();
    part_ids.push_back(type);
    part_map_[type] = parts[i];
  }
  dbfs_measure_sptr smm = sm.ptr();
  switch(selection_algo_type) {
    case dbrec_part_selector_algos::max_class_mutual_info : { sel_ = new dbfs_selector(smm, part_ids, class_cnt); break; }
    case dbrec_part_selector_algos::max_class_min_non_class_mutual_info : { sel_ = new dbfs_selector_max_class_min_non_class(smm, part_ids, class_cnt); break; }
    default: { vcl_cout << "In dbrec_part_selector() -- no such selection algo!\n"; throw 0; }
  }
}

//: return the top k features in the set, just a wrapper around the selector's corresponding method
//  check if the parts are the same (if they are compositional parts are the ids equal if not are the children the same?)
void dbrec_part_selector::get_top_features(int class_id, int k, vcl_vector<dbrec_part_sptr>& best_features)
{
  best_features.clear();
  
  dbrec_part_sptr class_root = h_->root(class_id);
  if (!class_root)
    throw 0;

  int current_cnt = 0;
  int feature_id;
  while (sel_->get_next_feature(class_id, current_cnt, feature_id)) {
    dbrec_part_sptr p = part_map_[feature_id];
    //: check if this feature had been initialized for this class
    dbrec_part_sptr dummy = class_root->get_part(p->type());
    if (!dummy) {
      continue;
    }
    best_features.push_back(p);
    if ((int)best_features.size() >= k)
      break;
  }

  //sel_->get_top_features(class_id, k, best_ids);
  //for (unsigned i = 0; i < best_ids.size(); i++) {
  //  best_features.push_back(part_map_[best_ids[i]]);
  //}
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_part_selection_measure as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_part_selection_measure const &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, dbrec_part_selection_measure &ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, dbrec_part_selection_measure* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    vcl_vector<dbrec_part_sptr> parts;
    ph = new dbrec_part_selection_measure(parts, 0);
    vsl_b_read(is, *ph);
  }
  else
    ph = 0;
}

void vsl_b_write(vsl_b_ostream& os, const dbrec_part_selection_measure* &ph)
{
  if (ph==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}
