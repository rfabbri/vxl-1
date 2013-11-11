// This is brcv/seg/dbdet/dbdet_keypoint.cxx
//:
// \file

#include "dbdet_keypoint.h"



//: Binary save self to stream.
void
dbdet_keypoint::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->x());
  vsl_b_write(os, this->y());
}


//: Binary load self from stream.
void
dbdet_keypoint::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    vsl_b_read(is, this->x());
    vsl_b_read(is, this->y());
    break;

  default:
    vcl_cerr << "I/O ERROR: dbdet_keypoint::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return IO version number;
short
dbdet_keypoint::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
dbdet_keypoint*
dbdet_keypoint::clone() const
{
  return new dbdet_keypoint(*this);
}


//: Print an ascii summary to the stream
void
dbdet_keypoint::print_summary(vcl_ostream &os) const
{
  os << "dbdet_keypoint("<<x()<<", "<<y()<<")";
}


//==============================================
//: Allows derived class to be loaded by base-class pointer.
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr);".  This loads derived class
//  objects from the stream, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const dbdet_keypoint& k)
{
  vsl_binary_loader<dbdet_keypoint>::instance().add(k);
}


//==============================================
//: Stream summary output for base class pointer
void vsl_print_summary(vcl_ostream& os,const dbdet_keypoint* k)
{
  if (k)
    k->print_summary(os);
  else
    os << "NULL dbdet_keypoint\n";
}
