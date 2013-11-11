// This is brcv/trk/dbmrf/pro/dbmrf_curvel_3d_storage.h

//:
// \file

#include "dbmrf_curvel_3d_storage.h"
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vgl/io/vgl_io_vector_3d.h>
#include <vsl/vsl_set_io.h>


//: Constructor
dbmrf_curvel_3d_storage::dbmrf_curvel_3d_storage()
 : camera_(0.0)
{
  camera_[0][0] = camera_[1][1] = camera_[2][2] = 1.0;
  bb_xform_.set_identity();
}


//: Constructor
dbmrf_curvel_3d_storage::dbmrf_curvel_3d_storage( 
                            const vcl_set<bmrf_curve_3d_sptr>& curves,
                            const vnl_double_3x4& camera,
                            const vgl_vector_3d<double>& dir,
                            const vnl_double_4x4& xform )
 : curvel_3d_(curves),
   camera_(camera),
   direction_(dir),
   bb_xform_(xform)
{
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
dbmrf_curvel_3d_storage::clone() const
{
  return new dbmrf_curvel_3d_storage(*this);
}


//: Set the curvel_3d storage set
void
dbmrf_curvel_3d_storage::set_curvel_3d( const vcl_set<bmrf_curve_3d_sptr>& curves)
{
  curvel_3d_ = curves;
}

//: Retrieve the curvel_3d storage set
void
dbmrf_curvel_3d_storage::get_curvel_3d( vcl_set<bmrf_curve_3d_sptr>& curves ) const
{
  curves = curvel_3d_;
}


//: Set the cameras
void 
dbmrf_curvel_3d_storage::set_camera( const vnl_double_3x4& camera )
{
  camera_ = camera;
}


//: Retrieve the cameras
vnl_double_3x4
dbmrf_curvel_3d_storage::camera() const
{
  return camera_;
}


//: Set the direction of motion
void 
dbmrf_curvel_3d_storage::set_direction( const vgl_vector_3d<double>& dir )
{
  direction_ = dir;
}


//: Retrieve the direction of motion
vgl_vector_3d<double> 
dbmrf_curvel_3d_storage::direction() const
{
  return direction_;
}

  
//: Set the bounding box transform
void 
dbmrf_curvel_3d_storage::set_bb_xform( const vnl_double_4x4& xform )
{
  bb_xform_ = xform;
}


//: Retrieve the bounding box transform
vnl_double_4x4 
dbmrf_curvel_3d_storage::bb_xform() const
{
  return bb_xform_;
}


//: Return IO version number;
short 
dbmrf_curvel_3d_storage::version() const
{
  return 2;
}


//: Binary save self to stream.
void 
dbmrf_curvel_3d_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  vsl_b_write(os, this->curvel_3d_);
  vsl_b_write(os, this->camera_);
  vsl_b_write(os, this->direction_);
  vsl_b_write(os, this->bb_xform_);
}


//: Binary load self from stream.
void 
dbmrf_curvel_3d_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    {
      int frame;
      vcl_string name;
      vsl_b_read(is, frame);
      vsl_b_read(is, name);
      break;
    }
    
  case 2:
    bpro1_storage::b_read(is);
    vsl_b_read(is, this->curvel_3d_);
    vsl_b_read(is, this->camera_);
    vsl_b_read(is, this->direction_);
    vsl_b_read(is, this->bb_xform_);
    break;

  default:
    vcl_cerr << "I/O ERROR: dbmrf_curvel_3d_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
