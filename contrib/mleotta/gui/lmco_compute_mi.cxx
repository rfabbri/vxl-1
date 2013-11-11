// This is contrib/mleotta/gui/lmco_compute_mi.cxx
#include "lmco_compute_mi.h"
//:
// \file
// \author Matt Leotta

#include <vcl_cmath.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_bilin_interp.h>
#include <imesh/algo/imesh_project.h>
#include "lmco_manager.h"


mesh_mi_cost_func::mesh_mi_cost_func(const vil_image_view<float>& eo_data1,
                                     const vil_image_view<float>& ir_data1,
                                     const vil_image_view<float>& eo_data2,
                                     const vil_image_view<float>& ir_data2,
                                     const vpgl_perspective_camera<double>& cam1,
                                     const vpgl_perspective_camera<double>& cam2,
                                     const imesh_mesh& mesh,
                                           double grid_step,
                                           bool use_rotation)
: vnl_cost_function(use_rotation?3:2),
  eo_data1_(eo_data1),
  ir_data1_(ir_data1),
  eo_data2_(eo_data2),
  ir_data2_(ir_data2),
  cam1_(cam1),
  cam2_(cam2),
  mesh_(mesh),
  step_(grid_step),
  use_rotation_(use_rotation)
{
  if(!mesh_.faces().has_normals())
    mesh_.compute_face_normals();   
}


//: compte the cost as negative mutual information
double mesh_mi_cost_func::f(vnl_vector<double> const& x)
{
  vpgl_perspective_camera<double> cam1 = params_to_cam(x);
  
  unsigned int ni = eo_data1_.ni();
  unsigned int nj = eo_data1_.nj();
  
  vcl_vector<vgl_point_2d<double> > img_pts1, img_pts2;
  compute_point_mapping(cam1, cam2_, ni, nj, step_, 
                        mesh_, mesh_.faces().normals(), img_pts1, img_pts2);
  
  
  // sample IR and EO intensities
  vcl_vector<vcl_vector<double> > d1_ir(3), d2_ir(3), d1_eo(3), d2_eo(3);
  for(unsigned i=0; i<3; ++i){
    bilin_sample(img_pts1, ir_data1_, d1_ir[i], i);
    bilin_sample(img_pts1, eo_data1_, d1_eo[i], i);
    bilin_sample(img_pts2, ir_data2_, d2_ir[i], i);
    bilin_sample(img_pts2, eo_data2_, d2_eo[i], i);
  }
  

  return - lmco_manager::instance()->compute_mi(d1_ir, d2_ir, d1_eo, d2_eo);

}


//: convert parameters into update camera
vpgl_perspective_camera<double> 
mesh_mi_cost_func::params_to_cam(vnl_vector<double> const& x) const
{
  vgl_h_matrix_3d<double> h;
  h.set_identity();
  if(use_rotation_)
    h.set_rotation_euler(0,0,x[2]);
  h.set_translation(x[0], x[1], 0);
  return vpgl_perspective_camera<double>::postmultiply(cam1_,h);
}


//-----------------------------------------------------------------------------

mesh_joint_mi_cost_func::mesh_joint_mi_cost_func(const vil_image_view<float>& eo_data1,
                                                 const vil_image_view<float>& ir_data1,
                                                 const vil_image_view<float>& eo_data2,
                                                 const vil_image_view<float>& ir_data2,
                                                 const vpgl_perspective_camera<double>& cam1,
                                                 const vpgl_perspective_camera<double>& cam2,
                                                 const imesh_mesh& mesh,
                                                       double grid_step,
                                                       bool use_rotation)
: vnl_cost_function(use_rotation?6:4),
  eo_data1_(eo_data1),
  ir_data1_(ir_data1),
  eo_data2_(eo_data2),
  ir_data2_(ir_data2),
  cam1_(cam1),
  cam2_(cam2),
  mesh_(mesh),
  step_(grid_step),
  use_rotation_(use_rotation)
{
  if(!mesh_.faces().has_normals())
    mesh_.compute_face_normals();
}


