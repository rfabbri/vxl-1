// This is brcv/seg/dbsta/algo/dbsta_truth_updater.h
#ifndef dbsta_truth_updater_h_
#define dbsta_truth_updater_h_

//:
// \file
// \brief Ground truth mixture updater 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/26/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsta/dbsta_distribution.h>
#include <dbsta/dbsta_mixture.h>
#include "dbsta_gaussian_stats.h"
#include "dbsta_mixture_updaters.h"


//: An updater for ground truth mixtures
template <class T>
class dbsta_truth_updater : public dbsta_data_updater<T>
{
 public:
 
  dbsta_truth_updater (const dbsta_distribution<T>& model,
                       const vbl_smart_ptr<dbsta_data_updater<T> >& updater)
   : model_dist_(model.clone()), updater_(updater) {}
   
  //: Destructor
  virtual ~dbsta_truth_updater(){ delete model_dist_; }
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const;
                            
 protected:
   
  //: A model for new distributions inserted
  dbsta_distribution<T> *model_dist_;
  
  //: The updater applied to the components
  vbl_smart_ptr<dbsta_data_updater<T> > updater_;
};


#endif // dbsta_truth_updater_h_
