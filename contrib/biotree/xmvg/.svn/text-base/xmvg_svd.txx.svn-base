// This is contrib/biotree/xmvg/xmvg_svd.txx
#ifndef xmvg_svd_txx_
#define xmvg_svd_txx_
//:
// \file
#include <vcl_cassert.h>
#include <xmvg/xmvg_svd.h>


//-------------------------------------------
template <class T>
xmvg_svd<T>::xmvg_svd(vnl_matrix<T> const &M) : 
  vnl_svd<T>(M)
{
  my_m_ = M.rows();
  my_n_ = M.columns();
  my_U_ = this->U();
  my_W_ = this->W();
  my_V_ = this->V();
  ct_U_ = my_U_.conjugate_transpose();
}

//: Solve the matrix-vector system M x = y, returning x.
template <class T>
vnl_vector<T> xmvg_svd<T>::xmvg_solve(vnl_vector<T> const& y)  const
{
  // fsm sanity check :
  if (y.size() != my_U_.rows())
  {
    vcl_cerr << __FILE__ << ": size of rhs is incompatible with no. of rows in U_\n"
             << "y =" << y << '\n'
             << "m_=" << my_m_ << '\n'
             << "n_=" << my_n_ << '\n'
             << "U_=\n" << my_U_
             << "V_=\n" << my_V_
             << "W_=\n" << my_W_;
  }

  vnl_vector<T> x(my_V_.rows());                   // Solution matrix.
  if (my_U_.rows() < my_U_.columns()) {               // Augment y with extra rows of
    vnl_vector<T> yy(my_U_.rows(), T(0));          // zeros, so that it matches
    if (yy.size()<y.size()) { // fsm
      vcl_cerr << "yy=" << yy << vcl_endl
               << "y =" << y  << vcl_endl;
      // the update() call on the next line will abort...
    }
    yy.update(y);                               // cols of u.transpose.
    x = ct_U_ * yy;
  }
  else
    x = ct_U_ * y;

  for (unsigned i = 0; i < x.size(); i++) {        // multiply with diagonal 1/W
    T weight = my_W_(i, i), zero_(0);
    if (weight != zero_)
      x[i] /= weight;
    else
      x[i] = zero_;
  }
  return my_V_ * x;                                // premultiply with v.
}


// Code for easy instantiation.
#undef XMVG_SVD_INSTANTIATE
#define XMVG_SVD_INSTANTIATE(T) \
template class xmvg_svd<T>;

#endif // xmvg_svd_txx_
