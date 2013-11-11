// This is contrib/biotree/xmvg/xmvg_perspective_camera.txx
#ifndef xmvg_perspective_camera_txx_
#define xmvg_perspective_camera_txx_
//:
// \file
#include <vcl_cassert.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4x4.h>
#include <xmvg/xmvg_perspective_camera.h>


//-------------------------------------------
template <class T>
xmvg_perspective_camera<T>::xmvg_perspective_camera() : 
  vpgl_perspective_camera<T>(), cached_xmvg_svd_(NULL)
{
 
}

template <class T>
xmvg_perspective_camera<T>::~xmvg_perspective_camera()
{
  if( cached_xmvg_svd_ != NULL ) delete cached_xmvg_svd_;
  cached_xmvg_svd_ = NULL;
}

template <class T>
xmvg_perspective_camera<T>::xmvg_perspective_camera( 
  const vpgl_calibration_matrix<T>& K,
  const vgl_homg_point_3d<T>& camera_center, 
  const vgl_h_matrix_3d<T>& R,
  const xmvg_source& source,
  vnl_int_2 dim ) :
  vpgl_perspective_camera<T>(K, camera_center, R), source_(source),
  img_dim_(dim), cached_xmvg_svd_(NULL)
{
}

template <class T>
xmvg_perspective_camera<T>::xmvg_perspective_camera( 
  const vpgl_calibration_matrix<T>& K,
  const vgl_homg_point_3d<T>& camera_center, 
  const vgl_h_matrix_3d<T>& R,
  const xmvg_source& source ) :
  vpgl_perspective_camera<T>(K, camera_center, R), source_(source),
  img_dim_(vnl_int_2(1024, 1024)), cached_xmvg_svd_(NULL)
{
}

// camera center C = -R^{-1} T where T is the translation.
template<class T>
xmvg_perspective_camera<T>::xmvg_perspective_camera(
  const vpgl_calibration_matrix<T> & K,
  const vgl_h_matrix_3d<T> &RT,
  const xmvg_source& source) :
  vpgl_perspective_camera<T>(K, RT.get_inverse().get_translation(), RT.get_upper_3x3()), 
  source_(source), img_dim_(vnl_int_2(1024, 1024)), cached_xmvg_svd_(NULL)

{
}

//: Constructor from a regular perspective camera
template <class T> xmvg_perspective_camera<T>::
xmvg_perspective_camera(const vpgl_perspective_camera<T>& cam,
                        const xmvg_source& source):
vpgl_perspective_camera<T>(cam), source_(source), img_dim_(vnl_int_2(1024, 1024)),
cached_xmvg_svd_(NULL)
{
}

//: Get xmvg_svd
template <class T>
xmvg_svd<T>* xmvg_perspective_camera<T>::get_xmvg_svd() const
{
  // Check if the cached copy is valid, if not recompute it.
  if( cached_xmvg_svd_ == NULL ){  

    cached_xmvg_svd_ = new xmvg_svd<T>(this->get_matrix()); //getting P_

    // Check that the projection matrix isn't degenerate.
    if( cached_xmvg_svd_->rank() != 3 )
      vcl_cerr << "vpgl_proj_camera::svd()\n"
               << "  Warning: Projection matrix is not rank 3, errors may occur.\n";
  }
  return cached_xmvg_svd_;
}

template <class T>
vgl_homg_line_3d_2_points<T> xmvg_perspective_camera<T>::xmvg_backproject( 
  const vgl_homg_point_2d<T>& image_point )
{
  // First find any point in the world that projects to the "image_point".
  vnl_vector_fixed<T,4> vnl_wp = get_xmvg_svd()->xmvg_solve(
    vnl_vector_fixed<T,3>( image_point.x(), image_point.y(), image_point.w() ) );
  vgl_homg_point_3d<T> wp( vnl_wp[0], vnl_wp[1], vnl_wp[2], vnl_wp[3] );

  // The ray is then defined by that point and the camera center.
  return vgl_homg_line_3d_2_points<T>( this->camera_center(), wp );
}

//: Post-multiply xray perspective camera with a 3-d Euclidean transformation
template <class T>
xmvg_perspective_camera<T> 
xmvg_perspective_camera<T>::
postmultiply( const xmvg_perspective_camera<T>& in_cam, 
              const vgl_h_matrix_3d<T>& euclid_trans)
{
  vpgl_perspective_camera<T> temp = 
    vpgl_perspective_camera<T>::postmultiply(in_cam, euclid_trans);
  return xmvg_perspective_camera<T>(temp, in_cam.source());
}

// Code for easy instantiation.
#undef XMVG_PERSPECTIVE_CAMERA_INSTANTIATE
#define XMVG_PERSPECTIVE_CAMERA_INSTANTIATE(T) \
template class xmvg_perspective_camera<T>;

#endif // xmvg_perspective_camera_txx_
