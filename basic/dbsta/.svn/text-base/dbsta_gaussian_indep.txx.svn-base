// This is brcv/seg/dbsta/dbsta_gaussian_indep.txx
#ifndef dbsta_gaussian_indep_txx_
#define dbsta_gaussian_indep_txx_

//:
// \file

#include "dbsta_gaussian_indep.h"
#include <vnl/vnl_math.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vnl/io/vnl_io_diag_matrix.h>

//: Update the covariance (and clear cached values)
template <class T, unsigned int n>
void 
dbsta_gaussian_indep<T,n>::set_covar(const vnl_diag_matrix<T>& covar) 
{ 
  det_covar_ = T(-1); 
  covar_ = covar;
}


template <class T>
static inline bool check_diag(const vnl_matrix<T>& M)
{
  for(unsigned int i=0; i<M.rows(); ++i)
    for(unsigned int j=0; j<M.cols(); ++j)
      if(i!=j && M(i,j) != T(0)) return false;
  return true;
}

//: Update the covariance (and clear cached values)
template <class T, unsigned int n>
void 
dbsta_gaussian_indep<T,n>::set_covar(const vnl_matrix<T>& covar) 
{ 
  assert(covar.rows() == n);
  assert(covar.rows() == covar.cols());
  assert(check_diag(covar));
  for(unsigned int i=0; i<n; ++i)
    covar_[i] = covar(i,i);
  det_covar_ = T(-1); 
}


//: Compute the determinant of the covariance matrix
template <class T, unsigned int n>
T 
dbsta_gaussian_indep<T,n>::det_covar() const
{
  if(det_covar_ < 0){
    det_covar_ = covar_.determinant(); 
  }
  return det_covar_;
}


//: Compute the normalization constant
template <class T, unsigned int n>
T
dbsta_gaussian_indep<T,n>::norm_const() const
{
  T d = det_covar();
  if(d<=T(0))
    return T(0);
    
  T z = 1.0,  two_pi = 2.0*vnl_math::pi;
  for(unsigned i=0; i<dim(); ++i) z *= two_pi;
  return vcl_sqrt(1/(d * z));
}

 
//: The squared Mahalanobis distance to this point
template <class T, unsigned int n>
T
dbsta_gaussian_indep<T,n>::sqr_mahalanobis_dist(const vnl_vector<T>& pt) const 
{ 
  const vnl_vector<T> &d = mean_-pt;
     
  if(det_covar()==T(0))
    return vcl_numeric_limits<T>::infinity();
  T result = dot_product(d,(covar_.solve(d)));
  if(result<T(0))
    return vcl_numeric_limits<T>::infinity();
  
  return result;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
template <class T, unsigned int n>
void dbsta_gaussian_indep<T,n>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, mean_);
  vsl_b_write(os, covar_);
  vsl_b_write(os, det_covar_);
  
}

//: Binary load self from stream. (not typically used)
template <class T, unsigned int n>

void dbsta_gaussian_indep<T,n>::b_read(vsl_b_istream &is)
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
short dbsta_gaussian_indep<T,n>::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
template <class T, unsigned int n>
void dbsta_gaussian_indep<T,n>::print_summary(vcl_ostream &os) const
{
  //os << *this;
}

//: Binary save dbsta_gaussian_indep to stream.
template<class T, unsigned int n>
void
vsl_b_write(vsl_b_ostream &os, const dbsta_gaussian_indep<T,n>* p)
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
vsl_b_read(vsl_b_istream &is, dbsta_gaussian_indep<T,n>* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new dbsta_gaussian_indep<T,n>();
    p->b_read(is);
  }
  else
    p = 0;
}


#define DBSTA_GAUSSIAN_INDEP_INSTANTIATE(T,n) \
template class dbsta_gaussian_indep<T,n >;\
template void vsl_b_read(vsl_b_istream &,  dbsta_gaussian_indep<T,n> * &); \
template void vsl_b_write(vsl_b_ostream &, const dbsta_gaussian_indep<T,n> *); \


#endif // dbsta_gaussian_indep_txx_