//: compte the cost as negative mutual information
double mesh_joint_mi_cost_func::f(vnl_vector<double> const& x)
{
  vpgl_perspective_camera<double> cam1 = params_to_cam1(x);
  vpgl_perspective_camera<double> cam2 = params_to_cam2(x);
  
  unsigned int ni = eo_data1_.ni();
  unsigned int nj = eo_data1_.nj();
  
  vcl_vector<vgl_point_2d<double> > img_pts1, img_pts2;
  compute_point_mapping(cam1, cam2, ni, nj, step_, 
                        mesh_, normals_, img_pts1, img_pts2);

  
  // sample IR and EO intensities
  vcl_vector<vcl_vector<double> > d1_ir(3), d2_ir(3), d1_eo(3), d2_eo(3);
  for(unsigned i=0; i<3; ++i){
    bilin_sample(img_pts1, ir_data1_, d1_ir[i], i);
    bilin_sample(img_pts1, eo_data1_, d1_eo[i], i);
    bilin_sample(img_pts2, ir_data2_, d2_ir[i], i);
    bilin_sample(img_pts2, eo_data2_, d2_eo[i], i);
  }

  
  return - lmco_manager::instance()->compute_mi(d1_ir, d2_ir, d1_eo, d2_eo);

}


//: convert parameters into update camera 1
vpgl_perspective_camera<double> 
mesh_joint_mi_cost_func::params_to_cam1(vnl_vector<double> const& x) const
{
  vgl_h_matrix_3d<double> h;
  h.set_identity();
  if(use_rotation_)
    h.set_rotation_euler(0,0,x[2]);
  h.set_translation(x[0], x[1], 0);
  return vpgl_perspective_camera<double>::postmultiply(cam1_,h);
}

//: convert parameters into update camera 2
vpgl_perspective_camera<double> 
mesh_joint_mi_cost_func::params_to_cam2(vnl_vector<double> const& x) const
{
  vgl_h_matrix_3d<double> h;
  h.set_identity();
  if(use_rotation_){
    h.set_rotation_euler(0,0,x[5]);
    h.set_translation(x[3], x[4], 0);
  }
  else{
    h.set_translation(x[2], x[3], 0);
  }
  return vpgl_perspective_camera<double>::postmultiply(cam2_,h);
}


//-----------------------------------------------------------------------------

void compute_point_mapping(const vpgl_perspective_camera<double>& cam1, 
                           const vpgl_perspective_camera<double>& cam2,
                           unsigned int ni, unsigned int nj, double step,
                           const imesh_mesh& mesh,
                           const vcl_vector<vgl_vector_3d<double> >& normals,
                           vcl_vector<vgl_point_2d<double> >& img_pts1, 
                           vcl_vector<vgl_point_2d<double> >& img_pts2)
{  
  
  // find a bounding box of the mesh in img1
  vcl_vector<vgl_point_2d<double> > img_pts;
  imesh_project_verts(mesh.vertices<3>(), cam1, img_pts);
  vgl_box_2d<unsigned int> bbox(0, ni, 0, nj);
  imesh_projection_bounds(img_pts, bbox);
  
  // generate sample points on a grid
  vcl_vector< vgl_point_2d<double> > grid;
  for(double i=bbox.min_x(); i<= bbox.max_x(); i+=step)
    for(double j=bbox.min_y(); j<= bbox.max_y(); j+=step)
      grid.push_back(vgl_point_2d<double>(i,j));
  
  // backproject the grid points onto the mesh
  vcl_vector<unsigned int > idx_2d;
  vcl_vector<vgl_point_3d<double> > pts_3d;
  imesh_project_onto_mesh(mesh, normals, cam1,
                          grid, idx_2d, pts_3d);
  
  // retain only grid points on the mesh
  img_pts.clear();
  for(unsigned int i=0; i<idx_2d.size(); ++i)
    img_pts.push_back(grid[idx_2d[i]]);
  
  
  vcl_vector<vgl_point_2d<double> > new_img_grid;
  for(unsigned int i=0; i<pts_3d.size(); ++i)
  {
    new_img_grid.push_back(cam2(vgl_homg_point_3d<double>(pts_3d[i])));
  }
  
  //check for occuluded points
  vcl_vector<unsigned int > new_idx_2d;
  vcl_vector<vgl_point_3d<double> > new_pts_3d;
  imesh_project_onto_mesh(mesh, normals, cam2,
                          new_img_grid, new_idx_2d, new_pts_3d);
  assert(pts_3d.size() == new_pts_3d.size());
  
  img_pts1.clear();
  img_pts2.clear();
  for(unsigned int i=0; i<new_pts_3d.size(); ++i)
  {
    double d = (pts_3d[i] - new_pts_3d[i]).length();
    if( d < 1e-8){
      img_pts1.push_back(img_pts[i]);
      img_pts2.push_back(new_img_grid[i]);
    }
  }
  
}

