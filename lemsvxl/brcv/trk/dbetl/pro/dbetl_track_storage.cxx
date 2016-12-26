// This is brl/dvidpro1/storage/dbetl_track_storage.cxx

//:
// \file

#include "dbetl_track_storage.h"

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
dbetl_track_storage::clone() const
{
  return new dbetl_track_storage(*this);
}


//: Set the tracks
void 
dbetl_track_storage::set_tracks(const vcl_vector<vcl_vector<dbetl_point_track_sptr> >& tracks)
{
  tracks_ = tracks;
}


//: Access the tracks
vcl_vector<vcl_vector<dbetl_point_track_sptr> >
dbetl_track_storage::tracks() const
{
  return tracks_;
}

//: Set the angles
void 
dbetl_track_storage::set_angles(const vcl_vector<double>& angles)
{
  angles_ = angles;
}


//: Access the angles
vcl_vector<double>
dbetl_track_storage::angles() const
{
  return angles_;
}


//: Set the camera
void 
dbetl_track_storage::set_camera(const dbetl_camera_sptr& camera)
{
  camera_ = camera;
}


//: Access the camera
dbetl_camera_sptr 
dbetl_track_storage::camera() const
{
  return camera_;
}
