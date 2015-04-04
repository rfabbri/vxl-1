#ifndef dvpgl_proj_camera_vsl_h_
#define dvpgl_proj_camera_vsl_h_

#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

//: Wrapper class on the new vpgl_perspective_camera to allow for old binary
// I/O
template <class T>
class dvpgl_proj_camera_vsl
{
 public:
  // ----------------- Constructors:----------------------

  //: Default constructor makes an identity camera.
  dvpgl_proj_camera_vsl();

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return vcl_string("vpgl_proj_camera"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(vcl_string const& cls) const
  { return cls==is_a() || cls==vcl_string("vpgl_camera"); }

  //: Return `this' if `this' is a vpgl_proj_camera, 0 otherwise
  // This is used by e.g. polymorphic binary i/o
  virtual vpgl_proj_camera<T> *cast_to_proj_camera() {return this;}
  virtual const vpgl_proj_camera<T> *cast_to_proj_camera() const {return this;}

  //: Return `this' if `this' is a vpgl_perspective_camera, 0 otherwise
  // This is used by e.g. the storage class
  // \todo code for affine camera and other children
  virtual vpgl_perspective_camera<T> *cast_to_perspective_camera() {return 0;}
  virtual const vpgl_perspective_camera<T> *cast_to_perspective_camera() const {return 0;}

  virtual ~dvpgl_proj_camera_vsl();

 private:
  vpgl_proj_camera<t> *cam_;
};



#endif // dvpgl_proj_camera_vsl_h_
