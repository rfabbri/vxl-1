#include "xscan_scan.h"
#include <vcl_fstream.h>
#include <xscan/xscan_uniform_orbit.h>

#include <vsl/vsl_basic_xml_element.h>
#include <vpgl/xio/vpgl_xio_calibration_matrix.h>
#include <vnl/vnl_math.h>


void xscan_scan::correct_uniform_orbit(xscan_scan& scan, const double& rotation_step, const int& n_views)
{
  if(n_views != scan.n_views()){
    xscan_orbit_base_sptr orbit_base = (scan.orbit());
    xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
    vcl_cerr << "old orbit was : " << orbit << "\n";

    vnl_double_3 rot_axis(0.0, 0.0, 1.0);
    // rotation in clockwise direction
    double rot_step = -rotation_step * vnl_math::pi / 180;
    vnl_quaternion<double> rot(rot_axis, rot_step);
    xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(rot,
        orbit.t(),
        orbit.r0(),
        orbit.t0());
    scan.set_orbit(new_orbit);
    vcl_cerr << "new orbit is : " << static_cast<const xscan_uniform_orbit&>(*new_orbit) << "\n";
    scan.set_scan_size(n_views);
    vcl_cerr << "new scan size : " << scan.n_views() << "\n";
  }
}

xmvg_source xscan_scan::source() const
{
  return source_;
}

xmvg_perspective_camera<double> xscan_scan::operator()(orbit_index i)
{
   // Euclidean transformation
   vgl_h_matrix_3d<double> RT = orbit_->global_transform(i); 

   return xmvg_perspective_camera<double>(kk_, RT, source_);
   
}

xmvg_perspective_camera<double> xscan_scan::operator()(orbit_index i) const 
{
   // Euclidean transformation
   vgl_h_matrix_3d<double> RT = orbit_->global_transform(i); 

   return xmvg_perspective_camera<double>(kk_, RT, source_);
   
}

xscan_scan::xscan_scan(unsigned nviews, 
                      const vcl_string path,
                      const xmvg_source& source, 
                      const vpgl_calibration_matrix<double>& K,
                      xscan_orbit_base_sptr orbit) :
  n_views_(nviews), image_file_path_(path), source_(source), kk_(K), orbit_(orbit)
{
}

vcl_ostream& operator << (vcl_ostream& stream, const xscan_scan& scan)
{
  stream.precision(15);
  stream << scan.n_views_ << '\n';
  stream << scan.image_file_path_ << '\n';
  stream << scan.kk_.get_matrix() << '\n';
  // what kind of orbit it has
  stream << scan.orbit_->class_id()<<'\n';
  scan.orbit_->output(stream);
  return stream;
}


vcl_istream& operator >> (vcl_istream& stream, xscan_scan & scan)
{
  vnl_double_3x3 K;
  vcl_string str;
  stream >> scan.n_views_;
  stream >> scan.image_file_path_  >>   K;
  scan.kk_ = K;
  stream >> str;
  if(scan.orbit_ == (xscan_orbit_base*) 0)
  {
    if(str == "xscan_uniform_orbit")
      scan.orbit_ = new xscan_uniform_orbit;
  }
  scan.orbit_->input(stream);
  return stream;
}

void x_write(vcl_ostream& os, xscan_scan scan)
{
  vsl_basic_xml_element element("xscan_scan");
  element.x_write_open(os);

  vsl_basic_xml_element nviews("nviews");
  nviews.append_cdata((int) scan.n_views());
  nviews.x_write(os);

  vsl_basic_xml_element path("image_file_path");
  path.append_cdata(scan.image_file_path());
  path.x_write(os);

  vpgl_calibration_matrix<double> kk = scan.kk();
  x_write(os, kk);
  xmvg_source src = scan.source();
  x_write(os, src);

  // what kind of orbit it has
  //vsl_basic_xml_element orbit("orbit_type");
  //orbit.append_cdata(scan.orbit()->class_id());
  //orbit.x_write(os);
  
  if(scan.orbit()!= (xscan_orbit_base*) 0)
    if (vcl_strcmp(scan.orbit()->class_id().data(), "xscan_uniform_orbit") == 0) {
      xscan_orbit_base_sptr orbit_base = (scan.orbit());
      xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
      x_write(os, orbit, "xscan_uniform_orbit");
    }
  element.x_write_close(os);
}
