#include <vbl/vbl_bounding_box.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_triangle_scan_iterator.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_plane_3d.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <vil/vil_image_view.h>

#include "psm_raytrace_operations.h"


bool visible(double *xverts_2d, double *yverts_2d, unsigned int v0, unsigned int v1, unsigned int v2)
{
  double v0x = xverts_2d[v1] - xverts_2d[v0];
  double v0y = yverts_2d[v1] - yverts_2d[v0];
  double v1x = xverts_2d[v2] - xverts_2d[v1];
  double v1y = yverts_2d[v2] - yverts_2d[v1];
  double cross_prod = v0x*v1y - v1x*v0y;
  return (cross_prod < 0.0);
}

void project_cube_vertices(vbl_bounding_box<double,3> const &cube, vpgl_perspective_camera<double> const& cam, double* xverts_2d, double* yverts_2d, float* vert_distances, psm_cube_face_list &visible_faces)
{
  // 3d coordinates of cube
  double xverts_3d[] = {cube.xmin(), cube.xmax()};
  double yverts_3d[] = {cube.ymin(), cube.ymax()};
  double zverts_3d[] = {cube.zmin(), cube.zmax()};

  vgl_point_3d<double> cam_center = vgl_point_3d<double>(cam.camera_center());

  unsigned int n=0;
  for (unsigned int k=0; k<2; ++k) {
    for (unsigned int j=0; j<2; ++j) {
      for (unsigned int i=0; i<2; ++i) {
        cam.project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
        vert_distances[n] =  (float)(cam_center - vgl_point_3d<double>(xverts_3d[i],yverts_3d[j],zverts_3d[k])).length();
        ++n;
      }
    }
  }

  visible_faces = psm_cube_face::NONE;
  if (visible(xverts_2d,yverts_2d, 2,0,6)) {
    visible_faces |= psm_cube_face::X_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 1,3,7)) {
    visible_faces |= psm_cube_face::X_HIGH;
  }
  if (visible(xverts_2d,yverts_2d, 0,1,5)) {
    visible_faces |= psm_cube_face::Y_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 7,3,2)) {
    visible_faces |= psm_cube_face::Y_HIGH;
  }
  if (visible(xverts_2d,yverts_2d, 0,2,1)) {
    visible_faces |= psm_cube_face::Z_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 4,5,6)) {
    visible_faces |= psm_cube_face::Z_HIGH;
  }
}

void project_cube_vertices(vbl_bounding_box<double,3> const &cube, vpgl_camera<double> const* cam, double* xverts_2d, double* yverts_2d, float* vert_distances, psm_cube_face_list &visible_faces, double bigZ)
{
  // 3d coordinates of cube
  double xverts_3d[] = {cube.xmin(), cube.xmax()};
  double yverts_3d[] = {cube.ymin(), cube.ymax()};
  double zverts_3d[] = {cube.zmin(), cube.zmax()};

  if (vpgl_perspective_camera<double> const* pcam = dynamic_cast<vpgl_perspective_camera<double> const*>(cam)) {
    vgl_point_3d<double> cam_center = vgl_point_3d<double>(pcam->camera_center());
    unsigned int n=0;
    for (unsigned int k=0; k<2; ++k) {
      for (unsigned int j=0; j<2; ++j) {
        for (unsigned int i=0; i<2; ++i) {
          pcam->project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
          vert_distances[n] =  (float)(cam_center - vgl_point_3d<double>(xverts_3d[i],yverts_3d[j],zverts_3d[k])).length();
          ++n;
        }
      }
    }
  } 
  else if (vpgl_rational_camera<double> const* rcam = dynamic_cast<vpgl_rational_camera<double> const*>(cam)) {
    unsigned int n=0;
    for (unsigned int k=0; k<2; ++k) {
      for (unsigned int j=0; j<2; ++j) {
        for (unsigned int i=0; i<2; ++i) {
          rcam->project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
          // find 3-d point along this ray where it intersects a plane high in space
          vgl_point_3d<double> ray_origin(0.0,0.0,bigZ);
          vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(xverts_2d[n],yverts_2d[n]),vgl_plane_3d<double>(0.0,0.0,1.0,-bigZ),ray_origin,ray_origin);
          vert_distances[n] =  (float)(ray_origin - vgl_point_3d<double>(xverts_3d[i],yverts_3d[j],zverts_3d[k])).length();
          ++n;
        }
      }
    }
  } 
  else {
    vcl_cerr << "ERROR: unsupported camera type " << vcl_endl;
    return;
  }

  visible_faces = psm_cube_face::NONE;
  if (visible(xverts_2d,yverts_2d, 2,0,6)) {
    visible_faces |= psm_cube_face::X_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 1,3,7)) {
    visible_faces |= psm_cube_face::X_HIGH;
  }
  if (visible(xverts_2d,yverts_2d, 0,1,5)) {
    visible_faces |= psm_cube_face::Y_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 7,3,2)) {
    visible_faces |= psm_cube_face::Y_HIGH;
  }
  if (visible(xverts_2d,yverts_2d, 0,2,1)) {
    visible_faces |= psm_cube_face::Z_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 4,5,6)) {
    visible_faces |= psm_cube_face::Z_HIGH;
  }
}


