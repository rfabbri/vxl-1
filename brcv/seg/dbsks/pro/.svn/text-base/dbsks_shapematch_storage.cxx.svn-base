// This is shp/dbsks/pro/dbsks_shapematch_storage.cxx

//:
// \file

#include "dbsks_shapematch_storage.h"

//: Constructor
dbsks_shapematch_storage::
dbsks_shapematch_storage() : dp_engine_(0), lm_engine_(0)
{}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbsks_shapematch_storage::
clone() const {
  return new dbsks_shapematch_storage(*this);
}


//: Return IO version number;
short dbsks_shapematch_storage::
version() const {
  return 1;
}


//: Binary save self to stream.
void dbsks_shapematch_storage::
b_write(vsl_b_ostream &os) const {
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
}


//: Binary load self from stream.
void dbsks_shapematch_storage::
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
    vcl_cerr << "I/O ERROR: dbsks_shapematch_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

