// This is /contrib/biotree/xmvg/xmvg_composite_filter_descriptor.h

#ifndef xmvg_composite_filter_descriptor_h_
#define xmvg_composite_filter_descriptor_h_

//: 
// \file    xmvg_composite_filter_descriptor.h
// \brief   a filter descriptor container.
// \author  Kongbin Kang
// \date    2006-01-16
// 
#include <vcl_vector.h>
#include <xmvg/xmvg_filter_descriptor.h>

class xmvg_composite_filter_descriptor{
  public:
    //: constructor
    xmvg_composite_filter_descriptor(vcl_vector<xmvg_filter_descriptor> const& fds);

    //: empty constructor
    xmvg_composite_filter_descriptor() {}

    //: number of filter descriptors
    int size() const { return fds_.size(); }

    vgl_box_3d<double> bounding_box() {return bounding_box_; }
    
    //: return the i-th filter descriptor
   xmvg_filter_descriptor const& operator[](int i) const 
   {
    return fds_[i];
   }

   xmvg_filter_descriptor& operator[](int i)
   {
    return fds_[i];
   }

  protected:

    vcl_vector<xmvg_filter_descriptor> fds_;

    // bounding box encompassing all the filters in a composite filter descriptor
    
    vgl_box_3d<double> bounding_box_;

private:
    void calculate_bounding_box(void);
};

#endif
