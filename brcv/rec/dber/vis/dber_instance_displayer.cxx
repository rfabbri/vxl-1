// This is brcv/rec/dber/vis/dber_instance_displayer.cxx

#include <dber/vis/dber_instance_displayer.h>
#include <dber/pro/dber_instance_storage.h>

#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <bgui/bgui_image_tableau.h>

#include <dbru/dbru_multiple_instance_object.h>

#include <vil/vil_new.h>

//: Create a tableau if the storage object is of type dber_instance_storage
vgui_tableau_sptr
dber_instance_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    {
      vcl_cout << "In dber_instance_displayer::make_tableau -"
               << " types don't match\n";
      return NULL;
    }

  // Cast the storage object into dbru storage object
  dber_instance_storage_sptr ins_storage;
  ins_storage.vertical_cast(storage);
  
  // Set up the grid
  unsigned nrows = ins_storage->get_nrows(), ncols = ins_storage->get_ncols();
  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(ncols, nrows);
  grid->set_grid_size_changeable(false);
  
  // Extract the instances
  vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames = ins_storage->get_frames();
  if(frames.size()==0)
    return grid;
  unsigned col = 0;
  vcl_cout << "adding frames to display: " << col << " ";
  for ( ; col < ncols && col < frames.size(); col++) {
    vcl_cout << "frame at: " << col << " ";
    unsigned np = frames[col].size();
    for (unsigned row = 0; row < np && row < nrows; row++) {
      vcl_cout << row << " ";
      dbru_multiple_instance_object_sptr obs = frames[col][row];
      if (!obs) 
        continue;
      vil_image_resource_sptr image = vil_new_image_resource_of_view(obs->get_image_i(1));
      if(image)
        {
          bgui_image_tableau_sptr itab = bgui_image_tableau_new(image);
          vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
          grid->add_at(vtab, col, row);
        }
    }
    vcl_cout << "...done!\n";
  }
    
  return grid; 
}