//: compute mutual info by sampling the images at the specified points
double compute_mi_at_pts(const vil_image_resource_sptr& img1,
                         const vcl_vector<vgl_point_2d<double> >& pts1,
                         const vil_image_resource_sptr& img2,
                         const vcl_vector<vgl_point_2d<double> >& pts2,
                               unsigned int plane)
{
  vcl_vector<double> d1, d2;
  bilin_sample(pts1, img1, d1, plane);
  bilin_sample(pts2, img2, d2, plane);
  return mutual_info(d1,d2);
}




//: Sample the image by bilinear interpolation at the requested points
//  sampling is done in the desired plane, values are scaled into [0 1]
void bilin_sample(const vcl_vector<vgl_point_2d<double> >& pts,
                  const vil_image_resource_sptr& image,
                        vcl_vector<double>& data,
                        unsigned int plane)
{
  if (image->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vxl_byte> img = image->get_view();
    for(unsigned n=0; n<pts.size(); ++n){
      data.push_back(vil_bilin_interp_safe(pts[n].x(), pts[n].y(), 
                     img.top_left_ptr()+plane*img.planestep(), 
                     img.ni(), img.nj(), img.istep(), img.jstep())/255.0);
    }
  }
  if (image->pixel_format()==VIL_PIXEL_FORMAT_UINT_16)
  {
    vil_image_view<vxl_uint_16> img = image->get_view();
    for(unsigned n=0; n<pts.size(); ++n){
      data.push_back(vil_bilin_interp_safe (pts[n].x(), pts[n].y(), 
                     img.top_left_ptr()+plane*img.planestep(), 
                     img.ni(), img.nj(), img.istep(), img.jstep())/65535.0);
    }
  }
}


//: Sample the image by bilinear interpolation at the requested points
//  sampling is done in the desired plane
void bilin_sample(const vcl_vector<vgl_point_2d<double> >& pts,
                  const vil_image_view<float>& img,
                        vcl_vector<double>& data,
                        unsigned int plane)
{
  for(unsigned n=0; n<pts.size(); ++n){
    data.push_back(vil_bilin_interp_safe( pts[n].x(), pts[n].y(), 
                                          img.top_left_ptr()+plane*img.planestep(), 
                                          img.ni(), img.nj(), img.istep(), img.jstep()) );
  }
}


