// This is brcv/seg/dbsta/algo/dbsta_adaptive_updater.txx
#ifndef dbsta_adaptive_updater_txx_
#define dbsta_adaptive_updater_txx_

//:
// \file
#include "dbsta_adaptive_updater.h"

#include <vcl_iostream.h>
#include <vcl_limits.h>

//: The main function
template <class T>
void 
dbsta_mg_statistical_updater<T>::operator() ( dbsta_distribution<T>& d, 
                                              const vnl_vector<T>& sample ) const
{
  assert(dynamic_cast<dbsta_mixture<T>*>(&d));
  dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d);
  assert(mixture.dim() == sample.size() || mixture.num_components() == 0);
  assert(dynamic_cast<dbsta_stats_data<T>* >(&(mixture.data())));
  T& mix_num = static_cast<dbsta_stats_data<T>& >(mixture.data()).num_observations;
  mix_num+=T(1.0);
  T alpha = 1.0/mix_num;
  
  // compute probability of belonging to each distribution
  vcl_vector<T> probs(mixture.num_components(),T(0));
  T sum_probs = T(0);
  for(unsigned int i=0; i<mixture.num_components(); ++i){
    probs[i] = mixture.distribution(i).probability(sample);
    // if within 3 standard deviations
    if(probs[i] > 4.43185e-3){
      probs[i] *= mixture.weight(i);
      sum_probs += probs[i];
    }
    else
      probs[i] = T(0.0);
  }
  
  if(sum_probs == T(0.0)){
    this->init_weight_ = alpha;
    this->insert(mixture,sample);
    mixture.normalize_weights();
  }
  else{
    for(unsigned int i=0; i<mixture.num_components(); ++i){    
      T weight = (T(1.0)-alpha) * mixture.weight(i);
      if(probs[i] > T(0)){
        probs[i] /= sum_probs;
        weight += alpha*probs[i];
        assert(dynamic_cast<dbsta_gaussian<T>* >(&mixture.distribution(i)));
        dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(mixture.distribution(i));
        assert(g.has_data());
        assert(dynamic_cast<dbsta_stats_data<T>* >(&(g.data())));
        T& num = static_cast<dbsta_stats_data<T>& >(g.data()).num_observations;
        num += probs[i];
        T rho = probs[i] * ((1.0-alpha)/num +alpha); //alpha * probs[i] / weight;
        gauss_updater_.update_gaussian(g, rho, sample);  
      } 
      mixture.set_weight(i, weight);
    }
  }
    
  mixture.sort(dbsta_sort_gaussian_fitness<T>);
}



