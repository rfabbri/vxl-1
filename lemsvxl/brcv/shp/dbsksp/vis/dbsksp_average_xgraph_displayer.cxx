// This is dbsksp/vis/dbsksp_average_xgraph_displayer.cxx

//:
// \file


#include "dbsksp_average_xgraph_displayer.h"

#include <dbsksp/pro/dbsksp_average_xgraph_storage.h>
#include <dbsksp/vis/dbsksp_xgraph_tableau.h>
#include <dbsksp/vis/dbsksp_soview_shock.h>




//: Create a tableau if the storage object is of type dbsksp_skgraph
vgui_tableau_sptr dbsksp_average_xgraph_displayer::
make_tableau( bpro1_storage_sptr storage) const
{
  // Return 0 tableau if the types don't match
  if( storage->type() != this->type())
    return 0;

  // Cast the storage object into a dbsksp_skgraph storage object
  dbsksp_xgraph_storage_sptr xgraph_storage;
  xgraph_storage.vertical_cast(storage);

  // create a new dbsksp_skgraph tableau
  dbsksp_xgraph_tableau_new shock_tab;
  shock_tab->set_shock_graph(xgraph_storage->xgraph());

  // Display elements of xshock graph
  shock_tab->add_xgraph_elms_to_display();
     
  shock_tab->set_current_grouping( "default" );
  return shock_tab;
}

