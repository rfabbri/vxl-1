
#include "dbrl_match_set_displayer.h"
#include "dbrl_match_set_tableau.h"
#include <pro/dbrl_match_set_storage.h>
#include <pro/dbrl_match_set_storage_sptr.h>

#include <georegister/dbrl_match_set_sptr.h>


//: Create a tableau if the storage object is of type dbbgm_distribution_image
vgui_tableau_sptr
dbrl_match_set_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a distribution_image storage object
  dbrl_match_set_storage_sptr dmatchset_storage;
  dmatchset_storage.vertical_cast(storage);
 
  return dbrl_match_set_tableau_new(dmatchset_storage->matchset());
}
