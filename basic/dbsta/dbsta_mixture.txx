// This is brcv/seg/dbsta/dbsta_mixture.txx
#ifndef dbsta_mixture_txx_
#define dbsta_mixture_txx_

//:
// \file
#include "dbsta_mixture.h"
#include <vcl_cassert.h>

// to instantiate the private component_sptr
#include <vbl/vbl_smart_ptr.txx>
#include <vbl/io/vbl_io_smart_ptr.txx>
#include <vsl/vsl_vector_io.h>
  
  

//: Return the number of dimensions in the space
template <class T>
unsigned int 
dbsta_mixture<T>::dim() const
{
  if(components_.empty())
    return 0;
  else
    return components_.front()->distribution->dim();
}

 
//: Copy Constructor
template <class T>
dbsta_mixture<T>::dbsta_mixture(const dbsta_mixture<T>& other)
 : dbsta_distribution<T>(other), components_(other.components_.size(),NULL)
{
  // deep copy of smart pointer data
  for(unsigned int i=0; i<components_.size(); ++i){
    components_[i] = new component(*other.components_[i]);
  }
}  


//: Normalize the weights of the components to add to 1.
template <class T>
void
dbsta_mixture<T>::normalize_weights()
{
  typedef typename vcl_vector<component_sptr>::iterator comp_itr;
  T sum = 0;
  for(comp_itr i = components_.begin(); i != components_.end(); ++i)
    sum += (*i)->weight;
  assert(sum > 0);
  for(comp_itr i = components_.begin(); i != components_.end(); ++i)
    (*i)->weight /= sum;
}


//: Compute the probablity of this point
// \note assumes weights have been normalized
template <class T>
T 
dbsta_mixture<T>::probability(const vnl_vector<T>& pt) const
{
  typedef typename vcl_vector<component_sptr>::const_iterator comp_itr;
  T prob = 0;
  for(comp_itr i = components_.begin(); i != components_.end(); ++i)
    prob += (*i)->weight * (*i)->distribution->probability(pt);
  return prob;
}

template <class T>
void dbsta_mixture<T>::b_write(vsl_b_ostream &os) const 
{
        vsl_b_write(os,version());
        vsl_b_write(os,components_.size());
        for(int i=0;i<static_cast<int>(components_.size());i++)
                components_[i]->b_write(os);
                
}
template <class T>
void dbsta_mixture<T>::b_read(vsl_b_istream &is)  
{
        if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  components_.clear();
  switch (ver)
  {
   case 1:
   int size_components;
   vsl_b_read(is, size_components);
   for(int i=0;i<size_components;i++)
        {

                components_.push_back(new component());
                components_[i]->b_read(is);
        }
 
    break;
   default:
    vcl_cerr << "dbsta_mixture: unknown I/O version " << ver << '\n';
  }
}

template <class T>
short dbsta_mixture<T>::version() const
{
        return 1;
}




#define DBSTA_MIXTURE_INSTANTIATE(T) \
template class dbsta_mixture<T >; \
template class vbl_smart_ptr<dbsta_mixture<T >::component>;



#endif // dbsta_mixture_txx_
