// This is brcv/rec/dber/pro/dber_instance_storage.cxx

//:
// \file

#include "dber_instance_storage.h"
//#include <dbru/dbru_object.h>
#include <dbru/dbru_osl.h>

//: Constructor
dber_instance_storage::dber_instance_storage() : n_rows_(25), n_cols_(50)
{
}

//: Destructor
dber_instance_storage::~dber_instance_storage() 
{
}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dber_instance_storage::clone() const
{
  return new dber_instance_storage(*this);
}

//: Return IO version number;
short dber_instance_storage::version() const
{
  return 1;
}
