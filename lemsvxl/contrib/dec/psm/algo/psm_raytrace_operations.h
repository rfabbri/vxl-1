#ifndef psm_raytrace_operations_h_
#define psm_raytrace_operations_h_

#include <vbl/vbl_bounding_box.h>
#include <vnl/algo/vnl_determinant.h>

#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_triangle_scan_iterator.h>

#include <hsds/hsds_fd_tree.h>
#include <psm/psm_apm_traits.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>

#include "psm_image_pyramid.h"
#include "psm_triangle_scan_iterator_aa.h"
#include "psm_triangle_interpolation_iterator_aa.h"
#include "psm_triangle_interpolation_iterator.h"
#include "psm_triangle_scan_iterator.h"

class psm_cube_face
{
public:
  static const unsigned char NONE = 0x00;
  static const unsigned char Z_LOW = 0x01;
  static const unsigned char Z_HIGH = 0x02;
  static const unsigned char Y_LOW = 0x04;
  static const unsigned char Y_HIGH = 0x08;
  static const unsigned char X_LOW = 0x10;
  static const unsigned char X_HIGH = 0x20;
  static const unsigned char ALL = 0x3F;
};


//: functor used for normalizing cell_expected image
template<class T>
class safe_normalize_functor
{
public:
  safe_normalize_functor(float tol = 1e-6) : tol_(tol) {}

  void operator()(float norm_divisor, T &pix) const 
  {
    if (norm_divisor > tol_)
      pix /= norm_divisor;
    else
      pix = 0;
  }
  float tol_;
};

//: Functor class to compute exponential of image
class image_exp_functor
{
public:
  float operator()(float x)       const { return x<0?vcl_exp(x):1.0f; }
};

//: Functor class to compute inverse of image
class safe_inverse_functor
{
public:
  safe_inverse_functor(float tol = 0.0f) : tol_(tol) {}

  float operator()(float x)      const { return (x > tol_)? 1.0f/x : 0.0f; }

  float tol_;
};


typedef unsigned char psm_cube_face_list;

void project_cube_vertices(vbl_bounding_box<double,3> const &cube, vpgl_perspective_camera<double> const& cam, double* xverts_2d, double* yverts_2d, float* vert_distances, psm_cube_face_list &visible_faces);
void project_cube_vertices(vbl_bounding_box<double,3> const &cube, vpgl_camera<double> const* cam, double* xverts_2d, double* yverts_2d, float* vert_distances, psm_cube_face_list &visible_faces, double bigZ = 1000.0);


bool alpha_seg_len_aa(double *xverts_2d, double* yverts_2d, float* vert_dist, psm_cube_face_list visible_faces, float alpha, vil_image_view<float> &alpha_distance);
bool alpha_seg_len(double *xverts_2d, double* yverts_2d, float* vert_dist, psm_cube_face_list visible_faces, float alpha, vil_image_view<float> &alpha_distance);


bool cube_exit_point(vbl_bounding_box<double,3> const& cube, vgl_point_3d<double> const& pt, vgl_vector_3d<double> const& direction, 
                     vgl_point_3d<double> &exit_pt, unsigned int &step_dir, bool &step_positive);

bool cube_visible(vbl_bounding_box<double,3> const& bbox, vpgl_camera<double> const* cam, unsigned int i0, unsigned int j0, unsigned int img_ni, unsigned int img_nj, bool do_front_test = true);

bool full_cube_visible(vbl_bounding_box<double,3> const& bbox, vpgl_camera<double> const* cam, unsigned int img_ni, unsigned int img_nj, bool do_front_test);
bool full_cube_visible(const double* xverts_2d, const double* yverts_2d, unsigned int img_ni, unsigned int img_nj);

psm_cube_face_list visible_faces(vbl_bounding_box<double,3> &bbox, vpgl_camera<double> const* cam);
psm_cube_face_list visible_faces(vbl_bounding_box<double,3> &bbox, vgl_point_3d<double> const& cam_center);


bool cube_camera_distance(vbl_bounding_box<double,3> const& cube, vpgl_perspective_camera<double> const& cam, vil_image_view<float> &front_dist, vil_image_view<float> &back_dist, psm_cube_face_list visible_faces=psm_cube_face::ALL);

