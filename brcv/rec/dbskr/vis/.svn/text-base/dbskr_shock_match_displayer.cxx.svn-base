// This is brcv/rec/dbskr/vis/dbskr_shock_match_displayer.cxx

//:
// \file

#include "dbskr_shock_match_displayer.h"
#include "dbskr_shock_match_tableau.h"

#include <dbskr/pro/dbskr_shock_match_storage.h>

//: Create a tableau if the storage object is of type shock
vgui_tableau_sptr
dbskr_shock_match_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an shock storage object
  dbskr_shock_match_storage_sptr shock_storage;
  shock_storage.vertical_cast(storage);

  // Create a new shock tableau
  dbskr_shock_match_tableau_sptr shock_tab = dbskr_shock_match_tableau_new();
  //set the boundary, instrinsic shock graph and the coarse shock graph
  shock_tab->set_sm_cor(shock_storage->get_sm_cor());

  return shock_tab;
}
