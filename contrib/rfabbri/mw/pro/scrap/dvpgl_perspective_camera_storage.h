// This is vpgl_perspective_camera_storage.h
#ifndef vpgl_perspective_camera_storage_h
#define vpgl_perspective_camera_storage_h
//:
//\file
//\brief Storage for perspective camera
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 05/05/2005 02:57:40 PM EDT
//
#include <bpro1/bpro1_storage.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <mw/pro/vpgl_perspective_camera_storage_sptr.h>

class vpgl_perspective_camera_storage : public bpro1_storage 
{
public:

  vpgl_perspective_camera_storage() {}
  virtual ~vpgl_perspective_camera_storage() {}
  virtual vcl_string type() const { return "vpgl perspective camera"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vpgl_perspective_camera_storage"; }

  const vpgl_perspective_camera<double> &get_camera() {
    return Cam_;
  }
 
  void set_camera(const vpgl_perspective_camera<double> &new_cam) {
    Cam_ = new_cam;
  }
  //: Return IO version number;
  short version() const { return 1; }
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  
protected:

private:

  vpgl_perspective_camera<double> Cam_;
};

//: Create a smart-pointer to a vpgl_perspective_camera_storage
struct vpgl_perspective_camera_storage_new : public vpgl_perspective_camera_storage_sptr
{
  typedef vpgl_perspective_camera_storage_sptr base;

  //: Constructor - creates a default vidpro1_curve_storage_sptr.
  vpgl_perspective_camera_storage_new() : base(new vpgl_perspective_camera_storage()) { }
};

#endif // vpgl_perspective_camera_storage_h