//: The main function
template <class T>
void 
dbsta_mg_window_updater<T>::operator() ( dbsta_distribution<T>& d, 
                                         const vnl_vector<T>& sample ) const
{
  assert(dynamic_cast<dbsta_mixture<T>*>(&d));
  dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d);
  assert(mixture.dim() == sample.size() || mixture.num_components() == 0);
  assert(dynamic_cast<dbsta_stats_data<T>* >(&(mixture.data())));
  T& mix_num = static_cast<dbsta_stats_data<T>& >(mixture.data()).num_observations;
  if(mix_num < gauss_updater_.window_size)
    mix_num+=T(1.0);
  T alpha = 1.0/mix_num;

  const unsigned num_components = mixture.num_components();

  // prune components by mahalanobis distance
  vcl_vector<T> probs(num_components,T(0));
  vcl_vector<unsigned int> matched;
  for(unsigned int i=0; i<num_components; ++i){
    assert(dynamic_cast<dbsta_gaussian<T>* >(&mixture.distribution(i)));
    dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(mixture.distribution(i));
    T sqr_dist = g.sqr_mahalanobis_dist(sample);
    if(sqr_dist < 9.0){ // within 3 standard deviations
      matched.push_back(i);
      probs[i] = sqr_dist;
    }
  }

  // if no matches add a new component
  if(matched.empty()){
    this->init_weight_ = alpha;
    this->insert(mixture,sample);
    mixture.normalize_weights();
  }
  else{
    // update the weights
    for(unsigned int i=0; i<num_components; ++i){
      T weight = (T(1.0)-alpha) * mixture.weight(i);
      mixture.set_weight(i,weight);
    }
    // special case of 1 match - don't need to compute probabilites
    if (matched.size() == 1){
      unsigned int m_idx = matched.front();
      mixture.set_weight(m_idx,mixture.weight(m_idx)+alpha);
      assert(dynamic_cast<dbsta_gaussian<T>* >(&mixture.distribution(m_idx)));
      dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(mixture.distribution(m_idx));
      assert(g.has_data());
      assert(dynamic_cast<dbsta_stats_data<T>* >(&(g.data())));
      T& num = static_cast<dbsta_stats_data<T>& >(g.data()).num_observations;
      num += T(1);
      T rho = (1.0-alpha)/num +alpha; //alpha * probs[i] / weight;
      gauss_updater_.update_gaussian(g, rho, sample);
    }
    else{
      // compute probabilites for each match
      typedef typename vcl_vector<unsigned int>::iterator m_itr;
      T sum_probs = T(0);
      for(m_itr itr = matched.begin(); itr != matched.end(); ++itr){
        const unsigned int i = *itr;
        dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(mixture.distribution(i));
        probs[i] = g.dist_probability(probs[i]) * mixture.weight(i);
        sum_probs += probs[i];
      }
      // update each match
      for(m_itr itr = matched.begin(); itr != matched.end(); ++itr){
        const unsigned int i = *itr;
        probs[i] /= sum_probs;
        mixture.set_weight(i,mixture.weight(i)+alpha*probs[i]);
        dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(mixture.distribution(i));
        assert(g.has_data());
        assert(dynamic_cast<dbsta_stats_data<T>* >(&(g.data())));
        T& num = static_cast<dbsta_stats_data<T>& >(g.data()).num_observations;
        num += probs[i];
        T rho = probs[i] * ((1.0-alpha)/num +alpha); //alpha * probs[i] / weight;
        gauss_updater_.update_gaussian(g, rho, sample);
      }
    }
  }

  mixture.sort(dbsta_sort_gaussian_fitness<T>);

  if(mixture.weight(mixture.num_components()-1) < vcl_numeric_limits<T>::epsilon()){
    mixture.remove_last(); 
    T sum = 0;
    for(unsigned int i=0; i<mixture.num_components(); ++i){
      sum += mixture.weight(i);
    }
    vcl_cout << "removed, total weight = " << sum << vcl_endl;
    mixture.normalize_weights();
  }

}

//: The main function
template <class T>
void 
dbsta_mg_grimson_statistical_updater<T>::operator() ( dbsta_distribution<T>& d, 
                                              const vnl_vector<T>& sample ) const
{
  assert(dynamic_cast<dbsta_mixture<T>*>(&d));
  dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d);
  assert(mixture.dim() == sample.size() || mixture.num_components() == 0);
  assert(dynamic_cast<dbsta_stats_data<T>* >(&(mixture.data())));
  T& mix_num = static_cast<dbsta_stats_data<T>& >(mixture.data()).num_observations;
  T alpha = 1.0/mix_num;
  
  bool no_match = true;
  for(unsigned int i=0; i<mixture.num_components(); ++i){
    assert(dynamic_cast<dbsta_gaussian<T>* >(&mixture.distribution(i)));
    dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(mixture.distribution(i));
    T weight = (T(1.0)-alpha) * mixture.weight(i);
    if(no_match && dbsta_fits_gaussian_model(g,sample,T(2.5))){
      weight += alpha;
      T rho = alpha / weight;
      gauss_updater_.update_gaussian(g, rho, sample);
      no_match = false;
    }
    mixture.set_weight(i, weight);
  }
  if(no_match){
    this->init_weight_ = alpha;
    this->insert(mixture,sample);
  }
    
  //reweighter_(mixture);
  mixture.sort(dbsta_sort_gaussian_fitness<T>);
}


