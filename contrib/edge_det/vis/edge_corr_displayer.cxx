// This is vis/edge_corr_displayer.cxx

//:
// \file

#include <vcl_vector.h>
#include <vcl_iostream.h>

#include "edge_corr_displayer.h"
#include <edge_det/pro/edge_corr_storage.h>

#include <edge_det/vis/edge_corr_tableau.h>
#include <edge_det/vis/edge_corr_tableau_sptr.h>


//: Create a tableau if the storage object is of type "edge_map_corr"
vgui_tableau_sptr
edge_corr_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an edgemap storage object
  edge_corr_storage_sptr edgecorr_storage;
  edgecorr_storage.vertical_cast(storage);

  // Create a new edge_corr tableau
  edge_corr_tableau_sptr edgecorr_tab = edge_corr_tableau_new(edgecorr_storage);

  return edgecorr_tab;
}