bool cube_camera_distance_aa(vbl_bounding_box<double,3> const& cube, vpgl_perspective_camera<double> const& cam, vil_image_view<float> &front_dist, vil_image_view<float> &back_dist, psm_cube_face_list visible_faces=psm_cube_face::ALL);

//: returns the sum of the weights
template<class T1, class T2, class tri_it_t>
bool tri_interpolated_weighted_sum(tri_it_t &tri_it, vil_image_view<T1> const& weights, vil_image_view<T2> const& values, T2 &val_sum, T1 &weight_sum, bool subtract)
{
  tri_it.reset();
  while(tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= values.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)values.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)vcl_max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)vcl_min((int)values.ni(),tri_it.endx());

    if (subtract) {
      for (unsigned int x = startx; x < endx; ++x) {
        T1 weight = tri_it.value_at(x)*weights(x,yu);
        val_sum -= weight * values(x,yu);
        weight_sum -= weight;
      }
    }else {
      for (unsigned int x = startx; x < endx; ++x) {
        T1 weight = tri_it.value_at(x)*weights(x,yu);
        val_sum += weight * values(x,yu);
        weight_sum += weight;
      }
    }
  }
  return true;
}

#if 1
//: returns the sum of the weights
template<class T, class tri_int_it_t>
bool tri_interpolated_weighted_sum(tri_int_it_t &tri_it, vil_image_view<T> const& values, T &val_sum, float &weight_sum, bool subtract)
{
  tri_it.reset();
  while(tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= values.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)values.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)vcl_max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)vcl_min((int)values.ni(),tri_it.endx());

    if (subtract) {
      for (unsigned int x = startx; x < endx; ++x) {
        float point_dist = tri_it.value_at(x);
        val_sum -= values(x,yu) * point_dist;
        weight_sum -= point_dist;
      }
    } else {
      for (unsigned int x = startx; x < endx; ++x) {
        float point_dist = tri_it.value_at(x);
        val_sum += values(x,yu) * point_dist;
        weight_sum += point_dist;
      }
    }
  }
  return true;
}
#endif

template<class T, class tri_it_t>
void tri_interpolate_values(tri_it_t &tri_it, vil_image_view<T> &img, bool subtract = false)
{
  tri_it.reset();
  while(tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)vcl_max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)vcl_min((int)img.ni(),tri_it.endx());

    if (subtract) {
      for (unsigned int x = startx; x < endx; ++x) {
        img(x,yu) -= tri_it.value_at(x);
      }
    }
    else {
      for (unsigned int x = startx; x < endx; ++x) {
        img(x,yu) += tri_it.value_at(x);
      }
    }
  }
  return;
}


#if 0
template<class T, class tri_it_T>
void tri_interpolate_values(tri_it_T &tri_it, double* xvals, double* yvals, T* vals, vil_image_view<T> &img, vil_image_view<float> &pix_coverage, unsigned int v0=0, unsigned int v1=1, unsigned int v2=2)
{
  // first compute s0, s1, s2 such that  val = s0*x + s1*y + s2 for any point within the triangle
  // (no need to compute barycentric coordinates at each step)
  // subtract 0.5 from xvals and yvals, so that interpolated value at pixel x,y evaluates to coordinates x+0.5, y+0.5 (center of pixel)
  double Acol0[] = {xvals[v0]-0.5, xvals[v1]-0.5, xvals[v2]-0.5};
  double Acol1[] = {yvals[v0]-0.5, yvals[v1]-0.5, yvals[v2]-0.5};
  double Acol2[] = {1.0, 1.0, 1.0};

  double Z[] = {vals[v0], vals[v1], vals[v2]};

  double detA = vnl_determinant(Acol0, Acol1, Acol2);
  double s0 = vnl_determinant(Z, Acol1, Acol2) / detA;
  double s1 = vnl_determinant(Acol0, Z, Acol2) / detA;
  double s2 = vnl_determinant(Acol0, Acol1, Z) / detA;

  tri_it.reset();
  while(tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu > img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)vcl_max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)vcl_min((int)img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      T interp_val = (T)(s0*x + s1*y + s2);
      if ( (min_img(x,yu) == 0) || (min_img(x,yu) > interp_val) ){
        min_img(x,yu) = interp_val;
      }
      if (max_img(x,yu) < interp_val) {
        max_img(x,yu) = interp_val;
      }
    }
  }
  return;

}
#endif

