// This is brcv/seg/dbdet/pro/dbdet_keypoint_storage.h

//:
// \file

#include "dbdet_keypoint_storage.h"
#include <dbdet/dbdet_keypoint.h>
#include <dbdet/dbdet_lowe_keypoint.h>
#include <vsl/vsl_vector_io.h>


bool dbdet_keypoint_storage::registered_ = false;

//: Constructor
dbdet_keypoint_storage::dbdet_keypoint_storage( const vcl_vector< dbdet_keypoint_sptr >& keypoints,
                                                  int ni, int nj)
 : keypoints_(keypoints), ni_(ni), nj_(nj)
{
}

//: Register vsol_spatial_object_2d types for I/O
void
dbdet_keypoint_storage::register_binary_io() const
{
  if(!registered_){
    vsl_add_to_binary_loader(dbdet_keypoint());
    vsl_add_to_binary_loader(dbdet_lowe_keypoint());

    registered_ = true;
  }
}


//: Return IO version number;
short
dbdet_keypoint_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void
dbdet_keypoint_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  vsl_b_write(os, keypoints_);
  vsl_b_write(os, ni_);
  vsl_b_write(os, nj_);
}


//: Binary load self from stream.
void
dbdet_keypoint_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    bpro1_storage::b_read(is);
    vsl_b_read(is, keypoints_);
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    break;
  }

  default:
    vcl_cerr << "I/O ERROR: dbdet_keypoint_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
dbdet_keypoint_storage::clone() const
{
  return new dbdet_keypoint_storage(*this);
}


//: Set the keypoint storage vector
void
dbdet_keypoint_storage::set_keypoints( const vcl_vector< dbdet_keypoint_sptr >& keypoints)
{
  keypoints_ = keypoints;
}


