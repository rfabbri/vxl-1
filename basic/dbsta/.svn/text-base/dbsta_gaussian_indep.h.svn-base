// This is brcv/seg/dbsta/dbsta_gaussian_indep.h
#ifndef dbsta_gaussian_indep_h_
#define dbsta_gaussian_indep_h_

//:
// \file
// \brief A Gaussian distribution, independent in each dimension
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 07/12/05
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbsta_gaussian.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_diag_matrix.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_typeinfo.h>


//: A Gaussian distribution, independent in each dimension 
// Thus, the covariance matrix is diagonal
template <class T, unsigned int n>
class dbsta_gaussian_indep : public dbsta_gaussian_indep_base<T>
{
 public:
  //: Constructor
  dbsta_gaussian_indep<T,n>() : mean_(T(0)), covar_(n,T(0)), det_covar_(T(-1)) {}
  
  //: Constructor
  dbsta_gaussian_indep<T,n>(const vnl_vector_fixed<T,n>& mean,
                            const vnl_vector_fixed<T,n>& covar) 
    : mean_(mean), covar_(covar), det_covar_(T(-1)){}
      
  //: Destructor
  virtual ~dbsta_gaussian_indep<T,n>(){}
  
  //: Clone
  // allocate a new copy of this object.
  // the caller is responsible for deletion 
  virtual dbsta_distribution<T>* clone() const
  { return new dbsta_gaussian_indep<T,n>(*this); }
  
  //: Return the number of dimensions in the space
  virtual unsigned int dim() const { return n; }
  
  //: The mean of the distribution
  virtual const vnl_vector_ref<T> mean() const { return mean_; }
  
  //: The covariance of the distribution
  virtual vnl_matrix<T> covar() const { return covar_.asMatrix(); }
  
  //: The covariance of the distribution
  virtual vnl_diag_matrix<T> diag_covar() const { return covar_; }
  
  //: Compute the determinant of the covariance matrix
  virtual T det_covar() const;
  
  //: Set the mean of the distribution
  void set_mean(const vnl_vector_fixed<T,n>& mean) { mean_ = mean; }  
  virtual void set_mean(const vnl_vector<T>& mean)
  { set_mean(vnl_vector_fixed<T,n>(mean)); }
  
  //: Set the covariance of the distribution (and clear cached values)
  virtual void set_covar(const vnl_diag_matrix<T>& covar);
  virtual void set_covar(const vnl_matrix<T>& covar);
      
  //: The squared Mahalanobis distance to this point
  virtual T sqr_mahalanobis_dist(const vnl_vector<T>& pt) const;
 

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
  virtual vcl_string is_a() const {       vcl_ostringstream os;os<<n;
      return vcl_string("dbsta_gaussian_indep"+vcl_string(typeid(T).name())+os.str());}

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(vcl_string const& cls) const { return cls==is_a(); }

  
 protected:
  
  //: Compute the normalization constant
  virtual T norm_const() const;

 private:
  vnl_vector_fixed<T,n> mean_;
  vnl_diag_matrix<T> covar_;
  mutable T det_covar_;
};


//: Binary save dbsta_gaussian_indep* to stream.
template <class T, unsigned int n>
void vsl_b_write(vsl_b_ostream &os, const dbsta_gaussian_indep<T,n>* p);

//: Binary load dbsta_gaussian_indep* from stream.
template <class T, unsigned int n>
void vsl_b_read(vsl_b_istream &is, dbsta_gaussian_indep<T,n>* &p);



#endif // dbsta_gaussian_indep_h_
