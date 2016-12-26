// This is brcv/rec/dbcvr/vis/dbcvr_curvematch_displayer.cxx

#include <dbcvr/vis/dbcvr_curvematch_displayer.h>
#include <dbcvr/pro/dbcvr_curvematch_storage.h>
#include <dbcvr/vis/dbcvr_curvematch_tableau.h>

//: Create a tableau if the storage object is of type curvematch
vgui_tableau_sptr
dbcvr_curvematch_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an curvematch storage object
  dbcvr_curvematch_storage_sptr curvematch_storage;
  curvematch_storage.vertical_cast(storage);

  // Create a new image tableau
  dbcvr_curvematch_tableau_sptr curvematch_tab = dbcvr_curvematch_tableau_new();
  curvematch_tab->set_curvematch( curvematch_storage->get_curvematch() );
  curvematch_tab->set_curve_cor( curvematch_storage->get_curve_cor() );

  return curvematch_tab;
}
