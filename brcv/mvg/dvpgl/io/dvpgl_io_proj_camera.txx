#include "dvpgl_io_proj_camera.h"


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


// helper struct holding
struct dvpgl_perspective_camera {
};

//-------------------------------
template <class T> void 
b_read_dvpgl(vsl_b_istream &is, vpgl_perspective_camera<T>* &self);
{
  if (!is) return;

  vnl_matrix_fixed<T,4,4> Rot;
  vgl_rotation_3D<T> vglRot;
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
     b_read_dvpgl(is, static_cast<vpgl_proj_camera<T> *>(self));
     K.b_read(is);
     self->set_calibration(K);
     vsl_b_read(is, camera_center);
     self->set_camera_center(camera_center);
     vsl_b_read(is, Rot);
     self->R_ = vgl_rotation_3d<T>(vgl_h_matrix_3d<T>(Rot));
     // TODO: if error, then try to load new core/vpgl/io style
    break;
   case 2:
     //vpgl_proj_camera<T>::b_read(is);
     b_read_dvpgl(is, static_cast<vpgl_proj_camera<T> *>(self));
     self->K_.b_read(is);
     vsl_b_read(is, self->camera_center_);
     vsl_b_read(is, self->q);
     self->R_ = vgl_rotation_3d<T>(vnl_quaternion<T>(self->q));
    break;
   default:
    vcl_cerr << "I/O ERROR: vpgl_persperctive_camera::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
  this->recompute_matrix();
}

//-------------------------------
//: Binary save self to stream.
// \remark cached_svd_ not written
template <class T> void vpgl_perspective_camera<T>::
b_write_dvpgl(vsl_b_ostream &os) const
{
  vsl_b_write(os, this->version());
  vpgl_proj_camera<T>::b_write(os);
  K_.b_write(os);
  vsl_b_write(os, camera_center_);
  vsl_b_write(os, static_cast<vnl_vector_fixed<T,4> >(R_.as_quaternion()));
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
    p->b_write(os);
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
    p->b_read(is);
  }
  else
    p = 0;
}

#undef DVPGL_IO_PROJ_CAMERA_INSTANTIATE
#define DVPGL_IO_PROJ_CAMERA_INSTANTIATE(T) // \
//template void vsl_add_to_binary_loader(vpgl_proj_camera<T > const& b);
