/*
 * elbow_levelset_function_storage.cxx
 *
 *  Created on: Feb 24, 2012
 *      Author: firat
 */

#include "elbow_levelset_function_storage.h"


//------------------------------------------------------------------------------
//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* elbow_levelset_function_storage::
clone() const {
  return new elbow_levelset_function_storage(*this);
}


//------------------------------------------------------------------------------
//: Return IO version number;
short elbow_levelset_function_storage::
version() const {
  return 1;
}


//------------------------------------------------------------------------------
//: Binary save self to stream.
void elbow_levelset_function_storage::
b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  bpro1_storage::b_write(os);
}


//------------------------------------------------------------------------------
//: Binary load self from stream.
void elbow_levelset_function_storage::
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
    vcl_cerr << "I/O ERROR: elbow_levelset_function_storage::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}







