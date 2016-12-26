// This is brcv/mvg/dbvrl/pro/dbvrl_region_storage.h

//:
// \file

#include "dbvrl_region_storage.h"
#include <dbvrl/dbvrl_region.h>
#include <vbl/io/vbl_io_smart_ptr.h>


//: Constructor
dbvrl_region_storage::dbvrl_region_storage( const dbvrl_region_sptr& region,
                                          const dbvrl_transform_2d_sptr& xform )
 : region_(region), xform_(xform)
{
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
dbvrl_region_storage::clone() const
{
  return new dbvrl_region_storage(*this);
}


//: Set the region 
void 
dbvrl_region_storage::set_region( const dbvrl_region_sptr& region )
{
  region_ = region;
}


//: Return the region
dbvrl_region_sptr 
dbvrl_region_storage::region() const
{
  return region_;
}


//: Set the transform 
void 
dbvrl_region_storage::set_transform( const dbvrl_transform_2d_sptr& xform )
{
  xform_ = xform;
}


//: Return the transform
dbvrl_transform_2d_sptr 
dbvrl_region_storage::transform() const
{
  return xform_;
}


//: Return IO version number;
short 
dbvrl_region_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
dbvrl_region_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  vsl_b_write(os, region_);
  vsl_b_write(os, xform_);
}


//: Binary load self from stream.
void 
dbvrl_region_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  {
    bpro1_storage::b_read(is);
    vsl_b_read(is, region_);
    vsl_b_read(is, xform_);
    break;
  }

  default:
    vcl_cerr << "I/O ERROR: dbvrl_region_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
