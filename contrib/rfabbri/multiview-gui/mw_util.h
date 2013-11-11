// This is mw_util.h
#ifndef mw_util_h
#define mw_util_h
//:
//\file
//\brief General utilities (probably to be moved into some vxl libs)
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 04/25/2005 10:28:03 AM EDT
//
#include <vsol/vsol_point_2d_sptr.h>
#include <vcl_vector.h>
#include <vcl_string.h>


// Header files that are generally needed, but not necessarily here:

#include <vcl_iostream.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>



bool load_con_file(
      vcl_string filename, 
      vcl_vector<vsol_point_2d_sptr> &points, 
      bool *is_open);

bool con_filenames(vcl_string image_fname,vcl_vector<vcl_string> &con_fnames);


bool read_cam(vcl_string img_name1, vcl_string img_name2, 
      vpgl_perspective_camera <double> *P1out,
      vpgl_perspective_camera <double> *P2out);
bool read_cam( vcl_string img_name1, 
      vpgl_perspective_camera <double> *P1out);

bool mw_get_prefix(vcl_string img_name, vcl_string *dir, vcl_string *prefix);

vgl_homg_point_2d<double> mw_epipolar_point_transfer( 
      const vgl_homg_point_2d<double> &p1, 
      const vgl_homg_point_2d<double> &p2, 
      const vpgl_fundamental_matrix<double> &f13,
      const vpgl_fundamental_matrix<double> &f23);

inline double
d_sqr(double x1, double y1, double x2, double y2)
{
   double dx= x1-x2, dy = y1-y2;
   return dx*dx + dy*dy;
}

#define mw_tolerance 1e-7
class mw_util{
public:
   static bool near_zero(double x) { return vcl_fabs(x) < mw_tolerance; }
};

#endif // mw_util_h