bool cube_camera_distance(vbl_bounding_box<double,3> const& cube, vpgl_perspective_camera<double> const& cam, vil_image_view<float> &front_dist, vil_image_view<float> &back_dist, psm_cube_face_list visible_faces)
{
  // 3d coordinates of cube
  double xverts_3d[] = {cube.xmin(), cube.xmax()};
  double yverts_3d[] = {cube.ymin(), cube.ymax()};
  double zverts_3d[] = {cube.zmin(), cube.zmax()};
  // 2d projected coordinates of cube
  double xverts_2d[8], yverts_2d[8];
  // distances from camera center of 3d vertices
  float vert_distances[8];

  vgl_point_3d<double> cam_center = vgl_point_3d<double>(cam.camera_center());

  unsigned int n=0;
  for (unsigned int k=0; k<2; ++k) {
    for (unsigned int j=0; j<2; ++j) {
      for (unsigned int i=0; i<2; ++i) {
        cam.project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
        vert_distances[n] =  (float)(cam_center - vgl_point_3d<double>(xverts_3d[i],yverts_3d[j],zverts_3d[k])).length();
        ++n;
      }
    }
  }
  // for each face, create two triangle iterators and fill in pixel data
  vgl_triangle_scan_iterator<double> tri_it;
  vgl_point_2d<double> tri_verts[3];

  // X_LOW
  // tri 0
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 2, 0, 6);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 2, 0, 6);

  // tri 1
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 4, 6);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 0, 4, 6);

  // X_HIGH
  // tri 0
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 1, 3, 7);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 1, 3, 7);
  // tri 1
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 7, 5, 1);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 7, 5, 1);

  // Y_LOW
  // tri 0
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 1, 5);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 0, 1, 5);
  // tri 1
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 4, 0, 5);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 4, 0, 5);

  // Y_HIGH
  // tri 0
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 7, 3, 2);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 7, 3, 2);
  // tri 1
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 2, 6, 7);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 2, 6, 7);

  // Z_LOW
  // tri 0
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 1, 2);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 0, 2, 1);
  // tri 1
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 1, 3, 2);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 1, 2, 3);

  // Z_HIGH
  // tri 0
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 4, 5, 6);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 4, 5, 6);
  // tri 1
  init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 5, 7, 6);
  tri_interpolate_min_max(tri_it, xverts_2d, yverts_2d, vert_distances, front_dist, back_dist, 5, 7, 6);

  return true;

}



