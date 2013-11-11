// This is brcv/seg/dbsta/dbsta_gaussian_sphere.txx
#ifndef dbsta_gaussian_sphere_txx_
#define dbsta_gaussian_sphere_txx_

//:
// \file

#include "dbsta_gaussian_sphere.h"
#include <vnl/vnl_math.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vnl/io/vnl_io_diag_matrix.h>


//: Update the covariance (and clear cached values)
template <class T, unsigned int n>
void 
dbsta_gaussian_sphere<T,n>::set_var(T var) 
{ 
  normalizing_constant_ = T(-1); 
  var_ = var;
}


template <class T>
static inline bool check_diag(const vnl_matrix<T>& M)
{
  for(unsigned int i=0; i<M.rows(); ++i)
    for(unsigned int j=0; j<M.cols(); ++j)
      if(i!=j && M(i,j) != T(0)) return false;
  return true;
}

template <class T>
static inline bool check_sphere(const vnl_matrix<T>& M)
{
  T var = M(0,0);
  for(unsigned int i=1; i<M.rows(); ++i)
    if(M(i,i) != var) return false;
  return true;
}

//: Update the covariance (and clear cached values)
template <class T, unsigned int n>
void 
dbsta_gaussian_sphere<T,n>::set_covar(const vnl_matrix<T>& covar) 
{ 
  assert(covar.rows() == n);
  assert(covar.rows() == covar.cols());
  assert(check_diag(covar));
  assert(check_sphere(covar));
  var_ = covar(0,0);
  normalizing_constant_ = T(-1); 
}


//: Compute the determinant of the covariance matrix
template <class T, unsigned int n>
T 
dbsta_gaussian_sphere<T,n>::det_covar() const
{
  T det = 1.0;
  for(unsigned i=0; i<dim(); ++i) det *= var_;
  return det;
}


//: Compute the normalization constant
template <class T, unsigned int n>
T
dbsta_gaussian_sphere<T,n>::norm_const() const
{
  if(normalizing_constant_ < 0)
    normalizing_constant_ = vcl_pow(var_/(2.0*vnl_math::pi),n/2.0);
  return normalizing_constant_;
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
template <class T, unsigned int n>
void dbsta_gaussian_sphere<T,n>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, mean_);
  vsl_b_write(os, var_);
  vsl_b_write(os, normalizing_constant_);
  
}

//: Binary load self from stream. (not typically used)
template <class T, unsigned int n>
void dbsta_gaussian_sphere<T,n>::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, mean_);
  vsl_b_read(is, var_);
  vsl_b_read(is, normalizing_constant_);

    break;
   default:
    vcl_cerr << "vsol_line_2d: unknown I/O version " << ver << '\n';
  }
}
//: Return IO version number;
template <class T, unsigned int n>
short dbsta_gaussian_sphere<T,n>::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
template <class T, unsigned int n>
void dbsta_gaussian_sphere<T,n>::print_summary(vcl_ostream &os) const
{
  //os << *this;
}

//: Binary save dbsta_gaussian_sphere to stream.
template<class T, unsigned int n>
void
vsl_b_write(vsl_b_ostream &os, const dbsta_gaussian_sphere<T,n>* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load dbsta_gaussian_sphere from stream.
template<class T, unsigned int n>
void
vsl_b_read(vsl_b_istream &is, dbsta_gaussian_sphere<T,n>* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new dbsta_gaussian_sphere<T,n>();
    p->b_read(is);
  }
  else
    p = 0;
}


#define DBSTA_GAUSSIAN_SPHERE_INSTANTIATE(T,n) \
template class dbsta_gaussian_sphere<T,n >;\
template void vsl_b_read(vsl_b_istream &, dbsta_gaussian_sphere<T,n> *&); \
template void vsl_b_write(vsl_b_ostream &, const dbsta_gaussian_sphere<T,n> *); \



#endif // dbsta_gaussian_sphere_txx_
