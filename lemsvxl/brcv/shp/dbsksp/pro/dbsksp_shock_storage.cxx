// This is shp/dbsksp/pro/dbsksp_shock_storage.cxx

//:
// \file

#include "dbsksp_shock_storage.h"

//: Constructor
dbsksp_shock_storage::
dbsksp_shock_storage():shock_graph_(0), active_edge_(0), active_node_(0)
{}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbsksp_shock_storage::
clone() const {
  return new dbsksp_shock_storage(*this);
}


//: Return IO version number;
short dbsksp_shock_storage::
version() const {
  return 1;
}


//: Binary save self to stream.
void dbsksp_shock_storage::
b_write(vsl_b_ostream &os) const {
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
}


//: Binary load self from stream.
void dbsksp_shock_storage::
b_read(vsl_b_istream &is){
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver){
    case 1:
    {
      bpro1_storage::b_read(is);
      break;
    }
    default:
    vcl_cerr << "I/O ERROR: dbsksp_shock_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

