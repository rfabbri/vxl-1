#include "imgr_skyscan_log.h"
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_quaternion.h>
#include <xscan/xscan_orbit_base.h>
#include <xscan/xscan_uniform_orbit.h>
#include <vil/vil_load.h>
#include <vul/vul_file_iterator.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>
#include <vcl_cstddef.h>
#include <vsl/vsl_basic_xml_element.h>

// Function copied from imgr_scan_resource_io.cxx file written by Prof. Mundy
// Look for a set of filenames that match the glob spec in filename
// The globbing format expects only '#' to represent numbers.
// Do not use "*" or "?"
// All "#" should be in one contiguous group.
static void parse_globbed_filenames(const vcl_string & input,
                                    vcl_vector<vcl_string> &filenames)
{
  filenames.clear();
  vcl_string filename = input;

  // Avoid confusing globbing functions
  if (filename.find("*") != filename.npos) return;
  if (filename.find("?") != filename.npos) return;

  // Check that all the #s are in a single group.
  vcl_size_t start = filename.find_first_of("#");
  if (start == filename.npos) return;
  vcl_size_t end = filename.find_first_not_of("#", start);
  if (filename.find_first_of("#",end) != filename.npos) return;
  if (end == filename.npos) end = filename.length();
  for (vcl_size_t i=start, j=start; i!=end; ++i, j+=12)
    filename.replace(j,1,"[0123456789]");


  // Search for the files
  for (vul_file_iterator fit(filename); fit; ++fit)
    filenames.push_back(fit());


  if (filenames.empty()) return;

  start = (start + filenames.front().size()) - input.size();
  end = (end + filenames.front().size()) - input.size();

  // Put them all in numeric order.
  vcl_sort(filenames.begin(), filenames.end());

  // Now discard non-contiguously numbered files.
  long count = vcl_atol(filenames.front().substr(start, end-start).c_str());
  vcl_vector<vcl_string>::iterator it=filenames.begin()+1;
  while (it != filenames.end())
  {
    if (vcl_atol(it->substr(start, end-start).c_str()) != ++count)
      break;
    ++it;
  }
  filenames.erase(it, filenames.end());
}

imgr_skyscan_log::imgr_skyscan_log(vcl_string log_fname)
: imgr_scan_images_resource()
{
  vcl_FILE *fp = vcl_fopen(log_fname.c_str(), "r");
  if(!fp){
    vcl_cout << "skyscan log file open failed\n";
    exit(1);
  }
  vcl_string images_fname(log_fname);
  int cut_point = images_fname.find(".log");
  images_fname.replace(cut_point, 8, "####.tif"); 

  imgr_skyscan_log_header header(fp);
  header_ = header;
  images_fname_ = images_fname;
  vcl_fclose(fp);

  compute_scan();
}

imgr_skyscan_log::~imgr_skyscan_log()
{
}

void imgr_skyscan_log::compute_scan()
{
  // I will not follow Kongbin's method of defining another class
  // derived from xscan_scan. I will create the xscan_scan object
  // directly since I have all the necessary setter functions in 
  // that class.
  xscan_scan scan;
  // Forming the calibration matrix
  double source_sensor_dist = header_.camera_to_source_dist_ * 1000;
#if 0
  //this variable is not used in the code.  PLEASE FIX!  -MM
  double source_rot_center_dist = header_.object_to_source_dist_ * 1000;
#endif

  double pixel_size_u = header_.cam_pixel_size_;
  double pixel_size_v = header_.cam_pixel_size_ * header_.cam_xy_ratio_;

  double principal_u = (header_.number_of_columns_ - 1) / 2.0;
  double principal_v = header_.optical_axis_;
  
  vnl_double_3x3 m(0.0);
  m[0][0] = source_sensor_dist / pixel_size_u;
  m[0][1] = 0;
  m[0][2] = principal_u;
  m[1][1] = source_sensor_dist / pixel_size_v;
  m[1][2] = principal_v;
  m[2][2] = 1;

  vpgl_calibration_matrix<double> K(m);
  scan.set_calibration_matrix(K);
  // orbit
  vnl_double_3 rot_axis(0.0, 0.0, 1.0);
  // rotation in clockwise direction
  double rot_step = -header_.rotation_step_ * vnl_math::pi / 180;
  vnl_quaternion<double> rot(rot_axis, rot_step);
  vnl_double_3 t(0.0,0.0,0.0); //pitch
  // set up the rotation from the world coordinate system to 
  // the camera coordinate system
  vnl_double_3x3 R0(0.0);
  R0[0][0] = 1;
  R0[1][2] = -1;
  R0[2][1] = 1;
  vnl_quaternion<double> r0(R0);
  // initial position of camera in world coordinate system
  vnl_double_3 center(0, -header_.object_to_source_dist_, 0);
  // initial position of camera in camera coordinate system
  vnl_double_3 t0(-R0*center);
  // set orbit of scan
  xscan_orbit_base_sptr orbit = new xscan_uniform_orbit(rot, t, r0, t0);
  scan.set_orbit(orbit);
  //set number of views of scan
  scan.set_scan_size(header_.number_of_files_);
  //set scan file path
  scan.set_image_file_path("dummy");

  cached_scan_ = scan;
}

xscan_scan imgr_skyscan_log::get_scan() const
{
  return cached_scan_;
}

void imgr_skyscan_log::set_scan(xscan_scan scan)
{
  cached_scan_ = scan;
}

vcl_vector<vcl_string> imgr_skyscan_log::get_imagenames() 
{
  if(filenames_.size() == 0){
    parse_globbed_filenames(images_fname_, filenames_);
  }
  return filenames_;
}

vcl_vector<vil_image_resource_sptr> imgr_skyscan_log::get_images()  const
{
#if defined(VCL_WIN32)
  vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
  _setmaxstdio(2048);
#endif


  if(filenames_.size() == 0){
  parse_globbed_filenames(images_fname_, filenames_);
  }
  
  vcl_vector<vil_image_resource_sptr> images;
  images.clear();
  for(unsigned i=0; i < filenames_.size(); i++)
  {
    vil_image_resource_sptr img_sptr = vil_load_image_resource(filenames_[i].data());
    images.push_back(img_sptr);
  }

  return images;
}

//: XML write
void x_write(vcl_ostream& os, imgr_skyscan_log log)
{
  vsl_basic_xml_element element("imgr_skyscan_log");
  element.x_write_open(os);
  x_write(os, log.header(), "imgr_skyscan_log_header");
  vsl_basic_xml_element fname("images_fname");
  fname.append_cdata(log.images_fname());
  fname.x_write(os);
  element.x_write_close(os);

}
