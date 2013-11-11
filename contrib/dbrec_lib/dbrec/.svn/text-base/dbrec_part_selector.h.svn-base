//:
// \file
// \brief part selector class inheriting from dbfs_measure to provide density and entropy measurement interface for part selection from training data
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   July 24, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//
//
//

#if !defined(_dbrec_part_selector_h)
#define _dbrec_part_selector_h

#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#include <dbfs/dbfs_measure.h>
#include <dbfs/dbfs_selector_sptr.h>
#include <dbrec/dbrec_part_sptr.h>
#include <dbrec/dbrec_part_context_sptr.h>
#include <vil/vil_image_view.h>
#include <vcl_map.h>
#include <vcl_utility.h>

//: during training, an instance of this selection measure should be created, it collects contexts from all the training images and makes the necessary measurements
//
//  we define the probability of observing a feature (part) on the images of a class
//  as area_obj/total_area where total_area is the total region of training images marked with this part's receptive fields in that class
//  and the area_obj is the total region of objects in the training images marked with this part's receptive fields in that class
//  so parts covering the object areas as selectively as possible are preferred
//
class dbrec_part_selection_measure : public dbfs_measure {
public:
  dbrec_part_selection_measure(const vcl_vector<dbrec_part_sptr>& parts, int class_cnt);

  //: measure the areas for the parts
  //  only the pixels which are true in the valid_region_mask are used in the measures, 
  //  the pixels with values larger than 0.9f in the obj_map are used in the measures as object regions, if a ground-truth map is being used, it should be converted to a float image and passed as obj_map
  void measure_training_image(dbrec_context_factory_sptr cf, const vil_image_view<bool>& valid_region_mask, const vil_image_view<float>& obj_map, int class_id);

  //:  the feature_id here is the type id of the part
  float prob(int class_id, int feature_id);
  float prob_class(int class_id);   // marginal prob of the class (number of classes is the number of root nodes in the hierarchy)
  float prob_feature(int feature_id);   // marginal prob of the feature
  float mutual_info_class_average(int feature_id);  // I(C, F=f)

  int get_class_cnt() const { return class_cnt_; }
  vcl_vector<dbrec_part_sptr>& get_parts() { return parts_; }

protected:
  //: cache the total regions and the total object regions for the parts
  void cache_values();

  int class_cnt_;
  float total_training_area_; // the total area of the training images seen

  vcl_vector<dbrec_part_sptr> parts_;   
  //                            area_obj, total_area
  vcl_vector<vcl_map<int, vcl_pair<float, float> > > areas_;  // for each class and for each part cache the area measures
  //      area_obj in class, total_class_area
  vcl_vector<vcl_pair<float, float> > class_areas_;  // total area of images from each class
  
};

typedef vbl_smart_ptr<dbrec_part_selection_measure> dbrec_part_selection_measure_sptr;

class dbrec_part_selector_algos {
public: 
  enum algos {
    max_class_mutual_info,
    max_class_min_non_class_mutual_info
  };
};

//: a wrapper class around dbfs_selector to handle dbrec related operations
class dbrec_part_selector {
public:
  //: the hierarchy makes sure that the selected parts for a class are the parts that were populated for that class
  dbrec_part_selector(dbrec_part_selection_measure_sptr sm, unsigned selection_algo_type, vcl_vector<dbrec_part_sptr>& parts, dbrec_hierarchy_sptr h, int class_cnt);

  //: return the top k features in the set, just a wrapper around the selector's corresponding method
  void get_top_features(int class_id, int k, vcl_vector<dbrec_part_sptr>& best_features); 

protected:
  dbrec_hierarchy_sptr h_;
  dbfs_selector_sptr sel_;
  vcl_map<int, dbrec_part_sptr> part_map_;  // keep a map for fast access
};

// Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_part_selection_measure as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_part_selection_measure const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec_part_selection_measure &ph);
void vsl_b_read(vsl_b_istream& is, dbrec_part_selection_measure* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec_part_selection_measure* &ph);


#endif  //_dbrec_part_selector_h
