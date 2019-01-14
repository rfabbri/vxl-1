#ifndef cali_calibrator_h
#define cali_calibrator_h

#include <string>
#include <xscan/xscan_scan.h>
#include <vil/vil_image_resource_sptr.h>
#include <cali/cali_param.h>

class cali_calibrator
{
public:
    cali_param par_;

    cali_calibrator(cali_param par);

    cali_calibrator(std::string scan_log_file, 
                       std::string conics_bin_file_base, 
                       int artf_imgs_num,
                       int interval);
    cali_calibrator(std::string scan_log_file, 
                       std::string conics_bin_file_base, 
                       int artf_imgs_num,cali_param par,
                       int interval);

  ~cali_calibrator(void)
  {
  }

  void calibrate(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr);

  // calibration done by taking into account the tolerance associated with the CMM coordinates
   void calibrate_with_tolerance(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,std::vector<double> x_coord_tol,std::vector<double> y_coord_tol,std::vector<double> z_coord_tol,std::vector<double> rad_tol);
  
// calibration done in two stages (also taking the tolerance into account)  
   void calibrate_in_two_stages(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,std::vector<double> x_coord_tol,std::vector<double> y_coord_tol,std::vector<double> z_coord_tol,std::vector<double> rad_tol);

    void calibrate_user_interaction(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,std::vector<double> x_coord_tol,std::vector<double> y_coord_tol,std::vector<double> z_coord_tol,std::vector<double> rad_tol);
  
  
protected:
  
  xscan_scan scan_;
  std::vector<vil_image_resource_sptr> resources_;
  int scan_interval_;
  std::string scan_log_file_;
  std::string conics_bin_file_base_; 
  int artf_imgs_num_;

  void create_scan(std::string fname);
};

#endif
