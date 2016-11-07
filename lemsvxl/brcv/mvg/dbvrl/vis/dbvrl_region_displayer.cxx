// This is brcv/mvg/dbvrl/vis/bvis1_dbvrl_displayer.cxx

//:
// \file

#include "dbvrl_region_displayer.h"
#include <dbvrl/pro/dbvrl_region_storage.h>
#include "dbvrl_region_tableau.h"
#include <bvis1/bvis1_manager.h>


//: Create a tableau if the storage object is of type dbvrl
vgui_tableau_sptr
dbvrl_region_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a dbvrl storage object
  dbvrl_region_storage_sptr dbvrl_storage;
  dbvrl_storage.vertical_cast(storage);

  dbvrl_region_tableau_new xform_tab;
  xform_tab->set_region(dbvrl_storage->region());
  xform_tab->set_transform(dbvrl_storage->transform());

  return xform_tab;
}
