#ifndef dvpgl_io_proj_camera_h_
#define dvpgl_io_proj_camera_h_
//:
// \file
// \brief the dvpgl development utilities for binary I/O
// \author Ricardo Fabbri
// \date  April 2 2015
//
#include <vsl/vsl_binary_io.h>
#include <vpgl/vpgl_proj_camera.h>



//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr)".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
template <class T>
void vsl_add_to_binary_loader(vpgl_proj_camera<T> const& b);


#endif

