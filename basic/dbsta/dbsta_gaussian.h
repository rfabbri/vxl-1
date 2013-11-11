// This is brcv/seg/dbsta/dbsta_gaussian.h
#ifndef dbsta_gaussian_h_
#define dbsta_gaussian_h_

//:
// \file
// \brief A Gaussian distribution for use in a mixture model 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/12/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_cmath.h>
#include "dbsta_distribution.h"
#include <vnl/vnl_vector_ref.h>
#include <vnl/vnl_diag_matrix.h>

//: forward declare vnl_matrix
template <class T> class vnl_matrix;

//: A Gaussian distribution 
// used as a component of the mixture
template <class T>
class dbsta_gaussian : public dbsta_distribution<T>
{
 public:
  //: Destructor
  virtual ~dbsta_gaussian<T>(){}
  
  //: Return the number of dimensions in the space
  virtual unsigned int dim() const = 0;
  
  //: The mean of the distribution
  virtual const vnl_vector_ref<T> mean() const = 0;
  
  //: The covariance of the distribution
  virtual vnl_matrix<T> covar() const = 0;
  
  //: compute the determinant of the covariance matrix
  virtual T det_covar() const = 0;
  
  //: Set the mean of the distribution
  virtual void set_mean(const vnl_vector<T>& mean) = 0;
  
  //: Set the covariance of the distribution
  virtual void set_covar(const vnl_matrix<T>& covar) = 0;
    
  //: The squared Mahalanobis distance to this point
  virtual T sqr_mahalanobis_dist(const vnl_vector<T>& pt) const = 0;

   //: The squared Mahalanobis distance for cylcic data (e.g angles where 360 and 0 are the same angle for most of the purposes) to this point
  //virtual T sqr_mahalanobis_dist_cyclic(const vnl_vector<T>& pt) const = 0;
  
  //: The Mahalanobis distance to this point
  T mahalanobis_dist(const vnl_vector<T>& pt) const 
  { return vcl_sqrt(sqr_mahalanobis_dist(pt)); }
  
  //: The Mahalanobis distance to this point
  /*T mahalanobis_dist_cyclic(const vnl_vector<T>& pt) const 
  { return vcl_sqrt(sqr_mahalanobis_dist_cyclic(pt)); }*/

  //: The probability of this sample given square mahalanobis distance
  inline T dist_probability(const T& sqr_mahal_dist) const
  {
    T n = norm_const();
    if(n<=T(0)) return T(0);
    return n*vcl_exp(-sqr_mahal_dist/2);
  }

  //: The probability of this sample
  virtual T probability(const vnl_vector<T>& pt) const
  { 
    return dist_probability(sqr_mahalanobis_dist(pt));
  }
  
 protected:
  //: Constructor
  dbsta_gaussian<T>(){}
  
  //: compute the normalization constant
  virtual T norm_const() const = 0;
};


//: A Gaussian distribution, independent in each dimension 
// Thus, the covariance matrix is diagonal
template <class T>
class dbsta_gaussian_indep_base : public dbsta_gaussian<T>
{
 public:
  //: The diagonal covariance of the distribution
  virtual vnl_diag_matrix<T> diag_covar() const = 0;
  //: Set the diagonal covariance of the distribution 
  virtual void set_covar(const vnl_diag_matrix<T>& covar) = 0;
};


//: A (hyper-)spherical Gaussian distribution
// Thus, the covariance matrix is the identity times a scalar variance
template <class T>
class dbsta_gaussian_sphere_base : public dbsta_gaussian_indep_base<T>
{
 public:
  //: The diagonal covariance of the distribution
  virtual vnl_diag_matrix<T> diag_covar() const { return vnl_diag_matrix<T>(this->dim(),var()); };
  //: Set the diagonal covariance of the distribution
  virtual void set_covar(const vnl_diag_matrix<T>& covar) { set_var(covar[0]);}
  
  //: The variance of the distribution
  virtual T var() const = 0;
  //: Set the variance of the distribution
  virtual void set_var(T covar) = 0;
};


#endif // dbsta_gaussian_h_
