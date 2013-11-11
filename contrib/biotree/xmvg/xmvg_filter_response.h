// This is /contrib/biotree/xmvg/xmvg_filter_response.h

#ifndef xmvg_filter_response_h_
#define xmvg_filter_response_h_

//: 
// \file    xmvg_filter_response.h
// \brief   a data structure to hold composite filter response
// \author  Kongbin Kang and H. Can Aras
// \date    2005-02-22
// 
#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector.h>
#include <vcl_string.h>

template <class T>
class xmvg_filter_response : public vbl_ref_count
{
  public:
    xmvg_filter_response(unsigned const & size = 0, T const &val = 0) 
      : res_(size, val) { }
    
    xmvg_filter_response(const vnl_vector<T> & res) 
      : res_(res) {}
    
    //: RTTI
    vcl_string class_id() {return "xmvg_filter_response";}

    T& operator[](unsigned i) { return res_[i];}
    const T& operator[](unsigned i) const { return res_[i];}

    T& get(unsigned i) { return res_[i];}
    const T& get(unsigned i) const { return res_[i];}

    unsigned const size() const { return res_.size(); } 

    T sum() { return res_.sum(); }

    bool all_zero() 
    {
      bool zero = true;
      for (unsigned int i=0; i<res_.size(); i++){
        if (res_[i] != 0)
          zero = false;
      }
      return zero;
    }

    xmvg_filter_response<T> & operator += (xmvg_filter_response<T> const & rhs) 
    {
      
      res_ += rhs.res_;
      return *this;
    }

    xmvg_filter_response<T> & operator /= (T const & rhs) 
    {
      for (unsigned int i=0; i<res_.size(); i++){
        res_[i] /= rhs;
      }
      return *this;
    } 

  double min_value() { return res_.min_value(); }

  double max_value() { return res_.max_value(); }

  protected:
    vnl_vector<T> res_;
};

template <class T>
void x_write(vcl_ostream& os, xmvg_filter_response<T> res);

template <class T>
vcl_ostream& operator << ( vcl_ostream& stream, const xmvg_filter_response<T> & resp);
#endif
