#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_math.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <xmvg/xmvg_source.h>
#include <xscan/xscan_uniform_orbit.h>
#include "imgr_dummy_scan.h"

imgr_dummy_scan::imgr_dummy_scan()
{
  //: setup a testing scan
  vnl_double_3 axis(0,0,1);
  vnl_quaternion<double> rot(axis, 0.6*vnl_math::pi/180);

#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  vnl_double_3 axis1(0,0,1);
#endif

  vnl_double_3 t(0,0,0);

  vnl_double_3x3 R0(0.0);
  
  R0[0][0] = 1;
  R0[1][2] = -1;
  R0[2][1] = 1;
  R0[3][3] = 1;


  vnl_quaternion<double> r0(R0);

  vnl_double_3 t0(0, 0, 18);

  xscan_orbit_base_sptr orbit = new xscan_uniform_orbit(rot, t,  r0, t0);

  // a camera
  vnl_double_3x3 m(0.0);

  m[0][0] = 2022.5; m[0][1]=0;m[0][2]=1000;
  m[1][1]=2022.5; m[1][2]=524;
  m[2][2]=1;
  
  vpgl_calibration_matrix<double> K(m);
  
  // image path
  vcl_string path="c:/images/CAD/MouseSmall/18_al05_1770_####.tif";

  // numer of views
  const int nviews = 600;
 
  // source
  xmvg_source source;

  // assemble those above into a scan plan
  n_views_ = nviews;
  image_file_path_ = path;
  source_ = source;
  kk_ = K;
  orbit_ = orbit;

}

imgr_dummy_scan::~imgr_dummy_scan()
{
}
