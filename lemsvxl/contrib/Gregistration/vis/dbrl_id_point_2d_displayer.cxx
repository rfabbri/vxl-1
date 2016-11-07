
#include "dbrl_id_point_2d_displayer.h"
#include <bgui/bgui_vsol2D_tableau.h>
#include <pro/dbrl_id_point_2d_storage.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <vgui/vgui_style.h>


//: Create a tableau if the storage object is of type 
vgui_tableau_sptr
dbrl_id_point_2d_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a distribution_image storage object
  dbrl_id_point_2d_storage_sptr id_point_storage;
  id_point_storage.vertical_cast(storage);
 
  bgui_vsol2D_tableau_sptr btab= bgui_vsol2D_tableau_new();
  vcl_vector< dbrl_id_point_2d_sptr > idpoints=  id_point_storage->points();

  vcl_vector< dbrl_id_point_2d_sptr >::iterator itr;
  for(itr=idpoints.begin();itr!=idpoints.end();itr++)
    {
        vsol_point_2d_sptr point=new vsol_point_2d((*itr)->x(),(*itr)->y());
        vgui_style_sptr sty= vgui_style::new_style(1.0,0.0,0.0,2.0,2.0);
        btab->add_vsol_point_2d(point,sty);
    }
  return btab;
}


