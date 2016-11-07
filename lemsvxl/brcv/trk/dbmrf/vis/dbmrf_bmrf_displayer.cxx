// This is breye1/bvis1/displayer/dbmrf_bmrf_displayer.cxx

#include "dbmrf_bmrf_displayer.h"
#include <dbmrf/pro/dbmrf_bmrf_storage.h>
#include <vgui/vgui_displaylist2D_tableau.h>
#include <bgui/bgui_bmrf_soview2D.h>
#include <bmrf/bmrf_network.h>


//: This defines the default style used to draw all epi_segments
vgui_style_sptr
dbmrf_bmrf_displayer::seg_style = vgui_style::new_style(0.0, 1.0, 0.0, 2.0, 2.0);

  
// Constructor
dbmrf_bmrf_displayer::dbmrf_bmrf_displayer()
{  
}

//: Create a tableau if the storage object is of type bmrf
vgui_tableau_sptr
dbmrf_bmrf_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into a bmrf storage object
  dbmrf_bmrf_storage_sptr bmrf_storage;
  bmrf_storage.vertical_cast(storage);

  bmrf_network_sptr network = bmrf_storage->network();
  int frame = bmrf_storage->frame();

  // Create a new tableau
  vgui_displaylist2D_tableau_sptr bmrf_tab = vgui_displaylist2D_tableau_new();
  if(network){
    bmrf_network::seg_node_map::const_iterator seg_itr;
    for( seg_itr = network->begin(frame); seg_itr != network->end(frame); ++seg_itr){
      bgui_bmrf_epi_seg_soview2D* obj = new bgui_bmrf_epi_seg_soview2D(seg_itr->first);
      bmrf_tab->add( obj );
      obj->set_style(seg_style);
    }
  }

  return bmrf_tab;
}
