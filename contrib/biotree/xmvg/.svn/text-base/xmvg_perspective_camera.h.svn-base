// This is contrib/biotree/xmvg/xmvg_perspective_camera.h
#ifndef XMVG_PERSPECTIVE_CAMERA_H_
#define XMVG_PERSPECTIVE_CAMERA_H_

//:
// \file
// \brief A class for the x-ray perspective camera model.
// \author H. Can Aras
// \date 02/20/05
// \cauthor Joseph Mundy, Kongbin Kang, Philip Klein
//
//  This class inherits from the vpgl_perspective_camera 
//  Check the descriptions for vpgl_perpective camera for more information
#include <vpgl/vpgl_perspective_camera.h>
#include <xmvg/xmvg_source.h>
#include <vnl/vnl_int_2.h>
#include <xmvg/xmvg_svd.h>
template <class T>
class xmvg_perspective_camera : public vpgl_perspective_camera<T>
{

public:

  //: Default constructor
  xmvg_perspective_camera();

  //: Destructor
  virtual ~xmvg_perspective_camera();

  //: Full constructor with image dimensions
  xmvg_perspective_camera(const vpgl_calibration_matrix<T>& K,
                           const vgl_homg_point_3d<T>& camera_center, 
                           const vgl_h_matrix_3d<T>& R,
                           const xmvg_source& source, 
                           vnl_int_2 dim);

  //: Full constructor
  xmvg_perspective_camera(const vpgl_calibration_matrix<T>& K,
                           const vgl_homg_point_3d<T>& camera_center, 
                           const vgl_h_matrix_3d<T>& R,
                           const xmvg_source& source);

  //: construct from internal and external calibration matrix of the camera
  xmvg_perspective_camera(const vpgl_calibration_matrix<T>& K,
                          const vgl_h_matrix_3d<T>& trans,
                          const xmvg_source& source);

  //: Constructor from a regular perspective camera
  xmvg_perspective_camera(const vpgl_perspective_camera<T>& cam,
                           const xmvg_source& source);

  //: Copy of the xray source parameter block
  xmvg_source source() const{return source_;}

  //: Post-multiply xray perspective camera with a 3-d Euclidean transformation
  static xmvg_perspective_camera<T> 
    postmultiply( const xmvg_perspective_camera<T>& in_cam, 
                  const vgl_h_matrix_3d<T>& euclid_trans);

  // Set image dimensions
  void set_img_dim(vnl_int_2 dim){ img_dim_ = dim; };
  // Get image dimensions
  vnl_int_2 get_img_dim(){ return img_dim_; };

  //: Find the 3d ray that goes through the camera center and the provided image point.
  vgl_homg_line_3d_2_points<T> xmvg_backproject( const vgl_homg_point_2d<T>& image_point );

protected:
  xmvg_source source_;
  vnl_int_2 img_dim_;

  //: The svd is cached when first computed and automatically recomputed when the matrix is changed.
  xmvg_svd<T>* get_xmvg_svd() const;
private:
  mutable xmvg_svd<T>* cached_xmvg_svd_;
};




#endif // xmvg_perspective_camera_h_