//: Compute the mutual information between vectors of corresponding data
//  it is assumed that the data are normalized to the range [0 1]
//  \param nbins is the number of bins used in the range [0 1]
double mutual_info(const vcl_vector<double>& d1, const vcl_vector<double>& d2, unsigned nbins)
{
  assert(d1.size() == d2.size());
  //joint histogram
  vcl_vector< vcl_vector< double > > jhist(nbins, vcl_vector< double >(nbins, 0.0));
  for(unsigned int n = 0; n<d1.size(); ++n){
    assert(d1[n] >= 0.0 && d1[n] <= 1.0);
    unsigned int i = static_cast<unsigned int>(vcl_floor(nbins*d1[n]));
    if(i == nbins) --i; 
    assert(d2[n] >= 0.0 && d2[n] <= 1.0);
    unsigned int j = static_cast<unsigned int>(vcl_floor(nbins*d2[n]));
    if(j == nbins) --j; 
    jhist[i][j] += 1.0/d1.size(); 
  }
  
  double H_ij = 0.0;
  for(unsigned int i = 0; i<nbins; ++i){
    for(unsigned int j = 0; j<nbins; ++j){
      double prob = jhist[i][j];
      H_ij += -(prob?prob*vcl_log(prob):0);
    }
  }
  
  double H_i = 0.0;
  for(unsigned int i = 0; i<nbins; ++i){
    double prob = 0.0;
    for(unsigned int j = 0; j<nbins; ++j){
      prob += jhist[i][j];
    }
    H_i += -(prob?prob*vcl_log(prob):0);
  }
  
  double H_j = 0.0;
  for(unsigned int j = 0; j<nbins; ++j){
    double prob = 0.0;
    for(unsigned int i = 0; i<nbins; ++i){
      prob += jhist[i][j];
    }
    H_j += -(prob?prob*vcl_log(prob):0);
  }
  
  return (H_i + H_j - H_ij)/0.69314718056;
}


//: Compute the mutual information between vectors of corresponding data with weights
//  it is assumed that the data are normalized to the range [0 1]
//  \param nbins is the number of bins used in the range [0 1]
double mutual_info_weighted(const vcl_vector<double>& d1, const vcl_vector<double>& d2, 
                            const vcl_vector<double>& w1, const vcl_vector<double>& w2, 
                            unsigned nbins)
{
  assert(d1.size() == d2.size());
  assert(w1.size() == w2.size());
  assert(d1.size() == w1.size());
  
  //joint histogram
  vcl_vector< vcl_vector< double > > jhist(nbins, vcl_vector< double >(nbins, 0.0));
  double total_weight = 0.0;
  for(unsigned int n = 0; n<d1.size(); ++n){
    assert(d1[n] >= 0.0 && d1[n] <= 1.0 );
    unsigned int i = static_cast<unsigned int>(vcl_floor(nbins*d1[n]));
    if(i == nbins) --i; 
    assert(d2[n] >= 0.0 && d2[n] <= 1.0);
    unsigned int j = static_cast<unsigned int>(vcl_floor(nbins*d2[n]));
    if(j == nbins) --j; 
    double w = w1[n]*w2[n]; 
    jhist[i][j] += w;
    total_weight += w;
  }
  
  assert(total_weight > 0.0);
  
  double H_ij = 0.0;
  for(unsigned int i = 0; i<nbins; ++i){
    for(unsigned int j = 0; j<nbins; ++j){
      jhist[i][j] /= total_weight;
      double prob = jhist[i][j];
      H_ij += -(prob?prob*vcl_log(prob):0);
    }
  }
  
  double H_i = 0.0;
  for(unsigned int i = 0; i<nbins; ++i){
    double prob = 0.0;
    for(unsigned int j = 0; j<nbins; ++j){
      prob += jhist[i][j];
    }
    H_i += -(prob?prob*vcl_log(prob):0);
  }
  
  double H_j = 0.0;
  for(unsigned int j = 0; j<nbins; ++j){
    double prob = 0.0;
    for(unsigned int i = 0; i<nbins; ++i){
      prob += jhist[i][j];
    }
    H_j += -(prob?prob*vcl_log(prob):0);
  }
  
  return (H_i + H_j - H_ij)/0.69314718056;
}
