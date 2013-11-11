//:
// \file
// \brief base class to implement various feature selection methods
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   July 23, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//
//
//


#if !defined(_dbfs_selector_h)
#define _dbfs_selector_h

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include "dbfs_measure_sptr.h"

class dbfs_selector : public vbl_ref_count
{
public:
  dbfs_selector(dbfs_measure_sptr ms, vcl_vector<int>& features, int class_cnt);
  //: initializes the feature ids from 0 to feature_cnt-1 in the features_ vector
  dbfs_selector(dbfs_measure_sptr ms, int feature_cnt, int class_cnt);
  
  //: override this method to get different sets of features, the base class implementation just sorts them wrt mutual info 
  virtual void select_features(int class_id);

  //: return the top k features in the set, 
  void get_top_features(int class_id, int k, vcl_vector<int>& best_features); 

  //: return the next feature in the selected/sorted feature list
  bool get_next_feature(int class_id, int& current_cnt, int& feature_id); 

protected:
  dbfs_measure_sptr measurer_;     // the helper clas
  vcl_vector<int> features_;  // each feature needs to be given a unique identifier
  int class_cnt_;             // 2 for class&non-class (binary classifications)

  vcl_vector<bool> mutual_infos_calculated_;
  vcl_vector<vcl_vector<vcl_pair<int, float> > > arr_;
  
};

class dbfs_selector_max_class_min_non_class : public dbfs_selector {
public:
  dbfs_selector_max_class_min_non_class(dbfs_measure_sptr ms, vcl_vector<int>& features, int class_cnt) : dbfs_selector(ms, features, class_cnt) {}
  dbfs_selector_max_class_min_non_class(dbfs_measure_sptr ms, int feature_cnt, int class_cnt) : dbfs_selector(ms, feature_cnt, class_cnt) {}

  //: maximize class mutual info while minimizing "non-class" mutual info of the features
  virtual void select_features(int class_id);
};

#endif  //_dbfs_selector_h