//: The main function
template <class T>
void 
dbsta_mg_grimson_window_updater<T>::operator() ( dbsta_distribution<T>& d, 
                                                 const vnl_vector<T>& sample ) const
{
  assert(dynamic_cast<dbsta_mixture<T>*>(&d));
  dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d);
  assert(mixture.dim() == sample.size() || mixture.num_components() == 0);
  assert(dynamic_cast<dbsta_stats_data<T>* >(&(mixture.data())));
  T& mix_num = static_cast<dbsta_stats_data<T>& >(mixture.data()).num_observations;
  if(mix_num < gauss_updater_.window_size)
    mix_num+=T(1.0);
  T alpha = T(1.0)/mix_num;
  
  bool no_match = true;
  for(unsigned int i=0; i<mixture.num_components(); ++i){
    assert(dynamic_cast<dbsta_gaussian<T>* >(&mixture.distribution(i)));
    dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(mixture.distribution(i));
    T weight = (T(1.0)-alpha) * mixture.weight(i);
    //if(weight < vcl_numeric_limits<T>::epsilon()*alpha)
    //  vcl_cout << "weight < "<<vcl_numeric_limits<T>::epsilon()*alpha <<vcl_endl;
    if(no_match && dbsta_fits_gaussian_model(g,sample,T(2.5))){
      weight += alpha;
      if(weight == alpha)
        vcl_cout << "weight == alpha" <<vcl_endl;
      T rho = alpha / weight;
      gauss_updater_.update_gaussian(g, rho, sample);
      no_match = false;
    }
    mixture.set_weight(i, weight);
  }
  if(no_match){
    this->init_weight_ = alpha;
    this->insert(mixture,sample);
  }
   
  mixture.sort(dbsta_sort_gaussian_fitness<T>);
  
  if(mixture.weight(mixture.num_components()-1) < vcl_numeric_limits<T>::epsilon()){
    mixture.remove_last(); 
    T sum = 0;
    for(unsigned int i=0; i<mixture.num_components(); ++i){
      sum += mixture.weight(i);
    }
    vcl_cout << "removed, total weight = " << sum << vcl_endl;
  }

}

//: The main function
template <class T>
void 
dbsta_mg_nn_statistical_updater<T>::operator() ( dbsta_distribution<T>& d, 
                                              const vnl_vector<T>& sample ) const
{
  assert(dynamic_cast<dbsta_mixture<T>*>(&d));
  dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d);
  assert(mixture.dim() == sample.size() || mixture.num_components() == 0);
  bool failure = true;
  for(unsigned int i=0; i<mixture.num_components(); ++i){
    assert(dynamic_cast<dbsta_gaussian<T>* >(&mixture.distribution(i)));
    dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(mixture.distribution(i));
    if(dbsta_fits_gaussian_model(g,sample,T(2.5))){
      assert(g.has_data());
      assert(dynamic_cast<dbsta_stats_data<T>* >(&(g.data())));
      T& num =static_cast<dbsta_stats_data<T>& >(g.data()).num_observations;
      num=weight_*num;
      gauss_updater_.update_gaussian(g,num,sample);
      failure = false;
      break;
    }
  }
  if(failure && weight_>threshweight_)
    this->insert(mixture,sample);
    
  reweighter_(mixture);
  mixture.sort(dbsta_sort_gaussian_fitness<T>);
}

#define DBSTA_ADAPTIVE_UPDATER_INSTANTIATE(T) \
template class dbsta_mg_statistical_updater<T >; \
template class dbsta_mg_nn_statistical_updater<T >;\
template class dbsta_mg_window_updater<T >;\
template class dbsta_mg_grimson_window_updater<T >


#endif // dbsta_adaptive_updater_txx_
