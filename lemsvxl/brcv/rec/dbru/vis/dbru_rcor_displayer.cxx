// This is brcv/rec/dbru/vis/dbru_rcor_displayer.cxx

//:
// \file

#include "dbru_rcor_displayer.h"
#include "dbru_rcor_tableau.h"

#include <dbru/pro/dbru_rcor_storage.h>

//: Create a tableau for the dbru_rcor_storage
vgui_tableau_sptr
dbru_rcor_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a rcor storage object
  dbru_rcor_storage_sptr rcor_storage;
  rcor_storage.vertical_cast(storage);

  // Create a new rcor tableau
  dbru_rcor_tableau_sptr rcor_tab = dbru_rcor_tableau_new();

  //set the region correspondence data structure
  rcor_tab->set_rcor( rcor_storage->get_rcor());

  return rcor_tab;
}
