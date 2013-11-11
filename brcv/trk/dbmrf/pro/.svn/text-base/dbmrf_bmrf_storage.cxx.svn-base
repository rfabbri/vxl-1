// This is brcv/trk/dbmrf/pro/dbmrf_bmrf_storage.h

//:
// \file

#include "dbmrf_bmrf_storage.h"
#include <vbl/io/vbl_io_smart_ptr.h>


//: Constructor
dbmrf_bmrf_storage::dbmrf_bmrf_storage( const bmrf_network_sptr& network )
 : network_(network)
{
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
dbmrf_bmrf_storage::clone() const
{
  return new dbmrf_bmrf_storage(*this);
}


//: Return IO version number;
short 
dbmrf_bmrf_storage::version() const
{
  return 1;
}


//: Binary save self to stream.
void 
dbmrf_bmrf_storage::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
  vsl_b_write(os, this->network_);
}


//: Binary load self from stream.
void 
dbmrf_bmrf_storage::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    bpro1_storage::b_read(is);
    vsl_b_read(is, this->network_);
    break;

  default:
    vcl_cerr << "I/O ERROR: dbmrf_bmrf_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//: Return the network smart pointer
bmrf_network_sptr
dbmrf_bmrf_storage::network() const
{
  return network_;
}
