// This is brcv/rec/dbskr/pro/dbskr_shock_match_storage.cxx

//:
// \file

#include <dbskr/pro/dbskr_shock_match_storage.h>

//: Constructor
dbskr_shock_match_storage::dbskr_shock_match_storage() 
{
}

//: Destructor
dbskr_shock_match_storage::~dbskr_shock_match_storage() 
{
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbskr_shock_match_storage::clone() const
{
  return new dbskr_shock_match_storage(*this);
}



