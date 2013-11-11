// This is brcv/rec/dbskr/vis/dbskr_shock_patch_match_displayer.cxx

//:
// \file

#include "dbskr_shock_patch_match_displayer.h"
#include "dbskr_shock_patch_match_tableau.h"

#include <dbskr/pro/dbskr_shock_patch_match_storage.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <bgui/bgui_image_tableau.h>

//: Create a tableau if the storage object is of type shock
vgui_tableau_sptr
dbskr_shock_patch_match_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an shock storage object
  dbskr_shock_patch_match_storage_sptr shock_storage;
  shock_storage.vertical_cast(storage);

  // Create match tableau
  dbskr_shock_patch_match_tableau_sptr left = dbskr_shock_patch_match_tableau_new();
  
  if (shock_storage->get_match()) {
    left->set_match(shock_storage->get_match());
    left->set_display_curve_match(false);
  } else if (shock_storage->get_curve_match()) {
    left->set_curve_match(shock_storage->get_curve_match());
    left->set_display_curve_match(true);
    left->set_display_shocks(false);
  } else 
    vcl_cout << "None of the matches (neither curve nor edit matches) are available in the storage!!\n";

  left->set_display_left(true);
  left->set_display_right(true);
  left->set_offset_x2(shock_storage->get_image1()->ni());
  left->set_offset_y2(0);
  left->set_offset_x3(shock_storage->get_image1()->ni());
  left->set_offset_y3(shock_storage->get_image1()->nj()>shock_storage->get_image2()->nj()?shock_storage->get_image1()->nj():shock_storage->get_image2()->nj());
  
  return left;
}
/*
//: Create a tableau if the storage object is of type shock
vgui_tableau_sptr
dbskr_shock_patch_match_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an shock storage object
  dbskr_shock_patch_match_storage_sptr shock_storage;
  shock_storage.vertical_cast(storage);

  // Create a new grid tableau
  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(2, 1);
  grid->set_grid_size_changeable(false);

  // Create two new match tableaus
  dbskr_shock_patch_match_tableau_sptr left = dbskr_shock_patch_match_tableau_new();
  left->set_match(shock_storage->get_match());
  left->set_display_left(true);
  left->set_display_right(false);
  vgui_viewer2D_tableau_sptr vtab_left = vgui_viewer2D_tableau_new(left);

  dbskr_shock_patch_match_tableau_sptr right = dbskr_shock_patch_match_tableau_new();
  right->set_match(shock_storage->get_match());
  right->set_display_left(false);
  right->set_display_right(true);
  vgui_viewer2D_tableau_sptr vtab_right = vgui_viewer2D_tableau_new(right);

  //: add the images
  bgui_image_tableau_sptr itab1 = bgui_image_tableau_new(shock_storage->get_image1());
  //vgui_viewer2D_tableau_sptr vtab1 = vgui_viewer2D_tableau_new(itab1);
  //grid->add_at(vtab1, 0, 0);
  //left->add_child(vtab1);
  
  bgui_image_tableau_sptr itab2 = bgui_image_tableau_new(shock_storage->get_image2());
  //vgui_viewer2D_tableau_sptr vtab2 = vgui_viewer2D_tableau_new(itab2);
  //grid->add_at(vtab2, 1, 0);
  //right->add_child(vtab2);

  /*bgui_selector_tableau_sptr selector_tab = bgui_selector_tableau_new();
  bvis1_view_tableau_sptr view_tab = bvis1_view_tableau_new(selector_tab, offset, absolute);
  view_tabs_.push_back( view_tab );
  grid_tab_->add_next(vgui_viewer2D_tableau_new(view_tab));
  */
/*
  //vtab_left->add_child(itab1);
  //vtab_right->add_child(itab2);

  //: add the match tableaus
  grid->add_at(vtab_left, 0, 0);
  grid->add_at(vtab_right, 1, 0);

  //grid->add_at(vtab1, 0, 0);
  //grid->add_at(vtab2, 1, 0);

  return grid;
}
/*
//: Update a tableau from a storage object
bool 
dbskr_shock_patch_match_displayer::update_tableau( vgui_tableau_sptr tab, 
                                                    const bpro1_storage_sptr& storage ) const
{
  if( storage->type() != this->type() )
    return false;
    
  vgui_grid_tableau_sptr grid = dynamic_cast<vgui_grid_tableau*>(tab.ptr());
  if(!grid)
    return false;
    
  // Cast the storage object into an shock storage object
  dbskr_shock_patch_match_storage_sptr shock_storage;
  shock_storage.vertical_cast(storage);

  if (grid->rows() == 1 && grid->cols() == 2) {
    //grid->remove_at(0, 0);
    //grid->remove_at(1, 0);

    dbskr_shock_patch_match_tableau_sptr left = dbskr_shock_patch_match_tableau_new();
    left->set_match(shock_storage->get_match());
    left->set_display_left(true);
    left->set_display_right(false);
    vgui_viewer2D_tableau_sptr vtab_left = vgui_viewer2D_tableau_new(left);

    dbskr_shock_patch_match_tableau_sptr right = dbskr_shock_patch_match_tableau_new();
    right->set_match(shock_storage->get_match());
    right->set_display_left(false);
    right->set_display_right(true);
    vgui_viewer2D_tableau_sptr vtab_right = vgui_viewer2D_tableau_new(right);

    grid->add_at(vtab_left, 0, 0);
    grid->add_at(vtab_right, 1, 0);
    
    return true;
  } else
    return false;
}

*/
