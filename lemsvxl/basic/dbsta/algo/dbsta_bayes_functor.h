// This is brcv/seg/dbsta/dbsta_bayes_functor.h
#ifndef dbsta_bayes_functor_h_
#define dbsta_bayes_functor_h_

//:
// \file
// \brief Functors for Bayesian classification 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/27/05
//
// 
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsta/dbsta_functor.h>


//: A functor that classifies a sample as one of the components of the mixture
template <class T>
class dbsta_bayes_functor : public dbsta_data_functor<T>
{
 public:
  
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return 1; }
  
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d, 
                                     const vnl_vector<T>& sample ) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    
    int best_index = -1;
    T best_probability = T(0);
    for(unsigned int i=0; i<mix.num_components(); ++i){
      T weight = mix.weight(i);
      if(weight > best_probability){
        T prob = mix.distribution(i).probability(sample) * weight;
        if(prob > best_probability){
          best_index = i;
          best_probability = prob;
        }
      }
    } 
    return vnl_vector<T>(1,static_cast<T>(best_index));
  }
  
 protected:
};


//: A functor that computes the probability of each component in a mixture
template <class T>
class dbsta_mixture_prob_functor : public dbsta_data_functor<T>
{
 public:
 
  dbsta_mixture_prob_functor(bool normalize = true, unsigned int num_cmps=3)
  : dbsta_data_functor<T>(), num_cmps_(num_cmps), normalize_(normalize) {}
    
  //: The size of the vector returned by the functor
  virtual unsigned int return_size() const { return num_cmps_; }
 
  //: The main function
  virtual vnl_vector<T> operator() ( const dbsta_distribution<T>& d, 
                                     const vnl_vector<T>& sample ) const
  {
    assert(dynamic_cast<const dbsta_mixture<T>* >(&d));
    const dbsta_mixture<T>& mix = static_cast<const dbsta_mixture<T>& >(d);
    
    vnl_vector<T> result(num_cmps_,T(0));
    if(num_cmps_==0)
      return result;
      
    double tmp = 0.0;
    for(unsigned int i=0; i<mix.num_components(); ++i){
      T w = mix.weight(i);
      if(w > T(0)){
        result[i] = w * mix.distribution(i).probability(sample);
        tmp += result[i];
      }
    }
    if(normalize_){
      if(tmp > vcl_numeric_limits<T>::epsilon()){
        for(unsigned int i=0; i<mix.num_components(); ++i){
          result[i] /= tmp;
        }
      }
      else{
        for(unsigned int i=0; i<mix.num_components(); ++i)
          result[i] = mix.weight(i);
      }
    }
    return result;
  }
  
 protected:
   unsigned int num_cmps_;
   bool normalize_;
};


#endif // dbsta_bayes_functor_h_
