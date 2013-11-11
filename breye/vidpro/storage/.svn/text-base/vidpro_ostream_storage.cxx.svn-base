// This is brl/vidpro/storage/vidpro_ostream_storage.cxx

//:
// \file

#include "vidpro_ostream_storage.h"
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>


//: Constructor
vidpro_ostream_storage::vidpro_ostream_storage()
{
}


//: Destructor
vidpro_ostream_storage::~vidpro_ostream_storage()
{
}


//: Return IO version number;
short 
vidpro_ostream_storage::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro_storage* 
vidpro_ostream_storage::clone() const
{
  return new vidpro_ostream_storage(*this);
}


//: Binary save self to stream.
void 
vidpro_ostream_storage::b_write(vsl_b_ostream &os) const
{

}


//: Binary load self from stream.
void 
vidpro_ostream_storage::b_read(vsl_b_ostream &is)
{

}



//: Return a smart pointer to the image resource
vidl_ostream_sptr
vidpro_ostream_storage::get_ostream()
{
  return ostream_;
}


//: Store the image in the storage class
void
vidpro_ostream_storage::set_ostream( const vidl_ostream_sptr& ostream )
{
  ostream_ = ostream;
}