bool alpha_seg_len_aa(double *xverts_2d, double* yverts_2d, float* vert_distances, psm_cube_face_list visible_faces, float alpha, vil_image_view<float> &alpha_distance)
{
  // multiply each vertex distance by alpha
  float vert_alpha_distances[8];
  float *vert_dist_ptr = vert_distances;
  float *vert_alpha_dist_ptr = vert_alpha_distances;
  for (unsigned int i=0; i<8; ++i) {
    *vert_alpha_dist_ptr++ = *vert_dist_ptr++ * alpha;
  }

  // for each face, create two triangle iterators and fill in pixel data
  // X_LOW
  // tri 0
  psm_triangle_interpolation_iterator_aa<float> tri_it(xverts_2d, yverts_2d, vert_alpha_distances, 2, 0, 6);
  if (visible_faces & psm_cube_face::X_LOW) 
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 4, 6);
  if (visible_faces & psm_cube_face::X_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);


  // X_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 1, 3, 7);
  if (visible_faces & psm_cube_face::X_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 7, 5, 1);
  if (visible_faces & psm_cube_face::X_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);


  // Y_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 1, 5);
  if (visible_faces & psm_cube_face::Y_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 4, 0, 5);
  if (visible_faces & psm_cube_face::Y_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Y_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 7, 3, 2);
  if (visible_faces & psm_cube_face::Y_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 2, 6, 7);
  if (visible_faces & psm_cube_face::Y_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Z_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 2, 1);
  if (visible_faces & psm_cube_face::Z_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 1, 2, 3);
  if (visible_faces & psm_cube_face::Z_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Z_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 4, 5, 6);
  if (visible_faces & psm_cube_face::Z_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false); // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_alpha_distances, 5, 7, 6);
  if (visible_faces & psm_cube_face::Z_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  return true;
}


bool alpha_seg_len(double *xverts_2d, double* yverts_2d, float* vert_distances, psm_cube_face_list visible_faces, float alpha, vil_image_view<float> &alpha_distance)
{
  // multiply each vertex distance by alpha
  float vert_alpha_distances[8];
  float *vert_dist_ptr = vert_distances;
  float *vert_alpha_dist_ptr = vert_alpha_distances;
  for (unsigned int i=0; i<8; ++i) {
    *vert_alpha_dist_ptr++ = *vert_dist_ptr++ * alpha;
  }

  // for each face, create two triangle iterators and fill in pixel data
  // X_LOW
  // tri 0
  psm_triangle_interpolation_iterator<float> tri_it(xverts_2d, yverts_2d, vert_alpha_distances, 2, 0, 6);
  if (visible_faces & psm_cube_face::X_LOW) 
    tri_interpolate_values(tri_it, alpha_distance, true);
  else
    tri_interpolate_values(tri_it, alpha_distance, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 4, 6);
  if (visible_faces & psm_cube_face::X_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);


  // X_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 1, 3, 7);
  if (visible_faces & psm_cube_face::X_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 7, 5, 1);
  if (visible_faces & psm_cube_face::X_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);


  // Y_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 1, 5);
  if (visible_faces & psm_cube_face::Y_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 4, 0, 5);
  if (visible_faces & psm_cube_face::Y_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Y_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 7, 3, 2);
  if (visible_faces & psm_cube_face::Y_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);  
  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 2, 6, 7);
  if (visible_faces & psm_cube_face::Y_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Z_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 0, 2, 1);
  if (visible_faces & psm_cube_face::Z_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);  
  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 1, 2, 3);
  if (visible_faces & psm_cube_face::Z_LOW)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  // Z_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 4, 5, 6);
  if (visible_faces & psm_cube_face::Z_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false); 
  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_alpha_distances, 5, 7, 6);
  if (visible_faces & psm_cube_face::Z_HIGH)
    tri_interpolate_values(tri_it, alpha_distance, true);
  else 
    tri_interpolate_values(tri_it, alpha_distance, false);

  return true;
}


