// This is shp/dbsksp/pro/dbsksp_average_xgraph_storage.cxx

//:
// \file

#include "dbsksp_average_xgraph_storage.h"


//------------------------------------------------------------------------------
//: Constructor
dbsksp_average_xgraph_storage::
dbsksp_average_xgraph_storage(): dbsksp_xgraph_storage()
{
}


//------------------------------------------------------------------------------
//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbsksp_average_xgraph_storage::
clone() const {
  return new dbsksp_average_xgraph_storage(*this);
}


//------------------------------------------------------------------------------
//: Return IO version number;
short dbsksp_average_xgraph_storage::
version() const {
  return 1;
}


//------------------------------------------------------------------------------
//: Binary save self to stream.
void dbsksp_average_xgraph_storage::
b_write(vsl_b_ostream &os) const 
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
}


//------------------------------------------------------------------------------
//: Binary load self from stream.
void dbsksp_average_xgraph_storage::
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
    vcl_cerr << "I/O ERROR: dbsksp_average_xgraph_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