template<class T, class tri_it_T>
void tri_interpolate_min_max(tri_it_T &tri_it, double* xvals, double* yvals, T* vals, vil_image_view<T> &min_img, vil_image_view<T> &max_img, unsigned int v0=0, unsigned int v1=1, unsigned int v2=2)
{
  // first compute s0, s1, s2 such that  val = s0*x + s1*y + s2 for any point within the triangle
  // (no need to compute barycentric coordinates at each step)
  // subtract 0.5 from xvals and yvals, so that interpolated value at pixel x,y evaluates to coordinates x+0.5, y+0.5 (center of pixel)
  double Acol0[] = {xvals[v0]-0.5, xvals[v1]-0.5, xvals[v2]-0.5};
  double Acol1[] = {yvals[v0]-0.5, yvals[v1]-0.5, yvals[v2]-0.5};
  double Acol2[] = {1.0, 1.0, 1.0};

  double Z[] = {vals[v0], vals[v1], vals[v2]};

  double detA = vnl_determinant(Acol0, Acol1, Acol2);
  double s0 = vnl_determinant(Z, Acol1, Acol2) / detA;
  double s1 = vnl_determinant(Acol0, Z, Acol2) / detA;
  double s2 = vnl_determinant(Acol0, Acol1, Z) / detA;

  tri_it.reset();
  while(tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= max_img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)max_img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)vcl_max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)vcl_min((int)max_img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      T interp_val = (T)(s0*x + s1*y + s2);
      if ( (min_img(x,yu) == 0) || (min_img(x,yu) > interp_val) ){
        min_img(x,yu) = interp_val;
      }
      if (max_img(x,yu) < interp_val) {
        max_img(x,yu) = interp_val;
      }
    }
  }
  return;
}

template<class T1, class T2>
void tri_weighted_sum(vgl_triangle_scan_iterator<double> &tri_it, vil_image_view<T1> &img, vil_image_view<T2> &weights, T1 &val_sum, T2 &weight_sum)
{
  tri_it.reset();
  while(tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)vcl_max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)vcl_min((int)img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      val_sum += img(x,yu)*weights(x,yu);
      weight_sum += weights(x,yu);
    }
  }
  return;

}

template<class T, class tri_it_T>
void tri_fill_value(tri_it_T &tri_it, vil_image_view<T> &img, T val)
{    
  tri_it.reset();
  while(tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)vcl_max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)vcl_min((int)img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      img(x,yu) = val;
    }
  }
  return;
}

template<class T>
void tri_fill_value_aa(psm_triangle_scan_iterator_aa &tri_it, vil_image_view<T> &img, vil_image_view<float> &aa_weights, T val)
{    
  tri_it.reset();
  while(tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0){
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)vcl_max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)vcl_min((int)img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      float pix_cov = tri_it.pix_coverage(x);
      if ((pix_cov < 0.0f) || (pix_cov > 1.0f)) {
        vcl_cerr << " error: pix_cov = " << pix_cov << vcl_endl;
      }
      aa_weights(x,yu) += tri_it.pix_coverage(x);
      img(x,yu) += val * pix_cov;
    }
  }
  return;
}


inline void init_triangle_scan_iterator(vgl_triangle_scan_iterator<double> &tri_it, const double* xverts, const double* yverts, unsigned int v0=0, unsigned int v1=1, unsigned int v2=2)
{
  // subract 0.5 to follow convention that the center of pixel i,j is located at coordinates i+0.5, j+0.5
  tri_it.a.x = xverts[v0] - 0.5; tri_it.a.y = yverts[v0] - 0.5;
  tri_it.b.x = xverts[v1] - 0.5; tri_it.b.y = yverts[v1] - 0.5;
  tri_it.c.x = xverts[v2] - 0.5; tri_it.c.y = yverts[v2] - 0.5;
}

