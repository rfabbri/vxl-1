// This is /contrib/biotree/xscan/xscan_scan.h

#ifndef XSCAN_SCAN_H_
#define XSCAN_SCAN_H_

//:
// \file   xscan_scan.h
// \brief  X-Ray Scan based on J. Mundy's dummy scan model.
// \author Kongbin Kang and H. Can Aras 
// \date   2005-02-27
// \verbatim
//  Modifications
//    pradeep  12/15/04  added a const method which returns an xmvg_perspective_camera
// \endverbatim
//

#include <vcl_string.h>
#include <xscan/xscan_orbit_base.h>
#include <xmvg/xmvg_source.h>
#include <xmvg/xmvg_perspective_camera.h>
#include <xscan/xscan_orbit_base_sptr.h>
#include <vcl_iostream.h>

class xscan_scan
{
  // member functions
public:
  //: return the source
  xmvg_source source() const;

  //: return the image file path
  vcl_string image_file_path() const {return image_file_path_;}

  //: set image path
  void set_image_file_path(char* path) {image_file_path_ = path;}

  //: scan size
  unsigned scan_size() const {return n_views_;}

  //: set scan size
  void set_scan_size(unsigned int n) { n_views_ = n;}

  //: set source
  void set_source(xmvg_source s) { source_ = s;}

  //: set calibration matrix
  void set_calibration_matrix(vpgl_calibration_matrix<double> k) { kk_ = k;}

  //: set orbit base smart pointer
  void set_orbit(xscan_orbit_base_sptr orbit) { orbit_ = orbit;}

  static void correct_uniform_orbit(xscan_scan& scan, const double& rotation_step, const int& n_views);
 
  //: return a global 
  xmvg_perspective_camera<double> operator()(const orbit_index t);

  //: const method which returns an xmvg_perspective_camera
  xmvg_perspective_camera<double> operator() (const orbit_index t) const;

  //: empty constructor
  xscan_scan(){}
 
  //: constructor
  xscan_scan(unsigned nviews, 
             const vcl_string image_file_path,
             const xmvg_source& source, 
             const vpgl_calibration_matrix<double>& k,
             xscan_orbit_base_sptr orbit );

  ~xscan_scan(){};

  //: get number of views
  unsigned int n_views() { return n_views_; }
  //: get image file path
  vcl_string image_file_path() { return image_file_path_; }
  //: get calibration matrix
  vpgl_calibration_matrix<double> kk() { return kk_; }
  //: get orbit
  xscan_orbit_base_sptr orbit() { return orbit_; }

protected:

  //: total number of views
  unsigned int n_views_;

  //: image path
  vcl_string image_file_path_;

  //: x-ray source
  xmvg_source source_;

  //: internal parameter matrix of the camera
  vpgl_calibration_matrix<double> kk_;

  //: a pointer to the orbit
  xscan_orbit_base_sptr orbit_;

  friend vcl_ostream& operator << (vcl_ostream& stream, const xscan_scan& b);

  //: in order to dynamically create orbit object, whenever a new orbit class added,
  // a switch statment should be added into the implementation of this stream function
  friend vcl_istream& operator >> (vcl_istream& stream, xscan_scan & scan);
};

void x_write(vcl_ostream& stream, xscan_scan b);

#endif
