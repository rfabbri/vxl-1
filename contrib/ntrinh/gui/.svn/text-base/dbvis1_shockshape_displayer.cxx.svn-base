// This is breye1/bvis1/displayer/bvis1_shockshape_displayer.cxx

//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu
// \date 10/30/04


#include "dbvis1_shockshape_displayer.h"
#include "vidpro1_shockshape_storage.h"
#include "dbgui_shockshape_tableau.h"
#include <bvis1/bvis1_manager.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>


//: Create a tableau if the storage object is of type shockshape
vgui_tableau_sptr
dbvis1_shockshape_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a shockshape storage object
  vidpro1_shockshape_storage_sptr shockshape_storage;
  shockshape_storage.vertical_cast(storage);

  // create a new shockshape tableau
  dbgui_shockshape_tableau_new shockshape_tab;
  
  //vcl_vector<vsol_spatial_object_2d_sptr > shock_pts = shockshape_storage->shock_line;
  //shockshape_tab->add_spatial_objects(shock_pts);
  
  return shockshape_tab;
}
