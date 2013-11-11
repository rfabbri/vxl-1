// This is ozge/face_gui/vis/dbru_facedb_displayer.cxx

#include <ozge/face_gui/vis/dbru_facedb_displayer.h>
#include <ozge/face_gui/pro/dbru_facedb_storage.h>
#include <ozge/face_gui/dbru_facedb.h>
//#include <dbinfo/dbinfo_observation.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <bgui/bgui_image_tableau.h>

//: Create a tableau if the storage object is of type dbru_facedb_storage
vgui_tableau_sptr
dbru_facedb_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    {
      vcl_cout << "In dbru_facedb_displayer::make_tableau -"
               << " types don't match\n";
      return NULL;
    }
  // Cast the storage object into dbru storage object
  dbru_facedb_storage_sptr facedb_storage;
  facedb_storage.vertical_cast(storage);
  // Set up the grid
  unsigned nrows = facedb_storage->get_nrows(), ncols = facedb_storage->get_ncols();
  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(ncols, nrows);
  grid->set_grid_size_changeable(false);
  
    // Extract the facedb
  dbru_facedb_sptr facedb = facedb_storage->facedb();
  if(facedb->n_subjects()==0)
    return grid;
  unsigned row = 0;
  vcl_cout << "adding subjects to display: " << row << " ";
  for ( ; row < nrows && row < facedb->n_subjects(); row++) {
    vcl_vector<vil_image_resource_sptr>& subj = facedb->get_subject(row);
    vcl_cout << "subject at row: " << row << " ";
    unsigned np = subj.size();
    for (unsigned col = 0; col < np && col < ncols; col++) {
      vcl_cout << col << " ";
      vil_image_resource_sptr fimg = subj[col];
      if (!fimg) 
        continue;
      else
        {
          bgui_image_tableau_sptr itab = bgui_image_tableau_new(fimg);
          vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
          grid->add_at(vtab, col, row);
        }
    }
    vcl_cout << "...done!\n";
  }
    
  return grid; 
}

