#ifndef XSCAN_ORBIT_BASE_H_
#define XSCAN_ORBIT_BASE_H_
//: 
// \file  xscan_orbits_base.h
// \brief  the base orbit class used to define interface. This is 
//         an abstract class and should not be used directly
// \author    Kongbin Kang
// \date        2005-02-25
// 
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <bio_defs.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

class xscan_orbit_base : public vbl_ref_count
{
  public:

    // RTTI
    virtual vcl_string class_id() { return "xscan_orbit_base";}
    
    //: constructor
    xscan_orbit_base() : vbl_ref_count() {}

    //:get the transform matrix from world coordinate system to ith frame coordinate 
    //system which in most case is camera frame coordinate system
    virtual vgl_h_matrix_3d<double> global_transform(const orbit_index i) const =0;

    // stream input and output
    virtual vcl_ostream& output(vcl_ostream& stream) const = 0;

    virtual vcl_istream& input(vcl_istream& stream) = 0;

  private:

};
 
#endif

