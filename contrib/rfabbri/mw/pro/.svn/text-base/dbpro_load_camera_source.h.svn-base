// This is dbpro_load_camera_source.h
#ifndef dbpro_load_camera_source_h
#define dbpro_load_camera_source_h
//:
//\file
//\brief Source process to load cam from file.
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 08/28/2009 02:59:27 PM PDT
//

#include <vpgl/vpgl_perspective_camera.h>
#include <dbpro/dbpro_process.h>
#include <mw/mw_util.h>

//: Reads a camera file.
//
// \todo support other types of cameras.
template <class T>
class dbpro_load_camera_source : public dbpro_source {
public:

  //: Constructs from a given file name prefix and camera type. This fname_prefix must be
  // something that has the entire file name of the camera, e.g.:
  //
  // /lib/data/frame_0001.camera
  //
  // or 
  //
  // /lib/data/frame_0001.extrinsic
  //
  // The extension is usually ignored and only the prefix
  //
  // /lib/data/frame_0001.camera
  //
  // is used.
  dbpro_load_camera_source(vcl_string fname_prefix, mw_util::camera_file_type type) 
    : name_prefix_(fname_prefix), ftype_(type) {}

  //: Execute the process
  dbpro_signal execute() {
    vpgl_perspective_camera<T> cam;

    bool retval = mw_util::read_cam_anytype(name_prefix_, ftype_, &cam);
    if (!retval)
      return DBPRO_INVALID;

    output(0, cam);
    return DBPRO_VALID;
  }

  vcl_string name_prefix_;
  mw_util::camera_file_type ftype_;
};


#endif // dbpro_load_camera_source_h
