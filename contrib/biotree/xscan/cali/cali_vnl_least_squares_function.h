#ifndef cali_vnl_least_squares_function_h
#define cali_vnl_least_squares_function_h

#include <vnl/vnl_least_squares_function.h>
#include <xscan/xscan_scan.h>
#include <vcl_string.h>
#include <vil/vil_image_resource_sptr.h>
#include <cali/cali_artf_corresponder.h>
#include <cali/cali_simulated_scan_resource.h>
#include<cali/cali_param.h>

class cali_vnl_least_squares_function: public vnl_least_squares_function

{
public:
    cali_param par_;
  cali_vnl_least_squares_function(vcl_vector<vil_image_resource_sptr> scan_resources, 
    xscan_scan scan, 
    vcl_string file_base, 
    int interval, 
     int num_of_images);

  cali_vnl_least_squares_function(cali_param par,vcl_vector<vil_image_resource_sptr> scan_resources, 
    xscan_scan scan, 
    vcl_string file_base, 
    int interval, 
     int num_of_images);


~cali_vnl_least_squares_function(void);


  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  static bool y_coord_compare(vsol_conic_2d_sptr x,vsol_conic_2d_sptr y);

    void set_lsqr_fn_params(vnl_vector<double> &x, 
                          vnl_quaternion<double> const& artf_rot, 
                          vgl_point_3d<double> const& artf_trans,
                          xscan_scan& scan);

  void set_lsqr_fn_params_with_tolerance(vnl_vector<double> &x, 
                          vnl_quaternion<double> const& artf_rot, 
                          vgl_point_3d<double> const& artf_trans,
                          xscan_scan& scan,vcl_vector<double>& x_coord_tol,vcl_vector<double>& y_coord_tol,
                          vcl_vector<double>& z_coord_tol,vcl_vector<double>& rad_tol);

    static void gen_scan_lsqr_fn_params(vnl_vector<double> const& x, 
                      vnl_quaternion<double> &artf_rot, 
                      vgl_point_3d<double> &artf_trans,
                      vnl_quaternion<double> &turn_table_rot,
                      vnl_double_3 &turn_table_trans,
                      double &x_scale,
                      double &y_scale,
                      vgl_point_2d<double> &princp_point);

 void lsqr_fn_params(vnl_vector<double> const& x, 
                      vnl_quaternion<double> &artf_rot, 
                      vgl_point_3d<double> &artf_trans,
                      vnl_quaternion<double> &turn_table_rot,
                      vnl_double_3 &turn_table_trans,
                      double &x_scale,
                      double &y_scale,
                      vgl_point_2d<double> &princp_point);

 void lsqr_fn_params_with_tolerance(vnl_vector<double> const& x, 
                      vnl_quaternion<double> &artf_rot, 
                      vgl_point_3d<double> &artf_trans,
                      vnl_quaternion<double> &turn_table_rot,
                      vnl_double_3 &turn_table_trans,
                      double &x_scale,
                      double &y_scale,
                      vgl_point_2d<double> &princp_point,
                      vcl_vector<double>& x_coord_tol,vcl_vector<double>& y_coord_tol,
                          vcl_vector<double>& z_coord_tol,vcl_vector<double>& rad_tol );
protected:

  
  cali_artf_corresponder corresp;
  vcl_vector<vil_image_resource_sptr> scan_res_;
  xscan_scan scan_;
  
  int interval_;
  int num_of_images_;
  vcl_vector<conic_vector_set> real_img_conics;

};

#endif

