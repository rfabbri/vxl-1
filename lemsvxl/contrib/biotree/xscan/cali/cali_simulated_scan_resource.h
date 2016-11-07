#ifndef cali_simulated_scan_resource_h
#define cali_simulated_scan_resource_h

#include <cali/cali_param.h>
#include <imgr/imgr_scan_resource.h>
#include <cali/cali_cylinder_artifact.h>
#include <cali/cali_artf_projector.h>
#include <imgr/imgr_image_view_3d_base_sptr.h>
#include <imgr/imgr_bounded_image_view_3d.h>
#include <xscan/xscan_scan.h>
#include <vcl_vector.h>

class cali_simulated_scan_resource :public imgr_scan_resource
{
protected:
    cali_cylinder_artifact* artifact_;
    cali_artf_projector *projector_;
   
    
public:
     cali_param par_;
  
     virtual ~cali_simulated_scan_resource(){
       delete artifact_;
       delete projector_;
     }
cali_simulated_scan_resource(xscan_scan const& scan, 
        vcl_vector<vil_image_resource_sptr> const& resources);
cali_simulated_scan_resource(xscan_scan const& scan, 
        vcl_vector<vil_image_resource_sptr> const& resources,cali_param par);
cali_simulated_scan_resource(xscan_scan const& scan, 
        vcl_vector<vil_image_resource_sptr> const& resources,vnl_quaternion<double>const & rot,vgl_point_3d<double> trans);
cali_simulated_scan_resource(xscan_scan const& scan, 
        vcl_vector<vil_image_resource_sptr> const& resources,cali_param par,vcl_vector<double> x_coord_tol,vcl_vector<double> y_coord_tol,
        vcl_vector<double> z_coord_tol,vcl_vector<double> rad_tol);
        
    vcl_vector<vil_image_resource_sptr>  simulate_scan(char* folder_name, char* file_name,
      vnl_quaternion<double> artf_rot, 
      vgl_point_3d<double> artf_trans,
      int interval);

     vcl_vector<vil_image_resource_sptr> gen_artifact_views(imgr_bounded_image_view_3d<unsigned char> views, 
                                                    vgl_box_3d<double> const& bounds,
                                                    char* folder_name, char* file_name,
                                                    int interval);
      vcl_vector<vil_image_resource_sptr> gen_artifact_center_views(imgr_bounded_image_view_3d<unsigned char> views, 
                                                    vgl_box_3d<double> const& bounds,
                                                    char* folder_name, char* file_name,
                                                    int interval);
  
     vcl_vector<vcl_vector<vsol_conic_2d> > gen_ball_projections(vnl_quaternion<double> artf_rot, 
                                                    vgl_point_3d<double> artf_trans,
                                                    int interval);

     


  static vcl_string gen_file_name(char* folder, char *file, int num);
private:

};

#endif