//: takes the mean of pixel values inside the cubes projection, weights by the segment length
template<class T>
bool cube_mean_aa(double* xverts_2d, double* yverts_2d, float* vert_dists, psm_cube_face_list visible_faces, vil_image_view<T> const& values, T &mean)
{
  // for each face, create two triangle iterators to interpolate distances
  T value_sum(0);
  float weight_sum(0);
  // X_LOW
  // tri 0
  psm_triangle_interpolation_iterator_aa<float> tri_it(xverts_2d, yverts_2d, vert_dists, 2, 0, 6);
  if (visible_faces & psm_cube_face::X_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 0, 4, 6);
  if (visible_faces & psm_cube_face::X_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);


  // X_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 1, 3, 7);
  if (visible_faces & psm_cube_face::X_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 7, 5, 1);
  if (visible_faces & psm_cube_face::X_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);


  // Y_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 0, 1, 5);
  if (visible_faces & psm_cube_face::Y_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 4, 0, 5);
  if (visible_faces & psm_cube_face::Y_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Y_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 7, 3, 2);
  if (visible_faces & psm_cube_face::Y_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 2, 6, 7);
  if (visible_faces & psm_cube_face::Y_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Z_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 0, 2, 1);
  if (visible_faces & psm_cube_face::Z_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 1, 2, 3);
  if (visible_faces & psm_cube_face::Z_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Z_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 4, 5, 6);
  if (visible_faces & psm_cube_face::Z_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 5, 7, 6);
  if (visible_faces & psm_cube_face::Z_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  if (vcl_fabs(weight_sum) > 1e-6)  {
    mean = value_sum / weight_sum;
  }
  else {
    mean = T(0);
    return false;
  }
  return true;

}


//: takes the mean of pixel values inside the cubes projection, weights by the segment length
template<class T>
bool cube_mean(double* xverts_2d, double* yverts_2d, float* vert_dists, psm_cube_face_list visible_faces, vil_image_view<T> const& values, T &mean)
{
  // for each face, create two triangle iterators to interpolate distances
  T value_sum(0);
  float weight_sum(0);
  // X_LOW
  // tri 0
  psm_triangle_interpolation_iterator<float> tri_it(xverts_2d, yverts_2d, vert_dists, 2, 0, 6);
  if (visible_faces & psm_cube_face::X_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 0, 4, 6);
  if (visible_faces & psm_cube_face::X_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);


  // X_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 1, 3, 7);
  if (visible_faces & psm_cube_face::X_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 7, 5, 1);
  if (visible_faces & psm_cube_face::X_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);


  // Y_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 0, 1, 5);
  if (visible_faces & psm_cube_face::Y_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 4, 0, 5);
  if (visible_faces & psm_cube_face::Y_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Y_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 7, 3, 2);
  if (visible_faces & psm_cube_face::Y_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 2, 6, 7);
  if (visible_faces & psm_cube_face::Y_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Z_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 0, 2, 1);
  if (visible_faces & psm_cube_face::Z_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 1, 2, 3);
  if (visible_faces & psm_cube_face::Z_LOW) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Z_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 4, 5, 6);
  if (visible_faces & psm_cube_face::Z_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 5, 7, 6);
  if (visible_faces & psm_cube_face::Z_HIGH) 
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  if (vcl_fabs(weight_sum) > 1e-6)  {
    mean = value_sum / weight_sum;
  }
  else {
    mean = T(0);
    return false;
  }
  return true;

}



