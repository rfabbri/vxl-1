// This is brcv/rec/dbkpr/pro/dbkpr_corr3d_storage.cxx

//:
// \file

#include "dbkpr_corr3d_storage.h"
#include <dbdet/dbdet_keypoint.h>
#include <dbdet/dbdet_lowe_keypoint.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/io/vnl_io_matrix_fixed.h>

#include <vpgl/vpgl_perspective_camera.h>



//: Constructor
dbkpr_corr3d_storage::dbkpr_corr3d_storage( const vcl_vector< dbdet_keypoint_corr3d_sptr >& corr,
                                                  int ni, int nj)
 : keypoints_3d_(corr), ni_(ni), nj_(nj), tree_(0)
{
}

//: Return IO version number;
short
dbkpr_corr3d_storage::version() const
{
  return 2;
}


//: Binary save self to stream.
void
dbkpr_corr3d_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  dvpgl_camera_storage::b_write(os);
  vsl_b_write(os, keypoints_3d_);
  vsl_b_write(os, ni_);
  vsl_b_write(os, nj_);
  //: Tree is not being written.
  
  // Old:
  //  vsl_b_write(os, version());
  //  bpro1_storage::b_write(os);
  //  vsl_b_write(os, keypoints_3d_);
  //  vsl_b_write(os, camera_.get_matrix());
  //  vsl_b_write(os, ni_);
  //  vsl_b_write(os, nj_);
}

//: Binary load self from stream.
void
dbkpr_corr3d_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    bpro1_storage::b_read(is);
    vsl_b_read(is, keypoints_3d_);
    vnl_matrix_fixed<double,3,4> camera_matrix;
    vsl_b_read(is, camera_matrix);
    vpgl_perspective_camera<double> *pcam;
    pcam = new vpgl_perspective_camera<double>();
    vpgl_perspective_decomposition( camera_matrix, *pcam ); 
    this->set_camera(pcam);
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    tree_ = 0;
    break;
  }
  case 2:
  {
    dvpgl_camera_storage::b_read(is);
    vsl_b_read(is, keypoints_3d_);
    vsl_b_read(is, ni_);
    vsl_b_read(is, nj_);
    tree_ = 0;
    break;
  }

  default:
    vcl_cerr << "I/O ERROR: dbkpr_corr3d_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage*
dbkpr_corr3d_storage::clone() const
{
  return new dbkpr_corr3d_storage(*this);
}


//: Set the correspondences storage vector
void
dbkpr_corr3d_storage::set_correspondences( const vcl_vector< dbdet_keypoint_corr3d_sptr >& corr)
{
  keypoints_3d_ = corr;
}

