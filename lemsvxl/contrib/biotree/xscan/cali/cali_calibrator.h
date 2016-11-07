#ifndef cali_calibrator_h
#define cali_calibrator_h

#include <vcl_string.h>
#include <xscan/xscan_scan.h>
#include <vil/vil_image_resource_sptr.h>
#include <cali/cali_param.h>

class cali_calibrator
{
public:
    cali_param par_;

    cali_calibrator(cali_param par);

    cali_calibrator(vcl_string scan_log_file, 
                       vcl_string conics_bin_file_base, 
                       int artf_imgs_num,
                       int interval);
    cali_calibrator(vcl_string scan_log_file, 
                       vcl_string conics_bin_file_base, 
                       int artf_imgs_num,cali_param par,
                       int interval);

  ~cali_calibrator(void)
  {
  }

  void calibrate(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr);

  // calibration done by taking into account the tolerance associated with the CMM coordinates
   void calibrate_with_tolerance(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,vcl_vector<double> x_coord_tol,vcl_vector<double> y_coord_tol,vcl_vector<double> z_coord_tol,vcl_vector<double> rad_tol);
  
// calibration done in two stages (also taking the tolerance into account)  
   void calibrate_in_two_stages(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,vcl_vector<double> x_coord_tol,vcl_vector<double> y_coord_tol,vcl_vector<double> z_coord_tol,vcl_vector<double> rad_tol);

    void calibrate_user_interaction(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,vcl_vector<double> x_coord_tol,vcl_vector<double> y_coord_tol,vcl_vector<double> z_coord_tol,vcl_vector<double> rad_tol);
  
  
protected:
  
  xscan_scan scan_;
  vcl_vector<vil_image_resource_sptr> resources_;
  int scan_interval_;
  vcl_string scan_log_file_;
  vcl_string conics_bin_file_base_; 
  int artf_imgs_num_;

  void create_scan(vcl_string fname);
};

#endif