//: takes the weighted mean of pixel values inside the cubes projection, weighted by the segment_len*weight
template<class T1, class T2>
bool cube_weighted_mean_aa(double* xverts_2d, double* yverts_2d, float* vert_dists, psm_cube_face_list visible_faces, vil_image_view<T1> const& img, vil_image_view<T2> const& weights, T1 &weighted_mean)
{
  // for each face, create two triangle iterators to interpolate distances
  // X_LOW
  // tri 0
  psm_triangle_interpolation_iterator_aa<float> tri_it(xverts_2d, yverts_2d, vert_dists, 2, 0, 6);
  T1 value_sum(0);
  T2 weight_sum(0);
  if (visible_faces & psm_cube_face::X_LOW) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 0, 4, 6);
  if (visible_faces & psm_cube_face::X_LOW) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);


  // X_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 1, 3, 7);
  if (visible_faces & psm_cube_face::X_HIGH) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);

  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 7, 5, 1);
  if (visible_faces & psm_cube_face::X_HIGH) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);


  // Y_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 0, 1, 5);
  if (visible_faces & psm_cube_face::Y_LOW) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 4, 0, 5);
  if (visible_faces & psm_cube_face::Y_LOW) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);

  // Y_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 7, 3, 2);
  if (visible_faces & psm_cube_face::Y_HIGH) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 2, 6, 7);
  if (visible_faces & psm_cube_face::Y_HIGH) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);

  // Z_LOW
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 0, 2, 1);
  if (visible_faces & psm_cube_face::Z_LOW) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 1, 2, 3);
  if (visible_faces & psm_cube_face::Z_LOW) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);

  // Z_HIGH
  // tri 0
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 4, 5, 6);
  if (visible_faces & psm_cube_face::Z_HIGH) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);
  // tri 1
  tri_it = psm_triangle_interpolation_iterator_aa<float>(xverts_2d, yverts_2d, vert_dists, 5, 7, 6);
  if (visible_faces & psm_cube_face::Z_HIGH) 
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, weights, img, value_sum, weight_sum, false);

  if (vcl_fabs(weight_sum) > 1e-6) 
    weighted_mean = value_sum / weight_sum;
  else {
    weighted_mean = T1(0);
    return false;
  }
  return true;
}




template<class T1, class T2>
bool cube_weighted_mean(vbl_bounding_box<double,3> const& cube, vpgl_perspective_camera<double> const& cam, vil_image_view<T1> &img, vil_image_view<T2> &weights, T1 &weighted_mean, psm_cube_face_list visible_faces = psm_cube_face::ALL, T2 weight_tol = 1e-8)
{
  // 3d coordinates of cube
  double xverts_3d[] = {cube.xmin(), cube.xmax()};
  double yverts_3d[] = {cube.ymin(), cube.ymax()};
  double zverts_3d[] = {cube.zmin(), cube.zmax()};
  // 2d projected coordinates of cube
  double xverts_2d[8], yverts_2d[8];

  unsigned int n=0;
  for (unsigned int k=0; k<2; ++k) {
    for (unsigned int j=0; j<2; ++j) {
      for (unsigned int i=0; i<2; ++i) {
        cam.project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
        ++n;
      }
    }
  }
  // for each face, create two triangle iterators and fill in pixel data
  T1 val_sum(0);
  T2 weight_sum(0);

  vgl_triangle_scan_iterator<double> tri_it;
  if (visible_faces & psm_cube_face::X_LOW) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 2, 0, 6);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 4, 6);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
  }
  if (visible_faces & psm_cube_face::X_HIGH) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 1, 3, 7);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 7, 5, 1);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
  }
  if (visible_faces & psm_cube_face::Y_LOW) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 1, 5);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 4, 0, 5);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
  }
  if (visible_faces & psm_cube_face::Y_HIGH) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 7, 3, 2);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 2, 6, 7);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
  }
  if (visible_faces & psm_cube_face::Z_LOW) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 1, 2);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 1, 3, 2);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
  }
  if (visible_faces & psm_cube_face::Z_HIGH) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 4, 5, 6);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 5, 7, 6);
    tri_weighted_sum(tri_it, img, weights, val_sum, weight_sum);
  }  
  if (vcl_fabs(weight_sum) > weight_tol) {
    weighted_mean = val_sum / weight_sum;
    return true;
  }
  else {
    weighted_mean = T1(0);
    return false;
  }

}

