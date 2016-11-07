// This is breye1/dbvis1/displayer/dbctrk_displayer.cxx
//:
// \file

#include "dbctrk_displayer.h"
#include <dbctrk/pro/dbctrk_storage.h>
#include <vcl_vector.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <vgui/vgui_style.h>
#include "dbctrk_soview2d.h"


//: Create a tableau if the storage object is of type dbctrk
vgui_tableau_sptr
dbctrk_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a dbctrk storage object
  dbctrk_storage_sptr dbctrk_storage;
  dbctrk_storage.vertical_cast(storage);

  vcl_vector< dbctrk_tracker_curve_sptr > tracked_curves;
  dbctrk_storage->get_tracked_curves(tracked_curves);
  bgui_vsol2D_tableau_sptr btab= bgui_vsol2D_tableau_new();

  vcl_vector< dbctrk_tracker_curve_sptr >::iterator itr;
  for(itr=tracked_curves.begin();itr!=tracked_curves.end();itr++)
  {

      float r,g,b;

      dbctrk_soview2D* obj=new dbctrk_soview2D(*itr);
      utils::set_changing_colors( (*itr)->match_id_ ,r, g, b );
      vgui_style_sptr sty= vgui_style::new_style(r,g,b,2.0,2.0);
      btab->add( obj );
      obj->set_style(sty);
  }



  return btab;

}

