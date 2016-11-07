// This is brcv/seg/dbdet/vis/dbdet_temporal_map_displayer.cxx

//:
// \file

#include <vcl_vector.h>
#include <vcl_iostream.h>

#include "dbdet_temporal_map_displayer.h"
#include <dbdet/pro/dbdet_sel_storage.h>


#include <Spatemp/pro/dbdet_temporal_map_storage_sptr.h>
#include <Spatemp/vis/dbdet_temporal_map_tableau_sptr.h>
#include <Spatemp/vis/dbdet_temporal_map_tableau.h>


//: Create a tableau if the storage object is of type "temporalmap"
vgui_tableau_sptr
dbdet_temporal_map_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a temporal map  storage object
  dbdet_temporal_map_storage_sptr tmap_storage;
  tmap_storage.vertical_cast(storage);

  // Create a new sel tableau
  dbdet_temporal_map_tableau_sptr tmap_tab = dbdet_temporal_map_tableau_new(tmap_storage);

  return tmap_tab;
}
