// This is brcv/seg/dbdet/dbdet_frenet_keypoint.cxx
//:
// \file
//

#include "dbdet_frenet_keypoint.h"

//: Binary save self to stream.
void
dbdet_frenet_keypoint::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  dbdet_keypoint::b_write(os);
  vsl_b_write(os,tx_);
  vsl_b_write(os,ty_);
  vsl_b_write(os,nx_);
  vsl_b_write(os,ny_ );
  vsl_b_write(os,k_);
  vsl_b_write(os,kdot_);
  vsl_b_write(os,valid_);

}


//: Binary load self from stream.
void
dbdet_frenet_keypoint::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    dbdet_keypoint::b_read(is);
    vsl_b_read(is,tx_);
    vsl_b_read(is,ty_);
    vsl_b_read(is,nx_);
    vsl_b_read(is,ny_ );
    vsl_b_read(is,k_);
    vsl_b_read(is,kdot_);
    vsl_b_read(is,valid_);
    break;

  default:
    vcl_cerr << "I/O ERROR: dbdet_frenet_keypoint::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short
dbdet_frenet_keypoint::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
dbdet_keypoint*
dbdet_frenet_keypoint::clone() const
{
  return new dbdet_frenet_keypoint(*this);
}


//: Print an ascii summary to the stream
void
dbdet_frenet_keypoint::print_summary(vcl_ostream &os) const
{
  os << "dbdet_frenet_keypoint("<<x()<<", "<<y()<<", "<<
    tx_<<", " <<
    ty_<<", " <<
    nx_<<", " <<
    ny_ <<", " <<
    k_<<", " <<
    kdot_ << ", " << valid_;
}

