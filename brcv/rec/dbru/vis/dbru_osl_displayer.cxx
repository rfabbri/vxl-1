// This is brcv/rec/dbru/vis/dbru_osl_displayer.cxx

#include <dbru/vis/dbru_osl_displayer.h>
#include <dbru/pro/dbru_osl_storage.h>
#include <dbru/dbru_osl.h>
#include <dbru/dbru_object.h>
#include <dbinfo/dbinfo_observation.h>
#include <bvis1/bvis1_manager.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <dbru/dbru_multiple_instance_object.h>
#include <vil/vil_new.h>


//: Create a tableau if the storage object is of type dbru_osl_storage
vgui_tableau_sptr
dbru_osl_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    {
      vcl_cout << "In dbru_osl_displayer::make_tableau -"
               << " types don't match\n";
      return NULL;
    }
  // Cast the storage object into dbru storage object
  dbru_osl_storage_sptr osl_storage;
  osl_storage.vertical_cast(storage);
  // Set up the grid
  unsigned nrows = osl_storage->get_nrows(), ncols = osl_storage->get_ncols();
  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(ncols, nrows);
  grid->set_grid_size_changeable(false);
  
    // Extract the osl
  dbru_osl_sptr osl = osl_storage->osl();
  if(osl->n_objects()==0)
    return grid;
  unsigned row = 0;
  vcl_cout << "CAUTION: Prototypes of each object occupies 2 rows:\n";
  vcl_cout << "         first row displays observation images,\n";
  vcl_cout << "         second row displayes middle multiple instance images if an instance is available.\n";

  for ( ; row < nrows && row < 2*osl->n_objects(); row+=2) {
    dbru_object_sptr obj = osl->get_object(row/2);
    vcl_cout << "obj at row: " << row << " ";
    unsigned np = obj->n_polygons();
    
    for (unsigned col = 0, nn = 0; nn < np && col < ncols; nn++) {
      vcl_cout << nn << " ";
      if (obj->get_polygon(nn)->size() == 0) 
        continue;
      
      dbinfo_observation_sptr obs = obj->get_observation(nn);
      if (obs) {
        vil_image_resource_sptr image = obs->image_cropped();
        if(image)
          {
            bgui_image_tableau_sptr itab = bgui_image_tableau_new(image);
            vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
            grid->add_at(vtab, col, row);
          }
      }
      
      dbru_multiple_instance_object_sptr ins = obj->get_instance(nn);
      if (ins) {
        vil_image_resource_sptr image = vil_new_image_resource_of_view(ins->get_image_i(ins->imgs_size()/2));
        //: get image of observation of the middle image to see if polygon of multiple instance is better than foreground polygon
        
        vcl_vector<vsol_point_2d_sptr> new_points;
        vsol_polygon_2d_sptr ins_poly = ins->get_poly();
        vsol_box_2d_sptr box = ins->get_img_box();
        vgl_vector_2d<double> trans(-box->get_min_x(), -box->get_min_y());
        for (unsigned jj = 0; jj < ins_poly->size(); jj++) {
          vsol_point_2d_sptr p = ins_poly->vertex(jj);
          vsol_point_2d_sptr pp = new vsol_point_2d(*p);
          pp->add_vector(trans);
          new_points.push_back(pp);
        }
        vsol_polygon_2d_sptr trans_poly = new vsol_polygon_2d(new_points);
        dbinfo_observation_sptr obs = new dbinfo_observation(0, image, trans_poly, true, false, false);
        vil_image_resource_sptr img = obs->image_cropped();
        if(img)
          {
            //bgui_image_tableau_sptr itab = bgui_image_tableau_new(image);
            bgui_image_tableau_sptr itab = bgui_image_tableau_new(img);
            vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
            grid->add_at(vtab, col, row+1);
          }
      }
      col++;
    
    } 
    vcl_cout << "...done!\n";
  }
    
  return grid; 
}