bool cube_exit_point(vbl_bounding_box<double,3> const& cube, vgl_point_3d<double> const& pt, vgl_vector_3d<double> const& direction, 
                     vgl_point_3d<double> &exit_pt, unsigned int &step_dim, bool &step_positive)
{
  // check each face
  if (direction.x() < 0) {
    // intersect with low x plane
    double lambda = (cube.xmin() - pt.x()) / direction.x();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.y() >= cube.ymin()) && (exit_pt.y() <= cube.ymax()) && 
      (exit_pt.z() >= cube.zmin()) && (exit_pt.z() <= cube.zmax()) ) {
        step_dim = 0;
        step_positive = false;
        return true;
    }
  }
  else {
    // intersect with high x plane
    double lambda = (cube.xmax() - pt.x()) / direction.x();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.y() >= cube.ymin()) && (exit_pt.y() <= cube.ymax()) && 
      (exit_pt.z() >= cube.zmin()) && (exit_pt.z() <= cube.zmax()) ) {
        step_dim = 0;
        step_positive = true;
        return true;
    }
  }
  if (direction.y() < 0) {
    // intersect with low y plane
    double lambda = (cube.ymin() - pt.y()) / direction.y();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.x() >= cube.xmin()) && (exit_pt.x() <= cube.xmax()) && 
      (exit_pt.z() >= cube.zmin()) && (exit_pt.z() <= cube.zmax()) ) {
        step_dim = 1;
        step_positive = false;
        return true;
    }
  }
  else {
    // intersect with high y plane
    double lambda = (cube.ymax() - pt.y()) / direction.y();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.x() >= cube.xmin()) && (exit_pt.x() <= cube.xmax()) && 
      (exit_pt.z() >= cube.zmin()) && (exit_pt.z() <= cube.zmax()) ) {
        step_dim = 1;
        step_positive = true;
        return true;
    }
  }
  if (direction.z() < 0) {
    // intersect with low z plane
    double lambda = (cube.zmin() - pt.z()) / direction.z();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.x() >= cube.xmin()) && (exit_pt.x() <= cube.xmax()) && 
      (exit_pt.y() >= cube.ymin()) && (exit_pt.y() <= cube.ymax()) ) {
        step_dim = 2;
        step_positive = false;
        return true;
    }
  }
  else {
    // intersect with high z plane
    double lambda = (cube.zmax() - pt.z()) / direction.z();
    exit_pt = pt + direction * lambda;
    if ( (exit_pt.x() >= cube.xmin()) && (exit_pt.x() <= cube.xmax()) && 
      (exit_pt.y() >= cube.ymin()) && (exit_pt.y() <= cube.ymax()) ) {
        step_dim = 2;
        step_positive = true;
        return true;
    }
  }
  return false;
}

bool full_cube_visible(const double* xverts_2d, const double* yverts_2d, unsigned int img_ni, unsigned int img_nj)
{
  const double* xvert_ptr = xverts_2d;
  const double* yvert_ptr = yverts_2d;
  for (unsigned int i=0; i<8; ++i, ++xvert_ptr, ++yvert_ptr) {
    if ( (*xvert_ptr < 0) || (*xvert_ptr > img_ni) ) {
      return false;
    }
    if ( (*yvert_ptr < 0) || (*yvert_ptr > img_nj) ) {
      return false;
    }
  }
  return true;
}


bool full_cube_visible(vbl_bounding_box<double,3> const& bbox, vpgl_camera<double> const* cam, unsigned int img_ni, unsigned int img_nj, bool do_front_test)
{
  // make sure cube vertices are in front of camera
  if (do_front_test) {
    if (vpgl_perspective_camera<double> const* pcam = dynamic_cast<vpgl_perspective_camera<double> const*>(cam)) {

      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmin(),bbox.ymin(),bbox.zmin())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmin(),bbox.ymin(),bbox.zmax())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmin(),bbox.ymax(),bbox.zmin())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmin(),bbox.ymax(),bbox.zmax())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmax(),bbox.ymin(),bbox.zmin())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmax(),bbox.ymin(),bbox.zmax())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmax(),bbox.ymax(),bbox.zmin())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmax(),bbox.ymax(),bbox.zmax())))
        return false;
    }
    // if not a perspective camera, no test
  }
  // make sure corners project into image bounds
  vbl_bounding_box<double,2> img_bb;
  img_bb.update(0.0, 0.0);
  img_bb.update(img_ni, img_nj);
  // project corners of block into image
  double u,v;
  cam->project(bbox.xmin(),bbox.ymin(),bbox.zmin(),u,v);
  if (!img_bb.inside(u,v))
    return false;
  cam->project(bbox.xmin(),bbox.ymin(),bbox.zmax(),u,v);
  if (!img_bb.inside(u,v))
    return false;
  cam->project(bbox.xmin(),bbox.ymax(),bbox.zmin(),u,v);
  if (!img_bb.inside(u,v))
    return false;
  cam->project(bbox.xmin(),bbox.ymax(),bbox.zmax(),u,v);
  if (!img_bb.inside(u,v))
    return false;
  cam->project(bbox.xmax(),bbox.ymin(),bbox.zmin(),u,v);
  if (!img_bb.inside(u,v))
    return false;
  cam->project(bbox.xmax(),bbox.ymin(),bbox.zmax(),u,v);
  if (!img_bb.inside(u,v))
    return false;
  cam->project(bbox.xmax(),bbox.ymax(),bbox.zmin(),u,v);
  if (!img_bb.inside(u,v))
    return false;
  cam->project(bbox.xmax(),bbox.ymax(),bbox.zmax(),u,v);
  if (!img_bb.inside(u,v))
    return false;

  return true;
}

