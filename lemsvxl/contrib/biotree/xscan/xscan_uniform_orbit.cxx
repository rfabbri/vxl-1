#include "xscan_uniform_orbit.h"
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vnl/xio/vnl_xio_quaternion.h>
#include <vnl/xio/vnl_xio_vector_fixed.h>

// a point in world coordinate system at i-th frame is, x'_w = rot(i) x_w + i t.
//  Then it map to camera coordinates as x_c'= r0 (rot(i) x_w + i t) + t0. 
// In the terms of x_w', we have x_c(i) = r0 rot(i)^{-1} (x_w' - i t) + t0 
// therefore the external parameter of i-th camera is a 3x4 matrix 
// [r0 rot(i)^{-1}, -r0 rot(i)^{-1} i t +t0]
vgl_h_matrix_3d<double> xscan_uniform_orbit::global_transform(const orbit_index i) const
{
  
  // r0 rot(i)^{-1}

  double angle = rot_.angle();

  vnl_vector_fixed<double,3> axis = rot_.axis();

  vnl_quaternion<double> irot(axis, i*angle);

  
 vnl_quaternion<double> r_i =  irot.inverse()* r0_;

#if 0 
  vnl_double_3x3 r0_m = r0_.rotation_matrix_transpose();
  vnl_double_3x3 irot_m = irot.rotation_matrix_transpose();

  vcl_cout << "rotation axis is" << irot.rotation_matrix_transpose() << '\n';


  vcl_cout << r0_m * irot_m << '\n';
#endif
  vnl_double_3x3 R = r_i.rotation_matrix_transpose();

  return vgl_h_matrix_3d<double> (R, -double(i)*R*t_+t0_);

}

// add this function later
//bool xscan_uniform_orbit::operator==(xscan_uniform_orbit const& other) const
//{
//  return true;
//}

//
// input and output
//
vcl_ostream& xscan_uniform_orbit::output (vcl_ostream& stream) const
{
  stream.precision(15);
  stream << "xscan_uniform_orbit\n";
  stream << "rotation_quaternion " << rot_ << '\n';
  stream << "pitch_is " << t_ << '\n';
  stream << "rotation_quaternion_to_the_first_camera " << r0_ <<'\n';
  stream << "translation_to_the_first_camera " << t0_ <<'\n';

  return stream;
}

vcl_istream& xscan_uniform_orbit::input (vcl_istream& stream)
{
  vcl_string str;
  stream >> str;
  if(str != "xscan_uniform_orbit"){
    vcl_cerr << "incorrect istream for xscan_uniform_orbit\n";
    vcl_exit(1);
  }
  stream >> str >> rot_;
  stream >> str >> t_ ;
  stream >> str >> r0_ ;
  stream >> str >> t0_ ;

  return stream;
}

vcl_ostream& operator << (vcl_ostream& stream, const xscan_uniform_orbit& orbit) 
{
  return orbit.output(stream);
}

vcl_istream& operator >> (vcl_istream& stream, xscan_uniform_orbit& orbit) 
{
  return orbit.input(stream);
}

void x_write(vcl_ostream& os, xscan_uniform_orbit orbit, vcl_string name)
{
  vsl_basic_xml_element element(name);
  element.x_write_open(os);
  x_write(os, orbit.rot(), "rotation_quaternion");
  x_write(os, orbit.t(),  "pitch_is");
  x_write(os, orbit.r0(), "rotation_quaternion_to_the_first_camera");
  x_write(os, orbit.t0(), "translation_to_the_first_camera");

  element.x_write_close(os);
}
