// This is brcv/seg/dbsta/dbsta_gaussian_full.txx
#ifndef dbsta_gaussian_full_txx_
#define dbsta_gaussian_full_txx_

//:
// \file

#include "dbsta_gaussian_full.h"
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_math.h>
#include <vcl_algorithm.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_matrix_fixed.h>

#include <vnl/io/vnl_io_vector_fixed.h>
#include <vnl/io/vnl_io_diag_matrix.h>


//: Update the covariance (and clear cached values)
template <class T, unsigned int n>
void 
dbsta_gaussian_full<T,n>::set_covar(const vnl_matrix_fixed<T,n,n>& covar) 
{ 
  det_covar_ = T(-1); 
  delete inv_covar_; 
  inv_covar_ = NULL;
  covar_ = covar;
}


//: Compute the determinant of the covariance matrix
template <class T, unsigned int n>
T 
dbsta_gaussian_full<T,n>::det_covar() const
{
  if(det_covar_ < 0){
    det_covar_ = vnl_determinant(covar_); 
  }
  return det_covar_;
}


//: Compute the normalization constant
template <class T, unsigned int n>
T
dbsta_gaussian_full<T,n>::norm_const() const
{
  T d = det_covar();
  if(d<=T(0))
    return T(0);
    
  T z = 1.0,  two_pi = 2.0*vnl_math::pi;
  for(unsigned i=0; i<dim(); ++i) z *= two_pi;
  return vcl_sqrt(1/(d * z));
}


//: Return the inverse of the covariance matrix
// \note this matrix is cached and updated only when needed
template <class T, unsigned int n>
const vnl_matrix_fixed<T,n,n>&
dbsta_gaussian_full<T,n>::inv_covar() const
{
  if(!inv_covar_){ 
    vnl_matrix_fixed<T,n,n> C = covar_;
    if(det_covar() == T(0)){
      // if the matrix is singular we can add a small lambda*I
      // before inverting to avoid divide by zero
      // Is this the best way to handle this?
      T lambda = T(0);
      for(unsigned i=0; i<n; ++i)
        lambda = vcl_max(lambda,C(i,i));
      lambda *= 1e-4;  
      for(unsigned i=0; i<n; ++i)
        C(i,i) += lambda;
    }
    if(n<5)
      inv_covar_ = new vnl_matrix_fixed<T,n,n>(vnl_inverse(vnl_matrix<T>(C)));
    else
      inv_covar_ = new vnl_matrix_fixed<T,n,n>(vnl_svd_inverse(vnl_matrix<T>(C)));
  }
  return *inv_covar_;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
template <class T, unsigned int n>
void dbsta_gaussian_full<T,n>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, mean_);
  vsl_b_write(os, covar_);
  vsl_b_write(os, det_covar_);
  
}

//: Binary load self from stream. (not typically used)
template <class T, unsigned int n>
void dbsta_gaussian_full<T,n>::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, mean_);
  vsl_b_read(is, covar_);
  vsl_b_read(is, det_covar_);

    break;
   default:
    vcl_cerr << "vsol_line_2d: unknown I/O version " << ver << '\n';
  }
}
//: Return IO version number;
template <class T, unsigned int n>
short dbsta_gaussian_full<T,n>::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
template <class T, unsigned int n>
void dbsta_gaussian_full<T,n>::print_summary(vcl_ostream &os) const
{
  //os << *this;
}

//: Binary save dbsta_gaussian_full to stream.
template<class T, unsigned int n>
void
vsl_b_write(vsl_b_ostream &os, const dbsta_gaussian_full<T,n>* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load dbsta_gaussian_indep from stream.
template<class T, unsigned int n>
void
vsl_b_read(vsl_b_istream &is, dbsta_gaussian_full<T,n>* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new dbsta_gaussian_full<T,n>();
    p->b_read(is);
  }
  else
    p = 0;
}

#define DBSTA_GAUSSIAN_FIXED_INSTANTIATE(T,n) \
template class dbsta_gaussian_full<T,n >;\
template void vsl_b_read(vsl_b_istream &, dbsta_gaussian_full<T,n> * &); \
template void vsl_b_write(vsl_b_ostream &, const dbsta_gaussian_full<T,n> *); \


#endif // dbsta_gaussian_full_txx_
