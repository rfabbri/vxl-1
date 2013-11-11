// This is /contrib/biotree/xmvg/xmvg_svd.h

#ifndef xmvg_svd_h_
#define xmvg_svd_h_

//: 
// \file    xmvg_svd.h
//          New svd class derived from vnl_svd to make the backproject function
//          more efficient by caching frequently used members
// \brief   
// \author  H. Can Aras
// \date    2005-05-27
// 

#include <vcl_iostream.h>
#include <vnl/algo/vnl_svd.h>

template <class T>
class xmvg_svd : public vnl_svd<T>
{
  public:
    xmvg_svd(vnl_matrix<T> const &M);
    vnl_vector<T> xmvg_solve(vnl_vector<T> const& y)  const;
  private:
    // cached for efficiency
    int my_m_, my_n_; //size of M
    vnl_matrix<T> my_U_;
    vnl_matrix<T> my_V_;
    
    typedef typename vnl_numeric_traits<T>::abs_t singval_t;
    
    vnl_diag_matrix<singval_t> my_W_;

    vnl_matrix<T> ct_U_; // conjugate transpose of my_U_
};

#endif
