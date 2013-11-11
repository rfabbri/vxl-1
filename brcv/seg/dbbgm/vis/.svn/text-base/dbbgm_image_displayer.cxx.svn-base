// This is brcv/seg/dbbgm/vis/dbbgm_image_displayer.cxx

//:
// \file

#include "dbbgm_image_displayer.h"
#include "dbbgm_image_tableau.h"
#include <dbbgm/pro/dbbgm_image_storage.h>
#include <bbgm/bbgm_image_sptr.h>


//: Create a tableau if the storage object is of type dbbgm_image
vgui_tableau_sptr
dbbgm_image_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a dbbgm_image storage object
  dbbgm_image_storage_sptr dimg_storage;
  dimg_storage.vertical_cast(storage);

  return dbbgm_image_tableau_new(dimg_storage->dist_image());
}


//: Update a tableau from a storage object
bool 
dbbgm_image_displayer::update_tableau( vgui_tableau_sptr tab,
                                       const bpro1_storage_sptr& storage ) const
{
  if( storage->type() != this->type() )
    return false;

  dbbgm_image_tableau_sptr dist_tab = dynamic_cast<dbbgm_image_tableau*>(tab.ptr());
  if(!dist_tab)
    return false;

  dist_tab->update();
  return true;
}