template<class T>
bool cube_fill_value_aa(double* xverts_2d, double* yverts_2d, psm_cube_face_list visible_faces, vil_image_view<T> &img, vil_image_view<float> &pix_coverage, T const& val)
{

  // for each face, create two triangle iterators and fill in pixel data
  psm_triangle_scan_iterator_aa tri_it(xverts_2d, yverts_2d, 2, 0, 6);
  if (visible_faces & psm_cube_face::X_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 2, 0, 6);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 0, 4, 6);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
  }
  if (visible_faces & psm_cube_face::X_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 1, 3, 7);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 7, 5, 1);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
  }
  if (visible_faces & psm_cube_face::Y_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 0, 1, 5);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 4, 0, 5);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
  }
  if (visible_faces & psm_cube_face::Y_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 7, 3, 2);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 2, 6, 7);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
  }
  if (visible_faces & psm_cube_face::Z_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 0, 1, 2);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 1, 3, 2);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
  }
  if (visible_faces & psm_cube_face::Z_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 4, 5, 6);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 5, 7, 6);
    tri_fill_value_aa(tri_it, img, pix_coverage, val);
  }  
  return true;
}

template<class T>
bool cube_fill_value_aa(vbl_bounding_box<double,3> const& cube, vpgl_perspective_camera<double> const& cam, vil_image_view<T> &img, T const& val, psm_cube_face_list visible_faces = psm_cube_face::ALL)
{
  // 3d coordinates of cube
  double xverts_3d[] = {cube.xmin(), cube.xmax()};
  double yverts_3d[] = {cube.ymin(), cube.ymax()};
  double zverts_3d[] = {cube.zmin(), cube.zmax()};
  // 2d projected coordinates of cube
  double xverts_2d[8], yverts_2d[8];

  unsigned int n=0;
  for (unsigned int k=0; k<2; ++k) {
    for (unsigned int j=0; j<2; ++j) {
      for (unsigned int i=0; i<2; ++i) {
        cam.project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
        ++n;
      }
    }
  }
  // for each face, create two triangle iterators and fill in pixel data
  psm_triangle_scan_iterator_aa tri_it(xverts_2d, yverts_2d, 2, 0, 6);
  if (visible_faces & psm_cube_face::X_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 2, 0, 6);
    tri_fill_value(tri_it,img,val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 0, 4, 6);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::X_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 1, 3, 7);
    tri_fill_value(tri_it,img,val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 7, 5, 1);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::Y_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 0, 1, 5);
    tri_fill_value(tri_it,img,val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 4, 0, 5);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::Y_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 7, 3, 2);
    tri_fill_value(tri_it,img,val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 2, 6, 7);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::Z_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 0, 1, 2);
    tri_fill_value(tri_it,img,val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 1, 3, 2);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::Z_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 4, 5, 6);
    tri_fill_value(tri_it,img,val);
    // tri 1
    tri_it = psm_triangle_scan_iterator_aa(xverts_2d, yverts_2d, 5, 7, 6);
    tri_fill_value(tri_it,img,val);
  }  
  return true;

}

template<class T>
bool cube_fill_value(double* xverts_2d, double* yverts_2d, psm_cube_face_list visible_faces, vil_image_view<T> &img, T const& val)
{

  // for each face, create two triangle iterators and fill in pixel data
  psm_triangle_scan_iterator tri_it(xverts_2d, yverts_2d, 2, 0, 6);
  if (visible_faces & psm_cube_face::X_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 2, 0, 6);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 0, 4, 6);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & psm_cube_face::X_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 1, 3, 7);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 7, 5, 1);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & psm_cube_face::Y_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 0, 1, 5);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 4, 0, 5);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & psm_cube_face::Y_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 7, 3, 2);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 2, 6, 7);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & psm_cube_face::Z_LOW) {
    // tri 0
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 0, 1, 2);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 1, 3, 2);
    tri_fill_value(tri_it, img, val);
  }
  if (visible_faces & psm_cube_face::Z_HIGH) {
    // tri 0
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 4, 5, 6);
    tri_fill_value(tri_it, img, val);
    // tri 1
    tri_it = psm_triangle_scan_iterator(xverts_2d, yverts_2d, 5, 7, 6);
    tri_fill_value(tri_it, img, val);
  }  
  return true;
}

