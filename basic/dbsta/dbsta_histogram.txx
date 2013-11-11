// This is brcv/seg/dbsta/dbsta_histogram.txx
#ifndef dbsta_histogram_txx_
#define dbsta_histogram_txx_

//:
// \file

#include "dbsta_histogram.h"



//: Constructor - each dimension has the same range and number of bins
template <class T>
dbsta_histogram<T>::dbsta_histogram( unsigned int dim, T min, T max, 
                                     unsigned int bins) 
 : dbsta_discrete_base<T>(dim,min,max,bins), total_(0)
{
  unsigned int array_size = bins;
  for(unsigned int i=1; i<dim; ++i)
    array_size *= bins;
  counts_.resize(array_size,0);
}
  

//: Constructor - each dimension has different parameters
template <class T>
dbsta_histogram<T>::dbsta_histogram( unsigned int dim, const vcl_vector<T>& min, 
                                     const vcl_vector<T>& max, 
                                     const vcl_vector<unsigned int>& bins)
 : dbsta_discrete_base<T>(dim,min,max,bins), total_(0)
{
  unsigned int array_size = this->num_bins_[0];
  for(unsigned int i=1; i<dim; ++i)
    array_size *= this->num_bins_[i];
  counts_.resize(array_size,0);
}


//: Update the count in the bin that this point fall into
// \returns false if the point is out of bounds
template <class T>
bool 
dbsta_histogram<T>::update(const vnl_vector<T>& pt)
{
  int idx = this->index(pt);
  if(idx < 0) 
    return false;
  ++counts_[idx];
  ++total_;
  return true;
}

template <class T>
void dbsta_histogram<T>::b_write(vsl_b_ostream &os) const 
{
        vsl_b_write(os,version());
}
template <class T>
void dbsta_histogram<T>::b_read(vsl_b_istream &is)  
{
        if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   default:
    vcl_cerr << "dbsta_histogram: unknown I/O version " << ver << '\n';
  }
}

template <class T>
short dbsta_histogram<T>::version() const
{
        return 1;
}

#define DBSTA_HISTOGRAM_INSTANTIATE(T) \
template class dbsta_histogram<T>


#endif // dbsta_histogram_txx_
