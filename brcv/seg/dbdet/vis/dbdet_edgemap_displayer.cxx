// This is brcv/seg/dbdet/vis/dbdet_edgemap_displayer.cxx

//:
// \file

#include <vcl_vector.h>
#include <vcl_iostream.h>

#include "dbdet_edgemap_displayer.h"
#include <dbdet/pro/dbdet_edgemap_storage.h>

#include <dbdet/vis/dbdet_edgemap_tableau.h>
#include <dbdet/vis/dbdet_edgemap_tableau_sptr.h>


//: Create a tableau if the storage object is of type "edge_map"
vgui_tableau_sptr
dbdet_edgemap_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an edgemap storage object
  dbdet_edgemap_storage_sptr edgemap_storage;
  edgemap_storage.vertical_cast(storage);

  // Create a new sel tableau
  dbdet_edgemap_tableau_sptr edgemap_tab = dbdet_edgemap_tableau_new();

  //pass the edge linker to this tableuau
  edgemap_tab->set_edgemap( edgemap_storage->get_edgemap());

  return edgemap_tab;
}
