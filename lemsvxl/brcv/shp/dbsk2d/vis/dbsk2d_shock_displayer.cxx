// This is brcv/shp/dbsk2d/vis/dbsk2d_shock_displayer.cxx

//:
// \file

#include "dbsk2d_shock_displayer.h"
#include "dbsk2d_shock_tableau.h"

#include <dbsk2d/pro/dbsk2d_shock_storage.h>

//: Create a tableau if the storage object is of type shock
vgui_tableau_sptr
dbsk2d_shock_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an shock storage object
  dbsk2d_shock_storage_sptr shock_storage;
  shock_storage.vertical_cast(storage);

  // Create a new shock tableau
  dbsk2d_shock_tableau_sptr shock_tab = dbsk2d_shock_tableau_new();
  //set the boundary, instrinsic shock graph and the coarse shock graph
  shock_tab->set_boundary( shock_storage->get_boundary());
  shock_tab->set_ishock_graph( shock_storage->get_ishock_graph());
  shock_tab->set_shock_graph( shock_storage->get_shock_graph());
  shock_tab->set_rich_map( shock_storage->get_rich_map());

  return shock_tab;
}
