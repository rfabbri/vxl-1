// This is brcv/seg/dbsta/algo/dbsta_gaussian_stats.txx
#ifndef dbsta_gaussian_stats_txx_
#define dbsta_gaussian_stats_txx_

//:
// \file

#include "dbsta_gaussian_stats.h"



//: Update the statistics given a Gaussian distribution and the number of observations
// both the distribution and observation count are updated
template <class T>
void
dbsta_gaussian_updater<T>::update_gaussian(dbsta_gaussian<T>& gaussian, 
                                           T rho, 
                                           const vnl_vector<T>& sample ) const
{
  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = 1.0 - rho;
  // compute the updated mean
  const vnl_vector<T>& old_mean = gaussian.mean();
  // This is static to avoid memory allocations during each update
  static vnl_vector<T> new_mean; 
  new_mean = (rho_comp * old_mean) +  (rho * sample);
  
  
  static vnl_vector<T> diff;
  diff = sample - old_mean;
  // handle special cases with restricted covariance matrices
  // if we know the types in advance it might be more efficient to make 
  // a new updater class for each of these and avoid the dynamic_cast
  if(dbsta_gaussian_sphere_base<T>* gs = dynamic_cast<dbsta_gaussian_sphere_base<T>*>(&gaussian))
  {
    T new_var = rho_comp * gs->var(); 
    new_var += (rho * rho_comp) * dot_product(diff,diff); 
    gs->set_var(new_var);
  }
  else if(dbsta_gaussian_indep_base<T>* gi = dynamic_cast<dbsta_gaussian_indep_base<T>*>(&gaussian))
  {
    vnl_diag_matrix<T> covar(gi->diag_covar());
    for(unsigned int i=0; i<gi->dim(); ++i){
      covar[i] *= rho_comp; 
      covar[i] += (rho * rho_comp) * (diff[i]*diff[i] + 3.8447e-4 /*1.53787e-5*/);
      //Note: the last term, 3.8447e-4 = (5/255)^2, is a hack to make the
      // variances converge to a lower limit 
    }
    gi->set_covar(covar);
  }
  else
  {
    vnl_matrix<T> new_covar = rho_comp * gaussian.covar();
    new_covar += (rho * rho_comp) * outer_product(diff,diff);
    gaussian.set_covar(new_covar);
  }
   
  gaussian.set_mean(new_mean);
}



#define DBSTA_GAUSSIAN_STATS_INSTANTIATE(T) \
template class dbsta_gaussian_updater<T >; \
template bool dbsta_sort_gaussian_fitness(const dbsta_distribution<T>& d1, const T& w1, \
                                          const dbsta_distribution<T>& d2, const T& w2)

#endif // dbsta_gaussian_stats_txx_
