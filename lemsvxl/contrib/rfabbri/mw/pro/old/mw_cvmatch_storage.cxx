#include "mw_cvmatch_storage.h"

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
mw_cvmatch_storage::clone() const
{
  return new mw_cvmatch_storage(*this);
}
