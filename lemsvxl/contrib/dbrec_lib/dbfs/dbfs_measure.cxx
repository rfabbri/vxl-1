//:
// \file
// \author Ozge C. Ozcanli (Brown)
// \date   July 23, 2009
//
#include "dbfs_measure.h"
#include <vcl_cmath.h>
#include <vcl_limits.h> 

void dbfs_measure_simple::cache_values()
{
  if (!counts_.size() || !counts_[0].size())
    return;

  //: cache these for fast access
  total_doc_count_ = 0;
  feature_counts_.clear();
  feature_counts_.assign(counts_[0].size()/2, 0);
  class_counts_.assign(counts_.size(), 0);
  for (unsigned i = 0; i < counts_.size(); i++) {
    int class_cnt = 0;
    class_counts_[i] = counts_[i][0] + counts_[i][1];  // add the number of docs containing and not containing the first feature
    for (unsigned j = 0; j < counts_[i].size(); j+=2) {
      feature_counts_[j/2] += counts_[i][j];
    }
  }
  //: assumes each document belongs to only one class in the training set
  for (unsigned i = 0; i < class_counts_.size(); i++)
    total_doc_count_ += class_counts_[i];
}
float dbfs_measure_simple::prob(int class_id, int feature_id)
{
  if (!total_doc_count_)
    cache_values();
  if (class_id >= (int)counts_.size() || feature_id >= (int)counts_[class_id].size())
    return -1;
  return float(counts_[class_id][2*feature_id])/total_doc_count_;
}
float dbfs_measure_simple::prob_class(int class_id)   // marginal prob of the class
{
  if (!total_doc_count_)
    cache_values();
  if (class_id >= (int)class_counts_.size())
    return -1;
  return float(class_counts_[class_id])/total_doc_count_;
}
float dbfs_measure_simple::prob_feature(int feature_id)   // marginal prob of the feature
{
  if (!total_doc_count_)
    cache_values();
  if (feature_id >= (int)feature_counts_.size())
    return -1;
  return float(feature_counts_[feature_id])/total_doc_count_;
}
//: I(C=c, F=f)
float dbfs_measure::mutual_info(int class_id, int feature_id)
{
  float prob2 = prob(class_id, feature_id);
  float div = prob2/(prob_class(class_id)*prob_feature(feature_id));
  if (div <= vcl_numeric_limits<float>::epsilon())
    return 0.0f;  
  float ldiv = (float)(vcl_log(div)/vcl_log(2.0));
  return prob2*ldiv;
}
//: I(C, F=f)
float dbfs_measure_simple::mutual_info_class_average(int feature_id)
{
  if (!total_doc_count_)
    cache_values();
  float sum = 0;
  for (unsigned i = 0; i < class_counts_.size(); i++) {
    sum += mutual_info(i, feature_id);
  }
  return sum;
}

