// This is breye1/dvidpro1/storage/vidpro1_shockshape_storage.h

//:
// \file

#include "vidpro1_shockshape_storage.h"
#include <vbl/io/vbl_io_smart_ptr.h>
#include <shockshape/shockshape.h>
#include <vsol/vsol_polyline_2d.h>

//: Constructor
vidpro1_shockshape_storage::
vidpro1_shockshape_storage(){
}

//: Constructor
vidpro1_shockshape_storage::
vidpro1_shockshape_storage( const shockshape_sptr& shockshape): shockshape_(shockshape)
{
//  shock_line = new vsol_polyline_2d();
//  bnd_right_line = new vsol_polyline_2d();
//  bnd_left_line = new vsol_polyline_2d();
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* vidpro1_shockshape_storage::
clone() const
{
  return new vidpro1_shockshape_storage(*this);
}


//: Return IO version number;
short 
vidpro1_shockshape_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
vidpro1_shockshape_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  /*vsl_b_write(os, region_);
  vsl_b_write(os, xform_);*/
}


//: Binary load self from stream.
void 
vidpro1_shockshape_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    bpro1_storage::b_read(is);
    /*vsl_b_read(is, region_);
    vsl_b_read(is, xform_);*/
    break;
  }

  default:
    vcl_cerr << "I/O ERROR: vidpro1_shockshape_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
