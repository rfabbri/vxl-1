// This is brcv/trk/dbinfo/vis/dbinfo_osl_displayer.cxx

#include <dbinfo/vis/dbinfo_osl_displayer.h>
#include <dbinfo/pro/dbinfo_osl_storage.h>
#include <dbinfo/dbinfo_osl.h>
#include <dbinfo/dbinfo_observation.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <bgui/bgui_image_tableau.h>


//: Create a tableau if the storage object is of type dbinfo_osl_storage
vgui_tableau_sptr
dbinfo_osl_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    {
      vcl_cout << "In dbinfo_osl_displayer::make_tableau -"
               << " types don't match\n";
      return NULL;
    }
  // Cast the storage object into dbinfo storage object
  dbinfo_osl_storage_sptr osl_storage;
  osl_storage.vertical_cast(storage);
  // Set up the grid
  unsigned nrows = 7, ncols = 15;
  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(ncols, nrows);
  
    // Extract the osl
  dbinfo_osl_sptr osl = osl_storage->osl();
  if(osl->size()==0)
    return grid;
  vcl_vector<vcl_string> clss = osl->classes();
  unsigned col = 0;
  for(vcl_vector<vcl_string>::iterator cit = clss.begin();
      cit != clss.end()&&col<ncols; ++cit, ++col)
    {
      vcl_vector<dbinfo_observation_sptr> protos;
      if(!osl->prototypes(*cit, protos))
        continue;
      unsigned np = protos.size();
      for(unsigned row = 0; row<np&&row<nrows; ++row)
        {
          vil_image_resource_sptr image = protos[row]->image();
          if(image)
            {
              bgui_image_tableau_sptr itab = bgui_image_tableau_new(image);
              vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
              grid->add_at(vtab, col, row);
            }
        }
    }
  return grid; 
}

