// This is /contrib/biotree/xmvg/xmvg_atomic_filter_2d_base.h

#ifndef xmvg_atomic_filter_2d_base_h_
#define xmvg_atomic_filter_2d_base_h_

//: 
// \file    xmvg_atomic_filter_2d_base.h
// \brief   an abstract class to represent splates from composite_filter_3d
// \author  Kongbin Kang and H. Can Aras
// \date    2005-03-17
// 
#include <vnl/vnl_int_2.h>
#include <vil/vil_image_view_base.h>
#include "xmvg_filter_response.h"

template<class T>
class xmvg_atomic_filter_2d_base
{
  public:

    //: return the size of the 2d filter
    virtual vnl_int_2 size() const = 0;
    
    //: an interface.for applying filter to image
//    virtual xmvg_filter_response<T> apply(vil_image_view_base& v ) = 0;

    //: return the center filter 2d
    virtual vnl_int_2 location() const = 0;

    //destructor
    virtual ~xmvg_atomic_filter_2d_base(){}
    
  private:
    
};

#endif
