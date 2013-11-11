#if !defined(XMVG_PARALLEL_BEAM_FILTER_TXX_)
#define XMVG_PARALLEL_BEAM_FILTER_TXX_

#include "xmvg_parallel_beam_filter_3d.h"
#include <vpgl/algo/vpgl_project.h>
#include "xmvg_transform_camera.h"
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x2.h>

//uncomment for checking/testing (and performance hit)
//#define XMVG_PARALLEL_BEAM_FILTER_3D_TEST


template <class filter_3d, class T>
xmvg_parallel_beam_filter_3d<filter_3d, T>::
xmvg_parallel_beam_filter_3d(filter_3d filter)
  : filter_(filter) {}
                                                                      

template <class filter_3d, class T>
xmvg_atomic_filter_2d<T> xmvg_parallel_beam_filter_3d<filter_3d, T>::splat(vgl_point_3d<double> centre, xmvg_perspective_camera<double> cam){
  /* Adjust the camera so that the center of the filter is at (0,0,0) and the filter is oriented along the z-axis.
     Find the unit axis vectors u and v in the image plane.
     Find the ray from the camera center to the center of the filter.
       

      Loop through all the pixels:  each iteration finds the new location of the pixel
      in world corodinates (using the deltas) and computes the ray trace/integral.
  */


  //transform the camera so the filter is at the origin and the z-axis is the orientation of the filter
  xmvg_perspective_camera<double> cam_trans
    = xmvg_transform_camera(cam, filter_.descriptor().rotation_axis(),
                            filter_.descriptor().rotation_angle(), 
                            centre);
                                                                    
    

  /* Soon we will compute vectors u and v that are the basis in image space
     but represented in world coordinates.
     However, we need to scale u and v to take into account
     that movements near the filter correspond
     to larger movements on the image plane, since
     the image plane is farther from the source than the filter.
     Justification for the scaling factor is in image_basis_vectors.tex
  */
  double X_proj, Y_proj, Z_proj=0;
  ((cam_trans.get_rotation_matrix()) * (cam_trans.camera_center())).get_nonhomogeneous(X_proj, Y_proj, Z_proj);

  /* Assign to image_plane_basis_ a matrix whose columns are
     the basis vectors u and v of the image coordinate system, represented in world coordinates,
     scaled by the compression ratio.
     Justification for the formula used is in image_basis_vectors.tex
  */
    vnl_double_3x3 R = cam_trans.get_rotation_matrix().get_matrix().extract(3,3);
    vnl_matrix_inverse<double> Rinv(R);
    vnl_matrix<double> inverse_calibration(3,2, 0.);
    inverse_calibration[0][0] = 1./cam_trans.get_calibration().x_scale();
    inverse_calibration[1][1] = 1./cam_trans.get_calibration().y_scale();
    vnl_matrix<double>
      image_plane_basis = -Z_proj * (Rinv * inverse_calibration);
    vgl_vector_3d<double> ray_start_increment_horizontal(image_plane_basis.get(0,0), image_plane_basis.get(1,0), image_plane_basis.get(2,0));


  //Find the ray from the camera center to the center of the filter.
  vgl_homg_point_3d<double> filter_center(0.,0.,0.);
  vgl_homg_point_3d<double> camera_center = cam_trans.camera_center();
  vgl_vector_3d<double> ray_direction = filter_center - camera_center;
  //Find the point where this ray pierces the image plane 
  vgl_point_2d<double> splat_center(cam_trans.project(filter_center));
  //Represent the camera center and splat_center as vnl vectors
  double cx, cy, cz;
  camera_center.get_nonhomogeneous(cx, cy, cz);
  vnl_double_3 camera_center_vnl(cx, cy, cz);
  vnl_double_2 splat_center_vnl(splat_center.x(), splat_center.y());

  //get the bounding box in the image plane
  vgl_box_2d<int> int_box_2d = get_box_2d(cam_trans, filter_.descriptor().box());

  int min_x = int_box_2d.min_x();   int max_x = int_box_2d.max_x();
  int min_y = int_box_2d.min_y();   int max_y = int_box_2d.max_y();

  unsigned u_size = max_x - min_x + 1;
  unsigned v_size = max_y - min_y + 1;
  // the variables to create an atomic_filter_2d
  vnl_int_2 left_upper(min_x, min_y);
  vnl_int_2 size(u_size, v_size);
  vbl_array_2d<double> image(u_size, v_size);

  //prepare to do incremental line integration
  typename filter_3d::line_integrator_t line_integrator = filter_.line_integrator(ray_direction);
#if defined(XMVG_PARALLEL_BEAM_FILTER_3D_TEST)
  //for testing ********
  old_xmvg_pipe_filter_3d test_filter(filter_.descriptor());
  typename old_xmvg_pipe_filter_3d::line_integrator_t test_line_integrator = test_filter.line_integrator(ray_direction);
#endif
  for(int j=min_y; j<=max_y; j++){
      // Compute the initial point from which the line of integration emanates.
      //find the ray that is one pixel left of the pixel we really want to start from
      //since the line integrator increments by ray_start_increment_horizontal before integrating 
   vnl_double_3 ray_start = image_plane_basis * (vnl_double_2(static_cast<double>(min_x-1),static_cast<double>(j)) - splat_center_vnl)
    + camera_center_vnl;
    vgl_point_3d<double> ray_start_vgl(ray_start[0], ray_start[1], ray_start[2]);
    line_integrator.init_ray_start(ray_start_vgl, ray_start_increment_horizontal);
#if defined(XMVG_PARALLEL_BEAM_FILTER_3D_TEST)
    test_line_integrator.init_ray_start(ray_start_vgl, ray_start_increment_horizontal);
#endif
    for(int i=min_x; i<=max_x; i++){
      double val = line_integrator.increment_ray_start_and_integrate();
#if defined(XMVG_PARALLEL_BEAM_FILTER_3D_TEST)
      double old_integrator_val = test_line_integrator.increment_ray_start_and_integrate();
      if (vcl_abs(val-old_integrator_val)>.000001){
        vcl_cout << "xmvg_parallel_beam_filter_3d.txx INTEGRATOR DISCREPANCY\n";
      }
      //test against traditional approach
      vgl_point_3d<double> test_ray_start_vgl(ray_start[0] + (i-min_x+1)*image_plane_basis.get(0,0),
                                      ray_start[1] + (i-min_x+1)*image_plane_basis.get(1,0),
                                      ray_start[2] + (i-min_x+1)*image_plane_basis.get(2,0));
      double inner = line_integral(test_ray_start_vgl, ray_direction, filter_.descriptor().inner_radius());
      double outer = line_integral(test_ray_start_vgl, ray_direction, filter_.descriptor().outer_radius());
      double val_check = 2 * inner - outer;
      if (vcl_abs(val - val_check) > .000001){
        vcl_cout << "xmvg_parallel_beam_filter_3d.txx DISREPANCY\n";
      }
#endif
      image[i - min_x][j - min_y] = val;
    }
  }
  // took sum aout from the cinstructor.. Gamze
  xmvg_atomic_filter_2d<double> result(left_upper, size, image);
  return result;
}



  /*  Should be computed by scaling the box,
      finding closest point in image plane of each vertex of the box,
      and building a bounding box for the closest points
  */
