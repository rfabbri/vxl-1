// This is brcv/seg/dbsta/dbsta_gaussian_sphere.h
#ifndef dbsta_gaussian_sphere_h_
#define dbsta_gaussian_sphere_h_

//:
// \file
// \brief A (hyper-)spherical Gaussian distribution (i.e. single variance)
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/13/05
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsta_gaussian.h"
#include <vnl/vnl_vector_fixed.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_typeinfo.h>

//: A (hyper-)spherical Gaussian distribution
// Thus, the covariance matrix is the identity times a scalar variance
template <class T, unsigned int n>
class dbsta_gaussian_sphere : public dbsta_gaussian_sphere_base<T>
{
 public:
  //: Constructor
  dbsta_gaussian_sphere<T,n>() : mean_(T(0)), var_(T(0)), normalizing_constant_(T(-1)) {}
  
  //: Constructor
  dbsta_gaussian_sphere<T,n>(const vnl_vector_fixed<T,n>& mean,
                        const T& var) 
    : mean_(mean), var_(var), normalizing_constant_(T(-1)) {}
      
  //: Destructor
  virtual ~dbsta_gaussian_sphere<T,n>(){}
  
  //: Clone
  // allocate a new copy of this object.
  // the caller is responsible for deletion 
  virtual dbsta_distribution<T>* clone() const
  { return new dbsta_gaussian_sphere<T,n>(*this); }
  
  //: Return the number of dimensions in the space
  virtual unsigned int dim() const { return n; }
  
  //: The mean of the distribution
  virtual const vnl_vector_ref<T> mean() const { return mean_; }
  
  virtual T var() const { return var_; }
  
  //: The covariance of the distribution
  virtual vnl_matrix<T> covar() const 
  { return var_*vnl_matrix<T>(n,n,vnl_matrix_identity); }
  
  //: Compute the determinant of the covariance matrix
  virtual T det_covar() const;
  
  //: Set the mean of the distribution
  void set_mean(const vnl_vector_fixed<T,n>& mean) { mean_ = mean; }  
  virtual void set_mean(const vnl_vector<T>& mean)
  { set_mean(vnl_vector_fixed<T,n>(mean)); }
  
  //: Set the covariance of the distribution (and clear cached values)
  virtual void set_var(T var);
  virtual void set_covar(const vnl_matrix<T>& covar);
      
  //: The squared Mahalanobis distance to this point
  virtual T sqr_mahalanobis_dist(const vnl_vector<T>& pt) const 
  { 
    vnl_vector<T> d = mean_-pt;
    return dot_product(d,d)/var_; 
  }
 
     // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {  vcl_ostringstream os;os<<n;
      return vcl_string("dbsta_gaussian_sphere"+vcl_string(typeid(T).name())+os.str()); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(vcl_string const& cls) const { return cls==is_a(); }
  
 protected:
  
  //: Compute the normalization constant
  virtual T norm_const() const;

 private:
  vnl_vector_fixed<T,n> mean_;
  T var_;
  mutable T normalizing_constant_;
};


//: Binary save dbsta_gaussian_sphere* to stream.
template <class T, unsigned int n>
void vsl_b_write(vsl_b_ostream &os, const dbsta_gaussian_sphere<T,n>* p);

//: Binary load dbsta_gaussian_sphere* from stream.
template <class T, unsigned int n>
void vsl_b_read(vsl_b_istream &is, dbsta_gaussian_sphere<T,n>* &p);

#endif // dbsta_gaussian_sphere_h_
