#ifndef cali_artf_projector_h
#define cali_artf_projector_h

#include <xmvg/xmvg_perspective_camera.h>
#include <dbil/dbil_bounded_image_view.h>
#include <vil/vil_image_resource_sptr.h>
#include <cali/cali_artf_model.h>
#include <vgl/vgl_sphere_3d.h>
#include <vsol/vsol_conic_2d.h>
#include <cali/cali_param.h>

class cali_artf_projector:public cali_param
{
private:
  cali_artf_model* artifact_;
  vcl_ofstream fstream;

protected:
   
  void project_sphere(xmvg_perspective_camera<double> camera, 
                                    vgl_homg_point_3d<double> center,
                                    double radius,
                                    double &a, double &b);

   vgl_point_2d<double> project_sphere_image(xmvg_perspective_camera<double> camera, 
                                    vgl_homg_point_3d<double> center,
                                    double radius,
                                    double &a, double &b);

public:
     cali_param par_;
  cali_artf_projector(cali_artf_model* artifact);
  cali_artf_projector(cali_artf_model* artifact,cali_param par);
  ~cali_artf_projector(void){};
  vil_image_resource_sptr build_projection_img(xmvg_perspective_camera<double> camera, dbil_bounded_image_view<unsigned char> & view);
  vil_image_resource_sptr build_centers_projection_img(xmvg_perspective_camera<double> camera, 
                                         dbil_bounded_image_view<unsigned char> & view);
  vcl_vector<vsol_conic_2d> build_ball_projections(xmvg_perspective_camera<double> camera);
};
#endif