bool cube_visible(vbl_bounding_box<double,3> const& bbox, vpgl_camera<double> const* cam, unsigned int i0, unsigned int j0, unsigned int img_ni, unsigned int img_nj, bool do_front_test)
{
  // make sure cube vertices are in front of camera
  if (do_front_test) {
    if (vpgl_perspective_camera<double> const* pcam = dynamic_cast<vpgl_perspective_camera<double> const*>(cam)) {

      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmin(),bbox.ymin(),bbox.zmin())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmin(),bbox.ymin(),bbox.zmax())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmin(),bbox.ymax(),bbox.zmin())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmin(),bbox.ymax(),bbox.zmax())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmax(),bbox.ymin(),bbox.zmin())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmax(),bbox.ymin(),bbox.zmax())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmax(),bbox.ymax(),bbox.zmin())))
        return false;
      if (pcam->is_behind_camera(vgl_homg_point_3d<double>(bbox.xmax(),bbox.ymax(),bbox.zmax())))
        return false;
    }
    // if not a perspective camera, no test
  }
  // make sure corners project into image bounds
  vbl_bounding_box<double,2> cube_proj_bb;
  vbl_bounding_box<double,2> img_bb;
  img_bb.update(i0, j0);
  img_bb.update(img_ni, img_nj);
  // project corners of block into image
  double u,v;
  cam->project(bbox.xmin(),bbox.ymin(),bbox.zmin(),u,v);
  cube_proj_bb.update(u,v);
  cam->project(bbox.xmin(),bbox.ymin(),bbox.zmax(),u,v);
  cube_proj_bb.update(u,v);
  cam->project(bbox.xmin(),bbox.ymax(),bbox.zmin(),u,v);
  cube_proj_bb.update(u,v);
  cam->project(bbox.xmin(),bbox.ymax(),bbox.zmax(),u,v);
  cube_proj_bb.update(u,v);
  cam->project(bbox.xmax(),bbox.ymin(),bbox.zmin(),u,v);
  cube_proj_bb.update(u,v);
  cam->project(bbox.xmax(),bbox.ymin(),bbox.zmax(),u,v);
  cube_proj_bb.update(u,v);
  cam->project(bbox.xmax(),bbox.ymax(),bbox.zmin(),u,v);
  cube_proj_bb.update(u,v);
  cam->project(bbox.xmax(),bbox.ymax(),bbox.zmax(),u,v);
  cube_proj_bb.update(u,v);
  // check for intersection 
  if (disjoint(cube_proj_bb, img_bb)) {
    // add to list
    return false;
  }
  return true;
}


