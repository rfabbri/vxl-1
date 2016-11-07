// This is brcv/seg/dbdet/vis/dbdet_sel_displayer.cxx

//:
// \file

#include <vcl_vector.h>
#include <vcl_iostream.h>

#include "dbdet_sel_displayer.h"
#include <dbdet/pro/dbdet_sel_storage.h>

#include <dbdet/vis/dbdet_sel_tableau.h>
#include <dbdet/vis/dbdet_sel_tableau_sptr.h>


//: Create a tableau if the storage object is of type "sel"
vgui_tableau_sptr
dbdet_sel_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a sel storage object
  dbdet_sel_storage_sptr sel_storage;
  sel_storage.vertical_cast(storage);

  // Create a new sel tableau
  dbdet_sel_tableau_sptr sel_tab = dbdet_sel_tableau_new(sel_storage);

  //pass the data structures to this tableau
  //sel_tab->set_data(sel_storage->EM(), sel_storage->CM(), sel_storage->ELG(), sel_storage->CFG(), sel_storage->EULM());

  return sel_tab;
}