template <class filter_3d, class T>
vgl_box_2d<int> xmvg_parallel_beam_filter_3d<filter_3d, T>::get_box_2d(const xmvg_perspective_camera<double> &cam,
                                                                       vgl_box_3d<double> const & box){
  //OLD CODE HERE
    vgl_box_2d<double> box_2d = vpgl_project::project_bounding_box(cam, box);
    int min_x = int(vcl_floor(box_2d.min_x()));   int max_x = int(vcl_ceil(box_2d.max_x()));
    int min_y = int(vcl_floor(box_2d.min_y()));   int max_y = int(vcl_ceil(box_2d.max_y()));
    return vgl_box_2d<int>(min_x, max_x, min_y, max_y);
}

//: XML write
template <class filter_3d, class T>
void x_write(vcl_ostream& os, xmvg_parallel_beam_filter_3d<filter_3d, T> f)
{
  x_write(os, f.filter());
}

#if defined(XMVG_PARALLEL_BEAM_FILTER_3D_TEST)
//The following is just for testing.
template <class filter_3d, class T>
double xmvg_parallel_beam_filter_3d<filter_3d, T>::line_integral(vgl_point_3d<double> ray_start, vgl_vector_3d<double> ray_direction, double radius)
{
  ray_direction = normalize(ray_direction);
  double r = radius;
  double H = filter_.descriptor().length();
  double x0 = ray_start.x();
  double y0 = ray_start.y();
  double z0 = ray_start.z();
  double x1 = ray_direction.x();
  double y1 = ray_direction.y();
  double z1 = ray_direction.z();

  double h = H/2;
  // First, we check the special case when the ray is parallel to the cylinder orientation
  if(x1 == 0 && y1 == 0)
  {
    // The ray may be passing through the cylinder
    // If the ray start is over the top tap and the ray direction is downwards OR
    // is below the bottom tap and the ray direction is upwards, then it intersects the cylinder
    if((x0*x0 + y0*y0 <= r*r) && (z0*z1 < 0))
      return H;
    else
      return 0;
  }
  // Coefficients of the equation Au^2 + Bu + C=0
  double A = x1*x1 + y1*y1;
  double B = 2*(x0*x1 + y0*y1);
  double C = x0*x0 + y0*y0 - r*r;
  double delta = B*B - 4*A*C;
  if(delta <= 0)
    return 0;
  else
  {
    double u_1 = (-B + sqrt(delta)) / (2*A);
    double u_2 = (-B - sqrt(delta)) / (2*A);
    // Sort u values
    if(u_1 > u_2)
    {
      double temp = u_1;
      u_1 = u_2;
      u_2 = temp;
    }
    // Handle another special case here where the ray direction is parallel to the cylinder taps
    if(z1 == 0)
    {
      if(vcl_abs(z0) > h)
        return 0;
      else
      {
        // Intersection points
        vgl_point_3d<double> p1 = ray_start + u_1 * ray_direction;
        vgl_point_3d<double> p2 = ray_start + u_2 * ray_direction;
        vgl_vector_3d<double> p = p1 - p2;
        return p.length();
      }
    }
    // Find the u values of taps and sort them, note that z1=0 case already handled above
    double u_tap_1 = (h - z0) / z1;
    double u_tap_2 = (-h - z0) / z1;
    if (u_tap_1 > u_tap_2)
    {
      double temp = u_tap_1;
      u_tap_1 = u_tap_2;
      u_tap_2 = temp;
    }
    // Check for different cases of intersection
    if(u_1 < u_tap_1)
    {
      if(u_2 < u_tap_1) // No intersection
        return 0;
      else if(u_2 <= u_tap_2) // Update small u
        u_1 = u_tap_1;
      else //Update both
      {
        u_1 = u_tap_1;
        u_2 = u_tap_2;
      }
    }
    else if(u_1 <= u_tap_2)
    {
      // u_2 < u_tap_1 is not a case
      if(u_2 <= u_tap_2) // Change nothing
        assert (1);
      else //Update big u
        u_2 = u_tap_2;
    }
    else // No intersection, i.e. u_1 > u_tap_2
    {
      // u_2 < u_tap_1 is not a case
      // u_2 <= u_tap_2 is not a case
      return 0;
    }
    // Intersection points
    vgl_point_3d<double> p1 = ray_start + u_1 * ray_direction;
    vgl_point_3d<double> p2 = ray_start + u_2 * ray_direction;
    vgl_vector_3d<double> p = p1 - p2;
    return p.length();
  }
}
#endif //just for testing

#define XMVG_PARALLEL_BEAM_FILTER_3D_INSTANTIATE_ALL_ARGS(Filter, T) \
  template class xmvg_parallel_beam_filter_3d<Filter, T >; \
  template void x_write(vcl_ostream& , xmvg_parallel_beam_filter_3d<Filter, T>)

#define XMVG_PARALLEL_BEAM_FILTER_3D_INSTANTIATE(Filter) \
  template class xmvg_parallel_beam_filter_3d<Filter >; \
  template void x_write(vcl_ostream& , xmvg_parallel_beam_filter_3d<Filter>)
#endif
