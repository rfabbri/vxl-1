//:
// \file
// \author Ozge C. Ozcanli (Brown)
// \date   July 23, 2009
//
#include "dbfs_selector.h"
#include "dbfs_measure.h"


#include <vcl_algorithm.h>
#include <vcl_iostream.h>

dbfs_selector::dbfs_selector(dbfs_measure_sptr ms, vcl_vector<int>& features, int class_cnt) : measurer_(ms), features_(features), class_cnt_(class_cnt), mutual_infos_calculated_(class_cnt, false), arr_(class_cnt, vcl_vector<vcl_pair<int, float> >())
{
}
//: initializes the feature ids from 0 to feature_cnt-1 in the features_ vector
dbfs_selector::dbfs_selector(dbfs_measure_sptr ms, int feature_cnt, int class_cnt) : measurer_(ms), class_cnt_(class_cnt), mutual_infos_calculated_(class_cnt, false), arr_(class_cnt, vcl_vector<vcl_pair<int, float> >())
{
  for (int i = 0; i < feature_cnt; i++)
    features_.push_back(i);
}

bool sort_by_mi(const vcl_pair<int, float>& lhs, const vcl_pair<int, float>& rhs )
{
  return lhs.second > rhs.second;
}

//: override this method to get different sets of features, the base class implementation just sorts them wrt mutual info 
void dbfs_selector::select_features(int class_id)
{
  arr_[class_id].clear();
  //: calculate mutual infos for each feature
  for (unsigned i = 0; i < features_.size(); i++) {
    float mi = measurer_->mutual_info(class_id, features_[i]);
    vcl_pair<int, float> id_mi_pair(features_[i], mi);
    arr_[class_id].push_back(id_mi_pair);
    vcl_cout << "(" << features_[i] << ", " << mi << ") ";
  }
  vcl_sort(arr_[class_id].begin(), arr_[class_id].end(), sort_by_mi);
  mutual_infos_calculated_[class_id] = true;
}

//: maximize class mutual info while minimizing "non-class" mutual info of the features
void dbfs_selector_max_class_min_non_class::select_features(int class_id)
{
  arr_[class_id].clear();
  //: calculate mutual infos for each feature
  for (unsigned i = 0; i < features_.size(); i++) {
    float mi = measurer_->mutual_info(class_id, features_[i]);
    for (int c = 0; c < class_cnt_; c++) {
      if (c == class_id) continue;
      float mi_nc = measurer_->mutual_info(c, features_[i]);
      mi -= mi_nc;
    }
    vcl_pair<int, float> id_mi_pair(features_[i], mi);
    arr_[class_id].push_back(id_mi_pair);
    vcl_cout << "(" << features_[i] << ", " << mi << ") ";
  }
  vcl_sort(arr_[class_id].begin(), arr_[class_id].end(), sort_by_mi);
  mutual_infos_calculated_[class_id] = true;
}

//: return the next feature in the selected/sorted feature list
bool dbfs_selector::get_next_feature(int class_id, int& current_cnt, int& feature_id)
{
  if (!mutual_infos_calculated_[class_id])
    select_features(class_id);

  if (current_cnt >= (int)arr_[class_id].size())
    return false;
  feature_id = arr_[class_id][current_cnt].first;
  current_cnt++;
  return true;
}

//: return the top k features in the set
void dbfs_selector::get_top_features(int class_id, int k, vcl_vector<int>& best_features)
{
  if (!mutual_infos_calculated_[class_id])
    select_features(class_id);

  best_features.clear();
  int s = k < (int)arr_[class_id].size() ? k : arr_[class_id].size();
  for (int i = 0; i < s; i++) {
    vcl_cout << "(" << arr_[class_id][i].first << ", " << arr_[class_id][i].second << ") ";
    best_features.push_back(arr_[class_id][i].first);
  }
  vcl_cout << "\n";
}
