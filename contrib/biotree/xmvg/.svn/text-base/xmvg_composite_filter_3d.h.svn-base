// This is /contrib/biotree/xmvg/xmvg_composite_filter_3d.h

#ifndef xmvg_composite_filter_3d_h_
#define xmvg_composite_filter_3d_h_
//: 
// \file    xmvg_composite_filter_3d.h
// \brief   this is an container for multiple atomic filters.
// \author  Kongbin Kang and H. Can Aras
// \date    2005-02-15
//
#include <vcl_vector.h>
#include <vsl/vsl_basic_xml_element.h>
#include "xmvg_perspective_camera.h"
#include "xmvg_composite_filter_2d.h"
#include "xmvg_atomic_filter_3d.h"
#include <bio_defs.h>

//: class F should be derived from class xmvg_atomic_filter_3d
template< class T, class F>
class xmvg_composite_filter_3d
{
public:
  //: empty filter
  xmvg_composite_filter_3d() {}
  
  //: 
  xmvg_composite_filter_3d(vcl_vector<F > & filters) 
    : filters_(filters) {}
  
  //: generate splats from 3d filter at location (x,y,z) 
  xmvg_composite_filter_2d<T> splat(const xmvg_perspective_camera<double> & camera,
                                         const vgl_point_3d<double>& c)
  {
    unsigned size = filters_.size();
    vcl_vector<xmvg_atomic_filter_2d<T> > splats(size);

    for(unsigned i = 0; i < size; i++){
      splats[i] = filters_[i].splat(c, camera);
    }

    return splats;
    
  }

  
#if 0  
  //deprecated 
  void push_back_atomic_filter(const F & af) 
  {
    filters_.push_back(af);
  }
#endif
  
  unsigned size() { return filters_.size();}

  friend void x_write(vcl_ostream& os, xmvg_composite_filter_3d<T, F>  f)
  {
    vsl_basic_xml_element element("xmvg_composite_filter_3d");
    element.x_write_open(os);
    for (unsigned i=0; i < f.size(); i++) 
      x_write(os, f.filters_[i]);
    element.x_write_close(os);
  }
protected:
  vcl_vector<F >  filters_;
};

#endif
