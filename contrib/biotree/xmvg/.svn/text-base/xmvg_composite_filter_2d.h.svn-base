// This is /contrib/biotree/xmvg/xmvg_composite_filter_2d.h

#ifndef XMVG_COMPOSITE_FILTER_2D_H_
#define XMVG_COMPOSITE_FILTER_2D_H_
//: 
// \file    xmvg_composite_filter_2d.h
// \brief   This is a container for multiple 2D atomic filters
// \author  H. Can Aras and Kongbin Kang
// \date    2005-03-18
// 

#include "xmvg_atomic_filter_2d.h"
#include <bio_defs.h>
#include <vsl/vsl_binary_io.h>
#include <xmvg/io/xmvg_io_atomic_filter_2d.h>
#include <vsl/vsl_vector_io.h>

template< class T>
class xmvg_composite_filter_2d
{
public:

  xmvg_composite_filter_2d(){}
    

  xmvg_composite_filter_2d( const vcl_vector< xmvg_atomic_filter_2d<T> >& filters) 
    : filters_(filters) {}

  //: access filter
  xmvg_atomic_filter_2d<T> & atomic_filter(unsigned i) { return filters_[i];}

  xmvg_atomic_filter_2d<T> const & atomic_filter(unsigned i) const { return filters_[i];}
 
  //: size of the filter
  int size() const { return filters_.size(); }

  //: used for testing in splr_splat_collection
  bool operator==(const xmvg_composite_filter_2d<T> & other) const{
      return filters_ == other.filters_;
  }
  void set_filters(const vcl_vector< xmvg_atomic_filter_2d<T> >& filters)
    {filters_=filters;}

  //: make residues zero
  void eliminate_residue_effect(){
    int num_filters = filters_.size();
    for(int num=0; num < num_filters; num++)
      filters_[num].eliminate_residue_effect();
  }

  //: convolve each row of the filter by the given kernel
  void convolve_rows_with_kernel(vcl_vector<T> kernel)
  {
    int num_filters = filters_.size();
    for(int num=0; num < num_filters; num++)
      filters_[num].convolve_rows_with_kernel(kernel);
  }

  vcl_vector< xmvg_atomic_filter_2d<T> > filters_;
};


#endif