#if 1
template<class T>
bool cube_fill_value(vbl_bounding_box<double,3> const& cube, vpgl_perspective_camera<double> const& cam, vil_image_view<T> &img, T const& val, psm_cube_face_list visible_faces = psm_cube_face::ALL)
{
  // 3d coordinates of cube
  double xverts_3d[] = {cube.xmin(), cube.xmax()};
  double yverts_3d[] = {cube.ymin(), cube.ymax()};
  double zverts_3d[] = {cube.zmin(), cube.zmax()};
  // 2d projected coordinates of cube
  double xverts_2d[8], yverts_2d[8];

  unsigned int n=0;
  for (unsigned int k=0; k<2; ++k) {
    for (unsigned int j=0; j<2; ++j) {
      for (unsigned int i=0; i<2; ++i) {
        cam.project(xverts_3d[i],yverts_3d[j],zverts_3d[k],xverts_2d[n],yverts_2d[n]);
        ++n;
      }
    }
  }
  // for each face, create two triangle iterators and fill in pixel data
  vgl_triangle_scan_iterator<double> tri_it;
  if (visible_faces & psm_cube_face::X_LOW) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 2, 0, 6);
    tri_fill_value(tri_it,img,val);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 4, 6);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::X_HIGH) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 1, 3, 7);
    tri_fill_value(tri_it,img,val);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 7, 5, 1);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::Y_LOW) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 1, 5);
    tri_fill_value(tri_it,img,val);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 4, 0, 5);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::Y_HIGH) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 7, 3, 2);
    tri_fill_value(tri_it,img,val);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 2, 6, 7);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::Z_LOW) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 0, 1, 2);
    tri_fill_value(tri_it,img,val);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 1, 3, 2);
    tri_fill_value(tri_it,img,val);
  }
  if (visible_faces & psm_cube_face::Z_HIGH) {
    // tri 0
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 4, 5, 6);
    tri_fill_value(tri_it,img,val);
    // tri 1
    init_triangle_scan_iterator(tri_it, xverts_2d, yverts_2d, 5, 7, 6);
    tri_fill_value(tri_it,img,val);
  }  
  return true;
}
#endif

template <psm_apm_type APM>
typename psm_apm_traits<APM>::obs_datatype mean_cell_observation(vbl_bounding_box<double,3> const& cell_bb, vil_image_view<typename psm_apm_traits<APM>::obs_mathtype> const& img, vpgl_perspective_camera<double> const& cam)
{
  // create 2d bounding box cell projection
  vbl_bounding_box<double,2> cell_proj_bb;

  // project each corner of the cell into the image and add to bb
  double xverts[] = {cell_bb.xmin(), cell_bb.xmax()};
  double yverts[] = {cell_bb.ymin(), cell_bb.ymax()};
  double zverts[] = {cell_bb.zmin(), cell_bb.zmax()};
  double u,v;
  for (unsigned int i=0; i<2; ++i) {
    for (unsigned int j=0; j<2; ++j) {
      for (unsigned int k=0; k<2; ++k) {
        cam.project(xverts[i],yverts[j],zverts[k],u,v);
        cell_proj_bb.update(u,v);
      }
    }
  }
  unsigned int xmin = vcl_max((unsigned int)0, (unsigned int)vcl_floor(cell_proj_bb.xmin()));
  unsigned int xmax = vcl_min(img.ni()-1, (unsigned int)vcl_ceil(cell_proj_bb.xmax()));
  unsigned int ymin = vcl_max((unsigned int)0, (unsigned int)vcl_floor(cell_proj_bb.ymin()));
  unsigned int ymax = vcl_min(img.nj()-1, (unsigned int)vcl_ceil(cell_proj_bb.ymax()));

  typename psm_apm_traits<APM>::obs_mathtype mean_obs(0);
  unsigned int nobs = 0;

  if ( (xmax >= xmin) && (ymax >= ymin) ) {
    // cell is at least partially in image
    for (unsigned int y=ymin; y<=ymax; ++y) {
      for (unsigned int x=xmin; x<=xmax; ++x) {
        mean_obs += img(x,y);
        ++nobs;
      }
    }
    mean_obs /= nobs;
  }

  return mean_obs;
}



//: function to compute percentage of pixel covered by triangle for antialiasing
float pixel_polygon_coverage(unsigned int i, unsigned int j, double *xverts_2d, double *yverts_2d, unsigned int v0, unsigned int v1, unsigned int v2);


#endif

