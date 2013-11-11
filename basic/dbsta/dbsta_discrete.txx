// This is brcv/seg/dbsta/dbsta_discrete.txx
#ifndef dbsta_discrete_txx_
#define dbsta_discrete_txx_

//:
// \file

#include "dbsta_discrete.h"
#include <vnl/vnl_vector.h>


//: Constructor
template <class T>
dbsta_discrete_base<T>::dbsta_discrete_base( unsigned int dim, T min, T max, 
                                             unsigned int bins)
 : dim_(dim), min_vals_(dim,min), max_vals_(dim,max), num_bins_(dim,bins)
{
  assert(dim > 0);
}


//: Constructor
template <class T>
dbsta_discrete_base<T>::dbsta_discrete_base( unsigned int dim, 
                                             const vcl_vector<T>& min, 
                                             const vcl_vector<T>& max, 
                                             const vcl_vector<unsigned int>& bins)
 : dim_(dim), min_vals_(min), max_vals_(max), num_bins_(bins)
{
  assert(dim > 0);
  assert(min_vals_.size() == dim);
  assert(max_vals_.size() == dim);
  assert(num_bins_.size() == dim);
}


//: Map a vector into an index for a discrete bin
// \returns -1 if the vector is out of bounds
template <class T>
int 
dbsta_discrete_base<T>::index(const vnl_vector<T>& pt) const
{
  assert(pt.size() == dim_);
  int index = 0;
  for(unsigned int i=0; i<dim_; ++i){
    if(i != 0)
      index *= num_bins_[i-1];
    T val = pt[i];
    if(val < min_vals_[i] || val >= max_vals_[i])
      return -1;
    T bin_float = (val - min_vals_[i])/(max_vals_[i]-min_vals_[i])*num_bins_[i];
    int bin = static_cast<int>(vcl_floor(bin_float));
    assert(bin >=0 && bin < (int)num_bins_[i]);
    index += bin;
  }
  return index;
}



#define DBSTA_DISCRETE_INSTANTIATE(T) \
template class dbsta_discrete_base<T>


#endif // dbsta_discrete_txx_
