//:
// \file
// \brief base class to provide density and entropy measurement interface for feature selection class
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

#if !defined(_dbfs_measure_h)
#define _dbfs_measure_h

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>

//: base class that does measurements for the selector, defines the interface for deriving classes which will be used by the selector
class dbfs_measure : public vbl_ref_count
{
public:
  virtual float prob(int class_id, int feature_id) = 0;
  virtual float prob_class(int class_id) = 0;   // marginal prob of the class
  virtual float prob_feature(int feature_id) = 0;   // marginal prob of the feature
  float mutual_info(int class_id, int feature_id);  // I(C=c, F=f)
  virtual float mutual_info_class_average(int feature_id) = 0;  // I(C, F=f)

protected:
};

//: a simple measurer class which is given a table of counts to calculate the probs respectively
//  For example in an application where there are a number of training documents, on each of which the occurrence of features can be counted,
//  then during training the following counts should be tabulated:
//      f1   !f1    f2   !f2  .   .   .
//   c1  a    b  
//   c2 
//   .
//   .
//      for instance, the value a is the number of documents in class c1 that contain feature f1
//                    the value b is the number of documents in class c1 that does not contain feature f1
//      so for the class c1, the sum of the values a and b is the total number of documents in that class
//
//  this class contains an M x 2*K matrix where M is the number of classes (M=2 for binary, class,non-class case) and K is the number of features
//  assumes that the feature ids are 0, 1, .. K-1 
class dbfs_measure_simple : public dbfs_measure
{
public:
  dbfs_measure_simple(const vcl_vector<vcl_vector<int> >& counts) : counts_(counts), total_doc_count_(0) {}

  float prob(int class_id, int feature_id);
  float prob_class(int class_id);   // marginal prob of the class
  float prob_feature(int feature_id);   // marginal prob of the feature
  float mutual_info_class_average(int feature_id);  // I(C, F=f)

protected:
  void cache_values();

  vcl_vector<vcl_vector<int> > counts_;
  //: cache these for fast access
  int total_doc_count_;
  vcl_vector<int> class_counts_;
  vcl_vector<int> feature_counts_;
  
};

#endif  //_dbfs_measure_h
