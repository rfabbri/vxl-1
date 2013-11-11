// This is brcv/seg/dbsta/algo/dbsta_truth_updater.txx
#ifndef dbsta_truth_updater_txx_
#define dbsta_truth_updater_txx_

//:
// \file
#include "dbsta_truth_updater.h"



//: The main function
template <class T>
void 
dbsta_truth_updater<T>::operator() ( dbsta_distribution<T>& d, 
                                     const vnl_vector<T>& sample ) const
{
  assert(dynamic_cast<dbsta_mixture<T>*>(&d));
  dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d);
  assert(model_dist_->dim()+1 == sample.size());
  
  // The actual sample is stored in the first n-1 dimensions
  vnl_vector<T> data(model_dist_->dim(),model_dist_->dim(),sample.data_block());
  
  // Get the index from the last dimension
  unsigned int index = static_cast<unsigned int>(sample[model_dist_->dim()]);
  
  while(mixture.num_components() <= index) 
    mixture.insert(*model_dist_,T(0));
    
  (*updater_)(mixture.distribution(index),data);

}






#define DBSTA_TRUTH_UPDATER_INSTANTIATE(T) \
template class dbsta_truth_updater<T >


#endif // dbsta_truth_updater_txx_
