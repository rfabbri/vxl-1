// This is brcv/rec/dber/pro/dber_edge_match_storage.cxx

//:
// \file

#include <dber/pro/dber_edge_match_storage.h>
//#include <dbskr/dbskr_sm_cor.h>

//: Constructor
dber_edge_match_storage::dber_edge_match_storage() 
{
}

//: Destructor
dber_edge_match_storage::~dber_edge_match_storage() 
{
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dber_edge_match_storage::clone() const
{
  return new dber_edge_match_storage(*this);
}



