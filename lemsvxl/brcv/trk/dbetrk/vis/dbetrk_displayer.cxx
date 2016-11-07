// This is breye1/dbetrk/vis/dbvis1_edgetrk_displayer.cxx
//:
// \file

#include <dbetrk/vis/dbetrk_displayer.h>
#include <dbetrk/pro/dbetrk_storage.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vcl_vector.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <dbctrk/dbctrk_algs.h>
#include <bvis1/bvis1_manager.h>
#include <dbctrk/dbctrk_utils.h>
#include <vcl_iostream.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_style.h>
#include <dbetrk/vis/dbetrk_soview2D.h>
//: Create a tableau if the storage object is of type edgetrk
vgui_tableau_sptr
dbvis1_edgetrk_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a ctrk storage object
  dbetrk_storage_sptr edgetrk_storage;
  edgetrk_storage.vertical_cast(storage);

  vcl_vector< dbetrk_edge_sptr > edge_nodes;
  edgetrk_storage->get_dbetrk_edges(edge_nodes);
  bgui_vsol2D_tableau_sptr btab= bgui_vsol2D_tableau_new();

  vcl_vector<dbetrk_edge_sptr >::iterator itr;
  int cnt=0;
  for(itr=edge_nodes.begin();itr!=edge_nodes.end();itr++,cnt++)
    {
        
        float r,g,b;
        dbetrk_soview2D * obj=new dbetrk_soview2D(*itr);
        utils::set_changing_colors( cnt ,r, g, b );
        vgui_style_sptr sty= vgui_style::new_style(r,g,b,2.0,2.0);
        btab->add( obj );
        obj->set_style(sty);
    }



  return btab;

}

