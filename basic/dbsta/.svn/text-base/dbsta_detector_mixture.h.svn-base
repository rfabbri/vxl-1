// This is brcv/seg/dbsta/algo/dbsta_detector_mixture.h
#ifndef dbsta_detector_mixture_h_
#define dbsta_detector_mixture_h_

//:
// \file
// \brief Detectors applying to mixtures 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/20/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsta/dbsta_mixture.h>
#include <dbsta/dbsta_detector.h>



//: Apply a detector to a specify indexed component only
template <class T>
class dbsta_mix_eq_index_detector : public dbsta_detector<T>
{
 public:
  //: Constructor
  dbsta_mix_eq_index_detector(const vbl_smart_ptr<dbsta_detector<T> >& d, 
                              unsigned int i = 0) 
   : detect(d), index(i) {}
  
  //: The main function
  virtual bool operator() (const dbsta_distribution<T>& d, 
                           const vnl_vector<T>& sample) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    if(index < mix.num_components() && mix.weight(index) > T(0))
      return (*detect)(mix.distribution(index),sample); 
    return false;
  }
   
  //: The detector to apply to components
  vbl_smart_ptr<dbsta_detector<T> > detect;
  //: The index to detect                      
  unsigned int index;
};


//: Apply a detector to all components less than an index
// return true if any of the tested components is true
template <class T>
class dbsta_mix_any_less_index_detector : public dbsta_detector<T>
{
 public:
  //: Constructor
  dbsta_mix_any_less_index_detector(const vbl_smart_ptr<dbsta_detector<T> >& d, 
                                    unsigned int i = 0)
   : detect(d), index(i) {}
  
  //: The main function
  virtual bool operator() (const dbsta_distribution<T>& d, 
                           const vnl_vector<T>& sample) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    unsigned int max_ind = vcl_min(index+1,mix.num_components());
    for(unsigned int i=0; i<max_ind; ++i)
      if( (*detect)(mix.distribution(i),sample) )
        return true;
    return false;
  }
   
  //: The detector to apply to components
  vbl_smart_ptr<dbsta_detector<T> > detect;
  //: The index to detect                      
  unsigned int index;
};


//: Apply a detector to each component in order
// return true if the first component to pass is the select index
template <class T>
class dbsta_mix_first_index_detector : public dbsta_detector<T>
{
 public:
  //: Constructor
  dbsta_mix_first_index_detector(const vbl_smart_ptr<dbsta_detector<T> >& d, 
                                 unsigned int i = 0)
   : detect(d), index(i) {}
  
  //: The main function
  virtual bool operator() (const dbsta_distribution<T>& d, 
                           const vnl_vector<T>& sample) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    if(mix.num_components() <= index)
      return false;
    unsigned int max_ind = vcl_min(index,mix.num_components()-1);
    for(unsigned int i=0; i<max_ind; ++i)
      if( (*detect)(mix.distribution(i),sample) )
        return false;
    return (*detect)(mix.distribution(index),sample);
  }
   
  //: The detector to apply to components
  vbl_smart_ptr<dbsta_detector<T> > detect;
  //: The index to detect                      
  unsigned int index;
};


//: This detector is different from the rest as it detects foreround.
//: Apply a detector to each component in order
// return true if the first component to pass is the select index and the weight 
// of the component is larger than a threshold
template <class T>
class dbsta_mix_first_index_above_min_weight_detector : public dbsta_detector<T>
{
 public:
  //: Constructor
  dbsta_mix_first_index_above_min_weight_detector(const vbl_smart_ptr<dbsta_detector<T> >& d, 
                                 T minweight, unsigned int i = 0 )
   : detect(d), index(i), minweight_(minweight) {}
  
  //: The main function
  virtual bool operator() (const dbsta_distribution<T>& d, 
                           const vnl_vector<T>& sample) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    if(mix.num_components() <= index)
      return false;
    unsigned int max_ind = vcl_min(index,mix.num_components()-1);
    for(unsigned int i=0; i<max_ind; ++i)
      if( (*detect)(mix.distribution(i),sample) )
        return true;
    if(mix.weight(index)>minweight_)
        return !(*detect)(mix.distribution(index),sample);
    else
        return false;
  }
   
  //: The detector to apply to components
  vbl_smart_ptr<dbsta_detector<T> > detect;
  //: The index to detect                      
  unsigned int index;
  //: minimum weight to threshold for background 
  T minweight_;
};



//: Apply a detector to each component in order 
// while the total weight is below a threshold.
// Return true if any tested component matches
template <class T>
class dbsta_top_weight_detector : public dbsta_detector<T>
{
 public:
  //: Constructor
  dbsta_top_weight_detector(const vbl_smart_ptr<dbsta_detector<T> >& d, const T& w=T(0.5))
   : detect(d), weight_thresh(w) {}
  
  //: The main function
  virtual bool operator() (const dbsta_distribution<T>& d, 
                           const vnl_vector<T>& sample) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    T total_weight = 0.0;
    for(unsigned int i=0; i<mix.num_components(); ++i){
      if(total_weight > weight_thresh)
        return false;
      if( (*detect)(mix.distribution(i),sample) )
        return true;
      total_weight += mix.weight(i);
    }
    return false;
  }
   
  //: The detector to apply to components
  vbl_smart_ptr<dbsta_detector<T> > detect;
  //: The index to detect                      
  T weight_thresh;
};



#endif // dbstavbl_smart_ptr<dbsta_detector<T> >_mixture_h_
