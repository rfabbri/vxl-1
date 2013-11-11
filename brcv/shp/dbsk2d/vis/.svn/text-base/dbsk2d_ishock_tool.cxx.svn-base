// This is brcv/dbsk2d/vis/dbsk2d_ishock_tool.cxx

//:
// \file

#include "dbsk2d_ishock_tool.h"
#include <bvis1/bvis1_manager.h>

dbsk2d_ishock_tool::dbsk2d_ishock_tool()
{
}

dbsk2d_ishock_tool::~dbsk2d_ishock_tool()
{
}

//: Set the tableau to work with
bool dbsk2d_ishock_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;
    
  tableau_.vertical_cast(tableau);
  if(!tableau_.ptr())
    return false;
    
  if (this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)))
    return true;
  
  tableau_ = 0;
  return false;
}

//: Set the storage class for the active tableau
bool dbsk2d_ishock_tool::set_storage ( const bpro1_storage_sptr& storage)
{
  if(!storage.ptr())
    return false;
    
  //make sure its a ishock storage class
  if (storage->type() == "shock"){
    storage_.vertical_cast(storage);
    return true;
  }

  storage_ = 0;
  return false;
}

//: Return the storage class of the tool
dbsk2d_shock_storage_sptr dbsk2d_ishock_tool::storage()
{
  return storage_;
}

//: Return the tableau of the tool
dbsk2d_shock_tableau_sptr dbsk2d_ishock_tool::tableau()
{
  return tableau_;
}
