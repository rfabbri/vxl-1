// This is brcv/rec/dber/vis/dber_edge_match_displayer.cxx

//:
// \file

#include "dber_edge_match_displayer.h"
#include "dber_edge_match_tableau.h"

#include <dber/pro/dber_edge_match_storage.h>

//: Create a tableau if the storage object is of type shock
vgui_tableau_sptr
dber_edge_match_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a storage object
  dber_edge_match_storage_sptr edge_storage;
  edge_storage.vertical_cast(storage);

  // Create a new tableau
  dber_edge_match_tableau_sptr edge_tab = dber_edge_match_tableau_new(edge_storage->get_lines1(),
                                                                      edge_storage->get_lines2(),
                                                                      edge_storage->get_assignment());
  edge_tab->set_dominant_dir1(edge_storage->get_dominant_dir1());
  edge_tab->set_dominant_dir2(edge_storage->get_dominant_dir2());

  return edge_tab;
}
