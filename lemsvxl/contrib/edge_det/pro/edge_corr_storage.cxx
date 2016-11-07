// This is pro/edge_corr_storage.h

//:
// \file

#include "edge_corr_storage.h"

//: Return IO version number;
short
edge_corr_storage::version() const
{
  return 1;
}


//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
edge_corr_storage::clone() const
{
  return new edge_corr_storage(*this);
}

