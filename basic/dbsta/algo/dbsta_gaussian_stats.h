// This is brcv/seg/dbsta/algo/dbsta_gaussian_stats.h
#ifndef dbsta_gaussian_stats_h_
#define dbsta_gaussian_stats_h_

//:
// \file
// \brief Statistics using Gaussians 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/15/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsta/dbsta_distribution.h>
#include <dbsta/dbsta_mixture.h>
#include <dbsta/dbsta_gaussian.h>
#include <dbsta/dbsta_updater.h>


//: A class used to store extra data for statistical operations
template <class T>
struct dbsta_stats_data : public dbsta_distribution<T>::user_data
{
  //: Constructor
  dbsta_stats_data(T num=T(0)) : num_observations(num) {}
    
  //: Clone the data
  virtual typename dbsta_distribution<T>::user_data* clone() const
  { return new dbsta_stats_data(*this); }
    
  //: The number of observations seen by the distribution
  T num_observations;
};


//: An updater that normalizes weights of a mixture based on number of observations
// Each components is weighted as num_observation / total_observations
// The original weights are ignored 
template <class T >
class dbsta_mixture_weight_by_obs_updater : public dbsta_updater<T>
{
 public:
  
  //: The main function
  virtual void operator() ( dbsta_distribution<T>& d ) const
  { 
    assert(dynamic_cast<dbsta_mixture<T>*>(&d));
    dbsta_mixture<T>& mixture = static_cast<dbsta_mixture<T>&>(d); 
    T total = 0;
    vcl_vector<T> new_weights(mixture.num_components(),T(0));
    for(unsigned int i=0; i<mixture.num_components(); ++i){
      dbsta_distribution<T>& d = mixture.distribution(i);
      assert(d.has_data());
      assert(dynamic_cast<dbsta_stats_data<T>* >(&(d.data())));
      T num = static_cast<dbsta_stats_data<T>& >(d.data()).num_observations;
      total += num;
      new_weights[i] = num;    
    }
    for(unsigned int i=0; i<mixture.num_components(); ++i)
      mixture.set_weight(i, new_weights[i]/total);   
  }

};



//: Used to sort a mixture of gaussians in decreasing order of fitness
template <class T>
bool dbsta_sort_gaussian_fitness(const dbsta_distribution<T>& d1, const T& w1,
                                 const dbsta_distribution<T>& d2, const T& w2)
{
  // run-time type check for debugging only
  assert(dynamic_cast<const dbsta_gaussian<T>*>(&d1));
  assert(dynamic_cast<const dbsta_gaussian<T>*>(&d2));
  T det1 = static_cast<const dbsta_gaussian<T>&>(d1).det_covar();
  T det2 = static_cast<const dbsta_gaussian<T>&>(d2).det_covar();
  T w1_sqr = w1*w1, w2_sqr = w2*w2;
  T v1 = w1_sqr, v2 = w2_sqr;
  for(unsigned int i=1; i<d1.dim(); ++i){
    v1 *= w1_sqr;
    v2 *= w2_sqr;
  }
  return v1/det1 > v2/det2;
}


//: Check if a sample fits a gaussian distribution
//  i.e. is the mahalanobis distance below some threshold
template <class T>
bool dbsta_fits_gaussian_model(const dbsta_gaussian<T>& gaussian, 
                               const vnl_vector<T>& sample, T threshold=T(2.5))
{
  return gaussian.mahalanobis_dist(sample) < threshold;
}


//: An updater for statistically updating Gaussian distributions
template <class T>
class dbsta_gaussian_updater : public dbsta_data_updater<T>
{
 public:
  
  //: The main function
  // make the appropriate type casts and call a helper function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const
  {
    assert(d.dim() == sample.size());
    assert(dynamic_cast<dbsta_gaussian<T>* >(&d));
    dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(d);
    assert(g.has_data());
    assert(dynamic_cast<dbsta_stats_data<T>* >(&(g.data())));
    T& num = static_cast<dbsta_stats_data<T>& >(g.data()).num_observations;   
    num+=T(1.0);
    update_gaussian(g, T(1.0)/num, sample);
  }
                            
  //: Update the statistics given a Gaussian distribution and a learning rate
  // \note if rho = 1/(num observations) then this just an online cumulative average 
  void update_gaussian(dbsta_gaussian<T>& gaussian, T rho, 
                       const vnl_vector<T>& sample ) const;

};


//: An updater for updating Gaussian distributions with a moving window
// When the number of samples exceeds the window size the most recent
// samples contribute more toward the distribution.
template <class T>
class dbsta_gaussian_window_updater : public dbsta_gaussian_updater<T>
{
 public:
  
  //: Constructor
  dbsta_gaussian_window_updater(unsigned int ws) : window_size(ws) {}
  
  //: The main function
  // make the appropriate type casts and call a helper function
  virtual void operator() ( dbsta_distribution<T>& d, 
                            const vnl_vector<T>& sample ) const
  {
    assert(d.dim() == sample.size());
    assert(dynamic_cast<dbsta_gaussian<T>* >(&d));
    dbsta_gaussian<T>& g = static_cast<dbsta_gaussian<T>& >(d);
    assert(g.has_data());
    assert(dynamic_cast<dbsta_stats_data<T>* >(&(g.data())));
    T& num = static_cast<dbsta_stats_data<T>& >(g.data()).num_observations;
    if(num < window_size)
      num+=T(1.0);
    update_gaussian(g,T(1.0)/num,sample);
  }
                            
  unsigned int window_size;

};



#endif // dbsta_gaussian_stats_h_
