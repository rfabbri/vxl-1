#ifndef XSCAN_UNIFORM_ORBIT_H_
#define XSCAN_UNIFORM_ORBIT_H_
//: 
// \file     xscan_uniform_orbits.h
// \brief    the orbit class used to generate a set of 4x4 homogenous transformation matrix. 
//           Note: in this class we assume the rotation axis is located on z-axi of world coordinate system 
// \author   Kongbin Kang and H. Can Aras
// \date     2005-02-25
// 
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <bio_defs.h>
#include "xscan_orbit_base.h"

class xscan_uniform_orbit : public xscan_orbit_base
{
  public:

    //: default constructor
    xscan_uniform_orbit() {}
    
    //: construct orbit from a rotation quaternion and translation
    xscan_uniform_orbit(const vnl_quaternion<double>& rot,
                        const vnl_double_3& t,
                        const vnl_quaternion<double>& r0,
                        const vnl_double_3& t0):
      rot_(rot), t_(t),r0_(r0), t0_(t0){}

    //: RTTI
    vcl_string class_id() {return "xscan_uniform_orbit";}
    
    //:get the transform matrix from world coordinate system to ith frame coordinate 
    //system which in most case is camera frame coordinate system
    vgl_h_matrix_3d<double> global_transform(const orbit_index i) const;

    //: equality check, add this function later
//    bool operator==(xscan_uniform_orbit const& other) const;

    // stream input and output
    vcl_ostream& output(vcl_ostream& stream) const; 

    vcl_istream& input(vcl_istream& stream);

    //: get rot
    vnl_quaternion<double> rot() { return rot_; }
    //: get t
    vnl_double_3 t() { return t_; }
    //: get r0
    vnl_quaternion<double> r0() { return r0_; }
    //: get t0
    vnl_double_3 t0() { return t0_; }

  private:

    //: incremental rotation of the turner table in the world coordinate system 
    vnl_quaternion<double> rot_;

    //: incremental translation of tuner table in the world coordinate
    vnl_double_3 t_;
    
    //: transformation from world coordinate to the first camera frame coordinates 
    // therefore the first camera's external parameter is [R0, T0]
    vnl_quaternion<double> r0_;
   
    //: it usually is  T0 = - R0 C where C is camera center
    vnl_double_3 t0_;

};


vcl_ostream& operator << (vcl_ostream& stream, const xscan_uniform_orbit& orbit) ;

vcl_istream& operator >> (vcl_istream& stream, xscan_uniform_orbit& orbit) ;

void x_write(vcl_ostream& stream, xscan_uniform_orbit b, vcl_string name);
   
#endif

