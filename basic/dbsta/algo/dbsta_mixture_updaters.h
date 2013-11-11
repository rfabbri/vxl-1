// This is brcv/seg/dbsta/algo/dbsta_mixture_updaters.h
#ifndef dbsta_mixture_updaters_h_
#define dbsta_mixture_updaters_h_

//:
// \file
// \brief Updaters that apply to mixtures of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/18/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsta/dbsta_mixture.h>
#include <dbsta/dbsta_updater.h>

/*
//: An updater that sorts mixture components
template <class T, class _sort >
class dbsta_mixture_sort_updater : public dbsta_updater<T>
{
 public:
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d ) const
  { 
    assert(dynamic_cast<dbsta_mixture<T>*>(&d));
    static_cast<dbsta_mixture<T>&>(d).sort(_sort); 
  }

};
*/

static inline bool always_pass(void*, void*) { return true; }

//: Apply a different updater object to the best component
//  Check each component with a boolean test
//  and apply an updater to the first that passes 
template <class T, class _updater>//, class _comp = always_pass >
class dbsta_mixture_best_updater : public dbsta_data_updater<T>
{
 public:
  
  //: Constructor
  dbsta_mixture_best_updater(const _updater& up) : updater(up) {}
    
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                    const vnl_vector<T>& sample ) const
  {
    assert(dynamic_cast<dbsta_mixture<T>*>(&d));
    dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d);
    for(unsigned int i=0; i<mixture.num_components(); ++i){
      if(_comp(mixture[i],sample)){
        updater(mixture.distribution,sample);
        return;
      }
    }
    on_failure(mixture, sample);
  }
  
  //: This function is called only if no update occurs
  virtual void on_failure(dbsta_mixture<T>& mixture, 
                          const vnl_vector<T>& sample ) {}
  
 protected:
  _updater updater;      
};


//: Apply a different updater object to all of the components
//  Check each component with a boolean test
//  and apply an updater to each that passes 
template <class T, class _updater>//, class _comp = always_pass >
class dbsta_mixture_all_updater : public dbsta_data_updater<T>
{
 public:
  
  //: Constructor
  dbsta_mixture_all_updater(const _updater& up) : updater(up) {}
    
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const
  {
    assert(dynamic_cast<dbsta_mixture<T>*>(&d));
    dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d);
    bool failure = true;
    for(unsigned int i=0; i<mixture.num_components(); ++i){
      if(_comp(mixture[i],sample)){
        updater(mixture.distribution,sample);
        failure = false;
      }
    }
    if(failure)
      on_failure(mixture, sample);
  }
    
  //: This function is called only if no update occurs
  virtual void on_failure(dbsta_mixture<T>& mixture, 
                          const vnl_vector<T>& sample ) {}
                          
 protected:
  _updater updater;      
};


//: An updater that normalizes weights of a mixture to equal values 
template <class T >
class dbsta_mixture_equal_weight_updater : public dbsta_updater<T>
{
 public:
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d ) const
  { 
    assert(dynamic_cast<dbsta_mixture<T>*>(&d));
    dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d); 
    
    for(unsigned int i=0; i<mixture.num_components(); ++i)
      mixture.set_weight(i, 1.0/mixture.num_components());   
  }

};




#endif // dbsta_mixture_updaters_h_
