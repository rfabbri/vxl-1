// This is contrib/mleotta/gui/lmco_compute_mi.h
#ifndef lmco_compute_mi_h_
#define lmco_compute_mi_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Functions to compute mutual information
// \author
//   Matt Leotta
//
// \verbatim
//  Modifications:
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>

#include <vnl/vnl_cost_function.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <imesh/imesh_mesh.h>


class mesh_mi_cost_func : public vnl_cost_function
{
public:
  mesh_mi_cost_func(const vil_image_view<float>& eo_data1,
                    const vil_image_view<float>& ir_data1,
                    const vil_image_view<float>& eo_data2,
                    const vil_image_view<float>& ir_data2,
                    const vpgl_perspective_camera<double>& cam1,
                    const vpgl_perspective_camera<double>& cam2,
                    const imesh_mesh& mesh,
                          double grid_step,
                          bool use_rotation = true);
  
  //: compte the cost as negative mutual information
  virtual double f(vnl_vector<double> const& x);
  
  //: convert parameters into update camera
  vpgl_perspective_camera<double> params_to_cam(vnl_vector<double> const& x) const;
  
private:
  vil_image_view<float> eo_data1_, ir_data1_;
  vil_image_view<float> eo_data2_, ir_data2_;
  vpgl_perspective_camera<double> cam1_;
  vpgl_perspective_camera<double> cam2_;
  imesh_mesh mesh_;
  double step_;
  bool use_rotation_;
};


class mesh_joint_mi_cost_func : public vnl_cost_function
{
public:
  mesh_joint_mi_cost_func(const vil_image_view<float>& eo_data1,
                          const vil_image_view<float>& ir_data1,
                          const vil_image_view<float>& eo_data2,
                          const vil_image_view<float>& ir_data2,
                          const vpgl_perspective_camera<double>& cam1,
                          const vpgl_perspective_camera<double>& cam2,
                          const imesh_mesh& mesh,
                          double grid_step,
                          bool use_rotation = true);
  
  //: compte the cost as negative mutual information
  virtual double f(vnl_vector<double> const& x);
  
  //: convert parameters into update camera 1
  vpgl_perspective_camera<double> params_to_cam1(vnl_vector<double> const& x) const;
  //: convert parameters into update camera 2
  vpgl_perspective_camera<double> params_to_cam2(vnl_vector<double> const& x) const;
  
private:
  vil_image_view<float> eo_data1_, ir_data1_;
  vil_image_view<float> eo_data2_, ir_data2_;
  vpgl_perspective_camera<double> cam1_;
  vpgl_perspective_camera<double> cam2_;
  imesh_mesh mesh_;
  vcl_vector<vgl_vector_3d<double> > normals_;
  double step_;
  bool use_rotation_;
};

//: Compute the mapping of a grid of point in view 1 to view 2 using
// the mesh model for transfer and removing self occluded points
void compute_point_mapping(const vpgl_perspective_camera<double>& cam1, 
                           const vpgl_perspective_camera<double>& cam2,
                           unsigned int ni, unsigned int nj, double step,
                           const imesh_mesh& mesh,
                           const vcl_vector<vgl_vector_3d<double> >& normals,
                           vcl_vector<vgl_point_2d<double> >& img_pts1, 
                           vcl_vector<vgl_point_2d<double> >& img_pts2);

//: compute mutual info by sampling the images at the specified points
double compute_mi_at_pts(const vil_image_resource_sptr& img1,
                         const vcl_vector<vgl_point_2d<double> >& pts1,
                         const vil_image_resource_sptr& img2,
                         const vcl_vector<vgl_point_2d<double> >& pts2,
                               unsigned int plane);


//: Sample the image by bilinear interpolation at the requested points
//  sampling is done in the desired plane, values are scaled into [0 1]
void bilin_sample(const vcl_vector<vgl_point_2d<double> >& pts,
                  const vil_image_resource_sptr& img,
                        vcl_vector<double>& data,
                        unsigned int plane);

//: Sample the image by bilinear interpolation at the requested points
//  sampling is done in the desired plane
void bilin_sample(const vcl_vector<vgl_point_2d<double> >& pts,
                  const vil_image_view<float>& img,
                        vcl_vector<double>& data,
                        unsigned int plane);


//: Compute the mutual information between vectors of corresponding data
//  it is assumed that the data are normalized to the range [0 1]
//  \param nbins is the number of bins used in the range [0 1]
double mutual_info(const vcl_vector<double>& d1, const vcl_vector<double>& d2, 
                   unsigned nbins=16);

//: Compute the mutual information between vectors of corresponding data with weights
//  it is assumed that the data are normalized to the range [0 1]
//  \param nbins is the number of bins used in the range [0 1]
double mutual_info_weighted(const vcl_vector<double>& d1, const vcl_vector<double>& d2, 
                            const vcl_vector<double>& w1, const vcl_vector<double>& w2, 
                            unsigned nbins=16);



#endif // lmco_compute_mi_h_
