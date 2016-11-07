// This is brcv/seg/dbsta/algo/dbsta_adaptive_updater.h
#ifndef dbsta_adaptive_updater_h_
#define dbsta_adaptive_updater_h_

//:
// \file
// \brief Adaptive updaters (i.e. Grimson and similar) 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/13/05
//
// This file contains updaters based on 
// "Adaptive background mixture models for real-time tracking"
// Grimson et. al.
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsta/dbsta_distribution.h>
#include <dbsta/dbsta_mixture.h>
#include "dbsta_gaussian_stats.h"
#include "dbsta_mixture_updaters.h"


//: A mixture of Gaussians adaptive updater
// base class for common functionality in adaptive updating schemes
template <class T>
class dbsta_mg_adaptive_updater : public dbsta_data_updater<T>
{
 public:
 
  //: Destructor
  virtual ~dbsta_mg_adaptive_updater(){ delete init_gaussian_; }
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const = 0;
                            
 protected:
  //: Constructor
  dbsta_mg_adaptive_updater(const dbsta_gaussian<T>& model, 
                            unsigned int max_cmp = 5, 
                            const T& init_wgt = T(0.1)) 
   : init_gaussian_(static_cast<dbsta_gaussian<T>*>(model.clone())), 
     max_components_(max_cmp), init_weight_(init_wgt) 
  {
    if(!init_gaussian_->has_data())
      init_gaussian_->set_data(dbsta_stats_data<T>(0));
  }
  
  void insert(dbsta_mixture<T>& mixture, const vnl_vector<T>& sample) const
  {
    bool removed = false;
    if(mixture.num_components() >= max_components_){
      removed = true;
      do{
        mixture.remove_last(); 
      }while(mixture.num_components() >= max_components_);
    }
    
    // if a mixture is removed renormalize the rest
    if(removed){
      T adjust = T(0.0);
      for(unsigned int i=0; i<mixture.num_components(); ++i)
        adjust += mixture.weight(i);
      adjust = (T(1.0)-init_weight_) / adjust;
      for(unsigned int i=0; i<mixture.num_components(); ++i)
         mixture.set_weight(i, mixture.weight(i)*adjust);
    }
    init_gaussian_->set_mean(sample);
    mixture.insert(*init_gaussian_,init_weight_);
  }
   
  //: A model for new Gaussians inserted
  dbsta_gaussian<T> *init_gaussian_;
  //: The maximum number of components in the mixture
  unsigned int max_components_;
  //: The initial weight of new components added to the mixture
  mutable T init_weight_; 

};


//: A mixture of Gaussians statistical updater
// This updater treats all data equally
template <class T>
class dbsta_mg_statistical_updater : public dbsta_mg_adaptive_updater<T>
{
 public:
  //: Constructor
  dbsta_mg_statistical_updater(const dbsta_gaussian<T>& model, 
                               unsigned int max_cmp = 5 )
   : dbsta_mg_adaptive_updater<T>(model, max_cmp, 1.0) {}

  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const;

 protected:
  dbsta_gaussian_updater<T>               gauss_updater_;
  dbsta_mixture_weight_by_obs_updater<T>  reweighter_;

};



//: A mixture of Gaussians statistical updater 
// This updater treats all data equally
template <class T>
class dbsta_mg_window_updater : public dbsta_mg_adaptive_updater<T>
{
 public:
  //: Constructor
  dbsta_mg_window_updater(const dbsta_gaussian<T>& model, 
                          unsigned int max_cmp = 5,
                          unsigned int window_size = 40) 
   : dbsta_mg_adaptive_updater<T>(model, max_cmp, 1.0),
     gauss_updater_(window_size) {}
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const;
 
 protected:
  dbsta_gaussian_window_updater<T>        gauss_updater_;
  dbsta_mixture_weight_by_obs_updater<T>  reweighter_;

};


//: A mixture of Gaussians statistical updater
// using the grimson approximation to prior probablilities
template <class T>
class dbsta_mg_grimson_statistical_updater : public dbsta_mg_adaptive_updater<T>
{
 public:
  //: Constructor
  dbsta_mg_grimson_statistical_updater(const dbsta_gaussian<T>& model, 
                                       unsigned int max_cmp = 5, 
                                       const T& init_wgt = T(0.0)) 
   : dbsta_mg_adaptive_updater<T>(model, max_cmp, init_wgt) {}
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const;
 
 protected:
  dbsta_gaussian_updater<T>               gauss_updater_;
  dbsta_mixture_weight_by_obs_updater<T>  reweighter_;

};


//: A mixture of Gaussians window updater 
// using the grimson approximation to prior probablilities
template <class T>
class dbsta_mg_grimson_window_updater : public dbsta_mg_adaptive_updater<T>
{
 public:
  //: Constructor
  dbsta_mg_grimson_window_updater(const dbsta_gaussian<T>& model, 
                          unsigned int max_cmp = 5, 
                          const T& init_wgt = T(0.0),
                          unsigned int window_size = 40) 
   : dbsta_mg_adaptive_updater<T>(model, max_cmp, init_wgt),
     gauss_updater_(window_size) {}
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const;
 
 protected:
  dbsta_gaussian_window_updater<T>        gauss_updater_;
  dbsta_mixture_weight_by_obs_updater<T>  reweighter_;

};


//: A mixture of Gaussians statistical updater which updates the mixture with weights provided
// This updater treats all data equally
template <class T>
class dbsta_mg_nn_statistical_updater : public dbsta_mg_adaptive_updater<T>
{
 public:
  //: Constructor
  dbsta_mg_nn_statistical_updater(const dbsta_gaussian<T>& model, 
                          unsigned int max_cmp = 5, 
                          const T& init_wgt = T(0.1),
                          const T tau_nn =0.05//threhsold for weights for the negiboring pixels
                          ) 
                          : dbsta_mg_adaptive_updater<T>(model, max_cmp, init_wgt),threshweight_(tau_nn){}
     
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample) const;
 
  void setweight(T weight){weight_=weight;}
 protected:
     T weight_;
     T threshweight_;
  dbsta_gaussian_updater<T>        gauss_updater_;
  dbsta_mixture_weight_by_obs_updater<T>  reweighter_;

};

#endif // dbsta_adaptive_updater_h_
