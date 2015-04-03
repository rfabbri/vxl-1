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
template <class T>
void vsl_add_to_binary_loader(vpgl_proj_camera<T> const& b)
{
  vsl_binary_loader<vpgl_proj_camera<T> >::instance().add(b);
}

#undef DVPGL_IO_PROJ_CAMERA_INSTANTIATE
#define DVPGL_IO_PROJ_CAMERA_INSTANTIATE(T) \
template void vsl_add_to_binary_loader(vpgl_proj_camera<T > const& b);