psm_cube_face_list visible_faces(vbl_bounding_box<double,3> &bbox, vpgl_camera<double> const* cam) {
  // 3d coordinates of cube
  double xverts_3d[] = {bbox.xmin(), bbox.xmax()};
  double yverts_3d[] = {bbox.ymin(), bbox.ymax()};
  double zverts_3d[] = {bbox.zmin(), bbox.zmax()};
  double xverts_2d[8],yverts_2d[8];

  unsigned int n=0;
  for (unsigned int k=0; k<2; ++k) {
    for (unsigned int j=0; j<2; ++j) {
      for (unsigned int i=0; i<2; ++i) {
        cam->project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
        ++n;
      }
    }
  }

  psm_cube_face_list vis_faces = psm_cube_face::NONE;
  if (visible(xverts_2d,yverts_2d, 2,0,6)) {
    vis_faces |= psm_cube_face::X_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 1,3,7)) {
    vis_faces |= psm_cube_face::X_HIGH;
  }
  if (visible(xverts_2d,yverts_2d, 0,1,5)) {
    vis_faces |= psm_cube_face::Y_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 7,3,2)) {
    vis_faces |= psm_cube_face::Y_HIGH;
  }
  if (visible(xverts_2d,yverts_2d, 0,2,1)) {
    vis_faces |= psm_cube_face::Z_LOW;
  }
  if (visible(xverts_2d,yverts_2d, 4,5,6)) {
    vis_faces |= psm_cube_face::Z_HIGH;
  }
  return vis_faces;
}


psm_cube_face_list visible_faces(vbl_bounding_box<double,3> &bbox, vgl_point_3d<double> const& cam_center)
{
  psm_cube_face_list faces = psm_cube_face::NONE;

  if (cam_center.x() > bbox.xmax()) {
    faces |= psm_cube_face::X_HIGH;
  }
  else if (cam_center.x() < bbox.xmin()) {
    faces |= psm_cube_face::X_LOW;
  }
  if (cam_center.y() > bbox.ymax()) {
    faces |= psm_cube_face::Y_HIGH;
  }
  else if (cam_center.y() < bbox.ymin()) {
    faces |= psm_cube_face::Y_LOW;
  }
  if (cam_center.z() > bbox.zmax()) {
    faces |= psm_cube_face::Z_HIGH;
  }
  else if (cam_center.z() < bbox.zmin()) {
    faces |= psm_cube_face::Z_LOW;
  }

  return faces;
}

//: function to compute percentage of pixel covered by triangle for antialiasing
float pixel_polygon_coverage(unsigned int i, unsigned int j, double *xverts_2d, double *yverts_2d, unsigned int v0, unsigned int v1, unsigned int v2)
{
  // TODO: more efficient implementation of this.
  // just subsample pixel and and determine ratio of points inside triangle for now
  unsigned int nsamples = 8;
  unsigned int inside_count = 0;
  double subgrid_origin_x = i + 1.0/(2.0*nsamples);
  double subgrid_origin_y = j + 1.0/(2.0*nsamples);

  for (unsigned int subi = 0; subi < nsamples; ++subi) {
    for (unsigned int subj = 0; subj < nsamples; ++subj) {
      double x = subgrid_origin_x + ((double)subi)/nsamples;
      double y = subgrid_origin_y + ((double)subj)/nsamples;
      // test if x,y is inside triangle v0 v1 v2
      // compute barycentric coordinates of 2d point
      vgl_vector_2d<double> d1(xverts_2d[v0] - xverts_2d[v2], yverts_2d[v0] - yverts_2d[v2]);
      vgl_vector_2d<double> d2(xverts_2d[v1] - xverts_2d[v2], yverts_2d[v1] - yverts_2d[v2]);
      vgl_vector_2d<double> d3(x - xverts_2d[v2], y - yverts_2d[v2]);

      // solve 2x2 system [ d1 d2 ] * x  = d3
      double det = d1.x() * d2.y() - d2.x()*d1.y();
      if (det == 0) {
        vcl_cerr << "error computing barycentic coordinates of subsampled point" << vcl_endl;
        return 0.0f;
      }
      double w0 = (d3.x()*d2.y() - d2.x()*d3.y())/det;
      double w1 = (d1.x()*d3.y() - d3.x()*d1.y())/det;
      if ((w0 <= 1.0) && (w0 >= 0.0) && (w1 <= 1.0) && (w1 >= 0.0)) {
        ++inside_count;
      }
    }
  }
  return ((float)inside_count)/(nsamples*nsamples);
}
