#include "dvpgl_io_proj_camera.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vgl/io/vgl_io_point_2d.h>
#include <vgl/io/vgl_io_point_3d.h>
#include <vsl/vsl_binary_loader.h>


//: Allows derived class to be loaded by base-class pointer.
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr);".  This loads derived class
//  objects from the stream, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
//template <class T>
//void vsl_add_to_binary_loader(vpgl_proj_camera<T> const& b)
//{
//  vsl_binary_loader<vpgl_proj_camera<T> >::instance().add(b);
//}

// dvpgl_calibration_matrix I/O -----------------------------------------------

template <class T> void 
b_read_dvpgl(vsl_b_istream &is, vpgl_calibration_matrix<T>* self)
{
  if (!is) return;
  assert(self);

  vgl_point_2d<T> pp;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
     T tmp;
     vsl_b_read(is, tmp);
     self->set_focal_length(tmp);

     vsl_b_read(is, pp);
     self->set_principal_point(pp);

     vsl_b_read(is, tmp);
     self->set_x_scale(tmp);
     vsl_b_read(is, tmp);
     self->set_y_scale(tmp);
     vsl_b_read(is, tmp);
     self->set_skew(tmp);
    break;
   default:
    vcl_cerr << "I/O ERROR: vpgl_calibration_matrix::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Binary save self to stream.
// \remark cached_svd_ not written
template <class T> void b_write_dvpgl(vsl_b_ostream &os, vpgl_calibration_matrix<T>* self)
{
  vsl_b_write(os, 1);  // matches latest in b_read_dvpgl
  vsl_b_write(os, self->focal_length_);
  vsl_b_write(os, self->principal_point_);
  vsl_b_write(os, self->x_scale_);
  vsl_b_write(os, self->y_scale_);
  vsl_b_write(os, self->skew_);
}

//: Binary save
template <class T> void
vsl_b_write_dvpgl(vsl_b_ostream &os, const vpgl_calibration_matrix<T> * p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    b_write_dvpgl(os, p);
  }
}


//: Binary load
template <class T> void
vsl_b_read_dvpgl(vsl_b_istream &is, vpgl_calibration_matrix<T>* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vpgl_calibration_matrix<T>();
    b_read_dvpgl(is, p);
  }
  else
    p = 0;
}

// dvpgl_perspective_camera I/O -----------------------------------------------

template <class T> void 
b_read_dvpgl(vsl_b_istream &is, vpgl_perspective_camera<T>* self)
{
  if (!is) return;

  vnl_matrix_fixed<T,4,4> Rot;
  vgl_rotation_3d<T> vglRot;
  vnl_vector_fixed<T,4> q;
  vpgl_calibration_matrix<T> K;
  vgl_point_3d<T> camera_center;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
     vcl_cerr << "[dvpgl_io] warning: camera i/o version 1 might also currently be the new vpgl I/O\n"; 
     // vpgl_proj_camera<T>::b_read(is);
     vsl_b_read(is, * (static_cast<vpgl_proj_camera<T> *>(self)));
     b_read_dvpgl(is, &K); // K.b_read(is);
     self->set_calibration(K);
     vsl_b_read(is, camera_center);
     self->set_camera_center(camera_center);
     vsl_b_read(is, Rot);
     self->set_rotation(vgl_rotation_3d<T>(vgl_h_matrix_3d<T>(Rot)));
     // TODO: if error, then try to load new core/vpgl/io style
    break;
   case 2:
     //vpgl_proj_camera<T>::b_read(is);
     vsl_b_read(is, *static_cast<vpgl_proj_camera<T> *>(self));
     b_read_dvpgl(is, &K); // K.b_read(is);
     self->set_calibration(K);
     vsl_b_read(is, camera_center);
     self->set_camera_center(camera_center);
     vsl_b_read(is, q);
     self->set_rotation(vgl_rotation_3d<T>(vnl_quaternion<T>(q)));
    break;
   default:
    vcl_cerr << "I/O ERROR: vpgl_persperctive_camera::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: Binary save self to stream.
// \remark cached_svd_ not written
template <class T> void
b_write_dvpgl(vsl_b_ostream &os, vpgl_perspective_camera<T>* self)
{
  vsl_b_write(os, 2); // matches version 2 in b_read_dvpgl
  b_write_dvpgl(os, static_cast<vpgl_proj_camera<T> *>(self));
  b_write_dvpgl(os, self->get_calibration()); // K.b_read(is);
  vsl_b_write(os, self->get_camera_center());
  vsl_b_write(os, static_cast<vnl_vector_fixed<T,4> >(self->get_rotation().as_quaternion()));
}

//: Binary save
template <class T> void
vsl_b_write_dvpgl(vsl_b_ostream &os, const vpgl_perspective_camera<T> * p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    b_write_dvpgl(os, p);
  }
}


//: Binary load
template <class T> void
vsl_b_read_dvpgl(vsl_b_istream &is, vpgl_perspective_camera<T>* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    p = new vpgl_perspective_camera<T>();
    b_read_dvpgl(is, p);
  }
  else
    p = 0;
}


#undef DVPGL_IO_PROJ_CAMERA_INSTANTIATE
#define DVPGL_IO_PROJ_CAMERA_INSTANTIATE(T) \
template void vsl_b_read(vsl_b_istream &is, vpgl_calibration_matrix<T >* &p); \
template void vsl_b_write(vsl_b_ostream &os, const vpgl_calibration_matrix<T > * p); \
template void b_read_dvpgl(vsl_b_istream &is, vpgl_calibration_matrix<T>* tis); \
template void b_write_dvpgl(vsl_b_ostream &os, const vpgl_calibration_matrix<T>* tis); \
template void vsl_b_read(vsl_b_istream &is, vpgl_perspective_camera<T >* &p); \
template void vsl_b_write(vsl_b_ostream &os, const vpgl_perspective_camera<T > * p); \
template void b_read_dvpgl(vsl_b_istream &is, vpgl_perspective_camera<T>* tis); \
template void b_write_dvpgl(vsl_b_ostream &os, const vpgl_perspective_camera<T>* tis);
  
//template void vsl_add_to_binary_loader(vpgl_proj_camera<T > const& b);
