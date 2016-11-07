// This is contrib/ntrinh/en292/pro/vidpro1_homog_2d_storage.h

//:
// \file

#include "vidpro1_homog_2d_storage.h"
#include <vbl/io/vbl_io_smart_ptr.h>


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
vidpro1_homog_2d_storage::clone() const
{
  return new vidpro1_homog_2d_storage(*this);
}



//: Return IO version number;
short 
vidpro1_homog_2d_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
vidpro1_homog_2d_storage::b_write(vsl_b_ostream &os) const
{
  
}


//: Binary load self from stream.
void 
vidpro1_homog_2d_storage::b_read(vsl_b_istream &is)
{
  
}
