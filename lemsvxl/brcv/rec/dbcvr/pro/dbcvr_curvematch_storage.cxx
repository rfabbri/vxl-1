/**************************************************************/
/*  Name: MingChing (mcchang)
//  File: vidpro1_curvematch_storage.h
//  Asgn: vidpro
//  Date: Thu Aug 28 14:28:55 EDT 2003
***************************************************************/
#include "dbcvr_curvematch_storage.h"

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* 
dbcvr_curvematch_storage::clone() const
{
  return new dbcvr_curvematch_storage(*this);
}
