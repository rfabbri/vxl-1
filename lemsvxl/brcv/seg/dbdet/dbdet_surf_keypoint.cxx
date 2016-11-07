// This is brcv/seg/dbdet/dbdet_surf_keypoint.cxx
//:
// \file

#include "dbdet_surf_keypoint.h"
#include <vnl/io/vnl_io_vector.h>
//#include <surf/surflib.h>
//#include <surf/image.h>

#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>




//: Binary save self to stream.
void
dbdet_surf_keypoint::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  dbdet_keypoint::b_write(os);
  vsl_b_write(os, scale_);
  vsl_b_write(os, orientation_);
  vsl_b_write(os, descriptor_);
}


//: Binary load self from stream.
void
dbdet_surf_keypoint::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    dbdet_keypoint::b_read(is);
    vsl_b_read(is, scale_);
    vsl_b_read(is, orientation_);
    vsl_b_read(is, descriptor_);
    break;

  default:
    vcl_cerr << "I/O ERROR: dbdet_surf_keypoint::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short
dbdet_surf_keypoint::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
dbdet_keypoint*
dbdet_surf_keypoint::clone() const
{
  return new dbdet_surf_keypoint(*this);
}


//: Print an ascii summary to the stream
void
dbdet_surf_keypoint::print_summary(vcl_ostream &os) const
{
  os << "dbdet_surf_keypoint("<<x()<<", "<<y()<<", "<<scale_<<", "<<orientation_<<")";
}

//=============================================================================

vcl_istream& operator >> (vcl_istream& is, dbdet_surf_keypoint& s)
{
  double x,y;
  int sgn;
  is >> x >> y >> s.scale_ >> s.orientation_ >> sgn;
  s.sign_ = sgn>0;
  s.set(x,y);
  s.descriptor_.set_size(64);
  for(unsigned i=0; i<64; ++i)
    is >> s.descriptor_[i];
  return is;
}
